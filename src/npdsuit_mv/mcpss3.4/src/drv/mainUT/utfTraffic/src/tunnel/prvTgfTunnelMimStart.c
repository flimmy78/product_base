/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelMimStart.c
*
* DESCRIPTION:
*       Tunnel Start: Mac In Mac Functionality
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
#include <common/tgfTunnelGen.h>
#include <tunnel/prvTgfTunnelMimStart.h>

/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* max iteration's name length */
#define PRV_TGF_MAX_ITERATION_NAME_LEN_CNS 100

/* number of send traffic iteration */
#define PRV_TGF_MAX_ITER_CNS              1

/* default VLAN Id */
#define PRV_TGF_SEND_VLANID_CNS           5

/* nextHop VLAN Id */
#define PRV_TGF_NEXTHOPE_VLANID_CNS       6

/* port number to send traffic to in default VLAN */
#define PRV_TGF_SEND_PORT_IDX_CNS         0

/* port number of unused port in default VLAN */
#define PRV_TGF_FDB_PORT_IDX_CNS          1

/* port number to send traffic to */
#define PRV_TGF_FREE_PORT_IDX_CNS         2

/* port number to receive traffic from in nextHop VLAN */
#define PRV_TGF_RECEIVE_PORT_IDX_CNS      3

#define PRV_UTF_LOG_VERIFY_GT_OK(rc, name)                                   \
    if (GT_OK != (rc))                                                       \
    {                                                                        \
        PRV_UTF_LOG2_MAC("\n[TGF]: %s FAILED, rc = [%d]\n", name, rc);       \
    }

/************************ common parts of packets **************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacket1L2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x34, 0x02},                /* daMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x22}                 /* saMac */
};
/* ethertype part of packet1 */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacket1EtherTypePart = {
    TGF_ETHERTYPE_0800_IPV4_TAG_CNS
};
/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacket1Ipv4Part = {
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x32,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    0xFF,               /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};
/* DATA of packet */
static GT_U8 prvTgfPacket1PayloadDataArr[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D
};
/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacket1PayloadPart = {
    sizeof(prvTgfPacket1PayloadDataArr),                 /* dataLength */
    prvTgfPacket1PayloadDataArr                          /* dataPtr */
};

/************************* packet 1 (IPv4 packet) **************************/

/* PARTS of packet 1 */
static TGF_PACKET_PART_STC prvTgfPacket1PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part}, /* type, partPtr */
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacket1EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacket1Ipv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacket1PayloadPart}
};
/* PACKET 1 to send */
static TGF_PACKET_STC prvTgfPacket1Info = {
    ( TGF_L2_HEADER_SIZE_CNS
    + TGF_ETHERTYPE_SIZE_CNS + TGF_IPV4_HEADER_SIZE_CNS
    + sizeof(prvTgfPacket1PayloadDataArr)),                       /* totalLen */
    sizeof(prvTgfPacket1PartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacket1PartArray                                        /* partsArray */
};

/**************************** packet MPLS **********************************/

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketMplsVlanTagPart = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_SEND_VLANID_CNS                       /* pri, cfi, VlanId */
};
/* MPLS ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketMplsMplsEtherTypePart = {
    TGF_ETHERTYPE_8847_MPLS_TAG_CNS
};
/* MPLS part */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsMplsPart = {
    0x66,               /* label */
    0,                  /* exp */
    1,                  /* stack */
    0xFF                /* timeToLive */
};

/* PARTS of packet MPLS */
static TGF_PACKET_PART_STC prvTgfPacketMplsPartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part}, /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketMplsVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketMplsMplsEtherTypePart},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsMplsPart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacket1Ipv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacket1PayloadPart}
};
/* PACKET MPLS to send */
static TGF_PACKET_STC prvTgfPacketMplsInfo = {
    ( TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS
    + TGF_ETHERTYPE_SIZE_CNS + TGF_MPLS_HEADER_SIZE_CNS
    + TGF_IPV4_HEADER_SIZE_CNS
    + sizeof(prvTgfPacket1PayloadDataArr)),                          /* totalLen */
    sizeof(prvTgfPacketMplsPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketMplsPartArray                                        /* partsArray */
};

/********************* packet MPLS with 3 labels ***************************/

/* MPLS 3 part */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsMpls3Part = {
    0x33,               /* label */
    0,                  /* exp */
    0,                  /* stack */
    0xdd                /* timeToLive */
};
/* MPLS 2 part */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsMpls2Part = {
    0x22,               /* label */
    0,                  /* exp */
    0,                  /* stack */
    0xee                /* timeToLive */
};
/* MPLS 1 part */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsMpls1Part = {
    0x11,               /* label */
    0,                  /* exp */
    1,                  /* stack */
    0xFF                /* timeToLive */
};

/* PARTS of packet MPLS 3 */
static TGF_PACKET_PART_STC prvTgfPacketMpls3PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part}, /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketMplsVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketMplsMplsEtherTypePart},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsMpls3Part},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsMpls2Part},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsMpls1Part},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacket1Ipv4Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacket1PayloadPart}
};
/* PACKET MPLS 3 to send */
static TGF_PACKET_STC prvTgfPacketMpls3Info = {
    ( TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS
    + TGF_ETHERTYPE_SIZE_CNS + TGF_MPLS_HEADER_SIZE_CNS
    + TGF_MPLS_HEADER_SIZE_CNS + TGF_MPLS_HEADER_SIZE_CNS
    + TGF_IPV4_HEADER_SIZE_CNS
    + sizeof(prvTgfPacket1PayloadDataArr)),                           /* totalLen */
    sizeof(prvTgfPacketMpls3PartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketMpls3PartArray                                        /* partsArray */
};

/******************************************************************************\
 *                            Private variables                               *
\******************************************************************************/

/* MAC for Tunnel Start Entry */
static TGF_MAC_ADDR prvTgfStartEntryMac = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x11
};

static const GT_CHAR prvTgfIterationNameArray[][PRV_TGF_MAX_ITERATION_NAME_LEN_CNS] = {
    "sending packet to port [%d] ...\n"
};
static TGF_PACKET_STC *prvTgfPacketInfoArray[] = {
    &prvTgfPacket1Info
};

/* stored default Vlan ID */
static GT_U16   prvTgfDefVlanId = 0;

/* PCL Rule index */
static GT_U32   prvTgfPclRuleIndex = 1;

/* TTI Rule index */
static GT_U32   prvTgfTtiRuleIndex = 1;

/* the Arp Address index of the Router ARP Table
 * to write to the UC Route entry Arp nextHopARPPointer field */
static GT_U32   prvTgfRouterArpIndex = 1;

/* line index for the tunnel start entry in the router ARP / tunnel start table (0..1023) */
static GT_U32   prvTgfTunnelStartLineIndex = 8;

/* define expected number of received packets (Sent packets) */
static GT_U8 prvTgfPacketsCountRxArray[][PRV_TGF_PORTS_NUM_CNS] = {
/* ports:  SEND VLN1 VLN2 RECEIVE */
             {1,  0,  0,  1}
};

/* define expected number of transmitted packets */
static GT_U8 prvTgfPacketsCountTxArray[][PRV_TGF_PORTS_NUM_CNS] = {
/* ports:  SEND VLN1 VLN2 RECEIVE */
             {1,  0,  0,  1}
};

/*************************** Restore config ***********************************/

/* parameters that is needed to be restored */
static struct
{
    PRV_TGF_TTI_MAC_MODE_ENT        macMode;
} prvTgfRestoreCfg;

/******************************************************************************/


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfCountersIpReset
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfCountersIpReset
(
    GT_VOID
)
{
    GT_U32       portIter;
    GT_STATUS rc = GT_OK;

    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        /* reset IP couters and set CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E mode */
        rc = prvTgfCountersIpSet(prvTgfPortsArray[portIter], portIter);
        PRV_UTF_VERIFY_GT_OK(rc, "prvTgfCountersIpSet");
    }

    return rc;
};

/*******************************************************************************
* prvTgfCountersIpCheck
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfCountersIpCheck
(
    GT_VOID
)
{
    GT_STATUS                  rc = GT_OK;
    GT_U32                     portIter;
    PRV_TGF_IP_COUNTER_SET_STC ipCounters;

    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        PRV_UTF_LOG1_MAC("IP counters for Port [%d]:\n", prvTgfPortsArray[portIter]);

        /* check IP counters */
        rc = prvTgfCountersIpGet(prvTgfDevNum, portIter, GT_TRUE, &ipCounters);
        PRV_UTF_VERIFY_GT_OK(rc, "prvTgfCountersIpGet");
    }

    PRV_UTF_LOG0_MAC("\n");

    return rc;
};

