/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* trafficTableUt.c
*
* DESCRIPTION:
*       manage the Rx Tx tables of the traffic sent from CPU and traffic
*       received at the CPU
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <trafficEngine/trafficTableUt.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsMem.h>

#define RX_DEFULT_NUM_OF_ENTRIES_CNS 10

static UT_TRAFFIC_RX_ENTRY *rxEntriesArr=NULL;
static UT_TRAFFIC_RX_MODE_ENT rxTableMode = UT_TRAFFIC_RX_CYCLIC_E;
static GT_U32  maxRxBufferSize = UT_TRAFFIC_RX_BUFFER_MAX_SIZE_CNS; /*1536*/
static GT_U32  maxNumOfRxEntries = RX_DEFULT_NUM_OF_ENTRIES_CNS;
static GT_U32  indexToInsertPacket = 0;
static GT_U32  doCapture = GT_FALSE;
static GT_BOOL wasInitDone = GT_FALSE;

static GT_U32 globalRxParamSize = 0;

static GT_SEM  utTrafficRxSem = 0;
#define RX_SEM_CREATE_MAC   if(utTrafficRxSem==0) osSemBinCreate("utTrafficRxSem", 1,&utTrafficRxSem)
#define RX_SEM_TAKE_MAC     osSemWait(utTrafficRxSem, OS_WAIT_FOREVER)
#define RX_SEM_GIVE_MAC     osSemSignal(utTrafficRxSem)


#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>
    static GT_U32 exMxPmRxParamSize = sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC);
    #define RX_PARAM_SIZE_DEFINED
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
    #include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>
    #ifndef RX_PARAM_SIZE_DEFINED
        static GT_U32 exMxRxParamSize = sizeof(CPSS_EXMX_NET_RX_PARAMS_STC);
        #define RX_PARAM_SIZE_DEFINED
    #endif /*RX_PARAM_SIZE_DEFINED*/
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
    #ifndef RX_PARAM_SIZE_DEFINED
        static GT_U32 dxChRxParamSize = sizeof(CPSS_DXCH_NET_RX_PARAMS_STC);
        #define RX_PARAM_SIZE_DEFINED
    #endif /*RX_PARAM_SIZE_DEFINED*/
#endif /*CHX_FAMILY*/

#ifndef RX_PARAM_SIZE_DEFINED
    static GT_U32 dxChRxParamSize = 0;
#endif /*RX_PARAM_SIZE_DEFINED*/
/* indication that test should fail on first fail */
GT_U32  utBreakOnFirstFail = 0;

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
        osFree(rxEntriesArr[index].packetBufferPtr);
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
        osFree(rxEntriesArr[index].specificDeviceFormatPtr);
    }

    /* set not used pointer */
    rxEntriesArr[index].specificDeviceFormatPtr = NULL;
}

/*******************************************************************************
* utTrafficRxInitLib
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
*
*******************************************************************************/
static GT_VOID utTrafficRxInitLib(void)
{
    int ii;

    if(wasInitDone == GT_TRUE)
        return;

#ifdef EXMXPM_FAMILY
    globalRxParamSize = exMxPmRxParamSize;
#elif defined EX_FAMILY
    globalRxParamSize = exMxRxParamSize;
#else /*CHX_FAMILY*/
    globalRxParamSize = dxChRxParamSize;
#endif

    /* create semaphore */
    RX_SEM_CREATE_MAC;

    rxEntriesArr = osMalloc(RX_DEFULT_NUM_OF_ENTRIES_CNS * sizeof(UT_TRAFFIC_RX_ENTRY));
    for(ii = 0; ii < RX_DEFULT_NUM_OF_ENTRIES_CNS; ii++)
    {
        rxEntriesArr[ii].isEmpty = GT_TRUE;
    }

    wasInitDone = GT_TRUE;
}

