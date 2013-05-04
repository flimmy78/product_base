/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPclEgrOrgVid.c
*
* DESCRIPTION:
*       Ingress PCL Double tagged packet Egress Pcl Original VID field
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/pcl/cpssPcl.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPclGen.h>
#include <common/tgfBridgeGen.h>
#include <pcl/prvTgfPclTrunkHashVal.h>

/* VID for internal tag */
#define PRV_TGF_PCL_TEST_VID1 20


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* Data of packet (long version) */
static GT_U8 prvTgfPacketPayloadLongDataArr[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
};

/* Payload part (long version) */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketLongPayloadPart =
{
    sizeof(prvTgfPacketPayloadLongDataArr), /* dataLength */
    prvTgfPacketPayloadLongDataArr          /* dataPtr */
};

static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x56},                /* daMac */
    {0x00, 0x66, 0x77, 0x88, 0x99, 0x00}                 /* saMac */
};

/* ethertype part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEthOthEtherTypePart =
{0x3456};

/* VLAN tag0 part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
    PRV_TGF_PCL_TEST_UP0  /*pri*/,
    PRV_TGF_PCL_TEST_CFI0 /*cfi*/,
    PRV_TGF_PCL_TEST_VID0 /*vid*/
};

/* VLAN tag0 part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag1Part =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
    PRV_TGF_PCL_TEST_UP0  /*pri*/,
    PRV_TGF_PCL_TEST_CFI0 /*cfi*/,
    PRV_TGF_PCL_TEST_VID1 /*vid*/
};

/* PARTS of packet Ethernet Other */
static TGF_PACKET_PART_STC prvTgfEthOthPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTag1Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEthOthEtherTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of ETH_OTHER packet */
#define PRV_TGF_ETH_OTHER_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + (TGF_VLAN_TAG_SIZE_CNS * 2) \
    + TGF_ETHERTYPE_SIZE_CNS \
    + sizeof(prvTgfPacketPayloadLongDataArr)

/* Eth Other packet to send */
static TGF_PACKET_STC prvTgfEthOtherPacketInfo =
{
    PRV_TGF_ETH_OTHER_PACKET_LEN_CNS,              /* totalLen */
    (sizeof(prvTgfEthOthPacketPartArray)
        / sizeof(prvTgfEthOthPacketPartArray[0])), /* numOfParts */
    prvTgfEthOthPacketPartArray                    /* partsArray */
};

/******************************************************************************\
 *                            Private variables                               *
\******************************************************************************/

/* stored default Vlan ID */
static GT_U16   prvTgfDefVlanId = 0;

/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPclEgrOrgVidVlanConfigurationSet
*
* DESCRIPTION:
*       Set test configuration
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
static GT_VOID prvTgfPclEgrOrgVidVlanConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U32                      i;

    PRV_UTF_LOG1_MAC(
        "\nVLAN %d CONFIGURATION:\n",
        PRV_TGF_PCL_TEST_VID0);
    PRV_UTF_LOG4_MAC("  Port members: [%d], [%d], [%d], [%d]\n",
            prvTgfPortsArray[0], prvTgfPortsArray[1],
            prvTgfPortsArray[2], prvTgfPortsArray[3]);

    /* create a vlan on ports (0,0), (0,8), (0,18), (0,23) */
    rc = prvTgfBrgDefVlanEntryWriteWithTaggingCmd(
        PRV_TGF_PCL_TEST_VID0,
        PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d", prvTgfDevNum);

    /* create a vlan on ports (0,0), (0,8), (0,18), (0,23) */
    rc = prvTgfBrgDefVlanEntryWriteWithTaggingCmd(
        PRV_TGF_PCL_TEST_VID1,
        PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d", prvTgfDevNum);

    for (i = 0; (i < 2); i++)
    {
        /* create a vlan on ports (0,0), (0,8), (0,18), (0,23) */
        rc = prvTgfBrgDefVlanEntryWriteWithTaggingCmd(
            (GT_U16)(PRV_TGF_PCL_MODIFIED_VLANID_0_CNS + i),
            PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d", prvTgfDevNum);
    }

    /* get default vlanId */
    rc = prvTgfBrgVlanPortVidGet(
        prvTgfDevsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS],
        &prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d, %d",
        prvTgfDevNum, prvTgfPortsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS]);

    /* set default vlanId */
    rc = prvTgfBrgVlanPortVidSet(
        prvTgfDevsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS],
        PRV_TGF_PCL_TEST_VID0);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
        prvTgfDevNum, prvTgfPortsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS]);
}