/*******************************************************************************
* prvTgfPortCaptureEnable
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfPortCaptureEnable
(
    IN GT_U8   portNum,
    IN GT_BOOL start
)
{
    GT_STATUS               rc = GT_OK;
    CPSS_INTERFACE_INFO_STC portInterface;

    /* setup nexthope portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = portNum;

    /* enable capture on nexthope port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, start);
    PRV_UTF_VERIFY_GT_OK(rc, "tgfTrafficGeneratorPortTxEthCaptureSet");

    return rc;
};

/*******************************************************************************
* prvTgfPortCapturePacketPrint
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfPortCapturePacketPrint
(
    IN GT_U8        portNum,
    IN TGF_MAC_ADDR macPattern,
    OUT GT_U32     *numTriggersPtr
)
{
    GT_STATUS               rc = GT_OK;
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    CPSS_NULL_PTR_CHECK_MAC(numTriggersPtr);
    *numTriggersPtr = 0;

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portNum);

    /* set nexthope portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = portNum;

    /* set pattern as MAC to select captured packets */
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    vfdArray[0].cycleCount = sizeof(TGF_MAC_ADDR);
    cpssOsMemCpy(vfdArray[0].patternPtr, macPattern, sizeof(TGF_MAC_ADDR));

    /* check TriggerCounters */
    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(&portInterface, 1, vfdArray, numTriggersPtr);
    rc = rc == GT_NO_MORE ? GT_OK : rc;

    return rc;
};

/*******************************************************************************
* prvTgfVlanSet
*
* DESCRIPTION:
*       Set VLAN entry
*
* INPUTS:
*       vlanId           - vlanId to be configured
*       portsMembers     - VLAN's ports Members
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfVlanSet
(
    IN GT_U16          vlanId,
    IN CPSS_PORTS_BMP_STC portsMembers
)
{
    GT_STATUS                          rc = GT_OK;
    PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    CPSS_PORTS_BMP_STC                 portsTagging;
    PRV_TGF_BRG_VLAN_INFO_STC          vlanInfo;

    /* clear ports tagging */
    cpssOsMemSet(&portsTaggingCmd, 0, sizeof(PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC));
    portsTagging.ports[0] = 0;
    portsTagging.ports[1] = 0;

    /* set vlan entry */
    cpssOsMemSet(&vlanInfo, 0, sizeof(vlanInfo));
    vlanInfo.autoLearnDisable = GT_TRUE; /* working in controlled learning */
    vlanInfo.naMsgToCpuEn     = GT_TRUE; /* working in controlled learning */

    /* set vlan entry */
    rc = prvTgfBrgVlanEntryWrite(prvTgfDevNum,
                                 vlanId,
                                 &portsMembers,
                                 &portsTagging,
                                 &vlanInfo,
                                 &portsTaggingCmd);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgVlanEntryWrite");

    return rc;
};

/*******************************************************************************
* prvTgfDefBrigeConfSet
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfDefBrigeConfSet
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    CPSS_PORTS_BMP_STC              vlanMembers = {{0, 0}};

    /* clear entry */
    cpssOsMemSet(&vlanMembers, 0, sizeof(vlanMembers));

    /* ports 0, 8 are VLAN_1 Members */
    vlanMembers.ports[prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS] >> 5] |=
        1 << (prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS] & 0x1f);
    vlanMembers.ports[prvTgfPortsArray[PRV_TGF_FDB_PORT_IDX_CNS] >> 5] |=
        1 << (prvTgfPortsArray[PRV_TGF_FDB_PORT_IDX_CNS] & 0x1f);

    prvTgfVlanSet(PRV_TGF_SEND_VLANID_CNS, vlanMembers);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfVlanSet");

    /* clear entry */
    cpssOsMemSet(&vlanMembers, 0, sizeof(vlanMembers));

    /* ports 18, 23 are VLAN_2 Members */
    vlanMembers.ports[prvTgfPortsArray[PRV_TGF_FREE_PORT_IDX_CNS] >> 5] |=
        1 << (prvTgfPortsArray[PRV_TGF_FREE_PORT_IDX_CNS] & 0x1f);
    vlanMembers.ports[prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS] >> 5] |=
        1 << (prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS] & 0x1f);

    prvTgfVlanSet(PRV_TGF_NEXTHOPE_VLANID_CNS, vlanMembers);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfVlanSet");

    /* get default vlanId */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfDefVlanId);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgVlanPortVidGet");

    /* set default vlanId */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], PRV_TGF_SEND_VLANID_CNS);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgVlanPortVidSet");

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgFdbFlush");

    /* create a macEntry for original packet 1 in VLAN 5 */
    rc = prvTgfBrgDefFdbMacEntryOnPortSet(
            prvTgfPacket1L2Part.daMac, PRV_TGF_SEND_VLANID_CNS, prvTgfDevNum,
            prvTgfPortsArray[PRV_TGF_FDB_PORT_IDX_CNS], GT_TRUE);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgDefFdbMacEntryOnPortSet");

    /* create a macEntry for tunneled packet 1 in VLAN 6 */
    rc = prvTgfBrgDefFdbMacEntryOnPortSet(
            prvTgfPacket1L2Part.daMac, PRV_TGF_NEXTHOPE_VLANID_CNS, prvTgfDevNum,
            prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfBrgDefFdbMacEntryOnPortSet");

    return rc;
};

/*******************************************************************************
* prvTgfConfigurationRestore
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConfigurationRestore
(
    IN GT_BOOL isPclSet,
    IN GT_BOOL isTtiSet
)
{
    GT_STATUS rc = GT_OK;

    /* invalidate PCL rules */
    if (isPclSet)
    {
        rc = prvTgfPclRuleValidStatusSet(CPSS_PCL_RULE_SIZE_STD_E, prvTgfPclRuleIndex, GT_FALSE);
        PRV_UTF_LOG_VERIFY_GT_OK(rc, "prvTgfPclRuleValidStatusSet");
        prvTgfPclRestore();
    }

    /* invalidate TTI rules */
    if (isTtiSet)
    {
        rc = prvTgfTtiRuleValidStatusSet(prvTgfTtiRuleIndex, GT_FALSE);
        PRV_UTF_LOG_VERIFY_GT_OK(rc, "prvTgfTtiRuleValidStatusSet");
    }

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    PRV_UTF_LOG_VERIFY_GT_OK(rc, "tgfTrafficTableRxPcktTblClear");

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    PRV_UTF_LOG_VERIFY_GT_OK(rc, "prvTgfBrgFdbFlush");

    /* restore default vlanId */
    rc = prvTgfBrgVlanPvidSet(prvTgfDefVlanId);
    PRV_UTF_LOG_VERIFY_GT_OK(rc, "prvTgfBrgVlanPvidSet");

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_SEND_VLANID_CNS);
    PRV_UTF_LOG_VERIFY_GT_OK(rc, "prvTgfBrgDefVlanEntryInvalidate");

    return rc;
};

