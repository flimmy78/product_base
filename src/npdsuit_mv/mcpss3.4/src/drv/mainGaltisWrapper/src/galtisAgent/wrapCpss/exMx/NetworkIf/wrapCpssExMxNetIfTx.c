/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxNetIfTx.c
*
* DESCRIPTION:
*       Wrapper functions for NetworkIf cpss functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
******************************************************************************/

#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>
#include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>
#include <cpss/exMx/exMxGen/trunk/cpssExMxTrunk.h>
#include <galtisAgent/wrapCpss/exMx/NetworkIf/cmdCpssExMxNetTransmit.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssMemLib.h>


/*******************************************************************************
 * Internal definitions
 ******************************************************************************/
#define GT_GALTIS_TRANSMITE_TASK_PRIOR_CNS            100

/******************************** Globals *************************************/

/* Transmit Table descriptors */
static GT_U32 txTblCapacity = 0;
PKT_DESC_STC* cpssTxPacketDescTbl = NULL;

/* Transmit task ID */
static GT_TASK taskId = 0;
static GT_SEM  txCmdSemId;                      /* Module semaphore id  */
static GT_BOOL taskBusyFlag = GT_FALSE;

/* Transmit Task operation mode  */
/*static MODE_SETINGS_STC modeSettings = {0, 1};*/
static MODE_SETINGS_STC modeSettings = {1, 1};
static GT_BOOL flagStopTransmit = GT_FALSE;
static GT_U8 txCookie = 0x10;

/******************************** Externals ***********************************/
extern GT_POOL_ID txBuffersPoolId;

static GT_U32  evReqHndl = 0;


/*============================
          TX Transsmit
=============================*/

