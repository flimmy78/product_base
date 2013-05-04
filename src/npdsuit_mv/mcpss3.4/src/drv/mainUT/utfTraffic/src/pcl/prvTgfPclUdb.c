/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPclUdb.c
*
* DESCRIPTION:
*       User Defined Bytes tests
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfPclGen.h>
#include <common/tgfVntGen.h>
#include <common/tgfCosGen.h>
#include <pcl/prvTgfPclUdb.h>
#include <tunnel/prvTgfTunnelTermEtherType.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* port to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS   0

/* default VLAN Id */
#define PRV_TGF_VLANID_CNS          5

/* VRF Id */
#define PRV_TGF_VRFID_CNS           0x0123

/* Qos Profile */
#define PRV_TGF_QOS_PROFILE_CNS     0x33

/* user defined ethertype */
#define PRV_TGF_UDE_TAG_CNS         0x8888

/* default number of packets to send */
static GT_U32   prvTgfBurstCount = 1;

/************************* General packet's parts *****************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55},                /* daMac */
    {0x00, 0x66, 0x77, 0x88, 0x99, 0x11}                 /* saMac */
};

/* ethertype part of IPV4 packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketIpv4EtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

/* ethertype part of IPV6 packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketIpv6EtherTypePart = {TGF_ETHERTYPE_86DD_IPV6_TAG_CNS};

/* ethertype part of ETH_OTHER packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherOtherTypePart = {0x2222};

/* Data of packet */
static GT_U8 prvTgfPacketPayloadDataArr[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19
};

/* Payload part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart =
{
    sizeof(prvTgfPacketPayloadDataArr), /* dataLength */
    prvTgfPacketPayloadDataArr          /* dataPtr */
};

/* Data of packet (long version) */
static GT_U8 prvTgfPacketPayloadLongDataArr[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
};

/* Payload part (long version) */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketLongPayloadPart =
{
    sizeof(prvTgfPacketPayloadLongDataArr), /* dataLength */
    prvTgfPacketPayloadLongDataArr          /* dataPtr */
};

/******************* Packet with not matched UDBs *************************/

/* UDP packet for not matched UDBs */
static GT_U8 prvTgfPacketNotMatchPayloadDataArr[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43
};

/* Payload part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketNotMatchPayloadPart =
{
    sizeof(prvTgfPacketNotMatchPayloadDataArr), /* dataLength */
    prvTgfPacketNotMatchPayloadDataArr          /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketNotMatchPartArray[] =
{
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketNotMatchPayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET_NOT_MATCH_LEN_CNS sizeof(prvTgfPacketNotMatchPayloadDataArr)

/* Length of packet with CRC */
#define PRV_TGF_PACKET_NOT_MATCH_CRC_LEN_CNS  PRV_TGF_PACKET_NOT_MATCH_LEN_CNS + TGF_CRC_LEN_CNS

/* packet to send */
static TGF_PACKET_STC prvTgfPacketNotMatchInfo =
{
    PRV_TGF_PACKET_NOT_MATCH_LEN_CNS,                                                 /* totalLen */
    sizeof(prvTgfPacketNotMatchPartArray) / sizeof(prvTgfPacketNotMatchPartArray[0]), /* numOfParts */
    prvTgfPacketNotMatchPartArray                                                     /* partsArray */
};
/******************************************************************************/

/******************************* TCP packet **********************************/

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketTcpIpPart =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x42,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    6,                  /* protocol */
    0x4C87,             /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};

/* packet's TCP part */
static TGF_PACKET_TCP_STC prvTgfPacketTcpPart =
{
    8,                  /* src port */
    0,                  /* dst port */
    123456,             /* sequence number */
    234567,             /* acknowledgment number */
    5,                  /* data offset */
    0x50,               /* reserved */
    0x10,               /* flags */
    4096,               /* window */
    0xFAF6,             /* csum */
    0                   /* urgent pointer */
};

/* PARTS of packet TCP */
static TGF_PACKET_PART_STC prvTgfTcpPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketTcpIpPart},
    {TGF_PACKET_PART_TCP_E,       &prvTgfPacketTcpPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of TCP packet */
#define PRV_TGF_TCP_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS + TGF_TCP_HEADER_SIZE_CNS + sizeof(prvTgfPacketPayloadDataArr)

/* Length of TCP packet with CRC */
#define PRV_TGF_TCP_PACKET_CRC_LEN_CNS  PRV_TGF_TCP_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* TCP packet to send */
static TGF_PACKET_STC prvTgfTcpPacketInfo =
{
    PRV_TGF_TCP_PACKET_LEN_CNS,                                             /* totalLen */
    sizeof(prvTgfTcpPacketPartArray) / sizeof(prvTgfTcpPacketPartArray[0]), /* numOfParts */
    prvTgfTcpPacketPartArray                                                /* partsArray */
};
/******************************************************************************/

/******************************* UDP packet **********************************/

/* packet's IPv4 part */
static TGF_PACKET_IPV4_STC prvTgfPacketUdpIpPart =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x36,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    17,                 /* protocol */
    0x4C88,             /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};

/* packet's UDP part */
static TGF_PACKET_UDP_STC prvTgfPacketUdpPart =
{
    8,                  /* src port */
    0,                  /* dst port */
    0x22,               /* length */
    0xD172              /* csum */
};

/* PARTS of packet UDP */
static TGF_PACKET_PART_STC prvTgfUdpPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketUdpIpPart},
    {TGF_PACKET_PART_UDP_E,       &prvTgfPacketUdpPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of UDP packet */
#define PRV_TGF_UDP_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS + TGF_UDP_HEADER_SIZE_CNS + sizeof(prvTgfPacketPayloadDataArr)

/* Length of UDP packet with CRC */
#define PRV_TGF_UDP_PACKET_CRC_LEN_CNS  PRV_TGF_UDP_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* UDP packet to send */
static TGF_PACKET_STC prvTgfUdpPacketInfo =
{
    PRV_TGF_UDP_PACKET_LEN_CNS,                                             /* totalLen */
    sizeof(prvTgfUdpPacketPartArray) / sizeof(prvTgfUdpPacketPartArray[0]), /* numOfParts */
    prvTgfUdpPacketPartArray                                                /* partsArray */
};
/******************************************************************************/

/******************************* IPv4 packet **********************************/

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4OtherPart =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2E,               /* totalLen */
    0,                  /* id */
    4,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    4,                  /* protocol */
    0x4C99,             /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4FragmentPart =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2E,               /* totalLen */
    0,                  /* id */
    1,                  /* flags */    /* more fragments     */
    0x200,              /* offset */   /* not first fragment */
    0x40,               /* timeToLive */
    4,                  /* protocol */
    0x4C99,             /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};

