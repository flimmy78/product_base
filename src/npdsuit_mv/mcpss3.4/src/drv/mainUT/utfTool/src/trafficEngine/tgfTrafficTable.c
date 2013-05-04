/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrafficTable.c
*
* DESCRIPTION:
*       Manage the Rx Tx tables of the traffic sent from CPU and traffic
*       received at the CPU
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <trafficEngine/tgfTrafficTable.h>
#include <gtOs/gtOsSem.h>


/******************************************************************************\
 *                              Define section                                *
\******************************************************************************/

#ifdef CHX_FAMILY
    #ifndef TGF_RX_PARAM_SIZE_DEFINED
        #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>

        static GT_U32 prvTgfDxChRxParamSize = sizeof(CPSS_DXCH_NET_RX_PARAMS_STC);
        #define TGF_RX_PARAM_SIZE_DEFINED
    #endif /* TGF_RX_PARAM_SIZE_DEFINED */
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #ifndef TGF_RX_PARAM_SIZE_DEFINED
        #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>

        static GT_U32 prvTgfExMxPmRxParamSize = sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC);
        #define TGF_RX_PARAM_SIZE_DEFINED
    #endif /* TGF_RX_PARAM_SIZE_DEFINED */
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    #ifndef TGF_RX_PARAM_SIZE_DEFINED
        #include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>

        static GT_U32 prvTgfExMxRxParamSize = sizeof(CPSS_EXMX_NET_RX_PARAMS_STC);
        #define TGF_RX_PARAM_SIZE_DEFINED
    #endif /* TGF_RX_PARAM_SIZE_DEFINED */
#endif /* EX_FAMILY */

#ifndef TGF_RX_PARAM_SIZE_DEFINED
    static GT_U32 prvTgfDxChRxParamSize = 0;
#endif /* TGF_RX_PARAM_SIZE_DEFINED */

#define TGF_RX_DEF_NUM_OF_ENTRIES_CNS 1024
static TGF_RX_ENTRY     prvTgfRxEntriesArr[TGF_RX_DEF_NUM_OF_ENTRIES_CNS];
static TGF_RX_MODE_ENT  prvTgfRxTableMode = TGF_RX_CYCLIC_E;
static GT_U32           prvTgfMaxRxBufferSize = TGF_RX_BUFFER_MAX_SIZE_CNS;
static GT_U32           prvTgfMaxNumOfRxEntries = TGF_RX_DEF_NUM_OF_ENTRIES_CNS;
static GT_U32           prvTgfIndexToInsertPacket = 0;
static GT_U32           prvTgfDoCapture = GT_FALSE;
static GT_BOOL          prvTgfIsInitialized = GT_FALSE;
static GT_U32           prvTgfGlobalRxParamSize = 0;
static GT_SEM           prvTgfTrafficRxSem = 0;
static GT_BOOL          prvTgfRxCbRegistered = GT_FALSE;

/* macro to check if library initialized */
#define TGF_RX_LIB_INIT_MAC                                                   \
            if (!prvTgfIsInitialized)                                         \
            {                                                                 \
                prvTgfTrafficTableRxLibInit();                                \
            }

/* macro to create semaphore */
#define TGF_RX_SEM_CREATE_MAC                                                 \
            if(prvTgfTrafficRxSem == 0)                                       \
            {                                                                 \
                osSemBinCreate("prvTgfTrafficRxSem", 1, &prvTgfTrafficRxSem); \
            }
/* macro to get semaphore */
#define TGF_RX_SEM_TAKE_MAC     osSemWait(prvTgfTrafficRxSem, OS_WAIT_FOREVER)
/* macro to release semaphore */
#define TGF_RX_SEM_GIVE_MAC     osSemSignal(prvTgfTrafficRxSem)


/******************************************************************************\
 *                     Extern function's declaration                          *
\******************************************************************************/

