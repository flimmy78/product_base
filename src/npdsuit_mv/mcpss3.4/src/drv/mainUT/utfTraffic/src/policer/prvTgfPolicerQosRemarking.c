/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPolicerQosRemarking.c
*
* DESCRIPTION:
*       Specific Policer features testing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/pcl/cpssPcl.h>
#include <cpss/generic/cpssCommonDefs.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <policer/prvTgfPolicerQosRemarking.h>

#include <common/tgfCommon.h>
#include <common/tgfIpGen.h>
#include <common/tgfPolicerGen.h>
#include <common/tgfPolicerStageGen.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfCosGen.h>
#include <common/tgfPortGen.h>


/* default VLAN Id */
#define PRV_TGF_VLANID_CNS            3

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_NUM_CNS     0

/* port number to receive traffic from */
#define PRV_TGF_RECEIVE_PORT_NUM_CNS  1

/* count of packets to send */
#define PRV_TGF_SEND_COUNT_CNS        1

/* default number of packets to send */
static GT_U32  prvTgfBurstCount   = 1;

/* PCL Rule index */
static GT_U32   prvTgfPclRuleIndex = 1;

/* QoS Profile index */
static GT_U32   prvTgfQosProfileIndex = 5;

/* Egress Counters Set number */
static GT_U8   prvTgfEgressCntrSetNum = 0;

/* Qos Profile index for Ingress Remark for Yellow packets */
static GT_U32   prvTgfYellowQosTableRemarkIndex = 10;

/* Qos Profile index for Ingress Remark for Red packets */
static GT_U32   prvTgfRedQosTableRemarkIndex = 20;

/* Policy Counters for check */
PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC prvTgfMngCntrGreen;
PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC prvTgfMngCntrRed;
PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC prvTgfMngCntrYellow;
PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC prvTgfMngCntrDrop;


/************************ common parts of packets **************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0x00, 0x00, 0x22, 0x22, 0x22, 0x22},               /* dstMac */
    {0x00, 0x00, 0x11, 0x11, 0x11, 0x11}                /* srcMac */
};

/* First VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    5, 0, PRV_TGF_VLANID_CNS                            /* pri, cfi, VlanId */
};

/************************* packet 1 (IPv4 packet) **************************/


/* DATA of packet */
static GT_U8 prvTgfPayloadDataArr[] = {
    0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};


/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart = {
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of single tagged packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of single tagged packet */
#define PRV_TGF_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)


/* Single tagged PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfo =
{
    PRV_TGF_PACKET_LEN_CNS,                                    /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArray                                        /* partsArray */
};


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfPolicerTestPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
*       portNum         - port number
*       packetInfoPtr   - (pointer to) the packet info
*       burstCount      - number of packets to send
*       milsInterval    - interval between of packets in milliseconds
*       isCaptureEnable - enable of capturing packets
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
static GT_VOID prvTgfPolicerTestPacketSend
(
    IN  TGF_PACKET_STC *packetInfoPtr
)
{
    GT_STATUS                   rc         = GT_OK;
    GT_U32                      portsCount = prvTgfPortsNum;
    GT_U32                      portIter   = 0;
    CPSS_INTERFACE_INFO_STC     portInterface;


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
                                 prvTgfDevNum, prvTgfBurstCount, 0, NULL);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_NUM_CNS];

    cpssOsTimerWkAfter(10);


    /* Start capturing Tx packets */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d, %d",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum, GT_TRUE);


    /* send Packet from port port 0 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS]);

    cpssOsTimerWkAfter(10);


    /* Stop capturing Tx packets */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d, %d",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum, GT_FALSE);

    cpssOsTimerWkAfter(10);
}


/*******************************************************************************
* prvTgfPolicerCountersReset
*
* DESCRIPTION:
*       Resets the value of specified Management and Policy Counters
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
static GT_VOID prvTgfPolicerCountersReset
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage
)
{
    GT_STATUS rc = GT_OK;

    cpssOsMemSet(&prvTgfMngCntrGreen, 0, sizeof(prvTgfMngCntrGreen));
    cpssOsMemSet(&prvTgfMngCntrRed, 0, sizeof(prvTgfMngCntrRed));
    cpssOsMemSet(&prvTgfMngCntrYellow, 0, sizeof(prvTgfMngCntrYellow));
    cpssOsMemSet(&prvTgfMngCntrDrop, 0, sizeof(prvTgfMngCntrDrop));

    rc = prvTgfPolicerManagementCountersSet(prvTgfDevNum, policerStage,
        PRV_TGF_POLICER_MNG_CNTR_SET0_E, PRV_TGF_POLICER_MNG_CNTR_GREEN_E, &prvTgfMngCntrGreen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerManagementCountersSet: %d", prvTgfDevNum);

    rc = prvTgfPolicerManagementCountersSet(prvTgfDevNum, policerStage,
        PRV_TGF_POLICER_MNG_CNTR_SET0_E, PRV_TGF_POLICER_MNG_CNTR_YELLOW_E, &prvTgfMngCntrYellow);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerManagementCountersSet: %d", prvTgfDevNum);

    rc = prvTgfPolicerManagementCountersSet(prvTgfDevNum, policerStage,
        PRV_TGF_POLICER_MNG_CNTR_SET0_E, PRV_TGF_POLICER_MNG_CNTR_RED_E, &prvTgfMngCntrRed);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerManagementCountersSet: %d", prvTgfDevNum);

    rc = prvTgfPolicerManagementCountersSet(prvTgfDevNum, policerStage,
        PRV_TGF_POLICER_MNG_CNTR_SET0_E, PRV_TGF_POLICER_MNG_CNTR_DROP_E, &prvTgfMngCntrDrop);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerManagementCountersSet: %d", prvTgfDevNum);
}


/*******************************************************************************
* prvTgfPolicerQosRemarkingPclConfigurationSet
*
* DESCRIPTION:
*       None
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
static GT_VOID prvTgfPolicerQosRemarkingPclConfigurationSet
(
    IN CPSS_PCL_DIRECTION_ENT           pclDirection,
    IN GT_U8                            portNum
)
{
    GT_STATUS rc = GT_OK;
    PRV_TGF_PCL_RULE_FORMAT_UNT         mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT         patt;
    PRV_TGF_PCL_ACTION_STC              action;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT    ruleFormat;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT    nonIpKey;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT    ipv4Key;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT    ipv6Key;

    /* clear mask, pattern and action */
    cpssOsMemSet(&mask,   0, sizeof(mask));
    cpssOsMemSet(&patt,   0, sizeof(patt));
    cpssOsMemSet(&action, 0, sizeof(action));

    /* init PCL Engine for send/receive port */
    if (pclDirection == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        ruleFormat = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

        /* difine mask, pattern and action */
        cpssOsMemSet(mask.ruleStdNotIp.macDa.arEther, 0xFF, sizeof(GT_ETHERADDR));

        cpssOsMemCpy(patt.ruleStdNotIp.macDa.arEther, prvTgfPacketL2Part.daMac,
                sizeof(prvTgfPacketL2Part.daMac));

        nonIpKey = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        ipv4Key = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        ipv6Key = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    }
    else
    {
        ruleFormat = PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
        action.egressPolicy          = GT_TRUE;

        /* difine mask, pattern and action */
        cpssOsMemSet(mask.ruleEgrStdNotIp.macDa.arEther, 0xFF, sizeof(GT_ETHERADDR));

        cpssOsMemCpy(patt.ruleEgrStdNotIp.macDa.arEther, prvTgfPacketL2Part.daMac,
                sizeof(prvTgfPacketL2Part.daMac));

        /* use PCL ID to avoid problems with EPCL based WAs */
        mask.ruleEgrStdNotIp.common.pclId = 0x3FF;
        patt.ruleEgrStdNotIp.common.pclId = (GT_U16) PRV_TGF_PCL_DEFAULT_ID_MAC(pclDirection, CPSS_PCL_LOOKUP_0_E, portNum);

        nonIpKey = PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
        ipv4Key = PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;
        ipv6Key = PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;
    }

    rc = prvTgfPclDefPortInit(
        portNum,
        pclDirection,
        CPSS_PCL_LOOKUP_0_E,
        nonIpKey, ipv4Key, ipv6Key);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclDefPortInit: %d", prvTgfDevNum);


    /* set action */
    action.pktCmd                = CPSS_PACKET_CMD_FORWARD_E;
    action.policer.policerEnable = GT_TRUE;
    action.policer.policerId     = prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS];

    rc = prvTgfPclRuleSet(
        ruleFormat,
        prvTgfPclRuleIndex, &mask, &patt, &action);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleSet: %d", prvTgfDevNum);
};


