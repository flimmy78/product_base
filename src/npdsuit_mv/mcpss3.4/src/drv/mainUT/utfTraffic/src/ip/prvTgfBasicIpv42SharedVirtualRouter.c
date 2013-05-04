/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBasicIpv42SharedVirtualRouter.c
*
* DESCRIPTION:
*       Basic IPV4 2 shared virtual Router
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
#include <ip/prvTgfBasicIpv42SharedVirtualRouter.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* number of VLANs */
#define PRV_TGF_VLAN_COUNT_CNS            4

/* number of Virtual Routers */
#define PRV_TGF_VR_COUNT_CNS              3

/* default VLAN Id */
#define PRV_TGF_VLANID_5_CNS              5

/* default VLAN Id */
#define PRV_TGF_VLANID_6_CNS              6

/* default VLAN Id */
#define PRV_TGF_VLANID_7_CNS              7

/* default VLAN Id */
#define PRV_TGF_VLANID_8_CNS              8

/* number of ports */
#define PRV_TGF_PORT_COUNT_CNS            4

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS         0

/* port number to receive traffic from */
#define PRV_TGF_RCV_PORT_IDX_CNS          3

/* port number to receive traffic from */
#define PRV_TGF_RCV_PORT1_IDX_CNS         2

/* port number to receive traffic from */
#define PRV_TGF_RCV_PORT2_IDX_CNS         1

/* MLL pair entry index */
#define PRV_TGF_MLL_INDEX_CNS             6

/* default vidx number */
#define PRV_TGF_DEF_VIDX_CNS              0

/* default number of packets to send */
static GT_U32        prvTgfBurstCount  = 1;

/* the Arp Address index of the Router ARP Table
   to write to the UC Route entry Arp nextHopARPPointer field */
static GT_U32        prvTgfRouterArpIndex[2] = {1, 2};

/* the Route entry index for UC Route entry Table */
static GT_U32        prvTgfRouteEntryBaseIndex[PRV_TGF_VR_COUNT_CNS] = {5, 6, 7};

/* the Virtual Route entry index for UC Route entry Table */
static GT_U32        prvTgfVirtualRouteEntryBaseIndex[2] = {0, 1};

/* the Virtual Route index */
static GT_U32        prvTgfVirtualRouteIndex[PRV_TGF_VR_COUNT_CNS] = {0, 1, 2};

/* the LPM DB id for LPM Table */
static GT_U32        prvTgfLpmDBId = 0;

/* the ARP MAC address to write to the Router ARP Table */
static TGF_MAC_ADDR  prvTgfArpMacArr[PRV_TGF_VLAN_COUNT_CNS] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x22},
                                                                {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}};

/****************************** Test packet 1 *********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},               /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}                /* saMac */
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
    4,                                 /* version */
    5,                                 /* headerLen */
    0,                                 /* typeOfService */
    0x2A,                              /* totalLen */
    0,                                 /* id */
    0,                                 /* flags */
    0,                                 /* offset */
    0x40,                              /* timeToLive */
    0x04,                              /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    { 1,  1,  1,  1},                  /* srcAddr */
    { 1,  1,  1,  3}                   /* dstAddr */
};

/* DATA of packet */
static GT_U8 prvTgfPayloadDataArr[] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart =
{
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET_LEN_CNS                                                 \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS +  \
    TGF_IPV4_HEADER_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* Length of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  (PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS)

/* Packet to send */
static TGF_PACKET_STC prvTgfPacketInfo =
{
    PRV_TGF_PACKET_LEN_CNS,                                           /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(prvTgfPacketPartArray[0]), /* numOfParts */
    prvTgfPacketPartArray                                             /* partsArray */
};

/****************************** Test packet 1 *********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacket1L2Part =
{
    {0x01, 0x00, 0x5E, 0x01, 0x01, 0x01},               /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}                /* saMac */
};
/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacket1VlanTagPart =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_6_CNS                          /* pri, cfi, VlanId */
};

/* packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacket1EtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacket1Ipv4Part =
{
    4,                                 /* version */
    5,                                 /* headerLen */
    0,                                 /* typeOfService */
    0x2A,                              /* totalLen */
    0,                                 /* id */
    0,                                 /* flags */
    0,                                 /* offset */
    0x40,                              /* timeToLive */
    0x04,                              /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    {  2,  2,  2,  2},                 /* srcAddr */
    {224,  1,  1,  1}                  /* dstAddr */
};

