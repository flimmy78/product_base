/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPclBothUserDefinedBytes.c
*
* DESCRIPTION:
*       Second Lookup match
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

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPclGen.h>
#include <common/tgfBridgeGen.h>
#include <pcl/prvTgfPclBothUserDefinedBytes.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* max iteration's name length */
#define PRV_TGF_MAX_ITERATION_NAME_LEN_CNS 100

/* number of send traffic iteration */
#define PRV_TGF_MAX_ITER_CNS              1

/* default VLAN Id */
#define PRV_TGF_VLANID_CNS                5

/* number of ports */
#define PRV_TGF_PORT_COUNT_CNS            PRV_TGF_PORTS_NUM_CNS

/* port to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS         0

/* port to send traffic to */
#define PRV_TGF_FDB_PORT_IDX_CNS          1

/* default number of packets to send */
static GT_U32        prvTgfBurstCount =   1;

/* define User Define Byte as end of MAC SA */
static GT_U8         prvTgfUdbArray[] = {0x00, 0x11};

/* UDB to check IPV4 packet: OFFSET_L2 0-8 bytes – Lion-B0 simulation */
static GT_U8         prvTgfUdb_L2_0_8_Array[] = {
    0x00, 0x00, 0x00, 0x00, 0x34, 0x02,
    0x00, 0x00, 0x00
};

/* UDB to check IPV4 packet: OFFSET_L4 0-15 bytes – Lion-B0 simulation */
static GT_U8         prvTgfUdb_L4_0_15_Array[] = {
    0xfa, 0xb5, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

/* UDB to check ARP packet: OFFSET_L2 0-11 bytes – Lion-B0 simulation */
static GT_U8         prvTgfUdb_L2_0_11_Array[] = {
    0x00, 0x00, 0x00, 0x00, 0x34, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x11
};

/******************************* IPv4 packet 1 **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacket1L2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}                 /* saMac */
};
/* ethertype part of packet1 */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacket1EtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};
/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacket1Ipv4Part = {
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2E,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    4,                  /* protocol */
    0x4C9D,             /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};
/* DATA of packet */
static GT_U8 prvTgfPacket1PayloadDataArr[] = {
    0xfa, 0xb5, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19
};
/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacket1PayloadPart = {
    sizeof(prvTgfPacket1PayloadDataArr),                 /* dataLength */
    prvTgfPacket1PayloadDataArr                          /* dataPtr */
};
/* PARTS of packet IPv4 */
static TGF_PACKET_PART_STC prvTgfPacket1PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part}, /* type, partPtr */
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacket1EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacket1Ipv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacket1PayloadPart}
};

/* LENGTH of packet 1 */
#define PRV_TGF_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + TGF_IPV4_HEADER_SIZE_CNS + sizeof(prvTgfPacket1PayloadDataArr)

/* PACKET1 to send */
static TGF_PACKET_STC prvTgfPacket1Info = {
    PRV_TGF_PACKET_LEN_CNS,                                       /* totalLen */
    sizeof(prvTgfPacket1PartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacket1PartArray                                        /* partsArray */
};

/****************************** ARP packet ************************************/

/* ethertype part of ARP packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketArpEtherTypePart = {
    TGF_ETHERTYPE_0806_ARP_TAG_CNS
};
/* packet's ARP header */
static TGF_PACKET_ARP_STC prvTgfPacketArpPart = {
    0x01,                                   /* HW Type */
    0x0800,                                 /* Protocol (IPv4= 0x0800) */
    0x06,                                   /* HW Len = 6 */
    0x04,                                   /* Proto Len = 4 */
    0x01,                                   /* Opcode */
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05},   /* nbytes: HW Address of Sender (MAC SA) */
    {0x11, 0x12, 0x13, 0x14},               /* mbytes: Protocol Address of Sender (SIP) */
    {0x20, 0x21, 0x22, 0x23, 0x24, 0x25},   /* nbytes: HW Address of Target (MAC DA) */
    {0x30, 0x31, 0x32, 0x33}                /* mbytes: Protocol Address of Target (DIP) */
};
/* PARTS of packet ARP */
static TGF_PACKET_PART_STC prvTgfPacketArpPartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part}, /* type, partPtr */
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketArpEtherTypePart},
    {TGF_PACKET_PART_ARP_E,       &prvTgfPacketArpPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacket1PayloadPart}
};

/* LENGTH of packet ARP */
#define PRV_TGF_PACKET_ARP_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + TGF_ARP_HEADER_SIZE_CNS + sizeof(prvTgfPacket1PayloadDataArr)