/*******************************************************************************
* startPacketTransmission
*
* DESCRIPTION:
*       Thi is thread function that performs trunsmition of packets defined
*       in the table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
static GT_STATUS startPacketTransmission(void)
{
    GT_U32      i;
    GT_U32      cycles, pcktNum;
    GT_STATUS   status;

    CPSS_EXMX_NET_TX_PARAMS_STC cpssPcktParams;
    GT_U32                      numOfBufs;
    GT_U8                       devNum=0;
    GT_TRUNK_ID                 trunkId;
    GT_U32                      numMembersPtr;
    CPSS_TRUNK_MEMBER_STC       membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U8                       tgtDev;
    GT_U32                      tgtPort;

    static GT_U32  txPacketSendCount = 0;
    static GT_U32  txPacketGetCount = 0;

    if (evReqHndl == 0)
    {
        CPSS_UNI_EV_CAUSE_ENT     evCause[1] = { CPSS_PP_TX_BUFFER_QUEUE_E };

        if (cpssEventBind(evCause, 1, &evReqHndl) != GT_OK)
        {
            return GT_FAIL;
        }

        /* convert pss events to those of cpss */
        for(i = 0 ; i < 128 ; i++)
        {
            /*extern CMD_PP_CONFIG *ppConfigList;
            if(ppConfigList[i].valid == GT_FALSE)
            {
                continue;
            }
            */

            /* unmask this event with all HW devices */
            /*status = cpssEventDeviceMaskSet(ppConfigList[i].devNum,CPSS_PP_TX_BUFFER_QUEUE_E,CPSS_EVENT_UNMASK_E);*/
            status = cpssEventDeviceMaskSet((GT_U8)i,CPSS_PP_TX_BUFFER_QUEUE_E,CPSS_EVENT_UNMASK_E);
            if(status == GT_NOT_FOUND)
            {
                /* this event not relevant to this device */
                status = GT_OK;
            }
            else
            if(status != GT_OK)
            {
                /* there are FA/XBAR events that not relevant to PP device,
                vice versa */
                continue;
            }
        }

    }

    cycles = 0;
    pcktNum = 0;
    while (((cycles < modeSettings.cyclesNum) ||
            (modeSettings.cyclesNum == -1)) && (flagStopTransmit == GT_FALSE))
    {
        for(i = 0; i < txTblCapacity; i++) /*For each descriptor*/
        {
            GT_U32 j;
            numOfBufs=0;

            /* If the transmition was disabled */
            if (flagStopTransmit == GT_TRUE)
            {
                break;
            }

            if (GT_TRUE != cpssTxPacketDescTbl[i].valid)
            {
                /* entry is not valid */
                continue;
            }

            for (j = 0; j < cpssTxPacketDescTbl[i].pcktsNum; j++)
            {/*For packet sent by this descriptor*/
                /* If the transmition was disabled */
                if (flagStopTransmit == GT_TRUE)
                {
                    break;
                }

                cpssPcktParams.txParams.cookie = &txCookie;
                cpssPcktParams.txParams.dropPrecedence = cpssTxPacketDescTbl[i].dropPrecedence;
                cpssPcktParams.txParams.evReqHndl = evReqHndl;


                if(cpssTxPacketDescTbl[i].cmdType == TX_BY_VIDX)
                {
                    cpssPcktParams.useVidx = GT_TRUE;
                    cpssPcktParams.txParams.vid = cpssTxPacketDescTbl[i].cmdParams.byVlan.vid;
                    cpssPcktParams.dest.vidx = cpssTxPacketDescTbl[i].cmdParams.byVlan.vidx;

                    /* dsaInfo is relevant only when addDsa = GT_TRUE and
                       useVidx = GT_FALSE. if cmdType == TX_BY_VIDX then
                       useVidx = GT_TRUE ==> we set addDsa = GT_FALSE */
                    cpssPcktParams.addDsa = GT_FALSE;

                }
                else{
                    cpssPcktParams.useVidx = GT_FALSE;
                    cpssPcktParams.txParams.vid = cpssTxPacketDescTbl[i].cmdParams.byPort.vid;

                    if(cpssTxPacketDescTbl[i].cmdParams.byPort.isTrunk == GT_TRUE)
                    {

                        trunkId = cpssTxPacketDescTbl[i].cmdParams.byPort.portTrunk;
                        devNum = cpssTxPacketDescTbl[i].cmdParams.byPort.devNum;

                        status = cpssExMxTrunkTableEntryGet(devNum, trunkId, &numMembersPtr, membersArray);
                        if(status != GT_OK)
                        {
                            return status;
                        }

                        if(numMembersPtr==0)
                        {
                            return GT_FAIL;/* just like in empty vlan --- packet was not sent */
                        }

                        /* The packet should be send throw any of the trunk members */
                        tgtDev = membersArray[0].device;
                        tgtPort = membersArray[0].port;
                    }
                    else
                    {
                        tgtDev  = cpssTxPacketDescTbl[i].cmdParams.byPort.devNum;
                        tgtPort = cpssTxPacketDescTbl[i].cmdParams.byPort.portTrunk;

                    }

                    cpssPcktParams.dest.devPort.tgtDev = tgtDev;
                    cpssPcktParams.dest.devPort.tgtPort = (GT_U8)tgtPort;

                    cpssPcktParams.addDsa = cpssTxPacketDescTbl[i].cmdParams.byPort.addDsa;
                    cpssPcktParams.dsaInfo.physicalMuxDev = cpssTxPacketDescTbl[i].cmdParams.byPort.dsaInfo.physicalMuxDev;
                    cpssPcktParams.dsaInfo.physicalMuxPort = cpssTxPacketDescTbl[i].cmdParams.byPort.dsaInfo.physicalMuxPort;
                    cpssPcktParams.dsaInfo.tcOnMuxDevice = cpssTxPacketDescTbl[i].cmdParams.byPort.dsaInfo.tcOnMuxDevice;


                }

                cpssPcktParams.txParams.macDaType   = cpssTxPacketDescTbl[i].macDaType;
                cpssPcktParams.txParams.packetEncap = cpssTxPacketDescTbl[i].encap;
                cpssPcktParams.txParams.recalcCrc   = cpssTxPacketDescTbl[i].appendCrc;
                cpssPcktParams.txParams.tagged      = cpssTxPacketDescTbl[i].tagged;
                cpssPcktParams.txParams.txQueue     = cpssTxPacketDescTbl[i].txQueue;
                cpssPcktParams.txParams.userPrioTag = cpssTxPacketDescTbl[i].userPrioTag;
                cpssPcktParams.invokeTxBufferQueueEvent = cpssTxPacketDescTbl[i].invokeTxBufferQueueEvent;

                if(cpssTxPacketDescTbl[i].txSyncMode==GT_TRUE)
                 {
                     status = cpssExMxNetIfSyncTxPacketSend(devNum, &cpssPcktParams, cpssTxPacketDescTbl[i].pcktData,
                                                    cpssTxPacketDescTbl[i].pcktDataLen, cpssTxPacketDescTbl[i].numOfBuffers);
                 }
                 else
                 {
                     status = cpssExMxNetIfTxPacketSend(devNum, &cpssPcktParams, cpssTxPacketDescTbl[i].pcktData,
                                                    cpssTxPacketDescTbl[i].pcktDataLen, cpssTxPacketDescTbl[i].numOfBuffers);
                     txPacketSendCount++;
                 }

                if((status != GT_OK) && (status != GT_NO_RESOURCE))
                {
                    return status;
                }

                /* if we get GT_NO_RESOURCE and the sdmaTxPacketSendCount
                   is bigger then MAX_NUM_OF_BUFFERS_AT_POOL
                   it means no more buffer available in the FIFO */
                if (((cpssTxPacketDescTbl[i].txSyncMode==GT_FALSE) &&
                    (cpssTxPacketDescTbl[i].invokeTxBufferQueueEvent==GT_TRUE)) ||
                    ((status == GT_NO_RESOURCE) && (txPacketSendCount >= MAX_NUM_OF_BUFFERS_AT_POOL)))
                {
                    GT_U8           devNum;
                    GT_U8           queueIdx;
                    GT_STATUS       retVal;
                    GT_PTR          cookie;

                    /* wait for the Tx-End event */
                    retVal = cpssEventSelect(evReqHndl,NULL,NULL,0);

                    if (retVal != GT_OK)
                    {
                        return retVal;
                    }

                    /* get all Tx end events for the packet */

                    /* Nathan -
                       I dont think we need this here because the galtis
                       application will not do anything with the returnd values,
                       no buffers to free. */
                    while(1)
                    {
                        retVal = cpssExMxNetIfTxBufferQueueGet(evReqHndl,&devNum,
                                                        &cookie,&queueIdx,&retVal);
                        if (retVal == GT_FAIL)
                        {
                            return retVal;
                        }
                        if(retVal == GT_NO_MORE)
                        {
                            break;
                        }
                        txPacketGetCount++;
                    }

                    if(status != GT_NO_RESOURCE)
                    {
                        if(txPacketSendCount == txPacketGetCount)
                        {
                            txPacketSendCount = 0;
                            txPacketGetCount = 0;
                        }
                        else
                        {
                            cmdOsPrintf("Number of sent packets NOT equal to number of packets get \n\n");
                        }
                    }

                    /* when we get GT_NO_RESOURCE then it means that not all packet that
                    was transmited were inserted into the FIFO, this is why
                    there is no need to check if the number of transmited
                    packets are equal to the number of packets in the FIFO*/
                    else
                    {
                        txPacketSendCount = 0;
                        txPacketGetCount = 0;

                        /*update the index inorder for the packet to be sent again */
                        j--;
                        continue;
                    }
                }

                /* in case transmit succeed */
                pcktNum++;
                cpssTxPacketDescTbl[i].numSentPackets++;


                /* wait n miliseconds before sending next packet */
                if(cpssTxPacketDescTbl[i].gap != 0)
                {
                    cmdOsTimerWkAfter(cpssTxPacketDescTbl[i].gap);
                }
            }

            /* wait n miliseconds before moving to the next entry */
            if(cpssTxPacketDescTbl[i].waitTime != 0)
            {
                cmdOsTimerWkAfter(cpssTxPacketDescTbl[i].waitTime);
            }
        }

        /* wait n miliseconds before starting the next cycle */
        if(modeSettings.gap != 0)
        {
            cmdOsTimerWkAfter(modeSettings.gap);
        }
        /* move to the next cycle */
        cycles++;
    }

    return GT_OK;
}

