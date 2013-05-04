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
* cmdNetReceive.c
*
* DESCRIPTION:
*       galtis agent interface for packet capture using networkIf
*
* DEPENDENCIES:
*       cmdNetReceive.h
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#ifdef SHARED_MEMORY
#include <gtOs/gtOsSharedUtil.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <gtOs/gtOsSharedUtil.h>
#include <gtOs/gtOsSharedIPC.h>
#endif

#include <cmdShell/common/cmdCommon.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <galtisAgent/wrapUtil/cmdCpssNetReceive.h>

#define CMD_RX_DEFULT_NUM_OF_ENTRIES_CNS 10

/******************************** Locals *************************************/

static CMD_RX_ENTRY *rxEntriesArr;
static GALTIS_RX_MODE_ENT rxTableMode = GALTIS_RX_CYCLIC_E;
static GT_U32  maxRxBufferSize = GALTIS_RX_BUFFER_MAX_SIZE_CNS; /*1536*/
static GT_U32  maxNumOfRxEntries = CMD_RX_DEFULT_NUM_OF_ENTRIES_CNS;
static GT_U32  indexToInsertPacket = 0;
static GT_U32  doCapture = GT_FALSE;
static GT_BOOL wasInitDone = GT_FALSE;

static GT_MUTEX  cmdCpssRxMtx = 0;
#define CMD_RX_MTX_CREATE_MAC   if(cmdCpssRxMtx==0) cmdOsMutexCreate("cmdCpssRxMtx", &cmdCpssRxMtx)
#define CMD_RX_MTX_LOCK_MAC     cmdOsMutexLock(cmdCpssRxMtx)
#define CMD_RX_MTX_UNLOCK_MAC     cmdOsMutexUnlock(cmdCpssRxMtx)

#ifdef SHARED_MEMORY
static CPSS_MP_REMOTE_COMMAND remoteCmd;
#endif


/*******************************************************************************
* freeRxPacketInfo
*
* DESCRIPTION:
*       free the specific rx packet info, by calling to the registered callback
*
* INPUTS:
*       index - index to the rxEntriesArr array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void freeRxPacketInfo
(
    IN GT_U32   index
)
{
    if(rxEntriesArr[index].packetBufferPtr)
    {
        cmdOsFree(rxEntriesArr[index].packetBufferPtr);
        rxEntriesArr[index].packetBufferPtr = NULL;
    }


    if(rxEntriesArr[index].specificDeviceFormatPtr == NULL)
    {
        return;
    }

    if(rxEntriesArr[index].freeRxInfoFunc)
    {
        /* free by call back*/
        rxEntriesArr[index].freeRxInfoFunc(rxEntriesArr[index].specificDeviceFormatPtr);
    }
    else
    {
        /* free with no specific extra needs */
        cmdOsFree(rxEntriesArr[index].specificDeviceFormatPtr);
    }

    /* set not used pointer */
    rxEntriesArr[index].specificDeviceFormatPtr = NULL;
}

/*******************************************************************************
* cmdCpssRxInitLib
*
* DESCRIPTION:
*       initialize the rx library
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
static GT_VOID cmdCpssRxInitLib(void)
{
    int ii;

    if(wasInitDone == GT_TRUE)
        return;

    /* create semaphore */
    CMD_RX_MTX_CREATE_MAC;

    rxEntriesArr = cmdOsMalloc(CMD_RX_DEFULT_NUM_OF_ENTRIES_CNS * sizeof(CMD_RX_ENTRY));
    for(ii = 0; ii < CMD_RX_DEFULT_NUM_OF_ENTRIES_CNS; ii++)
    {
        rxEntriesArr[ii].isEmpty = GT_TRUE;
    }

    wasInitDone = GT_TRUE;
}


#ifdef SHARED_MEMORY

