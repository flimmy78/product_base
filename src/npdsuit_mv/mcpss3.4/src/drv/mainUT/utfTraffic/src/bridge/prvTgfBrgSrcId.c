/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBrgSrcId.c
*
* DESCRIPTION:
*       Bridge Source-Id UT.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <bridge/prvTgfBrgSrcId.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS  2

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS     0

/* port number to send traffic to */
#define PRV_TGF_RECEIVE_PORT_IDX_CNS  1

/* default number of packets to send */
static GT_U32  prvTgfBurstCount   = 1;

/* FDB MAC entry */
static PRV_TGF_BRG_MAC_ENTRY_STC prvTgfMacEntry;

/* MAC Address */
static TGF_MAC_ADDR macAddr1 = {0,0,0,0,0,0x01};

static TGF_MAC_ADDR macAddr2 = {0,0,0,0,0,0x02};

/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}                /* srcMac */
};

/* First VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_CNS                            /* pri, cfi, VlanId */
};


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

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] = {
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of packet */
#define PRV_TGF_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfo = {
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArray                                        /* partsArray */
};

/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/
/*******************************************************************************
* prvTgfBrgSrcIdTestInit
*
* DESCRIPTION:
*       Set VLAN entry.
*
* INPUTS:
*       vlanId   - vlanId to be configured
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
static GT_VOID prvTgfBrgSrcIdTestInit
(
    IN GT_U16           vlanId
)
{
    GT_U32                      portIter  = 0;
    GT_U32                      portCount = 0;
    GT_STATUS                   rc        = GT_OK;
    CPSS_PORTS_BMP_STC          portsMembers;
    CPSS_PORTS_BMP_STC          portsTagging;
    PRV_TGF_BRG_VLAN_INFO_STC   vlanInfo;
    PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;

    /* clear entry */
    cpssOsMemSet(&vlanInfo, 0, sizeof(vlanInfo));
    cpssOsMemSet(&portsTaggingCmd, 0, sizeof(PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC));

    /* set vlan entry */
    vlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
    vlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.ipv4IgmpToCpuEn      = GT_FALSE;
    vlanInfo.mirrToRxAnalyzerEn   = GT_FALSE;
    vlanInfo.ipv6IcmpToCpuEn      = GT_FALSE;
    vlanInfo.ipCtrlToCpuEn        = PRV_TGF_BRG_IP_CTRL_NONE_E;
    vlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv4IpmBrgEn         = GT_FALSE;
    vlanInfo.ipv6IpmBrgEn         = GT_FALSE;
    vlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
    vlanInfo.ipv4UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv4McastRouteEn     = GT_FALSE;
    vlanInfo.ipv6UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv6McastRouteEn     = GT_FALSE;
    vlanInfo.stgId                = 0;
    vlanInfo.autoLearnDisable     = GT_TRUE;/* working in controlled learning */
    vlanInfo.naMsgToCpuEn         = GT_TRUE;/* working in controlled learning */
    vlanInfo.mruIdx               = 0;
    vlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;
    vlanInfo.floodVidx            = 0xFFF;
    vlanInfo.floodVidxMode        = PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;
    vlanInfo.portIsolationMode    = PRV_TGF_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
    vlanInfo.ucastLocalSwitchingEn = GT_FALSE;
    vlanInfo.mcastLocalSwitchingEn = GT_FALSE;

    /* set vlan entry */
    portsMembers.ports[0] = 0;
    portsMembers.ports[1] = 0;
    portsTagging.ports[0] = 0;
    portsTagging.ports[1] = 0;

    portCount = prvTgfPortsNum;
    for (portIter = 0; portIter < portCount; portIter++)
    {
        portsMembers.ports[prvTgfPortsArray[portIter] >> 5] |= 1 << (prvTgfPortsArray[portIter] & 0x1f);

        portsTaggingCmd.portsCmd[prvTgfPortsArray[portIter]] = PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }

    rc = prvTgfBrgVlanEntryWrite(prvTgfDevNum,
                                 vlanId,
                                 &portsMembers,
                                 &portsTagging,
                                 &vlanInfo,
                                 &portsTaggingCmd);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryWrite: %d", vlanId);


    /* Add 2 MAC Entries */

    /* clear entry */
    cpssOsMemSet(&prvTgfMacEntry, 0, sizeof(prvTgfMacEntry));

    prvTgfMacEntry.key.entryType                = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    prvTgfMacEntry.key.key.macVlan.vlanId       = vlanId;
    prvTgfMacEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;
    prvTgfMacEntry.dstInterface.devPort.devNum  = 0;
    prvTgfMacEntry.dstInterface.devPort.portNum = 0;
    prvTgfMacEntry.isStatic                     = GT_FALSE;
    prvTgfMacEntry.daCommand                    = PRV_TGF_PACKET_CMD_FORWARD_E;
    prvTgfMacEntry.saCommand                    = PRV_TGF_PACKET_CMD_FORWARD_E;
    prvTgfMacEntry.daRoute                      = GT_FALSE;
    prvTgfMacEntry.mirrorToRxAnalyzerPortEn     = GT_FALSE;
    prvTgfMacEntry.userDefined                  = 0;
    prvTgfMacEntry.daQosIndex                   = 0;
    prvTgfMacEntry.saQosIndex                   = 0;
    prvTgfMacEntry.daSecurityLevel              = 0;
    prvTgfMacEntry.saSecurityLevel              = 0;
    prvTgfMacEntry.appSpecificCpuCode           = GT_FALSE;
    prvTgfMacEntry.pwId                         = 0;
    prvTgfMacEntry.spUnknown                    = GT_FALSE;
    prvTgfMacEntry.sourceId                     = 1;

    cpssOsMemCpy(prvTgfMacEntry.key.key.macVlan.macAddr.arEther, macAddr1, sizeof(TGF_MAC_ADDR));

    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntrySet");

    prvTgfMacEntry.dstInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];
    prvTgfMacEntry.sourceId                     = 0;
    cpssOsMemCpy(prvTgfMacEntry.key.key.macVlan.macAddr.arEther, macAddr2, sizeof(TGF_MAC_ADDR));

    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntrySet");
}

