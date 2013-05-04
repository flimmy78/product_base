/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfCncEgrQue.c
*
* DESCRIPTION:
*     CPSS DXCH Centralized counters (CNC) Technology facility implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssMath.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPortGen.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfCncGen.h>
#include <common/tgfCosGen.h>
#include <cnc/prvTgfCncVlanL2L3.h>
#include <cnc/prvTgfCncEgrQue.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/******************************* Test packet **********************************/

/******************************* common payload *******************************/

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
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x00},                /* daMac */
    {0x00, 0x66, 0x77, 0x88, 0x99, 0x00}                 /* saMac */
};

/* ethertype part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEthOthEtherTypePart =
{0x3456};

#if PRV_TGF_CNC_TEST_TAGGED_PACKET_CNS
/* VLAN tag1 part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
    PRV_TGF_CNC_TEST_UP0  /*pri*/,
    PRV_TGF_CNC_TEST_CFI0 /*cfi*/,
    PRV_TGF_CNC_TEST_VID0 /*vid*/
};
#endif /*PRV_TGF_CNC_TEST_TAGGED_PACKET_CNS*/

/* PARTS of packet Ethernet Other */
static TGF_PACKET_PART_STC prvTgfEthOthPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
#if PRV_TGF_CNC_TEST_TAGGED_PACKET_CNS
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTag0Part},
#endif /*PRV_TGF_CNC_TEST_TAGGED_PACKET_CNS*/
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEthOthEtherTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of ETH_OTHER packet */
#define PRV_TGF_ETH_OTHER_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + PRV_TGF_CNC_TEST_VLAN_TAG_SIZE_CNS \
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
 *                           Private declarations                             *
\******************************************************************************/

/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfCncTestEgrQueConfigure
*
* DESCRIPTION:
*       This function configures Cnc EgrQue tests
*
* INPUTS:
*       None
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
GT_STATUS prvTgfCncTestEgrQueConfigure
(
    GT_VOID
)
{
    GT_STATUS                      rc;
    GT_U64                         indexRangesBmp;
    PRV_TGF_CNC_COUNTER_STC        counter;

    prvTgfCncTestVidDefConfigurationSet();

    /* all ranges relevant to VID map into the block    */
    /* correct both for 512 and for 2048 counter blocks */
    indexRangesBmp.l[0] = 0xFF;
    indexRangesBmp.l[1] = 0;
    counter.byteCount.l[0] = 0;
    counter.byteCount.l[1] = 0;
    counter.packetCount.l[0] = 0;
    counter.packetCount.l[1] = 0;

    rc = prvTgfCncTestCncBlockConfigure(
        PRV_TGF_CNC_TESTED_BLOCK_NUM_MAC(),
        PRV_TGF_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E,
        GT_TRUE /*enable*/,
        indexRangesBmp,
        PRV_TGF_CNC_COUNTER_FORMAT_MODE_0_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvTgfCncCounterClearByReadEnableSet(
        GT_TRUE /*enable*/);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfCncCounterClearByReadEnableSet");

    rc = prvTgfCncCounterClearByReadValueSet(
        PRV_TGF_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfCncCounterClearByReadValueSet");

    rc = prvTgfCncClientByteCountModeSet(
        PRV_TGF_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E,
        PRV_TGF_CNC_BYTE_COUNT_MODE_L2_E);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfCncClientByteCountModeSet");

    return GT_OK;
}

/*******************************************************************************
* prvTgfCncTestEgrQueTrafficGenerateAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check counters.
*
* INPUTS:
*       None
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCncTestEgrQueTrafficGenerateAndCheck
(
    GT_VOID
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          burstCount;
    GT_U32                          localEgrPort;
    GT_U32                          tailDropModeIdx;
    GT_U32                          cnModeIdx;
    CPSS_QOS_ENTRY_STC              qosEntry;
    PRV_TGF_COS_PROFILE_STC         qosProfile;
    GT_U32                          portOffset = 5; /* bit offset in the CNC index 
                                                  calculation for port */

    /* calculate indexes of counters for DP == 1 */
    /* text from xCat functional spec BTS #322.
      This is true for CH3 and Lion A0 too.
------------------------------------------------------------------
In (section 21.5.5.2), figure 88, Egress counters index format dp occupies 1 bit.
The format should be as below (DP is extended to 2 bits)
bits 1:0 = dp
bits 4:2 = tc
bits 9:5 = port
bit 10 = drop/pass
bits 15:11 = “0” 
*/

    localEgrPort = prvTgfPortsArray[PRV_TGF_CNC_RECEIVE_PORT_INDEX_CNS];

    if (GT_TRUE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E) &&
        0 == PRV_CPSS_PP_MAC(prvTgfDevNum)->revision)
    {
        /* conversion for Lion A devices           */
        /* each port group contains 16 ports       */
        localEgrPort &= 0x0F;
    }

    if (GT_TRUE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E) &&
        0 != PRV_CPSS_PP_MAC(prvTgfDevNum)->revision)
    {
        /* Lion B devices           */
        /* port bits are 4:9       */
        portOffset = 4;
    }


    tailDropModeIdx = ((localEgrPort << portOffset) | 1);

    cnModeIdx = (localEgrPort << portOffset);

    /* Set packets DP to 1, TC to 0 */

    qosProfile.dropPrecedence = 1;
    qosProfile.trafficClass   = 0;
    qosProfile.dscp           = 0;
    qosProfile.exp            = 0;
    qosProfile.userPriority   = 0;

    rc = prvTgfCosProfileEntrySet(
        PRV_TGF_CNC_TEST_QOS_PROFILE0, &qosProfile);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet");

    qosEntry.assignPrecedence =
        CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    qosEntry.enableModifyDscp =
        CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    qosEntry.enableModifyUp =
        CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    qosEntry.qosProfileId = PRV_TGF_CNC_TEST_QOS_PROFILE0;

    rc = prvTgfCosPortQosConfigSet(
        prvTgfPortsArray[PRV_TGF_CNC_SEND_PORT_INDEX_CNS], &qosEntry);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosConfigSet");

    rc = prvTgfCosPortQosTrustModeSet(
        prvTgfPortsArray[PRV_TGF_CNC_SEND_PORT_INDEX_CNS],
        CPSS_QOS_PORT_NO_TRUST_E);
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfCosPortQosTrustModeSet");

    /* PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E */

    burstCount = 3;

    if(GT_FALSE != prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E))
    {
        rc = prvTgfCncEgressQueueClientModeSet(
            PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E);
        UTF_VERIFY_EQUAL0_STRING_MAC(
            GT_OK, rc, "prvTgfCncEgressQueueClientModeSet");
    }

    prvTgfCncTestSendPacketAndCheck(
        &prvTgfEthOtherPacketInfo,
        burstCount,
        PRV_TGF_CNC_TESTED_BLOCK_NUM_MAC(),
        tailDropModeIdx,
        /* counting all packet bytes with CRC */
        ((prvTgfEthOtherPacketInfo.totalLen + 4) * burstCount));

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);
    
    /* PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E */

    burstCount = 2;

    rc = prvTgfCncEgressQueueClientModeSet(
        PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E);
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfCncEgressQueueClientModeSet");

    prvTgfCncTestSendPacketAndCheck(
        &prvTgfEthOtherPacketInfo,
        burstCount,
        PRV_TGF_CNC_TESTED_BLOCK_NUM_MAC(),
        cnModeIdx,
        /* byte count is equal to burst count in this mode */
        burstCount);
}



