/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* appDemoExMxPmEventHandle.c
*
* DESCRIPTION:
*       this library contains the implementation of the event handling functions
*       for the EXMXPM device  , and the redirection to the GalTis Agent for
*       extra actions .
*
********************************************************************************
*   ---> this file is compiled only under EXMXPM_FAMILY <---
********************************************************************************
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/userExit/userEventHandler.h>


/****************************************************************************
* Debug                                                                     *
****************************************************************************/

/* jumbo frames of 10K need 8 buffers, max packets length 15K */
#define BUFF_LEN    20

#ifdef IMPL_GALTIS
extern GT_STATUS cmdExMxPmRxPktReceive
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    IN GT_U32                               numOfBuff,
    IN GT_U8                               *packetBuffs[],
    IN GT_U32                               buffLen[],
    IN CPSS_EXMXPM_NET_RX_PARAMS_STC       *rxParamsPtr
);
#endif /* IMPL_GALTIS */

/*******************************************************************************
* RX_PACKET_RECEIVE_CB_FUN
*
* DESCRIPTION:
*       Function called to handle incoming Rx packet in the CPU
*
* APPLICABLE DEVICES: ALL Devices
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_OK                    - no error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*RX_PACKET_RECEIVE_CB_FUN)
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    IN GT_U32                               numOfBuff,
    IN GT_U8                               *packetBuffs[],
    IN GT_U32                               buffLen[],
    IN void                                *rxParamsPtr
);

#define MAX_REGISTER    5
static GT_U32                   numRegistrations=0;
static RX_PACKET_RECEIVE_CB_FUN rxPktReceiveCbArray[MAX_REGISTER];

/*******************************************************************************
* appDemoExMxPmNetRxPacketCbRegister
*
* DESCRIPTION:
*       register a CB function to be called on every RX packet to CPU
*
*       NOTE: function is implemented in the 'AppDemo' (application)
*
* INPUTS:
*       rxPktReceiveCbFun - CB function to be called on every RX packet to CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoExMxPmNetRxPacketCbRegister
(
    IN  RX_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
)
{
    if(numRegistrations >= MAX_REGISTER)
    {
        return GT_FULL;
    }

    rxPktReceiveCbArray[numRegistrations] = rxPktReceiveCbFun;

    numRegistrations++;

    return GT_OK;
}


/*******************************************************************************
* appDemoExMxPmNetRxPktHandle
*
* DESCRIPTION:
*       application routine to receive frames . -- EXMXPM function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
GT_STATUS appDemoExMxPmNetRxPktHandle
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx
)
{
    GT_STATUS                           rc;
    GT_U32                              numOfBuff = BUFF_LEN;
    GT_U8*                              packetBuffs[BUFF_LEN];
    GT_U32                              buffLenArr[BUFF_LEN];
    CPSS_EXMXPM_NET_RX_PARAMS_STC       rxParams;

    if(appDemoPpConfigList[devNum].cpuPortMode != PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        return GT_FAIL;
    }

    rc = cpssExMxPmNetIfSdmaRxPacketGet(devNum, queueIdx,&numOfBuff,
                                        packetBuffs,buffLenArr,&rxParams);                

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! 
                       * GT_BAD_SIZE - in this application demo no special treatment
                       */
    {
        /* no need to free buffers because not got any */
        return rc;
    }

    /* call all the functions that register the hook */
    {
        GT_U32  ii;
        for(ii = 0 ; ii < numRegistrations ; ii++)
        {
            (rxPktReceiveCbArray[ii])(devNum,queueIdx,numOfBuff,
                                      packetBuffs,buffLenArr,&rxParams);
        }
    }

    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdExMxPmRxPktReceive(devNum,queueIdx,numOfBuff,
                                 packetBuffs,buffLenArr,&rxParams);
#endif /* IMPL_GALTIS */
       
    /* now you need to free the buffers */
    rc = cpssExMxPmNetIfSdmaRxBufFree(devNum,queueIdx,packetBuffs,numOfBuff);

    return rc;
}

/*******************************************************************************
* appDemoExMxPmNetRxPktGet
*
* DESCRIPTION:
*       application routine to get the packets from the queue and 
*       put it in msgQ. -- EXMXPM function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       msgQId      - Message queue Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmNetRxPktGet
(
    IN GT_U8            devNum,
    IN GT_U8            queueIdx,
    IN CPSS_OS_MSGQ_ID  msgQId
)
{
    GT_STATUS                 rc;
    APP_DEMO_RX_PACKET_PARAMS rxParams;
    rxParams.numOfBuff = BUFF_LEN;
    cpssOsMutexLock(rxMutex);

    if(appDemoPpConfigList[devNum].cpuPortMode != PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        return GT_FAIL;
    }

    rc = cpssExMxPmNetIfSdmaRxPacketGet(devNum, queueIdx,&(rxParams.numOfBuff),
                            rxParams.packetBuffs,rxParams.buffLenArr,
                                          &(rxParams.exMxPmNetRxParams));
    cpssOsMutexUnlock(rxMutex);
    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! 
                       * GT_BAD_SIZE - in this application demo no special treatment
                       */
    {
        /* no need to free buffers because not got any */
        return rc;
    }

    rxParams.devNum = devNum;
    rxParams.queue = queueIdx;
    /* put in msgQ */
    rc = cpssOsMsgQSend(msgQId,&rxParams,
                        sizeof(APP_DEMO_RX_PACKET_PARAMS),CPSS_OS_MSGQ_WAIT_FOREVER);
    return rc;
}

/*******************************************************************************
* appDemoExMxPmNetRxPktTreat
*
* DESCRIPTION:
*       application routine to treat the packets . -- EXMXPM function
*
* INPUTS:
*       rxParamsPtr - (pointer to) rx paacket params
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmNetRxPktTreat
(
    IN APP_DEMO_RX_PACKET_PARAMS  *rxParamsPtr
)
{
    GT_STATUS   rc;
    GT_U8       devNum ;
    if (rxParamsPtr == NULL)
    {
        /* debug */
        return GT_BAD_PTR;
    }
    devNum = rxParamsPtr->devNum;

    if(appDemoPpConfigList[devNum].cpuPortMode != PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        return GT_FAIL;
    }

    /* call all the functions that register the hook */
    {
        GT_U32  ii;
        for(ii = 0 ; ii < numRegistrations ; ii++)
        {
            (rxPktReceiveCbArray[ii])(rxParamsPtr->devNum,rxParamsPtr->queue,
                                      rxParamsPtr->numOfBuff,rxParamsPtr->packetBuffs,
                                      rxParamsPtr->buffLenArr,&(rxParamsPtr->exMxPmNetRxParams));
        }
    }

    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdExMxPmRxPktReceive(rxParamsPtr->devNum,rxParamsPtr->queue,rxParamsPtr->numOfBuff,
                                 rxParamsPtr->packetBuffs,rxParamsPtr->buffLenArr,
                                 &(rxParamsPtr->exMxPmNetRxParams));
#endif /* IMPL_GALTIS */
    cpssOsMutexLock(rxMutex);   
    /* now you need to free the buffers */
    rc = cpssExMxPmNetIfSdmaRxBufFree(rxParamsPtr->devNum,rxParamsPtr->queue
                                      ,rxParamsPtr->packetBuffs,rxParamsPtr->numOfBuff);
    cpssOsMutexUnlock(rxMutex);

    return rc;
}