/*******************************************************************************
* packetTransminitionTask
*
* DESCRIPTION:
*       galtis task for packet transmission.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
GT_STATUS __TASKCONV cpssPacketTransminitionTask()
{
    GT_STATUS rc = GT_OK;

    /* while forever */
    while(1)
    {
        /* wait on the TX command semaphore */
        taskBusyFlag = GT_FALSE;
        cmdOsSigSemWait(txCmdSemId, CPSS_OS_SEM_WAIT_FOREVER_CNS);
        taskBusyFlag = GT_TRUE;
        rc = startPacketTransmission();
    }
}

/*******************************************************************************
* cmdCpssTxSetPacketDesc
*
* DESCRIPTION:
*       Creates new transmit parameters description entry in the table.
*
* INPUTS:
*       entryID      - Entry ID of packet descriptor to return.
*       packetDesc   - Packet descriptor fro new entry in the table
*       extended     - flag indicates if this is the extended table
*
* OUTPUTS:
*       newEntryIndex   - The index of the new entry in the table.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The creation is always for one segment. The pointer to the data is
*       allocated by the wrapper and always arrived in index 0 of packetDesc
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssTxSetPacketDesc
(
    IN  GT_U32        entryId,
    IN  PKT_DESC_STC *packetDesc,
    IN  GT_BOOL       extended,
    OUT GT_U32        *newEntryIndex
)
{
    GT_U32  entryIndex;


    /* Prepare local variables and check whether valid packet */
    if(packetDesc->pcktDataLen[0] == 0)
    {
        return GT_FAIL;
    }

    for(entryIndex = 0; entryIndex < txTblCapacity; entryIndex++)
    {
        if (GT_FALSE == cpssTxPacketDescTbl[entryIndex].valid)
        {
            /* we found a free space */
            break;
        }

        if(cpssTxPacketDescTbl[entryIndex].entryId == entryId)
        {
            /* Non-extended mode does not permit to change entry data */
            if (extended == GT_FALSE)
                return GT_ALREADY_EXIST ;

            /* Extended mode allows to add packet segments */
            break;

        }
    }

    if(entryIndex == txTblCapacity)
    {
        /* Entry not found - creates new one */
        txTblCapacity++;

        /* allocate new TX packet descriptor*/
        if(cpssTxPacketDescTbl == NULL)
        {
            cpssTxPacketDescTbl = cmdOsMalloc(txTblCapacity * sizeof(PKT_DESC_STC));
        }
        else
        {
            cpssTxPacketDescTbl = cmdOsRealloc(cpssTxPacketDescTbl,
                                        txTblCapacity * sizeof(PKT_DESC_STC));
        }
        if(cpssTxPacketDescTbl == NULL)
        {
            txTblCapacity = 0;
            return GT_OUT_OF_CPU_MEM;
        }
        cmdOsMemSet(&cpssTxPacketDescTbl[entryIndex], 0, sizeof(PKT_DESC_STC));
    }

    cmdOsMemCpy(&cpssTxPacketDescTbl[entryIndex], packetDesc, sizeof(PKT_DESC_STC));

    cpssTxPacketDescTbl[entryIndex].valid = GT_TRUE;

    *newEntryIndex = entryIndex;

    return GT_OK;
}



