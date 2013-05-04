/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* trafficEngineUt.h
*
* DESCRIPTION:
*       -- packet generator , parser.
*    -- with trace capabilities
*    -- each packet can be built from :
*        -- L2 - mac da,sa
*        -- vlan tag (optional) -- single/double tag(Q-in-Q)
*        -- Ethernet encapsulation (optional) -- LLC/SNAP/etherII/Novell
*        -- ether type
*        -- L3 (optional) - ipv4/6 / mpls / arp / ..
*        -- L4 (optional) - tcp / udp / icmp / IGMPv2,3,6 / MLD / RIPv1,2 ..
*        -- payload
*        -- CRC32
*    -- support tunneling (tunnel + passenger)
*        -- each passenger is like a new packet
*    -- each packet has - total length
*    -- other specific info:
*        -- table index , num packets , waitTime
*        -- Tx parameters (CPSS_DXCH_NET_TX_PARAMS_STC):
*            - DSA tag info , txSynch , invokeTxBufferQueueEvent , txQueue ...
*        -- for Rx traffic : CPU code (for 'to_cpu')
*            -- the engine need CB function to get the CPU code of this frame.
*
*-- special CPU codes:
*    list of CPU codes that can be attached to CB for advanced purposes:
*        distinguish between the 'rx in cpu'(any CPU code) and the 'Tx port capture' (tx_analyzer cpu code)
*
*
*-- basic init test capabilities:
*    -- set port in loopback mode
*    -- set port with force link up
*-- emulate SMB port counters:
*    -- read/clear port counter : all/rx/tx all/packets/bytes
*    -- 'capture' - set port as 'Egress mirror port' / use STC Sampling to cpu
*                 - set the CPU as the 'Tx analyzer port'
*                 - CPU will get those frames with specific CPU code.
*
*-- emulate SMB send to port:
*    -- fill table of frames to 'Ingress the device' from the port
*       (port should be in: loopback mode and in link up)
*    -- send all / specific table index
*    -- done in specific task.
*        -- this also allow use to break the sending if we want from terminal
*        -- set FLAG 'Break Sending' from terminal so task will stop sending
*
*
*
*-- support for multi devices types (Dx/ExMxPm/ExMx):
*    -- the engine of build/parse should not care about it.
*    -- the send/receive of packet (a 'table')in CPU will call CB with specific
*        Tx parameters (CPSS_DXCH_NET_TX_PARAMS_STC) /
*        Rx parameter (CPSS_DXCH_NET_RX_PARAMS_STC)
*        and attach those parameter to the frame -->
*            each entry in table point to ' cookie + CB' that hold the specific Rx/Tx info
*
*
*    -- SMP port emulation:
*        -- read/clear counters - attach CB for read counters (per device , per port)
*        -- 'capture' - attach CB for set CPU as analyzer , CB for set port as tx mirrored port
*            CB for set Tx port with Sampling to CPU
*
*
*
*-- NOTE : all traffic that received in the CPU saved in the same 'table'
*          'test' can distinguish between : rx and ' capture of tx' by the
*          mechanism of : 'Special CPU codes'
*
*   -SMB trigger emulation - TBD
*   -Expected results  - TBD
*   -Predefined traffic flows  - TBD
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __trafficEngineUth
#define __trafficEngineUth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* define common types */
typedef GT_U8   UT_MAC_ADDR[6];
typedef GT_U16  UT_VLAN_ID;
typedef GT_U8   UT_CFI;
typedef GT_U8   UT_VPT;
typedef GT_U16  UT_ETHER_TYPE;

/* struct for L2 part (mac addresses) */
typedef struct{
    UT_MAC_ADDR     daMac;
    UT_MAC_ADDR     saMac;
}UT_TRAFFIC_ENGINE_PACKET_L2_STC;

/* struct for ethertype part  */
typedef struct{
    UT_ETHER_TYPE   etherType;
}UT_TRAFFIC_ENGINE_PACKET_ETHERTYPE_STC;

/* struct for vlan tag part  */
typedef struct{
    UT_ETHER_TYPE   etherType;
    UT_VPT          vpt;
    UT_CFI          cfi;
    UT_VLAN_ID      vid;
}UT_TRAFFIC_ENGINE_PACKET_VLAN_TAG_STC;

/* enum used in function that convert data from UT<-->cpss formats*/
typedef enum{
    UT_CONVERT_DIRECTION_CPSS_TO_UT_E,
    UT_CONVERT_DIRECTION_UT_TO_CPSS_E
}UT_CONVERT_DIRECTION_ENT;


/* 4 bytes for CRC*/
#define UT_CRC_LEN_CNS  4

