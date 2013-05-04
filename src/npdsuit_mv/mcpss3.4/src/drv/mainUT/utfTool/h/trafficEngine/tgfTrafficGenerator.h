/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrafficGenerator.h
*
* DESCRIPTION:
*   - emulate traffic generator capabilities
*    reset counters
*    read counters
*    send traffic
*    capture received traffic
*    set trigger on received traffic
*    ...
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfTrafficGeneratorh
#define __tgfTrafficGeneratorh

#include <cpss/generic/port/cpssPortStat.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>

#include <trafficEngine/tgfTrafficEngine.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************\
 *                          Enumerations definitions                          *
\******************************************************************************/

/*
 * typedef: enum TGF_PACKET_TYPE_ENT
 *
 * Description: Enumeration of packet types that receive in the CPU table
 *
 * Enumerations:
 *    TGF_PACKET_TYPE_REGULAR_E - the packet received in the CPU ,
 *          was trapped/mirrored/forwarded due test configuration.
 *    TGF_PACKET_TYPE_CAPTURE_E - the packet received in the CPU ,
 *          was trapped by the 'Cpture' emulation , that we supply in order to
 *          capture traffic that egress a port.
 */
typedef enum
{
    TGF_PACKET_TYPE_REGULAR_E,
    TGF_PACKET_TYPE_CAPTURE_E
} TGF_PACKET_TYPE_ENT;


/*
 * typedef: enum TGF_CAPTURE_MODE_ENT
 *
 * Description: Enumeration of packet capture mode
 *
 * Enumerations:
 *    TGF_CAPTURE_MODE_PCL_E - packet is captured by PCL
 *
 *    TGF_CAPTURE_MODE_MIRRORING_E - packet is captured by mirroring to cpu port
 *
 */
typedef enum
{
    TGF_CAPTURE_MODE_PCL_E,
    TGF_CAPTURE_MODE_MIRRORING_E
} TGF_CAPTURE_MODE_ENT;


/******************************************************************************\
 *                               Struct definitions                           *
\******************************************************************************/

/*
 * typedef: structure TGF_NET_SDMA_TX_PARAMS_STC
 *
 * Description: structure of Tx parameters, that should be set to PP when using
 *              the SDMA channel
 *
 * fields:
 *      recalcCrc - GT_TRUE  - the PP should add CRC to the transmitted packet,
 *                  GT_FALSE - leave packet unchanged.
 *                  NOTE : The DXCH device always add 4 bytes of CRC when need
 *                         to recalcCrc = GT_TRUE
 *
 *
 *      txQueue - the queue that packet should be sent to CPU port. (0..7)
 *
 *      evReqHndl - The application handle got from cpssEventBind for
 *                  CPSS_PP_TX_BUFFER_QUEUE_E events.
 *
 *     invokeTxBufferQueueEvent - invoke Tx buffer Queue event.
 *              when the SDMA copies the buffers of the packet from the CPU ,
 *              the PP may invoke the event of CPSS_PP_TX_BUFFER_QUEUE_E.
 *              this event notify the CPU that the Tx buffers of the packet can
 *              be reused by CPU for other packets. (this in not event of
 *              CPSS_PP_TX_END_E that notify that packet left the SDMA)
 *              The invokeTxBufferQueueEvent parameter control the invoking of
 *              that event for this Packet.
 *              GT_TRUE - PP will invoke the event when buffers are copied (for
 *                        this packet's buffers).
 *              GT_FALSE - PP will NOT invoke the event when buffers are copied.
 *                        (for this packet's buffers).
 *              NOTE :
 *                  when the sent is done as "Tx synchronic" this parameter
 *                  IGNORED (the behavior will be as GT_FALSE)
 */
typedef struct
{
    GT_BOOL                         recalcCrc;
    GT_U8                           txQueue;
    GT_U32                          evReqHndl;
    GT_BOOL                         invokeTxBufferQueueEvent;
} TGF_NET_SDMA_TX_PARAMS_STC;

