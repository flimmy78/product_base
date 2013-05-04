/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfDetailedMimTunnelStart.c
*
* DESCRIPTION:
*       Verify the functionality of UP marking in MIM tunnel start
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

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfTunnelGen.h>
#include <common/tgfCosGen.h>
#include <tunnel/prvTgfDetailedMimTunnelStart.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* port to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS       0

/* egress port number  */
#define PRV_TGF_EGR_PORT_IDX_CNS        3

/* default VLAN Id 5 */
#define PRV_TGF_VLANID_5_CNS        5

/* default VLAN Id 6 */
#define PRV_TGF_VLANID_6_CNS        6

/* MAC in MAC payload size (in bytes) */
#define TGF_MIM_PAYLOAD_SIZE_CNS    4

/* default tunnel start pointer */
#define PRV_TGF_TS_PTR_CNS          1

/* default tunnel start entry index */
#define PRV_TGF_TS_INDEX_CNS        1

/* default TPID table entry index */
#define PRV_TGF_TPID_INDEX_CNS      1

/* default tunnel term entry index */
#define PRV_TGF_TTI_INDEX_CNS       2

/* default number of packets to send */
static GT_U32   prvTgfBurstCount = 1;

/*************************** Packet to send ***********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x22}                 /* saMac */
};

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_5_CNS                          /* pri, cfi, VlanId */
};

/* packet's Ipv4 ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketIpv4EtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4Part =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2E,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    0x04,               /* protocol */
    0x4C9D,             /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
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

/************************* Packet parts ***************************************/

/* PARTS of the packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET_LEN_CNS                                                 \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS +  \
    TGF_IPV4_HEADER_SIZE_CNS + sizeof(prvTgfPacketPayloadDataArr)

/* Length of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* Packet to send */
static TGF_PACKET_STC prvTgfPacketInfo =
{
    PRV_TGF_PACKET_LEN_CNS,                                           /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(prvTgfPacketPartArray[0]), /* numOfParts */
    prvTgfPacketPartArray                                             /* partsArray */
};


/******************************************************************************\
 *                            Private variables                               *
\******************************************************************************/

/* MAC for Tunnel Start Entry */
static TGF_MAC_ADDR prvTgfStartEntryMac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};

/* stored default VID */
static GT_U16   prvTgfDefVlanId = 0;