/*******************************************************************************
* rxInfoFree
*
* DESCRIPTION:
*       function to free the specific rx info format.
*
* INPUTS:
*      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void rxInfoFree
(
    IN void*  specificDeviceFormatPtr
)
{
    if(specificDeviceFormatPtr)
    {
        cmdOsFree(specificDeviceFormatPtr);
    }
}

/*******************************************************************************
* rxInfoCopy
*
* DESCRIPTION:
*       function to COPY the specific rx info format.
*
* INPUTS:
*      srcSpecificDeviceFormatPtr - (pointer to) the SOURCE specific device Rx
*                                   info format.
*                                   format of DXCH / EXMX /DXSAL ...
*
* OUTPUTS:
*      dstSpecificDeviceFormatPtr - (pointer to) the DESTINATION specific device Rx
*                                   info format.
*                                   format of DXCH / EXMX /DXSAL ...
*
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void rxInfoCopy
(
    IN  const void*  srcSpecificDeviceFormatPtr,
    OUT void*        dstSpecificDeviceFormatPtr
)
{
    if(srcSpecificDeviceFormatPtr == NULL || dstSpecificDeviceFormatPtr == NULL)
    {
        return;
    }

    osMemCpy(dstSpecificDeviceFormatPtr,
             srcSpecificDeviceFormatPtr,
             sizeof(CPSS_DXCH_NET_RX_PARAMS_STC));

    return;
}

#endif

/*******************************************************************************
* cmdCpssRxStartCapture
*
* DESCRIPTION:
*       Start collecting the recived packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssRxStartCapture
*
*******************************************************************************/
GT_STATUS cmdCpssRxStartCapture(void)
{
    if(wasInitDone == GT_FALSE)
        cmdCpssRxInitLib();

    CMD_RX_MTX_LOCK_MAC;
    doCapture = GT_TRUE;
    CMD_RX_MTX_UNLOCK_MAC;

    return GT_OK;
}


/*******************************************************************************
* cmdCpssRxStopCapture
*
* DESCRIPTION:
*       Stop collecting the recived packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssRxStopCapture
*
*******************************************************************************/
GT_STATUS cmdCpssRxStopCapture()
{
    CMD_RX_MTX_LOCK_MAC;
    doCapture = GT_FALSE;
    CMD_RX_MTX_UNLOCK_MAC;

    return GT_OK;
}

/*******************************************************************************
* cmdCpssRxSetMode
*
* DESCRIPTION:
*       set recived packets collection mode and parameters
*
* INPUTS:
*       rxMode        - collection mode
*       buferSize     - buffer size
*       numOfEnteries - number of entries
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success
*       GT_OUT_OF_CPU_MEM   - memory allocation Failure
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssRxSetMode
*
*******************************************************************************/
GT_STATUS cmdCpssRxSetMode
(
    IN GALTIS_RX_MODE_ENT   rxMode,
    IN GT_U32       buferSize,
    IN GT_U32       numOfEnteries
)
{
    GT_U32 ii;

    if (buferSize == 0 || numOfEnteries == 0)
    {
        return GT_BAD_PARAM;
    }

    if(wasInitDone == GT_FALSE)
        cmdCpssRxInitLib();

    /* Should NOT be inside sem wait (takes the same semaphore)*/
    cmdCpssRxPktClearTbl();

    CMD_RX_MTX_LOCK_MAC;

    rxTableMode = rxMode;
    maxRxBufferSize = buferSize;

    if (rxEntriesArr != NULL)
    {
        /* free the specific packet's device format */
        for(ii = 0 ; ii < maxNumOfRxEntries ; ii++)
        {
            if(rxEntriesArr[ii].isEmpty == GT_FALSE)
            {
                freeRxPacketInfo(ii);
            }
        }

        cmdOsFree(rxEntriesArr);

        wasInitDone = GT_FALSE;
    }

    rxEntriesArr = cmdOsMalloc(numOfEnteries * sizeof(CMD_RX_ENTRY));

    if(rxEntriesArr == NULL)
    {
        CMD_RX_MTX_UNLOCK_MAC;
        return GT_OUT_OF_CPU_MEM;
    }

    wasInitDone = GT_TRUE;

    for(ii = 0; ii < numOfEnteries; ii++)
    {
        rxEntriesArr[ii].isEmpty = GT_TRUE;
    }

    maxNumOfRxEntries = numOfEnteries;

    CMD_RX_MTX_UNLOCK_MAC;


/******************************************************
*    Send RxTx params to CPSS Shared Lib buffer for
*    RxTxProcess
*
******************************************************/
#ifdef SHARED_MEMORY



CPSS_RX_TX_MP_MODE_PARAMS_STC RxTxParams;
GT_STATUS rc;

RxTxParams.valid = GT_TRUE;
RxTxParams.copyRxInfoFunc =  rxInfoCopy;
RxTxParams.doCapture = doCapture;
RxTxParams.freeRxInfoFunc = rxInfoFree;
RxTxParams.indexToInsertPacket = indexToInsertPacket;
RxTxParams.maxNumOfRxEntries = maxNumOfRxEntries;
RxTxParams.maxRxBufferSize = maxRxBufferSize;
RxTxParams.rxEntriesArr = rxEntriesArr;
RxTxParams.rxTableMode = rxTableMode;
RxTxParams.specificDeviceFormatPtr = NULL;
RxTxParams.wasInitDone = wasInitDone;


 /*send Rx params to RxTx process */
rc = cpssMultiProcComSetRxTxParams(&RxTxParams);

#endif

return GT_OK;
}

