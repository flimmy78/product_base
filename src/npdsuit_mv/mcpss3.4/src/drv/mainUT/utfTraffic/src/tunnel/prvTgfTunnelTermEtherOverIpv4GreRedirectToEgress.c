/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgress.c
*
* DESCRIPTION:
*       Tunnel term Ethernet over Ipv4 GRE redirect to egress
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
#include <common/tgfTunnelGen.h>
#include <tunnel/prvTgfTunnelTermEtherOverIpv4GreRedirectToEgress.h>

/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* default VLAN Id */
#define PRV_TGF_VLANID_5_CNS            5

/* default VLAN Id */
#define PRV_TGF_VLANID_6_CNS            6

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS       0

/* egress port number to receive traffic from */
#define PRV_TGF_EGR_PORT_IDX_CNS        1

/* number of ports */
#define PRV_TGF_PORT_COUNT_CNS          4

/* default tunnel term entry index */
#define PRV_TGF_TTI_INDEX_CNS           2

/* number of vfd's (tagged vlans) */
#define PRV_TGF_VFD_NUM_TAGGED_CNS      4

/* number of vfd's (untagged vlans) */
#define PRV_TGF_VFD_NUM_UNTAGGED_CNS    2

/* default number of packets to send */
static GT_U32 prvTgfBurstCount = 3;

/* expected number of packets on ports */
static GT_U8 prvTgfPacketsCountRxTxArr[PRV_TGF_PORTS_NUM_CNS] = { 1, 1, 0, 0 };

/* flag to indecate mac entry state */
static GT_BOOL isMacEntryCreated = GT_FALSE;

/****************************** Test packet (ipv4) *****************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},   /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}    /* saMac */
};

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,        /* etherType */
    0, 0, PRV_TGF_VLANID_5_CNS              /* pri, cfi, VlanId */
};

/* packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4Part =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x58,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    0x2f,               /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS,/* csum */
    {10, 10, 10, 10},   /* srcAddr */
    { 4,  4,  4,  4}    /* dstAddr */
};

/***************** Incapsulation ethernet frame **********************/
/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2PartInt =
{
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x22},    /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x25}     /* saMac */
};

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPartInt =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,        /* etherType */
    0, 0, PRV_TGF_VLANID_5_CNS              /* pri, cfi, VlanId */
};

/* DATA of packet */
static GT_U8 prvTgfPayloadDataArr[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart =
{
    sizeof(prvTgfPayloadDataArr),  /* dataLength */
    prvTgfPayloadDataArr           /* dataPtr */
};

/* GRE array */
static GT_U8 prvTgfPacketGreDataArr[] = {0x00, 0x00, 0x65, 0x58};

/* GRE part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketGrePart =
{
    sizeof(prvTgfPacketGreDataArr),  /* dataLength */
    prvTgfPacketGreDataArr           /* dataPtr */
};

/*********************************** packet to send *******************************/
/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketGrePart},
    /* incapsulation ethernet frame part */
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2PartInt},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPartInt},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET_LEN_CNS \
      TGF_L2_HEADER_SIZE_CNS * 2  + \
      TGF_VLAN_TAG_SIZE_CNS  * 2  + \
      sizeof(prvTgfPacketGrePart) + \
      TGF_ETHERTYPE_SIZE_CNS      + \
      TGF_IPV4_HEADER_SIZE_CNS    + \
      sizeof(prvTgfPayloadDataArr)

/* PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfo =
{
    PRV_TGF_PACKET_LEN_CNS,                  /* totalLen */
    (sizeof(prvTgfPacketPartArray)
        / sizeof(prvTgfPacketPartArray[0])), /* numOfParts */
    prvTgfPacketPartArray                    /* partsArray */
};


/*************************** Restore config ***********************************/

