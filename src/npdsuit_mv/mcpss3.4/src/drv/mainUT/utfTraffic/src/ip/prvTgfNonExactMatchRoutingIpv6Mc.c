/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6Mc.c
*
* DESCRIPTION:
*       Non-exact match IPv6 MC prefix routing.
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
#include <ip/prvTgfIpNonExactMatchRoutingIpv6Mc.h>

/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* Send VLAN */
#define PRV_TGF_SEND_VLANID_CNS           5

/* Nexthop VLAN */
#define PRV_TGF_NEXTHOP_MC_VLANID_CNS     6

/* Default VLAN */
#define PRV_TGF_DEFAULT_VLANID_CNS        7

/* Port index to send traffic */
#define PRV_TGF_SEND_PORT_IDX_CNS         0

/* Port indexes to receive MC traffic */
#define PRV_TGF_MC_RCV1_PORT_IDX_CNS      1
#define PRV_TGF_MC_RCV2_PORT_IDX_CNS      2

/* Port index to receive default traffic */
#define PRV_TGF_DEFAULT_PORT_IDX_CNS      3

/* Default vidx number */
#define PRV_TGF_DEF_VIDX_CNS              0

/* Number of packets to send */
#define PRV_TGF_BURST_COUNT_CNS         128

/* Max prefix length */
#define PRV_TGF_MAX_PREFIX_LEN_CNS      128

/* Default MLL pair pointer index */
static GT_U32 prvTgfDefMllPointerIndex  = 0;

/* MLL pair pointer index */
static GT_U32 prvTgfMllPointerIndex     = 2;

/* Default Route entry index for MC Route entry Table */
static GT_U32 prvTgfDefRouteEntryIndex  = 1;

/* The Route entry index for MC Route entry Table */
static GT_U32 prvTgfRouteEntryIndex     = 10;

/* the LPM DB id for LPM Table */
static GT_U32 prvTgfLpmDBId             = 0;

/* the Virtual Router id for LPM Table */
static GT_U32 prvTgfVrId                = 0;

/* Base multicast group IP addresses */
static GT_U8 prvTgfBaseMcGrpIpv6[16] = {0xFF, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xEE, 0xFF};

/* Base multicast source IP addresses */
static GT_U8 prvTgfBaseMcSrcIpv6[16] = {0x65, 0x45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x32, 0x12};

/****************************** Test packets *********************************/

/* L2 part of IPv6 MC packet */
static TGF_PACKET_L2_STC prvTgfPacketL2PartMcIpv6 =
{
    {0x33, 0x33, 0x00, 0x00, 0xee, 0xff},               /* daMac = macGrp */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}                /* saMac */
};

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_SEND_VLANID_CNS                       /* pri, cfi, VlanId */
};

/* Packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypePartIpv6 =
{
    TGF_ETHERTYPE_86DD_IPV6_TAG_CNS
};

/* Packet's IPv6 */
static TGF_PACKET_IPV6_STC prvTgfPacketMcIpv6Part =
{
    6,                  /* version */
    0,                  /* trafficClass */
    0,                  /* flowLabel */
    0x02,               /* payloadLen */
    0x3b,               /* nextHeader */
    0x40,               /* hopLimit */
    {0x6545, 0, 0, 0, 0, 0, 0x0000, 0x3212}, /* TGF_IPV6_ADDR srcAddr */
    {0xff01, 0, 0, 0, 0, 0, 0x0000, 0xeeff}  /* TGF_IPV6_ADDR dstAddr */
};

/* Packet's payload */
static GT_U8 prvTgfPayloadDataArr[] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x1B
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart =
{
    sizeof(prvTgfPayloadDataArr),  /* dataLength */
    prvTgfPayloadDataArr           /* dataPtr */
};

/* PARTS of MC IPv6 packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArrayMcIpv6[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2PartMcIpv6},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypePartIpv6},
    {TGF_PACKET_PART_IPV6_E,      &prvTgfPacketMcIpv6Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* LENGTH of IPv6 packet */
#define PRV_TGF_IPV6_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS + \
    TGF_IPV6_HEADER_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* MC IPv6 PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfoMcIpv6 =
{
    PRV_TGF_IPV6_PACKET_LEN_CNS,                                       /* totalLen */
    sizeof(prvTgfPacketPartArrayMcIpv6) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArrayMcIpv6                                        /* partsArray */
};

