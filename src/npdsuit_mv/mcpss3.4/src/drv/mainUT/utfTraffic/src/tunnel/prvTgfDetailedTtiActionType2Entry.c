/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfDetailedTtiActionType2Entry.c
*
* DESCRIPTION:
*       Verify the functionality of TTI entry
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfIpGen.h>
#include <common/tgfCosGen.h>
#include <common/tgfTunnelGen.h>
#include <tunnel/prvTgfDetailedTtiActionType2Entry.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* port to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS       0

/* port to forward traffic to */
#define PRV_TGF_FORWARD_PORT_IDX_CNS    1

/* egress port number  */
#define PRV_TGF_EGR_PORT_IDX_CNS        3

/* default VLAN Id 5 */
#define PRV_TGF_VLANID_5_CNS        5

/* default VLAN Id 6 */
#define PRV_TGF_VLANID_6_CNS        6

/* default VLAN Id 7 */
#define PRV_TGF_VLANID_7_CNS        7

/* default VLAN Id 8 */
#define PRV_TGF_VLANID_8_CNS        8

/* default QOS profile */
#define PRV_TGF_QOS_PROFILE_DEFAULT_CNS 0

/* port QOS profile */
#define PRV_TGF_QOS_PROFILE_PORT_CNS    1

/* map QOS profile */
#define PRV_TGF_QOS_PROFILE_MAP_CNS     2

/* action QOS profile */
#define PRV_TGF_QOS_PROFILE_ACTION_CNS  3

/* default number of packets to send */
static GT_U32       prvTgfBurstCount = 1;

/* the Arp Address index of the Router ARP Table */
static GT_U32       prvTgfRouterArpIndex      = 123;

/* the Route entry index for UC Route entry Table */
static GT_U32       prvTgfRouteEntryBaseIndex = 3;

/* the LPM DB id for LPM Table */
static GT_U32       prvTgfLpmDBId             = 0;

/* the ARP MAC address to write to the Router ARP Table */
static TGF_MAC_ADDR prvTgfArpMac = {0x00, 0x00, 0x00, 0x00, 0xab, 0xcd};

/* ports <--> tag <--> vlan array */
static GT_U8        prvTgfPortIdxInVlanArr[][3] =
{
    {0 , 0, PRV_TGF_VLANID_5_CNS},
    {1 , 0, PRV_TGF_VLANID_5_CNS},
    {2, PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E, PRV_TGF_VLANID_6_CNS},
    {3, PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E, PRV_TGF_VLANID_6_CNS}
};

/*************************** Packet to send ***********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}                 /* saMac */
};

/* Nested VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketNestedVlanTagPart =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_7_CNS                          /* pri, cfi, VlanId */
};

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_5_CNS                          /* pri, cfi, VlanId */
};

/* packet's Ipv4 ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketIpv4EtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

/* packet's MPLS ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketMplsEtherTypePart = {TGF_ETHERTYPE_8847_MPLS_TAG_CNS};

/* MPLS packet with 2 labels - label 1 */
static TGF_PACKET_MPLS_STC prvTgfPacketMpls2Lbl1Part =
{
    0x66,               /* label */
    1,                  /* exp */
    0,                  /* stack */
    0xff                /* timeToLive */
};

/* MPLS packet with 2 labels - label 2 */
static TGF_PACKET_MPLS_STC prvTgfPacketMpls2Lbl2Part =
{
    0x11,               /* label */
    2,                  /* exp */
    1,                  /* stack */
    0xee                /* timeToLive */
};

/* MPLS packet with 3 labels - label 1 */
static TGF_PACKET_MPLS_STC prvTgfPacketMpls3Lbl1Part =
{
    0x66,               /* label */
    1,                  /* exp */
    0,                  /* stack */
    0xff                /* timeToLive */
};

/* MPLS packet with 3 labels - label 2 */
static TGF_PACKET_MPLS_STC prvTgfPacketMpls3Lbl2Part =
{
    0x11,               /* label */
    2,                  /* exp */
    0,                  /* stack */
    0xee                /* timeToLive */
};

/* MPLS packet with 3 labels - label 3 */
static TGF_PACKET_MPLS_STC prvTgfPacketMpls3Lbl3Part =
{
    0x33,               /* label */
    3,                  /* exp */
    1,                  /* stack */
    0xdd                /* timeToLive */
};

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4Part =
{
    4,                                 /* version */
    5,                                 /* headerLen */
    0,                                 /* typeOfService */
    0x2A,                              /* totalLen */
    0,                                 /* id */
    0,                                 /* flags */
    0,                                 /* offset */
    0x40,                              /* timeToLive */
    0x04,                              /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    {10, 10, 10, 10},                  /* srcAddr */
    { 4,  4,  4,  4}                   /* dstAddr */
};

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

/********************** QinQ Packet parts *************************************/

/* PARTS of the packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketNestedVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET_LEN_CNS                                             \
    TGF_L2_HEADER_SIZE_CNS + 2 * TGF_VLAN_TAG_SIZE_CNS +                   \
    TGF_ETHERTYPE_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS +                    \
    sizeof(prvTgfPacketPayloadDataArr)

/* Packet to send */
static TGF_PACKET_STC prvTgfPacketInfo =
{
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArray)
        / sizeof(prvTgfPacketPartArray[0]),                      /* numOfParts */
    prvTgfPacketPartArray                                        /* partsArray */
};

/************ Ipv4 Over MPLS Packet with 2 label parts *************************/

/* PARTS of the packet */
static TGF_PACKET_PART_STC prvTgfIpv4OverMpls2PacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketMplsEtherTypePart},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMpls2Lbl1Part},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMpls2Lbl2Part},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of packet */
#define PRV_TGF_IPV4_OVER_MPLS_2_PACKET_LEN_CNS                                \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS +  \
    2 * TGF_MPLS_HEADER_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS +                  \
    sizeof(prvTgfPacketPayloadDataArr)

/* Packet to send */
static TGF_PACKET_STC prvTgfIpv4OverMpls2PacketInfo =
{
    PRV_TGF_IPV4_OVER_MPLS_2_PACKET_LEN_CNS,                    /* totalLen */
    sizeof(prvTgfIpv4OverMpls2PacketPartArray)
        / sizeof(prvTgfIpv4OverMpls2PacketPartArray[0]),        /* numOfParts */
    prvTgfIpv4OverMpls2PacketPartArray                          /* partsArray */
};

/***************** MPLS Packet with 3 label parts *****************************/

/* PARTS of the packet */
static TGF_PACKET_PART_STC prvTgfMpls3PacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketMplsEtherTypePart},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMpls3Lbl1Part},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMpls3Lbl2Part},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMpls3Lbl3Part},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of packet */
#define PRV_TGF_MPLS_3_PACKET_LEN_CNS                                          \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS +  \
    3 * TGF_MPLS_HEADER_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS +                  \
    sizeof(prvTgfPacketPayloadDataArr)

/* Packet to send */
static TGF_PACKET_STC prvTgfMplsPacketInfo =
{
    PRV_TGF_MPLS_3_PACKET_LEN_CNS,                              /* totalLen */
    sizeof(prvTgfMpls3PacketPartArray)
        / sizeof(prvTgfMpls3PacketPartArray[0]),                /* numOfParts */
    prvTgfMpls3PacketPartArray                                  /* partsArray */
};

/****************** Ipv4 Over Ipv4 Packet parts *******************************/

/* packet's IPv4 over IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4OverIpv4Part =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2E,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    0xFF,               /* protocol */
    0x73CA,             /* csum */
    { 2,  2,  2,  2},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};

/* DATA of packet */
static GT_U8 prvTgfIpv4OverIpv4PayloadDataArr[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfIpv4OverIpv4PacketPayloadPart =
{
    sizeof(prvTgfIpv4OverIpv4PayloadDataArr),            /* dataLength */
    prvTgfIpv4OverIpv4PayloadDataArr                     /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfIpv4OverIpv4PacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4OverIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfIpv4OverIpv4PacketPayloadPart}
};

/* Length of packet */
#define PRV_TGF_IPV4_OVER_IPV4_PACKET_LEN_CNS                                  \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS +  \
    2 * TGF_IPV4_HEADER_SIZE_CNS + sizeof(prvTgfIpv4OverIpv4PayloadDataArr)

/* Length of packet with CRC */
#define PRV_TGF_IPV4_OVER_IPV4_PACKET_CRC_LEN_CNS                              \
    PRV_TGF_IPV4_OVER_IPV4_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* Packet to send */
static TGF_PACKET_STC prvTgfIpv4OverIpv4PacketInfo =
{
    PRV_TGF_IPV4_OVER_IPV4_PACKET_LEN_CNS,                        /* totalLen */
    sizeof(prvTgfIpv4OverIpv4PacketPartArray) / sizeof(prvTgfIpv4OverIpv4PacketPartArray[0]), /* numOfParts */
    prvTgfIpv4OverIpv4PacketPartArray                             /* partsArray */
};
/******************************************************************************/


/******************************************************************************\
 *                            Private variables                               *
\******************************************************************************/

/* stored default VID */
static GT_U16   prvTgfDefVlanId = 0;

static GT_BOOL  prvTgfRoutingEnable = GT_FALSE;