/*******************************************************************************
* prvTgfPolicerQosRemarkingConfigurationSet
*
* DESCRIPTION:
*       Set configuration.
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
static GT_VOID prvTgfPolicerQosRemarkingConfigurationSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT           policerStage,
    IN PRV_TGF_POLICER_STAGE_METER_MODE_ENT     stageMeterMode,
    IN PRV_TGF_POLICER_NON_CONFORM_CMD_ENT      pcktCmd,
    IN PRV_TGF_POLICER_METER_TB_PARAMS_UNT      *tbParamsPtr,
    IN PRV_TGF_COS_PROFILE_STC                  *yellowCosProfilePtr,
    IN PRV_TGF_COS_PROFILE_STC                  *redCosProfilePtr,
    IN PRV_TGF_POLICER_REMARK_MODE_ENT          egressRemarkMode,
    IN PRV_TGF_POLICER_L2_REMARK_MODEL_ENT      egressRemarkModel,
    IN PRV_TGF_POLICER_REMARK_TABLE_TYPE_ENT    egressRemarkTableType,
    IN GT_U32                                   egressRemarkParamValue,
    IN CPSS_DP_LEVEL_ENT                        egressConfLevel,
    IN PRV_TGF_POLICER_QOS_PARAM_STC            *egressQosParamPtr
)
{
    GT_STATUS                           rc = GT_OK;
    PRV_TGF_POLICER_ENTRY_STC           metteringEntry;
    PRV_TGF_POLICER_METER_TB_PARAMS_UNT tbParams;
    GT_U32                              metteringEntryIndex;
    GT_BOOL                             isMeterModePortEnable;
    CPSS_QOS_ENTRY_STC                  portQosCfg;


    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);


    /* b) enable metering */
    rc = prvTgfPolicerMeteringEnableSet(policerStage, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerMeteringEnableSet: %d", prvTgfDevNum);

    /* set metered Packet Size Mode as CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E */
    rc = prvTgfPolicerPacketSizeModeSet(policerStage,
            CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerPacketSizeModeSet: %d", prvTgfDevNum);

    /* set metering algorithm resolution as packets per second */
    rc = prvTgfPolicerMeterResolutionSet(policerStage,
            PRV_TGF_POLICER_METER_RESOLUTION_PACKETS_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerMeterResolutionSet: %d", prvTgfDevNum);

    /* set Policer Global stage mode Port or Flow */
    rc = prvTgfPolicerStageMeterModeSet(policerStage, stageMeterMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerStageMeterModeSet: %d", prvTgfDevNum);

    /* make sure we have counting */
    rc = prvTgfPolicerCountingModeSet(prvTgfDevNum, policerStage,
                                      PRV_TGF_POLICER_COUNTING_DISABLE_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingModeSet: %d", prvTgfDevNum);

    /* select Policer Global stage mode */
    isMeterModePortEnable = (stageMeterMode == PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E);

    /* c) enable metering per SEND Port */
    rc = prvTgfPolicerPortMeteringEnableSet(prvTgfDevNum, policerStage,
            prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS], isMeterModePortEnable);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPolicerPortMeteringEnableSet: %d %d",
            prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS]);

    /* d) set Counter Color mode to Drop Precedence */
    rc = prvTgfPolicerCountingColorModeSet(policerStage, PRV_TGF_POLICER_COLOR_COUNT_DP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingColorModeSet: %d", prvTgfDevNum); 


    /* h) create metering entry with index 0 - it means that port is 0 */
    metteringEntryIndex               = prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS];

    cpssOsMemSet(&metteringEntry, 0, sizeof(metteringEntry));
    metteringEntry.countingEntryIndex = metteringEntryIndex;
    metteringEntry.mngCounterSet      = PRV_TGF_POLICER_MNG_CNTR_SET0_E;
    metteringEntry.meterColorMode     = CPSS_POLICER_COLOR_BLIND_E;
    metteringEntry.meterMode          = PRV_TGF_POLICER_METER_MODE_SR_TCM_E;
    metteringEntry.tbParams.srTcmParams.cir = tbParamsPtr->srTcmParams.cir;
    metteringEntry.tbParams.srTcmParams.cbs = tbParamsPtr->srTcmParams.cbs;
    metteringEntry.tbParams.srTcmParams.ebs = tbParamsPtr->srTcmParams.ebs;
    metteringEntry.modifyUp           = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
    metteringEntry.modifyDscp         = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
    metteringEntry.modifyDp           = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
    metteringEntry.yellowPcktCmd      = pcktCmd;
    metteringEntry.redPcktCmd         = pcktCmd;
    metteringEntry.qosProfile         = prvTgfQosProfileIndex;
    metteringEntry.remarkMode         = egressRemarkMode;

    rc = prvTgfPolicerEntrySet(policerStage,
                               metteringEntryIndex,
                               &metteringEntry, &tbParams);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
            "prvTgfPolicerEntrySet: %d %d", prvTgfDevNum, metteringEntryIndex);


    if (policerStage < PRV_TGF_POLICER_STAGE_EGRESS_E)
    {
        /* Ingress Remarking Configurations*/

        if (pcktCmd == PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E)
        {
            rc = prvTgfCosProfileEntrySet((GT_U8)prvTgfQosProfileIndex, yellowCosProfilePtr);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);
        }

        /* Make nessasary configurations for "Remark by Table"*/
        if (pcktCmd == PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_E)
        {
            rc = prvTgfCosProfileEntrySet((GT_U8)prvTgfYellowQosTableRemarkIndex,
                                          yellowCosProfilePtr);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

            rc = prvTgfCosProfileEntrySet((GT_U8)prvTgfRedQosTableRemarkIndex,
                                          redCosProfilePtr);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);


            /* g) create Relative Qos Remarking Entry with index 5 */
            rc = prvTgfPolicerQosRemarkingEntrySet(policerStage,
                    prvTgfQosProfileIndex, prvTgfYellowQosTableRemarkIndex, prvTgfRedQosTableRemarkIndex);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerQosRemarkingEntrySet: %d", prvTgfDevNum);

            /* Configure Port QoS attributes */
            portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            portQosCfg.qosProfileId     = prvTgfQosProfileIndex;

            rc = prvTgfCosPortQosConfigSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS],
                                           &portQosCfg);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosConfigSet: %d", prvTgfDevNum);

            /* Configure Port to No_Trust */
            rc = prvTgfCosPortQosTrustModeSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS],
                                              CPSS_QOS_PORT_NO_TRUST_E);


            /* Configure Egress Counters set0: Port, TC and DP */
            rc = prvTgfPortEgressCntrModeSet(prvTgfEgressCntrSetNum,
                                             CPSS_EGRESS_CNT_PORT_E | CPSS_EGRESS_CNT_TC_E | CPSS_EGRESS_CNT_DP_E,
                                             prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_NUM_CNS],
                                             0,
                                             (GT_U8)yellowCosProfilePtr->trafficClass,
                                             yellowCosProfilePtr->dropPrecedence);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrModeSet: %d", prvTgfDevNum);
        }
    }
    else
    {
        /* Egress Remarking Configurations */

        /* Configure remark model */
        rc = prvTgfPolicerEgressL2RemarkModelSet(egressRemarkModel);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerEgressL2RemarkModelSet %d", prvTgfDevNum);


        rc = prvTgfPolicerEgressQosRemarkingEntrySet(egressRemarkTableType,
                                                     egressRemarkParamValue,
                                                     egressConfLevel ,
                                                     egressQosParamPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerEgressQosRemarkingEntrySet: %d", prvTgfDevNum);
    }

    /* reset the value of specified Management and Policy Counters */
    prvTgfPolicerCountersReset(policerStage);
}