/*******************************************************************************
* cmdCpssRxGetMode
*
* DESCRIPTION:
*       Get recived packets collection mode and parameters
*
* INPUTS:
*       None
*
* OUTPUTS:
*       mode        - GALTIS_RX_CYCLIC_E or GALTIS_RX_ONCE_E
*       buffSize    - packet buffer size
*       numEntries  - number of entries
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssRxGetMode
*
*******************************************************************************/
GT_STATUS cmdCpssRxGetMode
(
    OUT GALTIS_RX_MODE_ENT  *modePtr,
    OUT GT_U32      *buffSizePtr,
    OUT GT_U32      *numEntriesPtr
)
{
    CMD_RX_MTX_LOCK_MAC;
    *modePtr = rxTableMode;
    *buffSizePtr = maxRxBufferSize;
    *numEntriesPtr = maxNumOfRxEntries;
    CMD_RX_MTX_UNLOCK_MAC;

    return GT_OK;
}

/*******************************************************************************
* cmdCpssRxPktReceive
*
* DESCRIPTION:
*       Receive packet callback function . the caller wants to register its
*       packet info with the manager array.
*
*       This function give services to all the rx packets info formats.
*
* INPUTS:
*      devNum  - The device number in which the packet was received.
*      queue   - The Rx queue in which the packet was received.
*      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*                             NOTE : this pointer is allocated by the specific
*                             device "C" file.
*      freeRxInfoFunc - callback function to free the specific rx info format,
*      copyRxInfoFunc - callback function to copy the specific rx info format,
*      numOfBuff    - Num of used buffs in packetBuffs
*      packetBuffs  - The received packet buffers list
*      buffLen      - List of buffer lengths for packetBuffs
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FULL - when buffer is full
*       GT_BAD_STATE - the galtis-cmd not ready/set to "save" the packet with info
*       GT_BAD_PARAM - on bad parameter
*       GT_OUT_OF_CPU_MEM - on CPU memory allocation failure
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
GT_STATUS cmdCpssRxPktReceive
(
    IN GT_U8           devNum,
    IN GT_U8           queue,
    IN void*           specificDeviceFormatPtr,
    IN FREE_RX_INFO_FUNC freeRxInfoFunc,
    IN COPY_RX_INFO_FUNC copyRxInfoFunc,
    IN GT_U32          numOfBuff,
    IN GT_U8           *packetBuffs[],
    IN GT_U32          buffLen[]
)
{
    GT_STATUS   rc;
    GT_U32 tmpBuffSize;
    GT_U32 toCopyBufSize;
    GT_U32 rxPcktBufSize;
    GT_U32 ii;

    if(doCapture != GT_TRUE)
        return GT_BAD_STATE;

    if(wasInitDone == GT_FALSE)
        return GT_BAD_STATE;

    if(copyRxInfoFunc == NULL ||
       packetBuffs == NULL ||
       buffLen == NULL ||
       specificDeviceFormatPtr == NULL)/* must supply those */
    {
        return GT_BAD_PARAM;
    }

    CMD_RX_MTX_LOCK_MAC;

    if((rxTableMode == GALTIS_RX_ONCE_E && indexToInsertPacket < maxNumOfRxEntries - 1)
        || (rxTableMode == GALTIS_RX_CYCLIC_E))
    {
        if(rxEntriesArr[indexToInsertPacket].isEmpty != GT_TRUE)
        {
            freeRxPacketInfo(indexToInsertPacket);
        }

        /* get the packet actual length */
        rxPcktBufSize = 0;
        for(ii = 0; ii < numOfBuff; ii++)
        {
            rxPcktBufSize += buffLen[ii];
        }

        /* set the number of buffer bytes to copy */
        if (rxPcktBufSize > maxRxBufferSize)
        {
            /* packet length is bigger than destination buffer size. */
            toCopyBufSize = maxRxBufferSize;
        }
        else
        {
            toCopyBufSize = rxPcktBufSize;
        }

        /* allocate memory for the packet data */
        rxEntriesArr[indexToInsertPacket].packetBufferPtr = cmdOsMalloc(toCopyBufSize +1);

        if (NULL == rxEntriesArr[indexToInsertPacket].packetBufferPtr)
        {
            CMD_RX_MTX_UNLOCK_MAC;
            return GT_OUT_OF_CPU_MEM;
        }

        rxEntriesArr[indexToInsertPacket].isEmpty = GT_FALSE;
        rxEntriesArr[indexToInsertPacket].devNum = devNum;
        rxEntriesArr[indexToInsertPacket].queue = queue;
        rxEntriesArr[indexToInsertPacket].specificDeviceFormatPtr = specificDeviceFormatPtr;
        rxEntriesArr[indexToInsertPacket].freeRxInfoFunc = freeRxInfoFunc;
        rxEntriesArr[indexToInsertPacket].copyRxInfoFunc = copyRxInfoFunc;
        rxEntriesArr[indexToInsertPacket].packetOriginalLen = rxPcktBufSize;
        rxEntriesArr[indexToInsertPacket].bufferLen = toCopyBufSize + 1;

        cmdOsMemSet(rxEntriesArr[indexToInsertPacket].packetBufferPtr,0,toCopyBufSize +1);

        tmpBuffSize = 0;
        /* copy packet data to allocated buffer */
        for(ii = 0; ii < numOfBuff; ii++)
        {
            if(tmpBuffSize + buffLen[ii] > toCopyBufSize)
            {
                /* packet length is bigger than destination buffer size */
                cmdOsMemCpy(rxEntriesArr[indexToInsertPacket].packetBufferPtr + tmpBuffSize,
                         packetBuffs[ii], toCopyBufSize - tmpBuffSize);
                break;
            }
            /* copy the packet buffer to destination buffer */
            cmdOsMemCpy(rxEntriesArr[indexToInsertPacket].packetBufferPtr + tmpBuffSize,
                     packetBuffs[ii], buffLen[ii]);
            tmpBuffSize += buffLen[ii];
        }

        if((indexToInsertPacket == maxNumOfRxEntries -1))
            indexToInsertPacket = 0;
        else
            indexToInsertPacket++;

        rc = GT_OK;
    }
    else
    {
        rc = GT_FULL;
    }

    CMD_RX_MTX_UNLOCK_MAC;

    return rc;
}