#ifdef CHX_FAMILY
/* Register a CB function to be called on every RX packet to CPU */
extern GT_STATUS appDemoDxChNetRxPacketCbRegister
(
    IN  RX_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/* Register a CB function to be called on every RX packet to CPU */
extern GT_STATUS appDemoExMxPmNetRxPacketCbRegister
(
    IN  RX_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
);
#endif /* EXMXPM_FAMILY */

/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

/*******************************************************************************
* prvTgfTrafficTableRxPacketInfoFree
*
* DESCRIPTION:
*       Free the specific rx packet info, by calling to the registered callback
*
* INPUTS:
*       index - index to the prvTgfRxEntriesArr array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficTableRxPacketInfoFree
(
    IN GT_U32           index
)
{
    if (index >= prvTgfMaxNumOfRxEntries)
    {
        return GT_BAD_PARAM;
    }

    if(prvTgfRxEntriesArr[index].packetBufferPtr)
    {
        cpssOsFree(prvTgfRxEntriesArr[index].packetBufferPtr);
        prvTgfRxEntriesArr[index].packetBufferPtr = NULL;
    }

    if(prvTgfRxEntriesArr[index].specificDeviceFormatPtr == NULL)
    {
        return GT_OK;
    }

    if(prvTgfRxEntriesArr[index].freeRxInfoFunc)
    {
        /* free by call back*/
        prvTgfRxEntriesArr[index].freeRxInfoFunc(prvTgfRxEntriesArr[index].specificDeviceFormatPtr);
    }
    else
    {
        /* free with no specific extra needs */
        cpssOsFree(prvTgfRxEntriesArr[index].specificDeviceFormatPtr);
    }

    /* set not used pointer */
    prvTgfRxEntriesArr[index].specificDeviceFormatPtr = NULL;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficTableRxLibInit
*
* DESCRIPTION:
*       Initialize the rx library
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
*******************************************************************************/
static GT_VOID prvTgfTrafficTableRxLibInit
(
    GT_VOID
)
{
    GT_U32 i = 0;

    if(GT_TRUE == prvTgfIsInitialized)
        return;

#ifdef CHX_FAMILY
    prvTgfGlobalRxParamSize = prvTgfDxChRxParamSize;
#elif defined EXMXPM_FAMILY
    prvTgfGlobalRxParamSize = prvTgfExMxPmRxParamSize;
#elif defined EX_FAMILY
    prvTgfGlobalRxParamSize = prvTgfExMxRxParamSize;
#endif

    /* create semaphore */
    TGF_RX_SEM_CREATE_MAC;
    for(i = 0; i < TGF_RX_DEF_NUM_OF_ENTRIES_CNS; i++)
    {
        prvTgfRxEntriesArr[i].isEmpty = GT_TRUE;
    }

    prvTgfIsInitialized = GT_TRUE;
}


/******************************************************************************\
 *                              API implementation                            *
\******************************************************************************/

/*******************************************************************************
* tgfTrafficTableRxStartCapture
*
* DESCRIPTION:
*       Start/Stop collecting the received packets
*
* INPUTS:
*       enable - GT_TRUE - start capture
*                GT_FALSE - stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxStartCapture
(
    IN GT_BOOL          enable
)
{
    /* Initialize Rx library */
    TGF_RX_LIB_INIT_MAC;

    TGF_RX_SEM_TAKE_MAC;
    prvTgfDoCapture = enable;
    TGF_RX_SEM_GIVE_MAC;

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTableRxModeSet
*
* DESCRIPTION:
*       Set received packets collection mode and parameters
*
* INPUTS:
*       rxMode        - collection mode
*       buferSize     - buffer size
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_OUT_OF_CPU_MEM - memory allocation Failure
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxModeSet
(
    IN TGF_RX_MODE_ENT  rxMode,
    IN GT_U32           buferSize
)
{
    GT_STATUS   rc = GT_OK;
    /* Initialize Rx library */
    TGF_RX_LIB_INIT_MAC;

    /* Should NOT be inside sem wait (takes the same semaphore)*/
    rc = tgfTrafficTableRxPcktTblClear();
    if (rc != GT_OK)
    {
        return rc;
    }

    TGF_RX_SEM_TAKE_MAC;

    prvTgfRxTableMode     = rxMode;
    prvTgfMaxRxBufferSize = buferSize;
    TGF_RX_SEM_GIVE_MAC;

    return rc;
}