/* stored default TS entry */
static struct
{
    GT_U16                         etherType;
} prvTgfDefTunnelStartEntryStc;


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfBridgeConfigSet
*
* DESCRIPTION:
*       Set initial test settings:
*           - Create VLAN 5 with ports 0, 8
*           - Create VLAN 6 with ports 18, 23
*           - Set VLAN 5 as port 0 default VLAN ID (PVID)
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
static GT_STATUS prvTgfBridgeConfigSet
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    /* set default vlan entry for vlan ID 5 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_5_CNS, prvTgfPortsArray,
                                           NULL, NULL, prvTgfPortsNum / 2);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgDefVlanEntryWithPortsSet");

    /* set default vlan entry for vlan ID 6 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_6_CNS, prvTgfPortsArray + 2,
                                           NULL, NULL, prvTgfPortsNum / 2);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgDefVlanEntryWithPortsSet");

    /* get default port vlanId */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfDefVlanId);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgVlanPortVidGet");

    /* set VLAN 5 as port 0 default VLAN ID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_VLANID_5_CNS);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgVlanPortVidSet");

    return rc;
}

/*******************************************************************************
* prvTgfTtiConfigSet
*
* DESCRIPTION:
*       Set TTI test settings:
*           - Enable port 0 for Eth lookup
*           - Set Eth key lookup MAC mode to Mac DA
*           - Set TTI rule action
*           - Set TTI rule
*
* INPUTS:
*       macMode - MAC mode to use
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
static GT_STATUS prvTgfTtiConfigSet
(
    IN  PRV_TGF_TTI_MAC_MODE_ENT      macMode
)
{
    GT_STATUS                   rc = GT_OK;
    PRV_TGF_TTI_RULE_UNT        pattern;
    PRV_TGF_TTI_RULE_UNT        mask;
    PRV_TGF_TTI_ACTION_STC      ruleAction = {0};


    /* enable TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                      PRV_TGF_TTI_KEY_ETH_E, GT_TRUE);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiPortLookupEnableSet");

    /* set MAC mode */
    rc = prvTgfTtiMacModeSet(PRV_TGF_TTI_KEY_ETH_E, macMode);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiMacModeSet");

    /* clear entry */
    cpssOsMemSet((GT_VOID*) &ruleAction, 0, sizeof(ruleAction));
    cpssOsMemSet((GT_VOID*) &pattern,    0, sizeof(pattern));
    cpssOsMemSet((GT_VOID*) &mask,       0, sizeof(mask));

    /* configure TTI rule action */
    ruleAction.tunnelTerminate       = GT_FALSE;
    ruleAction.passengerPacketType   = PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E;
    ruleAction.tsPassengerPacketType = PRV_TGF_TUNNEL_PASSENGER_ETHERNET_E;

    ruleAction.command         = CPSS_PACKET_CMD_FORWARD_E;
    ruleAction.redirectCommand = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;

    ruleAction.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    ruleAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;
    ruleAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    ruleAction.tunnelStart    = GT_TRUE;
    ruleAction.tunnelStartPtr = PRV_TGF_TS_PTR_CNS;

    ruleAction.tag0VlanCmd = PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E;
    ruleAction.tag0VlanId  = PRV_TGF_VLANID_6_CNS;
    ruleAction.tag1VlanCmd = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;

    ruleAction.modifyUpEnable = PRV_TGF_TTI_MODIFY_UP_ENABLE_E;
    ruleAction.tag1UpCommand  = PRV_TGF_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E;

    ruleAction.userDefinedCpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP3_E;
    ruleAction.bridgeBypass       = GT_TRUE;
    ruleAction.ingressPipeBypass  = GT_TRUE;
    ruleAction.actionStop         = GT_TRUE;

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
            PRV_UTF_VERIFY_GT_OK(rc, "Get invalid macMode");
    }

    pattern.eth.common.vid      = PRV_TGF_VLANID_5_CNS;
    pattern.eth.common.pclId    = 3;
    pattern.eth.common.isTagged = GT_TRUE;

    /* configure TTI rule mask */
    cpssOsMemSet((GT_VOID*)mask.eth.common.mac.arEther, 0xFF,
                 sizeof(mask.eth.common.mac.arEther));
    cpssOsMemSet((GT_VOID*) &(mask.eth.common.vid), 0xFF,
                 sizeof(mask.eth.common.vid));
    cpssOsMemSet((GT_VOID*) &(mask.eth.common.pclId), 0xFF,
                 sizeof(mask.eth.common.pclId));

    /* set TTI rule */
    rc = prvTgfTtiRuleSet(PRV_TGF_TTI_INDEX_CNS, PRV_TGF_TTI_KEY_ETH_E,
                          &pattern, &mask, &ruleAction);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiRuleSet");

    return rc;
}

/*******************************************************************************
* prvTgfTunnelStartConfigSet
*
* DESCRIPTION:
*       Set TS test settings:
*           - Tunnel type: MIM
*           - Tag enable: true
*           - VLAN: 6
*           - UP mark mode: according to entry
*           - UP: 0x3
*           - MAC:  00:00:00:00:00:11
*           - TTL: 64
*           - Retain CRC: true
*           - iSid assign mode: according to entry
*           - iSid: 0x123456
*           - iUP mark mode: according to entry
*           - iUP: 0x5
*           - iDP mark mode: according to entry
*           - iDP: 0
*
* INPUTS:
*       qosType - type of QoS
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
static GT_STATUS prvTgfTunnelStartConfigSet
(
    IN CPSS_COS_PARAM_ENT       qosType
)
{
    GT_STATUS                       rc = GT_OK;
    PRV_TGF_TUNNEL_START_ENTRY_UNT  tsConfig;


    /* sets the MIM Ethernet type */
    rc = prvTgfTtiMimEthTypeSet(TGF_ETHERTYPE_88E7_MIM_TAG_CNS) ;
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiMimEthTypeSet");

    /* save default TPID table entry */
    rc = prvTgfBrgVlanTpidEntryGet(CPSS_DIRECTION_EGRESS_E,
                                   PRV_TGF_TPID_INDEX_CNS,
                                   &(prvTgfDefTunnelStartEntryStc.etherType));
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgVlanTpidEntryGet");

    /* sets TPID table entry */
    rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                   PRV_TGF_TPID_INDEX_CNS,
                                   TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgVlanTpidEntrySet");

    /* sets index of TPID */
    rc = prvTgfBrgVlanPortEgressTpidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS],
                                        CPSS_VLAN_ETHERTYPE0_E, PRV_TGF_TPID_INDEX_CNS);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgVlanPortEgressTpidSet");

    /* clear entry */
    cpssOsMemSet((GT_VOID*) &tsConfig, 0, sizeof(tsConfig));

    /* configure TTI entry */
    tsConfig.mimCfg.tagEnable      = GT_TRUE;
    tsConfig.mimCfg.vlanId         = PRV_TGF_VLANID_6_CNS;
    tsConfig.mimCfg.upMarkMode     = CPSS_COS_PARAM_UP_E == qosType ? PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E : PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
    tsConfig.mimCfg.up             = 0x3;
    tsConfig.mimCfg.ttl            = 0x40;
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(0,
                                               PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E))
    {
        tsConfig.mimCfg.retainCrc      = GT_FALSE;
    }
    else
    {
        tsConfig.mimCfg.retainCrc      = GT_TRUE;
    }
    tsConfig.mimCfg.iSidAssignMode = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
    tsConfig.mimCfg.iSid           = 0x123456;
    tsConfig.mimCfg.iUpMarkMode    = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
    tsConfig.mimCfg.iUp            = 0x5;
    tsConfig.mimCfg.iDpMarkMode    = CPSS_COS_PARAM_DP_E == qosType ? PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E : PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
    tsConfig.mimCfg.iDp            = 0x0;

    cpssOsMemCpy((GT_VOID*) tsConfig.mimCfg.macDa.arEther,
                 (GT_VOID*) prvTgfStartEntryMac,
                 sizeof(tsConfig.mimCfg.macDa.arEther));

    /* set TTI entry */
    rc = prvTgfTunnelStartEntrySet(PRV_TGF_TS_INDEX_CNS, CPSS_TUNNEL_MAC_IN_MAC_E, &tsConfig);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntrySet");

    return rc;
}

/*******************************************************************************
* prvTgfTunnelStartTestInit
*
* DESCRIPTION:
*       Set initial test settings:
*           - Create VLAN 5 with ports 0, 8
*           - Create VLAN 6 with ports 18, 23
*           - Set VLAN 5 as port 0 default VLAN ID (PVID)
*           - Enable port 0 for Eth lookup
*           - Set Eth key lookup MAC mode to Mac DA
*           - Set TTI rule action
*           - Set TTI rule
*           - Set tunnel start entry
*
* INPUTS:
*       macMode - MAC mode to use
*       qosType - type of QoS
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
static GT_VOID prvTgfTunnelStartTestInit
(
    IN PRV_TGF_TTI_MAC_MODE_ENT       macMode,
    IN CPSS_COS_PARAM_ENT             qosType
)
{
    GT_STATUS               rc         = GT_OK;
    GT_U8                   index      = 0;
    PRV_TGF_COS_PROFILE_STC cosProfile = {0};
    CPSS_QOS_ENTRY_STC      portQosCfg = {0};


    /* set bridge configuration */
    rc = prvTgfBridgeConfigSet();
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBridgeConfigSet: %d", prvTgfDevNum);

    /* set TS configuration */
    rc = prvTgfTunnelStartConfigSet(qosType);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTunnelStartConfigSet: %d", prvTgfDevNum);

    /* set TTI configuration */
    rc = prvTgfTtiConfigSet(macMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiConfigSet: %d", prvTgfDevNum);

    /* set QoS profile */
    if (qosType <= CPSS_COS_PARAM_DP_E)
    {
        /* configure cos profile */
        cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority   = 0x7;

        /* set cos configuration */
        index = 0;

        rc = prvTgfCosProfileEntrySet(index, &cosProfile);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

        /* configure QoS attributes */
        portQosCfg.qosProfileId     = index;
        portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;

        rc = prvTgfCosPortQosConfigSet(prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS], &portQosCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosConfigSet: %d", prvTgfDevNum);

        if (CPSS_COS_PARAM_DP_E == qosType)
        {
            /* maps Drop Precedence to Drop Eligibility Indicator bit */
            rc = prvTgfCosDpToCfiDeiMapSet(CPSS_DP_GREEN_E, 0);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosDpToCfiDeiMapSet: %d\n", prvTgfDevNum);
        }
    }
}

/*******************************************************************************
* prvTgfTunnelStartTestPacketSend
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
static GT_VOID prvTgfTunnelStartTestPacketSend
(
    IN  GT_U8                         portNum,
    IN  TGF_PACKET_STC               *packetInfoPtr
)
{
    GT_STATUS               rc            = GT_OK;
    GT_U32                  portsCount    = prvTgfPortsNum;
    GT_U32                  portIter      = 0;


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

    /* send Packet from port 0 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, portNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfStartTransmitingEth: %d", prvTgfDevNum);

    /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
}

/*******************************************************************************
* prvTgfTunnelStartTestReset
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
static GT_VOID prvTgfTunnelStartTestReset
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    PRV_UTF_LOG0_MAC("======= Restore Configuration =======\n");

    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* disable port 0 for MIM TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                      PRV_TGF_TTI_KEY_MIM_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* disable port 0 for ETH TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                      PRV_TGF_TTI_KEY_ETH_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* invalidate TTI rules */
    rc = prvTgfTtiRuleValidStatusSet(PRV_TGF_TTI_INDEX_CNS, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleValidStatusSet: %d, %d", prvTgfDevNum, GT_TRUE);

    /* restore TPID table entry */
    rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                   PRV_TGF_TPID_INDEX_CNS,
                                   prvTgfDefTunnelStartEntryStc.etherType);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d", prvTgfDevNum);

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
}


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfTunnelStartUpMarkingModeConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - write QoS profile
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
GT_VOID prvTgfTunnelStartUpMarkingModeConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfTunnelStartTestInit(PRV_TGF_TTI_MAC_MODE_DA_E, CPSS_COS_PARAM_UP_E);
}

/*******************************************************************************
* prvTgfTunnelStartUpMarkingModeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 MIM tunneled packet:
*               macSa = 00:00:00:00:00:22,
*               macDa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800
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
GT_VOID prvTgfTunnelStartUpMarkingModeTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    GT_U32                          numTriggers = 0;
    TGF_VFD_INFO_STC                vfdArray[3];
    CPSS_INTERFACE_INFO_STC         portInterface;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;


    PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* configure portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    /* enable capture on nexthope port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_PCL_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* send packet to be matched */
    prvTgfTunnelStartTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* disable capture on nexthope port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_PCL_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

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
            expectedCntrs.goodOctetsSent.l[0] = PRV_TGF_PACKET_CRC_LEN_CNS * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
            expectedCntrs.goodOctetsRcv.l[0]  = PRV_TGF_PACKET_CRC_LEN_CNS * prvTgfBurstCount;
            expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
            expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
        }

        /* calculate expected counters */
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
              if(GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(0,
                                                          PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E))
              {
                    expectedCntrs.goodOctetsSent.l[0] = (PRV_TGF_PACKET_CRC_LEN_CNS +
                                                         TGF_CRC_LEN_CNS +
                                                         TGF_L2_HEADER_SIZE_CNS +
                                                         TGF_VLAN_TAG_SIZE_CNS +
                                                         TGF_ETHERTYPE_SIZE_CNS) * prvTgfBurstCount;
                    expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
                    expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
                    expectedCntrs.goodOctetsRcv.l[0]  = (PRV_TGF_PACKET_CRC_LEN_CNS +
                                                         TGF_CRC_LEN_CNS +
                                                         TGF_L2_HEADER_SIZE_CNS +
                                                         TGF_VLAN_TAG_SIZE_CNS +
                                                         TGF_ETHERTYPE_SIZE_CNS) * prvTgfBurstCount;
                    expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
                    expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
              }
              else
              {
                    expectedCntrs.goodOctetsSent.l[0] = (PRV_TGF_PACKET_CRC_LEN_CNS +
                                                         TGF_L2_HEADER_SIZE_CNS +
                                                         TGF_VLAN_TAG_SIZE_CNS +
                                                         TGF_ETHERTYPE_SIZE_CNS) * prvTgfBurstCount;
                    expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
                    expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
                    expectedCntrs.goodOctetsRcv.l[0]  =  (PRV_TGF_PACKET_CRC_LEN_CNS +
                                                         TGF_L2_HEADER_SIZE_CNS +
                                                         TGF_VLAN_TAG_SIZE_CNS +
                                                         TGF_ETHERTYPE_SIZE_CNS) * prvTgfBurstCount;
                    expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
                    expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
              }
        }

        /* check Rx\Tx counters */
        PRV_TGF_VERIFY_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(isEqualCntrs, GT_TRUE, "get another counters values.");

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
    }

    /* set interface info */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* clear triggers */
    cpssOsMemSet((GT_VOID*) vfdArray, 0, sizeof(vfdArray));

    /* calculate triggers */
    vfdArray[0].mode       = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfStartEntryMac, sizeof(TGF_MAC_ADDR));

    vfdArray[1].mode          = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset        = TGF_L2_HEADER_SIZE_CNS;
    vfdArray[1].cycleCount    = TGF_ETHERTYPE_SIZE_CNS;
    vfdArray[1].patternPtr[0] = (TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS >> 8) & 0xFF;
    vfdArray[1].patternPtr[1] = TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS & 0xFF;

    vfdArray[2].mode          = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].offset        = TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS;
    vfdArray[2].cycleCount    = TGF_ETHERTYPE_SIZE_CNS;
    vfdArray[2].patternPtr[0] = (TGF_ETHERTYPE_88E7_MIM_TAG_CNS >> 8) & 0xFF;
    vfdArray[2].patternPtr[1] = TGF_ETHERTYPE_88E7_MIM_TAG_CNS & 0xFF;

    /* get trigger counters */
    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 3, vfdArray, &numTriggers);
    PRV_UTF_LOG2_MAC("    numTriggers = %d, rc = 0x%02X\n\n", numTriggers, rc);

    /* check TriggerCounters */
    rc = (rc == GT_NO_MORE ? GT_OK : rc) || (!(numTriggers == 7));
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum, numTriggers);
}

/*******************************************************************************
* prvTgfTunnelStartUpMarkingModeConfigRestore
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
GT_VOID prvTgfTunnelStartUpMarkingModeConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelStartTestReset();
}

/*******************************************************************************
* prvTgfTunnelStartMimEtherTypeConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
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
GT_VOID prvTgfTunnelStartMimEtherTypeConfigSet
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfTunnelStartTestInit(PRV_TGF_TTI_MAC_MODE_DA_E, CPSS_COS_PARAM_DP_E + 1);

    /* set the MIM Ethernet type */
    rc = prvTgfTtiMimEthTypeSet(TGF_ETHERTYPE_88E7_MIM_TAG_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMimEthTypeSet: %d\n", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfTunnelStartMimEtherTypeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macSa = 00:00:00:00:00:22,
*               macDa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800
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
GT_VOID prvTgfTunnelStartMimEtherTypeTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS               rc = GT_OK;
    GT_U32                  portIter;
    GT_U32                  packetSize;
    GT_U32                  numTriggers = 0;
    TGF_VFD_INFO_STC        vfdArray[2];
    CPSS_INTERFACE_INFO_STC portInterface;
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
    prvTgfTunnelStartTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
             if(GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(0,
                                                         PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E))
             {
                /* packet after tunnel start in VLAN 6 */
                packetSize = prvTgfPacketInfo.totalLen + TGF_L2_HEADER_SIZE_CNS
                    + TGF_ETHERTYPE_SIZE_CNS + TGF_MIM_PAYLOAD_SIZE_CNS + TGF_CRC_LEN_CNS;
             }
             else
             {
                 /* packet after tunnel start in VLAN 6 */
                packetSize = prvTgfPacketInfo.totalLen + TGF_L2_HEADER_SIZE_CNS
                    + TGF_ETHERTYPE_SIZE_CNS + TGF_MIM_PAYLOAD_SIZE_CNS;
             }
        }
        else
        {
            /* original packet before tunneling in VLAN 5 */
            packetSize = prvTgfPacketInfo.totalLen;
        }

        rc = prvTgfEthCountersCheck(
                prvTgfDevNum,
                prvTgfPortsArray[portIter],
                expectedRxPackets[portIter],
                expectedTxPackets[portIter],
                packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
    }

    /* set packet's pattern */
    cpssOsMemSet((GT_VOID*) vfdArray, 0, sizeof(vfdArray));

    /* set ether type for 'Service provider vlan tag' */
    vfdArray[0].mode          = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].offset        = TGF_L2_HEADER_SIZE_CNS;
    vfdArray[0].cycleCount    = TGF_ETHERTYPE_SIZE_CNS;
    vfdArray[0].patternPtr[0] = (TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS >> 8) & 0xFF;
    vfdArray[0].patternPtr[1] = TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS & 0xFF;

    /* set ether type for MIM */
    vfdArray[1].mode          = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].offset        = TGF_L2_HEADER_SIZE_CNS + TGF_MPLS_HEADER_SIZE_CNS;
    vfdArray[1].cycleCount    = TGF_ETHERTYPE_SIZE_CNS;
    vfdArray[1].patternPtr[0] = (TGF_ETHERTYPE_88E7_MIM_TAG_CNS >> 8) & 0xFF;
    vfdArray[1].patternPtr[1] = TGF_ETHERTYPE_88E7_MIM_TAG_CNS & 0xFF;

    /* check trigger counters and print captured packets from receive port */
    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(
            &portInterface, 2, vfdArray, &numTriggers);
    UTF_VERIFY_EQUAL5_STRING_MAC(3, numTriggers, "tgfTrafficGenerator"
            "PortTxEthTriggerCountersGet: rc = %d\n"
            "captured packet has fields don't matched with expected fields:\n"
            "    offset_0x%02x: 0x%x\n"
            "    offset_0x%02x: 0x%x",
            rc, TGF_L2_HEADER_SIZE_CNS, TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS,
            TGF_L2_HEADER_SIZE_CNS + TGF_MPLS_HEADER_SIZE_CNS,
            TGF_ETHERTYPE_88E7_MIM_TAG_CNS);
}