/*******************************************************************************
* utTrafficRxStartCapture
*
* DESCRIPTION:
*       Start/Stop collecting the received packets
*
* INPUTS:
*       start - GT_TRUE - start
*               GT_FALSE - stop
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficRxCapture(
    GT_BOOL enable
)
{
    if(wasInitDone == GT_FALSE)
        utTrafficRxInitLib();

    RX_SEM_TAKE_MAC;
    doCapture = enable;
    RX_SEM_GIVE_MAC;

    return GT_OK;
}

/*******************************************************************************
* utTrafficRxSetMode
*
* DESCRIPTION:
*       set received packets collection mode and parameters
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
*
*******************************************************************************/
GT_STATUS utTrafficRxSetMode
(
    IN UT_TRAFFIC_RX_MODE_ENT   rxMode,
    IN GT_U32       buferSize,
    IN GT_U32       numOfEnteries
)
{
    GT_U32 ii;

    if(wasInitDone == GT_FALSE)
        utTrafficRxInitLib();

    /* Should NOT be inside sem wait (takes the same semaphore)*/
    utTrafficRxPktClearTbl();

    RX_SEM_TAKE_MAC;

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

        osFree(rxEntriesArr);

        wasInitDone = GT_FALSE;
    }

    rxEntriesArr = osMalloc(numOfEnteries * sizeof(UT_TRAFFIC_RX_ENTRY));

    if(rxEntriesArr == NULL)
    {
        RX_SEM_GIVE_MAC;
        return GT_OUT_OF_CPU_MEM;
    }

    wasInitDone = GT_TRUE;

    for(ii = 0; ii < numOfEnteries; ii++)
    {
        rxEntriesArr[ii].isEmpty = GT_TRUE;
    }

    maxNumOfRxEntries = numOfEnteries;

    RX_SEM_GIVE_MAC;


    return GT_OK;
}

/*******************************************************************************
* utTrafficRxPktReceive
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
*       GT_BAD_STATE - the engine not ready/set to "save" the packet with info
*       GT_BAD_PARAM - on bad parameter
*       GT_OUT_OF_CPU_MEM - on CPU memory allocation failure
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS utTrafficRxPktReceive
(
    IN GT_U8           devNum,
    IN GT_U8           queue,
    IN void*           specificDeviceFormatPtr,
    IN UT_TRAFFIC_FREE_RX_INFO_FUNC freeRxInfoFunc,
    IN UT_TRAFFIC_COPY_RX_INFO_FUNC copyRxInfoFunc,
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

    RX_SEM_TAKE_MAC;

    if((rxTableMode == UT_TRAFFIC_RX_ONCE_E && indexToInsertPacket < maxNumOfRxEntries - 1)
        || (rxTableMode == UT_TRAFFIC_RX_CYCLIC_E))
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
        rxEntriesArr[indexToInsertPacket].packetBufferPtr = osMalloc(toCopyBufSize +1);

        if (NULL == rxEntriesArr[indexToInsertPacket].packetBufferPtr)
        {
            RX_SEM_GIVE_MAC;
            return GT_OUT_OF_CPU_MEM;
        }

        rxEntriesArr[indexToInsertPacket].isEmpty = GT_FALSE;
        rxEntriesArr[indexToInsertPacket].devNum = devNum;
        rxEntriesArr[indexToInsertPacket].queue = queue;
        rxEntriesArr[indexToInsertPacket].specificDeviceFormatPtr =
            osMalloc(globalRxParamSize);
        copyRxInfoFunc(specificDeviceFormatPtr,rxEntriesArr[indexToInsertPacket].specificDeviceFormatPtr);

        rxEntriesArr[indexToInsertPacket].freeRxInfoFunc = freeRxInfoFunc;
        rxEntriesArr[indexToInsertPacket].copyRxInfoFunc = copyRxInfoFunc;
        rxEntriesArr[indexToInsertPacket].packetOriginalLen = rxPcktBufSize;
        rxEntriesArr[indexToInsertPacket].bufferLen = toCopyBufSize + 1;

        osMemSet(rxEntriesArr[indexToInsertPacket].packetBufferPtr,0,toCopyBufSize +1);

        tmpBuffSize = 0;
        /* copy packet data to allocated buffer */
        for(ii = 0; ii < numOfBuff; ii++)
        {
            if(tmpBuffSize + buffLen[ii] > toCopyBufSize)
            {
                /* packet length is bigger than destination buffer size */
                osMemCpy(rxEntriesArr[indexToInsertPacket].packetBufferPtr + tmpBuffSize,
                         packetBuffs[ii], toCopyBufSize - tmpBuffSize);
                break;
            }
            /* copy the packet buffer to destination buffer */
            osMemCpy(rxEntriesArr[indexToInsertPacket].packetBufferPtr + tmpBuffSize,
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

    RX_SEM_GIVE_MAC;

    return rc;
}

/*******************************************************************************
* utTrafficRxPktClearTbl
*
* DESCRIPTION:
*       clear utTrafficRxPktClearTbl table
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
*
*******************************************************************************/
GT_STATUS utTrafficRxPktClearTbl(void)
{
    GT_U32 ii;

    if(wasInitDone == GT_FALSE)
        utTrafficRxInitLib();

    RX_SEM_TAKE_MAC;

    for(ii = 0; ii < maxNumOfRxEntries; ii++)
    {
        if(rxEntriesArr[ii].isEmpty == GT_FALSE)
        {
            freeRxPacketInfo(ii);
            rxEntriesArr[ii].isEmpty = GT_TRUE;
        }
    }

    indexToInsertPacket = 0;

    RX_SEM_GIVE_MAC;

    return GT_OK;
}


/*******************************************************************************
* utTrafficRxPkGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       currentIndex - the CURRENT entry's index
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
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
*       GT_OUT_OF_RANGE - index is out of range
*       GT_NOT_FOUND -  entry in index is entry not valid
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficRxPkGet
(
    INOUT GT_U32        currentIndex,
    OUT GT_U8           *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT GT_U32          *packetLenPtr,
    OUT GT_U8           *devNumPtr,
    OUT GT_U8           *queuePtr,
    OUT void*           specificDeviceFormatPtr
)
{

    if(wasInitDone == GT_FALSE)
        utTrafficRxInitLib();

    RX_SEM_TAKE_MAC;

    if(currentIndex >= maxNumOfRxEntries)
    {
        RX_SEM_GIVE_MAC;
        return GT_OUT_OF_RANGE;
    }

    if(rxEntriesArr[currentIndex].isEmpty == GT_TRUE)
    {
        RX_SEM_GIVE_MAC;
        return GT_NOT_FOUND;
    }

    /* if packet is smaller than gtBuf update the gtBufLength */
    if (rxEntriesArr[currentIndex].bufferLen < *packetBufLenPtr)
    {
        *packetBufLenPtr = rxEntriesArr[currentIndex].bufferLen;
    }

    if (*packetBufLenPtr > maxRxBufferSize)
    {
        /* the packet is bigger than the table packet size */
        *packetBufLenPtr = maxRxBufferSize;
    }

    /* set the real packet size */
    *packetLenPtr = rxEntriesArr[currentIndex].packetOriginalLen;

    /* copy the packet to user buffer */
    osMemCpy(packetBufPtr,
             rxEntriesArr[currentIndex].packetBufferPtr,
             *packetBufLenPtr);

    rxEntriesArr[currentIndex].copyRxInfoFunc(
        rxEntriesArr[currentIndex].specificDeviceFormatPtr,
        specificDeviceFormatPtr);
    *queuePtr  = rxEntriesArr[currentIndex].queue;
    *devNumPtr = rxEntriesArr[currentIndex].devNum;

    RX_SEM_GIVE_MAC;
    return GT_OK;
}