/* PARTS of packet IPv4 - IPV4 OTHER */
static TGF_PACKET_PART_STC prvTgfIpv4OtherPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4OtherPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* PARTS of packet IPv4 - IPV4 FRAGMENT */
static TGF_PACKET_PART_STC prvTgfIpv4FragmentPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4FragmentPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of IPv4 packet */
#define PRV_TGF_IPV4_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS + sizeof(prvTgfPacketPayloadDataArr)

/* Length of IPv4 packet with CRC */
#define PRV_TGF_IPV4_PACKET_CRC_LEN_CNS  PRV_TGF_IPV4_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* IPv4 Other packet to send */
static TGF_PACKET_STC prvTgfIpv4OtherPacketInfo =
{
    PRV_TGF_IPV4_PACKET_LEN_CNS,                      /* totalLen */
    (sizeof(prvTgfIpv4OtherPacketPartArray)
        / sizeof(prvTgfIpv4OtherPacketPartArray[0])), /* numOfParts */
    prvTgfIpv4OtherPacketPartArray                    /* partsArray */
};

/* IPv4 Fragment packet to send */
static TGF_PACKET_STC prvTgfIpv4FragmentPacketInfo =
{
    PRV_TGF_IPV4_PACKET_LEN_CNS,                         /* totalLen */
    (sizeof(prvTgfIpv4FragmentPacketPartArray)
        / sizeof(prvTgfIpv4FragmentPacketPartArray[0])), /* numOfParts */
    prvTgfIpv4FragmentPacketPartArray                    /* partsArray */
};

/******************************************************************************/

/***************************** Ethernet packet ********************************/

/* PARTS of packet IPv4 */
static TGF_PACKET_PART_STC prvTgfEthernetPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherOtherTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of Ethernet packet */
#define PRV_TGF_ETHERNET_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + sizeof(prvTgfPacketPayloadLongDataArr)

/* Length of Ethernet packet with CRC */
#define PRV_TGF_ETHERNET_PACKET_CRC_LEN_CNS  \
    PRV_TGF_ETHERNET_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* Ethernet packet to send */
static TGF_PACKET_STC prvTgfEthernetPacketInfo =
{
    PRV_TGF_ETHERNET_PACKET_LEN_CNS,                                                  /* totalLen */
    (sizeof(prvTgfEthernetPacketPartArray)
        / sizeof(prvTgfEthernetPacketPartArray[0])), /* numOfParts */
    prvTgfEthernetPacketPartArray                                                     /* partsArray */
};
/******************************************************************************/

/******************************* MPLS packet **********************************/

/* ethertype part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketMplsEtherTypePart = {TGF_ETHERTYPE_8847_MPLS_TAG_CNS};

/* packet's MPLS */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsPart =
{
    1,                  /* label */
    0,                  /* experimental use */
    1,                  /* stack */
    0x40                /* timeToLive */
};

/* PARTS of packet MPLS */
static TGF_PACKET_PART_STC prvTgfMplsPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketMplsEtherTypePart},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of MPLS packet */
#define PRV_TGF_MPLS_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + TGF_MPLS_HEADER_SIZE_CNS + sizeof(prvTgfPacketPayloadLongDataArr)

/* Length of MPLS packet with CRC */
#define PRV_TGF_MPLS_PACKET_CRC_LEN_CNS  \
    PRV_TGF_MPLS_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* MPLS packet to send */
static TGF_PACKET_STC prvTgfMplsPacketInfo =
{
    PRV_TGF_MPLS_PACKET_LEN_CNS,                 /* totalLen */
    (sizeof(prvTgfMplsPacketPartArray)
        / sizeof(prvTgfMplsPacketPartArray[0])), /* numOfParts */
    prvTgfMplsPacketPartArray                    /* partsArray */
};
/******************************************************************************/

/******************************* UDE packet ***********************************/

/* ethertype part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketUsedDefinedEtherTypePart = {PRV_TGF_UDE_TAG_CNS};

/* PARTS of packet IPv4 */
static TGF_PACKET_PART_STC prvTgfUdePacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketUsedDefinedEtherTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of UDE packet */
#define PRV_TGF_UDE_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + sizeof(prvTgfPacketPayloadLongDataArr)

/* Length of UDE packet with CRC */
#define PRV_TGF_UDE_PACKET_CRC_LEN_CNS  PRV_TGF_UDE_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* UDE packet to send */
static TGF_PACKET_STC prvTgfUdePacketInfo =
{
    PRV_TGF_UDE_PACKET_LEN_CNS,                                             /* totalLen */
    sizeof(prvTgfUdePacketPartArray) / sizeof(prvTgfUdePacketPartArray[0]), /* numOfParts */
    prvTgfUdePacketPartArray                                                /* partsArray */
};
/******************************************************************************/

/******************************* IPv6 packet **********************************/

/* packet's IPv6 */
static TGF_PACKET_IPV6_STC prvTgfPacketIpv6Part =
{
    41,                                 /* version */
    0,                                  /* trafficClass */
    0,                                  /* flowLabel */
    0x2E,                               /* payloadLen */
    41,                                 /* nextHeader */
    0x40,                               /* hopLimit */
    {22, 22, 22, 22, 22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  1,  1,  1,  1,  3}    /* dstAddr */
};

