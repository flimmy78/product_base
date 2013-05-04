/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVlanManipulation.c
*
* DESCRIPTION:
*       VLAN manipulation
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <bridge/prvTgfVlanManipulation.h>
#include <common/tgfCommon.h>

/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS  5

/* TAG1 VLAN Id */
#define PRV_TGF_VLANID1_CNS 10

/* TAG VLAN Id */
#define PRV_TGF_VLANID2_CNS 25

/* TAG VLAN Id */
#define PRV_TGF_VLANID3_CNS 4000

/* TAG VLAN Id */
#define PRV_TGF_VLANID4_CNS 100

/* ether types */
#define PRV_TGF_ETHERTYPE_0_CNS  BIT_0
#define PRV_TGF_ETHERTYPE_1_CNS  BIT_1
#define PRV_TGF_ETHERTYPE_2_CNS  BIT_2
#define PRV_TGF_ETHERTYPE_3_CNS  BIT_3
#define PRV_TGF_ETHERTYPE_4_CNS  BIT_4
#define PRV_TGF_ETHERTYPE_5_CNS  BIT_5
#define PRV_TGF_ETHERTYPE_6_CNS  BIT_6
#define PRV_TGF_ETHERTYPE_7_CNS  BIT_7

/* max entry index for TPID table */
#define PRV_TGF_MAX_ENTRY_INDEX_CNS  8

/* ingress port index */
static GT_U32 prvTgfIngrPortIdx = 3;
#define INGR_PORT_IDX_MAC prvTgfIngrPortIdx

/* egress port index */
static GT_U32 prvTgfEgrPortIdx = 2;
#define EGR_PORT_IDX_MAC prvTgfEgrPortIdx

/* default number of packets to send */
static GT_U32       prvTgfBurstCount   = 1;

/* array of source MACs for the tests */
static TGF_MAC_ADDR prvTgfSaMacArr[] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x03},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x04} };

/* array of destination MACs for the tests */
static TGF_MAC_ADDR prvTgfDaMacArr[] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x12},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x13},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x14} };

/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},               /* srcMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}                /* dstMac */
};

/* First VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_CNS                            /* pri, cfi, VlanId */
};

/* Second VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag1Part = {
    TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS,                 /* etherType */
    0, 0, PRV_TGF_VLANID1_CNS                           /* pri, cfi, VlanId */
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

/* flag to show if system is initialized*/
static GT_BOOL  prvTgfIsInitialized = GT_FALSE;

/* stored default PVID */
static GT_U16   prvTgfPvid = 0;

/* stored default VLAN Range */
static GT_U16   prvTgfDefVidRange = 0;

/* stored default TPID table entries */
static GT_U16   prvTgfDefEtherTypeArr[PRV_TGF_MAX_ENTRY_INDEX_CNS] = {0};

/* stored default egress etherTypes */
static struct
{
    GT_U16      etherType0;
    GT_U16      etherType1;
} prvTgfDefEgrEtherTypesStc;

