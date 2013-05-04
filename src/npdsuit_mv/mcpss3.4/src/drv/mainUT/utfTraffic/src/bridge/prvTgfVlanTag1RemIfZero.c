/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVlanTag1RemIfZero.c
*
* DESCRIPTION:
*       Bridge VLAN Tag1 Removal If Zero UT.
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
#include <common/tgfTunnelGen.h>
#include <bridge/prvTgfVlanTag1RemIfZero.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default VLAN0 Id */
#define PRV_TGF_VLANID_2_CNS  2

/* Zero VLAN Id */
#define PRV_TGF_VLANID_ZERO_CNS  0

/* VLAN Id 5*/
#define PRV_TGF_VLANID_5_CNS  5

/* VLAN Id 7*/
#define PRV_TGF_VLANID_7_CNS  7

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS     0

/* port number to send traffic to */
#define PRV_TGF_RECEIVE_PORT_IDX_CNS  2

/* default number of packets to send */
static GT_U32  prvTgfBurstCount   = 1;

/* parameters that is needed to be restored */
static PRV_TGF_TTI_MAC_MODE_ENT        restoreTtiMacMode;

/* TTI Rule index */
static GT_U32   prvTgfTtiRuleIndex = 1;

/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0x00, 0x00, 0x44, 0x33, 0x22, 0x11},               /* dstMac */
    {0x00, 0x00, 0x11, 0x11, 0x11, 0x11}                /* srcMac */
};

/* First VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_2_CNS                            /* pri, cfi, VlanId */
};

/* Second VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag1Part = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_7_CNS                          /* pri, cfi, VlanId */
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

/************************ Double tagged packet ********************************/