/* PARTS of packet IPv6 */
static TGF_PACKET_PART_STC prvTgfIpv6PacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv6EtherTypePart},
    {TGF_PACKET_PART_IPV6_E,      &prvTgfPacketIpv6Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of IPv6 packet */
#define PRV_TGF_IPV6_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS + TGF_IPV6_HEADER_SIZE_CNS + sizeof(prvTgfPacketPayloadDataArr)

/* Length of IPv6 packet with CRC */
#define PRV_TGF_IPV6_PACKET_CRC_LEN_CNS  PRV_TGF_IPV6_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* IPv6 packet to send */
static TGF_PACKET_STC prvTgfIpv6PacketInfo =
{
    PRV_TGF_IPV6_PACKET_LEN_CNS,                                              /* totalLen */
    sizeof(prvTgfIpv6PacketPartArray) / sizeof(prvTgfIpv6PacketPartArray[0]), /* numOfParts */
    prvTgfIpv6PacketPartArray                                                 /* partsArray */
};
/******************************************************************************/

/***************************** IPv6 TCP packet ********************************/

/* packet's IPv6 */
static TGF_PACKET_IPV6_STC prvTgfPacketIpv6IpPart =
{
    41,                                 /* version */
    0,                                  /* trafficClass */
    0,                                  /* flowLabel */
    0x2E,                               /* payloadLen */
    41,                                 /* nextHeader */
    0x40,                               /* hopLimit */
    {22, 22, 22, 22, 22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  1,  1,  1,  1,  3}    /* dstAddr */
};

/* packet's TCP part */
static TGF_PACKET_TCP_STC prvTgfPacketIpv6TcpPart =
{
    8,                  /* src port */
    0,                  /* dst port */
    123456,             /* sequence number */
    234567,             /* acknowledgment number */
    5,                  /* data offset */
    0x50,               /* reserved */
    0x10,               /* flags */
    4096,               /* window */
    0xFAF6,             /* csum */
    0                   /* urgent pointer */
};

/* PARTS of packet IPv6 */
static TGF_PACKET_PART_STC prvTgfIpv6TcpPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv6EtherTypePart},
    {TGF_PACKET_PART_IPV6_E,      &prvTgfPacketIpv6IpPart},
    {TGF_PACKET_PART_TCP_E,       &prvTgfPacketIpv6TcpPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of IPv6 packet */
#define PRV_TGF_IPV6_TCP_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS + TGF_IPV6_HEADER_SIZE_CNS + TGF_TCP_HEADER_SIZE_CNS + sizeof(prvTgfPacketPayloadDataArr)

/* Length of IPv6 packet with CRC */
#define PRV_TGF_IPV6_TCP_PACKET_CRC_LEN_CNS  PRV_TGF_IPV6_TCP_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* IPv6 packet to send */
static TGF_PACKET_STC prvTgfIpv6TcpPacketInfo =
{
    PRV_TGF_IPV6_TCP_PACKET_LEN_CNS,                                                /* totalLen */
    sizeof(prvTgfIpv6TcpPacketPartArray) / sizeof(prvTgfIpv6TcpPacketPartArray[0]), /* numOfParts */
    prvTgfIpv6TcpPacketPartArray                                                    /* partsArray */
};
/******************************************************************************/


/******************************************************************************\
 *                            Private variables                               *
\******************************************************************************/

/* is VLAN and FDB entry already written */
static GT_BOOL  prvTgfIsVlanFdbInitialized = GT_FALSE;

/* stored default VID */
static GT_U16   prvTgfDefVlanId = 0;


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfDxChPclQosProfileAndTrustModeSet
*
* DESCRIPTION:
*       Set Qos profile Qos Trust Mode for ingess port:
*
* INPUTS:
*       qosProfile     - qos Profile
*       trustMode      - packet DSCP and UP trust mode
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
static GT_STATUS prvTgfDxChPclQosProfileAndTrustModeSet
(
    IN GT_U32                               qosProfile,
    IN CPSS_QOS_PORT_TRUST_MODE_ENT         trustMode
)
{
    GT_STATUS           rc = GT_OK;
    CPSS_QOS_ENTRY_STC  portQosCfg;

    cpssOsMemSet(&portQosCfg, 0, sizeof(CPSS_QOS_ENTRY_STC));
    portQosCfg.qosProfileId = qosProfile;
    portQosCfg.enableModifyDscp =
        CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    portQosCfg.enableModifyUp =
        CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    portQosCfg.assignPrecedence =
        CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    rc = prvTgfCosPortQosConfigSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &portQosCfg);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = prvTgfCosPortQosTrustModeSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], trustMode);

    return rc;
}