/* parameters that is needed to be restored */
static struct
{
    PRV_TGF_TTI_MAC_MODE_ENT      macMode;
    GT_U32                        ethType1;
    GT_U32                        ethType2;
} prvTgfRestoreCfg;

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressBridgeConfigSet
*
* DESCRIPTION:
*       Set Bridge Configuration
*
* INPUTS:
*       isTaggedVlansUsed - to mark vlans tagged or not
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressBridgeConfigSet
(
    IN GT_BOOL isTaggedVlansUsed
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       tagArray[] = {0, 0};

    /* add tags */
    tagArray[0] = (GT_U8)isTaggedVlansUsed;
    tagArray[1] = (GT_U8)isTaggedVlansUsed;

    PRV_UTF_LOG0_MAC("======= Setting Bridge Configuration =======\n");

    /* set default vlan entry for vlan ID 5 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_5_CNS, prvTgfPortsArray,
                                           NULL, tagArray, 2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
                            "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* set default vlan entry for vlan ID 6 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_6_CNS, prvTgfPortsArray + 2,
                                           NULL, tagArray, 2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
                            "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* check is mac entry created */
    if(!isMacEntryCreated)
    {
        /* create FDB macEntry */
        rc = prvTgfBrgDefFdbMacEntryOnVidSet(prvTgfPacketL2Part.daMac,
                                PRV_TGF_VLANID_5_CNS, PRV_TGF_VLANID_5_CNS, GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
                                    "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);

        /* MAC entry successfully created */
        isMacEntryCreated = GT_TRUE;
    }
}

/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTtiConfigSet
*
* DESCRIPTION:
*       Set TTI test settings:
*           - Enable port 0 for Eth lookup
*           - Set Eth key lookup MAC mode to Mac DA
*           - Set TTI rule action
*           - Set TTI rule
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong devNum
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTtiConfigSet
(
    GT_VOID
)
{
    GT_STATUS                rc = GT_OK;
    PRV_TGF_TTI_MAC_MODE_ENT macMode = PRV_TGF_TTI_MAC_MODE_DA_E;
    PRV_TGF_TTI_RULE_UNT     pattern;
    PRV_TGF_TTI_RULE_UNT     mask;
    PRV_TGF_TTI_ACTION_STC   ruleAction = {0};
    GT_U32  ethType1 = 0x6558;
    GT_U32  ethType2 = 0x6677;
    GT_U32  ethTypeGet1;
    GT_U32  ethTypeGet2;

    /* clear pattern and mask */
    cpssOsMemSet((GT_VOID*) &pattern, 0, sizeof(pattern));
    cpssOsMemSet((GT_VOID*) &mask,    0, sizeof(mask));

    /* enable TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                      PRV_TGF_TTI_KEY_IPV4_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* save the current gre ethernet type 1 */
    rc = prvTgfTtiIpv4GreEthTypeGet(prvTgfDevNum, CPSS_TUNNEL_GRE0_ETHER_TYPE_E,
                                    &prvTgfRestoreCfg.ethType1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiIpv4GreEthTypeGet: %d", prvTgfDevNum);

    /* set gre ethernet type 1 */
    rc = prvTgfTtiIpv4GreEthTypeSet(prvTgfDevNum,
                                    CPSS_TUNNEL_GRE0_ETHER_TYPE_E, ethType1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiIpv4GreEthTypeSet: %d",
                                 prvTgfDevNum);

    /* get gre ethernet type 1 */
    rc = prvTgfTtiIpv4GreEthTypeGet(prvTgfDevNum,
                                    CPSS_TUNNEL_GRE0_ETHER_TYPE_E, &ethTypeGet1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiIpv4GreEthTypeGet: %d",
                                 prvTgfDevNum);
    /* check ethTypeGet 1 */
    UTF_VERIFY_EQUAL0_STRING_MAC(ethType1, ethTypeGet1, "wrong ethType1 get");

    /* save the current gre ethernet type 2 */
    rc = prvTgfTtiIpv4GreEthTypeGet(prvTgfDevNum, CPSS_TUNNEL_GRE1_ETHER_TYPE_E,
                                    &prvTgfRestoreCfg.ethType2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiIpv4GreEthTypeGet: %d", prvTgfDevNum);

    /* set gre ethernet type 2 */
    rc = prvTgfTtiIpv4GreEthTypeSet(prvTgfDevNum,
                                    CPSS_TUNNEL_GRE1_ETHER_TYPE_E, ethType2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiIpv4GreEthTypeSet: %d",
                                 prvTgfDevNum);

    /* get gre ethernet type 2 */
    rc = prvTgfTtiIpv4GreEthTypeGet(prvTgfDevNum,
                                    CPSS_TUNNEL_GRE1_ETHER_TYPE_E, &ethTypeGet2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiIpv4GreEthTypeGet: %d",
                                 prvTgfDevNum);
    /* check ethTypeGet 2 */
    UTF_VERIFY_EQUAL0_STRING_MAC(ethType2, ethTypeGet2, "wrong ethType2 get");

    /* save the current lookup Mac mode for IPv4 and DA */
    rc = prvTgfTtiMacModeGet(prvTgfDevNum, PRV_TGF_TTI_KEY_IPV4_E, &prvTgfRestoreCfg.macMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMacModeGet: %d", prvTgfDevNum);

    /* set MAC mode */
    rc = prvTgfTtiMacModeSet(PRV_TGF_TTI_KEY_IPV4_E, macMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMacModeSet: %d", prvTgfDevNum);

    /* configure TTI rule action */
    ruleAction.tunnelTerminate       = GT_TRUE;
    ruleAction.passengerPacketType   = PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E;
    ruleAction.tsPassengerPacketType = PRV_TGF_TUNNEL_PASSENGER_ETHERNET_E;

    ruleAction.copyTtlFromTunnelHeader = GT_FALSE;

    ruleAction.mplsCmd = PRV_TGF_TTI_MPLS_NOP_COMMAND_E;

    ruleAction.command         = CPSS_PACKET_CMD_FORWARD_E;
    ruleAction.redirectCommand = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;

    ruleAction.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    ruleAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;
    ruleAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    ruleAction.tunnelStart    = GT_FALSE;
    ruleAction.tunnelStartPtr = 0;

    ruleAction.tag0VlanCmd = PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E;
    ruleAction.tag0VlanId  = 0;
    ruleAction.tag1VlanCmd = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;

    ruleAction.modifyUpEnable = PRV_TGF_TTI_MODIFY_UP_ENABLE_E;

    ruleAction.userDefinedCpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP3_E;


    /* configure TTI rule */
    pattern.ipv4.common.pclId    = 1;
    pattern.ipv4.common.srcIsTrunk = GT_FALSE;
    pattern.ipv4.common.srcPortTrunk = prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS];
    cpssOsMemCpy((GT_VOID*)pattern.ipv4.common.mac.arEther,
                 (GT_VOID*)prvTgfPacketL2Part.daMac,
                 sizeof(pattern.ipv4.common.mac));
    pattern.ipv4.common.vid      = PRV_TGF_VLANID_5_CNS;
    pattern.ipv4.common.isTagged = GT_TRUE;
    pattern.ipv4.tunneltype = 4;
    pattern.ipv4.isArp = GT_FALSE;

    cpssOsMemCpy(pattern.ipv4.destIp.arIP, prvTgfPacketIpv4Part.dstAddr,
                 sizeof(pattern.ipv4.destIp.arIP));
    cpssOsMemCpy(pattern.ipv4.srcIp.arIP, prvTgfPacketIpv4Part.srcAddr,
                 sizeof(pattern.ipv4.srcIp.arIP));

    /* configure TTI rule mask */
    mask.ipv4.common.pclId = 0xFFF;
    mask.ipv4.common.srcIsTrunk = GT_TRUE;
    mask.ipv4.common.srcPortTrunk = 0xFFFF;
    cpssOsMemSet((GT_VOID*)mask.ipv4.common.mac.arEther, 0xFF,
                 sizeof(mask.ipv4.common.mac.arEther));
    mask.ipv4.common.vid = 0xFFFF;
    mask.ipv4.common.isTagged = GT_TRUE;

    mask.ipv4.tunneltype = 0xFFFF;
    mask.ipv4.isArp = GT_TRUE;
    cpssOsMemSet(mask.ipv4.srcIp.arIP,  0xFF, sizeof(mask.ipv4.srcIp.arIP));
    cpssOsMemSet(mask.ipv4.destIp.arIP, 0xFF, sizeof(mask.ipv4.destIp.arIP));


    /* set TTI rule */
    rc = prvTgfTtiRuleSet(PRV_TGF_TTI_INDEX_CNS, PRV_TGF_TTI_KEY_IPV4_E,
                          &pattern, &mask, &ruleAction);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleSet");
}


/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTrafficGenerateTagged
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:01,
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTrafficGenerateTagged
(
    GT_VOID
)
{
    GT_STATUS       rc = GT_OK;
    GT_U32          portsCount  = PRV_TGF_PORT_COUNT_CNS;
    GT_U32          portIter    = 0;
    GT_U32          packetSize  = 0;
    GT_U32          packetIter  = 0;
    GT_U32          numTriggers = 0;

    TGF_VFD_INFO_STC         vfdArray[PRV_TGF_VFD_NUM_TAGGED_CNS] = {{0}};
    CPSS_INTERFACE_INFO_STC  portInterface;

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* -------------------------------------------------------------------------
     * 2. Generating Traffic
     */

    /* send packet */
    rc = prvTgfTransmitPacketsWithCapture(prvTgfDevNum,
                    prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                    &prvTgfPacketInfo, prvTgfBurstCount, 0, NULL,
                    prvTgfDevNum,
                    prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS],
                    TGF_CAPTURE_MODE_MIRRORING_E, 10);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, &d\n",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* -------------------------------------------------------------------------
     * 3. Get Ethernet Counters
     */

    /* read and check ethernet counters */
    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
    {
        /* calculate packet length */
        packetSize = prvTgfPacketInfo.totalLen -
         (sizeof(prvTgfPacketGreDataArr) +  /* gre part dataLength */
                  TGF_L2_HEADER_SIZE_CNS  +
                  TGF_VLAN_TAG_SIZE_CNS   +
                  TGF_ETHERTYPE_SIZE_CNS  +
                  TGF_IPV4_HEADER_SIZE_CNS)
                                * (portIter == PRV_TGF_EGR_PORT_IDX_CNS);

        /* check ETH counters */
        rc = prvTgfEthCountersCheck(prvTgfDevNum,
                                    prvTgfPortsArray[portIter],
                                    prvTgfPacketsCountRxTxArr[portIter],
                                    prvTgfPacketsCountRxTxArr[portIter],
                                    packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfEthCountersCheck");
    }

    /* -------------------------------------------------------------------------
     * 4. Get Trigger Counters
     */

    /* setup nexthope portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters */

    /* check da mac */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].cycleCount = sizeof(prvTgfPacketL2PartInt.daMac);
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfPacketL2PartInt.daMac,
                 sizeof(prvTgfPacketL2PartInt.daMac));

    /* check sa mac */
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset     = vfdArray[0].offset + vfdArray[0].cycleCount;
    vfdArray[1].cycleCount = sizeof(prvTgfPacketL2PartInt.saMac);
    cpssOsMemCpy(vfdArray[1].patternPtr, prvTgfPacketL2PartInt.saMac,
                 sizeof(prvTgfPacketL2PartInt.saMac));

    /* check ethertype */
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].offset     = vfdArray[1].offset + vfdArray[1].cycleCount;
    vfdArray[2].cycleCount = TGF_ETHERTYPE_SIZE_CNS;
    vfdArray[2].patternPtr[0] = TGF_ETHERTYPE_8100_VLAN_TAG_CNS >> 8;
    vfdArray[2].patternPtr[1] = TGF_ETHERTYPE_8100_VLAN_TAG_CNS & 0xFF;

    /* check vlan id */
    vfdArray[3].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[3].offset     = vfdArray[2].offset + vfdArray[2].cycleCount;
    vfdArray[3].cycleCount = 2;
    vfdArray[3].patternPtr[0] = 0;
    vfdArray[3].patternPtr[1] = PRV_TGF_VLANID_5_CNS;

    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface,
                           PRV_TGF_VFD_NUM_TAGGED_CNS, vfdArray, &numTriggers);
    PRV_UTF_LOG1_MAC("numTriggers = %d\n\n", numTriggers);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
                    "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                    portInterface.devPort.devNum, portInterface.devPort.portNum);

    for(packetIter = 0; packetIter < prvTgfBurstCount; packetIter++)
    {
        /* check if captured packet has all correct vfd's */
        UTF_VERIFY_EQUAL1_STRING_MAC((1 << PRV_TGF_VFD_NUM_TAGGED_CNS) - 1,
                      numTriggers & ((1 << PRV_TGF_VFD_NUM_TAGGED_CNS) - 1),
            "   Errors while triggers summary result checking:"
            " packetIter =  %d\n", packetIter);

        /* check if captured packet has the same MAC DA as prvTgfArpMac */
        UTF_VERIFY_EQUAL6_STRING_MAC(GT_TRUE, numTriggers & BIT_0,
                "\n   MAC DA of captured packet must be: %02X:%02X:%02X:%02X:%02X:%02X",
                                        prvTgfPacketL2PartInt.daMac[0],
                                        prvTgfPacketL2PartInt.daMac[1],
                                        prvTgfPacketL2PartInt.daMac[2],
                                        prvTgfPacketL2PartInt.daMac[3],
                                        prvTgfPacketL2PartInt.daMac[4],
                                        prvTgfPacketL2PartInt.daMac[5]);

        /* check if captured packet has the same MAC SA as prvTgfArpMac */
        UTF_VERIFY_EQUAL6_STRING_MAC(GT_TRUE, (numTriggers & BIT_1) >> 1,
                "\n   MAC SA of captured packet must be: %02X:%02X:%02X:%02X:%02X:%02X",
                                        prvTgfPacketL2PartInt.saMac[0],
                                        prvTgfPacketL2PartInt.saMac[1],
                                        prvTgfPacketL2PartInt.saMac[2],
                                        prvTgfPacketL2PartInt.saMac[3],
                                        prvTgfPacketL2PartInt.saMac[4],
                                        prvTgfPacketL2PartInt.saMac[5]);

        /* check if captured packet has the same ethertype */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, (numTriggers & BIT_2) >> 2,
                        "\n   ehertype tag of captured packet must be: %04X",
                        TGF_ETHERTYPE_8100_VLAN_TAG_CNS);

        /* check if captured packet has the same vlan tag */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, (numTriggers & BIT_3) >> 3,
                        "\n   vlan tag of captured packet must be: %04X", 0x0005);

        /* prepare next iteration */
        numTriggers = numTriggers >> PRV_TGF_VFD_NUM_TAGGED_CNS;
    }
}