/*******************************************************************************
* prvTgfBrgSrcIdTestPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
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
static GT_VOID prvTgfBrgSrcIdTestPacketSend
(
    GT_VOID
)
{
    GT_STATUS       rc           = GT_OK;

    /* use different 'etherType for vlan tagging' instead of 0x8100 */
    /* this will make our packet that has '0x8100' to be recognized as 'untagged'
      by the 'traffic generator' and also the CPSS will not override the ethertype
      of tag0 with the DSA tag */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, &prvTgfPacketInfo, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d",
                                 prvTgfDevNum, prvTgfBurstCount, 0, NULL);

    /* send Packet from port 0 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
}

/*******************************************************************************
* prvTgfBrgSrcIdTestTableReset
*
* DESCRIPTION:
*       Function clears FDB, VLAN tables and internal table of captured packets.
*
* INPUTS:
*       vlanId - vlanId to be cleared
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
static GT_VOID prvTgfBrgSrcIdTestTableReset
(
    IN GT_U16 vlanId
)
{
    GT_STATUS   rc = GT_OK;

    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(vlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, vlanId);

    /* reset port default source-ID to 0 */
    rc = prvTgfBrgSrcIdPortDefaultSrcIdSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 0);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortDefaultSrcIdSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 0);

}

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/
/*******************************************************************************
* prvTgfBrgSrcIdPortForceConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports.
*           Set MAC table with two entries:
*              - MAC = 00:00:00:00:00:01, VLAN = 2, intType = PORT,
*               port = 0, sourceId = 1
*              - MAC = 00:00:00:00:00:02, VLAN = 2, intType = PORT,
*               port = 8, sourceId = 0
*           Source ID configuration:
*               Disable port force source-ID assigment
*               Configure Global Assigment mode to FDB SA-based.
*               Configure default port source-ID to 2.
*               Delete port 8 from source-ID group 1.
*               Enable Unicast Egreess filter.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is not captured in all ports.
*
*       Additional Configuration:
*           Enable Port Force Src-ID assigment.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is captured in port: 8.
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
GT_VOID prvTgfBrgSrcIdPortForceConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS       rc           = GT_OK;

    /* Disable port force source-ID assigment */
    rc = prvTgfBrgSrcIdPortSrcIdForceEnableSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortSrcIdForceEnableSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* Set global assign mode to FDB SA-based */
    rc = prvTgfBrgSrcIdGlobalSrcIdAssignModeSet(CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdGlobalSrcIdAssignModeSet: %d, %d", prvTgfDevNum, CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E);

    /* Set port default source-ID to 2 */
    rc = prvTgfBrgSrcIdPortDefaultSrcIdSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 2);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortDefaultSrcIdSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 2);

    /* Delete port 8 from source-ID group 1 */
    rc = prvTgfBrgSrcIdGroupPortDelete (prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdGroupPortDelete: %d, %d, %d",
                                 prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* Enable unicast egress filter for port 8 */
    rc = prvTgfBrgSrcIdPortUcastEgressFilterSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortUcastEgressFilterSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);

    /* set VLAN entry, TPID global tables */
    prvTgfBrgSrcIdTestInit(PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgSrcIdPortForceTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
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
GT_VOID prvTgfBrgSrcIdPortForceTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portsCount = prvTgfPortsNum;
    GT_U32       portIter   = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;

    /************************************/
    /* Phase 1 - FDB SA-based assigment */
    /************************************/

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfBrgSrcIdTestPacketSend();

    /* check counters */

    /* read counters */
    rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE, &portCntrs);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                 "get another goodPktsRcv counter than expected");

    /**********************************/
    /* Phase 2 - Port force assigment */
    /**********************************/

    /* Enable port force source-ID assigment */
    prvTgfBrgSrcIdPortSrcIdForceEnableSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE);

    /* send packet */
    prvTgfBrgSrcIdTestPacketSend();

    /* read counters */
    rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE, &portCntrs);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                 "get another goodPktsRcv counter than expected");
}