/*******************************************************************************
* prvTgfPclUdbTestInit
*
* DESCRIPTION:
*       Set initial test settings:
*           - set VLAN entry
*           - set default port VLAN ID
*           - init PCL engine
*           - configure the User Defined Bytes
*           - sets the Policy rule
*
* INPUTS:
*       ruleIndex     - index of the rule in the TCAM
*       ruleFormat    - format of the Rule
*       packetType    - packet Type
*       offsetType    - the type of offset
*       useVrId       - override VR ID
*       useQosProfile - override QoS profile
*       packetInfoPtr - (pointer to) packet fields info
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
static GT_VOID prvTgfPclUdbTestInit
(
    IN GT_U32                               ruleIndex,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT     ruleFormat,
    IN PRV_TGF_PCL_PACKET_TYPE_ENT          packetType,
    IN PRV_TGF_PCL_OFFSET_TYPE_ENT          offsetType,
    IN GT_BOOL                              useVrId,
    IN GT_BOOL                              useQosProfile,
    IN TGF_PACKET_STC                      *packetInfoPtr
)
{
    GT_STATUS                         rc     = GT_OK;
    GT_U32                            udbIdx = 0;
    GT_U32                            udbAbsIdx = 0;
    GT_U32                            offset = 0;
    PRV_TGF_PCL_RULE_FORMAT_UNT       pattern;
    PRV_TGF_PCL_RULE_FORMAT_UNT       mask;
    PRV_TGF_PCL_ACTION_STC            action;
    PRV_TGF_PCL_OVERRIDE_UDB_STC      udbOverride;
    GT_U8                             packetBuff[TGF_RX_BUFFER_MAX_SIZE_CNS];
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT  ethOthKey;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT  ipv4Key;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT  ipv6Key;
    GT_U32                            udbOffset;
    GT_U32                            ipHdrChkSumOffset;
    GT_BOOL                           isIpv4;
    GT_BOOL                           isIpv6;



    /* check if FDB and VLAN init is needed */
    if (GT_FALSE == prvTgfIsVlanFdbInitialized)
    {
        /* get default vlanId */
        rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfDefVlanId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

        prvTgfIsVlanFdbInitialized = GT_TRUE;
    }

    /* set default vlan entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d", prvTgfDevNum);

    /* set default vlanId */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d", prvTgfDevNum);

    /* default */
    ethOthKey = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    ipv4Key   = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    ipv6Key   = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    isIpv4    = GT_FALSE;
    isIpv6    = GT_FALSE;
    /* override default */
    switch (packetType)
    {
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E:
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E:
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E:
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E:
            ipv4Key = ruleFormat;
            isIpv4  = GT_TRUE;
            break ;
        case PRV_TGF_PCL_PACKET_TYPE_IPV6_E:
            ipv6Key = ruleFormat;
            isIpv6  = GT_TRUE;
            break ;
        default:
        case PRV_TGF_PCL_PACKET_TYPE_MPLS_E:
        case PRV_TGF_PCL_PACKET_TYPE_UDE_E:
        case PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E:
            ethOthKey = ruleFormat;
            break ;
    }

    /* two bytes of IPV4 Header check sum must be bypassed */
    /* the value updated at egress before loopback         */
    /* 0xFF is out of PCL range - default value            */
    offset = TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS;
    ipHdrChkSumOffset =
        (isIpv4 != GT_FALSE) ? (offset + 10) : 0xFFFFFFFF;

    /* build packet from input fields */
    rc = tgfTrafficEnginePacketBuild(packetInfoPtr, packetBuff,NULL,NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficEnginePacketBuild: %d", prvTgfDevNum);

    if (PRV_TGF_PCL_OFFSET_INVALID_E != offsetType)
    {
        /* init PCL Engine for send port */
        rc = prvTgfPclDefPortInit(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                  CPSS_PCL_LOOKUP_0_E,
                                  ethOthKey, ipv4Key, ipv6Key);
        UTF_VERIFY_EQUAL2_STRING_MAC(
            GT_OK, rc, "prvTgfPclDefPortInit: %d, %d", prvTgfDevNum,
            prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    }

    /* calc offset */
    switch (offsetType)
    {
        case PRV_TGF_PCL_OFFSET_L2_E:
            offset = 0;
            break;

        case PRV_TGF_PCL_OFFSET_L4_E:
            /* default */
            offset = TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS;
            if (isIpv4 != GT_FALSE)
            {
                offset = TGF_L2_HEADER_SIZE_CNS
                    + TGF_ETHERTYPE_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS;
            }
            if (isIpv6 != GT_FALSE)
            {
                offset = TGF_L2_HEADER_SIZE_CNS
                    + TGF_ETHERTYPE_SIZE_CNS + TGF_IPV6_HEADER_SIZE_CNS;
            }
            break;

        case PRV_TGF_PCL_OFFSET_L3_MINUS_2_E:
            /* begins just from ethertype */
            offset = TGF_L2_HEADER_SIZE_CNS;
            break;

        case PRV_TGF_PCL_OFFSET_MPLS_MINUS_2_E:
            /* begins just from ethertype */
            offset = TGF_L2_HEADER_SIZE_CNS;
            break;

        case PRV_TGF_PCL_OFFSET_INVALID_E:
            /* to invalidate UDBs */
            offset = 0;
            break;

        default:
            offset = 0;
            rc = GT_BAD_PARAM;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid offsetType: %d", offsetType);
    }

    /* clear config structures */
    cpssOsMemSet(&mask,        0, sizeof(mask));
    cpssOsMemSet(&pattern,     0, sizeof(pattern));
    cpssOsMemSet(&action,      0, sizeof(action));
    cpssOsMemSet(&udbOverride, 0, sizeof(udbOverride));

    /* Default state */

    /* set VRF Id == 0 for default VLAN  */
    rc = prvTgfBrgVlanVrfIdSet(PRV_TGF_VLANID_CNS ,0);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfBrgVlanVrfIdSet: %d", prvTgfDevNum);

    /* overriding of  User Defined Bytes */
    rc = prvTgfPclOverrideUserDefinedBytesSet(&udbOverride);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfPclOverrideUserDefinedBytesSet: %d", prvTgfDevNum);

    /* set 0 - Qos profile to SEND port */
    rc = prvTgfDxChPclQosProfileAndTrustModeSet(0, CPSS_QOS_PORT_TRUST_L2_L3_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfDxChPclQosProfileAndTrustModeSet: %d", prvTgfDevNum);


    /* configure UBDs - "break" inside */
    for (udbIdx = 0; (1); udbIdx++)
    {
        udbOffset = udbIdx;
        if  ((offset <= ipHdrChkSumOffset)
            && ((udbOffset + offset) >= ipHdrChkSumOffset))
        {
            /* bypass IPV4 header check sum */
            udbOffset += 2;
        }

        rc = prvTgfPclUdbIndexConvert(
            ruleFormat, udbIdx, &udbAbsIdx);
        if (rc != GT_OK)
        {
            /* end of UDBs of the specified rule format */
            break;
        }

        /* configures the User Defined Byte */
        rc = prvTgfPclUserDefinedByteSet(
            ruleFormat, packetType,
            (GT_U8)udbAbsIdx /*udbIndex*/,
            offsetType, (GT_U8)udbOffset/*offset*/);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfPclUserDefinedByteSet: %d", prvTgfDevNum);

        /* set pattern and mask */
        switch (ruleFormat)
        {
            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                pattern.ruleStdUdb.udb[udbIdx] = packetBuff[offset + udbOffset];
                mask.ruleStdUdb.udb[udbIdx]    = 0xFF;
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                pattern.ruleStdIpL2Qos.udb[udbIdx] = packetBuff[offset + udbOffset];
                mask.ruleStdIpL2Qos.udb[udbIdx]    = 0xFF;
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                pattern.ruleStdIpv4L4.udb[udbIdx] = packetBuff[offset + udbOffset];
                mask.ruleStdIpv4L4.udb[udbIdx] = 0xFF;
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                pattern.ruleStdNotIp.udb[udbIdx] = packetBuff[offset + udbOffset];
                mask.ruleStdNotIp.udb[udbIdx] = 0xFF;
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                pattern.ruleExtNotIpv6.udb[udbIdx] = packetBuff[offset + udbOffset];
                mask.ruleExtNotIpv6.udb[udbIdx] = 0xFF;
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                pattern.ruleExtUdb.udb[udbIdx] = packetBuff[offset + udbOffset];
                mask.ruleExtUdb.udb[udbIdx] = 0xFF;
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                pattern.ruleExtIpv6L2.udb[udbIdx] = packetBuff[offset + udbOffset];
                mask.ruleExtIpv6L2.udb[udbIdx] = 0xFF;
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                pattern.ruleExtIpv6L4.udb[udbIdx] = packetBuff[offset + udbOffset];
                mask.ruleExtIpv6L4.udb[udbIdx] = 0xFF;
                break;

            default:
                rc = GT_BAD_PARAM;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid ruleFormat: %d", ruleFormat);
        }
    }

    /* overriding of  User Defined Bytes if needed */
    if (GT_TRUE == useVrId || GT_TRUE == useQosProfile)
    {
        if (GT_TRUE == useVrId)
        {
            /* configure UDB override structure */
            switch (ruleFormat)
            {
                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                    udbOverride.vrfIdLsbEnableStdNotIp = GT_TRUE;
                    udbOverride.vrfIdMsbEnableStdNotIp = GT_TRUE;
                    break;

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                    udbOverride.vrfIdLsbEnableStdIpL2Qos = GT_TRUE;
                    udbOverride.vrfIdMsbEnableStdIpL2Qos = GT_TRUE;
                    break;

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                    udbOverride.vrfIdLsbEnableStdIpv4L4 = GT_TRUE;
                    udbOverride.vrfIdMsbEnableStdIpv4L4 = GT_TRUE;
                    break;

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                    udbOverride.vrfIdLsbEnableExtNotIpv6 = GT_TRUE;
                    udbOverride.vrfIdMsbEnableExtNotIpv6 = GT_TRUE;
                    break;

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                    udbOverride.vrfIdLsbEnableExtIpv6L2 = GT_TRUE;
                    udbOverride.vrfIdMsbEnableExtIpv6L2 = GT_TRUE;
                    break;

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                    udbOverride.vrfIdLsbEnableExtIpv6L4 = GT_TRUE;
                    udbOverride.vrfIdMsbEnableExtIpv6L4 = GT_TRUE;
                    break;

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                    udbOverride.vrfIdLsbEnableStdUdb   = GT_TRUE;
                    udbOverride.vrfIdMsbEnableStdUdb   = GT_TRUE;
                    break;

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                    udbOverride.vrfIdLsbEnableExtUdb   = GT_TRUE;
                    udbOverride.vrfIdMsbEnableExtUdb   = GT_TRUE;
                    break;

                default:
                    rc = GT_BAD_PARAM;
                    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid ruleFormat: %d", ruleFormat);
            }
        }

        if (GT_TRUE == useQosProfile)
        {
            /* configure UDB override structure */
            switch (ruleFormat)
            {

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                    udbOverride.qosProfileEnableStdUdb = GT_TRUE;
                    break;

                case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                    udbOverride.qosProfileEnableExtUdb = GT_TRUE;
                    break;

                default:
                    rc = GT_BAD_PARAM;
                    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid ruleFormat: %d", ruleFormat);
            }
        }

        /* overriding of  User Defined Bytes */
        rc = prvTgfPclOverrideUserDefinedBytesSet(&udbOverride);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclOverrideUserDefinedBytesSet: %d", prvTgfDevNum);
    }

    if (GT_TRUE == useVrId)
    {
        /* overrige VRF Id related UDB values */
        switch (ruleFormat)
        {
            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                pattern.ruleStdUdb.udb[0] = /* UDB0 */
                    (GT_U8)(PRV_TGF_VRFID_CNS & 0xFF);
                pattern.ruleStdUdb.udb[1] = /* UDB1 */
                    (GT_U8)((PRV_TGF_VRFID_CNS >> 8) & 0x0F);
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                pattern.ruleStdIpL2Qos.udb[0] = /* UDB18 */
                    (GT_U8)(PRV_TGF_VRFID_CNS & 0xFF);
                pattern.ruleStdIpL2Qos.udb[1] = /* UDB19 */
                    (GT_U8)((PRV_TGF_VRFID_CNS >> 8) & 0x0F);
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                pattern.ruleStdIpv4L4.udb[0] = /* UDB20 */
                    (GT_U8)(PRV_TGF_VRFID_CNS & 0xFF);
                pattern.ruleStdIpv4L4.udb[1] = /* UDB21 */
                    (GT_U8)((PRV_TGF_VRFID_CNS >> 8) & 0x0F);
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                pattern.ruleStdNotIp.udb[0] = /* UDB15 */
                    (GT_U8)(PRV_TGF_VRFID_CNS & 0xFF);
                pattern.ruleStdNotIp.udb[1] = /* UDB16 */
                    (GT_U8)((PRV_TGF_VRFID_CNS >> 8) & 0x0F);
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                pattern.ruleExtNotIpv6.udb[5] = /* UDB5 */
                    (GT_U8)(PRV_TGF_VRFID_CNS & 0xFF);
                pattern.ruleExtNotIpv6.udb[1] = /* UDB1 */
                    (GT_U8)((PRV_TGF_VRFID_CNS >> 8) & 0x0F);
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                pattern.ruleExtUdb.udb[1] = /* UDB1 */
                    (GT_U8)(PRV_TGF_VRFID_CNS & 0xFF);
                pattern.ruleExtUdb.udb[2] = /* UDB2 */
                    (GT_U8)((PRV_TGF_VRFID_CNS >> 8) & 0x0F);
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                pattern.ruleExtIpv6L2.udb[5] = /* UDB11 */
                    (GT_U8)(PRV_TGF_VRFID_CNS & 0xFF);
                pattern.ruleExtIpv6L2.udb[0] = /* UDB6 */
                    (GT_U8)((PRV_TGF_VRFID_CNS >> 8) & 0x0F);
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                pattern.ruleExtIpv6L4.udb[2] = /* UDB14 */
                    (GT_U8)(PRV_TGF_VRFID_CNS & 0xFF);
                pattern.ruleExtIpv6L4.udb[0] = /* UDB12 */
                    (GT_U8)((PRV_TGF_VRFID_CNS >> 8) & 0x0F);
                break;

            default:
                rc = GT_BAD_PARAM;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid ruleFormat: %d", ruleFormat);
        }

        /* set tested VRF Id for IN TTI Action  */
        prvTgfTunnelTermEtherTypeVrfIdAssignConfigurationSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                                             PRV_TGF_VRFID_CNS,
                                                             packetInfoPtr->partsArray[0].partPtr);
    }

    if (GT_TRUE == useQosProfile)
    {
        /* overrige Qos Profile Id related UDB values */
        switch (ruleFormat)
        {
            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                pattern.ruleStdUdb.udb[2] = /* UDB2 */
                    (GT_U8)PRV_TGF_QOS_PROFILE_CNS;
                break;

            case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                pattern.ruleExtUdb.udb[5] = /* UDB5 */
                    (GT_U8)PRV_TGF_QOS_PROFILE_CNS;
                break;

            default:
                rc = GT_BAD_PARAM;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid ruleFormat: %d", ruleFormat);
        }

        /* set tested Qos profile to SEND port */
        rc = prvTgfDxChPclQosProfileAndTrustModeSet(
            PRV_TGF_QOS_PROFILE_CNS, CPSS_QOS_PORT_NO_TRUST_E);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfDxChPclQosProfileAndTrustModeSet: %d", prvTgfDevNum);
    }


    /* set action cmd */
    action.pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;

    if (PRV_TGF_PCL_OFFSET_INVALID_E != offsetType)
    {
        /* sets the Policy Rule */
        rc = prvTgfPclRuleSet(ruleFormat, ruleIndex, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleSet: %d", prvTgfDevNum);
    }
}

/*******************************************************************************
* prvTgfPclUdbTestPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
*       portNum       - port number
*       packetInfoPtr - (pointer to) the packet info
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
static GT_VOID prvTgfPclUdbTestPacketSend
(
    IN  GT_U8                         portNum,
    IN  TGF_PACKET_STC               *packetInfoPtr
)
{
    GT_STATUS       rc         = GT_OK;
    GT_U32          portsCount = prvTgfPortsNum;
    GT_U32          portIter   = 0;


    /* use different 'etherType for vlan tagging' instead of 0x8100 */
    /* this will make our packet that has '0x8100' to be recognized as 'untagged'
      by the 'traffic generator' and also the CPSS will not override the ethertype
      of tag0 with the DSA tag */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

    /* -------------------------------------------------------------------------
     * 1. Sending unknown unicast
     */
    PRV_UTF_LOG0_MAC("======= Sending unknown unicast =======\n");

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, packetInfoPtr, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfSetTxSetupEth: %d, %d",
                                 prvTgfDevNum, prvTgfBurstCount, 0, 0);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* send Packet from port 0 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, portNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfStartTransmitingEth: %d", prvTgfDevNum);

    /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);

    /* Delay 100 milliseconds - give to sent packet to pass all pilelines in PP */
    cpssOsTimerWkAfter(100);
}

