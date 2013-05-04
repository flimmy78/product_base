/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelMacInMacKey.c
*
* DESCRIPTION:
*       Verify the functionality of MIM TTI key MAC DA or MAC SA
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
#include <common/tgfTunnelGen.h>
#include <tunnel/prvTgfTunnelMacInMacKey.h>


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

/* default number of packets to send */
static GT_U32   prvTgfBurstCount = 1;

/* additional L2 part */
static TGF_PACKET_L2_STC prvTgfPacketAdditionalL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x22}                 /* saMac */
};

/*************************** Backbone packet's part ***************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketBackboneL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x05},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}                 /* saMac */
};

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketBackboneVlanTagPart =
{
    TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS,                 /* etherType */
    0, 0, PRV_TGF_VLANID_5_CNS                          /* pri, cfi, VlanId */
};

/********************* Service incapsulation part *****************************/

/* packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketServiceEtherTypePart = {TGF_ETHERTYPE_88E7_MIM_TAG_CNS};

/* Data of packet */
static GT_U8 prvTgfPacketServicePayloadDataArr[] =
{
    0xa0, 0x12, 0x34, 0x56
};

/* Service Payload part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketServicePayloadPart =
{
    sizeof(prvTgfPacketServicePayloadDataArr), /* dataLength */
    prvTgfPacketServicePayloadDataArr          /* dataPtr */
};

/***************************** Original part **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}                 /* saMac */
};

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_5_CNS                          /* pri, cfi, VlanId */
};

/* packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4Part =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x32,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    0x04,               /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    {10, 10, 10, 10},   /* srcAddr */
    { 4,  4,  4,  4}    /* dstAddr */
};

/* Data of packet */
static GT_U8 prvTgfPacketPayloadDataArr[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d
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
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketBackboneL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketBackboneVlanTagPart},

    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketServiceEtherTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketServicePayloadPart},

    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET_LEN_CNS                                                 \
    2 * TGF_L2_HEADER_SIZE_CNS + 2 * TGF_VLAN_TAG_SIZE_CNS +                   \
    2 * TGF_ETHERTYPE_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS +                    \
    sizeof(prvTgfPacketPayloadDataArr) + sizeof(prvTgfPacketServicePayloadDataArr)

/* Length of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* Packet to send */
static TGF_PACKET_STC prvTgfPacketInfo =
{
    PRV_TGF_PACKET_LEN_CNS,                                           /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(prvTgfPacketPartArray[0]), /* numOfParts */
    prvTgfPacketPartArray                                             /* partsArray */
};
/******************************************************************************/


/******************************************************************************\
 *                            Private variables                               *
\******************************************************************************/

/* stored default VID */
static GT_U16   prvTgfDefVlanId = 0;