/* PARTS of double tagged packet */
static TGF_PACKET_PART_STC prvTgfPacketDoubleTagPartArray[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag1Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of double tagged packet */
#define PRV_TGF_PACKET_DOUBLE_TAG_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + 2 * TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of double tagged packet with CRC */
#define PRV_TGF_PACKET_DOUBLE_TAG_CRC_LEN_CNS  PRV_TGF_PACKET_DOUBLE_TAG_LEN_CNS + TGF_CRC_LEN_CNS

/* Double tagged PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfoDoubleTag =
{
    PRV_TGF_PACKET_DOUBLE_TAG_LEN_CNS,                                    /* totalLen */
    sizeof(prvTgfPacketDoubleTagPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketDoubleTagPartArray                                        /* partsArray */
};

/************************ Single tagged packet ********************************/

/* PARTS of single tagged packet */
static TGF_PACKET_PART_STC prvTgfPacketSingleTagPartArray[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of single tagged packet */
#define PRV_TGF_PACKET_SINGLE_TAG_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of single tagged packet with CRC */
#define PRV_TGF_PACKET_SINGLE_TAG_CRC_LEN_CNS  PRV_TGF_PACKET_SINGLE_TAG_LEN_CNS + TGF_CRC_LEN_CNS

/* Single tagged PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfoSingleTag =
{
    PRV_TGF_PACKET_SINGLE_TAG_LEN_CNS,                                    /* totalLen */
    sizeof(prvTgfPacketSingleTagPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketSingleTagPartArray                                        /* partsArray */
};

/************************ Untagged packet ********************************/

/* PARTS of untagged packet */
static TGF_PACKET_PART_STC prvTgfPacketUntagPartArray[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of untagged packet */
#define PRV_TGF_PACKET_UNTAG_LEN_CNS TGF_L2_HEADER_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of untagged packet with CRC */
#define PRV_TGF_PACKET_UNTAG_CRC_LEN_CNS  PRV_TGF_PACKET_UNTAG_LEN_CNS + TGF_CRC_LEN_CNS

/* Untagged PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfoUntag =
{
    PRV_TGF_PACKET_UNTAG_LEN_CNS,                                /* totalLen */
    sizeof(prvTgfPacketUntagPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketUntagPartArray                                        /* partsArray */
};

/******************************************************************************\
 *                            Private variables                               *
\******************************************************************************/

/*
 * typedef: enum PRV_TGF_PKT_TAG_TYPE_ENT
 *
 * Description: Enumeration of Packet Tag type.
 *
 * Enumerations:
 *    PRV_TGF_PKT_TAG_TYPE_UNTAGGED_E - packet is untagged
 *
 *    PRV_TGF_PKT_TAG_TYPE_SINGLE_TAGGED_E - packet is single tagged
 *
 *    PRV_TGF_PKT_TAG_TYPE_DOUBLE_TAGGED_E - packet is double tagged
 */
typedef enum
{
    PRV_TGF_PKT_TAG_TYPE_UNTAGGED_E,
    PRV_TGF_PKT_TAG_TYPE_SINGLE_TAGGED_E,
    PRV_TGF_PKT_TAG_TYPE_DOUBLE_TAGGED_E
}PRV_TGF_PKT_TAG_TYPE_ENT;


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/
/*******************************************************************************
* prvTgfVlanTag1RemIfZeroVlanCfg
*
* DESCRIPTION:
*       Set VLAN entry with recieved tag command.
*
* INPUTS:
*       vlanId       - vlanId to be configured
*       tagCmd       - VLAN tag command
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
static GT_VOID prvTgfVlanTag1RemIfZeroVlanCfg
(
    IN GT_U16                            vlanId,
    IN PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT tagCmd
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

        portsTaggingCmd.portsCmd[prvTgfPortsArray[portIter]] = tagCmd;
    }

    rc = prvTgfBrgVlanEntryWrite(prvTgfDevNum,
                                 vlanId,
                                 &portsMembers,
                                 &portsTagging,
                                 &vlanInfo,
                                 &portsTaggingCmd);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryWrite: %d", vlanId);

}


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroTtiEnable
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
static GT_STATUS prvTgfVlanTag1RemIfZeroTtiEnable
(
    IN  GT_U8                    portNum,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT ttiKeyType,
    IN  PRV_TGF_TTI_MAC_MODE_ENT macMode,
    IN  GT_BOOL                  isTtiEnable
)
{
    GT_STATUS rc = GT_OK;

    if (GT_TRUE == isTtiEnable)
    {
        /* save current Eth TTI key lookup MAC mode */
        rc = prvTgfTtiMacModeGet(prvTgfDevNum, ttiKeyType, &restoreTtiMacMode);
        PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiMacModeGet");
    }
    else
    {
        /* restore Eth TTI key lookup MAC mode */
        macMode = restoreTtiMacMode;
    }

    /* enable/disable port for ttiKeyType TTI lookup */
    rc = prvTgfTtiPortLookupEnableSet(portNum, ttiKeyType, isTtiEnable);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiPortLookupEnableSet");

    /* set TTI lookup MAC mode */
    rc = prvTgfTtiMacModeSet(ttiKeyType, macMode);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfTtiMacModeSet");

    return rc;
};

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroDefTtiEthRuleSet
*
* DESCRIPTION:
*       Configure Tunnel rule and action
*
* INPUTS:
*       isTagged    - GT_TRUE   packet is tagged
*                     GT_FALSE  packet is not tagged
*       tag1VlanId  - Tag1 Vlan ID assignment value for TTI action
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
static GT_STATUS prvTgfVlanTag1RemIfZeroDefTtiEthRuleSet
(
    GT_BOOL   isTagged,
    GT_U16    tag1VlanId
)
{
    GT_STATUS                   rc = GT_OK;
    PRV_TGF_TTI_ACTION_STC      ttiAction;
    PRV_TGF_TTI_RULE_UNT        ttiPattern;
    PRV_TGF_TTI_RULE_UNT        ttiMask;

    /* cpssDxChTtiRuleSet(CPSS_DXCH_TTI_ETH_RULE_STC, CPSS_DXCH_TTI_ACTION_2_STC) */

    /* set TTI Rule */
    cpssOsMemSet(&ttiPattern, 0, sizeof(ttiPattern));
    cpssOsMemCpy(ttiPattern.eth.common.mac.arEther, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));
    ttiPattern.eth.common.vid      = PRV_TGF_VLANID_2_CNS;
    ttiPattern.eth.common.isTagged = isTagged;
    ttiPattern.eth.common.pclId    = 3;

    cpssOsMemSet(&ttiMask, 0, sizeof(ttiMask));
    cpssOsMemSet(ttiMask.eth.common.mac.arEther, 0xFF, sizeof(TGF_MAC_ADDR));
    if (isTagged)
    {
        ttiMask.eth.common.vid = 0xFFF;
    }
    else
    {
        ttiMask.eth.common.vid = 0x0;
    }
    ttiMask.eth.common.isTagged = 0x1;
    ttiMask.eth.common.pclId = 0x3FF;

    /* set action - redirect to port 18 */
    cpssOsMemSet(&ttiAction, 0, sizeof(ttiAction));
    ttiAction.tunnelTerminate                   = GT_FALSE;
    ttiAction.command                           = CPSS_PACKET_CMD_FORWARD_E;
    ttiAction.redirectCommand                   = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
    ttiAction.interfaceInfo.type                = CPSS_INTERFACE_PORT_E;
    ttiAction.interfaceInfo.devPort.devNum      = prvTgfDevNum;
    ttiAction.interfaceInfo.devPort.portNum     = prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];
    ttiAction.tag0VlanCmd                       = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ttiAction.tag0VlanId                        = PRV_TGF_VLANID_5_CNS;
    ttiAction.tag1VlanCmd                       = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
    ttiAction.tag1VlanId                        = tag1VlanId;
    ttiAction.bindToPolicer                     = GT_FALSE;
    ttiAction.meterEnable                       = GT_FALSE;
    ttiAction.pcl0OverrideConfigIndex           = GT_FALSE;
    ttiAction.bridgeBypass                      = GT_TRUE;
    ttiAction.ingressPipeBypass                 = GT_TRUE;


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
* prvTgfVlanTag1RemIfZeroTunelCfg
*
* DESCRIPTION:
*       TTI Configuration
*
* INPUTS:
*       isTagged    - GT_TRUE   packet is tagged
*                     GT_FALSE  packet is not tagged
*       tag1VlanId  - Tag1 Vlan ID assignment value for TTI action
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
static GT_VOID prvTgfVlanTag1RemIfZeroTunelCfg
(
    GT_BOOL   isTagged,
    GT_U16    tag1VlanId
)
{
    GT_STATUS rc = GT_OK;


    /* enable TTI lookup for Eth key and MAC DA mode */
    rc = prvTgfVlanTag1RemIfZeroTtiEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                         PRV_TGF_TTI_KEY_ETH_E,
                         PRV_TGF_TTI_MAC_MODE_DA_E,
                         GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiEnable: %d", prvTgfDevNum);

    /* set TTI rules for Eth key */
    if (GT_OK == rc) {
        rc = prvTgfVlanTag1RemIfZeroDefTtiEthRuleSet(isTagged, tag1VlanId);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfDefTtiEthRuleSet: %d", prvTgfDevNum);
    }
}


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroTestPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
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
static GT_VOID prvTgfVlanTag1RemIfZeroTestPacketSend
(
    IN TGF_PACKET_STC          *packetInfoPtr
)
{
    GT_STATUS       rc         = GT_OK;
    GT_U32          portsCount = prvTgfPortsNum;
    GT_U32          portIter   = 0;


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


    /* send Packet from port port 0 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
}


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroTxEthTriggerCheck
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
static GT_VOID prvTgfVlanTag1RemIfZeroTxEthTriggerCheck
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


    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);


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
                                 "Packet VLAN TAG0/1 pattern is wrong");
}


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroTrafficGenerate
*
* DESCRIPTION:
*
* INPUTS:
*       tag1Mode            - VLAN Tag1 Removal mode
*       ingressPktTagType   - Ingress tag packet type: untagged, single or
*                             double
*       ttiActionTag1VlanId - Vlan Id value to be assigned to Tag1 Vlan in
*                             TTI action
*       isTagged            - GT_TRUE - packet ingresses as tagged
*                             GT_FALSE - packet ingresses as untagged
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
static GT_VOID prvTgfVlanTag1RemIfZeroTrafficGenerate
(
    IN PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT    tag1Mode,
    IN PRV_TGF_PKT_TAG_TYPE_ENT                         ingressPktTagType,
    IN GT_U16                                           ttiActionTag1VlanId,
    IN GT_BOOL                                          isTagged
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    TGF_VFD_INFO_STC                vfdArray[2];
    GT_U8                           vfdNum;
    CPSS_INTERFACE_INFO_STC         portInterface;
    GT_BOOL                         tag1IsRemoved;


    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);

    /* Configure vlan 2 and 5, where the Vlan command includes Tag1 */
    prvTgfVlanTag1RemIfZeroVlanCfg(PRV_TGF_VLANID_2_CNS,
                                   PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E);

    prvTgfVlanTag1RemIfZeroVlanCfg(PRV_TGF_VLANID_5_CNS,
                                   PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E);

    /* Configure Tunnel, match tagged packed and assigned Tag1 Vlan ID iz zero */
    prvTgfVlanTag1RemIfZeroTunelCfg(isTagged, ttiActionTag1VlanId);

    /* Enable/Disable Vlan Tag1 removal */
    rc = prvTgfBrgVlanRemoveVlanTag1IfZeroModeSet(tag1Mode);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgVlanRemoveVlanTag1IfZeroModeSet %d", GT_TRUE);


    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];

    /* Start capturing Tx packets */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d, %d",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum, GT_TRUE);

    /* -------------------------------------------------------------------------
     * 1. Sending unicast
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast =======\n");

    /* send packets */
    switch (ingressPktTagType)
    {
        case PRV_TGF_PKT_TAG_TYPE_UNTAGGED_E:
            prvTgfVlanTag1RemIfZeroTestPacketSend(&prvTgfPacketInfoUntag);
            break;
        case PRV_TGF_PKT_TAG_TYPE_SINGLE_TAGGED_E:
            prvTgfVlanTag1RemIfZeroTestPacketSend(&prvTgfPacketInfoSingleTag);
            break;
        case PRV_TGF_PKT_TAG_TYPE_DOUBLE_TAGGED_E:
            prvTgfVlanTag1RemIfZeroTestPacketSend(&prvTgfPacketInfoDoubleTag);
            break;
        default:
            PRV_UTF_LOG1_MAC("Ingress Packet Type is wrong ingressPktTagType = %d", ingressPktTagType);
    }
    cpssOsTimerWkAfter(10);


    /* Stop capturing Tx packets */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d, %d",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum, GT_FALSE);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        if (PRV_TGF_RECEIVE_PORT_IDX_CNS == portIter)
        {
            /* check that packet length includes double tag */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");

            /* Check if Tag1 is removed */
            tag1IsRemoved = GT_FALSE;

            switch (tag1Mode)
            {
                case PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E:
                    /* Tag1 is not removed */
                    /* Tag1 Vlan is Inner Tag and Tag0 is outer tag */
                    break;

                case PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E:
                    /* Check if packet ingressed as double tagged */
                    if (ingressPktTagType == PRV_TGF_PKT_TAG_TYPE_DOUBLE_TAGGED_E)
                    {
                        /* Tag1 is not removed */
                        /* Tag1 Vlan is Inner Tag and Tag0 is outer tag */
                    }
                    else if (ttiActionTag1VlanId == PRV_TGF_VLANID_ZERO_CNS)
                    {
                        /* Tag1 is removed */
                        tag1IsRemoved = GT_TRUE;
                    }
                    else
                    {
                        /* Tag1 is not zero and therefore is not removed */
                        /* Tag1 Vlan is Inner Tag and Tag0 is outer tag */
                    }
                    break;

                case PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E:
                    if (ttiActionTag1VlanId == PRV_TGF_VLANID_ZERO_CNS)
                    {
                        /* Tag1 is removed */
                        tag1IsRemoved = GT_TRUE;
                    }
                    else
                    {
                        /* Tag1 is not zero and therefore is not removed */
                        /* Tag1 Vlan is Inner Tag and Tag0 is outer tag */
                    }
                    break;

                default:
                    PRV_UTF_LOG1_MAC("Tag1 VLAN Remove mode is wrong, mode = %d", tag1Mode);
                    break;
            }

            /* Configure vfdArray */
            vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
            vfdArray[0].modeExtraInfo = 0;
            vfdArray[0].offset = 12;

            if (tag1IsRemoved == GT_TRUE)
            {
                vfdArray[0].cycleCount = 4;
                vfdArray[0].patternPtr[0] = 0x81;
                vfdArray[0].patternPtr[1] = 0x00;
                vfdArray[0].patternPtr[2] = 0x00;
                vfdArray[0].patternPtr[3] = 0x05;

                vfdNum = 1;
            }
            else
            {
                vfdArray[0].cycleCount = 4;
                vfdArray[0].patternPtr[0] = 0x81;
                vfdArray[0].patternPtr[1] = 0x00;
                vfdArray[0].patternPtr[2] = (GT_U8) (ttiActionTag1VlanId >> 8);
                vfdArray[0].patternPtr[3] = (GT_U8) (ttiActionTag1VlanId & 0xFF);

                vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
                vfdArray[1].modeExtraInfo = 0;
                vfdArray[1].offset = 16;
                vfdArray[1].cycleCount = 4;
                vfdArray[1].patternPtr[0] = 0x81;
                vfdArray[1].patternPtr[1] = 0x00;
                vfdArray[1].patternPtr[2] = 0x00;
                vfdArray[1].patternPtr[3] = 0x05;

                vfdNum = 2;
            }

            prvTgfVlanTag1RemIfZeroTxEthTriggerCheck(vfdNum, vfdArray);
        }
    }
}


