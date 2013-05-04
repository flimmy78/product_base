/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrafficEngine.h
*
* DESCRIPTION:
*    -- packet generator , parser.
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
*    -- each passenger is like a new packet
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
#ifndef __tgfTrafficEngineh
#define __tgfTrafficEngineh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************\
 *                              Define section                                *
\******************************************************************************/

/* 4 bytes for CRC*/
#define TGF_CRC_LEN_CNS                 4

/* ether type used for "vlan tag" */
#define TGF_ETHERTYPE_8100_VLAN_TAG_CNS 0x8100

/* ether type example that can be used for "double vlan tag" */
#define TGF_ETHERTYPE_9100_TAG_CNS      0x9100

/* ether type used for MPLS */
#define TGF_ETHERTYPE_8847_MPLS_TAG_CNS 0x8847

/* ether type used for IPv4 */
#define TGF_ETHERTYPE_0800_IPV4_TAG_CNS 0x0800

/* ether type used for IPv6 */
#define TGF_ETHERTYPE_86DD_IPV6_TAG_CNS 0x86DD

/* ether type used for ARP */
#define TGF_ETHERTYPE_0806_ARP_TAG_CNS  0x0806

/* ether type used for MIM */
#define TGF_ETHERTYPE_88E7_MIM_TAG_CNS  0x88E7

/* ether type used for 'Service provider vlan tag' */
#define TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS  0x88A8

/* ether type that not used by any protocol   */
#define TGF_ETHERTYPE_NON_VALID_TAG_CNS  0xFFFF


/* L2 header size (in bytes) */
#define TGF_L2_HEADER_SIZE_CNS          12

/* VLAN Tag size (in bytes) */
#define TGF_VLAN_TAG_SIZE_CNS           4

/* ETHERTYPE size (in bytes) */
#define TGF_ETHERTYPE_SIZE_CNS          2

/* MPLS header size (in bytes) */
#define TGF_MPLS_HEADER_SIZE_CNS        4

/* IPv4 header size (in bytes) */
#define TGF_IPV4_HEADER_SIZE_CNS        20

/* IPv6 header size (in bytes) */
#define TGF_IPV6_HEADER_SIZE_CNS        40

/* TCP header size (in bytes) */
#define TGF_TCP_HEADER_SIZE_CNS         20

/* UDP header size (in bytes) */
#define TGF_UDP_HEADER_SIZE_CNS         8

/* ARP header size (in bytes) */
#define TGF_ARP_HEADER_SIZE_CNS         28

/* ARPv6 header size (in bytes) */
#define TGF_ARPV6_HEADER_SIZE_CNS       52

/* ICMP header size (in bytes) */
#define TGF_ICMP_HEADER_SIZE_CNS        8

/* number of bytes in the pattern of VFD */
#define TGF_VFD_PATTERN_BYTES_NUM_CNS   16

/* set compiler to avoid non used parameter of the function */
#define TGF_PARAM_NOT_USED(x)   (GT_VOID) x

/* reset parameter according to it's size */
#define TGF_RESET_PARAM_MAC(param) cpssOsMemSet(&(param), 0, sizeof(param))

/* check GT_STATUS rc parameter */
#define TGF_RC_CHECK_MAC(rc) \
    if (GT_OK != rc)         \
    {                        \
        return rc;           \
    }


/******************************************************************************\
 *                              Common types section                          *
\******************************************************************************/

/* define common types */
typedef GT_U8   TGF_MAC_ADDR[6];
typedef GT_U8   TGF_IPV4_ADDR[4];
typedef GT_U16  TGF_IPV6_ADDR[8];
typedef GT_U16  TGF_VLAN_ID;
typedef GT_U8   TGF_CFI;
typedef GT_U8   TGF_PRI;
typedef GT_U16  TGF_L4_PORT;
typedef GT_U16  TGF_ETHER_TYPE;
typedef GT_U8   TGF_PROT_VER;
typedef GT_U8   TGF_TYPE_OF_SERVICE;
typedef GT_U16  TGF_IPV4_ID_FIELD;
typedef GT_U16  TGF_ICMP_ID_FIELD;
typedef GT_U8   TGF_TCP_FLAGS;
typedef GT_U8   TGF_TTL;
typedef GT_U8   TGF_PROT;
typedef GT_U16  TGF_HEADER_CRC;
typedef GT_U8   TGF_TRAFFIC_CLASS;
typedef GT_U32  TGF_FLOW_LABEL;
typedef GT_U8   TGF_HEXT_HEADER;
typedef GT_U8   TGF_HOP_LIMIT;
typedef GT_U16  TGF_TCP_URGENT_PTR;
typedef GT_U16  TGF_ARP_HW_TYPE;
typedef GT_U16  TGF_PROT_TYPE;
typedef GT_U8   TGF_MPLS_STACK;
typedef GT_U8   TGF_MPLS_EXP;
typedef GT_U32  TGF_MPLS_LBL;
typedef GT_U8   TGF_FLAG;
typedef GT_U16  TGF_NEXT_HEADER;


