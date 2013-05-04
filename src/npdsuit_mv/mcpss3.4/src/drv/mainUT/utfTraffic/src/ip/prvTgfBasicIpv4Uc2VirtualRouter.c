/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBasicIpv4Uc2VirtualRouter.c
*
* DESCRIPTION:
*       Basic IPV4 UC 2  virtual Router
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
#include <ip/prvTgfBasicIpv4Uc2VirtualRouter.h>



/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* number of VLANs */
#define PRV_TGF_VLAN_COUNT_CNS            2

/* default VLAN Id */
#define PRV_TGF_SEND_VLANID_CNS           5

/* nextHop VLAN Id */
#define PRV_TGF_NEXTHOPE_VLANID_CNS       6

/* number of ports */
#define PRV_TGF_PORT_COUNT_CNS            4

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS         0

/* nextHop port number to receive traffic from */
#define PRV_TGF_NEXTHOPE_PORT_IDX_CNS     3

/* the counter set for a route entry is linked to */
#define PRV_TGF_COUNT_SET_CNS     CPSS_IP_CNT_SET0_E

/* default number of packets to send */
static GT_U32        prvTgfBurstCount   = 1;

/* default number of iterations (one iteration for each VLAN) */
static GT_U32        prvTgfIterCount   = 1; /*PRV_TGF_VLAN_COUNT_CNS*/

/* VLANs array */
static GT_U8         prvTgfVlanArrays[PRV_TGF_VLAN_COUNT_CNS] =
{
    PRV_TGF_SEND_VLANID_CNS,
    PRV_TGF_NEXTHOPE_VLANID_CNS
};

/* NextHop VLANs array */
static GT_U8         prvTgfNextHopVlanArrays[PRV_TGF_VLAN_COUNT_CNS] =
{
    PRV_TGF_NEXTHOPE_VLANID_CNS,
    PRV_TGF_SEND_VLANID_CNS
};

/* tx ports array */
static GT_U8         prvTgfTxPortsIdxArray[PRV_TGF_VLAN_COUNT_CNS] =
{
    PRV_TGF_SEND_PORT_IDX_CNS,
    PRV_TGF_NEXTHOPE_PORT_IDX_CNS
};

/* NextHop ports array */
static GT_U8         prvTgfNextHopPortsIdxArray[PRV_TGF_VLAN_COUNT_CNS] =
{
    PRV_TGF_NEXTHOPE_PORT_IDX_CNS,
    PRV_TGF_SEND_PORT_IDX_CNS
};

/* VLANs array */
static GT_U8         prvTgfVlanPerPortArray[PRV_TGF_PORT_COUNT_CNS] = {
    PRV_TGF_SEND_VLANID_CNS, PRV_TGF_SEND_VLANID_CNS,
    PRV_TGF_NEXTHOPE_VLANID_CNS, PRV_TGF_NEXTHOPE_VLANID_CNS
};

/* the Arp Address index of the Router ARP Table
   to write to the UC Route entry Arp nextHopARPPointer field */
static GT_U32        prvTgfRouterArpIndex[PRV_TGF_VLAN_COUNT_CNS] = {1, 2};

/* the Route entry index for UC Route entry Table */
static GT_U32        prvTgfRouteEntryBaseIndex[PRV_TGF_VLAN_COUNT_CNS] = {25, 55};

/* the Virtual Route entry index for UC Route entry Table */
static GT_U32        prvTgfVirtualRouteEntryBaseIndex[PRV_TGF_VLAN_COUNT_CNS] = {30, 60};

/* the Virtual Route index */
/* Currently only vrId = 0 is supported in function cpssDxChIpLpmVirtualRouterAdd
 * There needs {1, 2} for the rihgt test
 */
static GT_U32        prvTgfVirtualRouteIndex[PRV_TGF_VLAN_COUNT_CNS] = {1, 1}; /*1, 2*/