/*******************************************************************************
* prvTgfPclEgrOrgVidConfigurationRestore
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
static GT_VOID prvTgfPclEgrOrgVidConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U32                      i;
    CPSS_INTERFACE_INFO_STC     interfaceInfo;
    PRV_TGF_PCL_LOOKUP_CFG_STC  lookupCfg;

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* clear PCL configuration table */
    cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
    cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

    interfaceInfo.type               = CPSS_INTERFACE_PORT_E;
    interfaceInfo.devPort.devNum     =
        prvTgfDevsArray[PRV_TGF_PCL_RECEIVE_PORT_INDEX_CNS];
    interfaceInfo.devPort.portNum    =
        prvTgfPortsArray[PRV_TGF_PCL_RECEIVE_PORT_INDEX_CNS];

    lookupCfg.enableLookup           = GT_FALSE;
    lookupCfg.lookupType             = PRV_TGF_PCL_LOOKUP_TYPE_SINGLE_E;
    lookupCfg.dualLookup             = GT_FALSE;
    lookupCfg.externalLookup         = GT_FALSE;
    lookupCfg.groupKeyTypes.nonIpKey =
        PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
    lookupCfg.groupKeyTypes.ipv4Key =
        PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;
    lookupCfg.groupKeyTypes.ipv6Key =
        PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;

    rc = prvTgfPclCfgTblSet(
        &interfaceInfo, CPSS_PCL_DIRECTION_EGRESS_E,
        CPSS_PCL_LOOKUP_0_E, &lookupCfg);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPclCfgTblSet");

    for (i = 0; (i < 2); i++)
    {
        /* invalidate PCL rules */
        rc = prvTgfPclRuleValidStatusSet(
            CPSS_PCL_RULE_SIZE_STD_E, i, GT_FALSE);
        UTF_VERIFY_EQUAL2_STRING_MAC(
            GT_OK, rc, "prvTgfPclRuleValidStatusSet: %d, %d",
            prvTgfDevNum, GT_TRUE);
    }

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* restore default vlanId to all ports */
    rc = prvTgfBrgVlanPvidSet(prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfBrgVlanPvidSet: %d, %d",
        prvTgfDevNum, prvTgfDefVlanId);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(
        PRV_TGF_PCL_TEST_VID0);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
        prvTgfDevNum, PRV_TGF_PCL_TEST_VID0);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(
        PRV_TGF_PCL_TEST_VID1);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
        prvTgfDevNum, PRV_TGF_PCL_TEST_VID1);

    for (i = 0; (i < 2); i++)
    {
        /* invalidate vlan entry (and reset vlan entry) */
        rc = prvTgfBrgDefVlanEntryInvalidate(
            (GT_U16)(PRV_TGF_PCL_MODIFIED_VLANID_0_CNS + i));
        UTF_VERIFY_EQUAL2_STRING_MAC(
            GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
            prvTgfDevNum, (PRV_TGF_PCL_MODIFIED_VLANID_0_CNS + i));
    }
}

/*******************************************************************************
* prvTgfPclEgrOrgVidTrafficGenerate
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
static GT_VOID prvTgfPclEgrOrgVidTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS            rc         = GT_OK;

    rc = prvTgfTransmitPacketsWithCapture(
        prvTgfDevsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS],
        &prvTgfEthOtherPacketInfo,
        1      /*burstCount*/,
        0      /*numVfd*/,
        NULL   /*vfdArray*/,
        prvTgfDevsArray[PRV_TGF_PCL_RECEIVE_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_PCL_RECEIVE_PORT_INDEX_CNS],
        TGF_CAPTURE_MODE_MIRRORING_E,
        200                                /*captureOnTime*/);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfTransmitPacketsWithCapture: %d",
        prvTgfDevNum);
}