/*******************************************************************************
* prvTgfBrgSrcIdPortForceConfigurationRestore
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
GT_VOID prvTgfBrgSrcIdPortForceConfigurationRestore
(
    GT_VOID
)
{
    /* Add port 8 to source-ID group 1 */
    prvTgfBrgSrcIdGroupPortAdd(prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* table reset */
    prvTgfBrgSrcIdTestTableReset(PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgSrcIdFdbSaAssigmentConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports.
*           Set MAC table with two entries:
*              - MAC = 00:00:00:00:00:01, VLAN = 2, intType = PORT,
*               port = 0, sourceId = 1
*              - MAC = 00:00:00:00:00:02, VLAN = 2, intType = PORT,
*               port = 8, sourceId = 0
*           Source ID configuration:
*               Disable port force source-ID assigment
*               Configure Global Assigment mode to FDB SA-based.
*               Configure default port source-ID to 2.
*               Delete port 8 from source-ID group 1.
*               Enable Unicast Egreess filter.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is not captured in port 8.
*
*       Additional Configuration:
*           Add to Source-ID group 1 port 8.
*           Delete from Source-ID group 2 port 8.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is captured on port: 8.
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
GT_VOID prvTgfBrgSrcIdFdbSaAssigmentConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS       rc           = GT_OK;

    /* Disable port force source-ID assigment */
    rc = prvTgfBrgSrcIdPortSrcIdForceEnableSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortSrcIdForceEnableSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* Set global assign mode to FDB SA-based */
    rc = prvTgfBrgSrcIdGlobalSrcIdAssignModeSet(CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdGlobalSrcIdAssignModeSet: %d, %d", prvTgfDevNum, CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E);

    /* Set port default source-ID to 2 */
    rc = prvTgfBrgSrcIdPortDefaultSrcIdSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 2);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortDefaultSrcIdSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 2);

    /* Delete port 8 from source-ID group 1 */
    rc = prvTgfBrgSrcIdGroupPortDelete (prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdGroupPortDelete: %d, %d, %d",
                                 prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* Enable unicast egress filter for port 8 */
    rc = prvTgfBrgSrcIdPortUcastEgressFilterSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortUcastEgressFilterSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);

    /* set VLAN entry, TPID global tables */
    prvTgfBrgSrcIdTestInit(PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgSrcIdFdbSaAssigmentTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
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
GT_VOID prvTgfBrgSrcIdFdbSaAssigmentTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portsCount = prvTgfPortsNum;
    GT_U32       portIter   = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;

    /************************************/
    /* Phase 1 - FDB SA-based assigment */
    /************************************/

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfBrgSrcIdTestPacketSend();

    /* check counters */

    /* read counters */
    rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE, &portCntrs);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                 "get another goodPktsRcv counter than expected");

    /************************************/
    /* Phase 2 - FDB SA-based assigment */
    /************************************/

    /* Add to Source-ID group 1 port 8 */
    prvTgfBrgSrcIdGroupPortAdd(prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* Delete from Source-ID group 0 port 8 */
    prvTgfBrgSrcIdGroupPortDelete(prvTgfDevNum, 0, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* send packet */
    prvTgfBrgSrcIdTestPacketSend();

    /* read counters */
    rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE, &portCntrs);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                 "get another goodPktsRcv counter than expected");
}

