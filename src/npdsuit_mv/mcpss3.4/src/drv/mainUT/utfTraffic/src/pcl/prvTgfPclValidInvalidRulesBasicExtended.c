/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPclValidInvalidRulesBasicExtended.c
*
* DESCRIPTION:
*       Ingress PCL key STD L2
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/pcl/cpssPcl.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPclGen.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfMirror.h>
#include <pcl/prvTgfPclValidInvalidRulesBasicExtended.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* max iteration's name length */
#define PRV_TGF_MAX_ITERATION_NAME_LEN_CNS 100

/* number of send traffic iteration */
#define PRV_TGF_MAX_ITER_CNS              6

/* default VLAN Id */
#define PRV_TGF_VLANID_CNS                5

/* number of ports */
#define PRV_TGF_PORT_COUNT_CNS            PRV_TGF_PORTS_NUM_CNS

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS         0

/* port number to forward traffic to */
#define PRV_TGF_FDB_PORT_IDX_CNS          2

/* analyzer port number */
#define PRV_TGF_ANALYZER_PORT_IDX_CNS     3

/* default number of packets to send */
static GT_U32        prvTgfBurstCount =   1;

/******************************* IPv4 packet 1 **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacket1L2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x55}                 /* saMac */
};
/* VLAN_TAG part of packet1 */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacket1VlanTagPart = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                     /* etherType */
    0, 0, PRV_TGF_VLANID_CNS                             /* pri, cfi, VlanId */
};
/* ethertype part of packet1 */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacket1EtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};
/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacket1Ipv4Part = {
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2A,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    0xFF,               /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    { 1,  1,  0,  2},   /* srcAddr */
    { 3,  3,  0,  1}    /* dstAddr */
};
/* DATA of packet */
static GT_U8 prvTgfPacket1PayloadDataArr[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacket1PayloadPart = {
    sizeof(prvTgfPacket1PayloadDataArr),                 /* dataLength */
    prvTgfPacket1PayloadDataArr                          /* dataPtr */
};
/* PARTS of packet IPv4 */
static TGF_PACKET_PART_STC prvTgfPacket1PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part}, /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacket1VlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacket1EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacket1Ipv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacket1PayloadPart}
};

/******************************* IPv4 packet 2 **********************************/

/* L2 part of packet2 */
static TGF_PACKET_L2_STC prvTgfPacket2L2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}                 /* saMac */
};
/* ethertype part of packet2 */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacket2EtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};
/* IPv4 part of packet2 */
static TGF_PACKET_IPV4_STC prvTgfPacket2Ipv4Part = {
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2E,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    0x04,               /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};
/* DATA of packet2 */
static GT_U8 prvTgfPacket2PayloadDataArr[] = {
    0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
/* PAYLOAD part of packet2 */
static TGF_PACKET_PAYLOAD_STC prvTgfPacket2PayloadPart = {
    sizeof(prvTgfPacket2PayloadDataArr),                 /* dataLength */
    prvTgfPacket2PayloadDataArr                          /* dataPtr */
};
/* PARTS of packet2 */
static TGF_PACKET_PART_STC prvTgfPacket2PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket2L2Part}, /* type, partPtr */
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacket2EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacket2Ipv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacket2PayloadPart}
};

/******************************* IPv6 packet **********************************/

/* packet's ethertype for IPv6 */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketIpv6EtherTypePart = {TGF_ETHERTYPE_86DD_IPV6_TAG_CNS};
/* packet's IPv6 */
static TGF_PACKET_IPV6_STC prvTgfPacketIpv6Part = {
    6,                                       /* version */
    0,                                       /* trafficClass */
    0,                                       /* flowLabel */
    0x02,                                    /* payloadLen */
    0x3b,                                    /* nextHeader */
    0x40,                                    /* hopLimit */
    {0x1111, 0, 0, 0, 0, 0, 0x0000, 0x1111}, /* TGF_IPV6_ADDR srcAddr */
    {0x1111, 0, 0, 0, 0, 0, 0x0000, 0x3322}  /* TGF_IPV6_ADDR dstAddr */
};
/* DATA of packet */
static GT_U8 prvTgfPacketIpv6PayloadDataArr[] = {
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00
};
/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketIpv6PayloadPart = {
    sizeof(prvTgfPacketIpv6PayloadDataArr),                 /* dataLength */
    prvTgfPacketIpv6PayloadDataArr                          /* dataPtr */
};
/* PARTS of packet IPv6 */
static TGF_PACKET_PART_STC prvTgfPacketIpv6PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part},   /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacket1VlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv6EtherTypePart},
    {TGF_PACKET_PART_IPV6_E,      &prvTgfPacketIpv6Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketIpv6PayloadPart}
};

