/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfCutThrough.c
*
* DESCRIPTION:
*       CPSS Cut-Through
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfPortGen.h>
#include <common/tgfIpGen.h>
#include <common/tgfCutThrough.h>
#include <cutThrough/prvTgfCutTrough.h>



/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS                5

/* nextHop VLAN Id */
#define PRV_TGF_NEXTHOPE_VLANID_CNS       6

/* number of ports */
#define PRV_TGF_PORT_COUNT_CNS            4

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS         0

/* another port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS 1

/* nextHop port number to receive traffic from */
#define PRV_TGF_NEXTHOPE_PORT_IDX_CNS     3

/* UP in packet */
#define PRV_TGF_UP_CNS                    0

/* UP in packet */
#define PRV_TGF_MRU_INDEX_CNS             0

/* UP in packet */
#define PRV_TGF_MRU_INDEX_2_CNS           1

/* DSA tag size */
#define PRV_TGF_DSA_TAG_SIZE_CNS          8

/* number of packets to send */
#define PRV_TGF_SEND_PACKETS_NUM_CNS      2

/* default packet length for Cut-Through mode */
#define PRV_TGF_DEF_PACKET_LEN_CNS        0x3FFF

/* default MRU value */
#define PRV_TGF_DEF_MRU_VALUE_CNS         0x5F2

/* maximal MRU value */
#define PRV_TGF_MAX_MRU_VALUE_CNS         0xFFFF

/* inlif index */
#define PRV_TGF_INLIF_INDEX_CNS           0x1000

/* default number of packets to send */
static GT_U32  prvTgfBurstCount = 1;

/* the Arp Address index of the Router ARP Table */
static GT_U32        prvTgfRouterArpIndex      = 1;

/* the Route entry index for UC Route entry Table */
static GT_U32        prvTgfRouteEntryBaseIndex = 3;

/* the LPM DB id for LPM Table */
static GT_U32        prvTgfLpmDBId             = 0;

/* the virtual router id */
static GT_U32        prvTgfVrId                = 0;

/* the ARP MAC address to write to the Router ARP Table */
static TGF_MAC_ADDR  prvTgfArpMac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};

/* received port indexes array */
static GT_U8 prvTgfRcvPortsIdxArr[PRV_TGF_SEND_PACKETS_NUM_CNS] =
{
    PRV_TGF_NEXTHOPE_PORT_IDX_CNS, PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS
};

/* expected number of packets on MC subscriber's ports */
static GT_U8 prvTgfPacketsCountRxTxArr[][PRV_TGF_PORTS_NUM_CNS] =
{
    {1, 0, 0, 1},
    {1, 1, 0, 0},
    {1, 1, 1, 1},
    {1, 0, 0, 0}
};

/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},          /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}           /* saMac */
};

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,               /* etherType */
    0, 0, PRV_TGF_VLANID_CNS                       /* pri, cfi, VlanId */
};

/* packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

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
    { 1,  1,  1,  1},                  /* srcAddr */
    { 1,  1,  1,  3}                   /* dstAddr */
};