/*******************************************************************************
* prvTgfPolicerCountersPrint
*
* DESCRIPTION:
*       Print the value of specified Management and Policy Counters
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
static GT_VOID prvTgfPolicerCountersPrint
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage
)
{
    GT_STATUS rc = GT_OK;

    /* get and print Management Counters GREEN */
    rc = prvTgfPolicerManagementCountersGet(prvTgfDevNum, policerStage,
        PRV_TGF_POLICER_MNG_CNTR_SET0_E, PRV_TGF_POLICER_MNG_CNTR_GREEN_E, &prvTgfMngCntrGreen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgf3PolicerManagementCountersGet: %d", prvTgfDevNum);

    if (prvTgfMngCntrGreen.duMngCntr.l[0] != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrGreen.duMngCntr.l[0] = %d\n", prvTgfMngCntrGreen.duMngCntr.l[0]);
    if (prvTgfMngCntrGreen.duMngCntr.l[1] != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrGreen.duMngCn tr.l[1] = %d\n", prvTgfMngCntrGreen.duMngCntr.l[1]);
    if (prvTgfMngCntrGreen.packetMngCntr != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrGreen.packetMngCntr = %d\n", prvTgfMngCntrGreen.packetMngCntr);

    /* get and print Management Counters YELLOW */
    rc = prvTgfPolicerManagementCountersGet(prvTgfDevNum, policerStage,
        PRV_TGF_POLICER_MNG_CNTR_SET0_E, PRV_TGF_POLICER_MNG_CNTR_YELLOW_E, &prvTgfMngCntrYellow);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerManagementCountersGet: %d", prvTgfDevNum);

    if (prvTgfMngCntrYellow.duMngCntr.l[0] != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrYellow.duMngCntr.l[0] = %d\n", prvTgfMngCntrYellow.duMngCntr.l[0]);
    if (prvTgfMngCntrYellow.duMngCntr.l[1] != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrYellow.duMngCntr.l[1] = %d\n", prvTgfMngCntrYellow.duMngCntr.l[1]);
    if (prvTgfMngCntrYellow.packetMngCntr != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrYellow.packetMngCntr = %d\n", prvTgfMngCntrYellow.packetMngCntr);

    /* get and print Management Counters RED */
    rc = prvTgfPolicerManagementCountersGet(prvTgfDevNum, policerStage,
        PRV_TGF_POLICER_MNG_CNTR_SET0_E, PRV_TGF_POLICER_MNG_CNTR_RED_E, &prvTgfMngCntrRed);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerManagementCountersGet: %d", prvTgfDevNum);

    if (prvTgfMngCntrRed.duMngCntr.l[0] != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrRed.duMngCntr.l[0] = %d\n", prvTgfMngCntrRed.duMngCntr.l[0]);
    if (prvTgfMngCntrRed.duMngCntr.l[1] != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrRed.duMngCntr.l[1] = %d\n", prvTgfMngCntrRed.duMngCntr.l[1]);
    if (prvTgfMngCntrRed.packetMngCntr != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrRed.packetMngCntr = %d\n", prvTgfMngCntrRed.packetMngCntr);

    /* get and print Management Counters DROP */
    rc = prvTgfPolicerManagementCountersGet(prvTgfDevNum, policerStage,
        PRV_TGF_POLICER_MNG_CNTR_SET0_E, PRV_TGF_POLICER_MNG_CNTR_DROP_E, &prvTgfMngCntrDrop);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerManagementCountersGet: %d", prvTgfDevNum);

    if (prvTgfMngCntrDrop.duMngCntr.l[0] != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrDrop.duMngCntr.l[0] = %d\n", prvTgfMngCntrDrop.duMngCntr.l[0]);
    if (prvTgfMngCntrDrop.duMngCntr.l[1] != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrDrop.duMngCntr.l[1] = %d\n", prvTgfMngCntrDrop.duMngCntr.l[1]);
    if (prvTgfMngCntrDrop.packetMngCntr != 0)
        PRV_UTF_LOG1_MAC("prvTgfMngCntrDrop.packetMngCntr = %d\n", prvTgfMngCntrDrop.packetMngCntr);

    PRV_UTF_LOG0_MAC("\n");
}

/*******************************************************************************
* prvTgfPolicerManagementCountersGreenCheck
*
* DESCRIPTION:
*       Check the value of specified Management Counters Green
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
static GT_VOID prvTgfPolicerManagementCountersGreenCheck
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage,
    IN GT_U32 duLowCntr,
    IN GT_U32 duHiCntr,
    IN GT_U32 packetMngCntr
)
{
    GT_STATUS rc = GT_OK;

    /* get Management Counters */
    rc = prvTgfPolicerManagementCountersGet(prvTgfDevNum, policerStage,
                                            PRV_TGF_POLICER_MNG_CNTR_SET0_E,
                                            PRV_TGF_POLICER_MNG_CNTR_GREEN_E,
                                            &prvTgfMngCntrGreen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
        "prvTgf3PolicerManagementCountersGet: %d", prvTgfDevNum);

    /* check counters */
    UTF_VERIFY_EQUAL1_STRING_MAC(
        duLowCntr, prvTgfMngCntrGreen.duMngCntr.l[0],
        "prvTgfMngCntrGreen.duMngCntr.l[0] = %d\n",
        prvTgfMngCntrGreen.duMngCntr.l[0]);

    UTF_VERIFY_EQUAL1_STRING_MAC(
        duHiCntr, prvTgfMngCntrGreen.duMngCntr.l[1],
        "prvTgfMngCntrGreen.duMngCntr.l[1] = %d\n",
        prvTgfMngCntrGreen.duMngCntr.l[1]);

    UTF_VERIFY_EQUAL1_STRING_MAC(
        packetMngCntr, prvTgfMngCntrGreen.packetMngCntr,
        "prvTgfMngCntrGreen.packetMngCntr = %d\n",
        prvTgfMngCntrGreen.packetMngCntr);
}

/*******************************************************************************
* prvTgfPolicerManagementCountersYellowCheck
*
* DESCRIPTION:
*       Check the value of specified Management Counters Yellow
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
static GT_VOID prvTgfPolicerManagementCountersYellowCheck
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage,
    IN GT_U32 duLowCntr,
    IN GT_U32 duHiCntr,
    IN GT_U32 packetMngCntr
)
{
    GT_STATUS                        rc = GT_OK;

    /* get Management Counters */
    rc = prvTgfPolicerManagementCountersGet(prvTgfDevNum, policerStage,
                                            PRV_TGF_POLICER_MNG_CNTR_SET0_E,
                                            PRV_TGF_POLICER_MNG_CNTR_YELLOW_E,
                                            &prvTgfMngCntrYellow);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
        "prvTgf3PolicerManagementCountersGet: %d", prvTgfDevNum);

    /* check counters */
    UTF_VERIFY_EQUAL1_STRING_MAC(
        duLowCntr, prvTgfMngCntrYellow.duMngCntr.l[0],
        "prvTgfMngCntrYellow.duMngCntr.l[0] = %d\n",
        prvTgfMngCntrYellow.duMngCntr.l[0]);

    UTF_VERIFY_EQUAL1_STRING_MAC(
        duHiCntr, prvTgfMngCntrYellow.duMngCntr.l[1],
        "prvTgfMngCntrYellow.duMngCntr.l[1] = %d\n",
        prvTgfMngCntrYellow.duMngCntr.l[1]);

    UTF_VERIFY_EQUAL1_STRING_MAC(
        packetMngCntr, prvTgfMngCntrYellow.packetMngCntr,
        "prvTgfMngCntrYellow.packetMngCntr = %d\n",
        prvTgfMngCntrYellow.packetMngCntr);
}

/*******************************************************************************
* prvTgfPolicerManagementCountersRedCheck
*
* DESCRIPTION:
*       Check the value of specified Management Counters Red
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
static GT_VOID prvTgfPolicerManagementCountersRedCheck
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage,
    IN GT_U32 duLowCntr,
    IN GT_U32 duHiCntr,
    IN GT_U32 packetMngCntr
)
{
    GT_STATUS                        rc = GT_OK;

    /* get Management Counters */
    rc = prvTgfPolicerManagementCountersGet(prvTgfDevNum, policerStage,
                                            PRV_TGF_POLICER_MNG_CNTR_SET0_E,
                                            PRV_TGF_POLICER_MNG_CNTR_RED_E,
                                            &prvTgfMngCntrRed);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
        "prvTgf3PolicerManagementCountersGet: %d", prvTgfDevNum);

    /* check counters */
    UTF_VERIFY_EQUAL1_STRING_MAC(
        duLowCntr, prvTgfMngCntrRed.duMngCntr.l[0],
        "prvTgfMngCntrRed.duMngCntr.l[0] = %d\n",
        prvTgfMngCntrRed.duMngCntr.l[0]);

    UTF_VERIFY_EQUAL1_STRING_MAC(
        duHiCntr, prvTgfMngCntrRed.duMngCntr.l[1],
        "prvTgfMngCntrRed.duMngCntr.l[1] = %d\n",
        prvTgfMngCntrRed.duMngCntr.l[1]);

    UTF_VERIFY_EQUAL1_STRING_MAC(
        packetMngCntr, prvTgfMngCntrRed.packetMngCntr,
        "prvTgfMngCntrRed.packetMngCntr = %d\n",
        prvTgfMngCntrRed.packetMngCntr);
}
/*******************************************************************************
* prvTgfPolicerManagementCountersDropCheck
*
* DESCRIPTION:
*       Check the value of specified Management Counters Drop
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
static GT_VOID prvTgfPolicerManagementCountersDropCheck
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage,
    IN GT_U32 duLowCntr,
    IN GT_U32 duHiCntr,
    IN GT_U32 packetMngCntr
)
{
    GT_STATUS                        rc = GT_OK;

    /* get Management Counters */
    rc = prvTgfPolicerManagementCountersGet(prvTgfDevNum, policerStage,
                                            PRV_TGF_POLICER_MNG_CNTR_SET0_E,
                                            PRV_TGF_POLICER_MNG_CNTR_DROP_E,
                                            &prvTgfMngCntrDrop);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
        "prvTgf3PolicerManagementCountersGet: %d", prvTgfDevNum);

    /* check counters */
    UTF_VERIFY_EQUAL1_STRING_MAC(
        duLowCntr, prvTgfMngCntrDrop.duMngCntr.l[0],
        "prvTgfMngCntrDrop.duMngCntr.l[0] = %d\n",
        prvTgfMngCntrDrop.duMngCntr.l[0]);

    UTF_VERIFY_EQUAL1_STRING_MAC(
        duHiCntr, prvTgfMngCntrDrop.duMngCntr.l[1],
        "prvTgfMngCntrDrop.duMngCntr.l[1] = %d\n",
        prvTgfMngCntrDrop.duMngCntr.l[1]);

    UTF_VERIFY_EQUAL1_STRING_MAC(
        packetMngCntr, prvTgfMngCntrDrop.packetMngCntr,
        "prvTgfMngCntrDrop.packetMngCntr = %d\n",
        prvTgfMngCntrDrop.packetMngCntr);
}


/*******************************************************************************
* prvTgfPolicerTxEthTriggerCheck
*
* DESCRIPTION:
*       Check Tx Triggers
*
* INPUTS:
*       vfdArray      - VFD array with expected results
*       vfdNum        - VFD number
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
static GT_VOID prvTgfPolicerTxEthTriggerCheck
(
    IN GT_U8                    vfdNum,
    IN TGF_VFD_INFO_STC         vfdArray[]
)
{
    GT_STATUS                       rc;
    GT_U8                           queue    = 0;
    GT_BOOL                         getFirst = GT_TRUE;
    GT_U32                          buffLen  = 0x600;
    GT_U32                          packetActualLength = 0;
    static  GT_U8                   packetBuff[0x600] = {0};
    TGF_NET_DSA_STC                 rxParam;
    GT_BOOL                         triggered;
    GT_U8                           devNum;


    /* Get entry from captured packet's table */
    rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                       getFirst, GT_TRUE, packetBuff,
                                       &buffLen, &packetActualLength,
                                       &devNum, &queue,
                                       &rxParam);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficGeneratorRxInCpuGet %d", TGF_PACKET_TYPE_CAPTURE_E);

    /* Check that the pattern is equal to vfdArray */
    rc = tgfTrafficGeneratorTxEthTriggerCheck(packetBuff,
                                              buffLen,
                                              vfdNum,
                                              vfdArray,
                                              &triggered);
    UTF_VERIFY_EQUAL2_STRING_MAC(rc, GT_OK, "Error in tgfTrafficGeneratorTxEthTriggerCheck %d, %d", buffLen, vfdNum);

    UTF_VERIFY_EQUAL0_STRING_MAC(GT_TRUE, triggered,
                                 "Packet VLAN TAG0 pattern is wrong");
}


/*******************************************************************************
* prvTgfPolicerCheckResults
*
* DESCRIPTION:
*       Check Tx Triggers
*
* INPUTS:
*       policerStage - policer stage
*       up          - User Priority
*
* OUTPUTS:
*       expectedBytesPtr - Number of received bytes
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfPolicerCheckResults
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT   policerStage,
    IN GT_U32                           up,
    OUT GT_U32                          *expectedBytesPtr
)
{
    GT_U32                              portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC       portCntrs;
    TGF_VFD_INFO_STC                    vfdArray[1];
    CPSS_PORT_EGRESS_CNTR_STC           egrCntr;
    GT_STATUS                           rc = GT_OK;

    /* check ETH counters */
    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_NUM_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        if (PRV_TGF_RECEIVE_PORT_NUM_CNS == portIter)
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");

            *expectedBytesPtr = (portCntrs.goodOctetsSent.l[0] * prvTgfBurstCount);
        }
    }

    /* Configure vfdArray */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].cycleCount = 4;
    vfdArray[0].patternPtr[0] = 0x81;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = (GT_U8)(up << 5);
    vfdArray[0].patternPtr[3] = (GT_U8)prvTgfPacketVlanTagPart.vid;


    prvTgfPolicerTxEthTriggerCheck(1, vfdArray);

    /* In Egress Policer stage, the egress counters show the result before
       Egress Policer Remark */
    if (policerStage < PRV_TGF_POLICER_STAGE_EGRESS_E)
    {
        /* Read Egress counters */
        rc = prvTgfPortEgressCntrsGet(prvTgfEgressCntrSetNum, &egrCntr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrsGet: %d", prvTgfDevNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(prvTgfBurstCount, egrCntr.outUcFrames, "The number of UC packets is not as expected %d", prvTgfBurstCount);
    }
}

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPolicerQosRemarkingVlanConfigurationSet
*
* DESCRIPTION:
*       Set configuration.
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
GT_VOID prvTgfPolicerQosRemarkingVlanConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_PORT_EGRESS_CNTR_STC           egrCntr;

    /* create a vlan on ports (0,0), (0,8), (0,18), (0,23) */
    rc = prvTgfBrgDefVlanEntryWriteWithTaggingCmd(PRV_TGF_VLANID_CNS, PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWriteWithTaggingCmd: %d",
                                 prvTgfDevNum);

    /* create a macEntry with PORT interface for DA of packet 1 */
    rc = prvTgfBrgDefFdbMacEntryOnPortSet(prvTgfPacketL2Part.daMac, PRV_TGF_VLANID_CNS,
                                          prvTgfDevNum,
                                          prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_NUM_CNS],
                                          GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);

    /* Read Egress counters */
    rc = prvTgfPortEgressCntrsGet(prvTgfEgressCntrSetNum, &egrCntr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrsGet: %d", prvTgfDevNum);

    cpssOsTimerWkAfter(10);
}

