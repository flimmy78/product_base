/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* trafficTableUt.h
*
* DESCRIPTION:
*       manage the Rx Tx tables of the traffic sent from CPU and traffic
*       received at the CPU
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __trafficTableUth
#define __trafficTableUth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UT_TRAFFIC_RX_BUFFER_MAX_SIZE_CNS   0x600   /*1536*/

#define UT_TRAFFIC_TX_BUFFER_MAX_SIZE_CNS   0x600   /*1536*/

/* indication that test should fail on first fail */
extern GT_U32   utBreakOnFirstFail;

/* macro that check return value and print error location */
#define RC_CHECK(returnVal)                     \
    {                                           \
        GT_STATUS _rc = returnVal;              \
        if(_rc != GT_OK)                        \
        {                                       \
            osPrintf(" failed rc=[0x%lx] in file[%s] in line [%d]\n",_rc,__FILE__,__LINE__);\
            if(utBreakOnFirstFail)              \
            {                                   \
                return _rc;                     \
            }                                   \
        }                                       \
    }

/*******************************************************************************
* UT_TRAFFIC_FREE_RX_INFO_FUNC
*
* DESCRIPTION:
*       prototype of callback function to free the specific rx info format.
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
typedef void (*UT_TRAFFIC_FREE_RX_INFO_FUNC)
(
    IN void*  specificDeviceFormatPtr
);

/*******************************************************************************
* UT_TRAFFIC_COPY_RX_INFO_FUNC
*
* DESCRIPTION:
*       prototype of callback function to COPY the specific rx info format.
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
typedef void (*UT_TRAFFIC_COPY_RX_INFO_FUNC)
(
    IN  const void*  srcSpecificDeviceFormatPtr,
    OUT void*        dstSpecificDeviceFormatPtr
);

/*
 * Typedef: enum UT_TRAFFIC_RX_MODE_ENT
 *
 * Description:
 *      The table entry adding mode for the received packets
 *
 * Enumerations:
 *      UT_TRAFFIC_RX_CYCLIC_E - for cyclic collection.
 *      UT_TRAFFIC_RX_ONCE_E   - for collecting until buffer is full
 */
typedef enum
{
    UT_TRAFFIC_RX_CYCLIC_E,
    UT_TRAFFIC_RX_ONCE_E
} UT_TRAFFIC_RX_MODE_ENT;

/***** Capture packet table entry  *******************************/

/*
 * Typedef: UT_TRAFFIC_RX_ENTRY
 *
 * Description:
 *      Entry of the Rx packet table
 *
 * Fields:
 *      isEmpty - is entry empty flag
 *      devNum  - The device number in which the packet was received.
 *      queue   - The Rx queue in which the packet was received.
 *      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
 *                             format of DXCH / EXMX /DXSAL ...
 *                             NOTE : this pointer is allocated by the specific
 *                             device "C" file.
 *      freeRxInfoFunc - callback function to free the specific rx info format,
 *      copyRxInfoFunc - callback function to copy the specific rx info format,
 *      packetBufferPtr - pointer to the packet buffer (allocated by this C file)
 *      packetOriginalLen - length of original packet (packet may have been too
 *                          long , so we didn't keep all of it only 1536 bytes)
 *      bufferLen -length of the buffer allocated in packetBufferPtr
 */
typedef struct {
    GT_BOOL         isEmpty;
    GT_U8           devNum;
    GT_U8           queue;
    void*           specificDeviceFormatPtr;
    UT_TRAFFIC_FREE_RX_INFO_FUNC freeRxInfoFunc;
    UT_TRAFFIC_COPY_RX_INFO_FUNC copyRxInfoFunc;
    GT_U8*          packetBufferPtr;
    GT_U32          packetOriginalLen;
    GT_U32          bufferLen;
} UT_TRAFFIC_RX_ENTRY;

/*******************************************************************************
* utTrafficRxCapture
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
);


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
);

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
);


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
GT_STATUS utTrafficRxPktClearTbl(void);

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
*
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
);

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
GT_BOOL utTrafficRxPktIsCaptureReady(void);

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
);

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
    IN void                                 *rxParamsPtr
);

/*******************************************************************************
* appDemoDxChNetRxPacketCbRegister
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
extern GT_STATUS appDemoDxChNetRxPacketCbRegister
(
    IN  RX_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __trafficTableUth */