/*******************************************************************************
* utTrafficRxPktIsCaptureReady
*
* DESCRIPTION:
*       check if the engine is ready to capture rx packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the engine is ready to capture rx packets
*       GT_FALSE - the engine is NOT ready to capture rx packets
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL utTrafficRxPktIsCaptureReady(void)
{
    return (doCapture == GT_TRUE && wasInitDone == GT_TRUE) ? GT_TRUE : GT_FALSE;
}


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
        osFree(specificDeviceFormatPtr);
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
             globalRxParamSize);

    return;
}

/*******************************************************************************
* rxPacketReceiveCb
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
GT_STATUS rxPacketReceiveCb
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    IN GT_U32                               numOfBuff,
    IN GT_U8                               *packetBuffs[],
    IN GT_U32                               buffLen[],
    IN void                                 *rxParamsPtr
)
{
    GT_STATUS rc;
    void *tmpRxParamPtr;

    tmpRxParamPtr = osMalloc(globalRxParamSize);
    if(tmpRxParamPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    osMemCpy(tmpRxParamPtr,rxParamsPtr,globalRxParamSize);

    rc = utTrafficRxPktReceive(devNum,queueIdx,tmpRxParamPtr,
                         &rxInfoFree,&rxInfoCopy,
                         numOfBuff,packetBuffs,buffLen);
    if(rc != GT_OK)
    {
        osFree(tmpRxParamPtr);
    }

    return rc;
}

/*******************************************************************************
* utBreakOnFirstFailSet
*
* DESCRIPTION:
*       indication that test should fail on first fail (or ignore fails)
*
* INPUTS:
*       breakOnFirstFail - to break on first fail
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - always
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utBreakOnFirstFailSet(
    GT_BOOL breakOnFirstFail
)
{
    utBreakOnFirstFail =  (breakOnFirstFail == GT_TRUE) ? 1 : 0;
    return GT_OK;
}

