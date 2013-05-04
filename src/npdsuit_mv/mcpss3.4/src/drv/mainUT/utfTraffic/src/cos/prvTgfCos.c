/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVnt.c
*
* DESCRIPTION:
*     CPSS DXCH Virtual Network Tester (VNT) Technology facility implementation.
*     VNT features:
*     - Operation, Administration, and Maintenance (OAM).
*     - Connectivity Fault Management (CFM).
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

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPortGen.h>
#include <common/tgfCosGen.h>
#include <cos/prvTgfCos.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* Qos Profiles used in test */
#define PRV_TGF_COS_TEST_PROFILE0 6
#define PRV_TGF_COS_TEST_PROFILE1 7

/* Traffic Classes used in test */
#define PRV_TGF_COS_TEST_TC0 6
#define PRV_TGF_COS_TEST_TC1 7

/* index of port in port-array for send port */
#define PRV_TGF_COS_SEND_PORT_INDEX_CNS        1

/* index of port in port-array for send port */
#define PRV_TGF_COS_RECEIVE_PORT_INDEX_CNS     2

/* Exp value used in test */
#define PRV_TGF_COS_EXP0_CNS        1

/* User Priorities used in test */
#define PRV_TGF_COS_TEST_UP0 6
#define PRV_TGF_COS_TEST_UP1 7

/* CFI used in test */
#define PRV_TGF_COS_TEST_CFI0 0
#define PRV_TGF_COS_TEST_CFI1 0

/* User Priorities profile indexes used in test */
#define PRV_TGF_COS_TEST_UP_PROFILE_INDEX_0         0
#define PRV_TGF_COS_TEST_UP_PROFILE_INDEX_1         1

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
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
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

/******************************* MPLS packet **********************************/

/* ethertype part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketMplsEtherTypePart =
{TGF_ETHERTYPE_8847_MPLS_TAG_CNS};

/* packet's MPLS */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsPart =
{
    1,                   /* label */
    PRV_TGF_COS_EXP0_CNS,/* experimental use */
    0,                   /* stack */
    0x40                 /* timeToLive */
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

/* MPLS packet to send */
static TGF_PACKET_STC prvTgfMplsPacketInfo =
{
    PRV_TGF_MPLS_PACKET_LEN_CNS,                 /* totalLen */
    (sizeof(prvTgfMplsPacketPartArray)
        / sizeof(prvTgfMplsPacketPartArray[0])), /* numOfParts */
    prvTgfMplsPacketPartArray                    /* partsArray */
};

/*************** Double tagged ETH_OTHER packet *******************************/

/* ethertype part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEthOthEtherTypePart =
{0x3456};

/* VLAN tag0 part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
    PRV_TGF_COS_TEST_UP0  /*pri*/,
    PRV_TGF_COS_TEST_CFI0 /*cfi*/,
    0 /*vid - UP tagged packet */
};

/* VLAN tag1 part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag1Part =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
    PRV_TGF_COS_TEST_UP1  /*pri*/,
    PRV_TGF_COS_TEST_CFI1 /*cfi*/,
    0 /*vid - UP tagged packet */
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
    TGF_L2_HEADER_SIZE_CNS + (2 * TGF_VLAN_TAG_SIZE_CNS) \
    + TGF_ETHERTYPE_SIZE_CNS \
    + sizeof(prvTgfPacketPayloadLongDataArr)

/* MPLS packet to send */
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
* prvTgfCosTestCommonConfigure
*
* DESCRIPTION:
*       This function configures Cos all tests
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
GT_STATUS prvTgfCosTestCommonConfigure
(
    GT_VOID
)
{
    GT_STATUS                      rc;
    PRV_TGF_COS_PROFILE_STC        profileEntry = {0,0,0,0,0};
    CPSS_QOS_ENTRY_STC             cosEntry;

    prvTgfEthCountersReset(prvTgfDevsArray[0]);/* force link up on all needed ports */
    /* Confgure Qos Profile0 */
    profileEntry.trafficClass = PRV_TGF_COS_TEST_TC0;
    rc = prvTgfCosProfileEntrySet(
        PRV_TGF_COS_TEST_PROFILE0, &profileEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosProfileEntrySet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* Confgure Qos Profile1 */
    profileEntry.trafficClass = PRV_TGF_COS_TEST_TC1;
    rc = prvTgfCosProfileEntrySet(
        PRV_TGF_COS_TEST_PROFILE1, &profileEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosProfileEntrySet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* Map Exp0 to profile1 */
    rc = prvTgfCosExpToProfileMapSet(
        PRV_TGF_COS_EXP0_CNS, PRV_TGF_COS_TEST_PROFILE1);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosExpToProfileMapSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* Map Up0 Cfi0 to profile0 */
    rc = prvTgfCosUpCfiDeiToProfileMapSet(PRV_TGF_COS_TEST_UP_PROFILE_INDEX_0,
        PRV_TGF_COS_TEST_UP0, PRV_TGF_COS_TEST_CFI0,
        PRV_TGF_COS_TEST_PROFILE0);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosUpCfiDeiToProfileMapSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* Map Up1 Cfi1 to profile1 */
    rc = prvTgfCosUpCfiDeiToProfileMapSet(PRV_TGF_COS_TEST_UP_PROFILE_INDEX_1,
        PRV_TGF_COS_TEST_UP1, PRV_TGF_COS_TEST_CFI1,
        PRV_TGF_COS_TEST_PROFILE1);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosUpCfiDeiToProfileMapSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* map send port to Qos Profile0 */
    cosEntry.qosProfileId     = PRV_TGF_COS_TEST_PROFILE0;
    cosEntry.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    cosEntry.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    cosEntry.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    rc = prvTgfCosPortQosConfigSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS], &cosEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosConfigSet FAILED, rc = [%d]", rc);
        return rc;
    }


    /*  set the per port per UP the upProfile index */
    rc = prvTgfCosPortUpProfileIndexSet(
        prvTgfDevsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        PRV_TGF_COS_TEST_UP0,
        PRV_TGF_COS_TEST_UP_PROFILE_INDEX_0);
    if(rc == GT_NOT_APPLICABLE_DEVICE)
    {
        /* this device not support multiple tables */
    }
    else if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosConfigSet FAILED, rc = [%d]", rc);
        return rc;
    }

    rc = prvTgfCosPortUpProfileIndexSet(
        prvTgfDevsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        PRV_TGF_COS_TEST_UP1,
        PRV_TGF_COS_TEST_UP_PROFILE_INDEX_1);
    if(rc == GT_NOT_APPLICABLE_DEVICE)
    {
        /* this device not support multiple tables */
    }
    else if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosConfigSet FAILED, rc = [%d]", rc);
        return rc;
    }


    return GT_OK;
}

/*******************************************************************************
* prvTgfCosTestCommonReset
*
* DESCRIPTION:
*       This function resets configuration of Cos
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
GT_STATUS prvTgfCosTestCommonReset
(
    GT_VOID
)
{
    GT_STATUS   rc;
    CPSS_QOS_ENTRY_STC             cosEntry;

    /* map send port to Qos Profile0 */
    cosEntry.qosProfileId     = 0;
    cosEntry.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    cosEntry.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    cosEntry.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    rc = prvTgfCosPortQosConfigSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS], &cosEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosConfigSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* reset Up Trust mode */
    rc = prvTgfCosPortQosTrustModeSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS], CPSS_QOS_PORT_NO_TRUST_E);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosTrustModeSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* reset Exp Trust mode */
    rc = prvTgfCosTrustExpModeSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS], GT_FALSE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosTrustModeSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /*  reset the per port per UP the upProfile index */
    rc = prvTgfCosPortUpProfileIndexSet(
        prvTgfDevsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        PRV_TGF_COS_TEST_UP0,
        0);
    if(rc == GT_NOT_APPLICABLE_DEVICE)
    {
        /* this device not support multiple tables */
    }
    else if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosConfigSet FAILED, rc = [%d]", rc);
        return rc;
    }

    rc = prvTgfCosPortUpProfileIndexSet(
        prvTgfDevsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        PRV_TGF_COS_TEST_UP1,
        0);
    if(rc == GT_NOT_APPLICABLE_DEVICE)
    {
        /* this device not support multiple tables */
    }
    else if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosConfigSet FAILED, rc = [%d]", rc);
        return rc;
    }


    return GT_OK;
}

/*******************************************************************************
* prvTgfCosTestSendPacketAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets of given Traffic classes
*
* INPUTS:
*       packetInfoPtr           - (pointer to) the packet info
*       burstCount              -  burst size
*       tc0                     -  traffic class to count in counter set0
*       tc1                     -  traffic class to count in counter set1
*       expectedEgressTc0 - amount of packets expected to egress With TC0
*       expectedEgressTc1 - amount of packets expected to egress With TC1
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
static GT_VOID prvTgfCosTestSendPacketAndCheck
(
    IN TGF_PACKET_STC   *packetInfoPtr,
    IN GT_U32            burstCount,
    IN GT_U8             tc0,
    IN GT_U8             tc1,
    IN GT_U32            expectedEgressTc0,
    IN GT_U32            expectedEgressTc1
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U8                           cntrSetId;
    CPSS_PORT_EGRESS_CNTR_STC       egressCntrs;
    GT_U32                          egressPktSum;
    GT_U8                           tcArr[2];
    GT_U32                          expectedPackets[2];

    tcArr[0] = tc0;
    tcArr[1] = tc1;

    expectedPackets[0] = expectedEgressTc0;
    expectedPackets[1] = expectedEgressTc1;

    PRV_UTF_LOG0_MAC("======= Configure Counters Sets =======\n");

    for (cntrSetId = 0; (cntrSetId < 2); cntrSetId++)
    {
        rc = prvTgfPortEgressCntrModeSet(
            cntrSetId,
            (CPSS_EGRESS_CNT_TC_E | CPSS_EGRESS_CNT_PORT_E) /*setModeBmp*/,
            prvTgfPortsArray[PRV_TGF_COS_RECEIVE_PORT_INDEX_CNS] /*portNum*/,
            0 /*vlanId*/,
            tcArr[cntrSetId]/*tc*/,
            CPSS_DP_GREEN_E /*dpLevel*/);
        UTF_VERIFY_EQUAL0_STRING_MAC(
            GT_OK, rc, "prvTgfPortEgressCntrModeSet\n");
    }

    PRV_UTF_LOG0_MAC("======= Reset Counters =======\n");

    for (cntrSetId = 0; (cntrSetId < 2); cntrSetId++)
    {
        /* reset counters */
        rc = prvTgfPortEgressCntrsGet(
            cntrSetId, &egressCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(
            GT_OK, rc, "prvTgfPortEgressCntrsGet\n");
    }

    PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(
        prvTgfDevsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        packetInfoPtr, burstCount, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(
        GT_OK, rc, "prvTgfSetTxSetupEth: %d, %d",
        prvTgfDevsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS], burstCount, 0, 0);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* send Packet from port 0 */
    rc = prvTgfStartTransmitingEth(
        prvTgfDevsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfStartTransmitingEth: %d %d",
        prvTgfDevsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS]);

    /* Delay 100 milliseconds - give to sent packet to pass all pilelines in PP */
    cpssOsTimerWkAfter(100);


    for (cntrSetId = 0; (cntrSetId < 2); cntrSetId++)
    {
        /* read counters */
        rc = prvTgfPortEgressCntrsGet(
            cntrSetId, &egressCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(
            GT_OK, rc, "prvTgfPortEgressCntrsGet\n");

        egressPktSum = egressCntrs.outUcFrames + egressCntrs.outMcFrames
            + egressCntrs.outCtrlFrames + egressCntrs.outBcFrames;

        UTF_VERIFY_EQUAL0_STRING_MAC(
            expectedPackets[cntrSetId], egressPktSum, "Egress Counter result\n");
    }

}


/*******************************************************************************
* prvTgfCosExpTestTrafficGenerateAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets with both TCs
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
GT_VOID prvTgfCosExpTestTrafficGenerateAndCheck
(
    GT_VOID
)
{
    GT_STATUS                 rc;

    /* sset Exp Trust mode */
    rc = prvTgfCosTrustExpModeSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS], GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosTrustModeSet FAILED, rc = [%d]", rc);
        return;
    }

    prvTgfCosTestSendPacketAndCheck(
        &prvTgfMplsPacketInfo,
        2 /*burstCount*/,
        PRV_TGF_COS_TEST_TC0 /*tc0*/,
        PRV_TGF_COS_TEST_TC1 /*tc1*/,
        0 /*expectedEgressTc0*/,
        2 /*expectedEgressTc1*/);

    /* reset Exp Trust mode */
    rc = prvTgfCosTrustExpModeSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS], GT_FALSE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosTrustModeSet FAILED, rc = [%d]", rc);
        return;
    }

    prvTgfCosTestSendPacketAndCheck(
        &prvTgfMplsPacketInfo,
        1 /*burstCount*/,
        PRV_TGF_COS_TEST_TC0 /*tc0*/,
        PRV_TGF_COS_TEST_TC1 /*tc1*/,
        1 /*expectedEgressTc0*/,
        0 /*expectedEgressTc1*/);
}

/*******************************************************************************
* prvTgfCosVlanTagSelectTestTrafficGenerateAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets with both TCs
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
GT_VOID prvTgfCosVlanTagSelectTestTrafficGenerateAndCheck
(
    GT_VOID
)
{
    GT_STATUS                 rc;

    /* set Up Trust mode */
    rc = prvTgfCosPortQosTrustModeSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        CPSS_QOS_PORT_TRUST_L2_E);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosPortQosTrustModeSet FAILED, rc = [%d]", rc);
        return;
    }

    /* set Up Vlan tag1 select mode */
    rc = prvTgfCosL2TrustModeVlanTagSelectSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        CPSS_VLAN_TAG1_E);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosL2TrustModeVlanTagSelectSet FAILED, rc = [%d]", rc);
        return;
    }

    prvTgfCosTestSendPacketAndCheck(
        &prvTgfEthOtherPacketInfo,
        2 /*burstCount*/,
        PRV_TGF_COS_TEST_TC0 /*tc0*/,
        PRV_TGF_COS_TEST_TC1 /*tc1*/,
        0 /*expectedEgressTc0*/,
        2 /*expectedEgressTc1*/);

    /* set Up Vlan tag0 select mode */
    rc = prvTgfCosL2TrustModeVlanTagSelectSet(
        prvTgfPortsArray[PRV_TGF_COS_SEND_PORT_INDEX_CNS],
        CPSS_VLAN_TAG0_E);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfCosL2TrustModeVlanTagSelectSet FAILED, rc = [%d]", rc);
        return;
    }

    prvTgfCosTestSendPacketAndCheck(
        &prvTgfEthOtherPacketInfo,
        1 /*burstCount*/,
        PRV_TGF_COS_TEST_TC0 /*tc0*/,
        PRV_TGF_COS_TEST_TC1 /*tc1*/,
        1 /*expectedEgressTc0*/,
        0 /*expectedEgressTc1*/);
}