/* the LPM DB id for LPM Table */
static GT_U32        prvTgfLpmDBId = 0;

/* the ARP MAC address to write to the Router ARP Table */
static TGF_MAC_ADDR  prvTgfArpMacArr[PRV_TGF_VLAN_COUNT_CNS] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x22},
                                                                {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}};

/* dst MAC address */
static TGF_MAC_ADDR  prvTgfDstMacArr[PRV_TGF_VLAN_COUNT_CNS] = {{0x00, 0x00, 0x00, 0x00, 0x34, 0x02},
                                                                {0x00, 0x00, 0x00, 0x00, 0x00, 0x05}};

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
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};
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
    0x76CB,             /* csum */
    { 1,  1,  1,  1},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
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
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/******************************************************************************/


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfBasicIpv4Uc2VirtualRouterBaseConfigurationSet
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
GT_VOID prvTgfBasicIpv4Uc2VirtualRouterBaseConfigurationSet
(
    GT_VOID
)
{
    GT_BOOL                     isTagged  = GT_FALSE;
    GT_U32                      vlanIter  = 0;
    GT_U32                      portIter  = 0;
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
    vlanInfo.vrfId                = prvTgfVirtualRouteIndex[0];
    /* create vlans */
    for (vlanIter = 0; vlanIter < PRV_TGF_VLAN_COUNT_CNS; vlanIter++)
    {
        rc = prvTgfBrgVlanEntryWrite(prvTgfDevNum, prvTgfVlanArrays[vlanIter],
                                     &portsMembers, &portsTagging,
                                     &vlanInfo, &portsTaggingCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryWrite: %d, %d",
                                     prvTgfDevNum, prvTgfVlanArrays[vlanIter]);
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
* prvTgfBasicIpv4Uc2VirtualRouterRouteConfigurationSet
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
GT_VOID prvTgfBasicIpv4Uc2VirtualRouterRouteConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                               rc = GT_OK;
    PRV_TGF_INLIF_ENTRY_STC                 inlifEntry;
    GT_ETHERADDR                            arpMacAddr;
    PRV_TGF_BRG_MAC_ENTRY_STC               macEntry;
    PRV_TGF_IP_UC_ROUTE_ENTRY_STC           ucRouteEntry;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    routeEntryInfo;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    defUcRouteEntryInfo[2];
    PRV_TGF_IP_MC_ROUTE_ENTRY_STC           cpssMcRouteEntry;
    GT_IPADDR                               ipAddr;
    GT_U32                                  vlanIter;
    GT_U32                                  i;

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
    inlifEntry.ipv6UcRouteEnable = GT_FALSE;
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

    /* make iteration */
    for (vlanIter = 0; vlanIter < prvTgfIterCount; vlanIter++)
    {
        /* -------------------------------------------------------------------------
         * 1. Enable Routing
         */

        /* enable Unicast IPv4 Routing on a Port */
        rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[prvTgfTxPortsIdxArray[vlanIter]],
                                       CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, GT_TRUE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d %d",
                                     prvTgfDevNum, prvTgfPortsArray[prvTgfTxPortsIdxArray[vlanIter]]);

        /* enable IPv4 Unicast Routing on Vlan */
        rc = prvTgfIpVlanRoutingEnable(prvTgfVlanArrays[vlanIter], CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, GT_TRUE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                     prvTgfDevNum, prvTgfVlanArrays[vlanIter]);

        /* -------------------------------------------------------------------------
        * 2. Create a new Virtual Router for the LPM DB
        */

        /* set a route entry with CMD_TRAP_TO_CPU as default ipv4 unicast */
        cpssOsMemSet(&ucRouteEntry, 0, sizeof(ucRouteEntry));

        ucRouteEntry.cmd                              = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        ucRouteEntry.nextHopVlanId                    = PRV_TGF_SEND_VLANID_CNS;
        ucRouteEntry.nextHopInterface.type            = CPSS_INTERFACE_PORT_E;
        ucRouteEntry.nextHopInterface.devPort.devNum  = prvTgfDevNum;
        ucRouteEntry.nextHopInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS];
        ucRouteEntry.nextHopARPPointer                = prvTgfRouterArpIndex[1];

        rc = prvTgfIpUcRouteEntriesWrite(prvTgfVirtualRouteEntryBaseIndex[vlanIter], &ucRouteEntry, 1);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesWrite: %d", prvTgfDevNum);

        /* set a route entry with CMD_TRAP_TO_CPU as default ipv4 multicast */
        cpssOsMemSet(&cpssMcRouteEntry, 0, sizeof(cpssMcRouteEntry));

        cpssMcRouteEntry.cmd            = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        cpssMcRouteEntry.RPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        rc = prvTgfIpMcRouteEntriesWrite(1, 1, &cpssMcRouteEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpMcRouteEntriesWrite: %d", prvTgfDevNum);

        cpssOsMemSet(defUcRouteEntryInfo, 0, sizeof(defUcRouteEntryInfo));

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterDel: %d", prvTgfDevNum);
        for (i = 0; i < 2; i++)
        {
            /* set defUcRouteEntryInfo */
            defUcRouteEntryInfo[i].ipLttEntry.routeType                = PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            defUcRouteEntryInfo[i].ipLttEntry.numOfPaths               = 0;
            defUcRouteEntryInfo[i].ipLttEntry.routeEntryBaseIndex      = prvTgfVirtualRouteEntryBaseIndex[vlanIter];
            defUcRouteEntryInfo[i].ipLttEntry.ucRPFCheckEnable         = GT_FALSE;
            defUcRouteEntryInfo[i].ipLttEntry.sipSaCheckMismatchEnable = GT_FALSE;
            defUcRouteEntryInfo[i].ipLttEntry.ipv6MCGroupScopeLevel    = 0; /*CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E*/
        }
        /* create Virtual Route */
        rc = prvTgfIpLpmVirtualRouterAdd(prvTgfLpmDBId,
                                         prvTgfVirtualRouteIndex[vlanIter],
                                         &defUcRouteEntryInfo[0], /*defIpv4UcRouteEntryInfo*/
                                         &defUcRouteEntryInfo[1], /*defIpv6UcRouteEntryInfo*/
                                         NULL,                    /*defIpv4McRouteLttEntry*/
                                         NULL);                   /*defIpv6McRouteLttEntry*/
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterAdd: %d", prvTgfDevNum);

        /* -------------------------------------------------------------------------
         * 3. Create the Route entry in Route Table (Next hop Table)
         */

        /* create a ARP MAC address to the Router ARP Table */
        cpssOsMemCpy(arpMacAddr.arEther, prvTgfArpMacArr[vlanIter], sizeof(TGF_MAC_ADDR));

        rc = prvTgfIpRouterArpAddrWrite(prvTgfRouterArpIndex[vlanIter], &arpMacAddr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRouterArpAddrWrite: %d", prvTgfDevNum);

        /* create a UC Route entry for the Route Table with CMD_ROUTE */
        cpssOsMemSet(&ucRouteEntry, 0, sizeof(ucRouteEntry));

        ucRouteEntry.cmd                              = CPSS_PACKET_CMD_ROUTE_E;
        ucRouteEntry.nextHopVlanId                    = prvTgfNextHopVlanArrays[vlanIter];
        ucRouteEntry.nextHopInterface.type            = CPSS_INTERFACE_PORT_E;
        ucRouteEntry.nextHopInterface.devPort.devNum  = prvTgfDevNum;
        ucRouteEntry.nextHopInterface.devPort.portNum = prvTgfPortsArray[prvTgfNextHopPortsIdxArray[vlanIter]];
        ucRouteEntry.nextHopARPPointer                = prvTgfRouterArpIndex[vlanIter];

        rc = prvTgfIpUcRouteEntriesWrite(prvTgfRouteEntryBaseIndex[vlanIter], &ucRouteEntry, 1);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesWrite: %d", prvTgfDevNum);

        /* reading and checkiug the UC Route entry from the Route Table */
        cpssOsMemSet(&ucRouteEntry, 0, sizeof(ucRouteEntry));

        rc = prvTgfIpUcRouteEntriesRead(prvTgfDevNum, prvTgfRouteEntryBaseIndex[vlanIter], &ucRouteEntry, 1);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesRead: %d", prvTgfDevNum);

        PRV_UTF_LOG4_MAC("cmd = %d, nextHopVlanId = %d, portNum = %d, ARPPtr = %d\n",
                         ucRouteEntry.cmd,
                         ucRouteEntry.nextHopVlanId,
                         ucRouteEntry.nextHopInterface.devPort.portNum,
                         ucRouteEntry.nextHopARPPointer);

        /* -------------------------------------------------------------------------
         * 4. Create prefix in LPM Table
         */

        /* set LPM prefix as the DIP of test packet */
        cpssOsMemCpy(ipAddr.arIP, prvTgfPacketIpv4Part.dstAddr, sizeof(ipAddr.arIP));

        /* set Route Entry Index the same as in Route Table */
        cpssOsMemSet(&routeEntryInfo, 0, sizeof(routeEntryInfo));
        routeEntryInfo.ipLttEntry.routeEntryBaseIndex = prvTgfRouteEntryBaseIndex[vlanIter];

        rc = prvTgfIpLpmIpv4UcPrefixAdd(prvTgfLpmDBId, prvTgfVirtualRouteIndex[vlanIter],
                                        ipAddr, 32, &routeEntryInfo, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixAdd: %d", prvTgfDevNum);

        /* -------------------------------------------------------------------------
         * 5. Set the FDB entry With DA_ROUTE
         */

        /* create a macEntry with .daRoute = GT_TRUE */
        cpssOsMemSet(&macEntry, 0, sizeof(macEntry));
        cpssOsMemCpy(macEntry.key.key.macVlan.macAddr.arEther, prvTgfDstMacArr[vlanIter], sizeof(TGF_MAC_ADDR));

        macEntry.key.entryType                  = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
        macEntry.key.key.macVlan.vlanId         = prvTgfVlanArrays[vlanIter];
        macEntry.dstInterface.type              = CPSS_INTERFACE_PORT_E;
        macEntry.dstInterface.devPort.devNum    = prvTgfDevNum;
        macEntry.dstInterface.devPort.portNum   = prvTgfPortsArray[prvTgfTxPortsIdxArray[vlanIter]];
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
}

/*******************************************************************************
* prvTgfBasicIpv4Uc2VirtualRouterTrafficGenerate
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
GT_VOID prvTgfBasicIpv4Uc2VirtualRouterTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                       rc          = GT_OK;
    GT_U32                          vlanIter    = 0;
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

    /* make iteration */
    for (vlanIter = 0; vlanIter < prvTgfIterCount; vlanIter++)
    {
        PRV_UTF_LOG2_MAC("======= Generating Traffic from port %d, VLAN %d =======\n",
                         prvTgfPortsArray[prvTgfTxPortsIdxArray[vlanIter]], prvTgfVlanArrays[vlanIter]);

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
        portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_CNS];

        /* enable capture on nexthope port */
        rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, &d\n",
                                     prvTgfDevNum, prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_CNS]);

        /* clear capturing RxPcktTable */
        rc = tgfTrafficTableRxPcktTblClear();
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");


        /* -------------------------------------------------------------------------
         * 2. Generating Traffic
         */

        /* number of parts in packet */
        partsCount = sizeof(prvTgfPacketPartArray) / sizeof(prvTgfPacketPartArray[0]);

        /* calculate packet size */
        rc = prvTgfPacketSizeGet(prvTgfPacketPartArray, partsCount, &packetSize);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPacketSizeGet:");

        /* build packet */
        packetInfo.totalLen   = packetSize;
        packetInfo.numOfParts = partsCount;
        packetInfo.partsArray = prvTgfPacketPartArray;

        /* set MAC DA and VlanId */
        cpssOsMemCpy(prvTgfPacketL2Part.daMac, prvTgfDstMacArr[vlanIter], sizeof(TGF_MAC_ADDR));
        prvTgfPacketVlanTagPart.vid = prvTgfVlanArrays[vlanIter];

        /* setup packet */
        rc = prvTgfSetTxSetupEth(prvTgfDevNum, &packetInfo, prvTgfBurstCount, 0, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d", prvTgfDevNum);

        /* send packet */
        rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[prvTgfTxPortsIdxArray[vlanIter]]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, &d\n",
                                     prvTgfDevNum, prvTgfPortsArray[prvTgfTxPortsIdxArray[vlanIter]]);


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
            switch (portIter) {
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

                case PRV_TGF_NEXTHOPE_PORT_IDX_CNS:

                    /* packetSize is not changed */
                    expectedCntrs.goodOctetsSent.l[0] = (packetSize + TGF_CRC_LEN_CNS - TGF_VLAN_TAG_SIZE_CNS) * prvTgfBurstCount;
                    expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
                    expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
                    expectedCntrs.brdcPktsSent.l[0]   = 0;
                    expectedCntrs.mcPktsSent.l[0]     = 0;
                    expectedCntrs.goodOctetsRcv.l[0]  = (packetSize + TGF_CRC_LEN_CNS - TGF_VLAN_TAG_SIZE_CNS) * prvTgfBurstCount;
                    expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
                    expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
                    expectedCntrs.brdcPktsRcv.l[0]    = 0;
                    expectedCntrs.mcPktsRcv.l[0]      = 0;

                    break;

                default:

                    /* for other ports */
                    expectedCntrs.goodOctetsSent.l[0] = 0;
                    expectedCntrs.goodPktsSent.l[0]   = 0;
                    expectedCntrs.ucPktsSent.l[0]     = 0;
                    expectedCntrs.brdcPktsSent.l[0]   = 0;
                    expectedCntrs.mcPktsSent.l[0]     = 0;
                    expectedCntrs.goodOctetsRcv.l[0]  = 0;
                    expectedCntrs.goodPktsRcv.l[0]    = 0;
                    expectedCntrs.ucPktsRcv.l[0]      = 0;
                    expectedCntrs.brdcPktsRcv.l[0]    = 0;
                    expectedCntrs.mcPktsRcv.l[0]      = 0;

                    break;
            }

            isOk =
                portCntrs.goodOctetsSent.l[0] == expectedCntrs.goodOctetsSent.l[0] &&
                portCntrs.goodPktsSent.l[0]   == expectedCntrs.goodPktsSent.l[0] &&
                portCntrs.ucPktsSent.l[0]     == expectedCntrs.ucPktsSent.l[0] &&
                portCntrs.brdcPktsSent.l[0]   == expectedCntrs.brdcPktsSent.l[0] &&
                portCntrs.mcPktsSent.l[0]     == expectedCntrs.mcPktsSent.l[0] &&
                portCntrs.goodOctetsRcv.l[0]  == expectedCntrs.goodOctetsRcv.l[0] &&
                portCntrs.goodPktsRcv.l[0]    == expectedCntrs.goodPktsRcv.l[0] &&
                portCntrs.ucPktsRcv.l[0]      == expectedCntrs.ucPktsRcv.l[0] &&
                portCntrs.brdcPktsRcv.l[0]    == expectedCntrs.brdcPktsRcv.l[0] &&
                portCntrs.mcPktsRcv.l[0]      == expectedCntrs.mcPktsRcv.l[0];

            UTF_VERIFY_EQUAL0_STRING_MAC(isOk, GT_TRUE, "get another counters values.");

            /* print expected values if bug */
            if (isOk != GT_TRUE) {
                PRV_UTF_LOG0_MAC("Expected values:\n");
                PRV_UTF_LOG1_MAC(" goodOctetsSent = %d\n", expectedCntrs.goodOctetsSent.l[0]);
                PRV_UTF_LOG1_MAC(" goodPktsSent = %d\n", expectedCntrs.goodPktsSent.l[0]);
                PRV_UTF_LOG1_MAC(" ucPktsSent = %d\n", expectedCntrs.ucPktsSent.l[0]);
                PRV_UTF_LOG1_MAC(" brdcPktsSent = %d\n", expectedCntrs.brdcPktsSent.l[0]);
                PRV_UTF_LOG1_MAC(" mcPktsSent = %d\n", expectedCntrs.mcPktsSent.l[0]);
                PRV_UTF_LOG1_MAC(" goodOctetsRcv = %d\n", expectedCntrs.goodOctetsRcv.l[0]);
                PRV_UTF_LOG1_MAC(" goodPktsRcv = %d\n", expectedCntrs.goodPktsRcv.l[0]);
                PRV_UTF_LOG1_MAC(" ucPktsRcv = %d\n", expectedCntrs.ucPktsRcv.l[0]);
                PRV_UTF_LOG1_MAC(" brdcPktsRcv = %d\n", expectedCntrs.brdcPktsRcv.l[0]);
                PRV_UTF_LOG1_MAC(" mcPktsRcv = %d\n", expectedCntrs.mcPktsRcv.l[0]);
                PRV_UTF_LOG0_MAC("\n");
            }
        }


        /* -------------------------------------------------------------------------
         * 4. Get Trigger Counters
         */

        PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

        /* get trigger counters where packet has MAC DA as prvTgfArpMac */
        vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
        vfdArray[0].modeExtraInfo = 0;
        vfdArray[0].offset = 0;
        vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
        cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfArpMacArr[vlanIter], sizeof(TGF_MAC_ADDR));

        rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 1, vfdArray, &numTriggers);
        PRV_UTF_LOG2_MAC("    numTriggers = %d, rc = 0x%02X\n\n", numTriggers, rc);

        /* check TriggerCounters */
        rc = rc == GT_NO_MORE ? GT_OK : rc;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                     portInterface.devPort.devNum, portInterface.devPort.portNum);

        /* check if captured packet has the same MAC DA as prvTgfArpMac */
        UTF_VERIFY_EQUAL6_STRING_MAC(1, numTriggers,
                "\n   MAC DA of captured packet must be: %02X:%02X:%02X:%02X:%02X:%02X",
                prvTgfArpMacArr[vlanIter][0], prvTgfArpMacArr[vlanIter][1], prvTgfArpMacArr[vlanIter][2],
                prvTgfArpMacArr[vlanIter][3], prvTgfArpMacArr[vlanIter][4], prvTgfArpMacArr[vlanIter][5]);


        /* -------------------------------------------------------------------------
         * 5. Get IP Counters
         */

        /* get and print ip counters values */
        for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++) {
            PRV_UTF_LOG1_MAC("IP counters for Port [%d]:\n", prvTgfPortsArray[portIter]);
            prvTgfCountersIpGet(prvTgfDevNum, portIter, GT_TRUE, &ipCounters);
        }
        PRV_UTF_LOG0_MAC("\n");

        /* enable capture on nexthope port */
        rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, &d\n",
                                     prvTgfDevNum, prvTgfPortsArray[PRV_TGF_NEXTHOPE_PORT_IDX_CNS]);
    }
}