/* Offset of Source IPv6 address */
#define PRV_TGF_SRC_IPV6_OFFSET_CNS    ( TGF_L2_HEADER_SIZE_CNS   +  \
                                         TGF_VLAN_TAG_SIZE_CNS    +  \
                                         TGF_ETHERTYPE_SIZE_CNS   +  \
                                         TGF_IPV6_HEADER_SIZE_CNS -  \
                                         (2 * sizeof(TGF_IPV6_ADDR)) )

/* Offset to store index of modified bit in IPv6 packet*/
#define PRV_TGF_MOD_BIT_IDX_IPV6_OFFSET_CNS ( TGF_L2_HEADER_SIZE_CNS + \
                                         TGF_VLAN_TAG_SIZE_CNS       + \
                                         TGF_ETHERTYPE_SIZE_CNS      + \
                                         TGF_IPV6_HEADER_SIZE_CNS )
/*
 * Description: Defines routines to log IPv6 address.
 *
 * Parameters:
 *      ip   -   IPv6 address
 */
#define PRV_TGF_LOG_IPV6_MAC(ip)                             \
    {                                                        \
        PRV_UTF_LOG6_MAC("%02x%02x:%02x%02x:%02x%02x:",      \
                ip[0], ip[1], ip[2], ip[3], ip[4], ip[5]);   \
        PRV_UTF_LOG6_MAC("%02x%02x:%02x%02x:%02x%02x:",      \
                ip[6], ip[7], ip[8], ip[9], ip[10], ip[11]); \
        PRV_UTF_LOG4_MAC("%02x%02x:%02x%02x  ",              \
                ip[12], ip[13], ip[14], ip[15]);             \
    }

/*************************** Restore config ***********************************/

/* Parameters needed to be restored */
static struct
{
    GT_U16 vid;
} prvTgfRestoreCfg;

/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McPortCountersCheck
*
* DESCRIPTION:
*       Read and check port's counters
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       packetsCountRx   - Expected number of Rx packets
*       packetsCountTx   - Expected number of Tx packets
*       packetSize       - Size of packets
*       prefixLen        - current prefix length
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfIpNonExactMatchRoutingIpv6McPortCountersCheck
(
    IN CPSS_INTERFACE_INFO_STC *portInterfacePtr,
    IN GT_U32  packetsCountRx,
    IN GT_U32  packetsCountTx,
    IN GT_U32 packetSize,
    IN GT_U32 prefixLen
)
{
    GT_STATUS                     rc = GT_OK;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;
    GT_BOOL                       isCountersOk;

    /* Read counters */
    rc = prvTgfReadPortCountersEth(portInterfacePtr->devPort.devNum,
            portInterfacePtr->devPort.portNum, GT_FALSE, &portCntrs);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfReadPortCountersEth");

    /* Check Tx and Rx counters */
    isCountersOk = portCntrs.goodPktsSent.l[0]   == packetsCountTx &&
                   portCntrs.goodPktsRcv.l[0]    == packetsCountRx &&
                   portCntrs.goodOctetsSent.l[0] == (packetSize + TGF_CRC_LEN_CNS) * packetsCountTx &&
                   portCntrs.goodOctetsRcv.l[0]  == (packetSize + TGF_CRC_LEN_CNS) * packetsCountRx;

    /* Print expected values in case unexpected number of packets is received */
    if (GT_FALSE == isCountersOk)
    {
        PRV_UTF_LOG1_MAC("Unexpected number of packets received for IP prefix length = %u:\n", prefixLen);
        PRV_UTF_LOG1_MAC("Port number = [%u]:\n", portInterfacePtr->devPort.portNum);
        PRV_UTF_LOG2_MAC("  goodPktsSent   = %u, expected value = %u\n",
                portCntrs.goodPktsSent.l[0], packetsCountTx);
        PRV_UTF_LOG2_MAC("  goodPktsRcv    = %u, expected value = %u\n",
                portCntrs.goodPktsRcv.l[0], packetsCountRx);
        PRV_UTF_LOG2_MAC("  goodOctetsSent = %u, expected value = %u\n",
                portCntrs.goodOctetsSent.l[0],((packetSize + TGF_CRC_LEN_CNS) * packetsCountTx));
        PRV_UTF_LOG2_MAC("  goodOctetsRcv  = %u, expected value = %u\n",
                portCntrs.goodOctetsRcv.l[0], ((packetSize + TGF_CRC_LEN_CNS) * packetsCountRx));
    }

    return GT_FALSE == isCountersOk ? GT_FAIL : rc;
}

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McCapturedPacketsVerify
*
* DESCRIPTION:
*       Verify that Rx Table contains expected captured packets.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       prefixLen        - current prefix length
*       defPort          - TRUE means data from default port shall be verified
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfIpNonExactMatchRoutingIpv6McCapturedPacketsVerify
(
    IN CPSS_INTERFACE_INFO_STC *portInterfacePtr,
    IN GT_U8 prefixLen,
    IN GT_BOOL defPort
)
{
    GT_STATUS       rc            = GT_OK;
    GT_U32          packetLen     = PRV_TGF_IPV6_PACKET_LEN_CNS;
    GT_U32          origPacketLen = 0;
    GT_BOOL         getFirst      = GT_TRUE;
    GT_BOOL         valid         = GT_FALSE;
    GT_U8           devNum        = 0;
    GT_U8           queue         = 0;
    TGF_NET_DSA_STC rxParam       = {0};
    GT_U8           packetBuf[PRV_TGF_IPV6_PACKET_LEN_CNS];

    /* Go through entries from rxNetworkIf table */
    while (GT_OK == rc)
    {
        rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                           getFirst, GT_FALSE, packetBuf,
                                           &packetLen, &origPacketLen,
                                           &devNum, &queue, &rxParam);
        getFirst = GT_FALSE; /* now we get the next */

        /* Skip all not expected packets */
        if ((rxParam.srcIsTrunk || !rxParam.dsaCmdIsToCpu) ||
            (rxParam.devNum != portInterfacePtr->devPort.devNum) ||
            (rxParam.portNum != portInterfacePtr->devPort.portNum))
        {
            continue;
        }

        /* Validate stored modified bit index value */
        if (GT_OK == rc)
        {
            /* Show error log in case unexpected value is received */
            valid =   defPort
                    ? packetBuf[PRV_TGF_MOD_BIT_IDX_IPV6_OFFSET_CNS] >= prefixLen
                    : packetBuf[PRV_TGF_MOD_BIT_IDX_IPV6_OFFSET_CNS] < prefixLen;
            if (valid)
            {
                PRV_UTF_LOG1_MAC("Unexpected packet received on port = %u\n", portInterfacePtr->devPort.portNum);
                PRV_UTF_LOG1_MAC(" Modified bit index = %u\n", packetBuf[PRV_TGF_MOD_BIT_IDX_IPV6_OFFSET_CNS]);
                PRV_UTF_LOG1_MAC(" IP prefix length = %u\n", prefixLen);
                PRV_UTF_LOG0_MAC(" Source MC IP = ");
                PRV_TGF_LOG_IPV6_MAC((packetBuf + PRV_TGF_SRC_IPV6_OFFSET_CNS));
                PRV_UTF_LOG0_MAC("\n");
            }
        }
    }

    /* Value GT_NO_MORE is ok in this case */
    return (GT_NO_MORE == rc ? GT_OK : rc);
}