/* stored default MIM B-EtherType in TPID table */
static GT_U16   prvTgfDefEtherType = 0;


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfTunnelTermTestInit
*
* DESCRIPTION:
*       Set initial test settings:
*           - Create VLAN 5 with ports 0, 8
*           - Create VLAN 6 with ports 18, 23
*           - Set VLAN 5 as port 0 default VLAN ID (PVID)
*           - Enable port 0 for MIM TTI lookup
*           - Set MIM TTI key lookup MAC mode to Mac DA
*           - Set TTI rule action
*           - Set TTI rule with un-masked parameters
*
* INPUTS:
*       ruleIndex   - index of the tunnel termination entry
*       macMode     - MAC mode to use
*       isMacMasked - enable\disable MAC mask
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
    IN PRV_TGF_TTI_MAC_MODE_ENT       macMode,
    IN GT_BOOL                        isMacMasked
)
{
    GT_STATUS                   rc       = GT_OK;
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

    /* get default port vlanId */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* set VLAN 5 as port 0 default VLAN ID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_VLANID_5_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", prvTgfDevNum);

    /* create a macEntry for tunneled terminated packet in VLAN 6 */
    rc = prvTgfBrgDefFdbMacEntryOnPortSet(
            prvTgfPacketL2Part.daMac, PRV_TGF_VLANID_6_CNS, prvTgfDevNum,
            prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS], GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);

    /* set MIM EtherType */
    rc = prvTgfTtiMimEthTypeSet(TGF_ETHERTYPE_88E7_MIM_TAG_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMimEthTypeSet: %x", TGF_ETHERTYPE_88E7_MIM_TAG_CNS);

    /* get MIM B-EtherType in TPID table */
    rc = prvTgfBrgVlanTpidEntryGet(CPSS_DIRECTION_INGRESS_E,1,&prvTgfDefEtherType);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntryGet: %d, %x",
                                 prvTgfDevNum,TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS);

    /* set MIM B-EtherType in TPID table */
    rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_INGRESS_E,1,TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %x",
                                 prvTgfDevNum,TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS);
    /* enable port 0 for MIM TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_TTI_KEY_MIM_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);

    /* set MIM TTI key lookup MAC mode */
    rc = prvTgfTtiMacModeSet(PRV_TGF_TTI_KEY_MIM_E, macMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMacModeSet: %d", prvTgfDevNum);

    /* clear entry */
    cpssOsMemSet((GT_VOID*) &ruleAction, 0, sizeof(ruleAction));

    /* configure TTI rule action */
    ruleAction.tunnelTerminate     = GT_TRUE;
    ruleAction.passengerPacketType = PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E;
    ruleAction.command             = CPSS_PACKET_CMD_FORWARD_E;
    ruleAction.redirectCommand     = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
    ruleAction.tag0VlanCmd         = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ruleAction.tag0VlanId          = PRV_TGF_VLANID_6_CNS;
    ruleAction.tag1VlanCmd         = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;
    ruleAction.tunnelStart         = GT_FALSE;

    ruleAction.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    ruleAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;
    ruleAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    /* clear entries */
    cpssOsMemSet((GT_VOID*) &pattern, 0, sizeof(pattern));
    cpssOsMemSet((GT_VOID*) &mask, 0, sizeof(mask));

    /* configure TTI rule */
    switch (macMode)
    {
        case PRV_TGF_TTI_MAC_MODE_DA_E:
            cpssOsMemCpy((GT_VOID*)pattern.mim.common.mac.arEther,
                         (GT_VOID*)prvTgfPacketBackboneL2Part.daMac,
                         sizeof(pattern.mim.common.mac.arEther));
            break;

        case PRV_TGF_TTI_MAC_MODE_SA_E:
            cpssOsMemCpy((GT_VOID*)pattern.mim.common.mac.arEther,
                         (GT_VOID*)prvTgfPacketBackboneL2Part.saMac,
                         sizeof(pattern.mim.common.mac.arEther));
            break;

        default:
            rc = GT_BAD_PARAM;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "Get invalid macMode: %d", macMode);
    }

    pattern.mim.common.vid   = PRV_TGF_VLANID_5_CNS;
    pattern.mim.common.pclId = 4;
    pattern.mim.iSid         = 0x123456;
    pattern.mim.iUp          = 0x5;
    pattern.mim.iDp          = 0;

    /* configure TTI rule mask */
    mask.mim.common.pclId = 0x3FF;
    mask.mim.common.vid = 0xFFF;
    cpssOsMemSet((GT_VOID*) &(mask.mim.iSid), 0xFF,
                 sizeof(mask.mim.iSid));
    cpssOsMemSet((GT_VOID*) &(mask.mim.iUp), 0xFF,
                 sizeof(mask.mim.iUp));
    cpssOsMemSet((GT_VOID*) &(mask.mim.iDp), 0xFF,
                 sizeof(mask.mim.iDp));

    if (isMacMasked)
    {
        cpssOsMemSet((GT_VOID*) &(mask.mim.common.mac), 0xFF, sizeof(mask.mim.common.mac));
    }

    /* set TTI rule */
    rc = prvTgfTtiRuleSet(ruleIndex, PRV_TGF_TTI_KEY_MIM_E, &pattern, &mask, &ruleAction);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleSet: %d", prvTgfDevNum);
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
    GT_STATUS               rc            = GT_OK;
    GT_U32                  portsCount    = prvTgfPortsNum;
    GT_U32                  portIter      = 0;
    CPSS_INTERFACE_INFO_STC portInterface = {0};


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

    /* configure portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    /* enable capture on nexthope port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

    /* send Packet from port 0 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, portNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfStartTransmitingEth: %d", prvTgfDevNum);

    /* disable capture on nexthope port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d", prvTgfDevNum);

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


    PRV_UTF_LOG0_MAC("======= Restore Configuration =======\n");

    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    if(GT_TRUE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E))
    {
        /* disable port 0 for MIM TTI lookup */
        rc = prvTgfTtiPortLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_TTI_KEY_MIM_E, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiPortLookupEnableSet: %d", prvTgfDevNum);
    }

    /* invalidate TTI rules */
    rc = prvTgfTtiRuleValidStatusSet(0, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleValidStatusSet: %d, %d", prvTgfDevNum, GT_FALSE);

    /* restore MIM B-EtherType in TPID table */
    rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_INGRESS_E,1,prvTgfDefEtherType);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %x",
                                 prvTgfDevNum,TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS);

    if(GT_TRUE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E))
    {
        /* restore default MIM EtherType */
        rc = prvTgfTtiMimEthTypeSet(0);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiMimEthTypeSet: %d", prvTgfDevNum);
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
}


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfTunnelTermUseMacSaInMimTtiLookupConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_SA_E
*         - set MAC mask as disable
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
GT_VOID prvTgfTunnelTermUseMacSaInMimTtiLookupConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfTunnelTermTestInit(0, PRV_TGF_TTI_MAC_MODE_SA_E, GT_FALSE);
}