/*******************************************************************************
* prvTgfPolicerQosRemarkingConfigurationRestore
*
* DESCRIPTION:
*       Restore configuration
*
* INPUTS:
*       policerStage    - Policer stage
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
GT_VOID prvTgfPolicerQosRemarkingConfigurationRestore
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_QOS_ENTRY_STC                  portQosCfg;
    PRV_TGF_COS_PROFILE_STC             cosProfile;
    PRV_TGF_POLICER_QOS_PARAM_STC       egressQosParam;
    GT_U32                              dpInd;
    CPSS_PORT_EGRESS_CNTR_STC           egrCntr;
    CPSS_INTERFACE_INFO_STC             interfaceInfo;
    PRV_TGF_PCL_LOOKUP_CFG_STC          lookupCfg;

    /* set the conformance level as GREEN */
    prvTgfPolicerConformanceLevelForce(0);

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush");

    /* Invalidate Vlan */
    rc = prvTgfBrgVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate");

    /* set default vlanId as 1 for Send port */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS], 1);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet");

     /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");


    /* Restore Policer and QOS Configurations */

    /* disable metering */
    rc = prvTgfPolicerMeteringEnableSet(policerStage, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerMeteringEnableSet: %d", prvTgfDevNum);

    /* disable metering per source Port */
    rc = prvTgfPolicerPortMeteringEnableSet(prvTgfDevNum, policerStage,
            prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS], GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPolicerPortMeteringEnableSet: %d %d",
            prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS]);

    /* reset Policer Global stage mode - PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E by default */
    rc = prvTgfPolicerStageMeterModeSet(policerStage, PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "pssDxChPolicerStageMeterModeSet: %d", prvTgfDevNum);

    /* reset metered Packet Size Mode as CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E */
    rc = prvTgfPolicerPacketSizeModeSet(policerStage,
            CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerPacketSizeModeSet: %d", prvTgfDevNum);

    /* reset counting mode to Billing */
    rc = prvTgfPolicerCountingModeSet(prvTgfDevNum, policerStage,
                                      PRV_TGF_POLICER_COUNTING_BILLING_IPFIX_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingModeSet: %d", prvTgfDevNum);

    /* flush counters */
    rc = prvTgfPolicerCountingWriteBackCacheFlush(policerStage);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingWriteBackCacheFlush: %d", prvTgfDevNum);


    /* set Counter Color mode to CL */
    rc = prvTgfPolicerCountingColorModeSet(policerStage, PRV_TGF_POLICER_COLOR_COUNT_CL_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingColorModeSet: %d", prvTgfDevNum); 


    if (policerStage < PRV_TGF_POLICER_STAGE_EGRESS_E)
    {
        /* Ingress stages */

        rc = prvTgfPolicerQosRemarkingEntrySet(policerStage, prvTgfQosProfileIndex, 0, 0);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerQosRemarkingEntrySet: %d", prvTgfDevNum);


        cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
        cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority   = 0;
        cosProfile.trafficClass   = 0;
        cosProfile.dscp           = 0x0;
        cosProfile.exp            = 0;

        rc = prvTgfCosProfileEntrySet((GT_U8)prvTgfQosProfileIndex, &cosProfile);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

        rc = prvTgfCosProfileEntrySet((GT_U8)prvTgfYellowQosTableRemarkIndex, &cosProfile);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

        rc = prvTgfCosProfileEntrySet((GT_U8)prvTgfRedQosTableRemarkIndex, &cosProfile);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet: %d", prvTgfDevNum);

        /* Configure Port QoS attributes */
        portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        portQosCfg.qosProfileId     = 0;

        rc = prvTgfCosPortQosConfigSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS],
                                       &portQosCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosConfigSet: %d", prvTgfDevNum);

        /* Configure Port to Trust L2 */
        rc = prvTgfCosPortQosTrustModeSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS],
                                          CPSS_QOS_PORT_TRUST_L2_E);

        /* restore Egress Counter configuration */
        rc = prvTgfPortEgressCntrModeSet(prvTgfEgressCntrSetNum, 0, prvTgfPortsArray[PRV_TGF_SEND_PORT_NUM_CNS],
                                         0, 0, 0);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrModeSet: %d", prvTgfDevNum);


        /* Clear Egress counters */
        rc = prvTgfPortEgressCntrsGet(prvTgfEgressCntrSetNum, &egrCntr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrsGet: %d", prvTgfDevNum);
    }
    else
    {
        /* Egress stage */

        /* Disable Remarking for conforming packets */
        rc = prvTgfPolicerEgressQosUpdateEnableSet(GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerEgressQosUpdateEnableSet %d", prvTgfDevNum);


        /* Configure remark model to TC */
        rc = prvTgfPolicerEgressL2RemarkModelSet(PRV_TGF_POLICER_L2_REMARK_MODEL_TC_E);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerEgressL2RemarkModelSet %d", prvTgfDevNum);

        egressQosParam.dp = CPSS_DP_GREEN_E;
        egressQosParam.dscp = 0x0;
        egressQosParam.exp = 0;
        egressQosParam.up = 0;

        for (dpInd = CPSS_DP_GREEN_E; dpInd <= CPSS_DP_RED_E; dpInd++)
        {
            rc = prvTgfPolicerEgressQosRemarkingEntrySet(PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E,
                                                         prvTgfPacketVlanTagPart.pri,
                                                         dpInd ,
                                                         &egressQosParam);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerEgressQosRemarkingEntrySet: %d", prvTgfDevNum);
        }

        /* clear PCL configuration table */
        cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
        cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

        interfaceInfo.type               = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.devNum     =
            prvTgfDevsArray[PRV_TGF_RECEIVE_PORT_NUM_CNS];
        interfaceInfo.devPort.portNum    =
            prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_NUM_CNS];

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

        /* disable egress policy per devices */
        rc = prvTgfPclEgressPclPacketTypesSet(
            prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_NUM_CNS],
            PRV_TGF_PCL_EGRESS_PKT_NON_TS_E, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclEgressPclPacketTypesSet: %d", prvTgfDevNum);


        /* invalidate PCL rules */
        rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_STD_E, prvTgfPclRuleIndex, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleValidStatusSet: %d", prvTgfDevNum);
    }
}


/*******************************************************************************
* prvTgfPolicerIngressQosRemarkingByEntryTrafficGenerate
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
GT_VOID prvTgfPolicerIngressQosRemarkingByEntryTrafficGenerate
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage
)
{
    GT_STATUS                           rc = GT_OK;
    GT_U32                              stage;
    GT_U32                              expectedBytes = 0;
    PRV_TGF_COS_PROFILE_STC             cosProfile;
    PRV_TGF_POLICER_METER_TB_PARAMS_UNT meterTbParams;


    if(GT_FALSE == prvTgfPolicerStageCheck(policerStage))
    {
        /* the device not supports this stage */
        return;
    }

    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG1_MAC("                    POLICER_STAGE_INGRESS_%d\n",
            policerStage);


    /* Disable Metering and Counting on all stages */
    for (stage = PRV_TGF_POLICER_STAGE_INGRESS_0_E; stage < PRV_TGF_POLICER_STAGE_NUM; stage++)
    {
        if(GT_FALSE == prvTgfPolicerStageCheck(stage))
        {
            /* the device not supports this stage */
            continue;
        }

        rc = prvTgfPolicerMeteringEnableSet(stage, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingModeSet: %d", prvTgfDevNum);

        rc = prvTgfPolicerCountingModeSet(prvTgfDevNum, stage, PRV_TGF_POLICER_COUNTING_DISABLE_E);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingModeSet: %d", prvTgfDevNum);
    }


    /* -------------------------------------------------------------------------
     * 1.1 Ingress Remarking By Entry per Source Port Metering with RED packets
     */
    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG0_MAC("1.1 Ingress Remarking By Entry per Source Port Metering"
                     " with RED packets\n");

    /* configure QoS Profile with index 5 - for Mark by entry command */
    cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
    cosProfile.dropPrecedence = CPSS_DP_RED_E;
    cosProfile.userPriority   = 7;
    cosProfile.trafficClass   = 4;
    cosProfile.dscp           = 0x32;
    cosProfile.exp            = 0;

    meterTbParams.srTcmParams.cir = 1;
    meterTbParams.srTcmParams.cbs = 1;
    meterTbParams.srTcmParams.ebs = 10304; /* 1 yellow packet */

    prvTgfPolicerQosRemarkingConfigurationSet(policerStage,
            PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E,
            PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E,
            &meterTbParams, &cosProfile, &cosProfile,
            0, 0, 0, 0, 0, NULL);

    /* set the conformance level as yellow */
    prvTgfPolicerConformanceLevelForce(1);

    /* send packet */
    prvTgfPolicerTestPacketSend(&prvTgfPacketInfo);

    /* Check Results */
    prvTgfPolicerCheckResults(policerStage, cosProfile.userPriority,
                              &expectedBytes);

    /* print the value of specified Management and Policy Counters */
    prvTgfPolicerCountersPrint(policerStage);

    /* check the value of specified Management Counters */
    /* 1 yellow packet after metering --> 1 green packet after remarking */
    prvTgfPolicerManagementCountersGreenCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersYellowCheck(policerStage, expectedBytes, 0, prvTgfBurstCount);
    prvTgfPolicerManagementCountersRedCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersDropCheck(policerStage, 0, 0, 0);


    /* -------------------------------------------------------------------------
     * 1.2 Ingress Remarking by Entry per Source Port Metering with GREEN packets
     */
    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG0_MAC("1.2 Ingress Remarking by Entry per Source Port Metering"
                     " with GREEN packets\n");


    /* configure QoS Profile with index 5 - for Mark by entry command */
    cpssOsMemSet(&cosProfile, 0, sizeof(cosProfile));
    cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
    cosProfile.userPriority   = 4;
    cosProfile.trafficClass   = 1;
    cosProfile.dscp           = 0x0;
    cosProfile.exp            = 0;


    meterTbParams.srTcmParams.cir = 1;
    meterTbParams.srTcmParams.cbs = 1;
    meterTbParams.srTcmParams.ebs = 1; /* 1 red packet */


    prvTgfPolicerQosRemarkingConfigurationSet(policerStage,
            PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E,
            PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E,
            &meterTbParams, &cosProfile, &cosProfile,
            0, 0, 0, 0, 0, NULL);

    /* set the conformance level as red */
    prvTgfPolicerConformanceLevelForce(2);

    /* send packets */
    prvTgfPolicerTestPacketSend(&prvTgfPacketInfo);

    /* Check Results */
    prvTgfPolicerCheckResults(policerStage, cosProfile.userPriority,
                              &expectedBytes);

    /* print the value of specified Management and Policy Counters */
    prvTgfPolicerCountersPrint(policerStage);

    /* check the value of specified Management Counters */
    /* 1 red packet after metering --> 1 yellow packet after remarking */
    prvTgfPolicerManagementCountersGreenCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersYellowCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersRedCheck(policerStage, expectedBytes, 0, prvTgfBurstCount);
    prvTgfPolicerManagementCountersDropCheck(policerStage, 0, 0, 0);

    prvTgfPolicerCountersReset(policerStage);
}


/*******************************************************************************
* prvTgfPolicerIngressQosRemarkingByTableTrafficGenerate
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
GT_VOID prvTgfPolicerIngressQosRemarkingByTableTrafficGenerate
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage
)
{
    GT_STATUS                           rc = GT_OK;
    GT_U32                              stage;
    GT_U32                              expectedBytes = 0;
    PRV_TGF_COS_PROFILE_STC             yellowCosProfile;
    PRV_TGF_COS_PROFILE_STC             redCosProfile;
    PRV_TGF_POLICER_METER_TB_PARAMS_UNT meterTbParams;

    if(GT_FALSE == prvTgfPolicerStageCheck(policerStage))
    {
        /* the device not supports this stage */
        return;
    }

    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG1_MAC("                    POLICER_STAGE_INGRESS_%d\n",
            policerStage);


    /* Disable Metering and Counting on all stages */
    for (stage = PRV_TGF_POLICER_STAGE_INGRESS_0_E; stage < PRV_TGF_POLICER_STAGE_NUM; stage++)
    {
        if(GT_FALSE == prvTgfPolicerStageCheck(stage))
        {
            /* the device not supports this stage */
            continue;
        }

        rc = prvTgfPolicerMeteringEnableSet(stage, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingModeSet: %d", prvTgfDevNum);

        rc = prvTgfPolicerCountingModeSet(prvTgfDevNum, stage, PRV_TGF_POLICER_COUNTING_DISABLE_E);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingModeSet: %d", prvTgfDevNum);
    }


    /* -------------------------------------------------------------------------
     * 1.1 Ingress Remarking By Table per Source Port Metering with RED packets
     */
    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG0_MAC("1.1 Ingress Remarking By Table per Source Port Metering"
                     " with RED packets\n");


    /* Set packet QoS values, that we should get after remarking */

    /* configure QoS Profile - for yellow */
    cpssOsMemSet(&yellowCosProfile, 0, sizeof(yellowCosProfile));
    yellowCosProfile.dropPrecedence = CPSS_DP_GREEN_E;
    yellowCosProfile.userPriority   = 2;
    yellowCosProfile.trafficClass   = 7;
    yellowCosProfile.dscp           = 0x32;
    yellowCosProfile.exp            = 0;

    /* configure QoS Profile - for red */
    cpssOsMemSet(&redCosProfile, 0, sizeof(redCosProfile));
    redCosProfile.dropPrecedence = CPSS_DP_RED_E;
    redCosProfile.userPriority   = 7;
    redCosProfile.trafficClass   = 0;
    redCosProfile.dscp           = 0x11;
    redCosProfile.exp            = 0;


    meterTbParams.srTcmParams.cir = 1;
    meterTbParams.srTcmParams.cbs = 1;
    meterTbParams.srTcmParams.ebs = 10304; /* 1 yellow packet */

    prvTgfPolicerQosRemarkingConfigurationSet(policerStage,
            PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E,
            PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_E,
            &meterTbParams, &yellowCosProfile, &redCosProfile,
            0, 0, 0, 0, 0, NULL);

    /* set the conformance level as yellow */
    prvTgfPolicerConformanceLevelForce(1);

    /* send packet */
    prvTgfPolicerTestPacketSend(&prvTgfPacketInfo);

    /* Check Results */
    prvTgfPolicerCheckResults(policerStage, yellowCosProfile.userPriority,
                              &expectedBytes);

    /* print the value of specified Management and Policy Counters */
    prvTgfPolicerCountersPrint(policerStage);

    /* check the value of specified Management Counters */
    /* 1 yellow packet after metering --> 1 green packet after remarking */
    prvTgfPolicerManagementCountersGreenCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersYellowCheck(policerStage, expectedBytes, 0, prvTgfBurstCount);
    prvTgfPolicerManagementCountersRedCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersDropCheck(policerStage, 0, 0, 0);

    prvTgfPolicerCountersReset(policerStage);
}


/*******************************************************************************
* prvTgfPolicerEgressQosRemarkingTrafficGenerate
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
GT_VOID prvTgfPolicerEgressQosRemarkingTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                               rc = GT_OK;
    PRV_TGF_POLICER_METER_TB_PARAMS_UNT     meterTbParams;
    PRV_TGF_POLICER_QOS_PARAM_STC           qosParam;
    GT_U32                                  stage;
    GT_U32                                  expectedBytes;
    PRV_TGF_POLICER_STAGE_TYPE_ENT          policerStage =
                                                PRV_TGF_POLICER_STAGE_EGRESS_E;

    if(GT_FALSE == prvTgfPolicerStageCheck(policerStage))
    {
        /* the device not supports this stage */
        return;
    }

    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG0_MAC("                      POLICER_STAGE_EGRESS\n");


    /* Disable Metering and Counting on all stages */
    for (stage = PRV_TGF_POLICER_STAGE_INGRESS_0_E; stage < PRV_TGF_POLICER_STAGE_NUM; stage++)
    {
        if(GT_FALSE == prvTgfPolicerStageCheck(stage))
        {
            /* the device not supports this stage */
            continue;
        }

        rc = prvTgfPolicerMeteringEnableSet(stage, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingModeSet: %d", prvTgfDevNum);

        rc = prvTgfPolicerCountingModeSet(prvTgfDevNum, stage, PRV_TGF_POLICER_COUNTING_DISABLE_E);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerCountingModeSet: %d", prvTgfDevNum);
    }
    /* -------------------------------------------------------------------------
     * 4.1 Egress Remarking with RED packets L2
     */
    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG0_MAC("4.1 Egress Remarking with YELLOW packets L2\n");
    PRV_UTF_LOG0_MAC("Expected results: RED Counters count the packets\n\n");

    /* Remark YELLOW packet to RED */

    meterTbParams.srTcmParams.cir = 1;
    meterTbParams.srTcmParams.cbs = 1;
    meterTbParams.srTcmParams.ebs = 10304; /* 1 yellow packet */

    /* Configure Qos Remarking entries UP Remarking and CL is YELLOW */
    cpssOsMemSet(&qosParam, 0, sizeof(qosParam));
    qosParam.up   = 1;
    qosParam.dscp = 0x20;
    qosParam.exp  = 0;
    qosParam.dp   = CPSS_DP_RED_E;


    /* set configuration for Egress YELLOW packets */
    prvTgfPolicerQosRemarkingConfigurationSet(policerStage,
                                              PRV_TGF_POLICER_STAGE_METER_MODE_FLOW_E,
                                              PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E,
                                              &meterTbParams,
                                              NULL, NULL,
                                              PRV_TGF_POLICER_REMARK_MODE_L2_E,
                                              PRV_TGF_POLICER_L2_REMARK_MODEL_UP_E,
                                              PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E,
                                              prvTgfPacketVlanTagPart.pri,
                                              CPSS_DP_YELLOW_E,
                                              &qosParam);

    prvTgfPolicerQosRemarkingPclConfigurationSet(CPSS_PCL_DIRECTION_EGRESS_E,
                                                 prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_NUM_CNS]);

    /* Disable Remarking for conforming packets */
    rc = prvTgfPolicerEgressQosUpdateEnableSet(GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerEgressQosUpdateEnableSet %d", prvTgfDevNum);

    /* set the conformance level as yellow */
    prvTgfPolicerConformanceLevelForce(1);

    /* send packets */
    prvTgfPolicerTestPacketSend(&prvTgfPacketInfo);


    /* Check Results */
    prvTgfPolicerCheckResults(policerStage, qosParam.up,
                              &expectedBytes);

    /* print the value of specified Management and Policy Counters */
    prvTgfPolicerCountersPrint(policerStage);

    /* check the value of specified Management Counters */
    prvTgfPolicerManagementCountersGreenCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersYellowCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersRedCheck(policerStage, expectedBytes, 0, prvTgfBurstCount);
    prvTgfPolicerManagementCountersDropCheck(policerStage, 0, 0, 0);



    /* -------------------------------------------------------------------------
     * 4.1 Egress Remarking with RED packets L2
     */
    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG0_MAC("4.1 Egress Remarking with YELLOW packets L2\n");
    PRV_UTF_LOG0_MAC("Expected results: RED Counters count the packets\n\n");

    /* Remark YELLOW packet to RED */

    meterTbParams.srTcmParams.cir = 1;
    meterTbParams.srTcmParams.cbs = 1;
    meterTbParams.srTcmParams.ebs = 10304; /* 1 yellow packet */

    /* Configure Qos Remarking entries UP Remarking and CL is YELLOW */
    cpssOsMemSet(&qosParam, 0, sizeof(qosParam));
    qosParam.up   = 1;
    qosParam.dscp = 0x20;
    qosParam.exp  = 0;
    qosParam.dp   = CPSS_DP_RED_E;


    /* set configuration for Egress YELLOW packets */
    prvTgfPolicerQosRemarkingConfigurationSet(policerStage,
                                              PRV_TGF_POLICER_STAGE_METER_MODE_FLOW_E,
                                              PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E,
                                              &meterTbParams,
                                              NULL, NULL,
                                              PRV_TGF_POLICER_REMARK_MODE_L2_E,
                                              PRV_TGF_POLICER_L2_REMARK_MODEL_UP_E,
                                              PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E,
                                              prvTgfPacketVlanTagPart.pri,
                                              CPSS_DP_YELLOW_E,
                                              &qosParam);

    prvTgfPolicerQosRemarkingPclConfigurationSet(CPSS_PCL_DIRECTION_EGRESS_E,
                                                 prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_NUM_CNS]);

    /* Disable Remarking for conforming packets */
    rc = prvTgfPolicerEgressQosUpdateEnableSet(GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerEgressQosUpdateEnableSet %d", prvTgfDevNum);

    /* set the conformance level as yellow */
    prvTgfPolicerConformanceLevelForce(1);

    /* send packets */
    prvTgfPolicerTestPacketSend(&prvTgfPacketInfo);


    /* Check Results */
    prvTgfPolicerCheckResults(policerStage, qosParam.up,
                              &expectedBytes);

    /* print the value of specified Management and Policy Counters */
    prvTgfPolicerCountersPrint(policerStage);

    /* check the value of specified Management Counters */
    prvTgfPolicerManagementCountersGreenCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersYellowCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersRedCheck(policerStage, expectedBytes, 0, prvTgfBurstCount);
    prvTgfPolicerManagementCountersDropCheck(policerStage, 0, 0, 0);


    /* Remark RED packet to GREEN */
    PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG0_MAC("4.1 Egress Remarking with RED packets L2\n");
    PRV_UTF_LOG0_MAC("Expected results: GREEN Counters count the packets\n\n");

    meterTbParams.srTcmParams.cir = 1;
    meterTbParams.srTcmParams.cbs = 1;
    meterTbParams.srTcmParams.ebs = 1; /* 1 red packet */

    /* Configure Qos Remarking entries for UP Remarking and CL is RED */
    cpssOsMemSet(&qosParam, 0, sizeof(qosParam));
    qosParam.up   = 7;
    qosParam.dscp = 0x20;
    qosParam.exp  = 0;
    qosParam.dp   = CPSS_DP_GREEN_E;

    /* set configuration for Egress YELLOW packets */
    prvTgfPolicerQosRemarkingConfigurationSet(policerStage,
                                              PRV_TGF_POLICER_STAGE_METER_MODE_FLOW_E,
                                              PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E,
                                              &meterTbParams,
                                              NULL, NULL,
                                              PRV_TGF_POLICER_REMARK_MODE_L2_E,
                                              PRV_TGF_POLICER_L2_REMARK_MODEL_UP_E,
                                              PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E,
                                              prvTgfPacketVlanTagPart.pri,
                                              CPSS_DP_RED_E,
                                              &qosParam);

    /* set the conformance level as red */
    prvTgfPolicerConformanceLevelForce(2);

    /* send packets */
    prvTgfPolicerTestPacketSend(&prvTgfPacketInfo);

    /* Check Results */
    prvTgfPolicerCheckResults(policerStage, qosParam.up, &expectedBytes);

    /* print the value of specified Management and Policy Counters */
    prvTgfPolicerCountersPrint(policerStage);

    /* check the value of specified Management Counters */
    prvTgfPolicerManagementCountersGreenCheck(policerStage, expectedBytes, 0, prvTgfBurstCount);
    prvTgfPolicerManagementCountersYellowCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersRedCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersDropCheck(policerStage, 0, 0, 0);


    /* Remark GREEN packet to RED */

	PRV_UTF_LOG0_MAC("================================"
                     "================================\n");
    PRV_UTF_LOG0_MAC("4.1 Egress Remarking with GREEN packets L2\n");
    PRV_UTF_LOG0_MAC("Expected results: RED Counters count the packets\n\n");

    meterTbParams.srTcmParams.cir = 1;
    meterTbParams.srTcmParams.cbs = 10304; /* 1 green packet */
    meterTbParams.srTcmParams.ebs = 1;

    /* Configure Qos Remarking entries for UP and CL is GREEN */
    cpssOsMemSet(&qosParam, 0, sizeof(qosParam));
    qosParam.up   = 2;
    qosParam.dscp = 0x20;
    qosParam.exp  = 0;
    qosParam.dp   = CPSS_DP_RED_E;

    /* set configuration for Egress RED packets */
    prvTgfPolicerQosRemarkingConfigurationSet(policerStage,
                                              PRV_TGF_POLICER_STAGE_METER_MODE_FLOW_E,
                                              PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E,
                                              &meterTbParams,
                                              NULL, NULL,
                                              PRV_TGF_POLICER_REMARK_MODE_L2_E,
                                              PRV_TGF_POLICER_L2_REMARK_MODEL_UP_E,
                                              PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E,
                                              prvTgfPacketVlanTagPart.pri,
                                              CPSS_DP_GREEN_E,
                                              &qosParam);

    /* Enable Remarking for conforming packets */
    rc = prvTgfPolicerEgressQosUpdateEnableSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPolicerEgressQosUpdateEnableSet %d", prvTgfDevNum);

    /* set the conformance level as green */
    prvTgfPolicerConformanceLevelForce(0);

    /* send packets */
    prvTgfPolicerTestPacketSend(&prvTgfPacketInfo);

    /* Check Results */
    prvTgfPolicerCheckResults(policerStage, qosParam.up, &expectedBytes);

    /* print the value of specified Management and Policy Counters */
    prvTgfPolicerCountersPrint(policerStage);

    /* check the value of specified Management Counters */
    prvTgfPolicerManagementCountersGreenCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersYellowCheck(policerStage, 0, 0, 0);
    prvTgfPolicerManagementCountersRedCheck(policerStage, expectedBytes, 0,
                                            prvTgfBurstCount);
    prvTgfPolicerManagementCountersDropCheck(policerStage, 0, 0, 0);


    prvTgfPolicerCountersReset(policerStage);
}