/*******************************************************************************
* prvTgfTrafficGenerate
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    sendIter;

    /* sending packets */
    for (sendIter = 0; sendIter < PRV_TGF_MAX_ITER_CNS; sendIter++)
    {
        PRV_UTF_LOG1_MAC(
            prvTgfIterationNameArray[sendIter],
            prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

        /* setup transmit params */
        rc = prvTgfSetTxSetupEth(
            prvTgfDevNum, prvTgfPacketInfoArray[sendIter],
            1 /*burstCount*/, 0 /*numVfd*/, NULL);
        PRV_UTF_VERIFY_GT_OK(rc, "prvTgfSetTxSetupEth");

        /* start transmitting */
        rc = prvTgfStartTransmitingEth(
            prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
        PRV_UTF_VERIFY_GT_OK(rc, "prvTgfStartTransmitingEth");
    }

    return rc;
};

/*******************************************************************************
* prvTgfDefPclRuleSet
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfDefPclRuleSet
(
    GT_VOID
)
{
    GT_STATUS                   rc = GT_OK;
    PRV_TGF_PCL_RULE_FORMAT_UNT mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT patt;
    PRV_TGF_PCL_ACTION_STC      action;

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInit(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
        CPSS_PCL_DIRECTION_INGRESS_E,
        CPSS_PCL_LOOKUP_0_E,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E /*nonIpKey*/,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E /*ipv4Key*/,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E /*ipv6Key*/);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfPclDefPortInit");

    /* difine mask, pattern and action */
    cpssOsMemSet(&mask, 0, sizeof(mask));
    cpssOsMemSet(mask.ruleStdIpL2Qos.macDa.arEther, 0xFF,
            sizeof(mask.ruleStdIpL2Qos.macDa.arEther));

    cpssOsMemSet(&patt, 0, sizeof(patt));
    cpssOsMemCpy(patt.ruleStdIpL2Qos.macDa.arEther, prvTgfPacket1L2Part.daMac,
            sizeof(prvTgfPacket1L2Part.daMac));

    cpssOsMemSet(&action, 0, sizeof(action));
    action.pktCmd                                           = CPSS_PACKET_CMD_FORWARD_E;
    action.redirect.redirectCmd                             = PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
    action.redirect.data.outIf.outInterface.type            = CPSS_INTERFACE_PORT_E;
    action.redirect.data.outIf.outInterface.devPort.devNum  = prvTgfDevNum;
    action.redirect.data.outIf.outInterface.devPort.portNum =
        prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];
    action.redirect.data.outIf.outlifType = PRV_TGF_OUTLIF_TYPE_TUNNEL_E;
    action.redirect.data.outIf.outlifPointer.tunnelStartPtr.tunnelType =
        PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E;
    action.redirect.data.outIf.outlifPointer.tunnelStartPtr.ptr = prvTgfTunnelStartLineIndex;

    /* set PCL rule */
    rc = prvTgfPclRuleSet(
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
        prvTgfPclRuleIndex, &mask, &patt, &action);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfPclRuleSet");

    return rc;
};

/*******************************************************************************
* prvTgfTtiEnable
*
* DESCRIPTION:
*       This function enables the TTI lookup
*       and sets the lookup Mac mode
*       for the specified key type
*
* INPUTS:
*       portNum - port number
*       ttiKeyType - TTI key type
*       macMode - MAC mode to use
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTtiEnable
(
    IN  GT_U8                    portNum,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT ttiKeyType,
    IN  PRV_TGF_TTI_MAC_MODE_ENT macMode,
    IN  GT_BOOL                  isTtiEnable
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    ethType;

    if (GT_TRUE == isTtiEnable)
    {
        /* set MIM EtherType */
        ethType = TGF_ETHERTYPE_88E7_MIM_TAG_CNS;

        /* save current Eth TTI key lookup MAC mode */
        rc = prvTgfTtiMacModeGet(prvTgfDevNum, ttiKeyType, &prvTgfRestoreCfg.macMode);
        PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiMacModeGet");
    }
    else
    {
        /* restore default MIM EtherType */
        ethType = 0;

        /* restore Eth TTI key lookup MAC mode */
        macMode = prvTgfRestoreCfg.macMode;
    }

    /* set MIM EtherType */
    rc = prvTgfTtiMimEthTypeSet(ethType);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiMimEthTypeSet");

    /* enable/disable port for ttiKeyType TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(portNum, ttiKeyType, isTtiEnable);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiPortLookupEnableSet");

    /* set TTI lookup MAC mode */
    rc = prvTgfTtiMacModeSet(ttiKeyType, macMode);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiMacModeSet");

    return rc;
};

/*******************************************************************************
* prvTgfDefTtiEthRuleSet
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfDefTtiEthRuleSet
(
    GT_VOID
)
{
    GT_STATUS              rc = GT_OK;
    PRV_TGF_TTI_ACTION_STC ttiAction;
    PRV_TGF_TTI_RULE_UNT   ttiPattern;
    PRV_TGF_TTI_RULE_UNT   ttiMask;

    /* cpssDxChTtiRuleSet(CPSS_DXCH_TTI_ETH_RULE_STC, CPSS_DXCH_TTI_ACTION_2_STC) */

    /* set action - redirect to port 23 */
    cpssOsMemSet(&ttiAction, 0, sizeof(ttiAction));
    ttiAction.tunnelTerminate               = GT_FALSE;
    ttiAction.command                       = CPSS_PACKET_CMD_FORWARD_E;
    ttiAction.redirectCommand               = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
    ttiAction.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    ttiAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;
    ttiAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];
    ttiAction.tag0VlanCmd                   = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ttiAction.tag0VlanId                    = PRV_TGF_NEXTHOPE_VLANID_CNS;
    ttiAction.tag1VlanCmd                   = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ttiAction.tunnelStart                   = GT_TRUE;
    ttiAction.tunnelStartPtr                = prvTgfTunnelStartLineIndex;

    /* set TTI Rule */
    cpssOsMemSet(&ttiPattern, 0, sizeof(ttiPattern));
    cpssOsMemCpy(ttiPattern.eth.common.mac.arEther, prvTgfPacket1L2Part.daMac, sizeof(TGF_MAC_ADDR));
    ttiPattern.eth.common.vid      = PRV_TGF_SEND_VLANID_CNS;
    ttiPattern.eth.common.pclId    = 3;

    cpssOsMemSet(&ttiMask, 0, sizeof(ttiMask));
    cpssOsMemSet(ttiMask.eth.common.mac.arEther, 0xFF, sizeof(TGF_MAC_ADDR));
    ttiMask.eth.common.vid         = 0x0F;
    ttiMask.eth.common.pclId       = 0x3FF;

    rc = prvTgfTtiRuleSet(prvTgfTtiRuleIndex, PRV_TGF_TTI_KEY_ETH_E,
                          &ttiPattern, &ttiMask, &ttiAction);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiRuleSet");

    /* debug - Check TTI Rule */
    cpssOsMemSet(&ttiPattern, 0, sizeof(ttiPattern));
    cpssOsMemSet(&ttiMask, 0, sizeof(ttiMask));
    cpssOsMemSet(&ttiAction, 0, sizeof(ttiAction));

    rc = prvTgfTtiRuleGet(prvTgfDevNum, prvTgfTtiRuleIndex, PRV_TGF_TTI_KEY_ETH_E,
                          &ttiPattern, &ttiMask, &ttiAction);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiRuleGet");

    return rc;
};