/*
 * typedef: structure TGF_NET_DSA_STC
 *
 * Description: structure of specific DSA tag parameters
 *
 * fields:
 *      dsaCmdIsToCpu - indicates that the DSA command is 'to CPU'
 *      cpuCode       - CPU codes that indicate the reason for sending
 *                      a packet to the CPU.
 *      srcIsTrunk    - is source Trunk
 *      devNum        - the source device number
 *      portNum       - the source port number
 *      originByteCount - The packet's original byte count (TO_CPU packets).
 */
typedef struct
{
    GT_BOOL                     dsaCmdIsToCpu;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;
    GT_BOOL                     srcIsTrunk;
    GT_U8                       devNum;
    GT_U8                       portNum;
    GT_U32                      originByteCount;
} TGF_NET_DSA_STC;


/******************************************************************************\
 *                            Public API section                              *
\******************************************************************************/

/*******************************************************************************
* tgfTrafficGeneratorCpuDeviceSet
*
* DESCRIPTION:
*       Set the CPU device though it we send traffic to the 'Loop back port'
*
* INPUTS:
*       cpuDevice - the CPU device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCpuDeviceSet
(
    IN GT_U8    cpuDevice
);

/*******************************************************************************
* tgfTrafficGeneratorPortLoopbackModeEnableSet
*
* DESCRIPTION:
*       Set port in 'loopback' mode
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (loopback/no loopback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortLoopbackModeEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
);

/*******************************************************************************
* tgfTrafficGeneratorPortForceLinkUpEnableSet
*
* DESCRIPTION:
*       Set port in 'force linkup' mode
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (force/not force)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortForceLinkUpEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
);

/*******************************************************************************
* tgfTrafficGeneratorPortCountersEthReset
*
* DESCRIPTION:
*       Reset the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortCountersEthReset
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr
);

/*******************************************************************************
* tgfTrafficGeneratorPortCountersEthRead
*
* DESCRIPTION:
*       Read the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       countersPtr - (pointer to) the counters of port
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortCountersEthRead
(
    IN  CPSS_INTERFACE_INFO_STC        *portInterfacePtr,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC  *countersPtr
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthTransmit
*
* DESCRIPTION:
*       Transmit the traffic to the port
*       NOTE:
*           1. allow to transmit with chunks of burst:
*              after x sent frames --> do sleep of y millisecond
*              see parameters
*
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       packetInfoPtr    - (pointer to) the packet info
*       burstCount       - number of frames (non-zero value)
*       numVfd           - number of VFDs
*       vfdArray         - pointer to array of VFDs (may be NULL if numVfd = 0)
*       sleepAfterXCount - do 'sleep' after X packets sent
*                          when = 0 , meaning NO SLEEP needed during the burst
*                          of 'burstCount'
*       sleepTime        - sleep time (in milliseconds) after X packets sent , see
*                          parameter sleepAfterXCount
*       traceBurstCount  - number of packets in burst count that will be printed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The packet will ingress to the PP after 'loopback' and will act as
*       it was received from a traffic generator connected to the port
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthTransmit
(
    IN CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN TGF_PACKET_STC                  *packetInfoPtr,
    IN GT_U32                           burstCount,
    IN GT_U32                           numVfd,
    IN TGF_VFD_INFO_STC                 vfdArray[],
    IN GT_U32                           sleepAfterXCount,
    IN GT_U32                           sleepTime,
    IN GT_U32                           traceBurstCount
);

/*******************************************************************************
* tgfTrafficGeneratorRxInCpuGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       packetType      - the packet type to get
*       getFirst        - get first/next entry
*       trace           - enable\disable packet tracing
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - length of the copied packet to gtBuf
*       packetLenPtr    - Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr        - Rx queue in which the packet was received.
*       rxParamsPtr     - specific device Rx info format.
*
* RETURNS:
*       GT_OK      - on success
*       GT_NO_MORE - on more entries
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       For 'captured' see API tgfTrafficGeneratorPortTxEthCaptureSet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorRxInCpuGet
(
    IN    TGF_PACKET_TYPE_ENT   packetType,
    IN    GT_BOOL               getFirst,
    IN    GT_BOOL               trace,
    OUT   GT_U8                *packetBufPtr,
    INOUT GT_U32               *packetBufLenPtr,
    OUT   GT_U32               *packetLenPtr,
    OUT   GT_U8                *devNumPtr,
    OUT   GT_U8                *queuePtr,
    OUT   TGF_NET_DSA_STC      *rxParamsPtr
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthTriggerCountersGet
*
* DESCRIPTION:
*       Get number of triggers on port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       numVfd           - number of VFDs
*       vfdArray         - pointer to array of VFDs
*
* OUTPUTS:
*       numTriggersBmpPtr - (pointer to) bitmap of triggers
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The 'captured' (emulation of capture) must be set on this port
*       see tgfTrafficGeneratorPortTxEthCaptureSet
*
*       Triggers bitmap has the following structure:
*           N - number of captured packets on this port
*           K - number of VFDs (equal to numVfd)
*           |      Packet0     |      Packet1     |     |       PacketN    |
*           +----+----+---+----+----+----+---+----+ ... +----+----+---+----+ 
*           |Bit0|Bit1|...|BitK|Bit0|Bit1|...|BitK|     |Bit0|Bit1|...|BitK|
*           +----+----+---+----+----+----+---+----+     +----+----+---+----+
*       Bit[i] for Packet[j] set to 1 means that VFD[i] is matched for captured
*       packet number j.
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthTriggerCountersGet
(
    IN  CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN  GT_U32                           numVfd,
    IN  TGF_VFD_INFO_STC                 vfdArray[],
    OUT GT_U32                          *numTriggersBmpPtr
);

/*******************************************************************************
* tgfTrafficGeneratorTxEthTriggerCheck
*
* DESCRIPTION:
*       Check if the buffer triggered by the trigger parameters
*
* INPUTS:
*       bufferPtr    - (pointer to) the buffer
*       bufferLength - length of the buffer
*       numVfd       - number of VFDs
*       vfdArray     - array of VFDs
* OUTPUTS:
*       triggeredPtr - (pointer to) triggered / not triggered
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The 'captured' (emulation of capture) must be set on this port see
*       tgfTrafficGeneratorPortTxEthCaptureSet, tgfTrafficGeneratorRxInCpuGet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorTxEthTriggerCheck
(
    IN  GT_U8                           *bufferPtr,
    IN  GT_U32                           bufferLength,
    IN  GT_U32                           numVfd,
    IN  TGF_VFD_INFO_STC                 vfdArray[],
    OUT GT_BOOL                         *triggeredPtr
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       mode             - packet capture mode
*       start            - start/stop capture on this port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start capture is emulated by setting next for the port:
*           1. loopback on the port (so all traffic that egress the port will
*              ingress the same way)
*           2. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop capture is emulated by setting next for the port:
*           1. disable loopback on the port
*           2. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthCaptureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN TGF_CAPTURE_MODE_ENT      mode,
    IN GT_BOOL                   start
);

/*******************************************************************************
* tgfTrafficGeneratorCaptureLoopTimeSet
*
* DESCRIPTION:
*       Set sleeping time before disabling packet's capturing
*
* INPUTS:
*       timeInMSec - time in miliseconds (0 for restore defaults)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCaptureLoopTimeSet
(
    IN GT_U32   timeInMSec
);

/*******************************************************************************
* tgfTrafficGeneratorEtherTypeForVlanTagSet
*
* DESCRIPTION:
*       set etherType to recognize 'Tagged packes' that send from the traffic
*       generator to the PP
*
* INPUTS:
*       etherType - new etherType
*                   (was initialized as 0x8100)
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
GT_VOID tgfTrafficGeneratorEtherTypeForVlanTagSet
(
    IN GT_U16   etherType
);

/*******************************************************************************
* tgfTrafficGeneratorLoopTimeSet
*
* DESCRIPTION:
*       Set sleeping time after sending burst of packets from the CPU
*
* INPUTS:
*       timeInSec - time in seconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorLoopTimeSet
(
    IN GT_U32   timeInSec
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfTrafficGeneratorh */

