/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfDynamicPartitionIpUcRouting.c
*
* DESCRIPTION:
*       IP LPM Engine
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfIpGen.h>
#include <ip/prvTgfBasicIpv4UcRouting.h>



/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* default VLAN Id */
#define PRV_TGF_SEND_VLANID_CNS           5

/* nextHop VLAN Id */
#define PRV_TGF_NEXTHOPE_VLANID_CNS       6

/* number of ports */
#define PRV_TGF_PORT_COUNT_CNS            4

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS         0

/* port number to send traffic to */
#define PRV_TGF_FDB_PORT_IDX_CNS          1

/* port number to send traffic to */
#define PRV_TGF_NEXTHOPE_PORT_IDX_18_CNS  2

/* nextHop port number to receive traffic from */
#define PRV_TGF_NEXTHOPE_PORT_IDX_23_CNS  3

/* the counter set for a route entry is linked to */
#define PRV_TGF_COUNT_SET_CNS     CPSS_IP_CNT_SET0_E

/* default number of packets to send */
static GT_U32        prvTgfBurstCount   = 1;

/* VLANs array */
static GT_U8         prvTgfVlanArray[] = {PRV_TGF_SEND_VLANID_CNS, PRV_TGF_NEXTHOPE_VLANID_CNS};

/* VLANs array */
static GT_U8         prvTgfVlanPerPortArray[PRV_TGF_PORT_COUNT_CNS] = {
    PRV_TGF_SEND_VLANID_CNS, PRV_TGF_SEND_VLANID_CNS,
    PRV_TGF_NEXTHOPE_VLANID_CNS, PRV_TGF_NEXTHOPE_VLANID_CNS
};


/* the LPM DB id for LPM Table */
static GT_U32        prvTgfLpmDBId             = 0;

/* virtual router id */
static GT_U32        vrId                      = 0;

/******************************* Test packet **********************************/


/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},               /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}                /* saMac */
};
/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_SEND_VLANID_CNS                       /* pri, cfi, VlanId */
};
/* packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypeIpv4Part = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypeIpv6Part = {TGF_ETHERTYPE_86DD_IPV6_TAG_CNS};

static TGF_IPV4_ADDR  dstAddr = {1,1,1,1};


/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4Part = {
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2A,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    0x04,               /* protocol */
    0x76C6,             /* csum */
    { 1,  1,  1,  1},   /* srcAddr */
    { 1,  1,  1,  8}    /* dstAddr */
};

/* packet's IPv6 */
static TGF_PACKET_IPV6_STC prvTgfPacketIpv6Part = {
    6,                                                            /* version */
    0,                                                            /* trafficClass */
    0,                                                            /* flowLabel */
    0x1a,                                                         /* payloadLen */
    0x3b,                                                         /* nextHeader */
    0x40,                                                         /* hopLimit */
    { 0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111},   /* srcAddr */
    { 0x2001,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1234}    /* dstAddr */
};