/* IPv4 prefix that will route the tunnel passenger packet */
static GT_U8 *prvTgfIpv4PrefixPtr = prvTgfPacketIpv4OverIpv4Part.dstAddr;

/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfTunnelTermRoutingInit
*
* DESCRIPTION:
*       Set IPv4 prefix that will route the tunnel passenger packet
*
* INPUTS:
*       ipv4Addr - IPv4 prefix
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
static GT_VOID prvTgfTunnelTermRoutingInit
(
    IN TGF_IPV4_ADDR ipv4Addr
)
{
    GT_STATUS                       rc = GT_OK;
    GT_IPADDR                       ipAddr;
    PRV_TGF_IP_UC_ROUTE_ENTRY_STC   routeEntriesArray[1];
    PRV_TGF_IP_UC_ROUTE_ENTRY_STC  *regularEntryPtr;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    PRV_TGF_BRG_MAC_ENTRY_STC            macEntry;

    /* create a macEntry with .daRoute = GT_TRUE */
    cpssOsMemSet(&macEntry, 0, sizeof(macEntry));
    cpssOsMemCpy(macEntry.key.key.macVlan.macAddr.arEther, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));

    macEntry.key.entryType                  = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.vlanId         = PRV_TGF_VLANID_5_CNS;
    macEntry.dstInterface.type              = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.devNum    = prvTgfDevNum;
    macEntry.dstInterface.devPort.portNum   = prvTgfPortsArray[PRV_TGF_FORWARD_PORT_IDX_CNS];
    macEntry.isStatic                       = GT_TRUE;
    macEntry.daRoute                        = GT_TRUE;

    rc = prvTgfBrgFdbMacEntrySet(&macEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntryWrite: %d", prvTgfDevNum);


    /* enable Unicast IPv4 Routing on a Port */
    rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                   CPSS_IP_UNICAST_E,
                                   CPSS_IP_PROTOCOL_IPV4_E,
                                   GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
            "prvTgfIpPortRoutingEnable: %d %d",
            prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* enable IPv4 Unicast Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_5_CNS,
                                   CPSS_IP_UNICAST_E,
                                   CPSS_IP_PROTOCOL_IPV4_E,
                                   GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
            "prvTgfIpVlanRoutingEnable: %d %d",
            prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* create a UC Route entry */
    cpssOsMemSet(routeEntriesArray, 0, sizeof(routeEntriesArray));

    regularEntryPtr = &routeEntriesArray[0];
    regularEntryPtr->cmd                   = CPSS_PACKET_CMD_ROUTE_E;
    regularEntryPtr->nextHopVlanId         = PRV_TGF_VLANID_6_CNS;
    regularEntryPtr->nextHopARPPointer     = prvTgfRouterArpIndex;
    regularEntryPtr->nextHopInterface.type = CPSS_INTERFACE_PORT_E;
    regularEntryPtr->nextHopInterface.devPort.devNum  = prvTgfDevNum;
    regularEntryPtr->nextHopInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    rc = prvTgfIpUcRouteEntriesWrite(prvTgfRouteEntryBaseIndex,
                                     routeEntriesArray,
                                     1 /* numOfRouteEntries */);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfIpUcRouteEntriesWrite: %d", prvTgfDevNum);

    /* create a new Ipv4 prefix */
    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = prvTgfRouteEntryBaseIndex;

    cpssOsMemCpy(ipAddr.arIP, ipv4Addr, sizeof(ipAddr.arIP));

    rc = prvTgfIpLpmIpv4UcPrefixAdd(prvTgfLpmDBId, 0, ipAddr, 32,
                                    &nextHopInfo, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfIpLpmIpv4UcPrefixAdd: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfTunnelTermPbrInit
*
* DESCRIPTION:
*       Set PBR Configuration
*
* INPUTS:
*       ipv4Addr - IPv4 prefix
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
static GT_VOID prvTgfTunnelTermPbrInit
(
    IN TGF_IPV4_ADDR ipv4Addr
)
{
    GT_STATUS                               rc = GT_OK;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_IPADDR                               ipAddr;
    PRV_TGF_BRG_MAC_ENTRY_STC               macEntry;

    PRV_UTF_LOG0_MAC("==== Setting PBR Configuration ====\n");


    /* create a macEntry with .daRoute = GT_TRUE */
    cpssOsMemSet(&macEntry, 0, sizeof(macEntry));
    cpssOsMemCpy(macEntry.key.key.macVlan.macAddr.arEther, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));

    macEntry.key.entryType                  = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.vlanId         = PRV_TGF_VLANID_5_CNS;
    macEntry.dstInterface.type              = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.devNum    = prvTgfDevNum;
    macEntry.dstInterface.devPort.portNum   = prvTgfPortsArray[PRV_TGF_FORWARD_PORT_IDX_CNS];
    macEntry.isStatic                       = GT_TRUE;
    macEntry.daRoute                        = GT_TRUE;

    rc = prvTgfBrgFdbMacEntrySet(&macEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntryWrite: %d", prvTgfDevNum);


    /* -------------------------------------------------------------------------
     * 1. PCL Config
     */

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInitExt1(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
        CPSS_PCL_DIRECTION_INGRESS_E,
        CPSS_PCL_LOOKUP_1_E, /* PBR must be in lookup 1 ! */
        PRV_TGF_PCL_PBR_ID_FOR_VIRTUAL_ROUTER_CNS, /* pclId */
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E /*nonIpKey*/,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E /*ipv4Key*/,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E /*ipv6Key*/);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfPclDefPortInitExt1: %d", prvTgfDevNum);

    /* -------------------------------------------------------------------------
     * 2. IP Config
     */

    /* enable IPv4 Unicast Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_5_CNS, CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_5_CNS);

    /* -------------------------------------------------------------------------
     * 3. Create a new Ipv4 prefix in a Virtual Router for the specified LPM DB
     */

    /* fill a nexthope info for the prefix */
    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));

    nextHopInfo.pclIpUcAction.pktCmd                        = CPSS_PACKET_CMD_FORWARD_E;
    nextHopInfo.pclIpUcAction.mirror.cpuCode                = CPSS_NET_CONTROL_E;
    nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount = GT_TRUE;
    nextHopInfo.pclIpUcAction.vlan.vlanId                   = PRV_TGF_VLANID_6_CNS;
    nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute         = GT_TRUE;
    nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex          = prvTgfRouterArpIndex;

    nextHopInfo.pclIpUcAction.redirect.redirectCmd          = PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
    nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.type            = CPSS_INTERFACE_PORT_E;
    nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.devNum  = prvTgfDevNum;
    nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    /* fill a destination IP address for the prefix */
    cpssOsMemCpy(ipAddr.arIP, ipv4Addr, sizeof(ipAddr.arIP));

    /* call CPSS function */
    rc = prvTgfIpLpmIpv4UcPrefixAdd(prvTgfLpmDBId, 0, ipAddr, 32, &nextHopInfo, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixAdd: %d", prvTgfDevNum);
}


/*******************************************************************************
* prvTgfTunnelTermDefVlanEntryInit
*
* DESCRIPTION:
*       Set initial test settings for vlan entries:
*           - create VLAN_1 with ports 0, 8
*           - create VLAN_2 with ports 18, 23
*           - Set VLAN_1 as port 0 default VLAN ID (PVID)
*
* INPUTS:
*       vlanId0 - first VLAN
*       vlanId1 - second VLAN
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
static GT_VOID prvTgfTunnelTermDefVlanEntryInit
(
    IN GT_U16                         vlanId0,
    IN GT_U16                         vlanId1
)
{
    GT_STATUS rc = GT_OK;
    GT_U8     tagArray[] = {0, 0, 0, 1};

    /* create a vlan 5 on ports (0,0), (0,8) */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(vlanId0, prvTgfPortsArray,
                                           NULL, tagArray, prvTgfPortsNum / 2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* create a vlan 6 on ports (0,18), (0,23)Tag */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(vlanId1, prvTgfPortsArray + 2,
                                           NULL, tagArray + 2, prvTgfPortsNum / 2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* get default port vlanId */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* set VLAN 5 as port 0 default VLAN ID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], vlanId0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
}

/*******************************************************************************
* prvTgfQosPortReset
*
* DESCRIPTION:
*       This function resets configuration of Qos for specified port
*
* INPUTS:
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfQosPortReset
(
    IN GT_U8 portNum
)
{
    GT_STATUS               rc           = GT_OK;
    GT_U8                   qosProfileId = 0;
    PRV_TGF_COS_PROFILE_STC qosProfile   = {0};
    CPSS_QOS_ENTRY_STC      portQosCfg   = {0};

    /* reset QoS profile with default values of DP, UP, TC, DSCP, Exp */
    rc = prvTgfCosProfileEntrySet(qosProfileId, &qosProfile);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

    /* reset port's default QoS Profile for not tagged packets or NO_TRUST mode */
    portQosCfg.qosProfileId     = qosProfileId;
    portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    rc = prvTgfCosPortQosConfigSet(portNum, &portQosCfg);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosConfigSet: %d", prvTgfDevNum);

    /* reset port's Qos Trust Mode as TRUST_L2_L3 */
    rc = prvTgfCosPortQosTrustModeSet(portNum, CPSS_QOS_PORT_TRUST_L2_L3_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosTrustModeSet: %d", prvTgfDevNum);

    /* reset port's Exp Trust mode */
    rc = prvTgfCosTrustExpModeSet(portNum, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosTrustExpModeSet: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfQosMapsReset
*
* DESCRIPTION:
*       This function resets QoS maps for specified pattern
*
* INPUTS:
*       pattern - pattern for UP, DSCP, MPLS fields in packet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfQosMapsReset
(
    IN GT_U8 pattern
)
{
    GT_STATUS               rc           = GT_OK;
    GT_U8                   qosProfileId = 0;

    if (pattern <= 63)
    {
        /* maps DSCP=pattern to a default QoS profile=0 */
        rc = prvTgfCosDscpToProfileMapSet(pattern, qosProfileId);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
                "prvTgfCosDscpToProfileMapSet: %d", prvTgfDevNum);
    }

    if (pattern <= 7)
    {
        /* maps UP=pattern to a default QoS profile=0 */
        rc = prvTgfCosUpCfiDeiToProfileMapSet(PRV_TGF_COS_UP_PROFILE_INDEX_DEFAULT_CNS,pattern, 0 /*cfiDeiBit*/, qosProfileId);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosUpCfiDeiToProfileMapSet: %d", prvTgfDevNum);

        /* maps EXP=pattern to a default QoS profile=0 */
        rc = prvTgfCosExpToProfileMapSet(pattern, qosProfileId);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosExpToProfileMapSet: %d", prvTgfDevNum);
    }
}

/*******************************************************************************
* prvTgfTunnelTermTestInit
*
* DESCRIPTION:
*       Set initial test settings:
*           - Create VLAN 5 with ports 0, 8
*           - Create VLAN 6 with ports 18, 23
*           - Set VLAN 5 as port 0 default VLAN ID (PVID)
*           - Set TTI rule
*           - Set TTI action
*
* INPUTS:
*       ruleIndex   - index of the tunnel termination entry
*       macMode     - MAC mode to use
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
static GT_VOID prvTgfTunnelTermTestInit
(
    IN GT_U32                         ruleIndex,
    IN PRV_TGF_TTI_MAC_MODE_ENT       macMode
)
{
    GT_STATUS                   rc       = GT_OK;
    GT_U32                      portIter = 0;
    PRV_TGF_TTI_RULE_UNT        pattern;
    PRV_TGF_TTI_RULE_UNT        mask;
    PRV_TGF_TTI_ACTION_STC      ruleAction = {0};


    /* set default vlan entry for vlan ID 5 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_5_CNS, prvTgfPortsArray,
                                           NULL, NULL, prvTgfPortsNum / 2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* set default vlan entry for vlan ID 6 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_6_CNS, prvTgfPortsArray + 2,
                                           NULL, NULL, prvTgfPortsNum / 2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* add taggs for ports */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortIdxInVlanArr[portIter][1])
        {
            rc = prvTgfBrgVlanTagMemberAdd(prvTgfPortIdxInVlanArr[portIter][2],
                                           prvTgfPortsArray[prvTgfPortIdxInVlanArr[portIter][0]],
                                           prvTgfPortIdxInVlanArr[portIter][1]);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTagMemberAdd: %d", prvTgfDevNum);
        }
    }

    /* get default port vlanId */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* set VLAN 5 as port 0 default VLAN ID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_VLANID_5_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* create a macEntry for tunneled terminated packet in VLAN 6 */
    rc = prvTgfBrgDefFdbMacEntryOnPortSet(
            prvTgfPacketL2Part.daMac, PRV_TGF_VLANID_6_CNS, prvTgfDevNum,
            prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS], GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);

    /* enable port 0 for Ethernet TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_TTI_KEY_ETH_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* set MIM TTI key lookup MAC mode */
    rc = prvTgfTtiMacModeSet(PRV_TGF_TTI_KEY_ETH_E, macMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMacModeSet: %d", prvTgfDevNum);

    /* clear entry */
    cpssOsMemSet((GT_VOID*) &ruleAction, 0, sizeof(ruleAction));

    /* configure TTI rule action */
    ruleAction.tunnelTerminate     = GT_FALSE;
    ruleAction.tsPassengerPacketType = PRV_TGF_TUNNEL_PASSENGER_OTHER_E;
    ruleAction.passengerPacketType = PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E;
    ruleAction.command             = CPSS_PACKET_CMD_FORWARD_E;
    ruleAction.redirectCommand     = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
    ruleAction.tag0VlanCmd         = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ruleAction.tag0VlanId          = PRV_TGF_VLANID_6_CNS;
    ruleAction.tag1VlanCmd         = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ruleAction.tag1VlanId          = PRV_TGF_VLANID_8_CNS;
    ruleAction.tunnelStart         = GT_FALSE;

    ruleAction.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    ruleAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;
    ruleAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    /* set TTI rule action */
    rc = prvTgfTtiRuleActionSet(ruleIndex, &ruleAction);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleActionSet: %d", prvTgfDevNum);

    /* clear entries */
    cpssOsMemSet((GT_VOID*) &pattern, 0, sizeof(pattern));
    cpssOsMemSet((GT_VOID*) &mask, 0, sizeof(mask));

    /* configure Ethernet TTI rule */
    pattern.eth.common.pclId  = 3;
    pattern.eth.common.vid    = PRV_TGF_VLANID_5_CNS;
    pattern.eth.isVlan1Exists = GT_TRUE;
    pattern.eth.vid1          = PRV_TGF_VLANID_7_CNS;

    /* configure TTI rule */
    switch (macMode)
    {
        case PRV_TGF_TTI_MAC_MODE_DA_E:
            cpssOsMemCpy((GT_VOID*)pattern.eth.common.mac.arEther,
                         (GT_VOID*)prvTgfPacketL2Part.daMac,
                         sizeof(pattern.eth.common.mac.arEther));
            break;

        case PRV_TGF_TTI_MAC_MODE_SA_E:
            cpssOsMemCpy((GT_VOID*)pattern.eth.common.mac.arEther,
                         (GT_VOID*)prvTgfPacketL2Part.saMac,
                         sizeof(pattern.eth.common.mac.arEther));
            break;

        default:
            rc = GT_BAD_PARAM;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid macMode: %d", macMode);
    }

    /* configure TTI rule mask */
    cpssOsMemSet((GT_VOID*)mask.eth.common.mac.arEther, 0xFF,
                 sizeof(mask.eth.common.mac.arEther));
    cpssOsMemSet((GT_VOID*) &(mask.eth.common.vid), 0xFF,
                 sizeof(mask.eth.common.vid));
    cpssOsMemSet((GT_VOID*) &(mask.eth.isVlan1Exists), 0xFF,
                 sizeof(mask.eth.isVlan1Exists));
    cpssOsMemSet((GT_VOID*) &(mask.eth.vid1), 0xFF,
                 sizeof(mask.eth.vid1));

    /* set TTI rule */
    rc = prvTgfTtiRuleSet(ruleIndex, PRV_TGF_TTI_KEY_ETH_E, &pattern, &mask, &ruleAction);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleSet: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfTunnelTermMplsKeyQosTestInit
*
* DESCRIPTION:
*       Set initial test settings:
*           - Create VLAN 5 with ports 0, 8
*           - Create VLAN 6 with ports 18, 23
*           - Set VLAN 5 as port 0 default VLAN ID (PVID)
*           - Set MPLS TTI key lookup MAC mode to Mac DA
*           - Set MPLS TTI rule to match MPLS packet with 2 labels
*           - Set TTI action to terminate the tunnel and set trust EXP to true
*           - Set EXP to QoS profile mapping entry
*           - Set IPv4 prefix that will route the tunnel passenger packet
*
* INPUTS:
*       ruleIndex     - index of the tunnel termination entry
*       macMode       - MAC mode to use
*       numOfLabels   - number of MPLS labels in the label stack
*       mplsCmd       - MPLS command
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
static GT_VOID prvTgfTunnelTermMplsKeyQosTestInit
(
    IN GT_U32                         ruleIndex,
    IN PRV_TGF_TTI_MAC_MODE_ENT       macMode,
    IN GT_U32                         numOfLabels,
    IN PRV_TGF_TTI_MPLS_COMMAND_ENT   mplsCmd
)
{
    GT_STATUS                       rc = GT_OK;
    PRV_TGF_TTI_RULE_UNT            pattern;
    PRV_TGF_TTI_RULE_UNT            mask;
    PRV_TGF_TTI_ACTION_STC          ruleAction;
    PRV_TGF_COS_PROFILE_STC         cosProfile;
    CPSS_QOS_ENTRY_STC              portQosCfg;
    PRV_TGF_IP_ROUTING_MODE_ENT     routingMode;
    GT_ETHERADDR                    arpMacAddr;


    /* set default vlan entries */
    prvTgfTunnelTermDefVlanEntryInit(PRV_TGF_VLANID_5_CNS, PRV_TGF_VLANID_6_CNS);

    /* set default SEND port QoS profile entry */
    cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
    cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
    cosProfile.userPriority   = PRV_TGF_QOS_PROFILE_PORT_CNS;
    cosProfile.trafficClass   = PRV_TGF_QOS_PROFILE_PORT_CNS;
    cosProfile.dscp           = PRV_TGF_QOS_PROFILE_PORT_CNS;
    cosProfile.exp            = PRV_TGF_QOS_PROFILE_PORT_CNS;

    rc = prvTgfCosProfileEntrySet(PRV_TGF_QOS_PROFILE_PORT_CNS, &cosProfile);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

    portQosCfg.qosProfileId     = PRV_TGF_QOS_PROFILE_PORT_CNS;
    portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
    portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;

    rc = prvTgfCosPortQosConfigSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &portQosCfg);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosConfigSet: %d", prvTgfDevNum);

    /* maps the packet MPLS exp=0 to a QoS profile */
    cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
    cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
    cosProfile.userPriority   = PRV_TGF_QOS_PROFILE_MAP_CNS;
    cosProfile.trafficClass   = PRV_TGF_QOS_PROFILE_MAP_CNS;
    cosProfile.dscp           = PRV_TGF_QOS_PROFILE_MAP_CNS;
    cosProfile.exp            = PRV_TGF_QOS_PROFILE_MAP_CNS;

    rc = prvTgfCosProfileEntrySet(PRV_TGF_QOS_PROFILE_MAP_CNS, &cosProfile);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

    rc = prvTgfCosExpToProfileMapSet(0, PRV_TGF_QOS_PROFILE_MAP_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosExpToProfileMapSet: %d", prvTgfDevNum);

    /* set QoS profile entry for TTI Action */
    cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
    cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
    cosProfile.userPriority   = PRV_TGF_QOS_PROFILE_ACTION_CNS;
    cosProfile.trafficClass   = PRV_TGF_QOS_PROFILE_ACTION_CNS;
    cosProfile.dscp           = PRV_TGF_QOS_PROFILE_ACTION_CNS;
    cosProfile.exp            = PRV_TGF_QOS_PROFILE_ACTION_CNS;

    rc = prvTgfCosProfileEntrySet(PRV_TGF_QOS_PROFILE_ACTION_CNS, &cosProfile);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

    /* set MIM EtherType */
    rc = prvTgfTtiMimEthTypeSet(TGF_ETHERTYPE_88E7_MIM_TAG_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMimEthTypeSet: %d", prvTgfDevNum);

    /* enable port 0 TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_TTI_KEY_MPLS_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* set MIM TTI key lookup MAC mode */
    rc = prvTgfTtiMacModeSet(PRV_TGF_TTI_KEY_MPLS_E, macMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMacModeSet: %d", prvTgfDevNum);

    /* clear entry */
    cpssOsMemSet((GT_VOID*) &ruleAction, 0, sizeof(ruleAction));
    cpssOsMemSet((GT_VOID*) &pattern, 0, sizeof(pattern));
    cpssOsMemSet((GT_VOID*) &mask, 0, sizeof(mask));

    /* configure TTI rule action */
    ruleAction.tunnelStart = GT_FALSE;/* we don't need TS so the field of :
                        tsPassengerPacketType not relevant */
    ruleAction.command               = CPSS_PACKET_CMD_FORWARD_E;
    ruleAction.mplsCmd               = mplsCmd;
    ruleAction.trustExp              = GT_TRUE;
    ruleAction.qosProfile            = PRV_TGF_QOS_PROFILE_ACTION_CNS;
    ruleAction.tunnelStart           = GT_FALSE;

    ruleAction.interfaceInfo.type    = CPSS_INTERFACE_PORT_E;
    ruleAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;

    ruleAction.tag1VlanCmd           = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;
    ruleAction.tag1VlanId            = 0;

    /*prvTgfRoutingEnable = GT_FALSE;*/
    if (prvTgfRoutingEnable == GT_TRUE)
    {
        ruleAction.tunnelTerminate     = GT_TRUE;
        ruleAction.passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV4_E;
        ruleAction.redirectCommand     = PRV_TGF_TTI_NO_REDIRECT_E;
        ruleAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS];
        ruleAction.tag0VlanCmd         = PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E;
        ruleAction.tag0VlanId          = 0;
    }
    else
    {
        ruleAction.tunnelTerminate     = GT_FALSE;
        /* when MPLS packet not terminated , we must set:
           1. 'passenger type to MPLS'
           2. since we do 'pop ' (one of the LSR operations) the packet will be
              considered 'MPLS routed' and that iomplies that the 'ARP pointer'
              will be used. So we need to set the ARP pointer into the ARP table.
              --> meaning the MAC addresses of the packet will be changed even
                  though the packet is NOT TS(tunnel start) and not TT(terminated)
        */
        ruleAction.passengerPacketType = PRV_TGF_TTI_PASSENGER_MPLS_E;
        ruleAction.redirectCommand     = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
        ruleAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];
        ruleAction.tag0VlanCmd         = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
        ruleAction.tag0VlanId          = 6;
        ruleAction.arpPtr              = prvTgfRouterArpIndex; /* an LSR is an intermediate MPLS router in the network */

        /* create a macEntry for tunneled terminated packet in VLAN 6 */
        rc = prvTgfBrgDefFdbMacEntryOnPortSet(
                prvTgfPacketL2Part.daMac, PRV_TGF_VLANID_6_CNS, prvTgfDevNum,
                prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS], GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);
    }

    /* configure TTI rule for MPLS labels */
    switch (numOfLabels)
    {
        case 0:
            break;

        case 1:
            pattern.mpls.label0 = 0x66;
            pattern.mpls.exp0   = 1;

            mask.mpls.label0    = 0xFFFFF;
            mask.mpls.exp0      = 0x07;
            break;

        case 2:
            pattern.mpls.label0 = 0x66;
            pattern.mpls.exp0   = 1;
            pattern.mpls.label1 = 0x11;
            pattern.mpls.exp1   = 2;

            mask.mpls.label0    = 0xFFFFF;
            mask.mpls.exp0      = 0x07;
            mask.mpls.label1    = 0xFFFFF;
            mask.mpls.exp1      = 0x07;
            break;

        case 3:
            pattern.mpls.label0 = 0x66;
            pattern.mpls.exp0   = 1;
            pattern.mpls.label1 = 0x11;
            pattern.mpls.exp1   = 2;
            pattern.mpls.label2 = 0x33;
            pattern.mpls.exp2   = 3;

            mask.mpls.label0    = 0xFFFFF;
            mask.mpls.exp0      = 0x07;
            mask.mpls.label1    = 0xFFFFF;
            mask.mpls.exp1      = 0x07;
            mask.mpls.label2    = 0xFFFFF;
            mask.mpls.exp2      = 0x07;
            break;

        default:
            rc = GT_BAD_PARAM;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid MPLS label: %d", numOfLabels);
    }

    /* configure TTI rule for MAC DA/SA */
    switch (macMode)
    {
        case PRV_TGF_TTI_MAC_MODE_DA_E:
            cpssOsMemCpy((GT_VOID*)pattern.mpls.common.mac.arEther,
                         (GT_VOID*)prvTgfPacketL2Part.daMac,
                         sizeof(pattern.mpls.common.mac.arEther));
            break;

        case PRV_TGF_TTI_MAC_MODE_SA_E:
            cpssOsMemCpy((GT_VOID*)pattern.mpls.common.mac.arEther,
                         (GT_VOID*)prvTgfPacketL2Part.saMac,
                         sizeof(pattern.mpls.common.mac.arEther));
            break;

        default:
            rc = GT_BAD_PARAM;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid macMode: %d", macMode);
    }

    cpssOsMemSet((GT_VOID*)mask.mpls.common.mac.arEther, 0xFF,
                 sizeof(mask.mpls.common.mac.arEther));

    /* configure MPLS TTI rule */
    pattern.mpls.common.pclId  = 2;
    pattern.mpls.common.vid    = PRV_TGF_VLANID_5_CNS;
    pattern.mpls.numOfLabels   = numOfLabels - 1;
    mask.mpls.common.pclId     = 0x3FF;
    mask.mpls.common.vid       = 0x0F;
    mask.mpls.numOfLabels      = 0xFF;

    /* set TTI rule */
    rc = prvTgfTtiRuleSet(ruleIndex, PRV_TGF_TTI_KEY_MPLS_E, &pattern, &mask, &ruleAction);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleSet: %d", prvTgfDevNum);

    /* write a ARP MAC address to the Router ARP Table */
    cpssOsMemCpy(arpMacAddr.arEther, prvTgfArpMac, sizeof(TGF_MAC_ADDR));

    rc = prvTgfIpRouterArpAddrWrite(prvTgfRouterArpIndex, &arpMacAddr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfIpRouterArpAddrWrite: %d", prvTgfDevNum);

    /* Set IPv4 prefix that will route the tunnel passenger packet */
    if (prvTgfRoutingEnable == GT_TRUE)
    {
        /* get routing mode */
        rc = prvTgfIpRoutingModeGet(&routingMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRoutingModeGet: %d", prvTgfDevNum);

        switch(routingMode)
        {
            case PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E:

                prvTgfTunnelTermPbrInit(prvTgfIpv4PrefixPtr);
                break;

            case PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E:

                prvTgfTunnelTermRoutingInit(prvTgfIpv4PrefixPtr);
                break;

            case PRV_TGF_IP_ROUTING_MODE_UNDEFINED_E:

                break;
        }
    }
}

/*******************************************************************************
* prvTgfTunnelTermIpv4KeyQosTestInit
*
* DESCRIPTION:
*       Set initial test settings:
*           - Create VLAN 5 with ports 0, 8
*           - Create VLAN 6 with ports 18, 23
*           - Set VLAN 5 as port 0 default VLAN ID (PVID)
*           - Set MPLS TTI key lookup MAC mode to Mac DA
*           - Set MPLS TTI rule to match MPLS packet with 2 labels
*           - Set TTI action to terminate the tunnel and set trust EXP to true
*           - Set EXP to QoS profile mapping entry
*           - Set IPv4 prefix that will route the tunnel passenger packet
*
* INPUTS:
*       ruleIndex     - index of the tunnel termination entry
*       macMode       - MAC mode to use
*       enableTrust   - enable trust QoS fields
*       cosParam      - Cos parameters (relevant only of enableTrust is GT_TRUE)
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
static GT_VOID prvTgfTunnelTermIpv4KeyQosTestInit
(
    IN GT_U32                         ruleIndex,
    IN PRV_TGF_TTI_MAC_MODE_ENT       macMode,
    IN GT_BOOL                        enableTrust,
    IN CPSS_COS_PARAM_ENT             cosParam
)
{
    GT_STATUS                       rc = GT_OK;
    PRV_TGF_TTI_RULE_UNT            pattern;
    PRV_TGF_TTI_RULE_UNT            mask;
    PRV_TGF_TTI_ACTION_STC          ruleAction;
    PRV_TGF_COS_PROFILE_STC         cosProfile;
    CPSS_QOS_ENTRY_STC              portQosCfg;
    PRV_TGF_IP_ROUTING_MODE_ENT     routingMode;
    GT_ETHERADDR                    arpMacAddr;


    /* set default vlan entries */
    prvTgfTunnelTermDefVlanEntryInit(PRV_TGF_VLANID_5_CNS, PRV_TGF_VLANID_6_CNS);

    /* enable port 0 TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                      PRV_TGF_TTI_KEY_IPV4_E,
                                      GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* set IPV4 TTI key lookup MAC mode */
    rc = prvTgfTtiMacModeSet(PRV_TGF_TTI_KEY_IPV4_E, macMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfTtiMacModeSet: %d", prvTgfDevNum);

    /* set default SEND port QoS profile entry */
    cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
    cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
    cosProfile.userPriority   = PRV_TGF_QOS_PROFILE_PORT_CNS;
    cosProfile.trafficClass   = PRV_TGF_QOS_PROFILE_PORT_CNS;
    cosProfile.dscp           = PRV_TGF_QOS_PROFILE_PORT_CNS;
    cosProfile.exp            = PRV_TGF_QOS_PROFILE_PORT_CNS;

    rc = prvTgfCosProfileEntrySet(PRV_TGF_QOS_PROFILE_PORT_CNS, &cosProfile);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

    /* set QOS profile */
    portQosCfg.qosProfileId     = PRV_TGF_QOS_PROFILE_PORT_CNS;
    portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
    portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;

    rc = prvTgfCosPortQosConfigSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &portQosCfg);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfCosPortQosConfigSet: %d", prvTgfDevNum);

    /* maps the packet MPLS exp=0, UP=0, DSCP=0 to a QoS profile */
    cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
    cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
    cosProfile.userPriority   = PRV_TGF_QOS_PROFILE_MAP_CNS;
    cosProfile.trafficClass   = PRV_TGF_QOS_PROFILE_MAP_CNS;
    cosProfile.dscp           = PRV_TGF_QOS_PROFILE_MAP_CNS;
    cosProfile.exp            = PRV_TGF_QOS_PROFILE_MAP_CNS;

    rc = prvTgfCosProfileEntrySet(PRV_TGF_QOS_PROFILE_MAP_CNS, &cosProfile);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

    /* maps DSCP=0 to a QoS profile */
    rc = prvTgfCosDscpToProfileMapSet(0, PRV_TGF_QOS_PROFILE_MAP_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfCosDscpToProfileMapSet: %d", prvTgfDevNum);

    /* maps EXP=0 to a QoS profile */
    rc = prvTgfCosExpToProfileMapSet(0, PRV_TGF_QOS_PROFILE_MAP_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfCosExpToProfileMapSet: %d", prvTgfDevNum);

    /* maps UP=0 to a QoS profile */
    rc = prvTgfCosUpCfiDeiToProfileMapSet(PRV_TGF_COS_UP_PROFILE_INDEX_DEFAULT_CNS,0, 0, PRV_TGF_QOS_PROFILE_MAP_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfCosUpCfiDeiToProfileMapSet: %d", prvTgfDevNum);

    /* set QoS profile entry for TTI Action */
    cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
    cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
    cosProfile.userPriority   = PRV_TGF_QOS_PROFILE_ACTION_CNS;
    cosProfile.trafficClass   = PRV_TGF_QOS_PROFILE_ACTION_CNS;
    cosProfile.dscp           = PRV_TGF_QOS_PROFILE_ACTION_CNS;
    cosProfile.exp            = PRV_TGF_QOS_PROFILE_ACTION_CNS;

    rc = prvTgfCosProfileEntrySet(PRV_TGF_QOS_PROFILE_ACTION_CNS, &cosProfile);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

    /* clear entry */
    cpssOsMemSet((GT_VOID*) &ruleAction, 0, sizeof(ruleAction));
    cpssOsMemSet((GT_VOID*) &pattern, 0, sizeof(pattern));
    cpssOsMemSet((GT_VOID*) &mask, 0, sizeof(mask));

    /* configure TTI rule action */
    ruleAction.tunnelTerminate     = GT_TRUE;
    ruleAction.tsPassengerPacketType = PRV_TGF_TUNNEL_PASSENGER_OTHER_E;
    ruleAction.passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV4_E;
    ruleAction.command             = CPSS_PACKET_CMD_FORWARD_E;
    ruleAction.qosProfile          = PRV_TGF_QOS_PROFILE_ACTION_CNS;
    ruleAction.redirectCommand     = PRV_TGF_TTI_NO_REDIRECT_E;
    ruleAction.tunnelStart         = GT_FALSE;
    ruleAction.tag1VlanCmd         = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;

    ruleAction.trustExp  = (CPSS_COS_PARAM_EXP_E  == cosParam)&&(enableTrust);
    ruleAction.trustDscp = (CPSS_COS_PARAM_DSCP_E == cosParam)&&(enableTrust);
    ruleAction.trustUp   = (CPSS_COS_PARAM_UP_E   == cosParam)&&(enableTrust);

    ruleAction.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    ruleAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;
    ruleAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS];

    /* configure TTI rule */
    switch (macMode)
    {
        case PRV_TGF_TTI_MAC_MODE_DA_E:
            cpssOsMemCpy((GT_VOID*)pattern.ipv4.common.mac.arEther,
                         (GT_VOID*)prvTgfPacketL2Part.daMac,
                         sizeof(pattern.ipv4.common.mac.arEther));
            break;

        case PRV_TGF_TTI_MAC_MODE_SA_E:
            cpssOsMemCpy((GT_VOID*)pattern.ipv4.common.mac.arEther,
                         (GT_VOID*)prvTgfPacketL2Part.saMac,
                         sizeof(pattern.ipv4.common.mac.arEther));
            break;

        default:
            rc = GT_BAD_PARAM;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
                    "Get invalid macMode: %d", macMode);
    }

    /* configure IPv4 TTI rule */
    pattern.ipv4.common.pclId = 1;
    pattern.ipv4.common.vid   = PRV_TGF_VLANID_5_CNS;
    cpssOsMemCpy(pattern.ipv4.destIp.arIP, prvTgfPacketIpv4Part.dstAddr,
                 sizeof(pattern.ipv4.destIp.arIP));
    cpssOsMemCpy(pattern.ipv4.srcIp.arIP, prvTgfPacketIpv4Part.srcAddr,
                 sizeof(pattern.ipv4.srcIp.arIP));

    mask.ipv4.common.pclId    = 0x3FF;
    mask.ipv4.common.vid      = 0x0F;
    cpssOsMemSet(mask.ipv4.common.mac.arEther, 0xFF,
                 sizeof(mask.ipv4.common.mac.arEther));
    cpssOsMemSet(mask.ipv4.destIp.arIP, 0xFF,
                 sizeof(mask.ipv4.destIp.arIP));
    cpssOsMemSet(mask.ipv4.srcIp.arIP, 0xFF,
                 sizeof(mask.ipv4.srcIp.arIP));

    /* set TTI rule */
    rc = prvTgfTtiRuleSet(ruleIndex, PRV_TGF_TTI_KEY_IPV4_E,
            &pattern, &mask, &ruleAction);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleSet: %d",
            prvTgfDevNum);

    /* write a ARP MAC address to the Router ARP Table */
    cpssOsMemCpy(arpMacAddr.arEther, prvTgfArpMac, sizeof(TGF_MAC_ADDR));

    rc = prvTgfIpRouterArpAddrWrite(prvTgfRouterArpIndex, &arpMacAddr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfIpRouterArpAddrWrite: %d", prvTgfDevNum);

    /* Set IPv4 prefix that will route the tunnel passenger packet */
    if (prvTgfRoutingEnable == GT_TRUE)
    {
        /* get routing mode */
        rc = prvTgfIpRoutingModeGet(&routingMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRoutingModeGet: %d", prvTgfDevNum);

        switch(routingMode)
        {
            case PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E:

                prvTgfTunnelTermPbrInit(prvTgfIpv4PrefixPtr);
                break;

            case PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E:

                prvTgfTunnelTermRoutingInit(prvTgfIpv4PrefixPtr);
                break;

            case PRV_TGF_IP_ROUTING_MODE_UNDEFINED_E:

                break;
        }
    }
}

/*******************************************************************************
* prvTgfTunnelTermTestPacketSend
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
static GT_VOID prvTgfTunnelTermTestPacketSend
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
}

/*******************************************************************************
* prvTgfTunnelTermTestReset
*
* DESCRIPTION:
*       Function clears FDB, VLAN tables and internal table of captured packets.
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
static GT_VOID prvTgfTunnelTermTestReset
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;
    GT_IPADDR   ipAddr;
    PRV_TGF_IP_ROUTING_MODE_ENT     routingMode;

    PRV_UTF_LOG0_MAC("======= Restore Configuration =======\n");

    /* get routing mode */
    rc = prvTgfIpRoutingModeGet(&routingMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRoutingModeGet: %d", prvTgfDevNum);

    /* reset QoS configuration for SEND port */
    prvTgfQosPortReset(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* reset QoS maps for pattern = 0 */
    prvTgfQosMapsReset(0);

    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    if(GT_TRUE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E))
    {
        /* disable port 0 for MIM TTI lookup */
        rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_TTI_KEY_MIM_E, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);
    }

    /* disable port 0 for Ethernet TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_TTI_KEY_ETH_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* disable port 0 for Ethernet TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_TTI_KEY_IPV4_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    if(GT_TRUE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E))
    {
        /* restore MIM EtherType */
        rc = prvTgfTtiMimEthTypeSet(0);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMimEthTypeSet: %d", prvTgfDevNum);
    }

    /* invalidate TTI rules */
    rc = prvTgfTtiRuleValidStatusSet(0, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleValidStatusSet: %d, %d", prvTgfDevNum, GT_FALSE);

    if (prvTgfRoutingEnable == GT_TRUE)
    {
        if (routingMode == PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E)
        {
            /* reset ip config */
            /* disable Unicast IPv4 Routing on a Port */
            rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], CPSS_IP_UNICAST_E,
                                           CPSS_IP_PROTOCOL_IPV4_E, GT_FALSE);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d %d",
                                         prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
        }

        /* disable IPv4 Unicast Routing on Vlan */
        rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_5_CNS, CPSS_IP_UNICAST_E,
                                       CPSS_IP_PROTOCOL_IPV4_E, GT_FALSE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                     prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

        /* delete IPv4 prefix */
        cpssOsMemCpy(ipAddr.arIP, prvTgfIpv4PrefixPtr, sizeof(ipAddr.arIP));
        rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, 0, ipAddr, 32);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);
        prvTgfRoutingEnable = GT_FALSE;
    }

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* restore default vlanId */
    rc = prvTgfBrgVlanPvidSet(prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPvidSet: %d, %d", prvTgfDevNum, prvTgfDefVlanId);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_5_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_5_CNS);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_6_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_6_CNS);

    /* remove PCL configurations */
    prvTgfPclRestore();
}


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfTunnelTermTag1VlanCommandFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - enable Ethernet TTI lookup
*         - set Ethernet TTI rule to match packets with VLAN tags 5 & 7
*           and specific MAC DA address
*         - set TTI action with the following parameters:
*              forward to egress interface
*              set Tag0 VLAN from entry with Tag0 VLAN value 6
*              set Tag1 VLAN from entry with Tag1 VLAN value 8
*         - set Egress Tag field in VLAN 6 entry to <outer Tag1, inner Tag0>
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
GT_VOID prvTgfTunnelTermTag1VlanCommandFieldConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfTunnelTermTestInit(0, PRV_TGF_TTI_MAC_MODE_DA_E);
}