/******************************************************************************\
 *                          Enumerations definitions                          *
\******************************************************************************/

/* enumeration that hold the types of VFD modes */
typedef enum
{
    TGF_VFD_MODE_OFF_E,
    TGF_VFD_MODE_STATIC_E,
    TGF_VFD_MODE_INCREMENT_E,
    TGF_VFD_MODE_DECREMENT_E,
    TGF_VFD_MODE_RANDOM_E,

    TGF_VFD_MODE_ARRAY_PATTERN_E,  /* mode to transmit array of patterns */
    
    TGF_VFD_MODE_INCREMENT_VALUE_E,/* increment by value --> see parameter incValue */
    TGF_VFD_MODE_DECREMENT_VALUE_E /* decrement by value --> see parameter incValue */

} TGF_VFD_MODE_ENT;

/* enumeration that holds the types of the packet's parts */
typedef enum
{
    TGF_PACKET_PART_L2_E,
    TGF_PACKET_PART_VLAN_TAG_E,
    TGF_PACKET_PART_ETHERTYPE_E,
    TGF_PACKET_PART_MPLS_E,
    TGF_PACKET_PART_IPV4_E,
    TGF_PACKET_PART_IPV6_E,
    TGF_PACKET_PART_TCP_E,
    TGF_PACKET_PART_UDP_E,
    TGF_PACKET_PART_ARP_E,
    TGF_PACKET_PART_ICMP_E,
    TGF_PACKET_PART_WILDCARD_E,
    TGF_PACKET_PART_PAYLOAD_E,

    TGF_PACKET_PART_MAX_E
} TGF_PACKET_PART_ENT;


/*
 * Protocol Numbers
 *
 * Note: In the Internet Protocol version 4 (IPv4) [RFC791] there is a field
 * called "Protocol" to identify the next level protocol.  This is an 8
 * bit field.  In Internet Protocol version 6 (IPv6) [RFC1883], this field
 * is called the "Next Header" field.
 *
 * Decimal  Keyword          Protocol                                 References
 * -------  ---------------  ---------------------------------------  ----------
 * 0        HOPOPT           IPv6 Hop-by-Hop Option                   [RFC1883]
 * 1        ICMP             Internet Control Message                 [RFC792]
 * 2        IGMP             Internet Group Management                [RFC1112]
 * 4        IP               IP in IP (encapsulation)                 [RFC2003]
 * 6        TCP              Transmission Control                     [RFC793]
 * 17       UDP              User Datagram                            [RFC768][JBP]
 * 41       IPv6             Ipv6                                     [Deering]
 * 43       IPv6-Route       Routing Header for IPv6                  [Deering]
 * 44       IPv6-Frag        Fragment Header for IPv6                 [Deering]
 * 58       IPv6-ICMP        ICMP for IPv6                            [RFC1883]
 * 59       IPv6-NoNxt       No Next Header for IPv6                  [RFC1883]
 * 60       IPv6-Opts        Destination Options for IPv6             [RFC1883]
 * 140-252  Unassigned                                                [IANA]
 * 253      Use for experimentation and testing                       [RFC3692]
 * 254      Use for experimentation and testing                       [RFC3692]
 * 255      Reserved                                                  [IANA]
 */