/* ether type used for "vlan tag" */
#define UT_ETHERTYPE_8100_TAG_CNS   0x8100
/* ether type example that can be used for "double vlan tag" */
#define UT_ETHERTYPE_9100_TAG_CNS   0x9100
/* ether type used for IPv4 */
#define UT_ETHERTYPE_0800_IPV4_TAG_CNS   0x0800

/* struct for any kind of wildcard/'Unknown format' */
typedef struct{
    GT_U32      numOfBytes;
    GT_U8       *bytesPtr;
}UT_TRAFFIC_ENGINE_PACKET_WILDCARD_STC;

/* struct for payload */
typedef UT_TRAFFIC_ENGINE_PACKET_WILDCARD_STC UT_TRAFFIC_ENGINE_PACKET_PAYLOAD_STC;

/* types of the parts */
typedef enum{
    UT_PACKET_PART_L2_E,
    UT_PACKET_PART_VLAN_TAG_E,
    UT_PACKET_PART_ETHERTYPE_E,
    UT_PACKET_PART_WILDCARD_E,
    UT_PACKET_PART_PAYLOAD_E,

    UT_PACKET_PART_MAX_E/* mast be last */
}UT_PACKET_PART_ENT;

/* struct hole info about the part */
typedef struct{
    UT_PACKET_PART_ENT   type;
    void                 *partPtr;/* point to one of the : UT_PACKET_PART_ENT formats */
}UT_PACKET_PART_STC;

/* struct hold the packet info */
typedef struct{
    GT_U32                  totalLen;/* not include CRC */
    GT_U32                  numOfParts;  /*number of elements in array of partsArray */
    UT_PACKET_PART_STC      *partsArray;/* (pointer to array of)
                                            parts must be given in actual order
                                            in frame
                                            this way we can support :
                                            1. tunneling (ipv4-over-ipv4)
                                            2. multi vlan tag
                                            ...
                                        */
}UT_TRAFFIC_ENGINE_PACKET_STC;

/* number of bytes in the pattern of VFD */
#define UT_TRAFFIC_ENGINE_VFD_PATTERN_BYTES_NUM_CNS     6

/* enumeration that hold the types of VFD modes */
typedef enum{
    UT_TRAFFIC_ENGINE_VFD_MODE_OFF_E,
    UT_TRAFFIC_ENGINE_VFD_MODE_RANDOM_E,    /* the pattern will be random */
    UT_TRAFFIC_ENGINE_VFD_MODE_INCREMENT_E, /* the pattern will be incremented by 1 every packet */
    UT_TRAFFIC_ENGINE_VFD_MODE_DECREMENT_E, /* the pattern will be decremented by 1 every packet */
    UT_TRAFFIC_ENGINE_VFD_MODE_STATIC_E     /* the pattern the same and will not be changed */
}UT_TRAFFIC_ENGINE_VFD_MODE_ENT;


/* struct hold the VFD info --
    override of the background of packet in specific  offset
*/
typedef struct{
    UT_TRAFFIC_ENGINE_VFD_MODE_ENT mode;
    GT_U32                         modeExtraInfo;/* info relate to mode :
                                                for increment,decrement  it is the current step (0,1,2...)
                                                and for static (to optimize copies)
                                                */
    GT_U32                         offset; /* offset in the packet to override */
    GT_U8                          patternPtr[UT_TRAFFIC_ENGINE_VFD_PATTERN_BYTES_NUM_CNS];/* the pattern for the packet in the needed offset */
    GT_U32                         cycleCount;/* number of bytes in patternPtr */
}UT_TRAFFIC_ENGINE_VFD_STC;