/*******************************************************************************
* prvTgfTrafficTableRxPcktReceive
*
* DESCRIPTION:
*       Receive packet callback function. The caller wants to register its
*       packet info with the manager array.
*
* INPUTS:
*      devNum               - device number in which the packet was received
*      queue                - Rx queue in which the packet was received
*      specificDevFormatPtr - (pointer to) the specific device Rx info format
*      freeRxInfoFunc       - callback to free the specific rx info format
*      copyRxInfoFunc       - callback to copy the specific rx info format
*      numOfBuff            - num of used buffs in packetBuffs
*      packetBuffs          - the received packet buffers list
*      buffLen              - list of buffer lengths for packetBuffs
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_FULL           - when buffer is full
*       GT_BAD_STATE      - the engine not ready
*       GT_BAD_PARAM      - on bad parameter
*       GT_OUT_OF_CPU_MEM - on CPU memory allocation failure
*
* COMMENTS:
*       This function give services to all the rx packets info formats.
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficTableRxPcktReceive
(
    IN GT_U8                 devNum,
    IN GT_U8                 queue,
    IN GT_VOID               *specificDeviceFormatPtr,
    IN TGF_FREE_RX_INFO_FUNC freeRxInfoFunc,
    IN TGF_COPY_RX_INFO_FUNC copyRxInfoFunc,
    IN GT_U32                numOfBuff,
    IN GT_U8                 *packetBuffs[],
    IN GT_U32                buffLen[]
)
{
    GT_STATUS   rc            = GT_OK;
    GT_U32      tmpBuffSize   = 0;
    GT_U32      toCopyBufSize = 0;
    GT_U32      rxPcktBufSize = 0;
    GT_U32      i             = 0;

    CPSS_NULL_PTR_CHECK_MAC(copyRxInfoFunc);
    CPSS_NULL_PTR_CHECK_MAC(packetBuffs);
    CPSS_NULL_PTR_CHECK_MAC(buffLen);
    CPSS_NULL_PTR_CHECK_MAC(specificDeviceFormatPtr);

    if(!prvTgfDoCapture || !prvTgfIsInitialized)
        return GT_BAD_STATE;

    TGF_RX_SEM_TAKE_MAC;

    if((prvTgfRxTableMode == TGF_RX_ONCE_E && prvTgfIndexToInsertPacket < prvTgfMaxNumOfRxEntries - 1)
        || (prvTgfRxTableMode == TGF_RX_CYCLIC_E))
    {
        if(prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].isEmpty != GT_TRUE)
        {
            prvTgfTrafficTableRxPacketInfoFree(prvTgfIndexToInsertPacket);
        }

        /* get the packet actual length */
        rxPcktBufSize = 0;
        for(i = 0; i < numOfBuff; i++)
        {
            rxPcktBufSize += buffLen[i];
        }

        /* set the number of buffer bytes to copy */
        if (rxPcktBufSize > prvTgfMaxRxBufferSize)
        {
            /* packet length is bigger than destination buffer size. */
            toCopyBufSize = prvTgfMaxRxBufferSize;
        }
        else
        {
            toCopyBufSize = rxPcktBufSize;
        }

        /* allocate memory for the packet data */
        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].packetBufferPtr = cpssOsMalloc(toCopyBufSize + 1);

        if (NULL == prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].packetBufferPtr)
        {
            TGF_RX_SEM_GIVE_MAC;
            return GT_OUT_OF_CPU_MEM;
        }

        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].isEmpty = GT_FALSE;
        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].devNum = devNum;
        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].queue = queue;
        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].specificDeviceFormatPtr =
            cpssOsMalloc(prvTgfGlobalRxParamSize);

        copyRxInfoFunc(specificDeviceFormatPtr,
                       prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].specificDeviceFormatPtr);

        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].freeRxInfoFunc = freeRxInfoFunc;
        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].copyRxInfoFunc = copyRxInfoFunc;
        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].packetOriginalLen = rxPcktBufSize;
        prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].bufferLen = toCopyBufSize + 1;

        cpssOsMemSet(prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].packetBufferPtr, 0, toCopyBufSize + 1);

        tmpBuffSize = 0;

        /* copy packet data to allocated buffer */
        for(i = 0; i < numOfBuff; i++)
        {
            if(tmpBuffSize + buffLen[i] > toCopyBufSize)
            {
                /* packet length is bigger than destination buffer size */
                cpssOsMemCpy(prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].packetBufferPtr + tmpBuffSize,
                             packetBuffs[i], toCopyBufSize - tmpBuffSize);
                break;
            }

            /* copy the packet buffer to destination buffer */
            cpssOsMemCpy(prvTgfRxEntriesArr[prvTgfIndexToInsertPacket].packetBufferPtr + tmpBuffSize,
                         packetBuffs[i], buffLen[i]);
            tmpBuffSize += buffLen[i];
        }

        if((prvTgfIndexToInsertPacket == prvTgfMaxNumOfRxEntries - 1))
            prvTgfIndexToInsertPacket = 0;
        else
            prvTgfIndexToInsertPacket++;

        rc = GT_OK;
    }
    else
    {
        rc = GT_FULL;
    }

    TGF_RX_SEM_GIVE_MAC;

    return rc;
}