/*******************************************************************************
* prvTgfBrgSrcIdFdbSaAssigmentConfigurationRestore
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
GT_VOID prvTgfBrgSrcIdFdbSaAssigmentConfigurationRestore
(
    GT_VOID
)
{
    /* Add to Source-ID group 2 port 8 */
    prvTgfBrgSrcIdGroupPortAdd(prvTgfDevNum, 0, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* table reset */
    prvTgfBrgSrcIdTestTableReset(PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgSrcIdFdbDaAssigmentConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports.
*           Set MAC table with two entries:
*              - MAC = 00:00:00:00:00:01, VLAN = 2, intType = PORT,
*               port = 0, sourceId = 1
*              - MAC = 00:00:00:00:00:02, VLAN = 2, intType = PORT,
*               port = 8, sourceId = 0
*           Source ID configuration:
*               Disable port force source-ID assigment
*               Configure Global Assigment mode to FDB SA-based.
*               Configure default port source-ID to 2.
*               Delete port 8 from source-ID group 1.
*               Enable Unicast Egreess filter.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is not captured in port 8.
*
*       Additional Configuration:
*           Add to Source-ID group 1 port 8.
*           Delete from Source-ID group 2 port 8.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is captured on port: 8.
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
GT_VOID prvTgfBrgSrcIdFdbDaAssigmentConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS       rc           = GT_OK;

    /* Disable port force source-ID assigment */
    rc = prvTgfBrgSrcIdPortSrcIdForceEnableSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortSrcIdForceEnableSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* Set global assign mode to FDB DA-based */
    rc = prvTgfBrgSrcIdGlobalSrcIdAssignModeSet(CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdGlobalSrcIdAssignModeSet: %d, %d", prvTgfDevNum, CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E);

    /* Set port default source-ID to 2 */
    rc = prvTgfBrgSrcIdPortDefaultSrcIdSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 2);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortDefaultSrcIdSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 2);

    /* Delete port 8 from source-ID group 0 */
    rc = prvTgfBrgSrcIdGroupPortDelete (prvTgfDevNum, 0, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdGroupPortDelete: %d, %d, %d",
                                 prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* Enable unicast egress filter for port 8 */
    rc = prvTgfBrgSrcIdPortUcastEgressFilterSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgSrcIdPortUcastEgressFilterSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE);

    /* set VLAN entry, TPID global tables */
    prvTgfBrgSrcIdTestInit(PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgSrcIdFdbDaAssigmentTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:00:11,
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
GT_VOID prvTgfBrgSrcIdFdbDaAssigmentTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portsCount = prvTgfPortsNum;
    GT_U32       portIter   = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;

    /************************************/
    /* Phase 1 - FDB DA-based assigment */
    /************************************/

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfBrgSrcIdTestPacketSend();

    /* check counters */

    /* read counters */
    rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE, &portCntrs);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                 "get another goodPktsRcv counter than expected");

    /************************************/
    /* Phase 2 - FDB DA-based assigment */
    /************************************/

    /* Add to Source-ID group 0 port 8 */
    prvTgfBrgSrcIdGroupPortAdd(prvTgfDevNum, 0, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* Delete from Source-ID group 1 port 8 */
    prvTgfBrgSrcIdGroupPortDelete(prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* send packet */
    prvTgfBrgSrcIdTestPacketSend();

    /* read counters */
    rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS], GT_TRUE, &portCntrs);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                 "get another goodPktsRcv counter than expected");

}

/*******************************************************************************
* prvTgfBrgSrcIdFdbDaAssigmentConfigurationRestore
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
GT_VOID prvTgfBrgSrcIdFdbDaAssigmentConfigurationRestore
(
    GT_VOID
)
{
    /* Add to Source-ID group 1 port 8 */
    prvTgfBrgSrcIdGroupPortAdd(prvTgfDevNum, 1, prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS]);

    /* table reset */
    prvTgfBrgSrcIdTestTableReset(PRV_TGF_VLANID_CNS);
}