/* stored default TPIDs */
static struct
{
    GT_U32      ingTag0TpidBmp;
    GT_U32      ingTag1TpidBmp;
    GT_U32      egrTag0TpidIndex;
    GT_U32      egrTag1TpidIndex;
} prvTgfDefTpidBmpStc;


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/
/*******************************************************************************
* prvTgfBrgVlanManipulationTestInit
*
* DESCRIPTION:
*       Set VLAN entry with received tag command, set global ingress/egress
*       TPID tables.
*
* INPUTS:
*       vlanId       - vlanId to be configured
*       tagCmd       - VLAN tag command
*       etherTypeNum - number of entries in array
*       etherTypeArr - array of Ethertypes
*       tag0TpidBmp  - bitmap represent entries in the TPID Table for TAG0
*       tag1TpidBmp  - bitmap represent entries in the TPID Table for TAG1
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
static GT_VOID prvTgfBrgVlanManipulationTestInit
(
    IN GT_U16                            vlanId,
    IN PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT tagCmd,
    IN GT_U32                            etherTypeNum,
    IN GT_U16                            etherTypeArr[],
    IN GT_U32                            tag0TpidBmp,
    IN GT_U32                            tag1TpidBmp
)
{
    GT_U32                      portIter  = 0;
    GT_U32                      portCount = 0;
    GT_U32                      ethIter   = 0;
    GT_U32                      index     = 0;
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


    /* Configure ingress TPID table */
    for (ethIter = 0, index = 0; ethIter < PRV_TGF_MAX_ENTRY_INDEX_CNS; ethIter++)
    {
        if ((1 == (tag0TpidBmp >> ethIter & 1)) || (1 == (tag1TpidBmp >> ethIter & 1)))
        {
            /* check index range in array of ethertypes */
            if (index > etherTypeNum)
            {
                PRV_UTF_LOG1_MAC("WARNING: index [%d] is out of range!\n", index);

                break;
            }

            if (!prvTgfIsInitialized)
            {
                /* save default TPID table entry */
                rc = prvTgfBrgVlanTpidEntryGet(CPSS_DIRECTION_INGRESS_E,
                                               ethIter,
                                               &(prvTgfDefEtherTypeArr[index]));
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntryGet: %d, %d, %d",
                                             prvTgfDevNum, CPSS_DIRECTION_INGRESS_E,
                                             ethIter);
            }

            /* set TPID table entry */
            rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_INGRESS_E,
                                           ethIter,
                                           etherTypeArr[index++]);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d, %d",
                                         prvTgfDevNum, CPSS_DIRECTION_INGRESS_E,
                                         ethIter, etherTypeArr[index - 1]);
        }
    }

    if (!prvTgfIsInitialized)
    {
        /* save ingress TPID select for TAG0 */
        rc = prvTgfBrgVlanPortIngressTpidGet(prvTgfDevNum,
                                             prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                             CPSS_VLAN_ETHERTYPE0_E,
                                             &(prvTgfDefTpidBmpStc.ingTag0TpidBmp));
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortIngressTpidGet: %d, %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                     CPSS_VLAN_ETHERTYPE0_E);

        /* save ingress TPID select for TAG1 */
        rc = prvTgfBrgVlanPortIngressTpidGet(prvTgfDevNum,
                                             prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                             CPSS_VLAN_ETHERTYPE1_E,
                                             &(prvTgfDefTpidBmpStc.ingTag1TpidBmp));
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortIngressTpidGet: %d, %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                     CPSS_VLAN_ETHERTYPE1_E);
    }

    /* Set ingress TPID select for TAG0 */
    rc = prvTgfBrgVlanPortIngressTpidSet(prvTgfDevNum,
                                         prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                         CPSS_VLAN_ETHERTYPE0_E,
                                         tag0TpidBmp);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortIngressTpidSet: %d, %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                 CPSS_VLAN_ETHERTYPE0_E, tag0TpidBmp);

    /* Set ingress TPID select for TAG1 */
    rc = prvTgfBrgVlanPortIngressTpidSet(prvTgfDevNum,
                                         prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                         CPSS_VLAN_ETHERTYPE1_E,
                                         tag1TpidBmp);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortIngressTpidSet: %d, %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                 CPSS_VLAN_ETHERTYPE1_E, tag1TpidBmp);

    if (!prvTgfIsInitialized)
    {
        /* save egress TPID table */
        rc = prvTgfBrgVlanTpidEntryGet(CPSS_DIRECTION_EGRESS_E,
                                       0,
                                       &(prvTgfDefEgrEtherTypesStc.etherType0));
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntryGet: %d, %d, %d",
                                     prvTgfDevNum, CPSS_DIRECTION_EGRESS_E, 0);

        rc = prvTgfBrgVlanTpidEntryGet(CPSS_DIRECTION_EGRESS_E,
                                       1,
                                       &(prvTgfDefEgrEtherTypesStc.etherType1));
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntryGet: %d, %d, %d",
                                     prvTgfDevNum, CPSS_DIRECTION_EGRESS_E, 1);
    }

    /* Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
      (by default Ethertype2-7 remain 0x8100). */
    rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                   0,
                                   0x9100);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d, %d",
                                 prvTgfDevNum, CPSS_DIRECTION_EGRESS_E,
                                 0, 0x9100);

    rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                   1,
                                   0x98a8);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d, %d",
                                 prvTgfDevNum, CPSS_DIRECTION_EGRESS_E,
                                 1, 0x98a8);

    if (!prvTgfIsInitialized)
    {
        /* save egress TPID select for TAG0 */
        rc = prvTgfBrgVlanPortEgressTpidGet(prvTgfDevNum,
                                            prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                            CPSS_VLAN_ETHERTYPE0_E,
                                            &(prvTgfDefTpidBmpStc.egrTag1TpidIndex));
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortEgressTpidGet: %d, %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                     CPSS_VLAN_ETHERTYPE0_E);

        /* save egress TPID select for TAG1 */
        rc = prvTgfBrgVlanPortEgressTpidGet(prvTgfDevNum,
                                            prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                            CPSS_VLAN_ETHERTYPE1_E,
                                            &(prvTgfDefTpidBmpStc.egrTag1TpidIndex));
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortIngressTpidGet: %d, %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                     CPSS_VLAN_ETHERTYPE1_E);
    }

    /* Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1. */
    rc = prvTgfBrgVlanPortEgressTpidSet(prvTgfDevNum,
                                        prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                        CPSS_VLAN_ETHERTYPE0_E,
                                        0);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortEgressTpidSet: %d, %d, %d, %d",
                                 prvTgfDevNum, 0,
                                 CPSS_VLAN_ETHERTYPE0_E, 0);

    rc = prvTgfBrgVlanPortEgressTpidSet(prvTgfDevNum,
                                        prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                        CPSS_VLAN_ETHERTYPE1_E,
                                        1);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortEgressTpidSet: %d, %d, %d, %d",
                                 prvTgfDevNum, 0,
                                 CPSS_VLAN_ETHERTYPE1_E, 1);

    /* system initialized successfully */
    prvTgfIsInitialized = GT_TRUE;
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTestPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface (port only is legal param)
*       packetInfoPtr - (pointer to) the packet info
*       vfdNum        - VFD number
*       vfdArray      - VFD array with expected results
*       srcDstIndex   - index in Src and Dst MAC address array
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
static GT_VOID prvTgfBrgVlanManipulationTestPacketSend
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN TGF_PACKET_STC          *packetInfoPtr,
    IN GT_U8                    vfdNum,
    IN TGF_VFD_INFO_STC         vfdArray[],
    IN GT_U8                    srcDstIndex
)
{
    GT_STATUS rc          = GT_OK;
    GT_U32    portsCount  = prvTgfPortsNum;
    GT_U32    portIter    = 0;
    GT_U32    numTriggers = 0;

    /* use different 'etherType for vlan tagging' instead of 0x8100 */
    /* this will make our packet that has '0x8100' to be recognized as 'untagged'
      by the 'traffic generator' and also the CPSS will not override the ethertype
      of tag0 with the DSA tag */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

    /* -------------------------------------------------------------------------
     * 1. Sending unknown unicast
     */
    PRV_UTF_LOG0_MAC("======= Sending unknown unicast =======\n");

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* set source and destination MAC address in Packet */
    cpssOsMemCpy(prvTgfPacketL2Part.saMac, prvTgfSaMacArr[srcDstIndex], sizeof(prvTgfPacketL2Part.saMac));
    cpssOsMemCpy(prvTgfPacketL2Part.daMac, prvTgfDaMacArr[srcDstIndex], sizeof(prvTgfPacketL2Part.daMac));

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, packetInfoPtr, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d",
                                 prvTgfDevNum, prvTgfBurstCount, 0, 0);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* Start capturing Tx packets */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(portInterfacePtr, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d, %d",
                                 portInterfacePtr->devPort.devNum, portInterfacePtr->devPort.portNum, GT_TRUE);

    /* send Packet from port 0 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC]);


    /* Stop capturing Tx packets */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(portInterfacePtr, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d, %d",
                                 portInterfacePtr->devPort.devNum, portInterfacePtr->devPort.portNum, GT_FALSE);

    PRV_UTF_LOG1_MAC("Port [%d] capturing:\n", portInterfacePtr->devPort.portNum);

    /* print captured packets and check TriggerCounters */
    rc = tgfTrafficGeneratorPortTxEthTriggerCountersGet(
            portInterfacePtr, vfdNum, vfdArray, &numTriggers);
    rc = rc == GT_NO_MORE ? GT_OK : rc;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc,
                                 "tgfTrafficGeneratorPortTxEthTriggerCountersGet:"
                                 " port = %d, rc = 0x%02X\n", portInterfacePtr->devPort.portNum, rc);

    /* check if captured packet has the same pattern as vfdArray */
    UTF_VERIFY_EQUAL0_STRING_MAC((1 << vfdNum) - 1, numTriggers,
                                 "Packet VLAN TAG0/1 pattern is wrong");

    /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTestTableReset
*
* DESCRIPTION:
*       Function clears FDB, VLAN tables and internal table of captured packets.
*
* INPUTS:
*       vlanId       - vlanId to be cleared
*       etherTypeNum - number of entries in array
*       tag0TpidBmp  - bitmap represent entries in the TPID Table for TAG0
*       tag1TpidBmp  - bitmap represent entries in the TPID Table for TAG1
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
static GT_VOID prvTgfBrgVlanManipulationTestTableReset
(
    IN GT_U16                         vlanId,
    IN GT_U32                         etherTypeNum,
    IN GT_U32                         tag0TpidBmp,
    IN GT_U32                         tag1TpidBmp
)
{
    GT_U32      ethIter = 0;
    GT_U32      index   = 0;
    GT_STATUS   rc      = GT_OK;


    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    if (prvTgfIsInitialized)
    {
        /* restore egress TPID select for TAG0 */
        rc = prvTgfBrgVlanPortEgressTpidSet(prvTgfDevNum,
                                            prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                            CPSS_VLAN_ETHERTYPE0_E,
                                            prvTgfDefTpidBmpStc.egrTag1TpidIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortEgressTpidSet: %d, %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                     CPSS_VLAN_ETHERTYPE0_E);

        /* restore egress TPID select for TAG1 */
        rc = prvTgfBrgVlanPortEgressTpidSet(prvTgfDevNum,
                                            prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                            CPSS_VLAN_ETHERTYPE1_E,
                                            prvTgfDefTpidBmpStc.egrTag1TpidIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortEgressTpidSet: %d, %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[EGR_PORT_IDX_MAC],
                                     CPSS_VLAN_ETHERTYPE1_E);

        /* restore egress TPID table */
        rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                       0,
                                       prvTgfDefEgrEtherTypesStc.etherType0);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d",
                                     prvTgfDevNum, CPSS_DIRECTION_EGRESS_E, 0);

        /* restore egress TPID table */
        rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                       1,
                                       prvTgfDefEgrEtherTypesStc.etherType1);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d",
                                     prvTgfDevNum, CPSS_DIRECTION_EGRESS_E, 1);

        /* restore ingress TPID select for TAG0 */
        rc = prvTgfBrgVlanPortIngressTpidSet(prvTgfDevNum,
                                             prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                             CPSS_VLAN_ETHERTYPE0_E,
                                             prvTgfDefTpidBmpStc.ingTag0TpidBmp);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortIngressTpidSet: %d, %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                     CPSS_VLAN_ETHERTYPE0_E);

        /* save ingress TPID select for TAG1 */
        rc = prvTgfBrgVlanPortIngressTpidSet(prvTgfDevNum,
                                             prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                             CPSS_VLAN_ETHERTYPE1_E,
                                             prvTgfDefTpidBmpStc.ingTag1TpidBmp);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortIngressTpidSet: %d, %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC],
                                     CPSS_VLAN_ETHERTYPE1_E);

        /* restore ingress TPID table */
        for (ethIter = 0, index = 0; ethIter < PRV_TGF_MAX_ENTRY_INDEX_CNS; ethIter++)
        {
            if ((1 == (tag0TpidBmp >> ethIter & 1)) || (1 == (tag1TpidBmp >> ethIter & 1)))
            {
                /* check index range in array of ethertypes */
                if (index > etherTypeNum)
                {
                    PRV_UTF_LOG1_MAC("WARNING: index [%d] is out of range!\n", index);

                    break;
                }

                /* set TPID table entry */
                rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_INGRESS_E,
                                               ethIter,
                                               prvTgfDefEtherTypeArr[index++]);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d, %d",
                                             prvTgfDevNum, CPSS_DIRECTION_INGRESS_E,
                                             ethIter, prvTgfDefEtherTypeArr[index - 1]);
            }
        }
    }

    if(GT_TRUE == prvTgfCommonIsDeviceForce(prvTgfDevNum))
    {
        /* 'free' (set 0x8100) entries 0..7 in the egress,ingress table */
        prvTgfCommonUseLastTpid(GT_TRUE);
        for(index = 0 ; index < PRV_TGF_MAX_ENTRY_INDEX_CNS ; index ++)
        {
            /* set additional configuration */
            prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_INGRESS_E,
                                           index, TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
            prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                           index, TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
        }
        prvTgfCommonUseLastTpid(GT_FALSE);
    }

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(vlanId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, vlanId);

    /* system restored */
    prvTgfIsInitialized = GT_FALSE;
}

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "Untagged" tagging command for all ports in VLAN.
*         - Build VLAN Entry with VID = 10 and portsMember = 0, 8, 18, 23 and
*           tagging command "TAG0" for all ports in VLAN to be sure that
*           decision is according to VID = 5.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};

   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 0;
   prvTgfEgrPortIdx = 1;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* Build VLAN Entry with VID = 10 and portsMember = 0, 8, 18, 23 and
       tagging command "TAG0" for all ports in VLAN to be sure that
       decision is according to VID = 5. */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID1_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 10