/*******************************************************************************
* prvTgfPclEgrOrgVidTrafficEgressVidCheck
*
* DESCRIPTION:
*       Checks traffic egress VID in the Tag
*
* INPUTS:
*       portIndex - port index
*       packetIndex - the index of the sent packet
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
static GT_VOID prvTgfPclEgrOrgVidTrafficEgressVidCheck
(
    IN GT_U32  portIndex,
    IN GT_U32  packetIndex
)
{
    GT_U16 egressVid;

    egressVid = (GT_U16)(PRV_TGF_PCL_MODIFIED_VLANID_0_CNS + packetIndex);

    prvTgfPclTrunkHashTrafficEgressVidCheck(
        portIndex, egressVid);
}

/*******************************************************************************
* prvTgfPclEgrOrgVidPclConfigurationSet
*
* DESCRIPTION:
*       Set test PCL configuration
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
static GT_VOID prvTgfPclEgrOrgVidPclConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                        rc = GT_OK;
    PRV_TGF_PCL_RULE_FORMAT_UNT      mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT      pattern;
    PRV_TGF_PCL_ACTION_STC           action;
    GT_U32                           ruleIndex;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    GT_U32                           i;

    ruleIndex                   = 0;
    ruleFormat                  = PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;


    /* init PCL Engine for sending to port 1 */
    rc = prvTgfPclDefPortInit(
        prvTgfPortsArray[PRV_TGF_PCL_RECEIVE_PORT_INDEX_CNS],
        CPSS_PCL_DIRECTION_EGRESS_E,
        CPSS_PCL_LOOKUP_0_E,
        PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,
        PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
        PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfPclDefPortInit: %d, %d", prvTgfDevNum,
        prvTgfPortsArray[PRV_TGF_PCL_SEND_PORT_INDEX_CNS]);

    /* set PCL rule 0 - CMD_DROP_HARD packet 1 - any Not Ipv4 with MAC DA = ... 34 02 */
    cpssOsMemSet(&mask, 0, sizeof(mask));
    cpssOsMemSet(&pattern, 0, sizeof(pattern));
    cpssOsMemSet(&action, 0, sizeof(action));

    /* original VID */
    mask.ruleEgrStdNotIp.common.vid    = 0xFFFF;

    action.egressPolicy = GT_TRUE;
    action.pktCmd       = CPSS_PACKET_CMD_FORWARD_E;
    /* vlanCmd - for ExMxPm, modifyVlan for DxCh */
    action.vlan.vlanCmd         = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
    action.vlan.modifyVlan      = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
    action.vlan.precedence      =
        CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

    for (i = 0; (i < 2); i++)
    {
        action.vlan.vlanId =
         (GT_U16)(PRV_TGF_PCL_MODIFIED_VLANID_0_CNS + i);

        pattern.ruleEgrStdNotIp.common.vid = (GT_U16)
            ((i == 0)
                ? PRV_TGF_PCL_TEST_VID0
                : PRV_TGF_PCL_TEST_VID1);

        rc = prvTgfPclRuleSet(
            ruleFormat, (ruleIndex + i), &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(
            GT_OK, rc, "prvTgfPclRuleSet: %d, %d, %d",
            prvTgfDevNum, ruleFormat, ruleIndex);
    }
}


/*******************************************************************************
* prvTgfPclEgrOrgVidTrafficGenerateAndCheck
*
* DESCRIPTION:
*       Generate traffic and check results
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
static GT_VOID prvTgfPclEgrOrgVidTrafficGenerateAndCheck
(
    GT_VOID
)
{
    GT_STATUS          rc         = GT_OK;

    rc = prvTgfPclEgressKeyFieldsVidUpModeSet(
        PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfPclEgressKeyFieldsVidUpModeSet: %d, %d",
        prvTgfDevNum, PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E);

    prvTgfPclEgrOrgVidTrafficGenerate();
    prvTgfPclEgrOrgVidTrafficEgressVidCheck(
        PRV_TGF_PCL_RECEIVE_PORT_INDEX_CNS, 0);

    rc = prvTgfPclEgressKeyFieldsVidUpModeSet(
        PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfPclEgressKeyFieldsVidUpModeSet: %d, %d",
        prvTgfDevNum, PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E);

    prvTgfPclEgrOrgVidTrafficGenerate();
    prvTgfPclEgrOrgVidTrafficEgressVidCheck(
        PRV_TGF_PCL_RECEIVE_PORT_INDEX_CNS, 1);

    rc = prvTgfPclEgressKeyFieldsVidUpModeSet(
        PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfPclEgressKeyFieldsVidUpModeSet: %d, %d",
        prvTgfDevNum, PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E);
}

/*******************************************************************************
* prvTgfPclEgrOrgVidTrafficTest
*
* DESCRIPTION:
*       Full Egress Original VID field test
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
GT_VOID prvTgfPclEgrOrgVidTrafficTest
(
    GT_VOID
)
{
    prvTgfPclEgrOrgVidVlanConfigurationSet();

    prvTgfPclEgrOrgVidPclConfigurationSet();

    prvTgfPclEgrOrgVidTrafficGenerateAndCheck();

    prvTgfPclEgrOrgVidConfigurationRestore();
}