/************** packets and ations for the each iteration ****************/

/* The names of each iteration */
static GT_U8 prvTgfIterationNameArray[][PRV_TGF_MAX_ITERATION_NAME_LEN_CNS] = {
    "Check PCL rule 0: Ipv4 TRAP to CPU",
    "Check PCL rule 1: Ipv6 FORWARD and MIRROR to RxAnalyzerPort (23)",
    "Invalidate PCL rule 0: Ipv4 TRAP to CPU",
    "Invalidate PCL rule 1: Ipv6 FORWARD and MIRROR to RxAnalyzerPort (23)",
    "Validate PCL rule 0: Ipv4 TRAP to CPU",
    "Validate PCL rule 1: Ipv6 FORWARD and MIRROR to RxAnalyzerPort (23)"
};

/* Packets for the each iteration */
static TGF_PACKET_PART_STC *prvTgfPacketsArray[] = {
    prvTgfPacket1PartArray,
    /*prvTgfPacket2PartArray,*/
    prvTgfPacketIpv6PartArray,
    prvTgfPacket2PartArray,
    prvTgfPacketIpv6PartArray,
    prvTgfPacket2PartArray,
    prvTgfPacketIpv6PartArray
};

/* PartsCount of Packets for the each iteration */
static GT_U32 prvTgfPartsCountArray[] = {
    sizeof(prvTgfPacket1PartArray) / sizeof(prvTgfPacket1PartArray[0]),
    /*sizeof(prvTgfPacket2PartArray) / sizeof(prvTgfPacket2PartArray[0]),*/
    sizeof(prvTgfPacketIpv6PartArray) / sizeof(prvTgfPacketIpv6PartArray[0]),
    sizeof(prvTgfPacket2PartArray) / sizeof(prvTgfPacket2PartArray[0]),
    sizeof(prvTgfPacketIpv6PartArray) / sizeof(prvTgfPacketIpv6PartArray[0]),
    sizeof(prvTgfPacket2PartArray) / sizeof(prvTgfPacket2PartArray[0]),
    sizeof(prvTgfPacketIpv6PartArray) / sizeof(prvTgfPacketIpv6PartArray[0])
};

/* Invalidating PCL Rules before the iteration */
static GT_BOOL prvTgfActionPclRulesInvalidate[] = {
    GT_FALSE, GT_FALSE, GT_TRUE, GT_FALSE, GT_FALSE, GT_FALSE
};

/* Validating PCL Rules before the iteration */
static GT_BOOL prvTgfActionPclRulesValidate[] = {
    GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE, GT_FALSE
};

/* Enable to reset counters before the iteration */
static GT_BOOL prvTgfActionContersReset[] = {
    GT_TRUE, GT_TRUE, GT_TRUE, GT_FALSE, GT_TRUE, GT_FALSE
};

/* Enable to accumulate counters */
static GT_BOOL prvTgfActionContersAccumulate[] = {
    GT_FALSE, GT_FALSE, GT_TRUE, GT_FALSE, GT_TRUE, GT_FALSE
};

/* Checking trapped packets to CPU */
static GT_BOOL prvTgfActionTrapToCpuCheck[] = {
    GT_TRUE, GT_FALSE, GT_FALSE, GT_TRUE, GT_FALSE, GT_TRUE
};

/* Checking mirrored packets from Analyzer port */
static GT_BOOL prvTgfActionMirrorToRxAnalyzerPortCheck[] = {
    GT_FALSE, GT_TRUE, GT_FALSE, GT_TRUE, GT_FALSE, GT_TRUE
};

/* define expected number of packets for each port on the each iteration */
static GT_U8 prvTgfPacketsCountArray[][PRV_TGF_PORT_COUNT_CNS] = {
/* ports: SEND FREE FDB ANALYZE */
            {1,  0,  0,  0},
            {1,  0,  1,  1},
            {0,  0,  0,  0},
            {2,  0,  2,  0},
            {0,  0,  0,  0},
            {2,  0,  1,  1}
};