/* DATA of packet */
static GT_U8 prvTgfPayloadDataArr[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart = {
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketIpv4PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypeIpv4Part},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketIpv6PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypeIpv6Part},
    {TGF_PACKET_PART_IPV6_E,      &prvTgfPacketIpv6Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*================================================================================*/
/*====================tgfDynamicPartitionIpLpmPushUpUcRouting=====================*/
/*================================================================================*/

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushUpUcBaseConfigurationSet
*
* DESCRIPTION:
*       Set Base Configuration
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
GT_VOID prvTgfDynamicPartitionIpLpmPushUpUcBaseConfigurationSet
(
    GT_VOID
)
{
    GT_BOOL                     isTagged  = GT_TRUE;
    GT_U32                      vlanIter  = 0;
    GT_U32                      portIter  = 0;
    GT_U32                      vlanCount = sizeof(prvTgfVlanArray);
    GT_STATUS                   rc        = GT_OK;
    CPSS_PORTS_BMP_STC          portsMembers = {{0, 0}};
    CPSS_PORTS_BMP_STC          portsTagging = {{0, 0}};
    PRV_TGF_BRG_VLAN_INFO_STC   vlanInfo;
    PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;

    PRV_UTF_LOG0_MAC("======= Setting Base Configuration =======\n");

    /* set vlan entry */
    cpssOsMemSet(&vlanInfo, 0, sizeof(vlanInfo));
    cpssOsMemSet(&portsTaggingCmd, 0, sizeof(portsTaggingCmd));

    vlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
    vlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.ipv4IgmpToCpuEn      = GT_FALSE;
    vlanInfo.mirrToRxAnalyzerEn   = GT_FALSE;
    vlanInfo.ipv6IcmpToCpuEn      = GT_FALSE;
    vlanInfo.ipCtrlToCpuEn        = PRV_TGF_BRG_IP_CTRL_NONE_E;
    vlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv4IpmBrgEn         = GT_FALSE;
    vlanInfo.ipv6IpmBrgEn         = GT_FALSE;
    vlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
    vlanInfo.ipv4UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv4McastRouteEn     = GT_FALSE;
    vlanInfo.ipv6UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv6McastRouteEn     = GT_FALSE;
    vlanInfo.stgId                = 0;
    vlanInfo.autoLearnDisable     = GT_TRUE;
    vlanInfo.naMsgToCpuEn         = GT_TRUE;
    vlanInfo.mruIdx               = 0;
    vlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;

    /* create vlans */
    for (vlanIter = 0; vlanIter < vlanCount; vlanIter++)
    {
        rc = prvTgfBrgVlanEntryWrite(prvTgfDevNum, prvTgfVlanArray[vlanIter],
                                     &portsMembers, &portsTagging,
                                     &vlanInfo, &portsTaggingCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryWrite: %d, %d",
                                     prvTgfDevNum, prvTgfVlanArray[vlanIter]);
    }

    /* add ports to vlan member */
    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
    {
        rc = prvTgfBrgVlanMemberAdd(prvTgfDevNum, prvTgfVlanPerPortArray[portIter],
                                    prvTgfPortsArray[portIter], isTagged);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMemberAdd: %d, %d, %d, %d",
                                     prvTgfDevNum, prvTgfVlanPerPortArray[portIter],
                                     prvTgfPortsArray[portIter], isTagged);
    }

    /* enable VLAN based MAC learning */
    rc = prvTgfBrgVlanLearningStateSet(PRV_TGF_SEND_VLANID_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanLearningStateSet: %d", prvTgfDevNum);
}



/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushUpUcRouteConfigurationSet
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
GT_VOID prvTgfDynamicPartitionIpLpmPushUpUcRouteConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                               rc = GT_OK;
    PRV_TGF_INLIF_ENTRY_STC                 inlifEntry;
    GT_ETHERADDR                            arpMacAddr;
    PRV_TGF_BRG_MAC_ENTRY_STC               macEntry;
    PRV_TGF_IP_UC_ROUTE_ENTRY_STC           routeEntriesArray;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_IPADDR                               ipv4Addr = {0};
    GT_IPV6ADDR                             ipv6Addr = {{0}};
    GT_U8                                   i;
    GT_U32                                  prefixLen;

    PRV_UTF_LOG0_MAC("======= Setting Route Configuration =======\n");

    /* -------------------------------------------------------------------------
     * 0. Set Inlif Entry - that needs for Puma devices
     */

    /* Bridge Section */
    inlifEntry.bridgeEnable              = GT_TRUE;
    inlifEntry.autoLearnEnable           = GT_TRUE;
    inlifEntry.naMessageToCpuEnable      = GT_TRUE;
    inlifEntry.naStormPreventionEnable   = GT_FALSE;
    inlifEntry.unkSaUcCommand            = CPSS_PACKET_CMD_BRIDGE_E;
    inlifEntry.unkDaUcCommand            = CPSS_PACKET_CMD_FORWARD_E;
    inlifEntry.unkSaNotSecurBreachEnable = GT_TRUE;
    inlifEntry.untaggedMruIndex          = 0;
    inlifEntry.unregNonIpMcCommand       = CPSS_PACKET_CMD_FORWARD_E;
    inlifEntry.unregIpMcCommand          = CPSS_PACKET_CMD_FORWARD_E;
    inlifEntry.unregIpv4BcCommand        = CPSS_PACKET_CMD_FORWARD_E;
    inlifEntry.unregNonIpv4BcCommand     = CPSS_PACKET_CMD_FORWARD_E;

    /* Router Section */
    inlifEntry.ipv4UcRouteEnable = GT_TRUE;
    inlifEntry.ipv4McRouteEnable = GT_FALSE;
    inlifEntry.ipv6UcRouteEnable = GT_TRUE;
    inlifEntry.ipv6McRouteEnable = GT_FALSE;
    inlifEntry.mplsRouteEnable   = GT_FALSE;
    inlifEntry.vrfId             = 0;

    /* SCT Section */
    inlifEntry.ipv4IcmpRedirectEnable         = GT_FALSE;
    inlifEntry.ipv6IcmpRedirectEnable         = GT_FALSE;
    inlifEntry.bridgeRouterInterfaceEnable    = GT_FALSE;
    inlifEntry.ipSecurityProfile              = 0;
    inlifEntry.ipv4IgmpToCpuEnable            = GT_FALSE;
    inlifEntry.ipv6IcmpToCpuEnable            = GT_FALSE;
    inlifEntry.udpBcRelayEnable               = GT_FALSE;
    inlifEntry.arpBcToCpuEnable               = GT_FALSE;
    inlifEntry.arpBcToMeEnable                = GT_FALSE;
    inlifEntry.ripv1MirrorEnable              = GT_FALSE;
    inlifEntry.ipv4LinkLocalMcCommandEnable   = GT_FALSE;
    inlifEntry.ipv6LinkLocalMcCommandEnable   = GT_FALSE;
    inlifEntry.ipv6NeighborSolicitationEnable = GT_FALSE;

    /* Misc Section */
    inlifEntry.mirrorToAnalyzerEnable = GT_FALSE;
    inlifEntry.mirrorToCpuEnable      = GT_FALSE;

    /* call api function */
    rc = prvTgfInlifEntrySet(PRV_TGF_INLIF_TYPE_EXTERNAL_E, 4096, &inlifEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfInlifEntrySet: %d", prvTgfDevNum);

    /* -------------------------------------------------------------------------
     * 1. Enable Routing
     */

    /* enable Unicast IP Routing on a Port */
    rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], CPSS_IP_UNICAST_E,
                                   CPSS_IP_PROTOCOL_IPV4V6_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* enable IP Unicast Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_SEND_VLANID_CNS, CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4V6_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* -------------------------------------------------------------------------
     * 2. Create the Route entry (Next hop) in Route table and Router ARP Table
     */

    for (i = 1; i <= 12; i++)
    {
        /* write a ARP MAC address to the Router ARP Table */
        arpMacAddr.arEther[0] = 0;
        arpMacAddr.arEther[1] = 0;
        arpMacAddr.arEther[2] = 0;
        arpMacAddr.arEther[3] = 0;
        arpMacAddr.arEther[4] = 0x22;
        arpMacAddr.arEther[5] = i;

        rc = prvTgfIpRouterArpAddrWrite(i, &arpMacAddr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRouterArpAddrWrite: %d", prvTgfDevNum);

        /* write a UC Route entry to the Route Table */
        cpssOsMemSet(&routeEntriesArray, 0, sizeof(routeEntriesArray));
        routeEntriesArray.cmd                        = CPSS_PACKET_CMD_ROUTE_E;
        routeEntriesArray.cpuCodeIndex               = 0;
        routeEntriesArray.appSpecificCpuCodeEnable   = GT_FALSE;
        routeEntriesArray.unicastPacketSipFilterEnable = GT_FALSE;
        routeEntriesArray.ttlHopLimitDecEnable       = GT_FALSE;
        routeEntriesArray.ttlHopLimDecOptionsExtChkByPass = GT_FALSE;
        routeEntriesArray.ingressMirror              = GT_FALSE;
        routeEntriesArray.qosProfileMarkingEnable    = GT_FALSE;
        routeEntriesArray.qosProfileIndex            = 0;
        routeEntriesArray.qosPrecedence              = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        routeEntriesArray.modifyUp                   = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
        routeEntriesArray.modifyDscp                 = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
        routeEntriesArray.countSet                   = PRV_TGF_COUNT_SET_CNS;
        routeEntriesArray.trapMirrorArpBcEnable      = GT_FALSE;
        routeEntriesArray.sipAccessLevel             = 0;
        routeEntriesArray.dipAccessLevel             = 0;
        routeEntriesArray.ICMPRedirectEnable         = GT_FALSE;
        routeEntriesArray.scopeCheckingEnable        = GT_FALSE;
        routeEntriesArray.siteId                     = CPSS_IP_SITE_ID_INTERNAL_E;
        routeEntriesArray.mtuProfileIndex            = 0;
        routeEntriesArray.isTunnelStart              = GT_FALSE;
        routeEntriesArray.nextHopVlanId              = PRV_TGF_NEXTHOPE_VLANID_CNS;
        if (i < 11)
        {
            routeEntriesArray.nextHopInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_23_CNS];
        }
        else
        {
            routeEntriesArray.nextHopInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_18_CNS];
        }
        routeEntriesArray.nextHopInterface.type      = CPSS_INTERFACE_PORT_E;
        routeEntriesArray.nextHopInterface.devPort.devNum = prvTgfDevNum;
        routeEntriesArray.nextHopARPPointer          = i;
        routeEntriesArray.nextHopTunnelPointer       = 0;

        rc = prvTgfIpUcRouteEntriesWrite(i, &routeEntriesArray, 1);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesWrite: %d", prvTgfDevNum);


        /* reading and checkiug the UC Route entry from the Route Table */
        cpssOsMemSet(&routeEntriesArray, 0, sizeof(routeEntriesArray));

        rc = prvTgfIpUcRouteEntriesRead(prvTgfDevNum, i, &routeEntriesArray, 1);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesRead: %d", prvTgfDevNum);

        PRV_UTF_LOG2_MAC("nextHopVlanId = %d, portNum = %d\n",
                         routeEntriesArray.nextHopVlanId,
                         routeEntriesArray.nextHopInterface.devPort.portNum);
    }
    /* -------------------------------------------------------------------------
     * 3. Create a new Ipv6 prefixes in a Virtual Router for the specified LPM DB
     */

    /* fill a nexthope info for the prefix */
    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = 11;
    prefixLen = 16;

    for (i = 0; i < 8; i++)
    {
        ipv6Addr.arIP[2 * i]       = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] >> 8);
        ipv6Addr.arIP[(2 * i) + 1] = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] & 0xFF);
    }

    /* call CPSS function */
    rc = prvTgfIpLpmIpv6UcPrefixAdd(prvTgfLpmDBId,
                                    vrId,
                                    ipv6Addr,
                                    prefixLen,
                                    &nextHopInfo,
                                    GT_FALSE,
                                    GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6UcPrefixAdd: %d", prvTgfDevNum);

    nextHopInfo.ipLttEntry.routeEntryBaseIndex = 12;
    prefixLen = 8;
    /* call CPSS function */

    rc = prvTgfIpLpmIpv6UcPrefixAdd(prvTgfLpmDBId,
                                    vrId,
                                    ipv6Addr,
                                    prefixLen,
                                    &nextHopInfo,
                                    GT_FALSE,
                                    GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6UcPrefixAdd: %d", prvTgfDevNum);

    /* -------------------------------------------------------------------------
     * 4. Create a new Ipv4 prefixes in a Virtual Router for the specified LPM DB
     */
    prefixLen = 32;
    /* fill a destination IP address for the prefix */
    cpssOsMemCpy(ipv4Addr.arIP, dstAddr, sizeof(ipv4Addr.arIP));

    for (i = 1; i < 11; i++)
    {
        nextHopInfo.ipLttEntry.routeEntryBaseIndex = i;
        if (i < 10)
        {
            prefixLen = prefixLen -2;
        }
        else
        {
            prefixLen = 32;
        }

        /* call CPSS function */
        rc = prvTgfIpLpmIpv4UcPrefixAdd(prvTgfLpmDBId,
                                        vrId,
                                        ipv4Addr,
                                        prefixLen,
                                        &nextHopInfo,
                                        GT_FALSE);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixAdd: %d", prvTgfDevNum);

    }

    /* -------------------------------------------------------------------------
     * 5. Set the FDB entry With DA_ROUTE
     */

    /* create a macEntry with .daRoute = GT_TRUE */
    cpssOsMemSet(&macEntry, 0, sizeof(macEntry));
    cpssOsMemCpy(macEntry.key.key.macVlan.macAddr.arEther, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));

    macEntry.key.entryType                  = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.vlanId         = PRV_TGF_SEND_VLANID_CNS;
    macEntry.dstInterface.type              = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.devNum    = prvTgfDevNum;
    macEntry.dstInterface.devPort.portNum   = prvTgfPortsArray[PRV_TGF_FDB_PORT_IDX_CNS];
    macEntry.isStatic                       = GT_TRUE;
    macEntry.daCommand                      = PRV_TGF_PACKET_CMD_FORWARD_E;
    macEntry.saCommand                      = PRV_TGF_PACKET_CMD_FORWARD_E;
    macEntry.daRoute                        = GT_TRUE;
    macEntry.sourceId                       = 0;
    macEntry.userDefined                    = 0;
    macEntry.daQosIndex                     = 0;
    macEntry.saQosIndex                     = 0;
    macEntry.daSecurityLevel                = 0;
    macEntry.saSecurityLevel                = 0;
    macEntry.appSpecificCpuCode             = GT_FALSE;
    macEntry.mirrorToRxAnalyzerPortEn       = GT_FALSE;

    rc = prvTgfBrgFdbMacEntrySet(&macEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntryWrite: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfIpUcRoutingTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
*
* INPUTS:
*       isIpv4Traffic   -     GT_TRUE:  generate ipv4 traffic,
*                             GT_FALSE: generate ipv6 traffic.
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
GT_VOID prvTgfIpUcRoutingTrafficGenerate
(
   IN GT_BOOL isIpv4Traffic
)
{
    GT_STATUS                       rc          = GT_OK;
    GT_U32                          partsCount  = 0;
    GT_U32                          packetSize  = 0;
    TGF_PACKET_STC                  packetInfo;
    GT_U32                          portIter    = 0;
    GT_U32                          numTriggers = 0;
    TGF_VFD_INFO_STC                vfdArray[1];
    CPSS_INTERFACE_INFO_STC         portInterface;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;
    PRV_TGF_IP_COUNTER_SET_STC      ipCounters;
    TGF_PACKET_PART_STC             *prvTgfPacketPartArrayPtr;
    GT_U8                           outputCapturePort;
    PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* clear capturing table */
    tgfTrafficTableRxPcktTblClear();

    if (isIpv4Traffic == GT_TRUE)
    {
        outputCapturePort = prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_23_CNS];
    }
    else
    {
        outputCapturePort = prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_18_CNS];
    }
    /* -------------------------------------------------------------------------
     * 1. Setup counters and enable capturing
     */

    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
    {
        /* reset ethernet counters */
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* reset IP couters and set ROUTE_ENTRY mode */
        rc = prvTgfCountersIpSet(prvTgfPortsArray[portIter], portIter);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCountersIpSet: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* setup nexthope portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = outputCapturePort;

    /* enable capture on nexthope port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_PCL_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, &d\n",
                                 prvTgfDevNum, portInterface.devPort.portNum);


    /* -------------------------------------------------------------------------
     * 2. Generating Traffic
     */

    /* number of parts in packet */
    if (isIpv4Traffic == GT_TRUE)
    {
        partsCount = sizeof(prvTgfPacketIpv4PartArray) / sizeof(prvTgfPacketIpv4PartArray[0]);
        prvTgfPacketPartArrayPtr = prvTgfPacketIpv4PartArray;
    }
    else
    {
        partsCount = sizeof(prvTgfPacketIpv6PartArray) / sizeof(prvTgfPacketIpv6PartArray[0]);
        prvTgfPacketPartArrayPtr = prvTgfPacketIpv6PartArray;

    }

    /* calculate packet size */
    rc = prvTgfPacketSizeGet(prvTgfPacketPartArrayPtr, partsCount, &packetSize);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPacketSizeGet:");

    /* build packet */
    packetInfo.totalLen   = packetSize;
    packetInfo.numOfParts = partsCount;
    packetInfo.partsArray = prvTgfPacketPartArrayPtr;

    /* setup packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, &packetInfo, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d", prvTgfDevNum);

    /* send packet */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, &d\n",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);


    /* -------------------------------------------------------------------------
     * 3. Get Ethernet Counters
     */

    /* read and check ethernet counters */
    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
    {
        GT_BOOL isOk;

        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* check Tx and Rx counters */
        switch (portIter)
        {
        case PRV_TGF_SEND_PORT_IDX_CNS:
            /* packetSize is not changed */
            expectedCntrs.goodOctetsSent.l[0] = (packetSize + TGF_CRC_LEN_CNS) * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
            expectedCntrs.brdcPktsSent.l[0]   = 0;
            expectedCntrs.mcPktsSent.l[0]     = 0;
            expectedCntrs.goodOctetsRcv.l[0]  = (packetSize + TGF_CRC_LEN_CNS) * prvTgfBurstCount;
            expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
            expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
            expectedCntrs.brdcPktsRcv.l[0]    = 0;
            expectedCntrs.mcPktsRcv.l[0]      = 0;

            break;
        case PRV_TGF_NEXTHOPE_PORT_IDX_23_CNS:
            if (isIpv4Traffic == GT_TRUE)
            {
                /* packetSize is not changed */
                expectedCntrs.goodOctetsSent.l[0] = (packetSize + TGF_CRC_LEN_CNS) * prvTgfBurstCount;
                expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
                expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
                expectedCntrs.brdcPktsSent.l[0]   = 0;
                expectedCntrs.mcPktsSent.l[0]     = 0;
                expectedCntrs.goodOctetsRcv.l[0]  = (packetSize + TGF_CRC_LEN_CNS) * prvTgfBurstCount;
                expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
                expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
                expectedCntrs.brdcPktsRcv.l[0]    = 0;
                expectedCntrs.mcPktsRcv.l[0]      = 0;
            }
            else
            {
                cpssOsMemSet(&expectedCntrs, 0 , sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));
            }
            break;
        case PRV_TGF_NEXTHOPE_PORT_IDX_18_CNS:
            if (isIpv4Traffic == GT_FALSE)
            {
                /* packetSize is not changed */
                expectedCntrs.goodOctetsSent.l[0] = (packetSize + TGF_CRC_LEN_CNS) * prvTgfBurstCount;
                expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
                expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
                expectedCntrs.brdcPktsSent.l[0]   = 0;
                expectedCntrs.mcPktsSent.l[0]     = 0;
                expectedCntrs.goodOctetsRcv.l[0]  = (packetSize + TGF_CRC_LEN_CNS) * prvTgfBurstCount;
                expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
                expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
                expectedCntrs.brdcPktsRcv.l[0]    = 0;
                expectedCntrs.mcPktsRcv.l[0]      = 0;
            }
            else
            {
                cpssOsMemSet(&expectedCntrs, 0 , sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));
            }
            break;
        default:
            /* for other ports */
            cpssOsMemSet(&expectedCntrs, 0 , sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));
            break;
        }
        PRV_TGF_VERIFY_COUNTERS_MAC(isOk, expectedCntrs, portCntrs);

        UTF_VERIFY_EQUAL0_STRING_MAC(isOk, GT_TRUE, "get another counters values.");

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isOk, expectedCntrs, portCntrs);

    }


    /* -------------------------------------------------------------------------
     * 4. Get Trigger Counters
     */

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
    vfdArray[0].patternPtr[0] = 0;
    vfdArray[0].patternPtr[1] = 0;
    vfdArray[0].patternPtr[2] = 0;
    vfdArray[0].patternPtr[3] = 0;
    vfdArray[0].patternPtr[4] = 0x22;

    if (isIpv4Traffic == GT_TRUE)
    {
        vfdArray[0].patternPtr[5] = 0x2;
    }
    else
    {
        vfdArray[0].patternPtr[5] = 0xb;
    }


    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 1, vfdArray, &numTriggers);
    PRV_UTF_LOG2_MAC("    numTriggers = %d, rc = 0x%02X\n\n", numTriggers, rc);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);


    /* -------------------------------------------------------------------------
     * 5. Get IP Counters
     */

    /* get and print ip counters values */
    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++) {
        PRV_UTF_LOG1_MAC("IP counters for Port [%d]:\n", prvTgfPortsArray[portIter]);
        prvTgfCountersIpGet(prvTgfDevNum, portIter, GT_TRUE, &ipCounters);
    }
    PRV_UTF_LOG0_MAC("\n");

    /* disable capture on nexthope port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_PCL_E, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, &d\n",
                                 prvTgfDevNum, portInterface.devPort.portNum);
}


/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushUpUcRoutingTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
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
GT_VOID prvTgfDynamicPartitionIpLpmPushUpUcRoutingTrafficGenerate
(
    GT_VOID
)
{
   prvTgfIpUcRoutingTrafficGenerate(GT_TRUE);/* ipv4 traffic */
   prvTgfIpUcRoutingTrafficGenerate(GT_FALSE);/* ipv6 traffic */
}

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushUpUcRoutingConfigurationRestore
*
* DESCRIPTION:
*       Restore configuration
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
*       2. Restore Route Configuration
*       1. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfDynamicPartitionIpLpmPushUpUcRoutingConfigurationRestore
(
    GT_VOID
)
{
    GT_U32      vlanIter  = 0;
    GT_U32      portIter  = 0;
    GT_U32      vlanCount = sizeof(prvTgfVlanArray);
    GT_STATUS   rc        = GT_OK;
    GT_IPADDR   ipv4Addr  = {0};
    GT_IPV6ADDR ipv6Addr  = {{0}};
    GT_U32      prefixLen;
    GT_U8       i;
    PRV_UTF_LOG0_MAC("======= Restoring Configuration =======\n");

    /* -------------------------------------------------------------------------
     * 2. Restore Route Configuration
     */

    /* delete the FDB entry */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", prvTgfDevNum);

    /* retsore ipv4 route configuration */

    /* fill the destination IP address for Ipv4 prefix in Virtual Router */
    cpssOsMemCpy(ipv4Addr.arIP, dstAddr, sizeof(ipv4Addr.arIP));
    prefixLen = 34;
    for (i = 1; i <= 10; i++)
    {
        prefixLen = prefixLen - 2;
        /* delete the Ipv4 prefix */
        rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, vrId, ipv4Addr, prefixLen);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);

    }

    /* restore IPv6 route configuration */
    /* fill a destination IP address for the prefix */
    for (i = 0; i < 8; i++)
    {
        ipv6Addr.arIP[2 * i]       = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] >> 8);
        ipv6Addr.arIP[(2 * i) + 1] = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] & 0xFF);
    }

    prefixLen = 24;
    for (i = 11; i <= 12; i++)
    {
        prefixLen = prefixLen - 8;

        /* delete the Ipv6 prefix */
        rc = prvTgfIpLpmIpv6UcPrefixDel(prvTgfLpmDBId, vrId, ipv6Addr, prefixLen);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6UcPrefixDel: %d", prvTgfDevNum);

    }


    /* -------------------------------------------------------------------------
     * 1. Restore Base Configuration
     */

    /* delete Ports from VLANs */
    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
    {
        /* delete port from VLAN */
        rc = prvTgfBrgVlanMemberRemove(prvTgfDevNum, prvTgfVlanPerPortArray[portIter],
                                       prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMemberRemove: %d, %d, %d",
                                     prvTgfDevNum, prvTgfVlanPerPortArray[portIter],
                                     prvTgfPortsArray[portIter]);
    }

    /* delete VLANs */
    for (vlanIter = 0; vlanIter < vlanCount; vlanIter++)
    {
        /* invalidate vlan entry */
        rc = prvTgfBrgVlanEntryInvalidate(prvTgfVlanArray[vlanIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d, %d",
                                     prvTgfDevNum, prvTgfVlanArray[vlanIter]);
    }
    /* clear capturing table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear: %d",
                                 prvTgfDevNum);

}



/*================================================================================*/
/*====================tgfDynamicPartitionIpLpmPushDownUcRouting===================*/
/*================================================================================*/

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushDownUcBaseConfigurationSet
*
* DESCRIPTION:
*       Set Base Configuration
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
GT_VOID prvTgfDynamicPartitionIpLpmPushDownUcBaseConfigurationSet
(
    GT_VOID
)
{
    prvTgfDynamicPartitionIpLpmPushUpUcBaseConfigurationSet();

}

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushDownUcRouteConfigurationSet
*
* DESCRIPTION:
*       Set Base Configuration
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
GT_VOID prvTgfDynamicPartitionIpLpmPushDownUcRouteConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                               rc = GT_OK;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_IPADDR                               ipv4Addr;
    GT_U32                                  prefixLen;

    /* set the same configuration as in push up test */
    prvTgfDynamicPartitionIpLpmPushUpUcRouteConfigurationSet();

    /* delete ipv4 prefix 1.1.1.1/32*/
    prefixLen = 32;
    /* fill the destination IP address for Ipv4 prefix in Virtual Router */
    cpssOsMemCpy(ipv4Addr.arIP, dstAddr, sizeof(ipv4Addr.arIP));
    rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, vrId, ipv4Addr, prefixLen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);

    /* add ipv4 prefix 1.1.1.1/12 */

    /* fill a nexthope info for the prefix */
    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = 10;
    prefixLen = 12;

    rc = prvTgfIpLpmIpv4UcPrefixAdd(prvTgfLpmDBId,
                                    vrId,
                                    ipv4Addr,
                                    prefixLen,
                                    &nextHopInfo,
                                    GT_FALSE);

    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixAdd: %d", prvTgfDevNum);

}


/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushDownUcRoutingTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
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
GT_VOID prvTgfDynamicPartitionIpLpmPushDownUcRoutingTrafficGenerate
(
    GT_VOID
)
{
   prvTgfDynamicPartitionIpLpmPushUpUcRoutingTrafficGenerate();
}


/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushDownUcRoutingConfigurationRestore
*
* DESCRIPTION:
*       Restore configuration
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
*       2. Restore Route Configuration
*       1. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfDynamicPartitionIpLpmPushDownUcRoutingConfigurationRestore
(
    GT_VOID
)
{
    GT_U32      vlanIter  = 0;
    GT_U32      portIter  = 0;
    GT_U32      vlanCount = sizeof(prvTgfVlanArray);
    GT_STATUS   rc        = GT_OK;
    GT_IPADDR   ipv4Addr  = {0};
    GT_IPV6ADDR ipv6Addr  = {{0}};
    GT_U32      prefixLen;
    GT_U8       i;
    PRV_UTF_LOG0_MAC("======= Restoring Configuration =======\n");

    /* -------------------------------------------------------------------------
     * 2. Restore Route Configuration
     */

    /* delete the FDB entry */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", prvTgfDevNum);

    /* retsore ipv4 route configuration */

    /* fill the destination IP address for Ipv4 prefix in Virtual Router */
    cpssOsMemCpy(ipv4Addr.arIP, dstAddr, sizeof(ipv4Addr.arIP));
    prefixLen = 32;
    for (i = 1; i <= 10; i++)
    {
        prefixLen = prefixLen - 2;
        /* delete the Ipv4 prefix */
        rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, vrId, ipv4Addr, prefixLen);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);

    }

    /* restore IPv6 route configuration */
    /* fill a destination IP address for the prefix */
    for (i = 0; i < 8; i++)
    {
        ipv6Addr.arIP[2 * i]       = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] >> 8);
        ipv6Addr.arIP[(2 * i) + 1] = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] & 0xFF);
    }

    prefixLen = 24;
    for (i = 11; i <= 12; i++)
    {
        prefixLen = prefixLen - 8;

        /* delete the Ipv6 prefix */
        rc = prvTgfIpLpmIpv6UcPrefixDel(prvTgfLpmDBId, vrId, ipv6Addr, prefixLen);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6UcPrefixDel: %d", prvTgfDevNum);

    }


    /* -------------------------------------------------------------------------
     * 1. Restore Base Configuration
     */

    /* delete Ports from VLANs */
    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
    {
        /* delete port from VLAN */
        rc = prvTgfBrgVlanMemberRemove(prvTgfDevNum, prvTgfVlanPerPortArray[portIter],
                                       prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMemberRemove: %d, %d, %d",
                                     prvTgfDevNum, prvTgfVlanPerPortArray[portIter],
                                     prvTgfPortsArray[portIter]);
    }

    /* delete VLANs */
    for (vlanIter = 0; vlanIter < vlanCount; vlanIter++)
    {
        /* invalidate vlan entry */
        rc = prvTgfBrgVlanEntryInvalidate(prvTgfVlanArray[vlanIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d, %d",
                                     prvTgfDevNum, prvTgfVlanArray[vlanIter]);
    }
    /* clear capturing table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear: %d",
                                 prvTgfDevNum);

}


/*================================================================================*/
/*====================tgfDynamicPartitionIpLpmDefragUcRouting=====================*/
/*================================================================================*/


/*******************************************************************************
* prvTgfDynamicPartitionIpLpmDefragUcBaseConfigurationSet
*
* DESCRIPTION:
*       Set Base Configuration
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
GT_VOID prvTgfDynamicPartitionIpLpmDefragUcBaseConfigurationSet
(
    GT_VOID
)
{
    prvTgfDynamicPartitionIpLpmPushUpUcBaseConfigurationSet();

}


/*******************************************************************************
* prvTgfDynamicPartitionIpLpmDefragUcRouteConfigurationSet
*
* DESCRIPTION:
*       Set Base Configuration
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
GT_VOID prvTgfDynamicPartitionIpLpmDefragUcRouteConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                               rc = GT_OK;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_IPADDR                               ipv4Addr = {0};
    GT_IPV6ADDR                             ipv6Addr = {{0}};
    GT_U8 i;
    /* set the same configuration as in push up test */
    prvTgfDynamicPartitionIpLpmPushUpUcRouteConfigurationSet();

    /* delete ipv4 prefix 1.1.1.1/32; 1.1.1.1/26; 1.1.1.1/22; 1.1.1.1/14*/

    /* fill the destination IP address for Ipv4 prefix in Virtual Router */
    cpssOsMemCpy(ipv4Addr.arIP, dstAddr, sizeof(ipv4Addr.arIP));
    rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, vrId, ipv4Addr, 32);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);
    rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, vrId, ipv4Addr, 26);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);
    rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, vrId, ipv4Addr, 22);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);
    rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, vrId, ipv4Addr, 14);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);

    /* add ipv6 prefix 2001::1234/12 */

    /* fill a nexthope info for the prefix */
    cpssOsMemSet(&nextHopInfo, 0, sizeof(nextHopInfo));
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = 10;

    /* fill a destination IP address for the prefix */
    for (i = 0; i < 8; i++)
    {
        ipv6Addr.arIP[2 * i]       = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] >> 8);
        ipv6Addr.arIP[(2 * i) + 1] = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] & 0xFF);
    }

    /* call CPSS function */
    rc = prvTgfIpLpmIpv6UcPrefixAdd(prvTgfLpmDBId,
                                    vrId,
                                    ipv6Addr,
                                    12,
                                    &nextHopInfo,
                                    GT_FALSE,
                                    GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6UcPrefixAdd: %d", prvTgfDevNum);

}


/*******************************************************************************
* prvTgfDynamicPartitionIpLpmDefragUcRoutingTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
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
GT_VOID prvTgfDynamicPartitionIpLpmDefragUcRoutingTrafficGenerate
(
    GT_VOID
)
{
   prvTgfDynamicPartitionIpLpmPushUpUcRoutingTrafficGenerate();
}


/*******************************************************************************
* prvTgfDynamicPartitionIpLpmDefragUcRoutingConfigurationRestore
*
* DESCRIPTION:
*       Restore configuration
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
*       2. Restore Route Configuration
*       1. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfDynamicPartitionIpLpmDefragUcRoutingConfigurationRestore
(
    GT_VOID
)
{
    GT_U32      vlanIter  = 0;
    GT_U32      portIter  = 0;
    GT_U32      vlanCount = sizeof(prvTgfVlanArray);
    GT_STATUS   rc        = GT_OK;
    GT_IPADDR   ipv4Addr  = {0};
    GT_IPV6ADDR ipv6Addr  = {{0}};
    GT_U32      prefixLen;
    GT_U8       i;
    PRV_UTF_LOG0_MAC("======= Restoring Configuration =======\n");

    /* -------------------------------------------------------------------------
     * 2. Restore Route Configuration
     */

    /* delete the FDB entry */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", prvTgfDevNum);

    /* retsore ipv4 route configuration */

    /* fill the destination IP address for Ipv4 prefix in Virtual Router */
    cpssOsMemCpy(ipv4Addr.arIP, dstAddr, sizeof(ipv4Addr.arIP));
    prefixLen = 32;
    for (i = 1; i <= 9; i++)
    {
        prefixLen = prefixLen - 2;
        /* delete the Ipv4 prefix */
        if (prefixLen != 32 && prefixLen != 26 &&
            prefixLen != 22 && prefixLen != 14 )
        {
            rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId, vrId, ipv4Addr, prefixLen);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);

        }

    }

    /* restore IPv6 route configuration */
    /* fill a destination IP address for the prefix */
    for (i = 0; i < 8; i++)
    {
        ipv6Addr.arIP[2 * i]       = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] >> 8);
        ipv6Addr.arIP[(2 * i) + 1] = (GT_U8)(prvTgfPacketIpv6Part.dstAddr[i] & 0xFF);
    }

    /* delete ipv6 prefixes 2001::1234/16;2001::1234/12; 2001::1234/8 */
    for (prefixLen = 8; prefixLen <= 16; prefixLen += 4)
    {
        /* delete the Ipv6 prefix */
        rc = prvTgfIpLpmIpv6UcPrefixDel(prvTgfLpmDBId, vrId, ipv6Addr, prefixLen);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6UcPrefixDel: %d", prvTgfDevNum);
    }

    /* -------------------------------------------------------------------------
     * 1. Restore Base Configuration
     */

    /* delete Ports from VLANs */
    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
    {
        /* delete port from VLAN */
        rc = prvTgfBrgVlanMemberRemove(prvTgfDevNum, prvTgfVlanPerPortArray[portIter],
                                       prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMemberRemove: %d, %d, %d",
                                     prvTgfDevNum, prvTgfVlanPerPortArray[portIter],
                                     prvTgfPortsArray[portIter]);
    }

    /* delete VLANs */
    for (vlanIter = 0; vlanIter < vlanCount; vlanIter++)
    {
        /* invalidate vlan entry */
        rc = prvTgfBrgVlanEntryInvalidate(prvTgfVlanArray[vlanIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d, %d",
                                     prvTgfDevNum, prvTgfVlanArray[vlanIter]);
    }
    /* clear capturing table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear: %d",
                                 prvTgfDevNum);

}