/*******************************************************************************
* prvTgfPclUdbTestReset
*
* DESCRIPTION:
*       Function clears FDB, VLAN tables and internal table of captured packets.
*
* INPUTS:
*       vlanId    - vlanId to be cleared
*       ruleSize  - size of Rule
*       ruleIndex - index of the rule to be invalidated in the TCAM
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
static GT_VOID prvTgfPclUdbTestReset
(
    IN GT_U16                   vlanId,
    IN CPSS_PCL_RULE_SIZE_ENT   ruleSize,
    IN GT_U32                   ruleIndex
)
{
    GT_STATUS   rc = GT_OK;
    CPSS_INTERFACE_INFO_STC     interfaceInfo;
    PRV_TGF_PCL_LOOKUP_CFG_STC  lookupCfg;


    PRV_UTF_LOG0_MAC("======= Restore Configuration =======\n");

    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* clear PCL configuration table */
    cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
    cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

    interfaceInfo.type               = CPSS_INTERFACE_PORT_E;
    interfaceInfo.devPort.devNum     = prvTgfDevNum;
    interfaceInfo.devPort.portNum    = prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS];
    lookupCfg.enableLookup           = GT_FALSE;
    lookupCfg.lookupType             = PRV_TGF_PCL_LOOKUP_TYPE_SINGLE_E;
    lookupCfg.dualLookup             = GT_FALSE;
    lookupCfg.externalLookup         = GT_FALSE;
    lookupCfg.groupKeyTypes.nonIpKey =
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    lookupCfg.groupKeyTypes.ipv4Key =
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    lookupCfg.groupKeyTypes.ipv6Key =
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

    rc = prvTgfPclCfgTblSet(&interfaceInfo, CPSS_PCL_DIRECTION_INGRESS_E,
                            CPSS_PCL_LOOKUP_0_E, &lookupCfg);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPclCfgTblSet");

    /* disable ingress policy on ports */
    rc = prvTgfPclPortIngressPolicyEnable(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclPortIngressPolicyEnable: %d", prvTgfDevNum);

    /* invalidate PCL rule */
    rc = prvTgfPclRuleValidStatusSet(ruleSize, ruleIndex, GT_FALSE);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPclRuleValidStatusSet");

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* restore default vlanId */
    rc = prvTgfBrgVlanPvidSet(prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPvidSet: %d, %d", prvTgfDevNum, prvTgfDefVlanId);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(vlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, vlanId);
}

/*******************************************************************************
* prvTgfPclTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
*
* INPUTS:
*       prvTgfPacketInfoPtr - (pointer to) the packet info
*       packetSize          - packet size
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
static GT_VOID prvTgfPclTrafficGenerate
(
    IN TGF_PACKET_STC   *prvTgfPacketInfoPtr,
    IN GT_U32            packetSize
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;


    PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* send packet to be matched */
    prvTgfPclUdbTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], prvTgfPacketInfoPtr);

    PRV_UTF_LOG0_MAC("======= Checking Counters =======\n");

    /* read and check ethernet counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* clear expected counters */
        cpssOsMemSet(&expectedCntrs, 0, sizeof(expectedCntrs));

        /* calculate expected counters */
        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            expectedCntrs.goodOctetsSent.l[0] = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
            expectedCntrs.goodOctetsRcv.l[0]  = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
            expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
        }

        /* check Rx\Tx counters */
        PRV_TGF_VERIFY_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(isEqualCntrs, GT_TRUE, "get another counters values.");

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
    }

    PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* send packet to be not matched */
    prvTgfPclUdbTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketNotMatchInfo);

    PRV_UTF_LOG0_MAC("======= Checking Counters =======\n");

    /* read and check ethernet counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* clear expected counters */
        cpssOsMemSet(&expectedCntrs, 0, sizeof(expectedCntrs));

        /* calculate expected Rx counters */
        expectedCntrs.pkts65to127Octets.l[0] = prvTgfBurstCount;
        expectedCntrs.goodOctetsSent.l[0]    = PRV_TGF_PACKET_NOT_MATCH_CRC_LEN_CNS * prvTgfBurstCount;
        expectedCntrs.goodPktsSent.l[0]      = prvTgfBurstCount;
        expectedCntrs.ucPktsSent.l[0]        = prvTgfBurstCount;

        /* calculate expected Tx counters */
        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            expectedCntrs.goodOctetsRcv.l[0] = PRV_TGF_PACKET_NOT_MATCH_CRC_LEN_CNS * prvTgfBurstCount;
            expectedCntrs.goodPktsRcv.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsRcv.l[0]     = prvTgfBurstCount;
        }

        /* check Rx\Tx counters */
        PRV_TGF_VERIFY_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(isEqualCntrs, GT_TRUE, "get another counters values.");

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
    }
}