/*******************************************************************************
* prvTgfBasicIpv4Uc2VirtualRouterConfigurationRestore
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
GT_VOID prvTgfBasicIpv4Uc2VirtualRouterConfigurationRestore
(
    GT_VOID
)
{
    GT_U32      vlanIter  = 0;
    GT_U32      portIter  = 0;
    GT_STATUS   rc        = GT_OK;
    GT_IPADDR   ipAddr;
    GT_U32      i;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    defUcRouteEntryInfo[2];
    PRV_TGF_IP_LTT_ENTRY_STC                defMcRouteLttEntry[2];

    PRV_UTF_LOG0_MAC("======= Restoring Configuration =======\n");
    cpssOsMemSet(defUcRouteEntryInfo, 0, sizeof(defUcRouteEntryInfo));
    cpssOsMemSet(defMcRouteLttEntry, 0, sizeof(defMcRouteLttEntry));

    /* delete the FDB entry */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", prvTgfDevNum);

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* -------------------------------------------------------------------------
     * 2. Restore Route Configuration
     */

    /* make iteration */
    for (vlanIter = 0; vlanIter < prvTgfIterCount; vlanIter++)
    {
        /* fill the destination IP address for LPM prefix */
        cpssOsMemCpy(ipAddr.arIP, prvTgfPacketIpv4Part.dstAddr, sizeof(ipAddr.arIP));

        /* delete the LPM prefix */
        rc = prvTgfIpLpmIpv4UcPrefixDel(prvTgfLpmDBId,prvTgfVirtualRouteIndex[vlanIter], ipAddr, 32);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixDel: %d", prvTgfDevNum);

        /* restore virtual router */

        /* delete virtual router */
        rc = prvTgfIpLpmVirtualRouterDel(prvTgfLpmDBId, prvTgfVirtualRouteIndex[vlanIter]);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterDel: %d", prvTgfDevNum);
        for (i = 0; i < 2; i++)
        {
            /* set defUcLttEntryInfo */
            defUcRouteEntryInfo[i].ipLttEntry.routeType                = PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            defUcRouteEntryInfo[i].ipLttEntry.numOfPaths               = 0;
            defUcRouteEntryInfo[i].ipLttEntry.routeEntryBaseIndex      = 0;
            defUcRouteEntryInfo[i].ipLttEntry.ucRPFCheckEnable         = GT_FALSE;
            defUcRouteEntryInfo[i].ipLttEntry.sipSaCheckMismatchEnable = GT_FALSE;
            defUcRouteEntryInfo[i].ipLttEntry.ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;

            /* set defMcLttEntryInfo */
            defMcRouteLttEntry[i].ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
            defMcRouteLttEntry[i].numOfPaths = 0;
            defMcRouteLttEntry[i].routeEntryBaseIndex = 1;
            defMcRouteLttEntry[i].routeType = 0;
            defMcRouteLttEntry[i].sipSaCheckMismatchEnable = GT_FALSE;
            defMcRouteLttEntry[i].ucRPFCheckEnable = GT_FALSE;
        }
        /* disable Unicast IPv4 Routing on a Port */
        rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[prvTgfTxPortsIdxArray[vlanIter]],
                                       CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, GT_FALSE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d %d",
                                     prvTgfDevNum, prvTgfPortsArray[prvTgfTxPortsIdxArray[vlanIter]]);

        /* disable IPv4 Unicast Routing on Vlan */
        rc = prvTgfIpVlanRoutingEnable(prvTgfVlanArrays[vlanIter], CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, GT_FALSE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                     prvTgfDevNum, prvTgfVlanArrays[vlanIter]);
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
    for (vlanIter = 0; vlanIter < PRV_TGF_VLAN_COUNT_CNS; vlanIter++)
    {
        /* invalidate vlan entry */
        rc = prvTgfBrgVlanEntryInvalidate(prvTgfVlanArrays[vlanIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d, %d",
                                     prvTgfDevNum, prvTgfVlanArrays[vlanIter]);
    }

    /* disable VLAN based MAC learning */
    rc = prvTgfBrgVlanLearningStateSet(PRV_TGF_SEND_VLANID_CNS, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanLearningStateSet: %d", prvTgfDevNum);
}