*           Success Criteria:
*               Output packet has no any tags
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdDoubleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 6;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfDaMacArr[0], vfdArray[0].cycleCount);

    vfdArray[1].cycleCount = 6;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 6;
    cpssOsMemCpy(vfdArray[1].patternPtr, prvTgfSaMacArr[0], vfdArray[1].cycleCount);

    vfdArray[2].cycleCount = 6;
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].modeExtraInfo = 0;
    vfdArray[2].offset = 12;
    cpssOsMemCpy(vfdArray[2].patternPtr, prvTgfPayloadDataArr, vfdArray[2].cycleCount);

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 3, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID1_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "Untagged" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 1;
    prvTgfEgrPortIdx  = 2;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_2_CNS,
                                      PRV_TGF_ETHERTYPE_3_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has no any tags
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdSingleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 6;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfDaMacArr[0], vfdArray[0].cycleCount);

    vfdArray[1].cycleCount = 6;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 6;
    cpssOsMemCpy(vfdArray[1].patternPtr, prvTgfSaMacArr[0], vfdArray[1].cycleCount);

    vfdArray[2].cycleCount = 6;
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].modeExtraInfo = 0;
    vfdArray[2].offset = 12;
    cpssOsMemCpy(vfdArray[2].patternPtr, prvTgfPayloadDataArr, vfdArray[2].cycleCount);

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 3, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS,2,
                                            PRV_TGF_ETHERTYPE_2_CNS,
                                            PRV_TGF_ETHERTYPE_3_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Set PVID to 5 – for untagged packet
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "Untagged" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationSet
(
    GT_VOID
)
{
    GT_U16      etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                                   TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
    GT_STATUS   rc = GT_OK;

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 2;
    prvTgfEgrPortIdx = 3;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* store default PVID */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], &prvTgfPvid);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d", prvTgfPortsArray[INGR_PORT_IDX_MAC]);

    /* set PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12
*           Success Criteria:
*               Output packet has no any tags
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdUntagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 6;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 0;
    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfDaMacArr[0], vfdArray[0].cycleCount);

    vfdArray[1].cycleCount = 6;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 6;
    cpssOsMemCpy(vfdArray[1].patternPtr, prvTgfSaMacArr[0], vfdArray[1].cycleCount);

    vfdArray[2].cycleCount = 6;
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].modeExtraInfo = 0;
    vfdArray[2].offset = 12;
    cpssOsMemCpy(vfdArray[2].patternPtr, prvTgfPayloadDataArr, vfdArray[2].cycleCount);

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoUntag, 3, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    /* restore PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 25 and portsMember = 0, 8, 18, 23
*           and "TAG0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Set VLAN RANGE to FFF (by default FFE)
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS   rc      = GT_OK;
    GT_U16      vlan0Id = 25;
    GT_U16      vlan1Id = 4095;
    GT_U16      etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                                   TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 3;
    prvTgfEgrPortIdx = 2;

    prvTgfPacketVlanTag0Part.vid = vlan0Id;
    prvTgfPacketVlanTag1Part.vid = vlan1Id;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(vlan0Id,
                                      PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* store default VLAN range */
    rc = prvTgfBrgVlanRangeGet(&prvTgfDefVidRange);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanRangeGet: %d", prvTgfDevNum);

    /* set VLAN range */
    rc = prvTgfBrgVlanRangeSet(0xFFF);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanRangeSet: %d, %d", prvTgfDevNum, 0xFFF);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 25
*               TAG1:  ethertype = 0x88a8, vlan = 4095
*           Success Criteria:
*               Output packet has one tag TAG0 with
*               ethertype = 0x9100, vlan = 25
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdDoubleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x19;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 1, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc      = GT_OK;
    GT_U16      vlan0Id = 25;

    prvTgfPacketVlanTag0Part.vid = PRV_TGF_VLANID_CNS;
    prvTgfPacketVlanTag1Part.vid = PRV_TGF_VLANID1_CNS;

    prvTgfBrgVlanManipulationTestTableReset(vlan0Id, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    /* restore default VLAN range */
    rc = prvTgfBrgVlanRangeSet(prvTgfDefVidRange);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanRangeSet: %d, %d",
                                 prvTgfDevNum, prvTgfDefVidRange);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "TAG0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 2;
   prvTgfEgrPortIdx = 1;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has one tag TAG0 with
*               ethertype = 0x9100, vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdSingleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x05;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 1, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "TAG0" tagging command for all ports in VLAN.
*         - Set PVID to 5 – for untagged packet
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdUntagConfigurationSet
(
    GT_VOID
)
{
    GT_U16      etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                                   TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
    GT_STATUS   rc = GT_OK;

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 1;
    prvTgfEgrPortIdx = 0;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* store default PVID */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], &prvTgfPvid);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d", prvTgfPortsArray[INGR_PORT_IDX_MAC]);

    /* set PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12
*           Success Criteria:
*               Output packet has one tag TAG0 with
*               ethertype = 0x9100, vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdUntagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x20; /* VPT is 1 because of CPSS default */
    vfdArray[0].patternPtr[3] = 0x05;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoUntag, 1, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdUntagConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    /* restore PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "TAG1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 3;
   prvTgfEgrPortIdx = 0;

    /* set VLAN entry, TPID global tables */
    if(GT_TRUE == prvTgfCommonIsDeviceForce(prvTgfDevNum))
    {
        /* don't use index 6,7 in the ingress table -- reserved for egress
           ethertypes over cascade/ring ports */
        prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                          PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E,
                                          2, etherTypeArr,
                                          PRV_TGF_ETHERTYPE_4_CNS,
                                          PRV_TGF_ETHERTYPE_5_CNS);
    }
    else
    {
        prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                          PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E,
                                          2, etherTypeArr,
                                          PRV_TGF_ETHERTYPE_6_CNS,
                                          PRV_TGF_ETHERTYPE_7_CNS);
    }
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 10
*           Success Criteria:
*               Output packet has one tag TAG1 with
*               ethertype = 0x98a8, vlan = 10
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdDoubleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x98;
    vfdArray[0].patternPtr[1] = 0xa8;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x0a;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 1, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_6_CNS,
                                            PRV_TGF_ETHERTYPE_7_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "TAG1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 3;
   prvTgfEgrPortIdx = 1;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has one tag TAG1 with