/*******************************************************************************
* prvTgfPclAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 TCP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
*
* INPUTS:
*       prvTgfPacketInfoPtr - (pointer to) the packet info
*       packetSize          - packet size
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
GT_VOID prvTgfPclAdditionalTrafficGenerate
(
    IN TGF_PACKET_STC   *prvTgfPacketInfoPtr,
    IN GT_U32            packetSize
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;


    PRV_UTF_LOG0_MAC("======= Generating Traffic (UDB invalid) =======\n");

    /* send packet to be matched */
    prvTgfPclUdbTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], prvTgfPacketInfoPtr);

    PRV_UTF_LOG0_MAC("======= Checking Counters (UDB invalid) =======\n");

    /* read and check ethernet counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* clear expected counters */
        cpssOsMemSet(&expectedCntrs, 0, sizeof(expectedCntrs));

        /* calculate expected Rx counters */
        expectedCntrs.goodOctetsSent.l[0] = packetSize * prvTgfBurstCount;
        expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
        expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;

        /* calculate expected Tx counters */
        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            expectedCntrs.goodOctetsRcv.l[0] = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsRcv.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsRcv.l[0]     = prvTgfBurstCount;
        }

        /* check Rx\Tx counters */
        PRV_TGF_VERIFY_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(isEqualCntrs, GT_TRUE, "get another counters values.");

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
    }
}


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPclIpv4TcpConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L4_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4TcpConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E, PRV_TGF_PCL_OFFSET_L4_E,
                         GT_FALSE, GT_FALSE, &prvTgfTcpPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4TcpTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 TCP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 TCP packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4TcpTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfTcpPacketInfo, PRV_TGF_TCP_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4TcpAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4TcpAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfTcpPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4TcpAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 TCP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4TcpAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfTcpPacketInfo, PRV_TGF_TCP_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4TcpConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclIpv4TcpConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_STD_E, 0);
}