/*******************************************************************************
* tgfTrafficTableRxPcktTblClear
*
* DESCRIPTION:
*       Clear table
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
*******************************************************************************/
GT_STATUS tgfTrafficTableRxPcktTblClear
(
    GT_VOID
)
{
    GT_U32 i = 0;

    /* Initialize Rx library */
    TGF_RX_LIB_INIT_MAC;

    TGF_RX_SEM_TAKE_MAC;

    for(i = 0; i < prvTgfMaxNumOfRxEntries; i++)
    {
        if(prvTgfRxEntriesArr[i].isEmpty == GT_FALSE)
        {
            prvTgfTrafficTableRxPacketInfoFree(i);
            prvTgfRxEntriesArr[i].isEmpty = GT_TRUE;
        }
    }

    prvTgfIndexToInsertPacket = 0;

    TGF_RX_SEM_GIVE_MAC;

    return GT_OK;
}


/*******************************************************************************
* tgfTrafficTableRxPcktGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       currentIndex    - the current entry's index
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - the length of the copied packet to gtBuf
*       packetLenPtr    - the Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr        - The Rx queue in which the packet was received.
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*
* RETURNS:
*       GT_OK - on success
*       GT_OUT_OF_RANGE - index is out of range
*       GT_NOT_FOUND -  entry in index is entry not valid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxPcktGet
(
    INOUT GT_U32        currentIndex,
    OUT   GT_U8         *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT   GT_U32        *packetLenPtr,
    OUT   GT_U8         *devNumPtr,
    OUT   GT_U8         *queuePtr,
    OUT   GT_VOID       *specificDeviceFormatPtr
)
{
    /* Initialize Rx library */
    TGF_RX_LIB_INIT_MAC;

    TGF_RX_SEM_TAKE_MAC;

    if(currentIndex >= prvTgfMaxNumOfRxEntries)
    {
        TGF_RX_SEM_GIVE_MAC;
        return GT_OUT_OF_RANGE;
    }

    if(prvTgfRxEntriesArr[currentIndex].isEmpty == GT_TRUE)
    {
        TGF_RX_SEM_GIVE_MAC;
        return GT_NOT_FOUND;
    }

    /* if packet is smaller than gtBuf update the gtBufLength */
    if (prvTgfRxEntriesArr[currentIndex].bufferLen < *packetBufLenPtr)
    {
        *packetBufLenPtr = prvTgfRxEntriesArr[currentIndex].bufferLen;
    }

    if (*packetBufLenPtr > prvTgfMaxRxBufferSize)
    {
        /* the packet is bigger than the table packet size */
        *packetBufLenPtr = prvTgfMaxRxBufferSize;
    }

    /* set the real packet size */
    *packetLenPtr = prvTgfRxEntriesArr[currentIndex].packetOriginalLen;

    /* copy the packet to user buffer */
    cpssOsMemCpy(packetBufPtr,
                 prvTgfRxEntriesArr[currentIndex].packetBufferPtr,
                 *packetBufLenPtr);

    prvTgfRxEntriesArr[currentIndex].copyRxInfoFunc(
        prvTgfRxEntriesArr[currentIndex].specificDeviceFormatPtr,
        specificDeviceFormatPtr);

    *queuePtr  = prvTgfRxEntriesArr[currentIndex].queue;
    *devNumPtr = prvTgfRxEntriesArr[currentIndex].devNum;

    TGF_RX_SEM_GIVE_MAC;

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTableRxPcktIsCaptureReady
*
* DESCRIPTION:
*       Check if the engine is ready to capture rx packets
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
*******************************************************************************/
GT_BOOL tgfTrafficTableRxPcktIsCaptureReady
(
    GT_VOID
)
{
    return (prvTgfDoCapture == GT_TRUE &&
            prvTgfIsInitialized == GT_TRUE) ? GT_TRUE : GT_FALSE;
}