typedef enum
{
    TGF_PROTOCOL_HOPOPT_E =  0,
    TGF_PROTOCOL_ICMP_E   =  1,
    TGF_PROTOCOL_IGMP_E   =  2,
    TGF_PROTOCOL_IP_E     =  4,
    TGF_PROTOCOL_TCP_E    =  6,
    TGF_PROTOCOL_UDP_E    = 17, /* 0x11 */
    TGF_PROTOCOL_ICMPV6_E = 58, /* 0x3A */

    TGF_PROTOCOL_MAX_E = 255
} TGF_PROTOCOL_ENT;

/*
 * ICMP Message Types
 *
 *   0  Echo Reply Message
 *   3  Destination Unreachable Message
 *   4  Source Quench Message
 *   5  Redirect Message
 *   8  Echo Message
 *   11  Time Exceeded Message
 *   12  Parameter Problem Message
 *   13  Timestamp Message
 *   14  Timestamp Reply Message
 *   15  Information Request Message
 *   16  Information Reply Message
 */
typedef enum
{
    TGF_ICMP_TYPE_ECHO_REPLY_E              =  0,
    TGF_ICMP_TYPE_DESTINATION_UNREACHABLE_E =  3,
    TGF_ICMP_TYPE_SOURCE_QUENCH_E           =  4,
    TGF_ICMP_TYPE_REDIRECT_E                =  5,
    TGF_ICMP_TYPE_ECHO_E                    =  8,
    TGF_ICMP_TYPE_TIME_EXCEEDED_E           = 11,
    TGF_ICMP_TYPE_PARAMETER_PROBLEM_E       = 12,
    TGF_ICMP_TYPE_TIMESTAMP_E               = 13,
    TGF_ICMP_TYPE_TIMESTAMP_REPLY_E         = 14,
    TGF_ICMP_TYPE_INFORMATION_E             = 15,
    TGF_ICMP_TYPE_INFORMATION_REPLY_E       = 16,

    TGF_ICMP_TYPE_MAX_E = 255
} TGF_ICMP_TYPE_ENT;

/******************************************************************************\
 *                               Struct definitions                           *
\******************************************************************************/

/* struct for L2 part (mac addresses) */
typedef struct
{
    TGF_MAC_ADDR        daMac;
    TGF_MAC_ADDR        saMac;
} TGF_PACKET_L2_STC;

/* struct for vlan tag part  */
typedef struct
{
    TGF_ETHER_TYPE      etherType;
    TGF_PRI             pri;
    TGF_CFI             cfi;
    TGF_VLAN_ID         vid;
} TGF_PACKET_VLAN_TAG_STC;

/* struct for ethertype part  */
typedef struct
{
    TGF_ETHER_TYPE      etherType;
} TGF_PACKET_ETHERTYPE_STC;

/* MPLS part of single frame */
typedef struct
{
    TGF_MPLS_LBL        label;
    TGF_MPLS_EXP        exp;
    TGF_MPLS_STACK      stack;
    TGF_TTL             timeToLive;
} TGF_PACKET_MPLS_STC;

/* auto calc checksum fields */
#define TGF_PACKET_AUTO_CALC_CHECKSUM_CNS       0xEEEE
/* auto calc length field */
#define TGF_PACKET_AUTO_CALC_LENGTH_CNS         0xFFFF

/* IPv4 part of single frame */
typedef struct
{
    TGF_PROT_VER        version;
    GT_U8               headerLen;
    TGF_TYPE_OF_SERVICE typeOfService;
    GT_U16              totalLen;
    TGF_IPV4_ID_FIELD   id;
    TGF_FLAG            flags;
    GT_U16              offset;
    TGF_TTL             timeToLive;
    TGF_PROT            protocol;
    TGF_HEADER_CRC      csum;     /* can use TGF_PACKET_AUTO_CALC_CHECKSUM_CNS for auto checksum */
    TGF_IPV4_ADDR       srcAddr;
    TGF_IPV4_ADDR       dstAddr;
} TGF_PACKET_IPV4_STC;

/* IPv6 part of single frame */
typedef struct
{
    TGF_PROT_VER        version;
    TGF_TRAFFIC_CLASS   trafficClass;
    TGF_FLOW_LABEL      flowLabel;
    GT_U16              payloadLen;
    TGF_NEXT_HEADER     nextHeader;
    TGF_HOP_LIMIT       hopLimit;
    TGF_IPV6_ADDR       srcAddr;
    TGF_IPV6_ADDR       dstAddr;
} TGF_PACKET_IPV6_STC;