/******************************************************************************/


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/* stored default Vlan ID */
static GT_U16   prvTgfDefVlanId = 0;

/*******************************************************************************
* prvTgfPclValidInvalidRulesBasicExtendedConfigurationSet
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
GT_VOID prvTgfPclValidInvalidRulesBasicExtendedConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;

    /* create a vlan on ports (0,0), (0,8), (0,18), (0,23) */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d", prvTgfDevNum);

    /* create a macEntry with PORT interface */
    rc = prvTgfBrgDefFdbMacEntryOnPortSet(prvTgfPacket1L2Part.daMac, PRV_TGF_VLANID_CNS,
                                          prvTgfDevNum, prvTgfPortsArray[PRV_TGF_FDB_PORT_IDX_CNS], GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);

    /* get default vlanId */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* set default vlanId */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfPclValidInvalidRulesBasicExtendedConfigurationSet
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
GT_VOID prvTgfPclValidInvalidRulesBasicExtendedPclConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                        rc        = GT_OK;
    GT_U32                           ruleIndex = 0;
    PRV_TGF_PCL_RULE_FORMAT_UNT      mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT      pattern;
    PRV_TGF_PCL_ACTION_STC           action;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ruleFormat = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInit(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                             CPSS_PCL_DIRECTION_INGRESS_E,
                             CPSS_PCL_LOOKUP_1_E,
                             PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                             PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                             PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPclDefPortInit: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* set PCL rule 0 - CMD_TRAP_TO_CPU any Ipv4 */
    cpssOsMemSet(&mask, 0, sizeof(mask));
    cpssOsMemSet(&pattern, 0, sizeof(pattern));
    cpssOsMemSet(&action, 0, sizeof(action));

    ruleIndex                               = 0;
    ruleFormat                              = PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
    mask.ruleExtNotIpv6.common.isIp         = GT_TRUE;
    mask.ruleExtNotIpv6.commonExt.isIpv6    = GT_TRUE;
    pattern.ruleExtNotIpv6.common.isIp      = GT_TRUE;
    pattern.ruleExtNotIpv6.commonExt.isIpv6 = GT_FALSE;
    action.pktCmd                           = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    action.mirror.cpuCode                   = CPSS_NET_CLASS_KEY_MIRROR_E;

    rc = prvTgfPclRuleSet(ruleFormat, ruleIndex, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfPclRuleSet: %d, %d, %d", prvTgfDevNum, ruleFormat, ruleIndex);

    /* set PCL rule 1 - CMD_FORWARD and MirrorToRxAnalyzerPort any Ipv6 */
    cpssOsMemSet(&mask, 0, sizeof(mask));
    cpssOsMemSet(&pattern, 0, sizeof(pattern));
    cpssOsMemSet(&action, 0, sizeof(action));

    ruleIndex                               = 1;
    ruleFormat                              = PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
    mask.ruleExtNotIpv6.common.isIp         = GT_TRUE;
    mask.ruleExtNotIpv6.commonExt.isIpv6    = GT_TRUE;
    pattern.ruleExtNotIpv6.common.isIp      = GT_TRUE;
    pattern.ruleExtNotIpv6.commonExt.isIpv6 = GT_TRUE;
    action.pktCmd                           = CPSS_PACKET_CMD_FORWARD_E;
    action.mirror.mirrorToRxAnalyzerPort    = GT_TRUE;

    rc = prvTgfPclRuleSet(ruleFormat, ruleIndex, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfPclRuleSet: %d, %d, %d", prvTgfDevNum, ruleFormat, ruleIndex);
}

/*******************************************************************************
* prvTgfPclValidInvalidRulesBasicExtendedTrafficGenerate
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
GT_VOID prvTgfPclValidInvalidRulesBasicExtendedTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                       rc          = GT_OK;
    GT_U32                          sendIter    = 0;
    GT_U32                          partsCount  = 0;
    GT_U32                          packetSize  = 0;
    TGF_PACKET_STC                  packetInfo;
    GT_U32                          portIter    = 0;
    GT_U32                          numTriggers = 0;
    CPSS_INTERFACE_INFO_STC         portInterface;
    TGF_VFD_INFO_STC                vfdArray[1];
    GT_U32                          packetLength;

    /* -------------------------------------------------------------------------
     * 0. Setup Capturing
     */

    /* Enable Analyzer port adding additional VLAN Tag to mirrored packets. */
    rc = prvTgfMirrorRxAnalyzerPortSet(prvTgfPortsArray[PRV_TGF_ANALYZER_PORT_IDX_CNS], prvTgfDevNum);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxAnalyzerPortSet: %d %d %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_ANALYZER_PORT_IDX_CNS], prvTgfDevNum);

    /* enable capture on RxAnalyzerPort */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_ANALYZER_PORT_IDX_CNS];

    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_PCL_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d\n",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_ANALYZER_PORT_IDX_CNS]);

    /* set capturing mode */
    rc = tgfTrafficTableRxModeSet(TGF_RX_CYCLIC_E, 600);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxModeSet: %d", prvTgfDevNum);

    /* set Cpu Device for TRAP to CPU action */
    rc = tgfTrafficGeneratorCpuDeviceSet(prvTgfDevNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorCpuDeviceSet: %d", prvTgfDevNum);

    /* make iteration */
    for (sendIter = 0; sendIter < PRV_TGF_MAX_ITER_CNS; sendIter++)
    {
        PRV_UTF_LOG1_MAC("======= Generating Traffic %d =======\n", sendIter);
        PRV_UTF_LOG1_MAC("%s\n", prvTgfIterationNameArray[sendIter]);

        /* -------------------------------------------------------------------------
         * 1. Invalidating or Validating all PCL Rules
         */

        /* Invalidating PCL Rules */
        if (GT_TRUE == prvTgfActionPclRulesInvalidate[sendIter])
        {
            PRV_UTF_LOG0_MAC("------- Invalidating PCL Rules -------\n");

            rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_EXT_E, 0, GT_FALSE);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleValidStatusSet: %d", prvTgfDevNum);

            rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_EXT_E, 1, GT_FALSE);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleValidStatusSet: %d", prvTgfDevNum);
        }

        /* Validating PCL Rules */
        if (GT_TRUE == prvTgfActionPclRulesValidate[sendIter])
        {
            PRV_UTF_LOG0_MAC("------- Validating PCL Rules -------\n");

            rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_EXT_E, 0, GT_TRUE);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleValidStatusSet: %d", prvTgfDevNum);

            rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_EXT_E, 1, GT_TRUE);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleValidStatusSet: %d", prvTgfDevNum);
        }

        /* -------------------------------------------------------------------------
         * 2. Reseting all counters and clearing the capturing table
         */


        if (GT_TRUE == prvTgfActionContersReset[sendIter])
        {
            PRV_UTF_LOG0_MAC("------- Reseting All Counters -------\n");

            for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
            {
                /* reset ethernet counters */
                rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                             prvTgfDevNum, prvTgfPortsArray[portIter]);
            }

            /* Clear capturing RxPcktTable */
            rc = tgfTrafficTableRxPcktTblClear();
            UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");
        }

        /* -------------------------------------------------------------------------
         * 3. Generating Traffic
         */

        /* number of parts in packet */
        partsCount = prvTgfPartsCountArray[sendIter];

        /* calculate packet size */
        rc = prvTgfPacketSizeGet(prvTgfPacketsArray[sendIter], partsCount, &packetSize);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPacketSizeGet:");

        /* build packet */
        packetInfo.totalLen   = packetSize;
        packetInfo.numOfParts = partsCount;
        packetInfo.partsArray = prvTgfPacketsArray[sendIter];

        /* setup packet */
        rc = prvTgfSetTxSetupEth(prvTgfDevNum, &packetInfo, prvTgfBurstCount, 0, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d", prvTgfDevNum);

        /* send packet */
        rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

        /* accumulate counters for some iterations and generate next packet */
        if (GT_TRUE == prvTgfActionContersAccumulate[sendIter]) continue;

        /* -------------------------------------------------------------------------
         * 4. Checking Ethernet Counters
         */

        PRV_UTF_LOG0_MAC("------- Checking Ethernet Counters -------\n");

        /* read and check ethernet counters */
        for (portIter = 0; portIter < PRV_TGF_PORT_COUNT_CNS; portIter++)
        {
            /* skip analizer port check for xCat A1 device because of errata */
            if ((portIter == PRV_TGF_ANALYZER_PORT_IDX_CNS) &&
                ((PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) &&
                 (PRV_CPSS_PP_MAC(prvTgfDevNum)->revision == 2)))
            {
                continue;
            }

            /* test sends tagged packet. But FDB target port is untagged. 
             packet size on FDB port will be 4 bytes less. 
             decrease length to 2 bytes when 2 packets are sent. 
             because one packet was sent to devices untagged. */
            packetLength = (portIter != PRV_TGF_FDB_PORT_IDX_CNS) ? 
                             packetInfo.totalLen : 
                             ((prvTgfPacketsCountArray[sendIter][portIter] == 1) ? (packetInfo.totalLen - 4) : (packetInfo.totalLen - 2));
            /* read and check ethernet counters */
            rc = prvTgfEthCountersCheck(prvTgfDevNum, prvTgfPortsArray[portIter],
                (GT_U8)(prvTgfPacketsCountArray[sendIter][portIter]
                * (portIter != PRV_TGF_FDB_PORT_IDX_CNS)),
                prvTgfPacketsCountArray[sendIter][portIter],
                packetLength, 1);
            UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "get another counters values.");
        }

        /* -------------------------------------------------------------------------
         * 5. Checking trapped packets to CPU
         */

        if (GT_TRUE == prvTgfActionTrapToCpuCheck[sendIter])
        {
            GT_U8                 packetBufPtr[80] = {0};
            GT_U32                packetBufLen = 68;
            GT_U32                packetLen;
            GT_U8                 devNum;
            GT_U8                 queueCpu;
            TGF_NET_DSA_STC       rxParams;

            PRV_UTF_LOG0_MAC("CPU port capturing:\n");

            rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_REGULAR_E,
                                               GT_TRUE, GT_TRUE,
                                               packetBufPtr,
                                               &packetBufLen,
                                               &packetLen,
                                               &devNum,
                                               &queueCpu,
                                               &rxParams);
        }

        /* -------------------------------------------------------------------------
         * 6. Checking mirrored packets from Analyzer port
         */

        if ((GT_TRUE == prvTgfActionMirrorToRxAnalyzerPortCheck[sendIter]) &&
            (!((PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) &&
               (PRV_CPSS_PP_MAC(prvTgfDevNum)->revision == 2))))
        {
            PRV_UTF_LOG0_MAC("RxAnalyzer port capturing and triggers:\n");

            /* get and check trigger counters */
            vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
            vfdArray[0].modeExtraInfo = 0;
            vfdArray[0].offset = 0;
            vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
            cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfPacket1L2Part.daMac, sizeof(TGF_MAC_ADDR));

            rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 1, vfdArray, &numTriggers);

            PRV_UTF_LOG2_MAC("numTriggers = %d, rc = 0x%02X\n\n", numTriggers, rc);

            rc = rc == GT_NO_MORE ? GT_OK : rc;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthTriggerCountersGet: %d, %d\n",
                                         portInterface.devPort.devNum, portInterface.devPort.portNum);
        }
    }

    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_PCL_E, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d\n",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_ANALYZER_PORT_IDX_CNS]);

    /* Clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");
}

/*******************************************************************************
* prvTgfPclValidInvalidRulesBasicExtendedConfigurationRestore
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
*       2. Restore PCL Configuration
*       1. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfPclValidInvalidRulesBasicExtendedConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;

    /* -------------------------------------------------------------------------
     * 2. Restore PCL Configuration
     */

    /* disable ingress policy on ports */
    rc = prvTgfPclPortIngressPolicyEnable(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclPortIngressPolicyEnable: %d", prvTgfDevNum);

    /* Invalidating PCL Rules */
    rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_EXT_E, 0, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleValidStatusSet: %d", prvTgfDevNum);

    rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_EXT_E, 1, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclRuleValidStatusSet: %d", prvTgfDevNum);

    /* -------------------------------------------------------------------------
     * 1. Restore Base Configuration
     */

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* restore default vlanId to all ports */
    rc = prvTgfBrgVlanPvidSet(prvTgfDefVlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPvidSet: %d, %d",
                                 prvTgfDevNum, prvTgfDefVlanId);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);

}