/*******************************************************************************
* prvTgfTunnelTermUseMacSaInMimTtiLookupTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 MIM tunneled packet:
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
GT_VOID prvTgfTunnelTermUseMacSaInMimTtiLookupTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    GT_U32                          numTriggers = 0;
    TGF_VFD_INFO_STC                vfdArray[1];
    CPSS_INTERFACE_INFO_STC         portInterface;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;
    GT_U32                          packetSize;


    PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
            /* original packet before tunnel terminate in VLAN 5 */
            packetSize = PRV_TGF_PACKET_CRC_LEN_CNS;

            expectedCntrs.goodOctetsSent.l[0] = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
            expectedCntrs.goodOctetsRcv.l[0]  = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
            expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
        }

        /* calculate expected counters */
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
            /* packet after tunnel terminate in VLAN 6 */
            packetSize = PRV_TGF_PACKET_CRC_LEN_CNS
                - TGF_L2_HEADER_SIZE_CNS
                - TGF_VLAN_TAG_SIZE_CNS
                - TGF_ETHERTYPE_SIZE_CNS - sizeof(prvTgfPacketServicePayloadDataArr)
                - TGF_VLAN_TAG_SIZE_CNS;

            expectedCntrs.goodOctetsSent.l[0] = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
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

    /* set interface info */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* clear triggers */
    cpssOsMemSet((GT_VOID*) vfdArray, 0, sizeof(vfdArray));

    /* calculate triggers */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = sizeof(TGF_MAC_ADDR);
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfPacketL2Part.saMac, sizeof(TGF_MAC_ADDR));

    /* get trigger counters */
    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 1, vfdArray, &numTriggers);
    PRV_UTF_LOG2_MAC("    numTriggers = %d, rc = 0x%02X\n\n", numTriggers, rc);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);
}

/*******************************************************************************
* prvTgfUseTunnelTermMacSaInMimTtiLookupConfigRestore
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
GT_VOID prvTgfTunnelTermUseMacSaInMimTtiLookupConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

/*******************************************************************************
* prvTgfTunnelTermMaskMacInMimTtiLookupConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - set MAC mask as enable
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
GT_VOID prvTgfTunnelTermMaskMacInMimTtiLookupConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfTunnelTermTestInit(0, PRV_TGF_TTI_MAC_MODE_DA_E, GT_TRUE);
}

/*******************************************************************************
* prvTgfTunnelTermMaskMacInMimTtiLookupTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 MIM tunneled packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:05,
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
GT_VOID prvTgfTunnelTermMaskMacInMimTtiLookupTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    GT_U32                          numTriggers = 0;
    TGF_VFD_INFO_STC                vfdArray[1];
    CPSS_INTERFACE_INFO_STC         portInterface;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;
    GT_U32                          packetSize;


    PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* override default DA */
    prvTgfPacketPartArray[4].partPtr = (GT_VOID*) &prvTgfPacketAdditionalL2Part;

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
            /* original packet before tunnel terminate in VLAN 5 */
            packetSize = PRV_TGF_PACKET_CRC_LEN_CNS;

            expectedCntrs.goodOctetsSent.l[0] = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
            expectedCntrs.goodOctetsRcv.l[0]  = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
            expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
        }

        /* calculate expected counters */
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
            /* packet after tunnel terminate in VLAN 6 */
            packetSize = PRV_TGF_PACKET_CRC_LEN_CNS
                - TGF_L2_HEADER_SIZE_CNS
                - TGF_VLAN_TAG_SIZE_CNS
                - TGF_ETHERTYPE_SIZE_CNS - sizeof(prvTgfPacketServicePayloadDataArr)
                - TGF_VLAN_TAG_SIZE_CNS;

            expectedCntrs.goodOctetsSent.l[0] = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
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

    /* set interface info */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* clear triggers */
    cpssOsMemSet((GT_VOID*) vfdArray, 0, sizeof(vfdArray));

    /* calculate triggers */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = sizeof(TGF_MAC_ADDR);
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfPacketAdditionalL2Part.saMac, sizeof(TGF_MAC_ADDR));

    /* get trigger counters */
    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 1, vfdArray, &numTriggers);
    PRV_UTF_LOG2_MAC("    numTriggers = %d, rc = 0x%02X\n\n", numTriggers, rc);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);
}