/*******************************************************************************
* prvTgfDefTtiMplsPushRuleSet
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfDefTtiMplsPushRuleSet
(
    IN PRV_TGF_TUNNEL_MPLS_LSR_PUSH_TYPE_ENT pushType
)
{
    GT_STATUS              rc = GT_OK;
    PRV_TGF_TTI_ACTION_STC ttiAction;
    PRV_TGF_TTI_RULE_UNT   ttiPattern;
    PRV_TGF_TTI_RULE_UNT   ttiMask;

    /* cpssDxChTtiRuleSet(CPSS_DXCH_TTI_MPLS_RULE_STC, CPSS_DXCH_TTI_ACTION_2_STC) */

    /* clear action */
    cpssOsMemSet(&ttiAction, 0, sizeof(ttiAction));

    /* set action MPLS command */
    switch(pushType) {
    case PRV_TGF_TUNNEL_MPLS_LSR_SWAP_E:
        ttiAction.mplsCmd = PRV_TGF_TTI_MPLS_SWAP_COMMAND_E;
        break;
    case PRV_TGF_TUNNEL_MPLS_LSR_POP_SWAP_E:
        ttiAction.mplsCmd = PRV_TGF_TTI_MPLS_POP_AND_SWAP_COMMAND_E;
        break;
    default:
        ttiAction.mplsCmd = PRV_TGF_TTI_MPLS_PUSH1_LABEL_COMMAND_E;
        break;
    }

    /* set action - redirect to port 23 */
    ttiAction.tunnelTerminate               = GT_FALSE;
    ttiAction.passengerPacketType           = PRV_TGF_TTI_PASSENGER_MPLS_E;
    ttiAction.tsPassengerPacketType         = PRV_TGF_TUNNEL_PASSENGER_OTHER_E;
    ttiAction.command                       = CPSS_PACKET_CMD_FORWARD_E;
    ttiAction.redirectCommand               = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
    ttiAction.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    ttiAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;
    ttiAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];
    ttiAction.tag0VlanCmd                   = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ttiAction.tag0VlanId                    = 6;
    ttiAction.tag1VlanCmd                   = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;
    ttiAction.tag1VlanId                    = 0;
    ttiAction.keepPreviousQoS               = GT_TRUE;
    ttiAction.tunnelStart                   = GT_TRUE;
    ttiAction.tunnelStartPtr                = prvTgfTunnelStartLineIndex;

    /* mask and pattern - mpls labels */
    cpssOsMemSet(&ttiMask, 0, sizeof(ttiMask));
    cpssOsMemSet(ttiMask.mpls.common.mac.arEther, 0xFF, sizeof(TGF_MAC_ADDR));
    ttiMask.mpls.common.vid         = 0x0F;  /* 4095 (0xFFF) in RDE test */
    ttiMask.mpls.common.pclId       = 0x3FF; /* 4095 (0xFFF) in RDE test */
    ttiMask.mpls.numOfLabels        = 0xFF;  /* 3 in RDE test */
    ttiMask.mpls.label0             = 0xFF;  /* 1048575 (0xFFFFF) in RDE test */
    ttiMask.mpls.common.isTagged    = GT_TRUE;
    ttiMask.mpls.exp0               = 7;

    cpssOsMemSet(&ttiPattern, 0, sizeof(ttiPattern));
    cpssOsMemCpy(ttiPattern.mpls.common.mac.arEther, prvTgfPacket1L2Part.daMac, sizeof(TGF_MAC_ADDR));
    ttiPattern.mpls.common.vid      = PRV_TGF_SEND_VLANID_CNS;
    ttiPattern.mpls.common.pclId    = 2;
    ttiPattern.mpls.common.isTagged = GT_TRUE;
    ttiPattern.mpls.exp0            = 0;

    /* set labels */
    switch(pushType) {
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_1_LABEL_E:
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_2_LABEL_E:
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_3_LABEL_E:
        ttiPattern.mpls.numOfLabels     = 0;
        ttiPattern.mpls.label0          = 0x66;
        break;
    case PRV_TGF_TUNNEL_MPLS_LSR_SWAP_E:
    case PRV_TGF_TUNNEL_MPLS_LSR_POP_SWAP_E:
        ttiPattern.mpls.numOfLabels     = 2;
        ttiMask.mpls.label1             = 0xFF;
        ttiMask.mpls.label2             = 0xFF;
        ttiPattern.mpls.label0          = 0x33;
        ttiPattern.mpls.label1          = 0x22;
        ttiPattern.mpls.label2          = 0x11;
        break;
    default:
        break;
    }

    /* set TTI Rule */
    rc = prvTgfTtiRuleSet(prvTgfTtiRuleIndex, PRV_TGF_TTI_KEY_MPLS_E,
                          &ttiPattern, &ttiMask, &ttiAction);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiRuleSet");

    /* debug - Check TTI Rule */
    cpssOsMemSet(&ttiPattern, 0, sizeof(ttiPattern));
    cpssOsMemSet(&ttiMask, 0, sizeof(ttiMask));
    cpssOsMemSet(&ttiAction, 0, sizeof(ttiAction));

    rc = prvTgfTtiRuleGet(prvTgfDevNum, prvTgfTtiRuleIndex, PRV_TGF_TTI_KEY_MPLS_E,
                          &ttiPattern, &ttiMask, &ttiAction);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiRuleGet");

    return rc;
};

/*******************************************************************************
* prvTgfDefTtiMplsPopRuleSet
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
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfDefTtiMplsPopRuleSet
(
    IN PRV_TGF_TUNNEL_MPLS_LSR_POP_TYPE_ENT popType
)
{
    GT_STATUS              rc = GT_OK;
    PRV_TGF_TTI_ACTION_STC ttiAction;
    PRV_TGF_TTI_RULE_UNT   ttiPattern;
    PRV_TGF_TTI_RULE_UNT   ttiMask;

    /* cpssDxChTtiRuleSet(CPSS_DXCH_TTI_MPLS_RULE_STC, CPSS_DXCH_TTI_ACTION_2_STC) */

    /* clear action */
    cpssOsMemSet(&ttiAction, 0, sizeof(ttiAction));

    /* set action MPLS command */
    if(PRV_TGF_TUNNEL_MPLS_LSR_POP_2_LABEL_E == popType)
        ttiAction.mplsCmd                   = PRV_TGF_TTI_MPLS_POP2_LABELS_COMMAND_E;
    else
        ttiAction.mplsCmd                   = PRV_TGF_TTI_MPLS_POP1_LABEL_COMMAND_E;

    /* set action - redirect to port 23 */
    ttiAction.tunnelTerminate               = GT_FALSE;
    ttiAction.passengerPacketType           = PRV_TGF_TTI_PASSENGER_MPLS_E;
    ttiAction.tsPassengerPacketType         = PRV_TGF_TUNNEL_PASSENGER_OTHER_E;
    ttiAction.command                       = CPSS_PACKET_CMD_FORWARD_E;
    ttiAction.redirectCommand               = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
    ttiAction.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    ttiAction.interfaceInfo.devPort.devNum  = prvTgfDevNum;
    ttiAction.interfaceInfo.devPort.portNum = prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];
    ttiAction.tag0VlanCmd                   = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ttiAction.tag0VlanId                    = 6;
    ttiAction.tag1VlanCmd                   = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;
    ttiAction.tag1VlanId                    = 0;
    ttiAction.keepPreviousQoS               = GT_TRUE;
    ttiAction.tunnelStart                   = GT_FALSE;
    ttiAction.tunnelStartPtr                = 0;
    ttiAction.arpPtr                        = prvTgfRouterArpIndex;

    /* mask and pattern - mpls labels */
    cpssOsMemSet(&ttiMask, 0, sizeof(ttiMask));
    cpssOsMemSet(ttiMask.mpls.common.mac.arEther, 0xFF, sizeof(TGF_MAC_ADDR));
    ttiMask.mpls.common.vid         = 0x0F;
    ttiMask.mpls.label0             = 0xFF;
    ttiMask.mpls.label1             = 0xFF;
    ttiMask.mpls.label2             = 0xFF;
    ttiMask.mpls.numOfLabels        = 0xFF;
    ttiMask.mpls.common.pclId       = 0x3FF;

    cpssOsMemSet(&ttiPattern, 0, sizeof(ttiPattern));
    cpssOsMemCpy(ttiPattern.mpls.common.mac.arEther, prvTgfPacket1L2Part.daMac, sizeof(TGF_MAC_ADDR));
    ttiPattern.mpls.common.vid      = PRV_TGF_SEND_VLANID_CNS;
    ttiPattern.mpls.label0          = 0x33;
    ttiPattern.mpls.label1          = 0x22;
    ttiPattern.mpls.label2          = 0x11;
    ttiPattern.mpls.numOfLabels     = 2;
    ttiPattern.mpls.common.pclId    = 2;

    /* set TTI Rule */
    rc = prvTgfTtiRuleSet(prvTgfTtiRuleIndex, PRV_TGF_TTI_KEY_MPLS_E,
                          &ttiPattern, &ttiMask, &ttiAction);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiRuleSet");

    /* debug - Check TTI Rule */
    cpssOsMemSet(&ttiPattern, 0, sizeof(ttiPattern));
    cpssOsMemSet(&ttiMask, 0, sizeof(ttiMask));
    cpssOsMemSet(&ttiAction, 0, sizeof(ttiAction));

    rc = prvTgfTtiRuleGet(prvTgfDevNum, prvTgfTtiRuleIndex, PRV_TGF_TTI_KEY_MPLS_E,
                          &ttiPattern, &ttiMask, &ttiAction);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiRuleGet");

    return rc;
};