*               ethertype = 0x98a8, vlan = 0
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdSingleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x98;
    vfdArray[0].patternPtr[1] = 0xa8;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x00;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 1, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "TAG1" tagging command for all ports in VLAN.
*         - Set PVID to 5 – for untagged packet
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdUntagConfigurationSet
(
    GT_VOID
)
{
    GT_U16      etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                                   TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
    GT_STATUS   rc = GT_OK;

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 3;
    prvTgfEgrPortIdx = 2;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* store default PVID */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], &prvTgfPvid);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d", prvTgfPortsArray[INGR_PORT_IDX_MAC]);

    /* set PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12
*           Success Criteria:
*               Output packet has one tag TAG1 with
*               ethertype = 0x98a8, vlan = 0
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdUntagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x98;
    vfdArray[0].patternPtr[1] = 0xa8;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x00;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoUntag, 1, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdUntagConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    /* restore PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag0, Inner Tag1" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 2;
   prvTgfEgrPortIdx = 0;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG1:  ethertype = 0x88a8, vlan = 10
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG1 with ethertype = 0x98a8,
*               vlan = 10
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[2];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x05;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x98;
    vfdArray[1].patternPtr[1] = 0xa8;
    vfdArray[1].patternPtr[2] = 0x00;
    vfdArray[1].patternPtr[3] = 0x0a;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 2, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag0, Inner Tag1" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};

   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 2;
   prvTgfEgrPortIdx = 1;


    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG1 with ethertype = 0x98a8,
*               vlan = 0
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[2];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x05;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x98;
    vfdArray[1].patternPtr[1] = 0xa8;
    vfdArray[1].patternPtr[2] = 0x00;
    vfdArray[1].patternPtr[3] = 0;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 2, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag0, Inner Tag1" for all ports in VLAN.
*         - Set PVID to 5 - for untagged packet
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationSet
(
    GT_VOID
)
{
    GT_U16      etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                                   TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
    GT_STATUS   rc = GT_OK;

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 2;
    prvTgfEgrPortIdx = 3;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* store default PVID */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], &prvTgfPvid);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d", prvTgfPortsArray[INGR_PORT_IDX_MAC]);

    /* set PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Output packet has two tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG1 with ethertype = 0x98a8,
*               vlan = 0
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[2];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x20; /* VPT is 1 because of CPSS default */
    vfdArray[0].patternPtr[3] = 0x05;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x98;
    vfdArray[1].patternPtr[1] = 0xa8;
    vfdArray[1].patternPtr[2] = 0x00;
    vfdArray[1].patternPtr[3] = 0;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoUntag, 2, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    /* restore PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "Outer Tag1, Inner Tag0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 1;
   prvTgfEgrPortIdx = 0;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG1:  ethertype = 0x88a8, vlan = 10
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG1 with
*               ethertype = 0x98a8, vlan = 10 and TAG0 with ethertype = 0x9100,
*               vlan = 5.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[2];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x98;
    vfdArray[0].patternPtr[1] = 0xa8;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x0a;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x91;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = 0x00;
    vfdArray[1].patternPtr[3] = 0x05;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 2, vfdArray, 0);

}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag1, Inner Tag0" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 1;
   prvTgfEgrPortIdx = 2;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG1 with
*               ethertype = 0x98a8, vlan = 0 and TAG0 with ethertype = 0x9100,
*               vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[2];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x98;
    vfdArray[0].patternPtr[1] = 0xa8;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x91;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = 0x00;
    vfdArray[1].patternPtr[3] = 0x05;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 2, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag1, Inner Tag0" for all ports in VLAN.
*         - Set PVID to 5 - for untagged packet
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationSet
(
    GT_VOID
)
{
    GT_U16      etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                                   TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
    GT_STATUS   rc = GT_OK;

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 1;
    prvTgfEgrPortIdx = 3;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* store default PVID */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], &prvTgfPvid);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d", prvTgfPortsArray[INGR_PORT_IDX_MAC]);

    /* set PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Output packet has two tags in the following order: TAG1 with
*               ethertype = 0x98a8, vlan = 0 and TAG0 with ethertype = 0x9100,
*               vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[2];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x98;
    vfdArray[0].patternPtr[1] = 0xa8;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x91;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = 0x20; /* VPT is 1 because of CPSS default */
    vfdArray[1].patternPtr[3] = 0x05;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoUntag, 2, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    /* restore PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 and
*           tagging command "Push Tag0" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 0;
   prvTgfEgrPortIdx = 1;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_PUSH_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 10
*           Success Criteria:
*               Output packet has tree tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG0 with ethertype = 0x8100,
*               vlan = 5, TAG1 with ethertype = 0x88a8, vlan = 10.
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdDoubleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[3];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x05;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x81;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = 0x00;
    vfdArray[1].patternPtr[3] = 0x05;

    vfdArray[2].cycleCount = 4;
    vfdArray[2].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[2].modeExtraInfo = 0;
    vfdArray[2].offset = 20;
    vfdArray[2].patternPtr[0] = 0x88;
    vfdArray[2].patternPtr[1] = 0xa8;
    vfdArray[2].patternPtr[2] = 0x00;
    vfdArray[2].patternPtr[3] = 0x0a;


    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 3, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Push Tag0" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
   /* change ingress and egress ports */
   prvTgfIngrPortIdx = 0;
   prvTgfEgrPortIdx = 2;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_PUSH_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG0 with ethertype = 0x8100,
*               vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdSingleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[2];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x05;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x81;
    vfdArray[1].patternPtr[1] = 0x00;
    vfdArray[1].patternPtr[2] = 0x00;
    vfdArray[1].patternPtr[3] = 0x05;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 2, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Push Tag0" for all ports in VLAN.