/*******************************************************************************
* cmdCpssTxGetPacketDesc
*
* DESCRIPTION:
*       Returns specific packet descriptor entry from the table.
*
* INPUTS:
*     entryIndex   - Entry index in packet descriptor table.
*
* OUTPUTS:
*     packetDesc   - Packet descriptor from the table.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*       GT_EMPTY - on invalid index
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssTxGetPacketDesc
(
    INOUT GT_U32      *entryIndex,
    OUT PKT_DESC_STC *packetDesc
)
{
    GT_U8   i;

    /* check if index is valid for the table */
    if(*entryIndex >= txTblCapacity)
        return GT_NOT_FOUND;


    for (; *entryIndex < txTblCapacity; (*entryIndex)++)
    {
        if (GT_TRUE == cpssTxPacketDescTbl[*entryIndex].valid)
        {
            /* Entry found */
            break;
        }
    }

    if (*entryIndex == txTblCapacity)
    {
        /* entry not found */
        return GT_NOT_FOUND;
    }

    /* copy packet descriptor */
    cmdOsMemSet(packetDesc, 0, sizeof(PKT_DESC_STC));
    cmdOsMemCpy(packetDesc, &cpssTxPacketDescTbl[*entryIndex], sizeof(PKT_DESC_STC));

    /* allocate memory for the pkt data */
    for( i = 0; i < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN; i++)
    {
        if(cpssTxPacketDescTbl[*entryIndex].pcktDataLen[i] == 0)
            break; /*End of the chain */

        packetDesc->pcktData[i] =
            cmdOsMalloc(cpssTxPacketDescTbl[*entryIndex].pcktDataLen[i]);

        if(packetDesc->pcktData[i] == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        /* copy pkt data */
        cmdOsMemCpy(packetDesc->pcktData[i],
                     cpssTxPacketDescTbl[*entryIndex].pcktData[i],
                     cpssTxPacketDescTbl[*entryIndex].pcktDataLen[i]);
    }

    return GT_OK;
}