/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/
/*******************************************************************************
* prvTgfVlanTag1RemIfZeroConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*
*       Generate Traffic:
*
*       Success Criteria:
*
*       Additional Configuration:
*
*       Generate Traffic:
*
*       Success Criteria:
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
GT_VOID prvTgfVlanTag1RemIfZeroConfigurationSet
(
    GT_VOID
)
{
    return;
}


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroDisabledTrafficGenerate
*
* DESCRIPTION:
*           Disable VLan Tag1 Removal.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 single tagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is not removed and it's value 0
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
GT_VOID prvTgfVlanTag1RemIfZeroDisabledTrafficGenerate
(
    void
)
{
    prvTgfVlanTag1RemIfZeroTrafficGenerate(PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E,
                                           PRV_TGF_PKT_TAG_TYPE_SINGLE_TAGGED_E,
                                           PRV_TGF_VLANID_ZERO_CNS,
                                           GT_TRUE);
}


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroSingleTagTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for not double tagged packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 single tagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is removed
*
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
GT_VOID prvTgfVlanTag1RemIfZeroSingleTagTrafficGenerate
(
    void
)
{
    prvTgfVlanTag1RemIfZeroTrafficGenerate(PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E,
                                           PRV_TGF_PKT_TAG_TYPE_SINGLE_TAGGED_E,
                                           PRV_TGF_VLANID_ZERO_CNS,
                                           GT_TRUE);
}


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroDoubleTagTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for all packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 double tagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is removed
*
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
GT_VOID prvTgfVlanTag1RemIfZeroDoubleTagTrafficGenerate
(
    void
)
{
    prvTgfVlanTag1RemIfZeroTrafficGenerate(PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E,
                                           PRV_TGF_PKT_TAG_TYPE_DOUBLE_TAGGED_E,
                                           PRV_TGF_VLANID_ZERO_CNS,
                                           GT_TRUE);
}

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroUntagTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for all packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 untagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is removed
*
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
GT_VOID prvTgfVlanTag1RemIfZeroUntagTrafficGenerate
(
    void
)
{
    prvTgfVlanTag1RemIfZeroTrafficGenerate(PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E,
                                           PRV_TGF_PKT_TAG_TYPE_UNTAGGED_E,
                                           PRV_TGF_VLANID_ZERO_CNS,
                                           GT_FALSE);
}

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroDoubleTagDisableTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for not double tagged packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 double tagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is not removed
*
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
GT_VOID prvTgfVlanTag1RemIfZeroDoubleTagDisableTrafficGenerate
(
    void
)
{
    prvTgfVlanTag1RemIfZeroTrafficGenerate(PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E,
                                           PRV_TGF_PKT_TAG_TYPE_DOUBLE_TAGGED_E,
                                           PRV_TGF_VLANID_ZERO_CNS,
                                           GT_TRUE);
}


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroTag1AssignmentIsNotZeroTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for all packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 2.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 untagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is removed
*
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
GT_VOID prvTgfVlanTag1RemIfZeroTag1AssignmentIsNotZeroTrafficGenerate
(
    void
)
{
    prvTgfVlanTag1RemIfZeroTrafficGenerate(PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E,
                                           PRV_TGF_PKT_TAG_TYPE_DOUBLE_TAGGED_E,
                                           PRV_TGF_VLANID_2_CNS,
                                           GT_TRUE);
}

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroConfigurationRestore
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
GT_VOID prvTgfVlanTag1RemIfZeroConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS       rc;


    /* restore TTI lookup and MAC DA mode for Eth key */
    rc = prvTgfVlanTag1RemIfZeroTtiEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                          PRV_TGF_TTI_KEY_ETH_E,
                                          PRV_TGF_TTI_MAC_MODE_DA_E,
                                          GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiEnable: %d", prvTgfDevNum);

    /* Invalidate VLAN entry 2 and 5 */
    rc = prvTgfBrgVlanEntryInvalidate(PRV_TGF_VLANID_2_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d", prvTgfDevNum);

    rc = prvTgfBrgVlanEntryInvalidate(PRV_TGF_VLANID_5_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d", prvTgfDevNum);

    /* Invalidate TTI rule */
    rc = prvTgfTtiRuleValidStatusSet(prvTgfTtiRuleIndex, GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfTtiRuleValidStatusSet: %d", prvTgfDevNum);

    /* Disable VLAN Tag1 0 Removal */
    rc = prvTgfBrgVlanRemoveVlanTag1IfZeroModeSet(PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgVlanRemoveVlanTag1IfZeroModeSet %d", GT_TRUE);

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");
}