*         - Set PVID to 5 - for untagged packet
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationSet
(
    GT_VOID
)
{
    GT_U16      etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                                   TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
    GT_STATUS   rc = 0;

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 0;
    prvTgfEgrPortIdx = 3;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_PUSH_TAG0_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* store default PVID */
    rc = prvTgfBrgVlanPortVidGet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], &prvTgfPvid);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidGet: %d", prvTgfPortsArray[INGR_PORT_IDX_MAC]);

    /* set PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Output packet has one tag: TAG0 with ethertype = 0x9100, vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdUntagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x20; /* VPT is 1 because of CPSS default */
    vfdArray[0].patternPtr[3] = 0x05;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoUntag, 1, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = 0;

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    /* restore PVID */
    rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanPortVidSet: %d, %d",
                                 prvTgfPortsArray[INGR_PORT_IDX_MAC], prvTgfPvid);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 and
*           tagging command "Pop Outer Tag" for all ports in VLAN.
*         - Build VLAN Entry with VID = 10 and portsMember = 0, 8, 18, 23 and
*           tagging command "UNTAGGED" for all ports in VLAN to be sure that
*           decision is according to VID = 5.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};

     /* change ingress and egress ports */
     prvTgfIngrPortIdx = 3;
     prvTgfEgrPortIdx = 0;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);

    /* Build VLAN Entry with VID = 10 and portsMember = 0, 8, 18, 23 and
       tagging command "UNTAGGED" for all ports in VLAN to be sure that
       decision is according to VID = 5. */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID1_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 10
*           Success Criteria:
*                Output packet has one tag: TAG1 with
*                ethertype = 0x98a8,vlan = 10.
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;

    /* the Pop Outer command does not changes Inner VLAN tag including TPID.
      So packet will enter to captured port and will be recognized as:
      TAG0:  absent, use VLAN assignment
      TAG1:  ethertype = 0x88a8, vlan = 10
      Captured (mirrored to CPU) packet will change TAG1 TPID to 0x8100 */
    vfdArray[0].patternPtr[0] = 0x81;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x0a;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 1, vfdArray, 0);

}

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID1_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Pop Outer Tag" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[2] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS,
                              TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS};
    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 3;
    prvTgfEgrPortIdx = 1;

    /* set VLAN entry, TPID global tables */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E,
                                      2, etherTypeArr,
                                      PRV_TGF_ETHERTYPE_0_CNS,
                                      PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet is untagged
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 6;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;

    cpssOsMemCpy(vfdArray[0].patternPtr, prvTgfPayloadDataArr, TGF_VFD_PATTERN_BYTES_NUM_CNS);

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 1, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationRestore
(
    GT_VOID
)
{
    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 2,
                                            PRV_TGF_ETHERTYPE_0_CNS,
                                            PRV_TGF_ETHERTYPE_1_CNS);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to:
*                       Ethertype0 = 0x8100,
*                       Ethertype1 = 0x88a8,
*                       Ethertype2 = 0x5000,
*                       Ethertype3 = 0xA0A0,
*                       Ethertype4 = 0x5050,
*                       Ethertype5 = 0x2525,
*                       Ethertype6 = 0x5555,
*                       Ethertype7 = 0xAAAA,
*         - Set ingress TPID select to: TAG0 is Ethertype0, Ethertype3, Ethertype5,
*           Ethertype6, Ethertype7, TAG1 is Ethertype1, Ethertype2, Ethertype4.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "TAG0" tagging command for all ports in VLAN.
*         - Build VLAN Entry with VID = 25 and portsMember = 0, 8, 18, 23
*           and "TAG0" tagging command for all ports in VLAN.
*         - Build VLAN Entry with VID = 4000 and portsMember = 0, 8, 18, 23
*           and "TAG0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[8] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS, TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS,
                               0x5000, 0xA0A0, 0x5050, 0x2525, 0x5555, 0xAAAA};
    GT_U8 etherTypeTag0 = PRV_TGF_ETHERTYPE_0_CNS | PRV_TGF_ETHERTYPE_3_CNS |
                          PRV_TGF_ETHERTYPE_5_CNS | PRV_TGF_ETHERTYPE_6_CNS |
                          PRV_TGF_ETHERTYPE_7_CNS;
    GT_U8 etherTypeTag1 = PRV_TGF_ETHERTYPE_1_CNS | PRV_TGF_ETHERTYPE_2_CNS |
                          PRV_TGF_ETHERTYPE_4_CNS;

    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 3;
    prvTgfEgrPortIdx = 2;

    /* Build VLAN Entry with VID = 5 */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E,
                                      8, etherTypeArr,
                                      etherTypeTag0, etherTypeTag1);

    /* Build VLAN Entry with VID = 25 */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID2_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E,
                                      8, etherTypeArr,
                                      etherTypeTag0, etherTypeTag1);

    /* Build VLAN Entry with VID = 4000 */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID3_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E,
                                      8, etherTypeArr,
                                      etherTypeTag0, etherTypeTag1);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment1TrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:03,
*               macSa = 00:00:00:00:00:13,
*               TAG0:  ethertype = 0x8100, vlan = 25
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:04,
*               macSa = 00:00:00:00:00:14,
*               TAG0:  ethertype = 0x8100, vlan = 4000
*
*           Success Criteria:
*               First  packet have TAG0:  ethertype =0x9100, vlan = 5
*               Second packet have TAG0:  ethertype =0x9100, vlan = 25
*               Third  packet have TAG0:  ethertype =0x9100, vlan = 4000
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment1TrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x05;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 1, vfdArray, 0);

    /* set TAG0 in Packet */
    prvTgfPacketVlanTag0Part.etherType = 0xA0A0;
    prvTgfPacketVlanTag0Part.vid       = PRV_TGF_VLANID2_CNS;

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].patternPtr[3] = 0x19;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 1, vfdArray, 1);

    /* set TAG0 in Packet */
    if(GT_TRUE == prvTgfCommonIsDeviceForce(portInterface.devPort.devNum))
    {
        prvTgfPacketVlanTag0Part.etherType = 0x2525;/*use value not in index 6 or 7 0xAAAA */ ;
    }
    else
    {
        prvTgfPacketVlanTag0Part.etherType = 0xAAAA;
    }

    prvTgfPacketVlanTag0Part.vid       = PRV_TGF_VLANID3_CNS;

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].patternPtr[2] = 0x0F;
    vfdArray[0].patternPtr[3] = 0xA0;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoSingleTag, 1, vfdArray, 2);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationRestore
(
    GT_VOID
)
{
    GT_U8 etherTypeTag0 = PRV_TGF_ETHERTYPE_0_CNS | PRV_TGF_ETHERTYPE_3_CNS |
                          PRV_TGF_ETHERTYPE_5_CNS | PRV_TGF_ETHERTYPE_6_CNS |
                          PRV_TGF_ETHERTYPE_7_CNS;
    GT_U8 etherTypeTag1 = PRV_TGF_ETHERTYPE_1_CNS | PRV_TGF_ETHERTYPE_2_CNS |
                          PRV_TGF_ETHERTYPE_4_CNS;

    /* restore default TAG0 in Packet */
    prvTgfPacketVlanTag0Part.etherType = TGF_ETHERTYPE_8100_VLAN_TAG_CNS;
    prvTgfPacketVlanTag0Part.vid       = PRV_TGF_VLANID_CNS;

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 8,
                                            etherTypeTag0, etherTypeTag1);

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID2_CNS, 8,
                                            etherTypeTag0, etherTypeTag1);

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID3_CNS, 8,
                                            etherTypeTag0, etherTypeTag1);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to:
*                       Ethertype0 = 0x8100,
*                       Ethertype1 = 0x88a8,
*                       Ethertype2 = 0x5000,
*                       Ethertype3 = 0xA0A0,
*                       Ethertype4 = 0x5050,
*                       Ethertype5 = 0x2525,
*                       Ethertype6 = 0x5555,
*                       Ethertype7 = 0xAAAA,
*         - Set ingress TPID select to: TAG0 is Ethertype0, Ethertype3, Ethertype5,
*           Ethertype6, Ethertype7, TAG1 is Ethertype1, Ethertype2, Ethertype4.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "TAG1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[8] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS, TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS,
                               0x5000, 0xA0A0, 0x5050, 0x2525, 0x5555, 0xAAAA};
    GT_U8 etherTypeTag0 = PRV_TGF_ETHERTYPE_0_CNS | PRV_TGF_ETHERTYPE_3_CNS |
                          PRV_TGF_ETHERTYPE_5_CNS | PRV_TGF_ETHERTYPE_6_CNS |
                          PRV_TGF_ETHERTYPE_7_CNS;
    GT_U8 etherTypeTag1 = PRV_TGF_ETHERTYPE_1_CNS | PRV_TGF_ETHERTYPE_2_CNS |
                          PRV_TGF_ETHERTYPE_4_CNS;
    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 2;
    prvTgfEgrPortIdx = 0;

    /* Build VLAN Entry with VID = 5 */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E,
                                      8, etherTypeArr,
                                      etherTypeTag0, etherTypeTag1);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment2TrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 25
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:03,
*               macSa = 00:00:00:00:00:13,
*               TAG0:  ethertype = 0xA0A0, vlan = 25
*               TAG1:  ethertype = 0x5050, vlan = 100
*
*           Success Criteria:
*               First  packet have TAG1:  ethertype =0x98a8, vlan = 25
*               Second packet have TAG1:  ethertype =0x98a8, vlan = 100
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment2TrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[1];

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set TAG1 in Packet */
    prvTgfPacketVlanTag1Part.vid = PRV_TGF_VLANID2_CNS;

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x98;
    vfdArray[0].patternPtr[1] = 0xa8;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x19;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 1, vfdArray, 0);

    /* set TAG0 in Packet */
    prvTgfPacketVlanTag0Part.etherType = 0xA0A0;

    /* set TAG1 in Packet */
    prvTgfPacketVlanTag1Part.etherType = 0x5050;
    prvTgfPacketVlanTag1Part.vid       = PRV_TGF_VLANID4_CNS;

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].patternPtr[3] = 0x64;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 1, vfdArray, 1);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationRestore
(
    GT_VOID
)
{
    GT_U8 etherTypeTag0 = PRV_TGF_ETHERTYPE_0_CNS | PRV_TGF_ETHERTYPE_3_CNS |
                          PRV_TGF_ETHERTYPE_5_CNS | PRV_TGF_ETHERTYPE_6_CNS |
                          PRV_TGF_ETHERTYPE_7_CNS;
    GT_U8 etherTypeTag1 = PRV_TGF_ETHERTYPE_1_CNS | PRV_TGF_ETHERTYPE_2_CNS |
                          PRV_TGF_ETHERTYPE_4_CNS;


    /* restore default TAG0 in Packet */
    prvTgfPacketVlanTag0Part.etherType = TGF_ETHERTYPE_8100_VLAN_TAG_CNS;
    prvTgfPacketVlanTag0Part.vid       = PRV_TGF_VLANID_CNS;

    /* restore default TAG1 in Packet */
    prvTgfPacketVlanTag1Part.etherType = TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS;
    prvTgfPacketVlanTag1Part.vid       = PRV_TGF_VLANID1_CNS;

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 8,
                                            etherTypeTag0, etherTypeTag1);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationChangeEgressTpidConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to:
*                       Ethertype0 = 0x8100,
*                       Ethertype1 = 0x88a8,
*                       Ethertype2 = 0x5000,
*                       Ethertype3 = 0xA0A0,
*                       Ethertype4 = 0x5050,
*                       Ethertype5 = 0x2525,
*                       Ethertype6 = 0x5555,
*                       Ethertype7 = 0xAAAA,
*         - Set ingress TPID select to: TAG0 is Ethertype0, Ethertype3, Ethertype5,
*           Ethertype6, Ethertype7, TAG1 is Ethertype1, Ethertype2, Ethertype4.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and "Outer Tag0, Inner Tag1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationChangeEgressTpidConfigurationSet
(
    GT_VOID
)
{
    GT_U16 etherTypeArr[8] = {TGF_ETHERTYPE_8100_VLAN_TAG_CNS, TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS,
                               0x5000, 0xA0A0, 0x5050, 0x2525, 0x5555, 0xAAAA};
    GT_U8 etherTypeTag0 = PRV_TGF_ETHERTYPE_0_CNS | PRV_TGF_ETHERTYPE_3_CNS |
                          PRV_TGF_ETHERTYPE_5_CNS | PRV_TGF_ETHERTYPE_6_CNS |
                          PRV_TGF_ETHERTYPE_7_CNS;
    GT_U8 etherTypeTag1 = PRV_TGF_ETHERTYPE_1_CNS | PRV_TGF_ETHERTYPE_2_CNS |
                          PRV_TGF_ETHERTYPE_4_CNS;
    /* change ingress and egress ports */
    prvTgfIngrPortIdx = 2;
    prvTgfEgrPortIdx = 1;

    /* Build VLAN Entry with VID = 5 */
    prvTgfBrgVlanManipulationTestInit(PRV_TGF_VLANID_CNS,
                                      PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E,
                                      8, etherTypeArr,
                                      etherTypeTag0, etherTypeTag1);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationChangeEgressTpidTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 100
*
*       Additional configuration:
*           Set egress TPID table to: Ethertype0 = 0x2222 and Ethertype1 = 0x3333
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 100
*
*           Success Criteria:
*               First  packet have TAG0:  ethertype =0x9100, vlan = 5;
*                                  TAG1:  ethertype =0x98a8, vlan = 100;
*               Second packet have TAG0:  ethertype =0x2222, vlan = 5;
*                                  TAG1:  ethertype =0x3333, vlan = 100;
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
GT_VOID prvTgfBrgVlanManipulationChangeEgressTpidTrafficGenerate
(
    GT_VOID
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    TGF_VFD_INFO_STC        vfdArray[2];
    GT_STATUS               rc = GT_OK;

    /* set port for packet capture */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum = 0;
    portInterface.devPort.portNum = prvTgfPortsArray[EGR_PORT_IDX_MAC];

    /* set TAG1 in Packet */
    prvTgfPacketVlanTag1Part.vid = PRV_TGF_VLANID4_CNS;

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].cycleCount = 4;
    vfdArray[0].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[0].modeExtraInfo = 0;
    vfdArray[0].offset = 12;
    vfdArray[0].patternPtr[0] = 0x91;
    vfdArray[0].patternPtr[1] = 0x00;
    vfdArray[0].patternPtr[2] = 0x00;
    vfdArray[0].patternPtr[3] = 0x05;

    vfdArray[1].cycleCount = 4;
    vfdArray[1].mode = TGF_VFD_MODE_STATIC_E;
    vfdArray[1].modeExtraInfo = 0;
    vfdArray[1].offset = 16;
    vfdArray[1].patternPtr[0] = 0x98;
    vfdArray[1].patternPtr[1] = 0xa8;
    vfdArray[1].patternPtr[2] = 0x00;
    vfdArray[1].patternPtr[3] = 0x64;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 2, vfdArray, 0);


    if(GT_TRUE == prvTgfCommonIsDeviceForce(portInterface.devPort.devNum))
    {
        /* 'free' (set 0x8100) entries 6,7 in the ingress table for entries for the egress :
        for cascade,ring ports */

        prvTgfCommonUseLastTpid(GT_TRUE);
        /* set additional configuration */
        rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_INGRESS_E,
                                       6, TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
        prvTgfCommonUseLastTpid(GT_FALSE);

        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d, %d",
                                     prvTgfDevNum, CPSS_DIRECTION_EGRESS_E,
                                     6, TGF_ETHERTYPE_8100_VLAN_TAG_CNS);

        prvTgfCommonUseLastTpid(GT_TRUE);
        rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_INGRESS_E,
                                       7, TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
        prvTgfCommonUseLastTpid(GT_FALSE);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d, %d",
                                     prvTgfDevNum, CPSS_DIRECTION_EGRESS_E,
                                     7, TGF_ETHERTYPE_8100_VLAN_TAG_CNS);

    }


    /* set additional configuration */
    rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                   0, 0x2222);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d, %d",
                                 prvTgfDevNum, CPSS_DIRECTION_EGRESS_E,
                                 0, 0x2222);

    rc = prvTgfBrgVlanTpidEntrySet(CPSS_DIRECTION_EGRESS_E,
                                   1, 0x3333);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanTpidEntrySet: %d, %d, %d, %d",
                                 prvTgfDevNum, CPSS_DIRECTION_EGRESS_E,
                                 1, 0x3333);

    /* set VFD array of expected fields value in received packet*/
    vfdArray[0].patternPtr[0] = 0x22;
    vfdArray[0].patternPtr[1] = 0x22;

    vfdArray[1].patternPtr[0] = 0x33;
    vfdArray[1].patternPtr[1] = 0x33;

    /* send packet */
    prvTgfBrgVlanManipulationTestPacketSend(&portInterface, &prvTgfPacketInfoDoubleTag, 2, vfdArray, 0);
}

/*******************************************************************************
* prvTgfBrgVlanManipulationChangeEgressTpidConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationChangeEgressTpidConfigurationRestore
(
    GT_VOID
)
{
    GT_U8 etherTypeTag0 = PRV_TGF_ETHERTYPE_0_CNS | PRV_TGF_ETHERTYPE_3_CNS |
                          PRV_TGF_ETHERTYPE_5_CNS | PRV_TGF_ETHERTYPE_6_CNS |
                          PRV_TGF_ETHERTYPE_7_CNS;
    GT_U8 etherTypeTag1 = PRV_TGF_ETHERTYPE_1_CNS | PRV_TGF_ETHERTYPE_2_CNS |
                          PRV_TGF_ETHERTYPE_4_CNS;

    /* restore default TAG0 in Packet */
    prvTgfPacketVlanTag0Part.etherType = TGF_ETHERTYPE_8100_VLAN_TAG_CNS;
    prvTgfPacketVlanTag0Part.vid       = PRV_TGF_VLANID_CNS;

    /* restore default TAG1 in Packet */
    prvTgfPacketVlanTag1Part.etherType = TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS;
    prvTgfPacketVlanTag1Part.vid       = PRV_TGF_VLANID1_CNS;

    prvTgfBrgVlanManipulationTestTableReset(PRV_TGF_VLANID_CNS, 8,
                                            etherTypeTag0, etherTypeTag1);
}