/*******************************************************************************
* cmdCpssTxBeginGetPacketDesc
*
* DESCRIPTION:
*       Indicates for cmdCpssTxGetPacketDesc function that user go to get many
*       records in one session. This API make exact copy of table so that
*       user will get table without changes that can be madce during get
*       operation. This mechanism is best suitable for Refresh table
*       operation.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssTxBeginGetPacketDesc
(
)
{
    return GT_OK;
}

/*******************************************************************************
* cmdCpssTxEndGetPacketDesc
*
* DESCRIPTION:
*       Finish operation began by cmdCpssTxBeginGetPacketDesc API function
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssTxEndGetPacketDesc
(
)
{
    return GT_OK;
}



/*******************************************************************************
* cmdCpssTxDelPacketDesc
*
* DESCRIPTION:
*       Delete packet descriptor entry from the table.
*
* INPUTS:
*       entryId     - Entry ID of packet descriptor to delete.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       After delete operation all record will be shifted
*       so the table will be not fragmented.
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssTxDelPacketDesc
(
    IN  GT_U32      entryId
)
{
    GT_U32          j;
    PKT_DESC_STC   *txPcktDescPtr;
    GT_U32          entryIndex;

    /* make sure there is no send */
    cmdCpssTxStop();

    if(cpssTxPacketDescTbl != NULL)
    {
        /* find the entry */
        for (entryIndex = 0; entryIndex < txTblCapacity; entryIndex++)
        {
            if (GT_FALSE == cpssTxPacketDescTbl[entryIndex].valid)
            {
                /* invalid entry */
                continue;
            }

            if (cpssTxPacketDescTbl[entryIndex].entryId == entryId)
            {
                /* Entry found */
                break;
            }
        }

        if (entryIndex == txTblCapacity)
        {
            /* entry not found */
            return GT_NOT_FOUND;
        }

        txPcktDescPtr = &cpssTxPacketDescTbl[entryIndex];

        /* release tx buffers */
        for( j=0; j < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN ; j++ )
        {
            if (0 == txPcktDescPtr->pcktDataLen[j])
            {
                break;
            }

            if(txPcktDescPtr->pcktData[j] != NULL)
            {

                cmdOsPoolFreeBuf(txBuffersPoolId, txPcktDescPtr->pcktData[j]);
            }
        }/*For all buffers for this descriptor*/

        /* clear the packet descriptor data */
        cmdOsMemSet(txPcktDescPtr, 0, sizeof(PKT_DESC_STC));
    }

    return GT_OK;
}