/* PACKET1 to send */
static TGF_PACKET_STC prvTgfPacketArpInfo = {
    PRV_TGF_PACKET_ARP_LEN_CNS,                                     /* totalLen */
    sizeof(prvTgfPacketArpPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketArpPartArray                                        /* partsArray */
};

/******************************************************************************/

/* Current packet to send */
static TGF_PACKET_STC *prvTgfPacketSendPtr = &prvTgfPacket1Info;


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/* stored default Vlan ID */
static GT_U16   prvTgfDefVlanId = 0;

/*******************************************************************************
* prvTgfPclBothUserDefinedBytesConfigurationSet
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
GT_VOID prvTgfPclBothUserDefinedBytesConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;

    /* create a vlan on ports (0,0), (0,8), (0,18), (0,23) */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d", prvTgfDevNum);

    /* create a macEntry for packet */
    rc = prvTgfBrgDefFdbMacEntryOnPortSet(prvTgfPacket1L2Part.daMac, PRV_TGF_VLANID_CNS,
                                          prvTgfDevNum,
                                          prvTgfPortsArray[PRV_TGF_FDB_PORT_IDX_CNS], GT_TRUE);
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
* prvTgfPclBothUserDefinedBytesConfigurationSet
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
GT_VOID prvTgfPclBothUserDefinedBytesPclConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                        rc = GT_OK;
    GT_U32                           ruleIndex;
    PRV_TGF_PCL_RULE_FORMAT_UNT      mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT      pattern;
    PRV_TGF_PCL_ACTION_STC           action;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    GT_U32                           udbIndex;
    GT_U8                            udbOffset;

    PRV_UTF_LOG0_MAC("==== TEST of TWO UDBs ====\n");

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInit(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                             CPSS_PCL_DIRECTION_INGRESS_E,
                             CPSS_PCL_LOOKUP_1_E,
                             PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                             PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                             PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPclDefPortInit: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* configure the User Defined Byte (UDB) 0 */
    udbOffset = 10;
    rc = prvTgfPclUdbIndexConvert(
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E, 0, &udbIndex);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPclUdbIndexConvert");


    rc = prvTgfPclUserDefinedByteSet(PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                                     PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E,
                                     udbIndex,
                                     PRV_TGF_PCL_OFFSET_L2_E,
                                     udbOffset);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclUserDefinedByteSet: %d", prvTgfDevNum);

    /* configure the User Defined Byte (UDB) 1 */
    udbOffset = 11;
    rc = prvTgfPclUdbIndexConvert(
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E, 1, &udbIndex);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPclUdbIndexConvert");

    rc = prvTgfPclUserDefinedByteSet(PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                             PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E,
                             udbIndex,
                             PRV_TGF_PCL_OFFSET_L2_E,
                             udbOffset);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclUserDefinedByteSet: %d", prvTgfDevNum);

    /* set udbErrorCmd */
    rc = prvTgfPclInvalidUdbCmdSet(PRV_TGF_UDB_ERROR_CMD_LOOKUP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclInvalidUdbCmdSet: %d", prvTgfDevNum);

    /* set PCL rule 0 - DROP packet where MAC SA ends on 00 11 */
    cpssOsMemSet(&mask,    0, sizeof(mask));
    cpssOsMemSet(&pattern, 0, sizeof(pattern));
    cpssOsMemSet(&action,  0, sizeof(action));
    cpssOsMemSet(&mask.ruleStdIpL2Qos.udb, 0xFF, sizeof(prvTgfUdbArray));
    cpssOsMemCpy(&pattern.ruleStdIpL2Qos.udb, prvTgfUdbArray, sizeof(prvTgfUdbArray));

    ruleIndex     = 0;
    ruleFormat    = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    action.pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;

    rc = prvTgfPclRuleSet(ruleFormat, ruleIndex, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfPclRuleSet: %d, %d, %d", prvTgfDevNum, ruleFormat, ruleIndex);
}

/*******************************************************************************
* prvTgfPclBothUserDefinedBytesTrafficGenerate
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
GT_VOID prvTgfPclBothUserDefinedBytesTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;

    /* reset ETH counters */
    rc = prvTgfEthCountersReset(prvTgfDevNum);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfEthCountersReset");

    /* setup packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, prvTgfPacketSendPtr, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d", prvTgfDevNum);

    /* send packet */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d\n",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* check ETH counters for FDB port */
    rc = prvTgfEthCountersCheck(prvTgfDevNum,
            prvTgfPortsArray[PRV_TGF_FDB_PORT_IDX_CNS], 0, 0,
            prvTgfPacketSendPtr->totalLen, prvTgfBurstCount);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfEthCountersCheck");
}

/*******************************************************************************
* prvTgfPclBothUserDefinedBytesConfigurationRestore
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
GT_VOID prvTgfPclBothUserDefinedBytesConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;

    /* restore current packet by default */
    prvTgfPacketSendPtr = &prvTgfPacket1Info;

    /* -------------------------------------------------------------------------
     * 2. Restore PCL Configuration
     */

    /* disable ingress policy on ports */
    rc = prvTgfPclPortIngressPolicyEnable(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclPortIngressPolicyEnable: %d", prvTgfDevNum);

    /* Invalidating PCL Rules */
    rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_STD_E, 0, GT_FALSE);
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

/*******************************************************************************
* prvTgfPclUserDefinedBytes_L2_0_8_ConfigurationSet
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
GT_VOID prvTgfPclUserDefinedBytes_L2_0_8_ConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                        rc = GT_OK;
    GT_U32                           ruleIndex;
    PRV_TGF_PCL_RULE_FORMAT_UNT      mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT      pattern;
    PRV_TGF_PCL_ACTION_STC           action;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    PRV_TGF_PCL_PACKET_TYPE_ENT      packetType;
    PRV_TGF_PCL_OFFSET_TYPE_ENT      offsetType;
    GT_U32                           udbIndex;
    GT_U8                            udbOffset;
    GT_U32                           udbOffsetMax = sizeof(prvTgfUdb_L2_0_8_Array);

    PRV_UTF_LOG0_MAC("==== TEST of IPV4 packet OFFSET_L2 0-8 bytes ====\n");

    /* set default values */
    ruleFormat = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E;
    packetType = PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E;
    offsetType = PRV_TGF_PCL_OFFSET_L2_E;

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInit(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                             CPSS_PCL_DIRECTION_INGRESS_E,
                             CPSS_PCL_LOOKUP_0_E,
                             ruleFormat,
                             ruleFormat,
                             ruleFormat);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPclDefPortInit: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* configure the User Defined Bytes */
    for (udbOffset = 0; udbOffset < udbOffsetMax; udbOffset++) {
        rc = prvTgfPclUdbIndexConvert(ruleFormat, udbOffset, &udbIndex);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPclUdbIndexConvert");

        rc = prvTgfPclUserDefinedByteSet(ruleFormat, packetType, udbIndex, offsetType, udbOffset);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclUserDefinedByteSet: %d", prvTgfDevNum);
    }

    /* set udbErrorCmd */
    rc = prvTgfPclInvalidUdbCmdSet(PRV_TGF_UDB_ERROR_CMD_LOOKUP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclInvalidUdbCmdSet: %d", prvTgfDevNum);

    /* set PCL rule 0 - DROP packet */
    cpssOsMemSet(&mask,    0, sizeof(mask));
    cpssOsMemSet(&pattern, 0, sizeof(pattern));
    cpssOsMemSet(&action,  0, sizeof(action));

    cpssOsMemSet(&mask.ruleStdUdb.udb, 0xFF, sizeof(prvTgfUdb_L2_0_8_Array));
    cpssOsMemCpy(&pattern.ruleStdUdb.udb, prvTgfUdb_L2_0_8_Array, sizeof(prvTgfUdb_L2_0_8_Array));
    action.pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;
    ruleIndex     = 0;

    rc = prvTgfPclRuleSet(ruleFormat, ruleIndex, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfPclRuleSet: %d, %d, %d",
                                 prvTgfDevNum, ruleFormat, ruleIndex);
}

/*******************************************************************************
* prvTgfPclUserDefinedBytes_L4_0_15_ConfigurationSet
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
GT_VOID prvTgfPclUserDefinedBytes_L4_0_15_ConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                        rc = GT_OK;
    GT_U32                           ruleIndex;
    PRV_TGF_PCL_RULE_FORMAT_UNT      mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT      pattern;
    PRV_TGF_PCL_ACTION_STC           action;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    PRV_TGF_PCL_PACKET_TYPE_ENT      packetType;
    PRV_TGF_PCL_OFFSET_TYPE_ENT      offsetType;
    GT_U32                           udbIndex;
    GT_U8                            udbOffset;
    GT_U32                           udbOffsetMax = sizeof(prvTgfUdb_L4_0_15_Array);
    PRV_TGF_PCL_OVERRIDE_UDB_STC     udbOverride;


    PRV_UTF_LOG0_MAC("==== TEST of IPV4 packet OFFSET_L4 0-15 bytes ====\n");

    /* set default values */
    ruleFormat = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E;
    packetType = PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E;
    offsetType = PRV_TGF_PCL_OFFSET_L4_E;

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInit(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                             CPSS_PCL_DIRECTION_INGRESS_E,
                             CPSS_PCL_LOOKUP_0_E,
                             ruleFormat,
                             ruleFormat,
                             ruleFormat);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPclDefPortInit: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* FIX for lion_B0: smem 0b800014 MUST be 00000000 */
    cpssOsMemSet(&udbOverride,  0, sizeof(udbOverride));
    prvTgfPclOverrideUserDefinedBytesSet(&udbOverride);

    /* configure the User Defined Bytes */
    for (udbOffset = 0; udbOffset < udbOffsetMax; udbOffset++) {
        rc = prvTgfPclUdbIndexConvert(ruleFormat, udbOffset, &udbIndex);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPclUdbIndexConvert");

        rc = prvTgfPclUserDefinedByteSet(ruleFormat, packetType, udbIndex, offsetType, udbOffset);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclUserDefinedByteSet: %d", prvTgfDevNum);
    }

    /* set udbErrorCmd */
    rc = prvTgfPclInvalidUdbCmdSet(PRV_TGF_UDB_ERROR_CMD_LOOKUP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclInvalidUdbCmdSet: %d", prvTgfDevNum);

    /* set PCL rule 0 - DROP packet */
    cpssOsMemSet(&mask,    0, sizeof(mask));
    cpssOsMemSet(&pattern, 0, sizeof(pattern));
    cpssOsMemSet(&action,  0, sizeof(action));

    cpssOsMemSet(&mask.ruleStdUdb.udb, 0xFF, sizeof(prvTgfUdb_L4_0_15_Array));
    cpssOsMemCpy(&pattern.ruleStdUdb.udb, prvTgfUdb_L4_0_15_Array, sizeof(prvTgfUdb_L4_0_15_Array));
    action.pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;
    ruleIndex     = 0;

    rc = prvTgfPclRuleSet(ruleFormat, ruleIndex, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfPclRuleSet: %d, %d, %d",
                                 prvTgfDevNum, ruleFormat, ruleIndex);
}

/*******************************************************************************
* prvTgfPclUserDefinedBytes_L2_0_11_ConfigurationSet
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
GT_VOID prvTgfPclUserDefinedBytes_L2_0_11_ConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                        rc = GT_OK;
    GT_U32                           ruleIndex;
    PRV_TGF_PCL_RULE_FORMAT_UNT      mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT      pattern;
    PRV_TGF_PCL_ACTION_STC           action;
    PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    PRV_TGF_PCL_PACKET_TYPE_ENT      packetType;
    PRV_TGF_PCL_OFFSET_TYPE_ENT      offsetType;
    GT_U32                           udbIndex;
    GT_U8                            udbOffset;
    GT_U32                           udbOffsetMax = sizeof(prvTgfUdb_L2_0_11_Array);

    PRV_UTF_LOG0_MAC("==== TEST of ARP packet OFFSET_L2 0-11 bytes ====\n");

    /* set ARP packet */
    prvTgfPacketSendPtr = &prvTgfPacketArpInfo;

    /* set default values */
    ruleFormat = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E;
    packetType = PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
    offsetType = PRV_TGF_PCL_OFFSET_L2_E;

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInit(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                             CPSS_PCL_DIRECTION_INGRESS_E,
                             CPSS_PCL_LOOKUP_0_E,
                             ruleFormat,
                             ruleFormat,
                             ruleFormat);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPclDefPortInit: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* configure the User Defined Bytes */
    for (udbOffset = 0; udbOffset < udbOffsetMax; udbOffset++) {
        rc = prvTgfPclUdbIndexConvert(ruleFormat, udbOffset, &udbIndex);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPclUdbIndexConvert");

        rc = prvTgfPclUserDefinedByteSet(ruleFormat, packetType, udbIndex, offsetType, udbOffset);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclUserDefinedByteSet: %d", prvTgfDevNum);
    }

    /* set udbErrorCmd */
    rc = prvTgfPclInvalidUdbCmdSet(PRV_TGF_UDB_ERROR_CMD_LOOKUP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclInvalidUdbCmdSet: %d", prvTgfDevNum);

    /* set PCL rule 0 - DROP packet */
    cpssOsMemSet(&mask,    0, sizeof(mask));
    cpssOsMemSet(&pattern, 0, sizeof(pattern));
    cpssOsMemSet(&action,  0, sizeof(action));

    cpssOsMemSet(&mask.ruleStdUdb.udb, 0xFF, sizeof(prvTgfUdb_L2_0_11_Array));
    cpssOsMemCpy(&pattern.ruleStdUdb.udb, prvTgfUdb_L2_0_11_Array, sizeof(prvTgfUdb_L2_0_11_Array));
    action.pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;
    ruleIndex     = 0;

    rc = prvTgfPclRuleSet(ruleFormat, ruleIndex, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfPclRuleSet: %d, %d, %d",
                                 prvTgfDevNum, ruleFormat, ruleIndex);
}