/*******************************************************************************
* prvTgfPclIpv4UdpConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L4_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4UdpConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E, PRV_TGF_PCL_OFFSET_L4_E,
                         GT_FALSE, GT_FALSE, &prvTgfUdpPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4UdpTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 UDP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 UDP packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4UdpTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfUdpPacketInfo, PRV_TGF_UDP_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4UdpAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4UdpAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfUdpPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4UdpAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 UDP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4UdpAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfUdpPacketInfo, PRV_TGF_UDP_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4UdpConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclIpv4UdpConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_STD_E, 0);
}

/*******************************************************************************
* prvTgfPclIpv4FragmentConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L3_MINUS_2_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4FragmentConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E, PRV_TGF_PCL_OFFSET_L3_MINUS_2_E,
                         GT_FALSE, GT_FALSE, &prvTgfIpv4FragmentPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4FragmentTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4FragmentTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(
        &prvTgfIpv4FragmentPacketInfo, PRV_TGF_IPV4_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4FragmentAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4FragmentAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfIpv4FragmentPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4FragmentAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4FragmentAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(
        &prvTgfIpv4FragmentPacketInfo, PRV_TGF_IPV4_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4FragmentConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclIpv4FragmentConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_STD_E, 0);
}

/*******************************************************************************
* prvTgfPclIpv4OtherConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L3_MINUS_2_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4OtherConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E, PRV_TGF_PCL_OFFSET_L3_MINUS_2_E,
                         GT_FALSE, GT_FALSE, &prvTgfIpv4OtherPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4OtherTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4OtherTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(
        &prvTgfIpv4OtherPacketInfo, PRV_TGF_IPV4_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4OtherAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4OtherAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfIpv4OtherPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4OtherAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4OtherAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(
        &prvTgfIpv4OtherPacketInfo, PRV_TGF_IPV4_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4OtherConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclIpv4OtherConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_STD_E, 0);
}

/*******************************************************************************
* prvTgfPclEthernetOtherConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L2_E
*         - set rule index to 0
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
GT_VOID prvTgfPclEthernetOtherConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                         PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E, PRV_TGF_PCL_OFFSET_L2_E,
                         GT_FALSE, GT_FALSE, &prvTgfEthernetPacketInfo);
}

/*******************************************************************************
* prvTgfPclEthernetOtherTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclEthernetOtherTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfEthernetPacketInfo, PRV_TGF_ETHERNET_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclEthernetOtherAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclEthernetOtherAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                         PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfEthernetPacketInfo);
}

/*******************************************************************************
* prvTgfPclEthernetOtherAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclEthernetOtherAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfEthernetPacketInfo, PRV_TGF_ETHERNET_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclEthernetOtherConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclEthernetOtherConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_STD_E, 0);
}

/*******************************************************************************
* prvTgfPclMplsConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_MPLS_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_MPLS_MINUS_2_E
*         - set rule index to 0
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
GT_VOID prvTgfPclMplsConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                         PRV_TGF_PCL_PACKET_TYPE_MPLS_E, PRV_TGF_PCL_OFFSET_MPLS_MINUS_2_E,
                         GT_FALSE, GT_FALSE, &prvTgfMplsPacketInfo);
}

/*******************************************************************************
* prvTgfPclMplsTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclMplsTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfMplsPacketInfo, PRV_TGF_MPLS_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclMplsAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_MPLS_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclMplsAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                         PRV_TGF_PCL_PACKET_TYPE_MPLS_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfMplsPacketInfo);
}

/*******************************************************************************
* prvTgfPclMplsAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclMplsAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfMplsPacketInfo, PRV_TGF_MPLS_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclMplsConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclMplsConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_EXT_E, 0);
}

/*******************************************************************************
* prvTgfPclUdeConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_UDE_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L2_E
*         - set rule index to 0
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
GT_VOID prvTgfPclUdeConfigSet
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_UDE_E, PRV_TGF_PCL_OFFSET_L2_E,
                         GT_FALSE, GT_FALSE, &prvTgfUdePacketInfo);

    /* set the EtherType to identify UDE */
    rc = prvTgfVntCfmEtherTypeSet(PRV_TGF_UDE_TAG_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfVntCfmEtherTypeSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_UDE_TAG_CNS);
}