/*******************************************************************************
* prvTgfTunnelTermTag1VlanCommandFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send QinQ packet with matching VLANS:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8988 (MIM Ether type)
*               iSid: 0x123456
*               iUP: 0x5
*               iDP: 0
*
*           Success Criteria:
*               Packet striped and forwarded to port 23 VLAN 6
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
GT_VOID prvTgfTunnelTermTag1VlanCommandFieldTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];
    GT_U32                  numTriggers = 0;
    GT_U8 expectedRxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};
    GT_U8 expectedTxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};

    /* reset ETH counters */
    rc = prvTgfEthCountersReset(prvTgfDevNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

    /* enable capturing of receive port */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* disable capturing of receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* check ETH counters */
    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        rc = prvTgfEthCountersCheck(
            prvTgfDevNum,
            prvTgfPortsArray[portIter],
            expectedRxPackets[portIter], expectedTxPackets[portIter],
            prvTgfPacketInfo.totalLen, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
    }

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters where packet has defined VLAN TAGS */
    cpssOsMemSet(&vfdArray[0], 0, sizeof(TGF_VFD_INFO_STC));
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);

    cpssOsMemSet(&vfdArray[1], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset = TGF_L2_HEADER_SIZE_CNS;
    vfdArray[1].cycleCount = TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[1].patternPtr[0] = 0x81;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = PRV_TGF_QOS_PROFILE_DEFAULT_CNS << 5;
    vfdArray[1].patternPtr[3] = PRV_TGF_VLANID_8_CNS;

    cpssOsMemSet(&vfdArray[2], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].offset = TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[2].cycleCount = TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[2].patternPtr[0] = 0x81;
    vfdArray[2].patternPtr[1] = 0x00;
    vfdArray[2].patternPtr[2] = PRV_TGF_QOS_PROFILE_DEFAULT_CNS << 5;
    vfdArray[2].patternPtr[3] = PRV_TGF_VLANID_6_CNS;

    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 3, vfdArray, &numTriggers);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);

    /* check if captured packet has the defined VLAN TAGS */
    UTF_VERIFY_EQUAL6_STRING_MAC(7, numTriggers,
            "\n   MAC DA of captured packet must be: %02x:%02x:%02x:%02x:%02x:%02x"
            "\n   VLAN TAGs of captured packet must be: 81 00 00 08 and 81 00 00 06\n",
            prvTgfPacketL2Part.daMac[0], prvTgfPacketL2Part.daMac[1], prvTgfPacketL2Part.daMac[2],
            prvTgfPacketL2Part.daMac[3], prvTgfPacketL2Part.daMac[4], prvTgfPacketL2Part.daMac[5]);
}