/*******************************************************************************
* cmdCpssRxPktClearTbl
*
* DESCRIPTION:
*       clear cmdCpssRxPktClearTbl table
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
* GalTis:
*       Table - rxNetworkIf
*
*******************************************************************************/
GT_STATUS cmdCpssRxPktClearTbl(void)
{
    GT_U32 ii;

    if(wasInitDone == GT_FALSE)
        cmdCpssRxInitLib();

    CMD_RX_MTX_LOCK_MAC;

    for(ii = 0; ii < maxNumOfRxEntries; ii++)
    {
        if(rxEntriesArr[ii].isEmpty == GT_FALSE)
        {
            freeRxPacketInfo(ii);
            rxEntriesArr[ii].isEmpty = GT_TRUE;
        }
    }

    indexToInsertPacket = 0;

    CMD_RX_MTX_UNLOCK_MAC;

#ifdef SHARED_MEMORY
    remoteCmd.remote = CPSS_MP_REMOTE_RXTX_E;
    remoteCmd.command = CPSS_MP_CMD_RXTX_RXPKT_CLEAR_TBL_E;

    cpssMultiProcComExecute(&remoteCmd);
#endif

    return GT_OK;
}

/*******************************************************************************
* cmdCpssRxPkGet
*
* DESCRIPTION:
*       Get Next entry from rxNetworkIf table
*
* INPUTS:
*       currentIndexPtr - the PREVIOUS entry's index (relevant only for getNext == GT_FALSE)
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       currentIndexPtr - get current entry's index
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - the length of the copied packet to gtBuf
*       packetLenPtr    - the Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr       - The Rx queue in which the packet was received.
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*
* RETURNS:
*       GT_OK - on success
*       GT_NO_MORE - when no more entries
*
* COMMENTS:
*
* GalTis:
*       Table - rxNetworkIf
*
*******************************************************************************/
static GT_STATUS cmdCpssRxPkGet
(
    INOUT GT_U32        *currentIndexPtr,
    OUT GT_U8           *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT GT_U32          *packetLenPtr,
    OUT GT_U8           *devNumPtr,
    OUT GT_U8           *queuePtr,
    OUT void*           specificDeviceFormatPtr,
    IN  GT_BOOL         getNext
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 currIndex; /* current index */

    if(wasInitDone == GT_FALSE)
        cmdCpssRxInitLib();

    CMD_RX_MTX_LOCK_MAC;

    if(getNext == GT_TRUE)
    {

        /* increment the current index */
        (*currentIndexPtr)++;

        if(*currentIndexPtr == maxNumOfRxEntries)
        {
            *currentIndexPtr = 0;
        }
        /* check if reached the end of table (support also the "cyclic table")*/
        if(indexToInsertPacket == (*currentIndexPtr))
        {
            CMD_RX_MTX_UNLOCK_MAC;
            return GT_NO_MORE;
        }
    }
    else
    {

        if(rxTableMode == GALTIS_RX_ONCE_E || /* the table is flat --> start at index 0 */
           (rxTableMode == GALTIS_RX_CYCLIC_E && /* the table was not "looped" yet -- "cyclic table"*/
            rxEntriesArr[indexToInsertPacket].isEmpty == GT_TRUE))
        {
            *currentIndexPtr = 0;
        }
        else
        {
            *currentIndexPtr = indexToInsertPacket; /* support "cyclic table" */
        }

    }

    currIndex = *currentIndexPtr;

    if(rxEntriesArr[currIndex].isEmpty == GT_FALSE)
    {
        /* if packet is smaller than gtBuf update the gtBufLength */
        if (rxEntriesArr[currIndex].bufferLen < *packetBufLenPtr)
        {
            *packetBufLenPtr = rxEntriesArr[currIndex].bufferLen;
        }

        if (*packetBufLenPtr > maxRxBufferSize)
        {
            /* the packet is bigger than the table packet size */
            *packetBufLenPtr = maxRxBufferSize;
        }

        /* set the real packet size */
        *packetLenPtr = rxEntriesArr[currIndex].packetOriginalLen;

        /* copy the packet to user buffer */
        cmdOsMemCpy(packetBufPtr,
                 rxEntriesArr[currIndex].packetBufferPtr,
                 *packetBufLenPtr);

        rxEntriesArr[currIndex].copyRxInfoFunc(
            rxEntriesArr[currIndex].specificDeviceFormatPtr,
            specificDeviceFormatPtr);
        *queuePtr  = rxEntriesArr[currIndex].queue;
        *devNumPtr = rxEntriesArr[currIndex].devNum;
    }
    else
    {
        rc = GT_NO_MORE;
    }

    CMD_RX_MTX_UNLOCK_MAC;
    return rc;
}