/******************************************************************************\
 *                            Public test functions                           *
\******************************************************************************/

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McBaseConfigurationSet
*
* DESCRIPTION:
*       Set Base Configuration
*
* INPUTS:
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv6McBaseConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS  rc = GT_OK;
    GT_U8      tagArray[]  = {1, 1, 1, 1};

    /* Create SEND VLAN on ports (0) */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_SEND_VLANID_CNS,
            prvTgfPortsArray, NULL, NULL, 1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* Create a VLAN for a MC subscribers on ports (1,2) */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_NEXTHOP_MC_VLANID_CNS,
            (prvTgfPortsArray + 1), NULL, (tagArray + 1), 2);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* Create a default MC VLAN for on port (3) */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_DEFAULT_VLANID_CNS,
            prvTgfPortsArray + 3, NULL, (tagArray + 3), 1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc,
            "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* Save default vlanId for restore */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum,
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                 &prvTgfRestoreCfg.vid);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d, %d",
                                 prvTgfDevNum, &prvTgfRestoreCfg.vid);

    /* Set default vlanId */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum,
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                 PRV_TGF_SEND_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfDevNum,
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* Create a static macEntry in SEND VLAN */
    rc = prvTgfBrgDefFdbMacEntryOnVidxSet(prvTgfPacketL2PartMcIpv6.daMac,
                                          PRV_TGF_SEND_VLANID_CNS,
                                          PRV_TGF_DEF_VIDX_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnVidxSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_SEND_VLANID_CNS);
}

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McRouteConfigurationSet
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv6McRouteConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                       rc = GT_OK;
    PRV_TGF_IP_MLL_PAIR_STC         mllPairEntry;
    PRV_TGF_IP_MC_ROUTE_ENTRY_STC   mcRouteEntryArray[1];
    PRV_TGF_IP_LTT_ENTRY_STC        mcRouteLttEntry;

    /* -------------------------------------------------------------------------
     * 1. Enable routing
     */

    /* Enable IPv6 Multicast Routing on a Port */
    rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                   CPSS_IP_MULTICAST_E,
                                   CPSS_IP_PROTOCOL_IPV6_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d %d",
                                 prvTgfDevNum,
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* Enable IPv6 Multicast Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_SEND_VLANID_CNS,
                                   CPSS_IP_MULTICAST_E,
                                   CPSS_IP_PROTOCOL_IPV6_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d %d",
                                 prvTgfDevNum, PRV_TGF_SEND_VLANID_CNS);

    /* -------------------------------------------------------------------------
     * 2. Setup default configuration
     */

    /* Create default MC Link List */
    cpssOsMemSet(&mllPairEntry, 0, sizeof(mllPairEntry));
    mllPairEntry.firstMllNode.mllRPFFailCommand                = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    mllPairEntry.firstMllNode.nextHopInterface.type            = CPSS_INTERFACE_PORT_E;
    mllPairEntry.firstMllNode.nextHopInterface.devPort.devNum  = prvTgfDevNum;
    mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_DEFAULT_PORT_IDX_CNS];
    mllPairEntry.firstMllNode.nextHopVlanId                    = PRV_TGF_DEFAULT_VLANID_CNS;
    mllPairEntry.firstMllNode.ttlHopLimitThreshold             = 4;
    mllPairEntry.firstMllNode.last                             = GT_TRUE;
    mllPairEntry.nextPointer                                   = 0;

    rc = prvTgfIpMllPairWrite(prvTgfDefMllPointerIndex,
                              PRV_TGF_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E,
                              &mllPairEntry);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfIpMllPairWrite");

    /* Create Default MC Route entry */
    cpssOsMemSet(&mcRouteEntryArray[0], 0, sizeof(mcRouteEntryArray[0]));
    mcRouteEntryArray[0].cmd                = CPSS_PACKET_CMD_ROUTE_E;
    mcRouteEntryArray[0].countSet           = CPSS_IP_CNT_SET2_E;
    mcRouteEntryArray[0].RPFFailCommand     = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    mcRouteEntryArray[0].internalMLLPointer = prvTgfDefMllPointerIndex;
    mcRouteEntryArray[0].externalMLLPointer = 0;

    /* Write Default Route entry */
    rc = prvTgfIpMcRouteEntriesWrite(prvTgfDefRouteEntryIndex, 1, mcRouteEntryArray);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfIpMcRouteEntriesWrite");

    /* -------------------------------------------------------------------------
     * 2. Setup test configuration
     */

    /* Create first and second MC Link List */
    cpssOsMemSet(&mllPairEntry, 0, sizeof(mllPairEntry));

    mllPairEntry.firstMllNode.mllRPFFailCommand                = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    mllPairEntry.firstMllNode.nextHopInterface.type            = CPSS_INTERFACE_PORT_E;
    mllPairEntry.firstMllNode.nextHopInterface.devPort.devNum  = prvTgfDevNum;
    mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV1_PORT_IDX_CNS];
    mllPairEntry.firstMllNode.nextHopVlanId                    = PRV_TGF_NEXTHOP_MC_VLANID_CNS;
    mllPairEntry.firstMllNode.ttlHopLimitThreshold             = 4;
    mllPairEntry.firstMllNode.last                             = GT_FALSE;

    mllPairEntry.secondMllNode.mllRPFFailCommand                = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    mllPairEntry.secondMllNode.nextHopInterface.type            = CPSS_INTERFACE_PORT_E;
    mllPairEntry.secondMllNode.nextHopInterface.devPort.devNum  = prvTgfDevNum;
    mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV2_PORT_IDX_CNS];
    mllPairEntry.secondMllNode.nextHopVlanId                    = PRV_TGF_NEXTHOP_MC_VLANID_CNS;
    mllPairEntry.secondMllNode.ttlHopLimitThreshold             = 4;
    mllPairEntry.secondMllNode.last                             = GT_TRUE;

    mllPairEntry.nextPointer                                    = 0;

    rc = prvTgfIpMllPairWrite(prvTgfMllPointerIndex,
                              PRV_TGF_IP_MLL_PAIR_READ_WRITE_WHOLE_E,
                              &mllPairEntry);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfIpMllPairWrite");

    /* create a MC Route entry */
    cpssOsMemSet(&mcRouteEntryArray[0], 0, sizeof(mcRouteEntryArray[0]));
    mcRouteEntryArray[0].cmd                = CPSS_PACKET_CMD_ROUTE_E;
    mcRouteEntryArray[0].countSet           = CPSS_IP_CNT_SET2_E;
    mcRouteEntryArray[0].RPFFailCommand     = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    mcRouteEntryArray[0].internalMLLPointer = 0;
    mcRouteEntryArray[0].externalMLLPointer = prvTgfMllPointerIndex;

    rc = prvTgfIpMcRouteEntriesWrite(prvTgfRouteEntryIndex, 1, mcRouteEntryArray);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesWrite");

    /* fill a nexthope info for the LPM */
    cpssOsMemSet(&mcRouteLttEntry, 0, sizeof(mcRouteLttEntry));
    mcRouteLttEntry.routeEntryBaseIndex = prvTgfRouteEntryIndex;
}

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McTrafficGenerate
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv6McTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc          = GT_OK;
    GT_U32    prefixIndex = 0;
    GT_U32    exRxCount;
    GT_U32    exTxCount;
    GT_U8     prefixLen;
    GT_U8     mask;
    GT_U8     byteIter;
    GT_U8     bitIter;
    PRV_TGF_IP_LTT_ENTRY_STC mcRouteLttEntry;
    CPSS_INTERFACE_INFO_STC  portInt = {0};
    GT_IPV6ADDR baseGrpIpAddr;
    GT_IPV6ADDR baseSrcIpAddr;
    GT_U8 ipAddrArray[PRV_TGF_BURST_COUNT_CNS][16];
    TGF_VFD_INFO_STC vfdArray[2] = {{0, 0}};

    /* Initialize GRP and SRC prefixes */
    cpssOsMemCpy(baseGrpIpAddr.arIP, prvTgfBaseMcGrpIpv6, sizeof(prvTgfBaseMcGrpIpv6));
    cpssOsMemCpy(baseSrcIpAddr.arIP, prvTgfBaseMcSrcIpv6, sizeof(prvTgfBaseMcSrcIpv6));

    /* Display used IP prefix */
    PRV_UTF_LOG0_MAC("Used MC SRC IP prefix = ");
    PRV_TGF_LOG_IPV6_MAC(prvTgfBaseMcSrcIpv6);
    PRV_UTF_LOG0_MAC("\n");

    /* Setup port interface */
    portInt.type            = CPSS_INTERFACE_PORT_E;
    portInt.devPort.devNum  = prvTgfDevNum;

    /* Fill a nexthope info for the LPM */
    cpssOsMemSet(&mcRouteLttEntry, 0, sizeof(mcRouteLttEntry));
    mcRouteLttEntry.routeEntryBaseIndex = prvTgfRouteEntryIndex;

    /* Generate Array of IP addresses */
    for (byteIter = 0; byteIter < 16; ++byteIter)
    {
        /* Invert appropriate bit */
        for (bitIter = 0; bitIter < 8; ++bitIter)
        {
            cpssOsMemCpy(ipAddrArray[prefixIndex], prvTgfBaseMcSrcIpv6, sizeof(prvTgfBaseMcSrcIpv6));
            mask = (GT_U8)(1 << (7 - bitIter));
            ipAddrArray[prefixIndex][byteIter] = (GT_U8)(prvTgfBaseMcSrcIpv6[byteIter] ^ mask);
            ++prefixIndex;
        }
    }

    /* Prepare VFD array entry for SRC IP address*/
    vfdArray[0].mode               = TGF_VFD_MODE_ARRAY_PATTERN_E;
    vfdArray[0].arrayPatternOffset = 0;
    vfdArray[0].offset             = PRV_TGF_SRC_IPV6_OFFSET_CNS;
    vfdArray[0].arrayPatternSize   = sizeof(GT_IPV6ADDR);
    vfdArray[0].cycleCount         = sizeof(GT_IPV6ADDR);
    vfdArray[0].arrayPatternPtr    = (GT_U8*)ipAddrArray;

    /* Prepare VFD array entry to transfer modified bit index */
    vfdArray[1].mode       = TGF_VFD_MODE_INCREMENT_E;
    vfdArray[1].incValue   = 1;
    vfdArray[1].offset     = PRV_TGF_MOD_BIT_IDX_IPV6_OFFSET_CNS;
    vfdArray[1].cycleCount = 1;

    /* Go trough the all prefix lengths */
    for (prefixLen = 1; prefixLen < PRV_TGF_MAX_PREFIX_LEN_CNS; ++prefixLen)
    {
        /* Create a MC Ipv6 prefix in a Virtual Router for the specified LPM DB */
        rc = prvTgfIpLpmIpv6McEntryAdd(prvTgfLpmDBId, prvTgfVrId,
                                       baseGrpIpAddr, PRV_TGF_MAX_PREFIX_LEN_CNS,
                                       baseSrcIpAddr, prefixLen,
                                       &mcRouteLttEntry, GT_FALSE, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6McEntryAdd: %d", prvTgfDevNum);

        /* Reset Ethernet counters */
        rc = prvTgfEthCountersReset(prvTgfDevNum);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfEthCountersReset");

        /* Clear capturing RxPcktTable */
        rc = tgfTrafficTableRxPcktTblClear();
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

        /* Enable capturing on MC receive port 1 */
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV1_PORT_IDX_CNS];

        rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInt, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
               "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d\n",
               prvTgfDevNum, prvTgfPortsArray[PRV_TGF_MC_RCV1_PORT_IDX_CNS]);

        /* Enable capturing on MC receive port 2 */
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV2_PORT_IDX_CNS];

        rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInt, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
               "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d\n",
               prvTgfDevNum, prvTgfPortsArray[PRV_TGF_MC_RCV2_PORT_IDX_CNS]);

        /* Enable capturing on default MC port */
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_DEFAULT_PORT_IDX_CNS];

        rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInt, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
               "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d\n",
               prvTgfDevNum, prvTgfPortsArray[PRV_TGF_DEFAULT_PORT_IDX_CNS]);

        /* Setup packet */
        rc = prvTgfSetTxSetupEth(prvTgfDevNum, &prvTgfPacketInfoMcIpv6,
                                 PRV_TGF_BURST_COUNT_CNS, 2, vfdArray);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfSetTxSetupEth: %d", prvTgfDevNum);

        /* Disable packet tracing */
        rc = tgfTrafficTracePacketByteSet(GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "tgfTrafficTracePacketByteSet: %d", prvTgfDevNum);

        /* Send packet */
        rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d\n",
            prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

        /* Disable capturing on default MC port */
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_DEFAULT_PORT_IDX_CNS];

        rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInt, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
               "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d\n",
               prvTgfDevNum, prvTgfPortsArray[PRV_TGF_DEFAULT_PORT_IDX_CNS]);

        /* Disable capturing on MC receive port 2 */
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV2_PORT_IDX_CNS];

        rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInt, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
               "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d\n",
               prvTgfDevNum, prvTgfPortsArray[PRV_TGF_MC_RCV2_PORT_IDX_CNS]);

        /* Disable capturing on MC receive port 1 */
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV1_PORT_IDX_CNS];

        rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInt, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
               "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d\n",
               prvTgfDevNum, prvTgfPortsArray[PRV_TGF_MC_RCV1_PORT_IDX_CNS]);

        /* Check port counters to verify MC ports contain expected number of packets */
        exRxCount = PRV_TGF_BURST_COUNT_CNS - prefixLen;
        exTxCount = PRV_TGF_BURST_COUNT_CNS - prefixLen;
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV1_PORT_IDX_CNS];

        rc = prvTgfIpNonExactMatchRoutingIpv6McPortCountersCheck(&portInt, exRxCount, exTxCount,
                prvTgfPacketInfoMcIpv6.totalLen, prefixLen);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfPortCountersCheck");

        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV2_PORT_IDX_CNS];

        rc = prvTgfIpNonExactMatchRoutingIpv6McPortCountersCheck(&portInt, exRxCount, exTxCount,
                prvTgfPacketInfoMcIpv6.totalLen, prefixLen);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfPortCountersCheck");

        /* Check port counters to verify default MC port contains expected number of packets */
        exRxCount = PRV_TGF_BURST_COUNT_CNS - exRxCount;
        exTxCount = PRV_TGF_BURST_COUNT_CNS - exTxCount;
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_DEFAULT_PORT_IDX_CNS];

        rc = prvTgfIpNonExactMatchRoutingIpv6McPortCountersCheck(&portInt, exRxCount, exTxCount,
                prvTgfPacketInfoMcIpv6.totalLen, prefixLen);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfPortCountersCheck");

        /* Verify captured packets on receiving ports */
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV1_PORT_IDX_CNS];

        rc = prvTgfIpNonExactMatchRoutingIpv6McCapturedPacketsVerify(&portInt, prefixLen, GT_FALSE);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfCapturedPacketsVerify");

        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_MC_RCV2_PORT_IDX_CNS];

        rc = prvTgfIpNonExactMatchRoutingIpv6McCapturedPacketsVerify(&portInt, prefixLen, GT_FALSE);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfCapturedPacketsVerify");

        /* Verify captured packets on default port */
        portInt.devPort.portNum = prvTgfPortsArray[PRV_TGF_DEFAULT_PORT_IDX_CNS];

        rc = prvTgfIpNonExactMatchRoutingIpv6McCapturedPacketsVerify(&portInt, prefixLen, GT_TRUE);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfCapturedPacketsVerify");

        /* Delete the MC Ipv6 prefix */
        rc = prvTgfIpLpmIpv6McEntriesFlush(prvTgfLpmDBId, prvTgfVrId);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6McEntriesFlush");
    }
}

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McConfigurationRestore
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
*       1. Restore Route Configuration
*       2. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfIpNonExactMatchRoutingIpv6McConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;

    /* Clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* -------------------------------------------------------------------------
     * 2. Restore Route Configuration
     */

    /* Flash all MC Ipv6 prefixes */
    rc = prvTgfIpLpmIpv6McEntriesFlush(prvTgfLpmDBId, prvTgfVrId);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv6McEntriesFlush");

    /* Disable multicast IPv6 routing on a Port */
    rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                   CPSS_IP_MULTICAST_E,
                                   CPSS_IP_PROTOCOL_IPV6_E, GT_FALSE);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable");

    /* Disable multicast IPv6 routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_SEND_VLANID_CNS, CPSS_IP_MULTICAST_E,
                                   CPSS_IP_PROTOCOL_IPV6_E, GT_FALSE);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable");

    /* -------------------------------------------------------------------------
     * 1. Restore Base Configuration
     */

    /* Flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d",
                                 prvTgfDevNum, GT_TRUE);

    /* Restore default vlanId to all ports */
    rc = prvTgfBrgVlanPvidSet(prvTgfRestoreCfg.vid);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPvidSet: %d, %d",
                                 prvTgfDevNum, prvTgfRestoreCfg.vid);

    /* Invalidate SEND VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_SEND_VLANID_CNS);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate 0");

    /* Invalidate VLAN entry for MC subscribers */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_NEXTHOP_MC_VLANID_CNS);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate 1");

    /* invalidate default VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_DEFAULT_VLANID_CNS);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate 2");
}