/*******************************************************************************
* prvTgfDefTunnelStartEntryMimSet
*
* DESCRIPTION:
*       Set a default MacInMac tunnel start entry
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfDefTunnelStartEntryMimSet
(
    GT_VOID
)
{
    GT_STATUS                      rc = GT_OK;
    CPSS_TUNNEL_TYPE_ENT           tunnelType;
    PRV_TGF_TUNNEL_START_ENTRY_UNT tunnelEntry;

    /* set a Tunnel Start Entry
     * cpssDxChTunnelStartEntrySet(,,CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC)
     */
    cpssOsMemSet(&tunnelEntry, 0, sizeof(tunnelEntry));
    tunnelEntry.mimCfg.tagEnable        = GT_TRUE;
    tunnelEntry.mimCfg.vlanId           = PRV_TGF_NEXTHOPE_VLANID_CNS;
    tunnelEntry.mimCfg.upMarkMode       = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
    tunnelEntry.mimCfg.up               = 0x3;
    tunnelEntry.mimCfg.ttl              = 64;
    cpssOsMemCpy(tunnelEntry.mimCfg.macDa.arEther, prvTgfStartEntryMac, sizeof(TGF_MAC_ADDR));
    tunnelEntry.mimCfg.retainCrc        = GT_FALSE;
    tunnelEntry.mimCfg.iSid             = 0x12345;
    tunnelEntry.mimCfg.iSidAssignMode   = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
    tunnelEntry.mimCfg.iUp              = 0x5;
    tunnelEntry.mimCfg.iUpMarkMode      = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
    tunnelEntry.mimCfg.iDp              = 0;
    tunnelEntry.mimCfg.iDpMarkMode      = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;

    rc = prvTgfTunnelStartEntrySet(prvTgfTunnelStartLineIndex, CPSS_TUNNEL_MAC_IN_MAC_E, &tunnelEntry);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntrySet");

    /* debug - check Tunnel Start Entry - vlanId and macDa fields */
    cpssOsMemSet(&tunnelEntry, 0, sizeof(tunnelEntry));

    rc = prvTgfTunnelStartEntryGet(prvTgfDevNum, prvTgfTunnelStartLineIndex, &tunnelType, &tunnelEntry);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntryGet");

    /* check tunnelType */
    rc = (CPSS_TUNNEL_MAC_IN_MAC_E == tunnelType)? GT_OK : GT_FAIL;
    if (GT_OK != rc)
        PRV_UTF_LOG1_MAC("tunnelType: %d\n", tunnelType);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntryGet");

    /* check VLAN Id field */
    rc = (PRV_TGF_NEXTHOPE_VLANID_CNS == tunnelEntry.mimCfg.vlanId)? GT_OK : GT_FAIL;
    if (GT_OK != rc)
        PRV_UTF_LOG1_MAC("tunnelEntry.mimCfg.vlanId: %d\n", tunnelEntry.mimCfg.vlanId);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntryGet");

    /* check MAC DA field */
    rc = cpssOsMemCmp(tunnelEntry.mimCfg.macDa.arEther, prvTgfStartEntryMac, sizeof(TGF_MAC_ADDR)) == 0 ?
        GT_OK : GT_FAIL;
    if (GT_OK != rc)
        PRV_UTF_LOG1_MAC("tunnelEntry.mimCfg.macDa.arEther[5]: 0x%2X\n", tunnelEntry.mimCfg.macDa.arEther[5]);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntryGet");

    return rc;
};

/*******************************************************************************
* prvTgfDefTunnelStartEntryMplsSet
*
* DESCRIPTION:
*       Set a default X-over-MPLS tunnel start entry
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfDefTunnelStartEntryMplsSet
(
    IN PRV_TGF_TUNNEL_MPLS_LSR_PUSH_TYPE_ENT pushType
)
{
    GT_STATUS                      rc = GT_OK;
    CPSS_TUNNEL_TYPE_ENT           tunnelType;
    PRV_TGF_TUNNEL_START_ENTRY_UNT tunnelEntry;

    /* set a Tunnel Start Entry
     * cpssDxChTunnelStartEntrySet(,,CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC)
     */
    cpssOsMemSet(&tunnelEntry, 0, sizeof(tunnelEntry));
    tunnelEntry.mplsCfg.tagEnable        = GT_TRUE;
    tunnelEntry.mplsCfg.vlanId           = PRV_TGF_NEXTHOPE_VLANID_CNS;
    cpssOsMemCpy(tunnelEntry.mplsCfg.macDa.arEther, prvTgfStartEntryMac, sizeof(TGF_MAC_ADDR));
    tunnelEntry.mplsCfg.ttl              = 0xEE;
    tunnelEntry.mplsCfg.exp1             = 1;

    /* set labels */
    switch(pushType) {
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_1_LABEL_E:
        tunnelEntry.mplsCfg.numLabels    = 1;
        tunnelEntry.mplsCfg.label1       = 0x11;
        break;
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_2_LABEL_E:
        tunnelEntry.mplsCfg.numLabels    = 2;
        tunnelEntry.mplsCfg.label1       = 0x11;
        tunnelEntry.mplsCfg.label2       = 0x22;
        break;
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_3_LABEL_E:
        tunnelEntry.mplsCfg.numLabels    = 3;
        tunnelEntry.mplsCfg.label1       = 0x11;
        tunnelEntry.mplsCfg.label2       = 0x22;
        tunnelEntry.mplsCfg.label3       = 0x33;
        break;
    case PRV_TGF_TUNNEL_MPLS_LSR_SWAP_E:
    case PRV_TGF_TUNNEL_MPLS_LSR_POP_SWAP_E:
        tunnelEntry.mplsCfg.numLabels    = 1;
        tunnelEntry.mplsCfg.label1       = 0x66;
        break;
    default:
        break;
    }

    rc = prvTgfTunnelStartEntrySet(prvTgfTunnelStartLineIndex, CPSS_TUNNEL_X_OVER_MPLS_E, &tunnelEntry);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntrySet");

    /* debug - check Tunnel Start Entry - vlanId and macDa fields */
    cpssOsMemSet(&tunnelEntry, 0, sizeof(tunnelEntry));

    rc = prvTgfTunnelStartEntryGet(prvTgfDevNum, prvTgfTunnelStartLineIndex, &tunnelType, &tunnelEntry);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntryGet");

    /* check tunnelType */
    rc = (CPSS_TUNNEL_X_OVER_MPLS_E == tunnelType)? GT_OK : GT_FAIL;
    if (GT_OK != rc)
        PRV_UTF_LOG1_MAC("tunnelType: %d\n", tunnelType);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntryGet");

    /* check VLAN Id field */
    rc = (PRV_TGF_NEXTHOPE_VLANID_CNS == tunnelEntry.mplsCfg.vlanId)? GT_OK : GT_FAIL;
    if (GT_OK != rc)
        PRV_UTF_LOG1_MAC("tunnelEntry.mplsCfg.vlanId: %d\n", tunnelEntry.mplsCfg.vlanId);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntryGet");

    /* check MAC DA field */
    rc = cpssOsMemCmp(tunnelEntry.mplsCfg.macDa.arEther, prvTgfStartEntryMac, sizeof(TGF_MAC_ADDR)) == 0 ?
        GT_OK : GT_FAIL;
    if (GT_OK != rc)
        PRV_UTF_LOG1_MAC("tunnelEntry.mplsCfg.macDa.arEther[5]: 0x%2X\n", tunnelEntry.mplsCfg.macDa.arEther[5]);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTunnelStartEntryGet");

    return rc;
};