/*******************************************************************************
* cmdCpssRxPkGetFirst
*
* DESCRIPTION:
*       Get Next entry from rxNetworkIf table
*
* INPUTS:
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       currentIndexPtr - get current entry's index
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - the length of the copied packet to gtBuf
*       packetLenPtr    - the Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr       - The Rx queue in which the packet was received.
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*
* RETURNS:
*       GT_OK - on success
*       GT_NO_MORE - when no more entries
*
* COMMENTS:
*
* GalTis:
*       Table - rxNetworkIf
*
*******************************************************************************/
GT_STATUS cmdCpssRxPkGetFirst
(
    OUT GT_U32          *currentIndexPtr,
    OUT GT_U8           *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT GT_U32          *packetLenPtr,
    OUT GT_U8           *devNumPtr,
    OUT GT_U8           *queuePtr,
    OUT void*           specificDeviceFormatPtr

)
{
    return cmdCpssRxPkGet(currentIndexPtr,packetBufPtr,
                      packetBufLenPtr,packetLenPtr,
                      devNumPtr,queuePtr,
                      specificDeviceFormatPtr,
                      GT_FALSE);/* get first */
}

/*******************************************************************************
* cmdCpssRxPkGetNext
*
* DESCRIPTION:
*       Get Next entry from rxNetworkIf table
*
* INPUTS:
*       packetBufLenPtr - the length of the user space for the packet
*       currentIndexPtr - the PREVIOUS entry's index
*
* OUTPUTS:
*       currentIndexPtr - get current entry's index
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - the length of the copied packet to gtBuf
*       packetLenPtr    - the Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr       - The Rx queue in which the packet was received.
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*
* RETURNS:
*       GT_OK - on success
*       GT_NO_MORE - when no more entries
*
* COMMENTS:
*
* GalTis:
*       Table - rxNetworkIf
*
*******************************************************************************/
GT_STATUS cmdCpssRxPkGetNext
(
    INOUT GT_U32        *currentIndexPtr,
    OUT GT_U8           *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT GT_U32          *packetLenPtr,
    OUT GT_U8           *devNumPtr,
    OUT GT_U8           *queuePtr,
    OUT void*           specificDeviceFormatPtr

)
{
    return cmdCpssRxPkGet(currentIndexPtr,packetBufPtr,
                      packetBufLenPtr,packetLenPtr,
                      devNumPtr,queuePtr,
                      specificDeviceFormatPtr,
                      GT_TRUE);/* get next */
}



/*******************************************************************************
* cmdCpssRxPktIsCaptureReady
*
* DESCRIPTION:
*       check if the galtis-cmd is ready to capture rx packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the galtis-cmd is ready to capure rx packets
*       GT_FALSE - the galtis-cmd is NOT ready to capure rx packets
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL cmdCpssRxPktIsCaptureReady(void)
{
    return (doCapture == GT_TRUE && wasInitDone == GT_TRUE) ? GT_TRUE : GT_FALSE;
}