/* DATA of packet */
static GT_U8 prvTgfPayloadDataArr[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x55
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart =
{
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfIpv4PacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of packet */
#define PRV_TGF_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfo =
{
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArray                                        /* partsArray */
};

/* LENGTH of IPv4 packet */
#define PRV_TGF_IPV4_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS +  \
    TGF_IPV4_HEADER_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of IPv4 packet with CRC */
#define PRV_TGF_IPV4_PACKET_CRC_LEN_CNS PRV_TGF_IPV4_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* IPv4 PACKET to send */
static TGF_PACKET_STC prvTgfIpv4PacketInfo =
{
    PRV_TGF_IPV4_PACKET_LEN_CNS,                                     /* totalLen */
    sizeof(prvTgfIpv4PacketPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfIpv4PacketPartArray                                        /* partsArray */
};


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfCutThroughBypassRouterAndPolicerBaseConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Create 2 Vlan entries -- (port[0], port[1]) and (port[2], port[3]);
*         - Enable Cut-Through;
*         - Set MRU value for a VLAN MRU profile;
*         - Create FDB entry.
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
GT_VOID prvTgfCutThroughBypassRouterAndPolicerBaseConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                   rc = GT_OK;
    PRV_TGF_BRG_MAC_ENTRY_STC   macEntry;


    /* create SEND VLAN */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_CNS, prvTgfPortsArray,
                                           NULL, NULL, 2);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);

    /* create a NEXTHOP VLAN */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_NEXTHOPE_VLANID_CNS, prvTgfPortsArray + 2,
                                           NULL, NULL, 2);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_NEXTHOPE_VLANID_CNS);

    /* Enable Cut Through forwarding for packets received on the port. */
    rc = prvTgfCutThroughPortEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE, GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCutThroughPortEnableSet: %d%d%d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE, GT_TRUE);

    /* Enable tagged packets with the specified UP to be Cut Through. */
    rc = prvTgfCutThroughUpEnableSet(PRV_TGF_UP_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCutThroughUpEnableSet: %d%d",
                                 prvTgfDevNum, GT_TRUE);

    /* Set maximal value for a VLAN MRU profile. */
    rc = prvTgfBrgVlanMruProfileValueSet(PRV_TGF_MRU_INDEX_CNS, PRV_TGF_MAX_MRU_VALUE_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMruProfileValueSet: %d%d",
                                 PRV_TGF_MRU_INDEX_CNS, PRV_TGF_MAX_MRU_VALUE_CNS);

    /* create a macEntry with .daRoute = GT_TRUE */
    cpssOsMemSet(&macEntry, 0, sizeof(macEntry));
    cpssOsMemCpy(macEntry.key.key.macVlan.macAddr.arEther,
                 prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));

    macEntry.key.entryType                = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.vlanId       = PRV_TGF_VLANID_CNS;
    macEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.devNum  = prvTgfDevNum;
    macEntry.dstInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS];
    macEntry.isStatic                     = GT_TRUE;
    macEntry.daRoute                      = GT_TRUE;

    rc = prvTgfBrgFdbMacEntrySet(&macEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntryWrite: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfCutThroughLttConfigurationSet
*
* DESCRIPTION:
*       Set LTT Route Configuration
*
* INPUTS:
*       sendPortNum    - port sending traffic
*       nextHopPortNum - port receiving traffic
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
static GT_VOID prvTgfCutThroughLttConfigurationSet
(
    GT_U8    sendPortNum,
    GT_U8    nextHopPortNum
)
{
    GT_STATUS                               rc = GT_OK;
    PRV_TGF_INLIF_ENTRY_STC                 inlifEntry;
    GT_ETHERADDR                            arpMacAddr;
    PRV_TGF_IP_UC_ROUTE_ENTRY_STC           routeEntriesArray[1];
    PRV_TGF_IP_UC_ROUTE_ENTRY_STC           *regularEntryPtr;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_IPADDR                               ipAddr;


    PRV_UTF_LOG0_MAC("==== Setting LTT Route Configuration ====\n");

    /* clear Inlif Entry */
    cpssOsMemSet(&inlifEntry, 0, sizeof(inlifEntry));

    /* fill Inlif Entry */
    inlifEntry.bridgeEnable              = GT_TRUE;
    inlifEntry.autoLearnEnable           = GT_TRUE;
    inlifEntry.naMessageToCpuEnable      = GT_TRUE;
    inlifEntry.unkSaUcCommand            = CPSS_PACKET_CMD_BRIDGE_E;
    inlifEntry.unkSaNotSecurBreachEnable = GT_TRUE;
    inlifEntry.ipv4UcRouteEnable         = GT_TRUE;
    
    /* set Inlif Entry */
    rc = prvTgfInlifEntrySet(PRV_TGF_INLIF_TYPE_EXTERNAL_E, PRV_TGF_INLIF_INDEX_CNS, &inlifEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfInlifEntrySet: %d", prvTgfDevNum);

    /* enable Unicast IPv4 Routing on a Port */
    rc = prvTgfIpPortRoutingEnable(sendPortNum, CPSS_IP_UNICAST_E,
                                   CPSS_IP_PROTOCOL_IPV4_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d %d",
                                 prvTgfDevNum, sendPortNum);

    /* enable IPv4 Unicast Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_CNS, CPSS_IP_UNICAST_E,
                                   CPSS_IP_PROTOCOL_IPV4_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                 prvTgfDevNum, sendPortNum);

    /* write a ARP MAC address to the Router ARP Table */
    cpssOsMemCpy(arpMacAddr.arEther, prvTgfArpMac, sizeof(TGF_MAC_ADDR));

    rc = prvTgfIpRouterArpAddrWrite(prvTgfRouterArpIndex, &arpMacAddr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRouterArpAddrWrite: %d", prvTgfDevNum);

    /* write a UC Route entry to the Route Table */
    cpssOsMemSet(routeEntriesArray, 0, sizeof(routeEntriesArray));

    regularEntryPtr = &routeEntriesArray[0];
    regularEntryPtr->cmd                        = CPSS_PACKET_CMD_ROUTE_E;
    regularEntryPtr->nextHopVlanId              = PRV_TGF_NEXTHOPE_VLANID_CNS;
    regularEntryPtr->nextHopInterface.type      = CPSS_INTERFACE_PORT_E;
    regularEntryPtr->nextHopInterface.devPort.devNum  = prvTgfDevNum;
    regularEntryPtr->nextHopInterface.devPort.portNum = nextHopPortNum;
    regularEntryPtr->nextHopARPPointer          = prvTgfRouterArpIndex;
    
    rc = prvTgfIpUcRouteEntriesWrite(prvTgfRouteEntryBaseIndex, routeEntriesArray, 1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesWrite: %d", prvTgfDevNum);

    /* fill a nexthope info for the prefix */
    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = prvTgfRouteEntryBaseIndex;

    /* fill a destination IP address for the prefix */
    cpssOsMemCpy(ipAddr.arIP, prvTgfPacketIpv4Part.dstAddr, sizeof(ipAddr.arIP));

    /* create a new Ipv4 prefix in a Virtual Router for the specified LPM DB */
    rc = prvTgfIpLpmIpv4UcPrefixAdd(prvTgfLpmDBId, prvTgfVrId, ipAddr, 32, &nextHopInfo, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixAdd: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfCutThroughPbrConfigurationSet
*
* DESCRIPTION:
*       Set PBR Configuration
*
* INPUTS:
*       sendPortNum     - port sending traffic
*       nextHopPortNum  - port receiving traffic
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
static GT_VOID prvTgfCutThroughPbrConfigurationSet
(
    GT_U8    sendPortNum,
    GT_U8    nextHopPortNum
)
{
    GT_STATUS                               rc = GT_OK;
    GT_ETHERADDR                            arpMacAddr;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_IPADDR                               ipAddr;


    PRV_UTF_LOG0_MAC("==== Setting PBR Configuration ====\n");

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInitExt1(sendPortNum,
                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                  CPSS_PCL_LOOKUP_1_E, /* PBR must be in lookup 1 ! */
                                  PRV_TGF_PCL_PBR_ID_FOR_VIRTUAL_ROUTER_CNS, /* pclId */
                                  PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E /*nonIpKey*/,
                                  PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E /*ipv4Key*/,
                                  PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E /*ipv6Key*/);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclDefPortInitExt1: %d", prvTgfDevNum);

    /* enable IPv4 Unicast Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_CNS, CPSS_IP_UNICAST_E,
                                   CPSS_IP_PROTOCOL_IPV4_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                 prvTgfDevNum, sendPortNum);

    /* write ARP MAC address to the Router ARP Table */
    cpssOsMemCpy(arpMacAddr.arEther, prvTgfArpMac, sizeof(TGF_MAC_ADDR));

    rc = prvTgfIpRouterArpAddrWrite(prvTgfRouterArpIndex, &arpMacAddr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRouterArpAddrWrite: %d", prvTgfDevNum);

    /* fill a nexthope info for the prefix */
    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));

    nextHopInfo.pclIpUcAction.pktCmd                        = CPSS_PACKET_CMD_FORWARD_E;
    nextHopInfo.pclIpUcAction.mirror.cpuCode                = CPSS_NET_CONTROL_E;
    nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount = GT_TRUE;
    nextHopInfo.pclIpUcAction.vlan.vlanId                   = PRV_TGF_NEXTHOPE_VLANID_CNS;
    nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute         = GT_TRUE;
    nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex          = prvTgfRouterArpIndex;

    nextHopInfo.pclIpUcAction.redirect.redirectCmd          = PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
    nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.type            = CPSS_INTERFACE_PORT_E;
    nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.devNum  = prvTgfDevNum;
    nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum = nextHopPortNum;

    /* fill a destination IP address for the prefix */
    cpssOsMemCpy(ipAddr.arIP, prvTgfPacketIpv4Part.dstAddr, sizeof(ipAddr.arIP));

    /* call CPSS function */
    rc = prvTgfIpLpmIpv4UcPrefixAdd(prvTgfLpmDBId, prvTgfVrId, ipAddr, 32, &nextHopInfo, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixAdd: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfCutThroughBypassRouterAndPolicerRouteConfigurationSet
*
* DESCRIPTION:
*       Set Route Configuration
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
GT_VOID prvTgfCutThroughBypassRouterAndPolicerRouteConfigurationSet
(
    GT_VOID
)
{
    PRV_TGF_IP_ROUTING_MODE_ENT routingMode;
    GT_STATUS                   rc = GT_OK;

    /* get routing mode */
    rc = prvTgfIpRoutingModeGet(&routingMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRoutingModeGet: %d", prvTgfDevNum);

    /* set config for PBR or LTT routing mode */
    switch(routingMode)
    {
        case PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E:
            /* set PBR configuration */
            prvTgfCutThroughPbrConfigurationSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                                prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_CNS]);
            break;

        case PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E:
            /* set LTT Route configuration */
            prvTgfCutThroughLttConfigurationSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                                prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_CNS]);
            break;

        default:
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, GT_BAD_PARAM, "Invalid routing mode: %d", routingMode);
    }
}

/*******************************************************************************
* prvTgfCutThroughBypassRouterAndPolicerTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           - Send traffic;
*           - Check routing mechanism is working;
*           - Enable bypassing the Router and Ingress Policer engines;
*           - Send traffic;
*           - Check that routing mechanism is ignored.
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
GT_VOID prvTgfCutThroughBypassRouterAndPolicerTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS   rc         = GT_OK;
    GT_U32      portIter   = 0;
    GT_U32      packetIter = 0;
    GT_U32      packetLen  = 0;
    

    /* iterate trough sent packets */
    for (packetIter = 0; packetIter < PRV_TGF_SEND_PACKETS_NUM_CNS; packetIter++)
    {
        /* -------------------------------------------------------------------------
         * 1. Setup counters and enable capturing
         */
        for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
        {
            /* reset ethernet counters */
            rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                         prvTgfDevNum, prvTgfPortsArray[portIter]);
        }
    
        /* clear capturing RxPcktTable */
        rc = tgfTrafficTableRxPcktTblClear();
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");
    
        /* -------------------------------------------------------------------------
         * 2. Generating Traffic
         */
        rc = prvTgfTransmitPacketsWithCapture(prvTgfDevNum,
                                              prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                              &prvTgfIpv4PacketInfo, prvTgfBurstCount, 0, NULL,
                                              prvTgfDevNum,
                                              prvTgfPortsArray[prvTgfRcvPortsIdxArr[packetIter]],
                                              TGF_CAPTURE_MODE_MIRRORING_E, 10);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of prvTgfTransmitPacketsWithCapture: %d, &d\n",
                                     prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    
        /* -------------------------------------------------------------------------
         * 3. Get Ethernet Counters
         */
    
        /* read and check ethernet counters */
        for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
        {
            /* calculate packet length */
            packetLen = prvTgfIpv4PacketInfo.totalLen - TGF_VLAN_TAG_SIZE_CNS * (portIter != PRV_TGF_SEND_PORT_IDX_CNS);
    
            /* ckeck ETH counters */
            rc = prvTgfEthCountersCheck(prvTgfDevNum,
                                        prvTgfPortsArray[portIter],
                                        (GT_U8) ((portIter == PRV_TGF_SEND_PORT_IDX_CNS) || 
                                                 (portIter == prvTgfRcvPortsIdxArr[packetIter])),
                                        prvTgfPacketsCountRxTxArr[packetIter][portIter],
                                        packetLen, prvTgfBurstCount);
            UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfEthCountersCheck");
        }
    
        /* print captured packet */
        rc = prvTgfPortCapturedPacketPrint(prvTgfDevNum, prvTgfPortsArray[prvTgfRcvPortsIdxArr[packetIter]]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortCapturedPacketPrint: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[prvTgfRcvPortsIdxArr[packetIter]]);
    
        /*  Enable bypassing the Router and Ingress Policer engines. */
        rc = prvTgfCutThroughBypassRouterAndPolicerEnableSet(GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCutThroughBypassRouterAndPolicerEnableSet: %d",
                                     GT_TRUE);
    }
}

/*******************************************************************************
* prvTgfCutThroughBypassRouterAndPolicerConfigurationRestore
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
GT_VOID prvTgfCutThroughBypassRouterAndPolicerConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;
    GT_IPADDR   ipAddr;

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", prvTgfDevNum);

    /* fill the destination IP address for Ipv4 prefix in Virtual Router */
    cpssOsMemCpy(ipAddr.arIP, prvTgfPacketIpv4Part.dstAddr, sizeof(ipAddr.arIP));

    /* delete the Ipv4 prefix */
    rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, prvTgfVrId, ipAddr, 32);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);

    /* disable Unicast IPv4 Routing on a Port */
    rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                   CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* disable IPv4 Unicast Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_CNS, CPSS_IP_UNICAST_E,
                                   CPSS_IP_PROTOCOL_IPV4_E, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);

    /*  Disable bypassing the Router and Ingress Policer engines. */
    rc = prvTgfCutThroughBypassRouterAndPolicerEnableSet(GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCutThroughBypassRouterAndPolicerEnableSet: %d",
                                 GT_FALSE);

    /* Disable Cut Through forwarding for packets received on the port. */
    rc = prvTgfCutThroughPortEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCutThroughPortEnableSet: %d%d%d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, GT_FALSE);

    /* Disable tagged packets with the specified UP to be Cut Through. */
    rc = prvTgfCutThroughUpEnableSet(PRV_TGF_UP_CNS, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCutThroughUpEnableSet: %d%d",
                                 prvTgfDevNum, GT_FALSE);

    /* restore MRU value for a VLAN MRU profile. */
    rc = prvTgfBrgVlanMruProfileValueSet(PRV_TGF_MRU_INDEX_CNS, 0x5F2);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMruProfileValueSet: %d%d",
                                 PRV_TGF_MRU_INDEX_CNS, PRV_TGF_DEF_MRU_VALUE_CNS);

    /* invalidate default VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);

    /* invalidate nexthop VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_NEXTHOPE_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_NEXTHOPE_VLANID_CNS);
}

/*******************************************************************************
* prvTgfCutThroughDefaultPacketLengthConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Create Vlan entry with all ports;
*         - Enable Cut-Through;
*         - Set MRU profile index for a VLAN;
*         - Set MRU value for a VLAN MRU profile;
*         - Set Default packet length to for Cut-Through mode.
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
GT_VOID prvTgfCutThroughDefaultPacketLengthConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    /* set default VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* Enable Cut Through forwarding for packets received on the port. */
    rc = prvTgfCutThroughPortEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE, GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCutThroughPortEnableSet: %d%d%d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE, GT_TRUE);

    /* Enable tagged packets with the specified UP to be Cut Through. */
    rc = prvTgfCutThroughUpEnableSet(PRV_TGF_UP_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCutThroughUpEnableSet: %d%d",
                                 prvTgfDevNum, GT_TRUE);

    /* Set Maximum Receive Unit MRU profile index for a VLAN. */
    rc = prvTgfBrgVlanMruProfileIdxSet(PRV_TGF_VLANID_CNS, PRV_TGF_MRU_INDEX_2_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMruProfileIdxSet: %d%d",
                                 PRV_TGF_VLANID_CNS, PRV_TGF_MRU_INDEX_2_CNS);

    /* Set MRU value for a VLAN MRU profile. */
    rc = prvTgfBrgVlanMruProfileValueSet(PRV_TGF_MRU_INDEX_2_CNS, 0x3C);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMruProfileValueSet: %d%d",
                                 PRV_TGF_MRU_INDEX_CNS, PRV_TGF_PACKET_CRC_LEN_CNS + PRV_TGF_DSA_TAG_SIZE_CNS);

    /* Set default packet length for Cut-Through mode. */
    rc = prvTgfCutThroughDefaultPacketLengthSet(0x3C);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Error in prvTgfCutThroughDefaultPacketLengthSet %d",
                                 PRV_TGF_PACKET_CRC_LEN_CNS);
}