/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTrafficGenerateUntagged
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:01,
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTrafficGenerateUntagged
(
    GT_VOID
)
{
    GT_STATUS       rc = GT_OK;
    GT_U32          portsCount  = PRV_TGF_PORT_COUNT_CNS;
    GT_U32          portIter    = 0;
    GT_U32          packetSize  = 0;
    GT_U32          packetIter  = 0;
    GT_U32          numTriggers = 0;
    TGF_VFD_INFO_STC              vfdArray[PRV_TGF_VFD_NUM_UNTAGGED_CNS];
    CPSS_INTERFACE_INFO_STC       portInterface;

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* clear triggers */
    cpssOsMemSet((GT_VOID*) vfdArray, 0, sizeof(vfdArray));

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* -------------------------------------------------------------------------
     * 2. Generating Traffic
     */

    /* send packet */
    rc = prvTgfTransmitPacketsWithCapture(prvTgfDevNum,
                    prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                    &prvTgfPacketInfo, prvTgfBurstCount, 0, NULL,
                    prvTgfDevNum,
                    prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS],
                    TGF_CAPTURE_MODE_MIRRORING_E, 10);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, &d\n",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* -------------------------------------------------------------------------
     * 3. Get Ethernet Counters
     */

    /* read and check ethernet counters */
    for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
    {
        /* calculate packet length */
        packetSize = prvTgfPacketInfo.totalLen -
         (sizeof(prvTgfPacketGreDataArr) +  /* gre part dataLength */
                  TGF_L2_HEADER_SIZE_CNS  +
                  TGF_VLAN_TAG_SIZE_CNS * 2 +
                  TGF_ETHERTYPE_SIZE_CNS  +
                  TGF_IPV4_HEADER_SIZE_CNS)
                                * (portIter == PRV_TGF_EGR_PORT_IDX_CNS);

        /* check ETH counters */
        rc = prvTgfEthCountersCheck(prvTgfDevNum,
                                    prvTgfPortsArray[portIter],
                                    prvTgfPacketsCountRxTxArr[portIter],
                                    prvTgfPacketsCountRxTxArr[portIter],
                                    packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfEthCountersCheck");
    }

    /* -------------------------------------------------------------------------
     * 4. Get Trigger Counters
     */

    /* setup nexthope portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* get trigger counters */

    /* check da mac */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].cycleCount = sizeof(prvTgfPacketL2PartInt.daMac);
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfPacketL2PartInt.daMac,
                 sizeof(prvTgfPacketL2PartInt.daMac));

    /* check sa mac */
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset     = vfdArray[0].offset + vfdArray[0].cycleCount;
    vfdArray[1].cycleCount = sizeof(prvTgfPacketL2PartInt.saMac);
    cpssOsMemCpy(vfdArray[1].patternPtr, prvTgfPacketL2PartInt.saMac,
                 sizeof(prvTgfPacketL2PartInt.saMac));

    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface,
                           PRV_TGF_VFD_NUM_UNTAGGED_CNS, vfdArray, &numTriggers);
    PRV_UTF_LOG1_MAC("numTriggers = %d\n\n", numTriggers);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
                    "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                    portInterface.devPort.devNum, portInterface.devPort.portNum);

    for(packetIter = 0; packetIter < prvTgfBurstCount; packetIter++)
    {
        /* check if captured packet has all correct vfd's */
        UTF_VERIFY_EQUAL1_STRING_MAC((1 << PRV_TGF_VFD_NUM_UNTAGGED_CNS) - 1,
                      numTriggers & ((1 << PRV_TGF_VFD_NUM_UNTAGGED_CNS) - 1),
            "   Errors while triggers summary result checking:"
            " packetIter =  %d\n", packetIter);

        /* check if captured packet has the same MAC DA as prvTgfArpMac */
        UTF_VERIFY_EQUAL6_STRING_MAC(GT_TRUE, numTriggers & BIT_0,
                "\n   MAC DA of captured packet must be: %02X:%02X:%02X:%02X:%02X:%02X",
                                        prvTgfPacketL2PartInt.daMac[0],
                                        prvTgfPacketL2PartInt.daMac[1],
                                        prvTgfPacketL2PartInt.daMac[2],
                                        prvTgfPacketL2PartInt.daMac[3],
                                        prvTgfPacketL2PartInt.daMac[4],
                                        prvTgfPacketL2PartInt.daMac[5]);

        /* check if captured packet has the same MAC SA as prvTgfArpMac */
        UTF_VERIFY_EQUAL6_STRING_MAC(GT_TRUE, (numTriggers & BIT_1) >> 1,
                "\n   MAC SA of captured packet must be: %02X:%02X:%02X:%02X:%02X:%02X",
                                        prvTgfPacketL2PartInt.saMac[0],
                                        prvTgfPacketL2PartInt.saMac[1],
                                        prvTgfPacketL2PartInt.saMac[2],
                                        prvTgfPacketL2PartInt.saMac[3],
                                        prvTgfPacketL2PartInt.saMac[4],
                                        prvTgfPacketL2PartInt.saMac[5]);

        /* prepare next iteration */
        numTriggers = numTriggers >> PRV_TGF_VFD_NUM_UNTAGGED_CNS;
    }
}