/*******************************************************************************
* prvTgfTrafficTableRxInfoFree
*
* DESCRIPTION:
*       Function to free the specific rx info format.
*
* INPUTS:
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK      - on success
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficTableRxInfoFree
(
    IN GT_VOID          *specificDeviceFormatPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(specificDeviceFormatPtr);

    if(specificDeviceFormatPtr)
    {
        cpssOsFree(specificDeviceFormatPtr);
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficTableRxInfoCopy
*
* DESCRIPTION:
*       Function to COPY the specific rx info format.
*
* INPUTS:
*       srcSpecificDeviceFormatPtr - (pointer to) the SOURCE specific device Rx
*                                    info format.
*
* OUTPUTS:
*      dstSpecificDeviceFormatPtr - (pointer to) the DESTINATION specific device Rx
*                                   info format.
*
* RETURNS:
*       GT_OK      - on success
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficTableRxInfoCopy
(
    IN  const GT_VOID   *srcSpecificDeviceFormatPtr,
    OUT       GT_VOID   *dstSpecificDeviceFormatPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(srcSpecificDeviceFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(dstSpecificDeviceFormatPtr);

    cpssOsMemCpy(dstSpecificDeviceFormatPtr,
                 srcSpecificDeviceFormatPtr,
                 prvTgfGlobalRxParamSize);

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTableRxPcktReceiveCb
*
* DESCRIPTION:
*       Function called to handle incoming Rx packet in the CPU
*
* INPUTS:
*       devNum       - Device number.
*       queueIdx     - The queue from which this packet was received.
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (pointer to)information parameters of received packets
*
* RETURNS:
*       GT_OK      - on success
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxPcktReceiveCb
(
    IN GT_U8            devNum,
    IN GT_U8            queueIdx,
    IN GT_U32           numOfBuff,
    IN GT_U8            *packetBuffs[],
    IN GT_U32           buffLen[],
    IN GT_VOID          *rxParamsPtr
)
{
    GT_STATUS rc             = GT_OK;
    GT_VOID   *tmpRxParamPtr = NULL;

    CPSS_NULL_PTR_CHECK_MAC(packetBuffs);
    CPSS_NULL_PTR_CHECK_MAC(buffLen);
    CPSS_NULL_PTR_CHECK_MAC(rxParamsPtr);

    tmpRxParamPtr = cpssOsMalloc(prvTgfGlobalRxParamSize);
    if(tmpRxParamPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    cpssOsMemCpy(tmpRxParamPtr, rxParamsPtr, prvTgfGlobalRxParamSize);

    rc = prvTgfTrafficTableRxPcktReceive(devNum, queueIdx, tmpRxParamPtr,
                                         &prvTgfTrafficTableRxInfoFree,
                                         &prvTgfTrafficTableRxInfoCopy,
                                         numOfBuff, packetBuffs, buffLen);
    cpssOsFree(tmpRxParamPtr);
    return rc;
}

/*******************************************************************************
* tgfTrafficTableNetRxPacketCbRegister
*
* DESCRIPTION:
*       Register a CB function to be called on every RX packet to CPU
*
* INPUTS:
*       rxPktReceiveCbFun - CB function to be called on every RX packet to CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_STATE - om illegal state
*
* COMMENTS:
*       Function is implemented in the AppDemo application
*
*******************************************************************************/
GT_STATUS tgfTrafficTableNetRxPacketCbRegister
(
    IN  RX_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
)
{
    GT_STATUS   rc = GT_FAIL;

    /* check if CB already registered */
    if (GT_FALSE == prvTgfRxCbRegistered)
    {
#ifdef CHX_FAMILY
        rc = appDemoDxChNetRxPacketCbRegister(rxPktReceiveCbFun);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
        rc = appDemoExMxPmNetRxPacketCbRegister(rxPktReceiveCbFun);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
        rc = appDemoExMxNetRxPacketCbRegister(rxPktReceiveCbFun);
#endif /* EX_FAMILY */
        if (GT_OK != rc)
        {
            return rc;
        }

        prvTgfRxCbRegistered = GT_TRUE;
    }
    else
    {
        return GT_ALREADY_EXIST;
    }

    return rc;
}