/*******************************************************************************
* prvTgfDefArpEntrySet
*
* DESCRIPTION:
*       Set a default ARP entry
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfDefArpEntrySet
(
    GT_VOID
)
{
    GT_STATUS                      rc = GT_OK;
    GT_ETHERADDR                   arpMacAddr;

    /* convert MAC address to other format */
    cpssOsMemCpy(arpMacAddr.arEther, prvTgfStartEntryMac, sizeof(TGF_MAC_ADDR));

    /* write a ARP MAC address to the Router ARP Table */
    rc = prvTgfIpRouterArpAddrWrite(prvTgfRouterArpIndex, &arpMacAddr);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfIpRouterArpAddrWrite");

    return rc;
};

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfTunnelMimStart
*
* DESCRIPTION:
*       MIM Tunnel Start
*
* INPUTS:
*       mimSourceType - Type of MAC in MAC Test
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
GT_VOID prvTgfTunnelMimStart
(
    IN PRV_TGF_TUNNEL_MIM_START_TYPE_ENT startType
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    GT_BOOL   isPclSet = GT_FALSE;
    GT_BOOL   isTtiSet = GT_FALSE;
    GT_U32    numTriggers = 0;
    PRV_TGF_IP_ROUTING_MODE_ENT     routingMode;

    PRV_UTF_LOG1_MAC("\nVLAN %d CONFIGURATION:\n", PRV_TGF_SEND_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  Port members: [%d], [%d]\n",
            prvTgfPortsArray[0], prvTgfPortsArray[1]);

    PRV_UTF_LOG1_MAC("VLAN %d CONFIGURATION:\n", PRV_TGF_NEXTHOPE_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  Port members: [%d], [%d]\n",
            prvTgfPortsArray[2], prvTgfPortsArray[3]);

    PRV_UTF_LOG0_MAC("FDB CONFIGURATION:\n");
    PRV_UTF_LOG1_MAC("  MAC DA ...:34:02 to port [%d]\n", prvTgfPortsArray[1]);
    PRV_UTF_LOG1_MAC("  MAC DA ...:00:11 to port [%d]\n", prvTgfPortsArray[3]);

    PRV_UTF_LOG0_MAC("PCL CONFIGURATION:\n");
    if(PRV_TGF_TUNNEL_MIM_START_FROM_PCL_E == startType)
        PRV_UTF_LOG0_MAC("  forward the packet to tunnel start egress interface.\n");
    else
        PRV_UTF_LOG0_MAC("  CLEAR\n");

    PRV_UTF_LOG0_MAC("TTI CONFIGURATION:\n");
    PRV_UTF_LOG2_MAC("  FORWARD packet with MAC DA ..:34:02 to port [%d]\n"
            "  in VLAN %d with MIM tunnel (MAC DA must be ...:00:11)\n",
            prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], PRV_TGF_NEXTHOPE_VLANID_CNS);

    PRV_UTF_LOG0_MAC("TRAFFIC GENERATION:\n");
    PRV_UTF_LOG1_MAC("  Sending packet to port [%d]\n",
            prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    PRV_UTF_LOG0_MAC("EXPECTED RESULTS:\n");
    PRV_UTF_LOG1_MAC("  packet is not forwarded in VLAN %d\n",
            PRV_TGF_SEND_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  packet is forwarded to port [%d] VLAN %d with MIM tunnel\n\n",
            prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], PRV_TGF_NEXTHOPE_VLANID_CNS);

    /* -------------------------------------------------------------------------
     * 1. Set configuration
     */

    /* set VLANs */
    rc = prvTgfDefBrigeConfSet();
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefBrigeConfSet: %d", prvTgfDevNum);

    /* set PCL rules */
    if(PRV_TGF_TUNNEL_MIM_START_FROM_PCL_E == startType && GT_OK == rc)
    {
        rc = prvTgfDefPclRuleSet();
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfDefPclRuleSet: %d", prvTgfDevNum);
        isPclSet = (GT_OK == rc);
    }

    /* enable TTI lookup for Eth key and MAC DA mode */
    if (GT_OK == rc) {
        rc = prvTgfTtiEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                             PRV_TGF_TTI_KEY_ETH_E,
                             PRV_TGF_TTI_MAC_MODE_DA_E,
                             GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiEnable: %d", prvTgfDevNum);
    }

    /* set TTI rules for Eth key */
    if (GT_OK == rc) {
        rc = prvTgfDefTtiEthRuleSet();
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefTtiEthRuleSet: %d", prvTgfDevNum);
        isTtiSet = (GT_OK == rc);
    }

    /* set a MIM Tunnel Start Entry */
    if (GT_OK == rc) {
        rc = prvTgfDefTunnelStartEntryMimSet();
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefTunnelStartEntryMimSet: %d", prvTgfDevNum);
    }

    /* -------------------------------------------------------------------------
     * 2. Generate traffic and Check counters
     */

    if (rc == GT_OK)
    {
        /* get routing mode */
        rc = prvTgfIpRoutingModeGet(&routingMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRoutingModeGet: %d", prvTgfDevNum);

        if (routingMode == PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E)
        {
            /* reset IP counters */
            rc = prvTgfCountersIpReset();
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfCountersIpReset: %d", prvTgfDevNum);
        }

        /* reset ETH counters */
        rc = prvTgfEthCountersReset(prvTgfDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* select IPv4 packet to send */
        prvTgfPacketInfoArray[0] = &prvTgfPacket1Info;

        /* enable capturing of receive port */
        rc = prvTgfPortCaptureEnable(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* generate traffic */
        rc = prvTgfTrafficGenerate();
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTrafficGenerate: %d", prvTgfDevNum);

        /* disable capturing of receive port */
        rc = prvTgfPortCaptureEnable(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* check ETH counters */
        for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
        {
            if (PRV_TGF_RECEIVE_PORT_IDX_CNS == portIter)
            {
                /* packet after tunnel start in VLAN 6 */
                packetSize = TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + 2
                    + prvTgfPacketInfoArray[0]->totalLen + TGF_CRC_LEN_CNS;
            }
            else
            {
                /* original packet before tunneling in VLAN 5 */
                packetSize = prvTgfPacketInfoArray[0]->totalLen;
            }

            /* check ETH counters */
            rc = prvTgfEthCountersCheck(
                prvTgfDevNum,
                prvTgfPortsArray[portIter],
                prvTgfPacketsCountRxArray[0][portIter],
                prvTgfPacketsCountTxArray[0][portIter],
                packetSize, 1 /*burstCount*/);
            UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfEthCountersCheck");
        }

        if (routingMode == PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E)
        {
            /* check IP counters */
            rc = prvTgfCountersIpCheck();
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfEthCountersCheck: %d", prvTgfDevNum);
        }

        /* print captured packets from receive port with prvTgfStartEntryMac */
        rc = prvTgfPortCapturePacketPrint(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS],
                prvTgfStartEntryMac, &numTriggers);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
                                     "prvTgfPortCapturePacketPrint:"
                                     " port = %d, rc = 0x%02X\n",
                                     prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], rc);

        /* check if captured packet has the same MAC DA as prvTgfArpMac */
        UTF_VERIFY_EQUAL6_STRING_MAC(1, numTriggers,
                "\n   MAC DA of captured packet must be: %02X:%02X:%02X:%02X:%02X:%02X\n",
                prvTgfStartEntryMac[0], prvTgfStartEntryMac[1], prvTgfStartEntryMac[2],
                prvTgfStartEntryMac[3], prvTgfStartEntryMac[4], prvTgfStartEntryMac[5]);
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfPortCapturePacketPrint: %d", prvTgfDevNum);
    }

    /* -------------------------------------------------------------------------
     * 3. Restore configuration
     */

    /* restore TTI lookup and MAC DA mode for Eth key */
    rc = prvTgfTtiEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                         PRV_TGF_TTI_KEY_ETH_E,
                         PRV_TGF_TTI_MAC_MODE_DA_E,
                         GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiEnable: %d", prvTgfDevNum);

    /* restore common configuration */
    rc = prvTgfConfigurationRestore(isPclSet, isTtiSet);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfConfigurationRestore: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfTunnelMplsLsrPush
*
* DESCRIPTION:
*       MPLS LSR PUSH Functionality
*
* INPUTS:
*       pushType - Type of MPLS LSR PUSH Tunnel Start Test
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
GT_VOID prvTgfTunnelMplsPush
(
    IN PRV_TGF_TUNNEL_MPLS_LSR_PUSH_TYPE_ENT pushType
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    GT_BOOL   isTtiSet = GT_FALSE;
    GT_U32    numTriggers = 0;
    PRV_TGF_IP_ROUTING_MODE_ENT     routingMode;

    PRV_UTF_LOG1_MAC("\nVLAN %d CONFIGURATION:\n", PRV_TGF_SEND_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  Port members: [%d], [%d]\n",
            prvTgfPortsArray[0], prvTgfPortsArray[1]);

    PRV_UTF_LOG1_MAC("VLAN %d CONFIGURATION:\n", PRV_TGF_NEXTHOPE_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  Port members: [%d], [%d]\n",
            prvTgfPortsArray[2], prvTgfPortsArray[3]);

    PRV_UTF_LOG0_MAC("FDB CONFIGURATION:\n");
    PRV_UTF_LOG1_MAC("  MAC DA ...:34:02 to port [%d]\n", prvTgfPortsArray[1]);
    PRV_UTF_LOG1_MAC("  MAC DA ...:00:11 to port [%d]\n", prvTgfPortsArray[3]);

    PRV_UTF_LOG0_MAC("PCL CONFIGURATION:\n");
    PRV_UTF_LOG0_MAC("  CLEAR\n");

    PRV_UTF_LOG0_MAC("TTI CONFIGURATION:\n");
    PRV_UTF_LOG2_MAC("  FORWARD packet with MAC DA ..:34:02 to port [%d]\n"
            "  in VLAN %d with MIM tunnel (MAC DA must be ...:00:11)\n",
            prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], PRV_TGF_NEXTHOPE_VLANID_CNS);

    PRV_UTF_LOG0_MAC("TRAFFIC GENERATION:\n");
    PRV_UTF_LOG0_MAC("  Sending MPLS packet with\n");
    switch (pushType) {
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_1_LABEL_E:
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_2_LABEL_E:
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_3_LABEL_E:
        PRV_UTF_LOG0_MAC("   label_1=0x66\n"); break;
    case PRV_TGF_TUNNEL_MPLS_LSR_SWAP_E:
    case PRV_TGF_TUNNEL_MPLS_LSR_POP_SWAP_E:
        PRV_UTF_LOG0_MAC("   label_1=0x33\n   label_2=0x22\n   label_3=0x11\n"); break;
    default:
        break;
    }
    PRV_UTF_LOG1_MAC("   and MAC DA=...:34:02\n  to port [%d]\n",
            prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    PRV_UTF_LOG0_MAC("EXPECTED RESULTS:\n");
    PRV_UTF_LOG1_MAC("  packet is not forwarded in VLAN %d\n", PRV_TGF_SEND_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  packet is forwarded to port [%d] VLAN %d with ",
            prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], PRV_TGF_NEXTHOPE_VLANID_CNS);
    switch (pushType) {
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_1_LABEL_E:
        PRV_UTF_LOG0_MAC("2 labels:\n"
            "   label_0=0x11(outer label)\n   lebel_1=0x66\n"); break;
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_2_LABEL_E:
        PRV_UTF_LOG0_MAC("3 labels:\n"
            "   label_0=0x22(outer label)\n   label_1=0x11\n   lebel_2=0x66\n"); break;
    case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_3_LABEL_E:
        PRV_UTF_LOG0_MAC("4 labels:\n"
            "   label_0=0x33(outer label)\n   label_1=0x22\n   label_2=0x11\n"
            "   lebel_3=0x66\n"); break;
    case PRV_TGF_TUNNEL_MPLS_LSR_SWAP_E:
        PRV_UTF_LOG0_MAC("3 labels:\n"
            "   label_0=0x66(outer label)\n   label_1=0x22\n   lebel_2=0x11\n"); break;
    case PRV_TGF_TUNNEL_MPLS_LSR_POP_SWAP_E:
        PRV_UTF_LOG0_MAC("2 labels:\n"
            "   label_0=0x66(outer label)\n   lebel_2=0x11\n"); break;
    default:
        break;
    }
    PRV_UTF_LOG0_MAC("   and MAC DA=...:00:11\n\n");


    /* -------------------------------------------------------------------------
     * 1. Set configuration
     */

    /* set VLANs */
    rc = prvTgfDefBrigeConfSet();
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefBrigeConfSet: %d", prvTgfDevNum);

    /* enable TTI lookup for MPLS key and MAC DA mode */
    if (GT_OK == rc) {
        rc = prvTgfTtiEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                             PRV_TGF_TTI_KEY_MPLS_E,
                             PRV_TGF_TTI_MAC_MODE_DA_E,
                             GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiEnable: %d", prvTgfDevNum);
    }

    /* set TTI rules for MPLS Push key */
    if (GT_OK == rc) {
        rc = prvTgfDefTtiMplsPushRuleSet(pushType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefTtiMplsPushRuleSet: %d", prvTgfDevNum);
        isTtiSet = (GT_OK == rc);
    }

    /* set a X-over-MPLS Tunnel Start Entry */
    if (GT_OK == rc) {
        rc = prvTgfDefTunnelStartEntryMplsSet(pushType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefTunnelStartEntryMplsSet: %d", prvTgfDevNum);
    }


    /* -------------------------------------------------------------------------
     * 2. Generate traffic and Check counters
     */

    /* get routing mode */
    rc = prvTgfIpRoutingModeGet(&routingMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRoutingModeGet: %d", prvTgfDevNum);

    if (GT_OK == rc)
    {
        if (routingMode == PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E)
        {
            /* reset IP counters */
            rc = prvTgfCountersIpReset();
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfCountersIpReset: %d", prvTgfDevNum);
        }

        /* reset ETH counters */
        rc = prvTgfEthCountersReset(prvTgfDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* select MPLS packet to send */
        switch (pushType) {
        case PRV_TGF_TUNNEL_MPLS_LSR_SWAP_E:
        case PRV_TGF_TUNNEL_MPLS_LSR_POP_SWAP_E:
            prvTgfPacketInfoArray[0] = &prvTgfPacketMpls3Info;
            break;
        default:
            prvTgfPacketInfoArray[0] = &prvTgfPacketMplsInfo;
            break;
        }

        /* enable capturing of receive port */
        rc = prvTgfPortCaptureEnable(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* generate traffic */
        rc = prvTgfTrafficGenerate();
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTrafficGenerate: %d", prvTgfDevNum);

        /* disable capturing of receive port */
        rc = prvTgfPortCaptureEnable(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* check ETH counters */
        for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
        {
            if (PRV_TGF_RECEIVE_PORT_IDX_CNS == portIter)
            {
                /* packet after tunnel start in VLAN 6 */
                packetSize = prvTgfPacketInfoArray[0]->totalLen;
                switch (pushType) {
                case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_1_LABEL_E:
                    packetSize += TGF_MPLS_HEADER_SIZE_CNS;
                    break;
                case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_2_LABEL_E:
                    packetSize += 2 * TGF_MPLS_HEADER_SIZE_CNS;
                    break;
                case PRV_TGF_TUNNEL_MPLS_LSR_PUSH_3_LABEL_E:
                    packetSize += 3 * TGF_MPLS_HEADER_SIZE_CNS;
                    break;
                case PRV_TGF_TUNNEL_MPLS_LSR_SWAP_E:
                    break;
                case PRV_TGF_TUNNEL_MPLS_LSR_POP_SWAP_E:
                    packetSize -= TGF_MPLS_HEADER_SIZE_CNS;
                    break;
                default:
                    break;
                }
            }
            else
            {
                /* original packet before tunneling in VLAN 5 */
                packetSize = prvTgfPacketInfoArray[0]->totalLen;
            }

            /* check ETH counters */
            rc = prvTgfEthCountersCheck(
                prvTgfDevNum,
                prvTgfPortsArray[portIter],
                prvTgfPacketsCountRxArray[0][portIter],
                prvTgfPacketsCountTxArray[0][portIter],
                packetSize, 1 /*burstCount*/);
            UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfEthCountersCheck");
        }

        if (routingMode == PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E)
        {
            /* check IP counters */
            rc = prvTgfCountersIpCheck();
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfEthCountersCheck: %d", prvTgfDevNum);
        }

        /* print captured packets from receive port with prvTgfStartEntryMac */
        rc = prvTgfPortCapturePacketPrint(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS],
                prvTgfStartEntryMac, &numTriggers);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
                                     "prvTgfPortCapturePacketPrint:"
                                     " port = %d, rc = 0x%02X\n",
                                     prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], rc);

        /* check if captured packet has the same MAC DA as prvTgfArpMac */
        UTF_VERIFY_EQUAL6_STRING_MAC(1, numTriggers,
                "\n   MAC DA of captured packet must be: %02X:%02X:%02X:%02X:%02X:%02X\n",
                prvTgfStartEntryMac[0], prvTgfStartEntryMac[1], prvTgfStartEntryMac[2],
                prvTgfStartEntryMac[3], prvTgfStartEntryMac[4], prvTgfStartEntryMac[5]);
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfPortCapturePacketPrint: %d", prvTgfDevNum);
    }


    /* -------------------------------------------------------------------------
     * 3. Restore configuration
     */

    /* restore TTI lookup and MAC DA mode for Eth key */
    rc = prvTgfTtiEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                         PRV_TGF_TTI_KEY_MPLS_E,
                         PRV_TGF_TTI_MAC_MODE_DA_E,
                         GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiEnable: %d", prvTgfDevNum);

    /* restore common configuration */
    rc = prvTgfConfigurationRestore(GT_FALSE, isTtiSet);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfConfigurationRestore: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfTunnelMplsLsrPop
*
* DESCRIPTION:
*       MPLS LSR POP Functionality
*
* INPUTS:
*       popType - Type of MPLS LSR POP Tunnel Start Test
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
GT_VOID prvTgfTunnelMplsPop
(
    IN PRV_TGF_TUNNEL_MPLS_LSR_POP_TYPE_ENT popType
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    portIter;
    GT_U32    packetSize;
    GT_BOOL   isTtiSet = GT_FALSE;
    GT_U32    numTriggers = 0;
    PRV_TGF_IP_ROUTING_MODE_ENT     routingMode;

    PRV_UTF_LOG1_MAC("\nVLAN %d CONFIGURATION:\n", PRV_TGF_SEND_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  Port members: [%d], [%d]\n",
            prvTgfPortsArray[0], prvTgfPortsArray[1]);

    PRV_UTF_LOG1_MAC("VLAN %d CONFIGURATION:\n", PRV_TGF_NEXTHOPE_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  Port members: [%d], [%d]\n",
            prvTgfPortsArray[2], prvTgfPortsArray[3]);

    PRV_UTF_LOG0_MAC("FDB CONFIGURATION:\n");
    PRV_UTF_LOG1_MAC("  MAC DA ...:34:02 to port [%d]\n", prvTgfPortsArray[1]);
    PRV_UTF_LOG1_MAC("  MAC DA ...:00:11 to port [%d]\n", prvTgfPortsArray[3]);

    PRV_UTF_LOG0_MAC("PCL CONFIGURATION:\n");
    PRV_UTF_LOG0_MAC("  CLEAR\n");

    PRV_UTF_LOG0_MAC("TTI CONFIGURATION:\n");
    PRV_UTF_LOG2_MAC("  FORWARD packet with MAC DA ..:34:02 to port [%d]\n"
            "  in VLAN %d with MIM tunnel (MAC DA must be ...:00:11)\n",
            prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], PRV_TGF_NEXTHOPE_VLANID_CNS);

    PRV_UTF_LOG0_MAC("TRAFFIC GENERATION:\n");
    PRV_UTF_LOG0_MAC("  Sending MPLS packet with 3 labels:\n"
                     "   label_0=0x33(outer label)\n   label_1=0x22\n   label_2=0x11\n");
    PRV_UTF_LOG1_MAC("   and MAC DA=...:34:02\n  to port [%d]\n",
            prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    PRV_UTF_LOG0_MAC("EXPECTED RESULTS:\n");
    PRV_UTF_LOG1_MAC("  packet is not forwarded in VLAN %d\n", PRV_TGF_SEND_VLANID_CNS);
    PRV_UTF_LOG2_MAC("  packet is forwarded to port [%d] VLAN %d with ",
            prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], PRV_TGF_NEXTHOPE_VLANID_CNS);
    if(PRV_TGF_TUNNEL_MPLS_LSR_POP_1_LABEL_E == popType)
        PRV_UTF_LOG0_MAC("2 labels:\n   label_0=0x22(outer label)\n   lebel_1=0x11\n");
    else if(PRV_TGF_TUNNEL_MPLS_LSR_POP_2_LABEL_E == popType)
        PRV_UTF_LOG0_MAC("1 labels:\n   label_0=0x11(outer label)\n");
    PRV_UTF_LOG0_MAC("   and MAC DA=...:00:11\n\n");


    /* -------------------------------------------------------------------------
     * 1. Set configuration
     */

    /* set VLANs */
    rc = prvTgfDefBrigeConfSet();
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefBrigeConfSet: %d", prvTgfDevNum);

    /* enable TTI lookup for MPLS key and MAC DA mode */
    if (GT_OK == rc) {
        rc = prvTgfTtiEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                             PRV_TGF_TTI_KEY_MPLS_E,
                             PRV_TGF_TTI_MAC_MODE_DA_E,
                             GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiEnable: %d", prvTgfDevNum);
    }

    /* set TTI rules for MPLS Pop key */
    if (GT_OK == rc) {
        rc = prvTgfDefTtiMplsPopRuleSet(popType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefTtiMplsPopRuleSet: %d", prvTgfDevNum);
        isTtiSet = (GT_OK == rc);
    }

    /* set ARP Entry (Write ARP address 00:00:00:00:00:11 at index 1) */
    if (GT_OK == rc) {
        rc = prvTgfDefArpEntrySet();
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefArpEntrySet: %d", prvTgfDevNum);
    }


    /* -------------------------------------------------------------------------
     * 2. Generate traffic and Check counters
     */

    /* get routing mode */
    rc = prvTgfIpRoutingModeGet(&routingMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfIpRoutingModeGet: %d", prvTgfDevNum);

    if (GT_OK == rc)
    {
        if (routingMode == PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E)
        {
            /* reset IP counters */
            rc = prvTgfCountersIpReset();
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfCountersIpReset: %d", prvTgfDevNum);
        }

        /* reset ETH counters */
        rc = prvTgfEthCountersReset(prvTgfDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* select MPLS packet to send */
        prvTgfPacketInfoArray[0] = &prvTgfPacketMpls3Info;

        /* enable capturing of receive port */
        rc = prvTgfPortCaptureEnable(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* generate traffic */
        rc = prvTgfTrafficGenerate();
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTrafficGenerate: %d", prvTgfDevNum);

        /* disable capturing of receive port */
        rc = prvTgfPortCaptureEnable(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, rc, "prvTgfEthCountersReset: %d", prvTgfDevNum);

        /* check ETH counters */
        for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
        {
            if (PRV_TGF_RECEIVE_PORT_IDX_CNS == portIter)
            {
                /* packet after tunnel start in VLAN 6 */
                packetSize = prvTgfPacketInfoArray[0]->totalLen;
                switch (popType) {
                case PRV_TGF_TUNNEL_MPLS_LSR_POP_1_LABEL_E:
                    packetSize -= TGF_MPLS_HEADER_SIZE_CNS;
                    break;
                case PRV_TGF_TUNNEL_MPLS_LSR_POP_2_LABEL_E:
                    packetSize -= 2 * TGF_MPLS_HEADER_SIZE_CNS;
                    break;
                default:
                    break;
                }
            }
            else
            {
                /* original packet before tunneling in VLAN 5 */
                packetSize = prvTgfPacketInfoArray[0]->totalLen;
            }

            if (PRV_TGF_SEND_PORT_IDX_CNS != portIter)
            {
                packetSize -= TGF_VLAN_TAG_SIZE_CNS;
            }

            /* check ETH counters */
            rc = prvTgfEthCountersCheck(
                prvTgfDevNum,
                prvTgfPortsArray[portIter],
                prvTgfPacketsCountRxArray[0][portIter],
                prvTgfPacketsCountTxArray[0][portIter],
                packetSize, 1 /*burstCount*/);
            UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "ERROR of prvTgfEthCountersCheck");
        }

        if (routingMode == PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E)
        {
            /* check IP counters */
            rc = prvTgfCountersIpCheck();
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfEthCountersCheck: %d", prvTgfDevNum);
        }

        /* print captured packets from receive port with prvTgfStartEntryMac */
        rc = prvTgfPortCapturePacketPrint(
                prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS],
                prvTgfStartEntryMac, &numTriggers);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
                                     "prvTgfPortCapturePacketPrint:"
                                     " port = %d, rc = 0x%02X\n",
                                     prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], rc);

        /* check if captured packet has the same MAC DA as prvTgfArpMac */
        UTF_VERIFY_EQUAL6_STRING_MAC(1, numTriggers,
                "\n   MAC DA of captured packet must be: %02X:%02X:%02X:%02X:%02X:%02X\n",
                prvTgfStartEntryMac[0], prvTgfStartEntryMac[1], prvTgfStartEntryMac[2],
                prvTgfStartEntryMac[3], prvTgfStartEntryMac[4], prvTgfStartEntryMac[5]);
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_OK, rc, "prvTgfPortCapturePacketPrint: %d", prvTgfDevNum);
    }


    /* -------------------------------------------------------------------------
     * 3. Restore configuration
     */

    /* restore TTI lookup and MAC DA mode for Eth key */
    rc = prvTgfTtiEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                         PRV_TGF_TTI_KEY_MPLS_E,
                         PRV_TGF_TTI_MAC_MODE_DA_E,
                         GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiEnable: %d", prvTgfDevNum);

    /* restore common configuration */
    rc = prvTgfConfigurationRestore(GT_FALSE, isTtiSet);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfConfigurationRestore: %d", prvTgfDevNum);
}