/*******************************************************************************
* prvTgfTunnelStartMimEtherTypeConfigRestore
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
GT_VOID prvTgfTunnelStartMimEtherTypeConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelStartTestReset();
}

/*******************************************************************************
* prvTgfTunnelStartIdeiMarkingModeConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
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
GT_VOID prvTgfTunnelStartIdeiMarkingModeConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfTunnelStartTestInit(PRV_TGF_TTI_MAC_MODE_DA_E, CPSS_COS_PARAM_DP_E);
}

/*******************************************************************************
* prvTgfTunnelStartIdeiMarkingModeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macSa = 00:00:00:00:00:22,
*               macDa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800
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
GT_VOID prvTgfTunnelStartIdeiMarkingModeTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    CPSS_INTERFACE_INFO_STC portInterface;
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
    prvTgfTunnelStartTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
            if(GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(0,
                                                         PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E))
             {
                /* packet after tunnel start in VLAN 6 */
                packetSize = prvTgfPacketInfo.totalLen + TGF_L2_HEADER_SIZE_CNS
                    + TGF_ETHERTYPE_SIZE_CNS + TGF_MIM_PAYLOAD_SIZE_CNS + TGF_CRC_LEN_CNS;
            }
            else
            {
                /* packet after tunnel start in VLAN 6 */
                packetSize = prvTgfPacketInfo.totalLen + TGF_L2_HEADER_SIZE_CNS
                    + TGF_ETHERTYPE_SIZE_CNS + TGF_MIM_PAYLOAD_SIZE_CNS;
            }
        }
        else
        {
            /* original packet before tunneling in VLAN 5 */
            packetSize = prvTgfPacketInfo.totalLen;
        }

        rc = prvTgfEthCountersCheck(
                prvTgfDevNum,
                prvTgfPortsArray[portIter],
                expectedRxPackets[portIter],
                expectedTxPackets[portIter],
                packetSize, prvTgfBurstCount);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
    }

    /* print captured packets from receive port */
    rc = prvTgfPortCapturedPacketPrint(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortCapturedPacketPrint: %d",
            prvTgfDevNum, prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS]);
}

/*******************************************************************************
* prvTgfTunnelStartIdeiMarkingModeConfigRestore
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
GT_VOID prvTgfTunnelStartIdeiMarkingModeConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelStartTestReset();
}

/*******************************************************************************
* prvTgfTunnelStartMimMacSaAssignmentConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
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
GT_VOID prvTgfTunnelStartMimMacSaAssignmentConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfTunnelStartTestInit(PRV_TGF_TTI_MAC_MODE_DA_E, CPSS_COS_PARAM_DP_E);
}