/* TCP part of single frame */
typedef struct
{
    TGF_L4_PORT         srcPort;
    TGF_L4_PORT         dstPort;
    GT_U32              sequenceNum;
    GT_U32              acknowledgeNum;
    GT_U8               dataOffset;
    GT_U8               reserved;
    TGF_TCP_FLAGS       flags;
    GT_U16              windowSize;
    TGF_HEADER_CRC      csum;/* can use TGF_PACKET_AUTO_CALC_CHECKSUM_CNS for auto checksum */
    TGF_TCP_URGENT_PTR  urgentPtr;
} TGF_PACKET_TCP_STC;

/* UDP part of single frame */
typedef struct
{
    TGF_L4_PORT         srcPort;
    TGF_L4_PORT         dstPort;
    GT_U16              length;  /* can use TGF_PACKET_AUTO_CALC_LENGTH_CNS to build from ipv4/6 info */
    TGF_HEADER_CRC      csum;/* can use TGF_PACKET_AUTO_CALC_CHECKSUM_CNS for auto checksum */
} TGF_PACKET_UDP_STC;

/* ARP part of single frame */
typedef struct
{
    TGF_ARP_HW_TYPE     hwType;
    TGF_PROT_TYPE       protType;
    GT_U8               hwLen;
    GT_U8               protLen;
    GT_U16              opCode;
    TGF_MAC_ADDR        srcMac;
    TGF_IPV4_ADDR       srcIp;
    TGF_MAC_ADDR        dstMac;
    TGF_IPV4_ADDR       dstIp;
} TGF_PACKET_ARP_STC;

/* ARPv6 part of single frame */
typedef struct
{
    TGF_ARP_HW_TYPE     hwType;
    TGF_PROT_TYPE       protType;
    GT_U8               hwLen;
    GT_U8               protLen;
    GT_U16              opCode;
    TGF_MAC_ADDR        srcMac;
    TGF_IPV6_ADDR       srcIp;
    TGF_MAC_ADDR        dstMac;
    TGF_IPV6_ADDR       dstIp;
} TGF_PACKET_ARPV6_STC;

/* ICMP part of single frame */
typedef struct
{
    TGF_TYPE_OF_SERVICE typeOfService;
    GT_U8               code;
    TGF_HEADER_CRC      csum;
    TGF_ICMP_ID_FIELD   id;
    GT_U16              sequenceNum;
    GT_U32              getwayAddr;
    GT_U8               errorPointer;
} TGF_PACKET_ICMP_STC;

/* struct for any kind of wildcard/'Unknown format' */
typedef struct
{
    GT_U32              numOfBytes;
    GT_U8              *bytesPtr;
} TGF_PACKET_WILDCARD_STC;

/* struct for payload */
typedef struct
{
    GT_U32              dataLength;
    GT_U8              *dataPtr;
} TGF_PACKET_PAYLOAD_STC;

/* struct that holds info about the part */
typedef struct
{
    TGF_PACKET_PART_ENT type;
    GT_VOID            *partPtr;
} TGF_PACKET_PART_STC;

/* struct hold the packet info */
typedef struct
{
    GT_U32               totalLen;   /* not include CRC */
    GT_U32               numOfParts; /* num of elements in of partsArray */
    TGF_PACKET_PART_STC *partsArray;
} TGF_PACKET_STC;

/* struct that holds the VFD info */
typedef struct
{
    TGF_VFD_MODE_ENT    mode;
    GT_U32              modeExtraInfo; /* info relate to mode :
                                                for increment,decrement  it is the current step (0,1,2...)
                                                and for static (to optimize copies)
                                       */
    GT_U32              incValue; /* info relate to mode :
                                                for increment_value,decrement_value  it is the value for inc/dec
                                  */
    GT_U32              offset; /* offset in the packet to override */
    GT_U8               patternPtr[TGF_VFD_PATTERN_BYTES_NUM_CNS];/* the pattern for the packet in the needed offset */
    GT_U32              cycleCount;/* number of bytes in patternPtr */

    GT_U8*              arrayPatternPtr; /* buffer of arrays in bytes for each burst */
    GT_U32              arrayPatternOffset; /* offset in patternt buffer -- start number of array in pattern */
    GT_U32              arrayPatternSize; /* size (in bytes) of pattern for single burst */
} TGF_VFD_INFO_STC;