/*******************************************************************************
* prvTgfPclUdeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclUdeTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfUdePacketInfo, PRV_TGF_UDE_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclUdeAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_UDE_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclUdeAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_UDE_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfEthernetPacketInfo);
}

/*******************************************************************************
* prvTgfPclUdeAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclUdeAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfUdePacketInfo, PRV_TGF_UDE_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclUdeConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclUdeConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_EXT_E, 0);
}

/*******************************************************************************
* prvTgfPclIpv6ConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV6_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L3_MINUS_2_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv6ConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV6_E, PRV_TGF_PCL_OFFSET_L3_MINUS_2_E,
                         GT_FALSE, GT_FALSE, &prvTgfIpv6PacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv6TrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv6TrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfIpv6PacketInfo, PRV_TGF_IPV6_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv6AdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV6_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv6AdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV6_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfIpv6PacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv6AdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv6AdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfIpv6PacketInfo, PRV_TGF_IPV6_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv6ConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclIpv6ConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_EXT_E, 0);
}

/*******************************************************************************
* prvTgfPclIpv6TcpConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV6_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L4_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv6TcpConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV6_E, PRV_TGF_PCL_OFFSET_L4_E,
                         GT_FALSE, GT_FALSE, &prvTgfIpv6TcpPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv6TcpTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv6TcpTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfIpv6TcpPacketInfo, PRV_TGF_IPV6_TCP_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv6TcpAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV6_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv6TcpAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV6_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_FALSE, &prvTgfIpv6TcpPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv6TcpAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv6TcpAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfIpv6TcpPacketInfo, PRV_TGF_IPV6_TCP_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv6TcpConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclIpv6TcpConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_EXT_E, 0);
}

/*******************************************************************************
* prvTgfPclEthernetOtherVridConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L2_E
*         - set useVrId to GT_TRUE
*         - set rule index to 0
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
GT_VOID prvTgfPclEthernetOtherVridConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E, PRV_TGF_PCL_OFFSET_L2_E,
                         GT_TRUE, GT_FALSE, &prvTgfEthernetPacketInfo);
}

/*******************************************************************************
* prvTgfPclEthernetOtherVridTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclEthernetOtherVridTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfEthernetPacketInfo, PRV_TGF_ETHERNET_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclEthernetOtherVridAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set useVrId to GT_TRUE
*         - set rule index to 0
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
GT_VOID prvTgfPclEthernetOtherVridAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_TRUE, GT_FALSE, &prvTgfEthernetPacketInfo);
}

/*******************************************************************************
* prvTgfPclEthernetOtherVridAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclEthernetOtherVridAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfEthernetPacketInfo, PRV_TGF_ETHERNET_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclEthernetOtherVridConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclEthernetOtherVridConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_STD_E, 0);

    /* restore TTI config */
    prvTgfTunnelTermEtherTypeVrfIdAssignConfigRestore(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
}

/*******************************************************************************
* prvTgfPclIpv4UdpQosConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L3_MINUS_2_E
*         - set useQosProfile to GT_TRUE
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4UdpQosConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E, PRV_TGF_PCL_OFFSET_L3_MINUS_2_E,
                         GT_FALSE, GT_TRUE, &prvTgfUdpPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4UdpQosTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 UDP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 UDP packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4UdpQosTrafficGenerate
(
    GT_VOID
)
{
    /* generate traffic */
    prvTgfPclTrafficGenerate(&prvTgfUdpPacketInfo, PRV_TGF_UDP_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4UdpQosAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set useQosProfile to GT_TRUE
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4UdpQosAdditionalConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration (UDB invalid) =======\n");

    /* init configuration */
    prvTgfPclUdbTestInit(0, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
                         PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E, PRV_TGF_PCL_OFFSET_INVALID_E,
                         GT_FALSE, GT_TRUE, &prvTgfUdpPacketInfo);
}

/*******************************************************************************
* prvTgfPclIpv4UdpQosAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 UDP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4UdpQosAdditionalTrafficGenerate
(
    GT_VOID
)
{
    /* generate additional traffic */
    prvTgfPclAdditionalTrafficGenerate(&prvTgfUdpPacketInfo, PRV_TGF_UDP_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfPclIpv4UdpQosConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfPclIpv4UdpQosConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfPclUdbTestReset(PRV_TGF_VLANID_CNS, CPSS_PCL_RULE_SIZE_EXT_E, 0);
}