/*******************************************************************************
* utTrafficEnginePacketBuild
*
* DESCRIPTION:
*       build packet from input fields
*
* INPUTS:
*       packetInfoPtr   - packet fields info
*
* OUTPUTS:
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_BAD_STATE                - parser caused bad state that should not
*                                     happen
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketBuild(
    IN  UT_TRAFFIC_ENGINE_PACKET_STC    *packetInfoPtr,
    OUT GT_U8                           *bufferPtr
);

/*******************************************************************************
* utTrafficEnginePacketParse
*
* DESCRIPTION:
*       parse packet from bytes to parts and fields
*
* INPUTS:
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*       bufferLength-length of the buffer
*
* OUTPUTS:
*       packetInfoPtrPtr   - (pointer to) pointer to packet fields info
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_BAD_STATE                - parser caused bad state that should not
*                                     happen
*
* COMMENTS:
*
*       the (*packetInfoPtrPtr) is pointer to static memory that is 'reused' for
*       every new packet that need 'parse' , so use this packet info before
*       calling to new 'parse'
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketParse(
    IN GT_U8                             *bufferPtr,
    IN GT_U32                            bufferLength,
    OUT  UT_TRAFFIC_ENGINE_PACKET_STC    **packetInfoPtrPtr
);

/*******************************************************************************
* utTrafficEnginePacketVfdApply
*
* DESCRIPTION:
*       apply VFD info on the buffer
*
* INPUTS:
*       vfdPtr - (pointer to) VFD info
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*                   before modification
*       bufferLength - the number of bytes in the buffer (not include CRC bytes)
* OUTPUTS:
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*                   after modification
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketVfdApply(
    IN    UT_TRAFFIC_ENGINE_VFD_STC       *vfdPtr,
    INOUT GT_U8                           *bufferPtr,
    IN    GT_U32                           bufferLength
);

/*******************************************************************************
* utTrafficEnginePacketVfdCheck
*
* DESCRIPTION:
*       Check if the VFD parameters match the buffer
*
* INPUTS:
*       vfdPtr - (pointer to) VFD info (must be  vfdPtr->mode ==
*                                       UT_TRAFFIC_ENGINE_VFD_MODE_STATIC_E)
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*       bufferLength - the number of bytes in the buffer (not include CRC bytes)
* OUTPUTS:
*       matchPtr    - (pointer to) do we have a match
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketVfdCheck(
    IN    UT_TRAFFIC_ENGINE_VFD_STC       *vfdPtr,
    IN    GT_U8                           *bufferPtr,
    IN    GT_U32                           bufferLength,
    OUT   GT_BOOL                          *matchPtr
);

/*******************************************************************************
* utTrafficEnginePacketCheckSum16BitsCalc
*
* DESCRIPTION:
*       calculate checksum of 16 bits
* INPUTS:
*       bytesPtr   - (pointer) to start of section in packet need to be calculated
*       numBytes   - number of bytes need to be included in the calculation
* OUTPUTS:
*       checkSumArray - array of 2 bytes with the calculated check sum
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*
* COMMENTS:
*        If there's a field CHECKSUM within the input-buffer
*           it supposed to be zero before calling this function.
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketCheckSum16BitsCalc(
    IN GT_U8                *bytesPtr,
    IN GT_U32               numBytes,
    OUT GT_U8               checkSumArray[2]
);

/*******************************************************************************
* utTrafficTracePacketBytes
*
* DESCRIPTION:
*       trace the packet bytes . will trace only if was set to 'GT_TRUE' in
*       function utTrafficTracePacketByteSet
*
* INPUTS:
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*       length      - length of packet
* OUTPUTS:
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficTracePacket(
    IN GT_U8                           *bufferPtr,
    IN GT_U32                           length
);

/*******************************************************************************
* utTrafficTraceFieldInPartInPacketSet
*
* DESCRIPTION:
*       open/close the tracing for a field
*
* INPUTS:
*       packetPartType  - the part that represent the 'context' of field
*       trace           - trace enable/disable
*       fieldOffset     - offset in bytes of field from start of structure that
*                         represent the packetPartType
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong packetPartType parameter.
*       GT_NO_SUCH                  - the field is not member of the part
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTraceFieldInPartInPacketSet
(
    IN UT_PACKET_PART_ENT   packetPartType,
    IN GT_BOOL              trace,
    IN GT_U32               fieldOffset
);

/*******************************************************************************
* utTrafficTraceAllFieldsInPartSet
*
* DESCRIPTION:
*       open/close the tracing for all fields in specific packet
*
* INPUTS:
*       packetPartType  - the part that represent the 'context' of field
*       trace           - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTraceAllFieldsInPartSet
(
    IN UT_PACKET_PART_ENT   packetPartType,
    IN GT_BOOL              trace
);

/*******************************************************************************
* utTrafficTraceAllFieldsInPartsInPacketSet
*
* DESCRIPTION:
*       open/close the tracing for all fields in packet (all parts)
*
* INPUTS:
*       trace           - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTraceAllFieldsInPartsInPacketSet
(
    IN GT_BOOL              trace
);

/*******************************************************************************
* utTrafficTracePacketByteSet
*
* DESCRIPTION:
*       open/close the tracing on bytes of packets
*
* INPUTS:
*       trace           - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTracePacketByteSet
(
    IN GT_BOOL              trace
);

/*******************************************************************************
* utTrafficTraceInit
*
* DESCRIPTION:
*       initialize the DB of the trace utility
*
* INPUTS:
*       None
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_INIT_ERROR   - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTraceInit
(
    void
);







#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __trafficEngineUth */