/*******************************************************************************
* prvTgfCutThroughDefaultPacketLengthTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*         - Send packet;
*         - Check that all ports received the packet;
*         - Set Default packet length to be > MRU value;
*         - Send packet;
*         - Check that no port received the packet.
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
GT_VOID prvTgfCutThroughDefaultPacketLengthTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS   rc         = GT_OK;
    GT_U32      portIter   = 0;
    GT_U32      packetIter = 0;
    GT_U32      packetLen  = 0;


    /* iterate trough sent packets */
    for (packetIter = 0; packetIter < PRV_TGF_SEND_PACKETS_NUM_CNS; packetIter++)
    {
        /* -------------------------------------------------------------------------
         * 1. Setup counters
         */
        for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
        {
            /* reset ethernet counters */
            rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                         prvTgfDevNum, prvTgfPortsArray[portIter]);
        }

        /* -------------------------------------------------------------------------
         * 2. Generating Traffic
         */

        /* setup Packet */
        rc = prvTgfSetTxSetupEth(prvTgfDevNum, &prvTgfPacketInfo, prvTgfBurstCount, 0, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d", prvTgfDevNum);

        /* send Packet */
        rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, &d",
                                     prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
        
        /* -------------------------------------------------------------------------
         * 3. Get Ethernet Counters
         */
    
        /* read and check ethernet counters */
        for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
        {
            /* calculate packet length */
            packetLen = prvTgfPacketInfo.totalLen - TGF_VLAN_TAG_SIZE_CNS * (portIter != PRV_TGF_SEND_PORT_IDX_CNS);
    
            /* ckeck ETH counters */
            rc = prvTgfEthCountersCheck(prvTgfDevNum,
                                        prvTgfPortsArray[portIter],
                                        (GT_U8) (portIter == PRV_TGF_SEND_PORT_IDX_CNS),
                                        prvTgfPacketsCountRxTxArr[PRV_TGF_SEND_PACKETS_NUM_CNS + packetIter][portIter],
                                        packetLen, prvTgfBurstCount);
            UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfEthCountersCheck");
        }
    
        /* Set default packet length for Cut-Through mode to be bigger then MRU value */
       rc = prvTgfCutThroughDefaultPacketLengthSet(0x4C);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Error in prvTgfCutThroughDefaultPacketLengthSet %d",
                                     0x4C);
    }
}