/*******************************************************************************
* prvTgfUseTunnelTermTag1VlanCommandFieldConfigRestore
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
GT_VOID prvTgfTunnelTermTag1VlanCommandFieldConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

/*******************************************************************************
* prvTgfTunnelTermTrustExpQosFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - enable MPLS TTI lookup
*         - set MPLS TTI rule to match MPLS packet with 2 labels
*         - set TTI action to terminate the tunnel and set trust EXP field to true
*         - set EXP to QoS profile mapping entry with EXP matching the EXP of
*           the outer MPLS label
*         - set IPv4 prefix that will route the tunnel passenger packet
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
GT_VOID prvTgfTunnelTermTrustExpQosFieldConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");
    prvTgfRoutingEnable = GT_TRUE;
    prvTgfIpv4PrefixPtr = prvTgfPacketIpv4Part.dstAddr;

    /* init configuration */
    prvTgfTunnelTermMplsKeyQosTestInit(0, PRV_TGF_TTI_MAC_MODE_DA_E, 2,
                                       PRV_TGF_TTI_MPLS_NOP_COMMAND_E);
}

/*******************************************************************************
* prvTgfTunnelTermTrustExpQosFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send QinQ packet with matching VLANS:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8847 (MPLS Ether type)
*
*           Success Criteria:
*               Verify that the QoS fields are set according to the QoS profile
*               assigned in the EXP to QoS table
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
GT_VOID prvTgfTunnelTermTrustExpQosFieldTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];
    GT_U32                  numTriggers = 0;
    GT_U8 expectedRxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};
    GT_U8 expectedTxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};

    /* reset ETH counters */
    rc = prvTgfEthCountersReset(prvTgfDevNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

    /* enable capturing of receive port */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_FORWARD_PORT_IDX_CNS];
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfIpv4OverMpls2PacketInfo);

    /* disable capturing of receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* check ETH counters */
    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
            /* packet after tunneling in VLAN 6 */
            packetSize = prvTgfIpv4OverMpls2PacketInfo.totalLen
                - 2 * TGF_MPLS_HEADER_SIZE_CNS;
        }
        else
        {
            /* original packet before tunneling in VLAN 5 */
            packetSize = prvTgfIpv4OverMpls2PacketInfo.totalLen;
        }

        rc = prvTgfEthCountersCheck(
            prvTgfDevNum,
            prvTgfPortsArray[portIter],
            expectedRxPackets[portIter], expectedTxPackets[portIter],
            packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
    }

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters where packet has defined VLAN TAGS */
    cpssOsMemSet(&vfdArray[0], 0, sizeof(TGF_VFD_INFO_STC));
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfArpMac, sizeof(TGF_MAC_ADDR));
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);

    cpssOsMemSet(&vfdArray[1], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset = TGF_L2_HEADER_SIZE_CNS;
    vfdArray[1].cycleCount = TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[1].patternPtr[0] = 0x81;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = PRV_TGF_QOS_PROFILE_DEFAULT_CNS << 5;
    vfdArray[1].patternPtr[3] = PRV_TGF_VLANID_6_CNS;

    cpssOsMemSet(&vfdArray[2], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].offset = TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[2].cycleCount = 4;
    vfdArray[2].patternPtr[0] = 0x08;
    vfdArray[2].patternPtr[1] = 0x00;
    vfdArray[2].patternPtr[2] = 0x45;
    vfdArray[2].patternPtr[3] = PRV_TGF_QOS_PROFILE_DEFAULT_CNS << 2;

    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 3, vfdArray, &numTriggers);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);

    /* check MACDA, UP, DSCP fields of captured packet */
    UTF_VERIFY_EQUAL6_STRING_MAC(7, numTriggers,
            "\n   MAC DA of captured packet must be: %02x:%02x:%02x:%02x:%02x:%02x"
            "\n   Field UP   in VLAN Tag    must be 1: (81 00 00 06)"
            "\n   Field DSCP in IPv4 header must be 1: (08 00 45 04)\n",
            prvTgfArpMac[0], prvTgfArpMac[1], prvTgfArpMac[2],
            prvTgfArpMac[3], prvTgfArpMac[4], prvTgfArpMac[5]);
}