/*******************************************************************************
* prvTgfTunnelTermMaskMacInMimTtiLookupConfigRestore
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
GT_VOID prvTgfTunnelTermMaskMacInMimTtiLookupConfigRestore
(
    GT_VOID
)
{
    /* restore default DA */
    prvTgfPacketPartArray[4].partPtr = (GT_VOID*) &prvTgfPacketL2Part;

    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

/*******************************************************************************
* prvTgfTunnelTermMimBasicConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - set MAC mask as disable
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
GT_VOID prvTgfTunnelTermMimBasicConfigSet
(
    GT_VOID
)
{
    PRV_UTF_LOG0_MAC("======= Setting Configuration =======\n");

    /* init configuration */
    prvTgfTunnelTermTestInit(0, PRV_TGF_TTI_MAC_MODE_DA_E, GT_TRUE);
}

/*******************************************************************************
* prvTgfTunnelTermMimBasicTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 MIM tunneled packet:
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
GT_VOID prvTgfTunnelTermMimBasicTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    GT_U32                          numTriggers = 0;
    TGF_VFD_INFO_STC                vfdArray[1];
    CPSS_INTERFACE_INFO_STC         portInterface;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;
    GT_U32                          packetSize;


    PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* send packet to be matched */
    prvTgfTunnelTermTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
            /* original packet before tunnel terminate in VLAN 5 */
            packetSize = PRV_TGF_PACKET_CRC_LEN_CNS;

            expectedCntrs.goodOctetsSent.l[0] = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
            expectedCntrs.goodOctetsRcv.l[0]  = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsRcv.l[0]    = prvTgfBurstCount;
            expectedCntrs.ucPktsRcv.l[0]      = prvTgfBurstCount;
        }

        /* calculate expected counters */
        if (PRV_TGF_EGR_PORT_IDX_CNS == portIter)
        {
            /* packet after tunnel terminate in VLAN 6 */
            packetSize = PRV_TGF_PACKET_CRC_LEN_CNS
                - TGF_L2_HEADER_SIZE_CNS
                - TGF_VLAN_TAG_SIZE_CNS
                - TGF_ETHERTYPE_SIZE_CNS - sizeof(prvTgfPacketServicePayloadDataArr)
                - TGF_VLAN_TAG_SIZE_CNS;

            expectedCntrs.goodOctetsSent.l[0] = packetSize * prvTgfBurstCount;
            expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
            expectedCntrs.ucPktsSent.l[0]     = prvTgfBurstCount;
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

    /* set interface info */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_CNS];

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterface.devPort.portNum);

    /* clear triggers */
    cpssOsMemSet((GT_VOID*) vfdArray, 0, sizeof(vfdArray));

    /* calculate triggers */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = sizeof(TGF_MAC_ADDR);
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfPacketL2Part.saMac, sizeof(TGF_MAC_ADDR));

    /* get trigger counters */
    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 1, vfdArray, &numTriggers);
    PRV_UTF_LOG2_MAC("    numTriggers = %d, rc = 0x%02X\n\n", numTriggers, rc);

    /* check TriggerCounters */
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum);
}

/*******************************************************************************
* prvTgfTunnelTermBasicMimConfigRestore
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
GT_VOID prvTgfTunnelTermBasicMimConfigRestore
(
    GT_VOID
)
{
    /* restore configuration */
    prvTgfTunnelTermTestReset();
}