/* enum of checksum fields that may be in packet */
typedef enum{
    TGF_AUTO_CHECKSUM_FIELD_IPV4_E,
    TGF_AUTO_CHECKSUM_FIELD_TCP_E,
    TGF_AUTO_CHECKSUM_FIELD_UDP_E,


    TGF_AUTO_CHECKSUM_FIELD_LAST_E /* must be last */
}TGF_AUTO_CHECKSUM_FIELD_ENT;

/* struct for auto checksum calculations */
typedef struct {
    GT_BOOL enable;     /* do we need auto recalc */
    GT_U8*  startPtr;   /* pointer to start of packet to do checksum */
    GT_U32  numBytes;   /* number of bytes to do checksum on */
    GT_U8*  checkSumPtr;/* pointer to checksum field */
}TGF_AUTO_CHECKSUM_INFO_STC;

/* the IPv6 pseudo header need 40 bytes :
16 - sip
16 - dip
4  - Upper-Layer Packet Length
3  - zero
1  - Next Header

the Ipv4 pseudo header need 12 bytes:
4 - sip
4 - dip
1 - zero
1 - protocol
2 - TCP/UDP Length
*/
#define TGF_AUTO_CHECKSUM_PSEUDO_NUM_BYTES_CNS  40

/* struct for extra checksum info */
typedef struct{
    struct{
        GT_U8   buffer[TGF_AUTO_CHECKSUM_PSEUDO_NUM_BYTES_CNS];/* pseudo buffer length */
        GT_U32  numBytes;/* actual number of bytes used in the buffer */
    }pseudo;/*pseudo header bytes for TCP/UDP header checksum calc */
    GT_BOOL  isIpv4;/* ipv4 or ipv6*/
    GT_U8*   startL3HeadrPtr;/*pointer to start of IP header (ipv4/6)*/

    GT_U8*  udpLengthFieldPtr;/* pointer to UDP field , for auto calc from the IPv4/6 header
                                when NULL --> ignored */

}TGF_AUTO_CHECKSUM_EXTRA_INFO_STC;

/******************************************************************************\
 *                            Public API section                              *
\******************************************************************************/