/*******************************************************************************
* prvTgfTunnelTermTrustExpQosFieldConfigRestore
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
GT_VOID prvTgfTunnelTermTrustExpQosFieldConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

/*******************************************************************************
* prvTgfTunnelTrustExpQosFieldNonTermConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - set port default QoS profile
*         - set MPLS TTI rule to match MPLS packet with 3 labels
*         - set TTI action with pop 2 MPLS command
*         - set trust EXP field to true
*         - set redirect packet to egress interface
*         - set EXP to QoS profile entry
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
GT_VOID prvTgfTunnelTrustExpQosFieldNonTermConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");
    prvTgfRoutingEnable = GT_FALSE;

    /* init configuration */
    prvTgfTunnelTermMplsKeyQosTestInit(0, PRV_TGF_TTI_MAC_MODE_DA_E, 3,
                                       PRV_TGF_TTI_MPLS_POP2_LABELS_COMMAND_E);
}

/*******************************************************************************
* prvTgfTunnelTrustExpQosFieldNonTermTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send QinQ packet with matching VLANS:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8847 (MPLS Ether type)
*
*           Success Criteria:
*               Packet is forward correctly with 1 MPLS label
*               QoS fields are set according to the QoS profile matching
*               the EXP field in the last MPLS label that was not popped
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
GT_VOID prvTgfTunnelTrustExpQosFieldNonTermTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];
    GT_U32                  numTriggers = 0;
    GT_U8 expectedRxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};
    GT_U8 expectedTxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};

    /* reset ETH counters */
    rc = prvTgfEthCountersReset(prvTgfDevNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

    /* enable capturing of receive port */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfMplsPacketInfo);

    /* disable capturing of receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* check ETH counters */
    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
            /* packet after tunneling in VLAN 6 */
            packetSize = prvTgfMplsPacketInfo.totalLen
                - TGF_MPLS_HEADER_SIZE_CNS - TGF_VLAN_TAG_SIZE_CNS;
        }
        else
        {
            /* original packet before tunneling in VLAN 5 */
            packetSize = prvTgfMplsPacketInfo.totalLen;
        }

        rc = prvTgfEthCountersCheck(
            prvTgfDevNum,
            prvTgfPortsArray[portIter],
            expectedRxPackets[portIter], expectedTxPackets[portIter],
            packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
    }

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters where packet has MAC DA as prvTgfArpMac */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfArpMac, sizeof(TGF_MAC_ADDR));

    cpssOsMemSet(&vfdArray[1], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset = TGF_L2_HEADER_SIZE_CNS;
    vfdArray[1].cycleCount = TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS;
    vfdArray[1].patternPtr[0] = 0x81;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = PRV_TGF_QOS_PROFILE_DEFAULT_CNS << 5;
    vfdArray[1].patternPtr[3] = PRV_TGF_VLANID_6_CNS;
    vfdArray[1].patternPtr[4] = 0x88;
    vfdArray[1].patternPtr[5] = 0x47;

    cpssOsMemSet(&vfdArray[2], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].offset = TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS;
    vfdArray[2].cycleCount = 6;
    vfdArray[2].patternPtr[0] = 0x00;
    vfdArray[2].patternPtr[1] = 0x03;
    vfdArray[2].patternPtr[2] = 0x31;
    vfdArray[2].patternPtr[3] = 0xdd;
    vfdArray[2].patternPtr[4] = 0x45;
    vfdArray[2].patternPtr[5] = PRV_TGF_QOS_PROFILE_DEFAULT_CNS << 2;

    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 3, vfdArray, &numTriggers);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);

    /* check MACDA, UP, DSCP fields of captured packet */
    UTF_VERIFY_EQUAL6_STRING_MAC(7, numTriggers,
            "\n   MAC DA of captured packet must be: %02x:%02x:%02x:%02x:%02x:%02x"
            "\n   Field UP   in VLAN Tag    must be 1: (81 00 00 06 88 47)"
            "\n   MPLSLabel and DSCP fields must be 1: (00 03 31 dd 45 00)\n",
            prvTgfArpMac[0], prvTgfArpMac[1], prvTgfArpMac[2],
            prvTgfArpMac[3], prvTgfArpMac[4], prvTgfArpMac[5]);
}