/*******************************************************************************
* cmdCpssTxClearPacketDesc
*
* DESCRIPTION:
*       Clear all packet descriptors entries from the table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssTxClearPacketDesc(void)
{
    GT_U32 i,j;

    /* make sure there is no send */
    cmdCpssTxStop();

    if(cpssTxPacketDescTbl != NULL)
    {
         /* release tx buffers */
         for(i = 0; i < txTblCapacity;i++)
         {
             if (GT_TRUE != cpssTxPacketDescTbl[i].valid)
             {
                 /* entry is not valid */
                 continue;
             }

             for( j=0; j < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN ; j++ )
             {
                 if (0 == cpssTxPacketDescTbl[i].pcktDataLen[j])
                 {
                     break;
                 }

                 if (cpssTxPacketDescTbl[i].pcktData[j] != NULL)
                     cmdOsPoolFreeBuf(txBuffersPoolId, cpssTxPacketDescTbl[i].pcktData[j]);

             }/*For all buffers for this descriptor*/
         }/*For all descriptors*/

         /* free allocated table */
         cmdOsFree(cpssTxPacketDescTbl);
         cpssTxPacketDescTbl = NULL;
         txTblCapacity = 0;
    }

    return GT_OK;
}


/*******************************************************************************
* cmdCpssTxStart
*
* DESCRIPTION:
*       Starts transmition of packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdCpssTxStart
*
*******************************************************************************/
GT_STATUS cmdCpssTxStart
(
)
{
    GT_STATUS rc = GT_OK;
    static GT_BOOL   enterOnce = GT_FALSE;

    /* check if there are element in the transmit table */
    if(txTblCapacity == 0)
        return GT_OK;

    /* creat the task only once */
    if(enterOnce == GT_FALSE)
    {
        if(cmdOsSigSemBinCreate("txCmd",CPSS_OS_SEMB_EMPTY_E,&txCmdSemId) != GT_OK)
        {
            return GT_FAIL;
        }
        if(cmdOsTaskCreate("GalPktTx",                           /* Task Name                      */
                        GT_GALTIS_TRANSMITE_TASK_PRIOR_CNS,   /* Task Priority                  */
                        0x1000,                               /* Stack Size _4KB                */
                        (unsigned (__TASKCONV *)(void*))cpssPacketTransminitionTask, /* Starting Point */
                        (GT_VOID*)NULL,                       /* there is no arguments */
                        &taskId) != GT_OK)                    /* returned task ID */
            return GT_FAIL;
        enterOnce = GT_TRUE;
    }

    /* check if the last transmition is done */
    if(taskBusyFlag == GT_TRUE)
        return GT_FAIL;

    flagStopTransmit = GT_FALSE;

    /* send a signal for the task to start the transmission */
    cmdOsSigSemSignal(txCmdSemId);

    return  rc;
}

/*******************************************************************************
* cmdCpssTxStop
*
* DESCRIPTION:
*       Stop transmition of packets.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdCpssTxStop
*
*******************************************************************************/
GT_STATUS cmdCpssTxStop
(
    void
)
{
    /* there is no send task running */
    if(taskId == 0)
    {
        return GT_NO_CHANGE;
    }

    flagStopTransmit = GT_TRUE;
    return GT_OK;
}


/*******************************************************************************
* cmdCpssTxSetMode
*
* DESCRIPTION:
*       This command will set the the transmit parameters.
*
* INPUTS:
*       cyclesNum - The maximum number of loop cycles (-1 = forever)
*       gap       - The time to wit between two cycles
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdTxSetMode
*       Interface   - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssTxSetMode
(
    IN GT_U32   cyclesNum,
    IN GT_U32   gap
)
{
    modeSettings.cyclesNum = cyclesNum;
    modeSettings.gap = gap;

    return GT_OK;
}


/*******************************************************************************
* cmdCpssTxGetMode
*
* DESCRIPTION:
*       This command will get the the transmit parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       cyclesNum - The maximum number of loop cycles (-1 = forever)
*       gap       - The time to wit between two cycles.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssTxGetMode
*
*******************************************************************************/
GT_STATUS cmdCpssTxGetMode
(
    OUT GT_U32 *cyclesNum,
    OUT GT_U32 *gap
)
{
    *cyclesNum = modeSettings.cyclesNum;
    *gap = modeSettings.gap;

    return GT_OK;
}