/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressConfigurationRestore
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;


    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* Invalidate TTI rule */
    rc = prvTgfTtiRuleValidStatusSet(PRV_TGF_TTI_INDEX_CNS, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleValidStatusSet: %d", prvTgfDevNum);

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* Invalidate VLAN entry 6 */
    rc = prvTgfBrgVlanEntryInvalidate(PRV_TGF_VLANID_6_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d", prvTgfDevNum);

    /* Invalidate VLAN entry 5 */
    rc = prvTgfBrgVlanEntryInvalidate(PRV_TGF_VLANID_5_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d", prvTgfDevNum);

    /* restore the lookup Mac mode for IPv4 and DA */
    rc = prvTgfTtiMacModeSet(PRV_TGF_TTI_KEY_IPV4_E, prvTgfRestoreCfg.macMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMacModeSet: %d", prvTgfDevNum);

    /* disable the TTI lookup for IPv4 at the port */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_TTI_KEY_IPV4_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* restore gre ethernet type 1 */
    rc = prvTgfTtiIpv4GreEthTypeSet(prvTgfDevNum,
                                    CPSS_TUNNEL_GRE0_ETHER_TYPE_E, prvTgfRestoreCfg.ethType1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiIpv4GreEthTypeSet: %d",
                                 prvTgfDevNum);

    /* restore gre ethernet type 2 */
    rc = prvTgfTtiIpv4GreEthTypeSet(prvTgfDevNum,
                                    CPSS_TUNNEL_GRE1_ETHER_TYPE_E, prvTgfRestoreCfg.ethType2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiIpv4GreEthTypeSet: %d",
                                 prvTgfDevNum);
}