/* DATA of packet */
static GT_U8 prvTgfPayload1DataArr[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacket1PayloadPart =
{
    sizeof(prvTgfPayload1DataArr),                       /* dataLength */
    prvTgfPayload1DataArr                                /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacket1PartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacket1VlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacket1EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacket1Ipv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacket1PayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET1_LEN_CNS                                                \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS +  \
    TGF_IPV4_HEADER_SIZE_CNS + sizeof(prvTgfPayload1DataArr)

/* Length of packet with CRC */
#define PRV_TGF_PACKET1_CRC_LEN_CNS  (PRV_TGF_PACKET1_LEN_CNS + TGF_CRC_LEN_CNS)

/* Packet to send */
static TGF_PACKET_STC prvTgfPacket1Info =
{
    PRV_TGF_PACKET1_LEN_CNS,                                            /* totalLen */
    sizeof(prvTgfPacket1PartArray) / sizeof(prvTgfPacket1PartArray[0]), /* numOfParts */
    prvTgfPacket1PartArray                                              /* partsArray */
};

/*************************** Restore config ***********************************/
/* parameters that is needed to be restored */
static struct
{
    CPSS_PORTS_BMP_STC  defPortMembers;
} prvTgfRestoreCfg;

/******************************************************************************/

/*******************************************************************************
* prvTgfBasicIpv4Uc2VirtualRouterPacketSend
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
static GT_VOID prvTgfBasicIpv4Uc2VirtualRouterPacketSend
(
    IN  GT_U8                         portNum,
    IN  TGF_PACKET_STC               *packetInfoPtr
)
{
    GT_STATUS       rc         = GT_OK;


    /* use different 'etherType for vlan tagging' instead of 0x8100 */
    /* this will make our packet that has '0x8100' to be recognized as 'untagged'
      by the 'traffic generator' and also the CPSS will not override the ethertype
      of tag0 with the DSA tag */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

    /* -------------------------------------------------------------------------
     * 1. Sending unknown unicast
     */
    PRV_UTF_LOG0_MAC("======= Sending unknown unicast =======\n");

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
* prvTgfBasicIpv42VirtualRouterUcTrafficGenerate
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
static GT_VOID prvTgfBasicIpv42VirtualRouterUcTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_U16                          iter         = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;
    PRV_TGF_IP_COUNTER_SET_STC      ipCounters;


    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* reset IP couters */
        rc = prvTgfCountersIpSet(prvTgfPortsArray[portIter], portIter);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCountersIpSet: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    PRV_UTF_LOG0_MAC("======= Generating UC Traffic =======\n");

    for (iter = 0; iter < PRV_TGF_VR_COUNT_CNS; iter++)
    {
        /* set Vlan ID */
        prvTgfPacketVlanTagPart.vid = (GT_U16) (iter + PRV_TGF_VLANID_5_CNS);

        /* send packet */
        prvTgfBasicIpv4Uc2VirtualRouterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);
    }

    PRV_UTF_LOG0_MAC("======= Checking Counters =======\n");

    /* read and check ethernet counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* read IP counters */
        PRV_UTF_LOG1_MAC("IP counters for Port [%d]:\n", prvTgfPortsArray[portIter]);

        rc = prvTgfCountersIpGet(prvTgfDevNum, portIter, GT_TRUE, &ipCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCountersIpGet: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* clear expected counters */
        cpssOsMemSet(&expectedCntrs, 0, sizeof(expectedCntrs));

        /* calculate expected counters */
        switch (portIter)
        {
            case PRV_TGF_SEND_PORT_IDX_CNS:
                /* packetSize is not changed */
                expectedCntrs.goodOctetsSent.l[0] = PRV_TGF_PACKET_CRC_LEN_CNS * PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.goodOctetsRcv.l[0]  = PRV_TGF_PACKET_CRC_LEN_CNS * PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.goodPktsRcv.l[0]    = PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.goodPktsSent.l[0]   = PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.ucPktsRcv.l[0]      = PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.ucPktsSent.l[0]     = PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.brdcPktsRcv.l[0]    = 0;
                expectedCntrs.mcPktsRcv.l[0]      = 0;
                break;

            case PRV_TGF_RCV_PORT_IDX_CNS:
                /* packetSize is not changed */
                expectedCntrs.goodOctetsSent.l[0] = PRV_TGF_PACKET_CRC_LEN_CNS * PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.goodPktsSent.l[0]   = PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.ucPktsSent.l[0]     = PRV_TGF_VR_COUNT_CNS * prvTgfBurstCount;
                expectedCntrs.brdcPktsRcv.l[0]    = 0;
                expectedCntrs.mcPktsRcv.l[0]      = 0;
                break;

            default:
                /* for other ports */
                expectedCntrs.goodOctetsSent.l[0] = 0;
                expectedCntrs.goodPktsSent.l[0]   = 0;
                expectedCntrs.ucPktsSent.l[0]     = 0;
                expectedCntrs.brdcPktsSent.l[0]   = 0;
                expectedCntrs.mcPktsSent.l[0]     = 0;
                expectedCntrs.goodOctetsRcv.l[0]  = 0;
                expectedCntrs.goodPktsRcv.l[0]    = 0;
                expectedCntrs.ucPktsRcv.l[0]      = 0;
                expectedCntrs.brdcPktsRcv.l[0]    = 0;
                expectedCntrs.mcPktsRcv.l[0]      = 0;
        }

        /* check Rx\Tx counters */
        PRV_TGF_VERIFY_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(isEqualCntrs, GT_TRUE, "get another counters values.");

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
    }

    /* print captured packet */
    rc = prvTgfPortCapturedPacketPrint(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RCV_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortCapturedPacketPrint: %d, %d\n",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RCV_PORT_IDX_CNS]);
}

/*******************************************************************************
* prvTgfBasicIpv42VirtualRouterMcTrafficGenerate
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
static GT_VOID prvTgfBasicIpv42VirtualRouterMcTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_U16                          iter         = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;
    PRV_TGF_IP_COUNTER_SET_STC      ipCounters;


    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* reset IP couters */
        rc = prvTgfCountersIpSet(prvTgfPortsArray[portIter], portIter);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCountersIpSet: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    PRV_UTF_LOG0_MAC("======= Generating MC Traffic =======\n");

    for (iter = 0; iter < PRV_TGF_VR_COUNT_CNS - 1; iter++)
    {
        /* set Vlan ID */
        prvTgfPacket1VlanTagPart.vid = (GT_U16) (iter + PRV_TGF_VLANID_6_CNS);

        /* send packet */
        prvTgfBasicIpv4Uc2VirtualRouterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacket1Info);
    }

    PRV_UTF_LOG0_MAC("======= Checking Counters =======\n");

    /* read and check ethernet counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* read IP counters */
        PRV_UTF_LOG1_MAC("IP counters for Port [%d]:\n", prvTgfPortsArray[portIter]);

        prvTgfCountersIpGet(prvTgfDevNum, portIter, GT_TRUE, &ipCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCountersIpGet: %d, %d\n",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* clear expected counters */
        cpssOsMemSet(&expectedCntrs, 0, sizeof(expectedCntrs));

        /* calculate expected counters */
        switch (portIter)
        {
            case PRV_TGF_SEND_PORT_IDX_CNS:
                /* packetSize is not changed */
                expectedCntrs.goodOctetsSent.l[0] = PRV_TGF_PACKET1_CRC_LEN_CNS * (PRV_TGF_VR_COUNT_CNS - 1) * prvTgfBurstCount;
                expectedCntrs.goodOctetsRcv.l[0]  = PRV_TGF_PACKET1_CRC_LEN_CNS * (PRV_TGF_VR_COUNT_CNS - 1) * prvTgfBurstCount;
                expectedCntrs.goodPktsRcv.l[0]    = (PRV_TGF_VR_COUNT_CNS - 1) * prvTgfBurstCount;
                expectedCntrs.goodPktsSent.l[0]   = (PRV_TGF_VR_COUNT_CNS - 1) * prvTgfBurstCount;
                expectedCntrs.mcPktsRcv.l[0]      = (PRV_TGF_VR_COUNT_CNS - 1) * prvTgfBurstCount;
                expectedCntrs.mcPktsSent.l[0]     = (PRV_TGF_VR_COUNT_CNS - 1) * prvTgfBurstCount;
                expectedCntrs.brdcPktsRcv.l[0]    = 0;
                expectedCntrs.ucPktsRcv.l[0]      = 0;
                break;

            case PRV_TGF_RCV_PORT1_IDX_CNS:
            case PRV_TGF_RCV_PORT2_IDX_CNS:
                /* packetSize is not changed */
                expectedCntrs.goodOctetsSent.l[0] = PRV_TGF_PACKET1_CRC_LEN_CNS * prvTgfBurstCount;
                expectedCntrs.goodPktsSent.l[0]   = prvTgfBurstCount;
                expectedCntrs.mcPktsSent.l[0]     = prvTgfBurstCount;
                expectedCntrs.brdcPktsRcv.l[0]    = 0;
                expectedCntrs.ucPktsRcv.l[0]      = 0;
                break;

            default:
                /* for other ports */
                expectedCntrs.goodOctetsSent.l[0] = 0;
                expectedCntrs.goodPktsSent.l[0]   = 0;
                expectedCntrs.ucPktsSent.l[0]     = 0;
                expectedCntrs.brdcPktsSent.l[0]   = 0;
                expectedCntrs.mcPktsSent.l[0]     = 0;
                expectedCntrs.goodOctetsRcv.l[0]  = 0;
                expectedCntrs.goodPktsRcv.l[0]    = 0;
                expectedCntrs.ucPktsRcv.l[0]      = 0;
                expectedCntrs.brdcPktsRcv.l[0]    = 0;
                expectedCntrs.mcPktsRcv.l[0]      = 0;
        }

        /* check Rx\Tx counters */
        PRV_TGF_VERIFY_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(isEqualCntrs, GT_TRUE, "get another counters values.");

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
    }

    /* print captured packet */
    rc = prvTgfPortCapturedPacketPrint(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RCV_PORT1_IDX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortCapturedPacketPrint: %d, %d\n",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RCV_PORT1_IDX_CNS]);
}

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterStatusSet
*
* DESCRIPTION:
*       Enable or disable routing mechanism
*
* INPUTS:
*       status - enable\disable routing
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
static GT_VOID prvTgfBasicIpv42SharedVirtualRouterStatusSet
(
    IN GT_BOOL  status
)
{
    GT_STATUS   rc = GT_OK;


    /* enable UC IPv4 Routing on a Port */
    rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                   CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, status);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d", prvTgfDevNum);

    /* enable MC IPv4 Routing on a Port */
    rc = prvTgfIpPortRoutingEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                   CPSS_IP_MULTICAST_E, CPSS_IP_PROTOCOL_IPV4_E, status);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpPortRoutingEnable: %d", prvTgfDevNum);

    /* enable IPv4 UC Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_5_CNS, CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, status);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d", prvTgfDevNum);

    /* enable IPv4 UC Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_6_CNS, CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, status);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d", prvTgfDevNum);

    /* enable IPv4 MC Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_6_CNS, CPSS_IP_MULTICAST_E, CPSS_IP_PROTOCOL_IPV4_E, status);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d", prvTgfDevNum);

    /* enable IPv4 UC Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_7_CNS, CPSS_IP_UNICAST_E, CPSS_IP_PROTOCOL_IPV4_E, status);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d", prvTgfDevNum);

    /* enable IPv4 UC Routing on Vlan */
    rc = prvTgfIpVlanRoutingEnable(PRV_TGF_VLANID_7_CNS, CPSS_IP_MULTICAST_E, CPSS_IP_PROTOCOL_IPV4_E, status);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpVlanRoutingEnable: %d", prvTgfDevNum);
}

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterBaseConfigSet
*
* DESCRIPTION:
*       Set Base Configuration
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterBaseConfigSet
(
    GT_VOID
)
{
    GT_STATUS           rc          = GT_OK;
    CPSS_PORTS_BMP_STC  portMembers = {{0, 0}};


    PRV_UTF_LOG0_MAC("======= Setting Base Configuration =======\n");

    /* set default vlan entry for vlan ID 5 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_5_CNS, prvTgfPortsArray,
                                           NULL, NULL, 1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* set default vlan entry for vlan ID 6 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_6_CNS, prvTgfPortsArray,
                                           NULL, NULL, 1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* set default vlan entry for vlan ID 7 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_7_CNS, prvTgfPortsArray,
                                           NULL, NULL, 1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* set default vlan entry for vlan ID 7 */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_8_CNS, prvTgfPortsArray + 1,
                                           NULL, NULL, PRV_TGF_VR_COUNT_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d", prvTgfDevNum);

    /* create FDB macEntry */
    rc = prvTgfBrgDefFdbMacEntryOnVidSet(prvTgfPacketL2Part.daMac, PRV_TGF_VLANID_5_CNS,
                                         PRV_TGF_VLANID_5_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);

    /* create FDB macEntry */
    rc = prvTgfBrgDefFdbMacEntryOnVidSet(prvTgfPacketL2Part.daMac, PRV_TGF_VLANID_6_CNS,
                                         PRV_TGF_VLANID_5_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);

    /* create FDB macEntry */
    rc = prvTgfBrgDefFdbMacEntryOnVidSet(prvTgfPacketL2Part.daMac, PRV_TGF_VLANID_7_CNS,
                                         PRV_TGF_VLANID_5_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnPortSet: %d", prvTgfDevNum);

    /* save default port members for MC group */
    rc = prvTgfBrgMcEntryRead(prvTgfDevNum, PRV_TGF_DEF_VIDX_CNS,
                              &(prvTgfRestoreCfg.defPortMembers));
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgMcEntryRead: %d, %d",
                                 prvTgfDevNum, PRV_TGF_DEF_VIDX_CNS);

    /* create bmp with ports for MC group */
    CPSS_PORTS_BMP_PORT_SET_MAC(&portMembers, prvTgfPortsArray[PRV_TGF_RCV_PORT_IDX_CNS]);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portMembers, prvTgfPortsArray[PRV_TGF_RCV_PORT1_IDX_CNS]);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portMembers, prvTgfPortsArray[PRV_TGF_RCV_PORT2_IDX_CNS]);
    
    /* write MC Group entry */
    rc = prvTgfBrgMcEntryWrite(prvTgfDevNum, PRV_TGF_DEF_VIDX_CNS, &portMembers);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgMcEntryWrite: %d, %d",
                                 prvTgfDevNum, PRV_TGF_DEF_VIDX_CNS);

    /* create a static macEntry in SEND VLAN */
    rc = prvTgfBrgDefFdbMacEntryOnVidxSet(prvTgfPacket1L2Part.daMac,
                                          PRV_TGF_VLANID_6_CNS,
                                          PRV_TGF_DEF_VIDX_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnVidxSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_6_CNS);

    /* create a static macEntry in SEND VLAN */
    rc = prvTgfBrgDefFdbMacEntryOnVidxSet(prvTgfPacket1L2Part.daMac,
                                          PRV_TGF_VLANID_7_CNS,
                                          PRV_TGF_DEF_VIDX_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnVidxSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_7_CNS);
}

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterRouteConfigSet
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterRouteConfigSet
(
    GT_VOID
)
{
    GT_STATUS                               rc = GT_OK;
    PRV_TGF_IP_UC_ROUTE_ENTRY_STC           ucRouteEntry;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    defIpv4UcRouteEntryInfo;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    defIpv6UcRouteEntryInfo;
    PRV_TGF_IP_LTT_ENTRY_STC                defIpv4McRouteLttEntry;
    PRV_TGF_IP_LTT_ENTRY_STC                defIpv6McRouteLttEntry;
    PRV_TGF_IP_MC_ROUTE_ENTRY_STC           cpssMcRouteEntry;
    PRV_TGF_IP_LTT_ENTRY_STC                *ipLttEntryPtr = NULL;
    GT_U32                                  i;

    PRV_UTF_LOG0_MAC("======= Setting Route Configuration =======\n");

    /*------------------------- Routing config -------------------------------*/

    /* set a route entry with CMD_TRAP_TO_CPU as default ipv4 unicast */
    cpssOsMemSet(&ucRouteEntry, 0, sizeof(ucRouteEntry));

    /* create new route entry */
    ucRouteEntry.cmd                   = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    ucRouteEntry.nextHopInterface.type = CPSS_INTERFACE_VID_E;

    rc = prvTgfIpUcRouteEntriesWrite(prvTgfVirtualRouteEntryBaseIndex[0], &ucRouteEntry, 1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesWrite: %d", prvTgfDevNum);

    /* set a route entry with CMD_TRAP_TO_CPU as default ipv4 multicast */
    cpssOsMemSet(&cpssMcRouteEntry, 0, sizeof(cpssMcRouteEntry));

    cpssMcRouteEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

    rc = prvTgfIpMcRouteEntriesWrite(prvTgfVirtualRouteEntryBaseIndex[1], 1, &cpssMcRouteEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpMcRouteEntriesWrite: %d", prvTgfDevNum);

    /*--------------------- Create virtual routers ---------------------------*/

    /* delete default Virtual Router */
    rc = prvTgfIpLpmVirtualRouterDel(prvTgfLpmDBId, 0);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterDel: %d", prvTgfDevNum);

    /* clear entry */
    cpssOsMemSet(&defIpv4UcRouteEntryInfo, 0, sizeof(PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    cpssOsMemSet(&defIpv6UcRouteEntryInfo, 0, sizeof(PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            ipLttEntryPtr = &defIpv4UcRouteEntryInfo.ipLttEntry;
        }
        if (i == 1)
        {
            ipLttEntryPtr = &defIpv6UcRouteEntryInfo.ipLttEntry;
        }
        /* set defUcRouteEntryInfo */
        ipLttEntryPtr->routeType                = PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E;
        ipLttEntryPtr->numOfPaths               = 0;
        ipLttEntryPtr->routeEntryBaseIndex      = prvTgfVirtualRouteEntryBaseIndex[0];
        ipLttEntryPtr->ucRPFCheckEnable         = GT_FALSE;
        ipLttEntryPtr->sipSaCheckMismatchEnable = GT_FALSE;
        ipLttEntryPtr->ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    }

    /* create shared Virtual Router */
    rc = prvTgfIpLpmVirtualRouterSharedAdd(prvTgfLpmDBId,
                                           prvTgfVirtualRouteIndex[0],
                                           &defIpv4UcRouteEntryInfo,
                                           &defIpv6UcRouteEntryInfo,
                                           NULL /*&defIpv4McRouteLttEntry*/,
                                           NULL /*&defIpv6McRouteLttEntry*/);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterAdd: %d", prvTgfDevNum);

    /* clear entry */
    cpssOsMemSet(&defIpv4McRouteLttEntry, 0, sizeof(defIpv4McRouteLttEntry));
    cpssOsMemSet(&defIpv6McRouteLttEntry, 0, sizeof(defIpv6McRouteLttEntry));
    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            ipLttEntryPtr = &defIpv4McRouteLttEntry;
        }
        if (i == 1)
        {
            ipLttEntryPtr = &defIpv6McRouteLttEntry;
        }

    /* set defMcRouteLttEntry. */
    ipLttEntryPtr->routeType                = PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    ipLttEntryPtr->numOfPaths               = 0;
    ipLttEntryPtr->routeEntryBaseIndex      = prvTgfVirtualRouteEntryBaseIndex[1];
    ipLttEntryPtr->ucRPFCheckEnable         = GT_FALSE;
    ipLttEntryPtr->sipSaCheckMismatchEnable = GT_FALSE;
    ipLttEntryPtr->ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    }
    /* create non-shared Virtual Router */
    rc = prvTgfIpLpmVirtualRouterAdd(prvTgfLpmDBId,
                                     prvTgfVirtualRouteIndex[1],
                                     NULL, /* defIpv4UcRouteEntryInfo */
                                     NULL, /* defIpv6UcRouteEntryInfo */
                                     &defIpv4McRouteLttEntry,
                                     &defIpv6McRouteLttEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterAdd: %d", prvTgfDevNum);

    /* create non-shared Virtual Router */
    rc = prvTgfIpLpmVirtualRouterAdd(prvTgfLpmDBId,
                                     prvTgfVirtualRouteIndex[2],
                                     NULL, /* defIpv4UcRouteEntryInfo */
                                     NULL, /* defIpv6UcRouteEntryInfo */
                                     &defIpv4McRouteLttEntry,
                                     &defIpv6McRouteLttEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterAdd: %d", prvTgfDevNum);

    /*-------------------------- Bind vlans into VR --------------------------*/

    /* set VRF Id */
    rc = prvTgfBrgVlanVrfIdSet(PRV_TGF_VLANID_5_CNS, 5);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanVrfIdSet: %d", prvTgfDevNum);

    /* set VRF Id */
    rc = prvTgfBrgVlanVrfIdSet(PRV_TGF_VLANID_6_CNS, prvTgfVirtualRouteIndex[1]);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanVrfIdSet: %d", prvTgfDevNum);

    /* set VRF Id */
    rc = prvTgfBrgVlanVrfIdSet(PRV_TGF_VLANID_7_CNS, prvTgfVirtualRouteIndex[2]);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanVrfIdSet: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterUcIpConfigSet
*
* DESCRIPTION:
*       Set UC IP Configuration
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterUcIpConfigSet
(
    GT_VOID
)
{
    GT_STATUS                       rc = GT_OK;
    GT_ETHERADDR                    arpMacAddr;
    GT_IPADDR                       ipAddr;
    PRV_TGF_IP_UC_ROUTE_ENTRY_STC   ucRouteEntry;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;


    /*-------------------------- Config route table --------------------------*/

    /* create a ARP MAC address to the Router ARP Table */
    cpssOsMemCpy(arpMacAddr.arEther, prvTgfArpMacArr[0], sizeof(TGF_MAC_ADDR));

    rc = prvTgfIpRouterArpAddrWrite(prvTgfRouterArpIndex[0], &arpMacAddr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRouterArpAddrWrite: %d", prvTgfDevNum);

    /* create a UC Route entry for the Route Table with CMD_ROUTE */
    cpssOsMemSet(&ucRouteEntry, 0, sizeof(ucRouteEntry));

    ucRouteEntry.cmd                              = CPSS_PACKET_CMD_ROUTE_E;
    ucRouteEntry.nextHopVlanId                    = PRV_TGF_VLANID_8_CNS;
    ucRouteEntry.nextHopInterface.type            = CPSS_INTERFACE_PORT_E;
    ucRouteEntry.nextHopInterface.devPort.devNum  = prvTgfDevNum;
    ucRouteEntry.nextHopInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_RCV_PORT_IDX_CNS];
    ucRouteEntry.nextHopARPPointer                = prvTgfRouterArpIndex[0];

    rc = prvTgfIpUcRouteEntriesWrite(prvTgfRouteEntryBaseIndex[0], &ucRouteEntry, 1);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesWrite: %d", prvTgfDevNum);

    /*-------------------------- Create prefixes -----------------------------*/

    /* set LPM prefix as the DIP of test packet */
    cpssOsMemCpy(ipAddr.arIP, prvTgfPacketIpv4Part.dstAddr, sizeof(ipAddr.arIP));

    /* set Route Entry Index the same as in Route Table */
    cpssOsMemSet(&routeEntryInfo, 0, sizeof(routeEntryInfo));
    routeEntryInfo.ipLttEntry.routeEntryBaseIndex = prvTgfRouteEntryBaseIndex[0];

    /* create prefix in LPM Table */
    rc = prvTgfIpLpmIpv4UcPrefixAdd(prvTgfLpmDBId, prvTgfVirtualRouteIndex[0],
                                    ipAddr, 32, &routeEntryInfo, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixAdd: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfBasicIpv42NonSharedVirtualRouterMcIpConfigSet
*
* DESCRIPTION:
*       Set MC IP Configuration
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
GT_VOID prvTgfBasicIpv42NonSharedVirtualRouterMcIpConfigSet
(
    GT_VOID
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          iter = 0;
    GT_IPADDR                       ipGroup, ipSrc;
    PRV_TGF_IP_MLL_PAIR_STC         mllPairEntry;
    PRV_TGF_IP_MC_ROUTE_ENTRY_STC   mcRouteEntry;
    PRV_TGF_IP_LTT_ENTRY_STC        mcRouteLttEntry;


    /* MC IP config for non-shared VRs */
    for (iter = 0; iter < PRV_TGF_VR_COUNT_CNS - 1; iter++)
    {
        /* clear entry */
        cpssOsMemSet((GT_VOID*) &mllPairEntry, 0, sizeof(mllPairEntry));

        /* configure MLL entry */
        mllPairEntry.firstMllNode.mllRPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        mllPairEntry.firstMllNode.nextHopVlanId     = PRV_TGF_VLANID_8_CNS;

        mllPairEntry.firstMllNode.nextHopInterface.type            = CPSS_INTERFACE_PORT_E;
        mllPairEntry.firstMllNode.nextHopInterface.devPort.devNum  = prvTgfDevNum;
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum = prvTgfPortsArray[iter + 1];
        mllPairEntry.firstMllNode.last                             = (GT_BOOL) iter;

        /* write a MC Link List */
        rc = prvTgfIpMllPairWrite(PRV_TGF_MLL_INDEX_CNS + iter,
                                  PRV_TGF_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E,
                                  &mllPairEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpMllPairWrite: %d", prvTgfDevNum);

        /* clear entry */
        cpssOsMemSet((GT_VOID*) &mcRouteEntry, 0, sizeof(mcRouteEntry));

        mcRouteEntry.cmd                = CPSS_PACKET_CMD_ROUTE_E;
        mcRouteEntry.internalMLLPointer = PRV_TGF_MLL_INDEX_CNS + iter;

        /* create a MC Route entry */
        rc = prvTgfIpMcRouteEntriesWrite(prvTgfRouteEntryBaseIndex[iter + 1], 1, &mcRouteEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpUcRouteEntriesWrite: %d", prvTgfDevNum);

        /* clear entry */
        cpssOsMemSet((GT_VOID*) &mcRouteLttEntry, 0, sizeof(mcRouteLttEntry));

        /* set route base index */
        mcRouteLttEntry.routeEntryBaseIndex = prvTgfRouteEntryBaseIndex[iter + 1];

        cpssOsMemCpy((GT_VOID*) ipGroup.arIP, prvTgfPacket1Ipv4Part.dstAddr, sizeof(ipGroup.arIP));
        cpssOsMemCpy((GT_VOID*) ipSrc.arIP,   prvTgfPacket1Ipv4Part.srcAddr, sizeof(ipSrc.arIP));

        /* add IP MC route */
        rc = prvTgfIpLpmIpv4McEntryAdd(prvTgfLpmDBId, prvTgfVirtualRouteIndex[iter + 1], ipGroup,
                                       32, ipSrc, 32, &mcRouteLttEntry, GT_FALSE, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4McEntryAdd: %d", prvTgfDevNum);
    }
}

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterEnable
*
* DESCRIPTION:
*       Enable routing mechanism
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterEnable
(
    GT_VOID
)
{
    /* enable routing */
    prvTgfBasicIpv42SharedVirtualRouterStatusSet(GT_TRUE);
}

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterTrafficGenerate
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterTrafficGenerate
(
    GT_VOID
)
{
    /* Generate UC traffic */
    prvTgfBasicIpv42VirtualRouterUcTrafficGenerate();

    /* Generate MC traffic */
    prvTgfBasicIpv42VirtualRouterMcTrafficGenerate();
}

/*******************************************************************************
* prvTgfBasicIpv4Uc2SharedVRConfigurationRestore
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
*       2. Restore Route Configuration
*       1. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfBasicIpv4Uc2SharedVRConfigRestore
(
    GT_VOID
)
{

    GT_U32      vlanIter = 0;
    GT_U32      vrIter   = 0;
    GT_STATUS   rc       = GT_OK;
    PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT    defUcRouteEntryInfo[2];
    PRV_TGF_IP_LTT_ENTRY_STC                defMcRouteLttEntry[2];
    GT_U32 i;


    PRV_UTF_LOG0_MAC("======= Restoring Configuration =======\n");
    cpssOsMemSet(defUcRouteEntryInfo, 0, sizeof(defUcRouteEntryInfo));
    cpssOsMemSet(defMcRouteLttEntry, 0, sizeof(defMcRouteLttEntry));

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /*-------------------- Restore Route Configuration -----------------------*/

    /* flush the UC IPv4 Routing table */
    rc = prvTgfIpLpmIpv4UcPrefixesFlush(prvTgfLpmDBId, prvTgfVirtualRouteIndex[0]);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixesFlush: %d", prvTgfDevNum);

    /* flush the MC IPv4 Routing table */
    for (vrIter = 1; vrIter < PRV_TGF_VR_COUNT_CNS; vrIter++)
    {
        rc = prvTgfIpLpmIpv4McEntriesFlush(prvTgfLpmDBId, prvTgfVirtualRouteIndex[vrIter]);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmIpv4UcPrefixesFlush: %d", prvTgfDevNum);
    }

    /* disable routing */
    prvTgfBasicIpv42SharedVirtualRouterStatusSet(GT_FALSE);

    /* restore virtual router */
    for (vrIter = 0; vrIter < PRV_TGF_VR_COUNT_CNS; vrIter++)
    {
        /* delete virtual router */
        rc = prvTgfIpLpmVirtualRouterDel(prvTgfLpmDBId, prvTgfVirtualRouteIndex[vrIter]);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterDel: %d", prvTgfDevNum);
    }
    for (i = 0; i < 2; i++)
    {
        /* set defUcLttEntryInfo */
        defUcRouteEntryInfo[i].ipLttEntry.routeType                = PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E;
        defUcRouteEntryInfo[i].ipLttEntry.numOfPaths               = 0;
        defUcRouteEntryInfo[i].ipLttEntry.routeEntryBaseIndex      = 0;
        defUcRouteEntryInfo[i].ipLttEntry.ucRPFCheckEnable         = GT_FALSE;
        defUcRouteEntryInfo[i].ipLttEntry.sipSaCheckMismatchEnable = GT_FALSE;
        defUcRouteEntryInfo[i].ipLttEntry.ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;

        /* set defMcLttEntryInfo */
        defMcRouteLttEntry[i].ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        defMcRouteLttEntry[i].numOfPaths = 0;
        defMcRouteLttEntry[i].routeEntryBaseIndex = 1;
        defMcRouteLttEntry[i].routeType = 0;
        defMcRouteLttEntry[i].sipSaCheckMismatchEnable = GT_FALSE;
        defMcRouteLttEntry[i].ucRPFCheckEnable = GT_FALSE;
    }
    /* create default Virtual Route */
    rc = prvTgfIpLpmVirtualRouterAdd(prvTgfLpmDBId,
                                     0,
                                     &defUcRouteEntryInfo[0],/* defIpv4UcRouteEntryInfo */
                                     &defUcRouteEntryInfo[1],/* defIpv6UcRouteEntryInfo */
                                     &defMcRouteLttEntry[0], /*defIpv4McRouteLttEntry*/
                                     &defMcRouteLttEntry[0]);/*defIpv6McRouteLttEntry*/
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpLpmVirtualRouterAdd: %d", prvTgfDevNum);

    /*-------------------- Restore Base Configuration ------------------------*/

    /* delete the FDB entry */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", prvTgfDevNum);

    /* restore MC Group entry */
    rc = prvTgfBrgMcEntryWrite(prvTgfDevNum, PRV_TGF_DEF_VIDX_CNS,
                               &(prvTgfRestoreCfg.defPortMembers));
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgMcEntryWrite: %d, %d",
                                 prvTgfDevNum, PRV_TGF_DEF_VIDX_CNS);

    /* delete VLANs */
    for (vlanIter = 0; vlanIter < PRV_TGF_VLAN_COUNT_CNS; vlanIter++)
    {
        /* invalidate vlan entry */
        rc = prvTgfBrgDefVlanEntryInvalidate((GT_U16)(PRV_TGF_VLANID_5_CNS + vlanIter));
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d, %d",
                                     prvTgfDevNum, PRV_TGF_VLANID_5_CNS + vlanIter);
    }
}