/*******************************************************************************
* prvTgfCutThroughDefaultPacketLengthConfigurationRestore
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
GT_VOID prvTgfCutThroughDefaultPacketLengthConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;


    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* restore default packet length for Cut-Through mode */
    rc = prvTgfCutThroughDefaultPacketLengthSet(PRV_TGF_DEF_PACKET_LEN_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Error in prvTgfCutThroughDefaultPacketLengthSet %d",
                                 PRV_TGF_DEF_PACKET_LEN_CNS);

    /* Set Maximum Receive Unit MRU profile index for a VLAN */
    rc = prvTgfBrgVlanMruProfileIdxSet(PRV_TGF_VLANID_CNS, PRV_TGF_MRU_INDEX_2_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMruProfileIdxSet: %d%d",
                                 PRV_TGF_VLANID_CNS, PRV_TGF_MRU_INDEX_2_CNS);

    /* restore MRU value for a VLAN MRU profile */
    rc = prvTgfBrgVlanMruProfileValueSet(PRV_TGF_MRU_INDEX_2_CNS, PRV_TGF_DEF_MRU_VALUE_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMruProfileValueSet: %d%d",
                                 PRV_TGF_MRU_INDEX_CNS, PRV_TGF_DEF_MRU_VALUE_CNS);

    /* Disable Cut Through forwarding for packets received on the port */
    rc = prvTgfCutThroughPortEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCutThroughPortEnableSet: %d%d%d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, GT_FALSE);

    /* Disable tagged packets with the specified UP to be Cut Through */
    rc = prvTgfCutThroughUpEnableSet(PRV_TGF_UP_CNS, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCutThroughUpEnableSet: %d%d",
                                 prvTgfDevNum, GT_FALSE);

    /* invalidate vlan entry */
    rc = prvTgfBrgVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);
}