/*******************************************************************************
* prvTgfTunnelTrustExpQosFieldNonTermConfigRestore
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
GT_VOID prvTgfTunnelTrustExpQosFieldNonTermConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

/*******************************************************************************
* prvTgfTunnelTermTrustAnyQosFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - set port default QoS profile
*         - set IPv4 TTI rule to match IPv4 tunneled packets
*         - set TTI action to terminate the tunnel, not to trust any QoS fields
*           and with default QoS profile index
*         - set IPv4 prefix that will route the tunnel passenger packet
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
GT_VOID prvTgfTunnelTermTrustAnyQosFieldConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");
    prvTgfRoutingEnable = GT_TRUE;
    prvTgfIpv4PrefixPtr = prvTgfPacketIpv4OverIpv4Part.dstAddr;

    /* init configuration */
    prvTgfTunnelTermIpv4KeyQosTestInit(0, PRV_TGF_TTI_MAC_MODE_DA_E,
                                       GT_FALSE, CPSS_COS_PARAM_DSCP_E);
}

/*******************************************************************************
* prvTgfTunnelTermTrustAnyQosFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send Ipv4-Over-Ipv4 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800 (Ipv4 Ether type)
*
*           Success Criteria:
*               Packet is forward correctly with 1 MPLS label
*               QoS fields are set according to the QoS profile matching
*               the EXP field in the last MPLS label that was not popped
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
GT_VOID prvTgfTunnelTermTrustAnyQosFieldTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];
    GT_U32                  numTriggers = 0;
    GT_U8 expectedRxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};
    GT_U8 expectedTxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};

    /* reset ETH counters */
    rc = prvTgfEthCountersReset(prvTgfDevNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

    /* enable capturing of receive port */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfIpv4OverIpv4PacketInfo);

    /* disable capturing of receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* check ETH counters */
    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
            /* packet after tunneling in VLAN 6 */
            packetSize = prvTgfIpv4OverIpv4PacketInfo.totalLen
                - TGF_IPV4_HEADER_SIZE_CNS;
            packetSize = packetSize < 60 ? 60 : packetSize;
        }
        else
        {
            /* original packet before tunneling in VLAN 5 */
            packetSize = prvTgfIpv4OverIpv4PacketInfo.totalLen;
        }

        rc = prvTgfEthCountersCheck(
            prvTgfDevNum,
            prvTgfPortsArray[portIter],
            expectedRxPackets[portIter], expectedTxPackets[portIter],
            packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
    }

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters where packet has defined VLAN TAGS */
    cpssOsMemSet(&vfdArray[0], 0, sizeof(TGF_VFD_INFO_STC));
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfArpMac, sizeof(TGF_MAC_ADDR));
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);

    cpssOsMemSet(&vfdArray[1], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset = TGF_L2_HEADER_SIZE_CNS;
    vfdArray[1].cycleCount = TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[1].patternPtr[0] = 0x81;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = PRV_TGF_QOS_PROFILE_ACTION_CNS << 5;
    vfdArray[1].patternPtr[3] = PRV_TGF_VLANID_6_CNS;

    cpssOsMemSet(&vfdArray[2], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].offset = TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[2].cycleCount = 4;
    vfdArray[2].patternPtr[0] = 0x08;
    vfdArray[2].patternPtr[1] = 0x00;
    vfdArray[2].patternPtr[2] = 0x45;
    vfdArray[2].patternPtr[3] = PRV_TGF_QOS_PROFILE_ACTION_CNS << 2;

    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 3, vfdArray, &numTriggers);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);

    /* check MACDA, UP, DSCP fields of captured packet */
    UTF_VERIFY_EQUAL6_STRING_MAC(7, numTriggers,
            "\n   MAC DA of captured packet must be: %02x:%02x:%02x:%02x:%02x:%02x"
            "\n   Field UP   in VLAN Tag    must be 1: (81 00 60 06)"
            "\n   Field DSCP in IPv4 header must be 1: (08 00 45 0C)\n",
            prvTgfArpMac[0], prvTgfArpMac[1], prvTgfArpMac[2],
            prvTgfArpMac[3], prvTgfArpMac[4], prvTgfArpMac[5]);
}

/*******************************************************************************
* prvTgfTunnelTermTrustAnyQosFieldConfigRestore
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
GT_VOID prvTgfTunnelTermTrustAnyQosFieldConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

/*******************************************************************************
* prvTgfTunnelTermTrustDscpQosFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - set port default QoS profile
*         - set IPv4 TTI rule to match IPv4 tunneled packets
*         - set TTI action to terminate the tunnel and to trust DSCP
*         - set DSCP to QoS profile mapping entry with DSCP matching the DSCP
*           of the passenger packet
*         - set IPv4 prefix that will route the tunnel passenger packet
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
GT_VOID prvTgfTunnelTermTrustDscpQosFieldConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");
    prvTgfRoutingEnable = GT_TRUE;
    prvTgfIpv4PrefixPtr = prvTgfPacketIpv4OverIpv4Part.dstAddr;

    /* init configuration */
    prvTgfTunnelTermIpv4KeyQosTestInit(0, PRV_TGF_TTI_MAC_MODE_DA_E,
                                       GT_TRUE, CPSS_COS_PARAM_DSCP_E);
}

/*******************************************************************************
* prvTgfTunnelTermTrustDscpQosFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send Ipv4-Over-Ipv4 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800 (Ipv4 Ether type)
*
*           Success Criteria:
*               Packet is forward correctly with 1 MPLS label
*               QoS fields are set according to the QoS profile matching
*               the EXP field in the last MPLS label that was not popped
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
GT_VOID prvTgfTunnelTermTrustDscpQosFieldTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];
    GT_U32                  numTriggers = 0;
    GT_U8 expectedRxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};
    GT_U8 expectedTxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};

    /* reset ETH counters */
    rc = prvTgfEthCountersReset(prvTgfDevNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

    /* enable capturing of receive port */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfIpv4OverIpv4PacketInfo);

    /* disable capturing of receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* check ETH counters */
    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
            /* packet after tunneling in VLAN 6 */
            packetSize = prvTgfIpv4OverIpv4PacketInfo.totalLen
                - TGF_IPV4_HEADER_SIZE_CNS;
            packetSize = packetSize < 60 ? 60 : packetSize;
        }
        else
        {
            /* original packet before tunneling in VLAN 5 */
            packetSize = prvTgfIpv4OverIpv4PacketInfo.totalLen;
        }

        rc = prvTgfEthCountersCheck(
            prvTgfDevNum,
            prvTgfPortsArray[portIter],
            expectedRxPackets[portIter], expectedTxPackets[portIter],
            packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
    }

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters where packet has defined VLAN TAGS */
    cpssOsMemSet(&vfdArray[0], 0, sizeof(TGF_VFD_INFO_STC));
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfArpMac, sizeof(TGF_MAC_ADDR));
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);

    cpssOsMemSet(&vfdArray[1], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset = TGF_L2_HEADER_SIZE_CNS;
    vfdArray[1].cycleCount = TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[1].patternPtr[0] = 0x81;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = PRV_TGF_QOS_PROFILE_MAP_CNS << 5;
    vfdArray[1].patternPtr[3] = PRV_TGF_VLANID_6_CNS;

    cpssOsMemSet(&vfdArray[2], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].offset = TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[2].cycleCount = 4;
    vfdArray[2].patternPtr[0] = 0x08;
    vfdArray[2].patternPtr[1] = 0x00;
    vfdArray[2].patternPtr[2] = 0x45;
    vfdArray[2].patternPtr[3] = PRV_TGF_QOS_PROFILE_MAP_CNS << 2;

    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 3, vfdArray, &numTriggers);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);

    /* check MACDA, UP, DSCP fields of captured packet */
    UTF_VERIFY_EQUAL6_STRING_MAC(7, numTriggers,
            "\n   MAC DA of captured packet must be: %02x:%02x:%02x:%02x:%02x:%02x"
            "\n   Field UP   in VLAN Tag    must be 1: (81 00 40 06)"
            "\n   Field DSCP in IPv4 header must be 1: (08 00 45 08)\n",
            prvTgfArpMac[0], prvTgfArpMac[1], prvTgfArpMac[2],
            prvTgfArpMac[3], prvTgfArpMac[4], prvTgfArpMac[5]);
}

/*******************************************************************************
* prvTgfTunnelTermTrustDscpQosFieldConfigRestore
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
GT_VOID prvTgfTunnelTermTrustDscpQosFieldConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

/*******************************************************************************
* prvTgfTunnelTermTrustUpQosFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - set port default QoS profile
*         - set IPv4 TTI rule to match IPv4 tunneled packets
*         - set TTI action to terminate the tunnel and to trust DSCP
*         - set DSCP to QoS profile mapping entry with DSCP matching the DSCP
*           of the passenger packet
*         - set IPv4 prefix that will route the tunnel passenger packet
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
GT_VOID prvTgfTunnelTermTrustUpQosFieldConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");
    prvTgfRoutingEnable = GT_TRUE;
    prvTgfIpv4PrefixPtr = prvTgfPacketIpv4OverIpv4Part.dstAddr;

    /* init configuration */
    prvTgfTunnelTermIpv4KeyQosTestInit(0, PRV_TGF_TTI_MAC_MODE_DA_E,
                                       GT_TRUE, CPSS_COS_PARAM_UP_E);
}

/*******************************************************************************
* prvTgfTunnelTermTrustUpQosFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send Ipv4-Over-Ipv4 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800 (Ipv4 Ether type)
*
*           Success Criteria:
*               Packet is forward correctly with 1 MPLS label
*               QoS fields are set according to the QoS profile matching
*               the EXP field in the last MPLS label that was not popped
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
GT_VOID prvTgfTunnelTermTrustUpQosFieldTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];
    GT_U32                  numTriggers = 0;
    GT_U8 expectedRxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};
    GT_U8 expectedTxPackets[PRV_TGF_PORTS_NUM_CNS] = {1,  0,  0,  1};

    /* reset ETH counters */
    rc = prvTgfEthCountersReset(prvTgfDevNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

    /* enable capturing of receive port */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfIpv4OverIpv4PacketInfo);

    /* disable capturing of receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface,
            TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* check ETH counters */
    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
            /* packet after tunneling in VLAN 6 */
            packetSize = prvTgfIpv4OverIpv4PacketInfo.totalLen
                - TGF_IPV4_HEADER_SIZE_CNS;
            packetSize = packetSize < 60 ? 60 : packetSize;
        }
        else
        {
            /* original packet before tunneling in VLAN 5 */
            packetSize = prvTgfIpv4OverIpv4PacketInfo.totalLen;
        }

        rc = prvTgfEthCountersCheck(
            prvTgfDevNum,
            prvTgfPortsArray[portIter],
            expectedRxPackets[portIter], expectedTxPackets[portIter],
            packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
    }

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters where packet has defined VLAN TAGS */
    cpssOsMemSet(&vfdArray[0], 0, sizeof(TGF_VFD_INFO_STC));
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfArpMac, sizeof(TGF_MAC_ADDR));
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);

    cpssOsMemSet(&vfdArray[1], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset = TGF_L2_HEADER_SIZE_CNS;
    vfdArray[1].cycleCount = TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[1].patternPtr[0] = 0x81;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = PRV_TGF_QOS_PROFILE_ACTION_CNS << 5;
    vfdArray[1].patternPtr[3] = PRV_TGF_VLANID_6_CNS;

    cpssOsMemSet(&vfdArray[2], 0, sizeof(TGF_VFD_INFO_STC));
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].offset = TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[2].cycleCount = 4;
    vfdArray[2].patternPtr[0] = 0x08;
    vfdArray[2].patternPtr[1] = 0x00;
    vfdArray[2].patternPtr[2] = 0x45;
    vfdArray[2].patternPtr[3] = PRV_TGF_QOS_PROFILE_ACTION_CNS << 2;

    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 3, vfdArray, &numTriggers);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);

    /* check MACDA, UP, DSCP fields of captured packet */
    UTF_VERIFY_EQUAL6_STRING_MAC(7, numTriggers,
            "\n   MAC DA of captured packet must be: %02x:%02x:%02x:%02x:%02x:%02x"
            "\n   Field UP   in VLAN Tag    must be 1: (81 00 60 06)"
            "\n   Field DSCP in IPv4 header must be 1: (08 00 45 0C)\n",
            prvTgfArpMac[0], prvTgfArpMac[1], prvTgfArpMac[2],
            prvTgfArpMac[3], prvTgfArpMac[4], prvTgfArpMac[5]);
}

/*******************************************************************************
* prvTgfTunnelTermTrustUpQosFieldConfigRestore
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
GT_VOID prvTgfTunnelTermTrustUpQosFieldConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