/*******************************************************************************
* tgfTrafficEnginePacketBuild
*
* DESCRIPTION:
*       Build packet from input fields
*
* INPUTS:
*       packetInfoPtr - packet fields info
*
* OUTPUTS:
*       bufferPtr - (pointer to) the buffer that hold the packet bytes
*       checksumInfoArr - (array of) info about fields that need auto checksum build
*       checksumExtraInfoPtr -  (pointer to) single extra info for auto checksum build
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*       Buffer must be allocated for CRC (4 bytes)
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketBuild
(
    IN  TGF_PACKET_STC    *packetInfoPtr,
    OUT GT_U8             *bufferPtr,
    OUT TGF_AUTO_CHECKSUM_INFO_STC *checksumInfoPtr,
    OUT TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
);

/*******************************************************************************
* tgfTrafficEnginePacketParse
*
* DESCRIPTION:
*       Parse packet from bytes to parts and fields
*
* INPUTS:
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       bufferLength - length of the buffer
*
* OUTPUTS:
*       packetInfoPtrPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*       The packetInfoPtrPtr is pointer to static memory that is 'reused' for
*       every new packet that need 'parse' , so use this packet info before
*       calling to new 'parse'
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketParse
(
    IN  GT_U8              *bufferPtr,
    IN  GT_U32              bufferLength,
    OUT TGF_PACKET_STC    **packetInfoPtrPtr
);

/*******************************************************************************
* tgfTrafficEnginePacketVfdApply
*
* DESCRIPTION:
*       Apply VFD info on the buffer
*
* INPUTS:
*       vfdPtr       - (pointer to) VFD info
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*                       before modification
*       bufferLength - the number of bytes in the buffer (not include CRC bytes)
* OUTPUTS:
*       bufferPtr - (pointer to) the buffer that hold the packet bytes
*                    after modification
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketVfdApply
(
    IN    TGF_VFD_INFO_STC       *vfdPtr,
    INOUT GT_U8                  *bufferPtr,
    IN    GT_U32                  bufferLength
);

/*******************************************************************************
* tgfTrafficEnginePacketVfdCheck
*
* DESCRIPTION:
*       Check if the VFD parameters match the buffer
*
* INPUTS:
*       vfdPtr       - (pointer to) VFD info
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       bufferLength - the number of bytes in the buffer (not include CRC bytes)
* OUTPUTS:
*       matchPtr - (pointer to) do we have a match
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       vfdPtr->mode should be TGF_VFD_MODE_STATIC_E
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketVfdCheck
(
    IN  TGF_VFD_INFO_STC       *vfdPtr,
    IN  GT_U8                  *bufferPtr,
    IN  GT_U32                  bufferLength,
    OUT GT_BOOL                *matchPtr
);

/*******************************************************************************
* tgfTrafficEnginePacketCheckSum16BitsCalc
*
* DESCRIPTION:
*       Calculate checksum of 16 bits
*
* INPUTS:
*       bytesPtr - (pointer) to start of section in packet need to be calculated
*       numBytes - number of bytes need to be included in the calculation
*       pseudoBytesPtr - (pointer to ) start of pseudo info
*       pseudoNumBytes - number of pseudo bytes
* OUTPUTS:
*       checkSumArray - array of 2 bytes with the calculated check sum
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       If there's a field CHECKSUM within the input-buffer
*       it supposed to be zero before calling this function.
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketCheckSum16BitsCalc
(
    IN  GT_U8              *bytesPtr,
    IN  GT_U32              numBytes,
    IN  GT_U8              *pseudoBytesPtr,
    IN  GT_U32              pseudoNumBytes,
    OUT GT_U8               checkSumArray[2]
);

/*******************************************************************************
* tgfTrafficTracePacketBytes
*
* DESCRIPTION:
*       Trace the packet bytes
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       length        - length of packet
*       isCrcIncluded - enable\disable printout CRC in log
* OUTPUTS:
*
* RETURNS:
*       GT_OK      - on success.
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       Before use this fucntion set 'GT_TRUE' in tgfTrafficTracePacketByteSet
*
*******************************************************************************/
GT_STATUS tgfTrafficTracePacket
(
    IN GT_U8           *bufferPtr,
    IN GT_U32           length,
    IN GT_BOOL          isCrcIncluded
);

/*******************************************************************************
* tgfTrafficTraceFieldInPartInPacketSet
*
* DESCRIPTION:
*       Open/Close tracing for a field
*
* INPUTS:
*       packetPartType - the part that represent the 'context' of field
*       trace          - trace enable/disable
*       fieldOffset    - offset in bytes of field from start of structure that
*                        represent the packetPartType
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong packetPartType parameter.
*       GT_OUT_OF_RANGE - on out of range fieldNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTraceFieldInPartInPacketSet
(
    IN TGF_PACKET_PART_ENT   packetPartType,
    IN GT_BOOL               trace,
    IN GT_U32                fieldOffset
);

/*******************************************************************************
* tgfTrafficTraceAllFieldsInPartSet
*
* DESCRIPTION:
*       Open/Close the tracing for all fields in specific packet
*
* INPUTS:
*       packetPartType - the part that represent the 'context' of field
*       trace          - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTraceAllFieldsInPartSet
(
    IN TGF_PACKET_PART_ENT   packetPartType,
    IN GT_BOOL               trace
);

/*******************************************************************************
* tgfTrafficTraceAllFieldsInPartsInPacketSet
*
* DESCRIPTION:
*       Open/Close the tracing for all fields in packet (all parts)
*
* INPUTS:
*       trace - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTraceAllFieldsInPartsInPacketSet
(
    IN GT_BOOL              trace
);

/*******************************************************************************
* tgfTrafficTracePacketByteSet
*
* DESCRIPTION:
*       Open/Close the tracing on bytes of packets
*
* INPUTS:
*       trace - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTracePacketByteSet
(
    IN GT_BOOL              trace
);

/*******************************************************************************
* tgfTrafficTraceInit
*
* DESCRIPTION:
*       Initialize the DB of the trace utility
*
* INPUTS:
*       None
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK         - on success.
*       GT_INIT_ERROR - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTraceInit
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfTrafficEngineh */

