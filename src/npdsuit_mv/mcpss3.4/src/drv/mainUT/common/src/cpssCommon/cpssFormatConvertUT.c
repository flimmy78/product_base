/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssFormatConvertUT.c
*
* DESCRIPTION:
*       Unit tests for Format Convertion utility
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpssCommon/cpssFormatConvert.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTtiTypes.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnelTypes.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>


#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Forward declarations */
static GT_STATUS prvGetTtMplsRaw
(
   IN CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC   *stcDataPtr,
   OUT GT_U32                              *hwFormatArray
);

static GT_STATUS prvTtiIpv4HwGet
(
    IN  CPSS_DXCH_TTI_IPV4_RULE_STC        *ipv4StcPtr,
    OUT GT_U32                             *hwFormatArray
);

static GT_STATUS ttiActionLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_STC           *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
);

/* the size of tti rule key/mask in words */
#define TTI_RULE_SIZE_CNS               6

/* the size of tti action in words */
#define TTI_ACTION_SIZE_CNS             4

/* the size of tunnel termination TCAM rule key/mask in words */
#define TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS   PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_LINE_RULE_SIZE_CNS

/* the max number of loops for prvCpssFormat4 UT */
#define UT_FORMAT_4_MAX_NUM_OF_LOOPS_CNS    100

/*
Structure
typedef struct
{
    GT_U32                  srcPortTrunk;
    GT_U32                  srcIsTrunk;
    GT_U8                   srcDev;
    GT_U16                  vid;
    GT_ETHERADDR            macDa;
    GT_U32                  label1;
    GT_U32                  sBit1;
    GT_U32                  exp1;
    GT_U32                  label2;
    GT_U32                  sBit2;
    GT_U32                  exp2;

} CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC;

CH2 HW format

Bits    Field name Description
6:0     {1'b0, SrcPort[5:0]/SrcTrunk[6:0] Source port or Source trunk-ID
7       SrcIsTrunk
        0 - Source is not a trunk
        1 - Source is a trunk
12:8    SrcDev[4:0] Source device
13      reserved reserved
25:14   VID[11:0] Packet VID assignment
73:26   MAC_DA[47:0] Tunnel header MAC DA
74      TunnelKeyType For MPLS TT key, this is 1
75      reserved reserved
78:76   EXP2 EXP in Label 2
79      Sbit2 S-bit in Label 2
99:80   Label2 Label 2
102:100 EXP1 EXP in Label 1
103     Sbit1 S-bit in Label 1
123:104 Label1 Label 1
126:124 reserved reserved
127     Ttkey Must be set to 1
191:128 reserved reserved

*/

#define TT_MPLS_STC   CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC

static CPSS_FORMAT_CONVERT_FIELD_INFO_STC ttMplsKey[] =
{
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, srcPortTrunk, 0, 7),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, srcIsTrunk,   7, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, srcDev,       8, 5),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, vid,         14, 12),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC(TT_MPLS_STC,   macDa,  CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E,  26, 48),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, label1,      104, 20),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, sBit1,       103, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, exp1,        100, 3),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, label2,       80, 20),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, sBit2,        79, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(TT_MPLS_STC, exp2,         76, 3),
    /* bit 127 - Ttkey Must be set to 1 */
    CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_MAC(1 , 127, 1, 0),
    /* bit 74  - TunnelKeyType For MPLS TT key, this is 1 */
    CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_MAC(1 , 74, 1, 0),

    CPSS_FORMAT_CONVERT_FIELD_INFO_END_MAC
};

static CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC ttMplsStc1 = {
    1,
    0,
    5,
    0x555,
    {{0x01, 0x12, 0x23, 0x45, 0x67, 0xaa}},
    0xffff,
    1,
    5,
    0x1111,
    0,
    7
};

#if 0
static GT_U32 ttMpls1Raw[4] = {
    0xa9554501,
    0x488d159e,
    0x11117404,
    0x80ffffd0};
#endif
/* Sample 2 */
/*
typedef struct
{
    GT_U32                  pclId;
    GT_BOOL                 srcIsTrunk;
    GT_U32                  srcPortTrunk;
    GT_ETHERADDR            mac;
    GT_U16                  vid;
    GT_BOOL                 isTagged;
    GT_BOOL                 dsaSrcIsTrunk;
    GT_U8                   dsaSrcPortTrunk;
    GT_U8                   dsaSrcDevice;

} CPSS_DXCH_TTI_RULE_COMMON_STC;

typedef struct
{
    CPSS_DXCH_TTI_RULE_COMMON_STC common;
    GT_U32                  tunneltype;
    GT_IPADDR               srcIp;
    GT_IPADDR               destIp;
    GT_BOOL                 isArp;

} CPSS_DXCH_TTI_IPV4_RULE_STC;

  bits      width  name
9 : 0       10      PCL ID
17 : 10     8       LocalSrcPort/LocalSrcTrunkID
18 : 18     1       SrcIsTrunk
30 : 19     12      VLAN-ID
31 : 31     1       Packet Is Tagged
79 : 32     48      MAC_DA/MAC_SA
86 : 80     7       Source Device
89 : 87     3       Tunneling Protocol
121 : 90    32      SIP
153 : 122   32      DIP
154 : 154   1       IsARP
166 : 160   7       DSA Tag Source Port/Trunk
167 : 167   1       DSA SrcIsTrunk
190 : 168   23      Reserved
191 : 191   1       Must be 1. IPv4 / ARP TTI Key
159 : 155   5       Reserved

*/

#define TTI_CMN_STC CPSS_DXCH_TTI_RULE_COMMON_STC
static CPSS_FORMAT_CONVERT_FIELD_INFO_STC ttiRuleCommon[] =
{
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC   (TTI_CMN_STC, pclId,          0, 10),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC(TTI_CMN_STC, srcIsTrunk,    CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  18, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC   (TTI_CMN_STC, srcPortTrunk,  10, 8),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC(TTI_CMN_STC, mac,           CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E,  32, 48),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC   (TTI_CMN_STC, vid,           19, 12),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC(TTI_CMN_STC, isTagged,      CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  31, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC(TTI_CMN_STC, dsaSrcIsTrunk, CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  167, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC   (TTI_CMN_STC, dsaSrcPortTrunk, 160, 7),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC   (TTI_CMN_STC, dsaSrcDevice,     80, 7),

    CPSS_FORMAT_CONVERT_FIELD_INFO_END_MAC
};

#define TTI_IPV4_STC CPSS_DXCH_TTI_IPV4_RULE_STC

static CPSS_FORMAT_CONVERT_FIELD_INFO_STC ttiRuleIpv4[] =
{
    CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCTURE_GEN_MAC(TTI_IPV4_STC, common, 0, ttiRuleCommon),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC    (TTI_IPV4_STC, tunneltype,  87, 3),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_IPV4_STC, srcIp,  CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E,  90, 32),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_IPV4_STC, destIp, CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E,  122, 32),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_IPV4_STC, isArp,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  154, 1),
    /* bit 191  - must be 1 */
    CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_MAC(1 , 191, 1, 0),

    CPSS_FORMAT_CONVERT_FIELD_INFO_END_MAC
};



GT_STATUS prvCpssFormat1UT(void)
{
    GT_U32                                  rawData[10];
    GT_U32                                  rawDataSample[10];
    CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC   stcData;
    GT_STATUS rc;
    int       result;

    /* convert STC to Raw */

    /* clean raw data */
    cpssOsMemSet(rawData, 0, sizeof(rawData));

    /* convert STC to RAW:
      IN  ttMplsStc1 - STC data
      IN  ttMplsKey  - fields conversion array
      OUT rawData    - converted raw data */
    rc = cpssFormatConvertStcToRaw(ttMplsKey,&ttMplsStc1, NULL, rawData);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat1UT Stc to Raw failed rc %d\n", rc);
        return rc;
    }

    /* compare raw data with sample */
    cpssOsMemSet(rawDataSample, 0, sizeof(rawDataSample));
    rc = prvGetTtMplsRaw(&ttMplsStc1,rawDataSample);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat1UT Stc Raw Sample failed rc %d\n", rc);
        return rc;
    }

    /* compare Raw with Raw */
    result = cpssOsMemCmp(&rawData,&rawDataSample,sizeof(rawDataSample));
    if(result != 0)
    {
        cpssOsPrintf("prvCpssFormat1UT Raw != Raw %d\n", result);
        return GT_FAIL;
    }

    /* clean STC data */
    cpssOsMemSet(&stcData, 0, sizeof(stcData));

    rc = cpssFormatConvertRawToStc(ttMplsKey,rawData, &stcData, NULL);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat1UT Raw to Stc failed rc %d\n", rc);
        return rc;
    }

    /* compare STC with STC */
    result = cpssOsMemCmp(&stcData,&ttMplsStc1,sizeof(stcData));
    if(result != 0)
    {
        cpssOsPrintf("prvCpssFormat1UT Stc != Stc %d\n", result);
        return GT_FAIL;
    }

    return rc;
}

static CPSS_DXCH_TTI_IPV4_RULE_STC  ttiRuleIpv4Stc1 = {
   {0x1 /* must be 1 */, GT_TRUE, 0x17, {{0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa}}, 0x11, GT_FALSE, GT_FALSE, 0x5, 0x10},
   0x1, {0x132887ff/*{0x13, 0x28, 0x87, 0xff}*/}, {0x001188d1/*{0x00, 0x11, 0x88, 0xd1}*/}, GT_TRUE
};

GT_STATUS prvCpssFormat2UT(void)
{
    GT_U32                                  rawData[10];
    GT_U32                                  rawDataSample[10];
    CPSS_DXCH_TTI_IPV4_RULE_STC             stcData;
    GT_STATUS rc;
    int       result;
    /* convert STC to Raw */
    cpssOsMemSet(rawData, 0, sizeof(rawData));

    rc = cpssFormatConvertStcToRaw(ttiRuleIpv4,&ttiRuleIpv4Stc1, NULL, rawData);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat2UT Stc to Raw failed rc %d\n", rc);
        return rc;
    }

    /* compare raw data with sample */
    cpssOsMemSet(rawDataSample, 0, sizeof(rawDataSample));
    rc = prvTtiIpv4HwGet(&ttiRuleIpv4Stc1,rawDataSample);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat2UT Stc Raw Sample failed rc %d\n", rc);
        return rc;
    }

    /* compare Raw with Raw */
    result = cpssOsMemCmp(&rawData,&rawDataSample,sizeof(rawDataSample));
    if(result != 0)
    {
        cpssOsPrintf("prvCpssFormat2UT Raw != Raw %d\n", result);
        return GT_FAIL;
    }

    cpssOsMemSet(&stcData, 0, sizeof(stcData));

    rc = cpssFormatConvertRawToStc(ttiRuleIpv4,rawData, &stcData, NULL);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat2UT Raw to Stc failed rc %d\n", rc);
        return rc;
    }

    /* compare STC with STC */
    result = cpssOsMemCmp(&stcData,&ttiRuleIpv4Stc1,sizeof(stcData));
    if(result != 0)
    {
        cpssOsPrintf("prvCpssFormat2UT Stc != Stc %d\n", result);
        return GT_FAIL;
    }

    return rc;
}

/* TTI Action:
typedef struct
{
    GT_BOOL                                 tunnelTerminate;
    CPSS_DXCH_TTI_PASSENGER_TYPE_ENT        passengerPacketType;
    GT_BOOL                                 copyTtlFromTunnelHeader;

    CPSS_PACKET_CMD_ENT                     command;

    CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT      redirectCommand;
    CPSS_INTERFACE_INFO_STC                 egressInterface;
    GT_BOOL                                 tunnelStart;
    GT_U32                                  tunnelStartPtr;
    GT_U32                                  routerLookupPtr;
    GT_U32                                  vrfId;
    GT_BOOL                                 targetIsTrunk;
    GT_U8                                   virtualSrcPort;
    GT_U8                                   virtualSrcdev;
    GT_BOOL                                 useVidx;

    GT_BOOL                                 sourceIdSetEnable;
    GT_U32                                  sourceId;

    CPSS_DXCH_TTI_VLAN_COMMAND_ENT          vlanCmd;
    GT_U16                                  vlanId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT vlanPrecedence;
    GT_BOOL                                 nestedVlanEnable;

    GT_BOOL                                 bindToPolicer;
    GT_U32                                  policerIndex;

    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT         qosTrustMode;
    GT_U32                                  qosProfile;
    CPSS_DXCH_TTI_MODIFY_UP_ENT             modifyUpEnable;
    CPSS_DXCH_TTI_MODIFY_DSCP_ENT           modifyDscpEnable;
    GT_U32                                  up;
    GT_BOOL                                 remapDSCP;

    GT_BOOL                                 mirrorToIngressAnalyzerEnable;
    CPSS_NET_RX_CPU_CODE_ENT                userDefinedCpuCode;
    GT_BOOL                                 matchCounterEnable;
    GT_U32                                  matchCounterIndex;
    GT_BOOL                                 vntl2Echo;
    GT_BOOL                                 bridgeBypass;
    GT_BOOL                                 actionStop;

} CPSS_DXCH_TTI_ACTION_STC;

*/


static CPSS_DXCH_TTI_ACTION_STC           ttiAction1 =
{
    GT_TRUE,
    CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E,
    GT_FALSE,
    CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
    CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E,
    {CPSS_INTERFACE_TRUNK_E, {0x0, 0x0}, 0x55, 0, 0},
    GT_TRUE,
    0xa5a,
    0,
    0,
    0,
    GT_TRUE,
    0x1a,
    CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E,
    0xEC1,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E,
    GT_FALSE,
    GT_TRUE,
    0x208,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E,
    CPSS_DXCH_TTI_QOS_UNTRUST_E,
    0x62,
    CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E,
    CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E,
    0x5,
    GT_FALSE,
    GT_FALSE,
    CPSS_NET_FIRST_USER_DEFINED_E + 20,
    GT_FALSE,
    GT_TRUE,
    GT_FALSE
};

/* Value conversion tables */
static CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC ttiPassgPckTypeCnvArr[] =
{
      {CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E,             0},
      {CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E,             1},
      {CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E,     2},
      {CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E,  3},
      CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_TAB_END_CNS
};

static CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC ttiAssignAttrCnvArr[] =
{
      {CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E,  1},
      {CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E,  0},
      CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_TAB_END_CNS
};

static CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC ttiModifyAttrCnvArr[] =
{
      {CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E,  0},
      {CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E,  1},
      {CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,  2},
      CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_TAB_END_CNS
};

static CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC ttiVlanCmdCnvArr[] =
{
      {CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E,      0},
      {CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E,    1},
      {CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E,      2},
      {CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E,         3},
      CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_TAB_END_CNS
};

static CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC ttiQosTrustCnvArr[] =
{
      {CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E,     0},
      {CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E,     1},
      {CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E,     2},
      {CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E,  3},
      {CPSS_DXCH_TTI_QOS_UNTRUST_E,           4},
      {CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E,    5},
      CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_TAB_END_CNS
};


/* function for TTI Action command conversion */
static GT_STATUS prvTtiCmdCnv (
    IN  GT_BOOL     stcToRaw,
    IN  GT_U32      srcValue,
    OUT GT_U32      *dstValuePtr
)
{
    if(stcToRaw == GT_TRUE)
    {
        switch (srcValue)
        {
            case CPSS_PACKET_CMD_FORWARD_E:
                *dstValuePtr = 0;
                break;
            case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
                *dstValuePtr = 1;
                break;
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                *dstValuePtr = 2;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                *dstValuePtr = 3;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                *dstValuePtr = 4;
                break;
            default:
                return GT_NOT_FOUND;
        }
    }
    else
    {
        switch (srcValue)
        {
            case 0:
                *dstValuePtr = CPSS_PACKET_CMD_FORWARD_E;
                break;
            case 1:
                *dstValuePtr = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
                break;
            case 2:
                *dstValuePtr = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
                break;
            case 3:
                *dstValuePtr = CPSS_PACKET_CMD_DROP_HARD_E;
                break;
            case 4:
                *dstValuePtr = CPSS_PACKET_CMD_DROP_SOFT_E;
                break;
            default:
                return GT_NOT_FOUND;
        }
    }
    return GT_OK;
}

/* function for TTI Action CPU code conversion */
static GT_STATUS prvTtiCpuCodeCnv (
    IN  GT_BOOL     stcToRaw,
    IN  GT_U32      srcValue,
    OUT GT_U32      *dstValuePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;
    CPSS_NET_RX_CPU_CODE_ENT               cpuCode;

    if(stcToRaw == GT_TRUE)
    {
        if ((srcValue >= CPSS_NET_FIRST_USER_DEFINED_E) &&
            (srcValue <= CPSS_NET_LAST_USER_DEFINED_E))
        {
            rc = prvCpssDxChNetIfCpuToDsaCode(srcValue,
                                         &dsaCpuCode);
            if(rc != GT_OK)
            {
                return GT_NOT_FOUND;
            }

            *dstValuePtr = (GT_U32)dsaCpuCode;
        }
        else
            return GT_NOT_FOUND;
    }
    else
    {
        rc = prvCpssDxChNetIfDsaToCpuCode (srcValue, &cpuCode);
        if(rc != GT_OK)
        {
            return GT_NOT_FOUND;
        }

        *dstValuePtr = (GT_U32)cpuCode;
    }
#else
    if(stcToRaw == GT_TRUE)
    {
        if ((srcValue >= CPSS_NET_FIRST_USER_DEFINED_E) &&
            (srcValue <= CPSS_NET_LAST_USER_DEFINED_E))
        {
            *dstValuePtr = PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E + ((srcValue - CPSS_NET_FIRST_USER_DEFINED_E) % 64);
        }
        else
            return GT_NOT_FOUND;
    }
    else
    {
        *dstValuePtr = CPSS_NET_FIRST_USER_DEFINED_E + (srcValue - PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E);
    }
#endif
    return GT_OK;
}

#define TTI_ACTION  CPSS_DXCH_TTI_ACTION_STC

/* conditions for TTI Action redirect command */
static CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC ttiRedirectCondRouter =
CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(TTI_ACTION, redirectCommand,
                                        0xff, CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E);

static CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC ttiRedirectCondVrf =
CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(TTI_ACTION, redirectCommand,
                                        0xff, CPSS_DXCH_TTI_VRF_ID_ASSIGN_E);

static CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC ttiRedirectCondEgr =
CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(TTI_ACTION, redirectCommand,
                                        0xff, CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E);

/* CPSS_INTERFACE_INFO_STC structure conditions */

/* interface type is not multicast - neither VIDX nor VLAN
   check bit 1 is 0 */
static CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC infIsNotMc =
  CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(CPSS_INTERFACE_INFO_STC, type, 0x2, 0);

static CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC infIsVidx =
  CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(CPSS_INTERFACE_INFO_STC, type, 0xff, CPSS_INTERFACE_VIDX_E);

static CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC infIsVid =
  CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(CPSS_INTERFACE_INFO_STC, type, 0xff, CPSS_INTERFACE_VID_E);

static CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC infIsPort =
  CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(CPSS_INTERFACE_INFO_STC, type, 0xff, CPSS_INTERFACE_PORT_E);

static CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC infIsTrunk =
  CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(CPSS_INTERFACE_INFO_STC, type, 0xff, CPSS_INTERFACE_TRUNK_E);

/* Egress interface convertion table */
static CPSS_FORMAT_CONVERT_FIELD_INFO_STC ttiActionEggrIf[] =
{
    /* the type field is splitted to useVidx and isTrunk */
    /* bit 1 is used for useVidx */
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_MAC(CPSS_INTERFACE_INFO_STC, type,  46, 1, 1),

    /* bit 0 is used for isTrunk with condition that useVidx = 0 */
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_COND_MAC (CPSS_INTERFACE_INFO_STC, type,  34, 1, 0, &infIsNotMc),

    /* VIDX interface */
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_COND_MAC(CPSS_INTERFACE_INFO_STC, vidx,  34, 12, &infIsVidx),

    /* VID interface - just write constant 0xFFF */
    CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_COND_MAC(0xFFF, 34, 12, &infIsVid, 0),

    /* Trunk interface */
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_COND_MAC(CPSS_INTERFACE_INFO_STC, trunkId,  35, 7, &infIsTrunk),

    /* Port interface */
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_COND_MAC(CPSS_INTERFACE_INFO_STC, devPort.portNum,  35, 6, &infIsPort),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_COND_MAC(CPSS_INTERFACE_INFO_STC, devPort.devNum,  41, 5, &infIsPort),

    CPSS_FORMAT_CONVERT_FIELD_INFO_END_MAC
};

/* TTI Action conversion table */
static CPSS_FORMAT_CONVERT_FIELD_INFO_STC ttiAction[] =
{
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, tunnelTerminate,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  96, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_MAC (TTI_ACTION, passengerPacketType, 89, 2, 0, ttiPassgPckTypeCnvArr),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, copyTtlFromTunnelHeader,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  91, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_FUNC_MAC (TTI_ACTION, command, 0, 3, 0, prvTtiCmdCnv),

    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, sourceIdSetEnable,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  113, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC    (TTI_ACTION, sourceId,  114, 5),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_MAC (TTI_ACTION, vlanCmd, 49, 2, 0, ttiVlanCmdCnvArr),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC    (TTI_ACTION, vlanId,  51, 12),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_MAC (TTI_ACTION, vlanPrecedence, 47, 1, 0, ttiAssignAttrCnvArr),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, nestedVlanEnable,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  48, 1),

    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, bindToPolicer,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  64, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_MAC(TTI_ACTION, policerIndex,  65, 8, 0),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_MAC(TTI_ACTION, policerIndex,  106, 4, 8),

    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_MAC (TTI_ACTION, qosPrecedence, 12, 1, 0, ttiAssignAttrCnvArr),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_MAC (TTI_ACTION, qosTrustMode, 86, 3, 0, ttiQosTrustCnvArr),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC    (TTI_ACTION, qosProfile,  14, 7),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_MAC (TTI_ACTION, modifyUpEnable, 23, 2, 0, ttiModifyAttrCnvArr),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_MAC (TTI_ACTION, modifyDscpEnable, 21, 2, 0, ttiModifyAttrCnvArr),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC    (TTI_ACTION, up,  93, 3),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, remapDSCP,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  63, 1),

    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, mirrorToIngressAnalyzerEnable,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  11, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_FUNC_MAC (TTI_ACTION, userDefinedCpuCode, 3, 8, 0, prvTtiCpuCodeCnv),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, vntl2Echo,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  73, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, bridgeBypass,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  92, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC (TTI_ACTION, actionStop,  CPSS_FORMAT_CONVERT_FIELD_BOOL_E,  97, 1),

    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_MAC (TTI_ACTION, redirectCommand,  32, 2, 0),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_MAC (TTI_ACTION, redirectCommand,  119, 1, 2),

    /* router redirect command */
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_COND_MAC (TTI_ACTION, routerLookupPtr,  34, 13, 0, &ttiRedirectCondRouter),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_COND_MAC (TTI_ACTION, routerLookupPtr,  120, 2, 13, &ttiRedirectCondRouter),

    /* VRF ID redirect command */
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_COND_MAC(TTI_ACTION, vrfId,  34, 12, &ttiRedirectCondVrf),

    /* Egress interface redirection command */
    /* Save condition to context            */
    CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_CONDITION_MAC(&ttiRedirectCondEgr),
    CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCTURE_GEN_MAC(TTI_ACTION, egressInterface, 1 /* use condition */, ttiActionEggrIf),
    CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_CONTEXT_COND_MAC(TTI_ACTION, tunnelStart,CPSS_FORMAT_CONVERT_FIELD_BOOL_E, 74, 1),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_CONTEXT_COND_MAC(TTI_ACTION, tunnelStartPtr, 75, 10, 0),
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_CONTEXT_COND_MAC(TTI_ACTION, tunnelStartPtr, 110, 3, 10),

    CPSS_FORMAT_CONVERT_FIELD_INFO_END_MAC
};

static GT_STATUS prvCpssFormat3_1UTint(CPSS_DXCH_TTI_ACTION_STC * inTtiActionPtr)
{
    GT_U32                                  rawData[10];
    GT_STATUS rc;

    /* convert STC to Raw */
    cpssOsMemSet(rawData, 0, sizeof(rawData));

    rc = cpssFormatConvertStcToRaw(ttiAction,inTtiActionPtr, NULL, rawData);

    return rc;
}

static GT_STATUS prvCpssFormat3UTint(CPSS_DXCH_TTI_ACTION_STC * inTtiActionPtr)
{
    GT_U32                                  rawData[10];
    GT_U32                                  rawDataSample[10];
    CPSS_DXCH_TTI_ACTION_STC                stcData;
    GT_STATUS rc;
    int       result;

    /* convert STC to Raw */
    cpssOsMemSet(rawData, 0, sizeof(rawData));

    rc = cpssFormatConvertStcToRaw(ttiAction,inTtiActionPtr, NULL, rawData);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT Stc to Raw failed rc %d\n", rc);
        return rc;
    }

    /* compare raw data with sample */
    cpssOsMemSet(rawDataSample, 0, sizeof(rawDataSample));
    rc = ttiActionLogic2HwFormat(0, inTtiActionPtr, rawDataSample);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT Stc Raw Sample failed rc %d\n", rc);
        return rc;
    }

    /* compare Raw with Raw */
    result = cpssOsMemCmp(&rawData,&rawDataSample,sizeof(rawDataSample));
    if(result != 0)
    {
        cpssOsPrintf("prvCpssFormat3UT Raw != Raw %d\n", result);
        return GT_FAIL;
    }

    cpssOsMemSet(&stcData, 0, sizeof(stcData));

    rc = cpssFormatConvertRawToStc(ttiAction, rawData, &stcData, NULL);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT Raw to Stc failed rc %d\n", rc);
        return rc;
    }

    /* compare STC with STC */
    result = cpssOsMemCmp(&stcData,inTtiActionPtr,sizeof(stcData));
    if(result != 0)
    {
        cpssOsPrintf("prvCpssFormat3UT Stc != Stc %d\n", result);
        return GT_FAIL;
    }

     return rc;
}

GT_STATUS prvCpssFormat3UT(void)
{
    GT_STATUS rc;

    CPSS_DXCH_TTI_ACTION_STC ttiAction = ttiAction1;

    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT trunk interface case failure\n", rc);
        return rc;
    }

    /* use VIDX interface */
    cpssOsMemSet(&ttiAction.egressInterface, 0, sizeof (ttiAction.egressInterface));
    ttiAction.egressInterface.type = CPSS_INTERFACE_VIDX_E;
    ttiAction.egressInterface.vidx = 0x7a5;

    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT vidx interface case failure\n", rc);
        return rc;
    }

    /* use Port interface */
    cpssOsMemSet(&ttiAction.egressInterface, 0, sizeof (ttiAction.egressInterface));
    ttiAction.egressInterface.type = CPSS_INTERFACE_PORT_E;
    ttiAction.egressInterface.devPort.devNum = 0x13;
    ttiAction.egressInterface.devPort.portNum = 0x31;

    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT port interface failure\n", rc);
        return rc;
    }

    /* use VRF1 redirection */
    cpssOsMemSet(&ttiAction.egressInterface, 0, sizeof (ttiAction.egressInterface));
    ttiAction.redirectCommand = CPSS_DXCH_TTI_VRF_ID_ASSIGN_E;
    ttiAction.vrfId = 0xa5a;
    ttiAction.tunnelStart = GT_FALSE;
    ttiAction.tunnelStartPtr = 0;

    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT VRF1 redirection case failure\n", rc);
        return rc;
    }

    ttiAction.vrfId = 0x5a5;
    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT VRF2 redirection case failure\n", rc);
        return rc;
    }


    /* use Router redirection */
    ttiAction.redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
    ttiAction.vrfId = 0;
    ttiAction.routerLookupPtr = 0x7AA;

    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT Router1 redirection case failure\n", rc);
        return rc;
    }

    ttiAction.routerLookupPtr = 0x555;
    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT Router2 redirection case failure\n", rc);
        return rc;
    }

    /* no redirection */
    ttiAction.redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;
    ttiAction.routerLookupPtr = 0;

    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat3UT No redirection case failure\n", rc);
        return rc;
    }

    return rc;
}

/* check field values UT */
GT_STATUS prvCpssFormat5UT(void)
{
    GT_STATUS rc;

    CPSS_DXCH_TTI_ACTION_STC ttiAction = ttiAction1;

    rc = prvCpssFormat3UTint(&ttiAction);

    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat5UT trunk interface case failure\n", rc);
        return rc;
    }

    /* check trunk id 7 bits */
    ttiAction.egressInterface.trunkId = 0x80;
    rc = prvCpssFormat3_1UTint(&ttiAction);
    if(rc != GT_BAD_PARAM)
    {
        cpssOsPrintf("prvCpssFormat5UT trunk interface case failure %d\n", rc);
        return rc;
    }

    /* set maximal value of trunk id */
    ttiAction.egressInterface.trunkId = 0x7F;
    rc = prvCpssFormat3UTint(&ttiAction);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat5UT trunk interface case failure\n", rc);
        return rc;
    }

    /* set wrong passenger packet type */
    ttiAction.passengerPacketType =  CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E + 1;
    rc = prvCpssFormat3_1UTint(&ttiAction);
    if(rc != GT_BAD_PARAM)
    {
        cpssOsPrintf("prvCpssFormat5UT passenger packet type failure %d\n", rc);
        return rc;
    }

    /* set acceptable passenger packet type */
    ttiAction.passengerPacketType =  CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E;
    rc = prvCpssFormat3UTint(&ttiAction);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat5UT passenger packet type failure failure\n", rc);
        return rc;
    }

    /* set wrong command */
    ttiAction.command = 5;
    rc = prvCpssFormat3_1UTint(&ttiAction);
    if(rc != GT_BAD_PARAM)
    {
        cpssOsPrintf("prvCpssFormat5UT command failure %d\n", rc);
        return rc;
    }

    /* set acceptable passenger packet type */
    ttiAction.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    rc = prvCpssFormat3UTint(&ttiAction);
    if(rc != GT_OK)
    {
        cpssOsPrintf("prvCpssFormat5UT command failure failure\n", rc);
        return rc;
    }

    return GT_OK;
}

GT_STATUS prvCpssFormatAllUT(void)
{
    GT_STATUS rc;

    /* check TT MPLS Entry */
    rc = prvCpssFormat1UT();
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check TTI IPV4 entry with substructure */
    rc = prvCpssFormat2UT();
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check TTI Action with substructure, conditions, convertions e.t.c */
    rc = prvCpssFormat3UT();
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check field values validations */
    rc = prvCpssFormat5UT();
    if(rc != GT_OK)
    {
        return rc;
    }

    return rc;
}

/* performance testing */
GT_STATUS prvCpssFormat4UT(int loopsNum)
{
    GT_STATUS rc;
    GT_U32    startTick, endTick, formatTicks, classicTicks;
    GT_U32    rawData[20];
    int  ii;

    CPSS_DXCH_TTI_ACTION_STC ttiActionStc = ttiAction1;

    /* override input argument */
    loopsNum = UT_FORMAT_4_MAX_NUM_OF_LOOPS_CNS;

    startTick = cpssOsTickGet();
    for(ii = 0 ; ii < loopsNum; ii++)
    {
        /* convert STC to Raw */
        cpssOsMemSet(rawData, 0, sizeof(GT_U32)*TTI_ACTION_SIZE_CNS);

        rc = cpssFormatConvertStcToRaw(ttiAction,&ttiActionStc, NULL, rawData);
        if(rc != GT_OK)
        {
            cpssOsPrintf("prvCpssFormat4UT Stc to Raw failed rc %d\n", rc);
            return rc;
        }

    }
    endTick = cpssOsTickGet();
    formatTicks = endTick - startTick;

    startTick = cpssOsTickGet();
    for(ii = 0 ; ii < loopsNum; ii++)
    {
        rc = ttiActionLogic2HwFormat(0, &ttiActionStc, rawData);
        if(rc != GT_OK)
        {
            cpssOsPrintf("prvCpssFormat4UT Stc Raw Sample failed rc %d\n", rc);
            return rc;
        }

    }
    endTick = cpssOsTickGet();
    classicTicks = endTick - startTick;

    cpssOsPrintf("TTI Action conversion performance test\n");
    cpssOsPrintf("Loops Number %d  Format ticks %d  Classic Ticks %d\n",
                 loopsNum, formatTicks, classicTicks);

    startTick = cpssOsTickGet();
    for(ii = 0 ; ii < loopsNum; ii++)
    {
        /* convert STC to Raw */
        cpssOsMemSet(rawData, 0, sizeof(GT_U32)*TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS);

        rc = cpssFormatConvertStcToRaw(ttMplsKey,&ttMplsStc1, NULL, rawData);
        if(rc != GT_OK)
        {
            cpssOsPrintf("prvCpssFormat4UT Stc to Raw failed rc %d\n", rc);
            return rc;
        }

    }
    endTick = cpssOsTickGet();
    formatTicks = endTick - startTick;

    startTick = cpssOsTickGet();
    for(ii = 0 ; ii < loopsNum; ii++)
    {
        rc = prvGetTtMplsRaw(&ttMplsStc1,rawData);
        if(rc != GT_OK)
        {
            cpssOsPrintf("prvCpssFormat4UT Stc Raw Sample failed rc %d\n", rc);
            return rc;
        }

    }
    endTick = cpssOsTickGet();
    classicTicks = endTick - startTick;
    cpssOsPrintf("TT MPLS Key conversion performance test\n");
    cpssOsPrintf("Loops Number %d  Format ticks %d  Classic Ticks %d\n",
                 loopsNum, formatTicks, classicTicks);


    return GT_OK;

}

/*   CPSS Functions */
/*******************************************************************************
* tunnelTermConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       tunnelType        - type of the tunnel; valid options:
*                           CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                           CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                           CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_X_OVER_MPLS_E
*       convertMask       - whether to convert config or config mask
*       logicFormatPtr    - points to tunnel termination configuration in
*                           logic format
*
* OUTPUTS:
*       hwFormatArray     - the configuration in HW format (6 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/

static GT_STATUS tunnelTermConfigLogic2HwFormat
(
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  GT_BOOL                             convertMask,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *logicFormatPtr,
    OUT GT_U32                              *hwFormatArray
)
{
    GT_BOOL     isXoverMplsTunnel;  /* if tunnel type is X-over-MPLS tunnel */
    GT_U32      i;                  /* index */

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS);

    /* classify tunnel type */
    switch (tunnelType)
    {
    case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
    case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
    case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
    case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:
        isXoverMplsTunnel = GT_FALSE;
        break;
    case CPSS_TUNNEL_X_OVER_MPLS_E:
        isXoverMplsTunnel = GT_TRUE;
        break;
    default:
        return GT_BAD_PARAM;
    }


    /* zero out hw format */
    for (i = 0 ; i < TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS ; i++)
        hwFormatArray[i] = 0;

    /* convert X-over-IPv4 tunnel termination config to hw format */
    if (isXoverMplsTunnel == GT_FALSE)
    {
        /* handle word 0 (bits 0-31) */
        hwFormatArray[0] |= (logicFormatPtr->ipv4Cfg.srcPortTrunk & 0x7f) |
            ((logicFormatPtr->ipv4Cfg.srcIsTrunk & 0x1) << 7) |
            ((logicFormatPtr->ipv4Cfg.srcDev & 0x1f) << 8) |
            ((logicFormatPtr->ipv4Cfg.vid & 0xfff) << 14) |
            ((logicFormatPtr->ipv4Cfg.macDa.arEther[5] & 0x3f) << 26);

        /* handle word 1 (bits 32-63) */
        hwFormatArray[1] |= ((logicFormatPtr->ipv4Cfg.macDa.arEther[5] >> 6) & 0x3) |
            (logicFormatPtr->ipv4Cfg.macDa.arEther[4] << 2) |
            (logicFormatPtr->ipv4Cfg.macDa.arEther[3] << 10) |
            (logicFormatPtr->ipv4Cfg.macDa.arEther[2] << 18) |
            ((logicFormatPtr->ipv4Cfg.macDa.arEther[1] & 0x3f) << 26);

        /* handle word 2 (bits 64- 95) */
        hwFormatArray[2] |= ((logicFormatPtr->ipv4Cfg.macDa.arEther[1] >> 6) & 0x3) |
            (logicFormatPtr->ipv4Cfg.macDa.arEther[0] << 2) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[3] << 16) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[2] << 24);

        /* add ipv4 tunnel type to word 2 */
        switch (tunnelType)
        {
        case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
            hwFormatArray[2] |= (0x1 << 11);
            break;
        case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
            hwFormatArray[2] |= (0x1 << 12);
            break;
        case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
            hwFormatArray[2] |= (0x1 << 13);
            break;
        case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:
            hwFormatArray[2] |= (0x1 << 14);
            break;
        default:
            /* other type aren't possible when isXoverIpv4Tunnel == GT_TRUE */
            /* the default case is to prevent compiler warning */
            break;
        }

        /* tunnel type bit is 0 for ipv4 tunnel but it can't be masked */
        if (convertMask == GT_TRUE)
            hwFormatArray[2] |= (0x1 << 10);

        /* handle word 3 (bits 94-127) */
        hwFormatArray[3] |= (logicFormatPtr->ipv4Cfg.srcIp.arIP[1]) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[0] << 8) |
            (0x1 << 31); /* mark TCAM rule as tunnel termination */

        /* handle word 4 (bits 128-159) */
        hwFormatArray[4] |= (logicFormatPtr->ipv4Cfg.destIp.arIP[3]) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[2] << 8) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[1] << 16) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[0] << 24);

        /* word 5 (bits 160-191) - reserved */
    }
    /* convert X-over-MPLS tunnel termination config to hw format */
    else
    {
        /* handle word 0 (bits 0-31) */
        hwFormatArray[0] |= (logicFormatPtr->mplsCfg.srcPortTrunk & 0x7f) |
            ((logicFormatPtr->mplsCfg.srcIsTrunk & 0x1) << 7) |
            ((logicFormatPtr->mplsCfg.srcDev & 0x1f) << 8) |
            ((logicFormatPtr->mplsCfg.vid & 0xfff) << 14) |
            ((logicFormatPtr->mplsCfg.macDa.arEther[5] & 0x3f) << 26);

        /* handle word 1 (bits 32-63) */
        hwFormatArray[1] |= ((logicFormatPtr->mplsCfg.macDa.arEther[5] >> 6) & 0x3) |
            (logicFormatPtr->mplsCfg.macDa.arEther[4] << 2) |
            (logicFormatPtr->mplsCfg.macDa.arEther[3] << 10) |
            (logicFormatPtr->mplsCfg.macDa.arEther[2] << 18) |
            ((logicFormatPtr->mplsCfg.macDa.arEther[1] & 0x3f) << 26);

        /* handle word 2 (bits 64- 95) */
        hwFormatArray[2] |= ((logicFormatPtr->mplsCfg.macDa.arEther[1] >> 6) & 0x3) |
            (logicFormatPtr->mplsCfg.macDa.arEther[0] << 2) |
            (0x1 << 10) | /* tunnel key type == MPLS, can't be masked */
            ((logicFormatPtr->mplsCfg.exp2 & 0x7) << 12) |
            ((logicFormatPtr->mplsCfg.sBit2 & 0x1) << 15) |
            ((logicFormatPtr->mplsCfg.label2 & 0xffff) << 16);

        /* handle word 3 (bits 94-127) */
        hwFormatArray[3] |= ((logicFormatPtr->mplsCfg.label2 >> 16) & 0xf) |
            ((logicFormatPtr->mplsCfg.exp1 & 0x7) << 4) |
            ((logicFormatPtr->mplsCfg.sBit1 & 0x1) << 7) |
            ((logicFormatPtr->mplsCfg.label1 & 0xfffff) << 8) |
            (0x1 << 31); /* mark TCAM rule as tunnel termination */

        /* word 4 (bits 128-159) - reserved */
        /* word 5 (bits 160-191) - reserved */
    }

    return GT_OK;
}

static GT_STATUS prvGetTtMplsRaw(
   IN CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC   *stcDataPtr,
   OUT GT_U32                              *hwFormatArray)
{
   CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    logicFormat;
   GT_STATUS rc;

   logicFormat.mplsCfg = *stcDataPtr;

   rc = tunnelTermConfigLogic2HwFormat(
                CPSS_TUNNEL_X_OVER_MPLS_E, GT_FALSE,
                &logicFormat, hwFormatArray);

   return rc;

}

/*******************************************************************************
* ttiConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:  All DxCh3 devices
*
* INPUTS:
*       devNum            - device number
*       keytype           - type of the tunnel; valid options:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*       convertMask       - whether to convert config or config mask
*       logicFormatPtr    - points to tunnel termination configuration in
*                           logic format
*
* OUTPUTS:
*       hwFormatArray     - the configuration in HW format (6 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/

/* the size of tti macToMe table entry in words */
#define TTI_MAC2ME_SIZE_CNS             4


/* maximum value for IPv4 tunnel type field */
#define IPV4_TUNNEL_TYPE_MAX_CNS        7

/* maximum value for MPLS label field */
#define MPLS_LABEL_MAX_CNS              1048575

/* maximum value for MPLS labels field */
#define MPLS_LABELS_MAX_CNS             3

/* maximum value for MPLS protocol above MPLS field */
#define MPLS_PROTOCOL_ABOVE_MAX_CNS     2

/* maximum value for ETH cfi field */
#define ETH_CFI_MAX_CNS                 1

/* maximum value for vidx interface field */
#define VIDX_INTERFACE_MAX_CNS          4095

static GT_STATUS ttiConfigLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             convertMask,
    IN  CPSS_DXCH_TTI_RULE_UNT             *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    GT_BOOL                 srcIsTrunk;
    GT_U32                  srcPortTrunk;
    GT_BOOL                 isTagged;
    GT_BOOL                 dsaSrcIsTrunk;
    GT_U8                   dsaSrcPortTrunk;
    GT_U8                   dsaSrcDevice;

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TTI_RULE_SIZE_CNS);

    /* to avoid warnings */
    devNum = devNum;

    /* check ipv4/mpls/eth configuration pararmeters only for config, not for mask */
    if (convertMask == GT_FALSE)
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            if (logicFormatPtr->ipv4.common.pclId != 1)
                return GT_BAD_PARAM;
            srcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->ipv4.common.srcIsTrunk);
            srcPortTrunk = logicFormatPtr->ipv4.common.srcPortTrunk;
            isTagged = BOOL2BIT_MAC(logicFormatPtr->ipv4.common.isTagged);
            dsaSrcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->ipv4.common.dsaSrcIsTrunk);
            dsaSrcPortTrunk = logicFormatPtr->ipv4.common.dsaSrcPortTrunk;
            dsaSrcDevice = logicFormatPtr->ipv4.common.dsaSrcDevice;

            if (logicFormatPtr->ipv4.tunneltype > IPV4_TUNNEL_TYPE_MAX_CNS)
                return GT_BAD_PARAM;
            break;

        case CPSS_DXCH_TTI_KEY_MPLS_E:
            if (logicFormatPtr->mpls.common.pclId != 2)
                return GT_BAD_PARAM;
            srcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->mpls.common.srcIsTrunk);
            srcPortTrunk = logicFormatPtr->mpls.common.srcPortTrunk;
            isTagged = BOOL2BIT_MAC(logicFormatPtr->mpls.common.isTagged);
            dsaSrcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->mpls.common.dsaSrcIsTrunk);
            dsaSrcPortTrunk = logicFormatPtr->mpls.common.dsaSrcPortTrunk;
            dsaSrcDevice = logicFormatPtr->mpls.common.dsaSrcDevice;

            PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(logicFormatPtr->mpls.exp0);
            PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(logicFormatPtr->mpls.exp1);
            PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(logicFormatPtr->mpls.exp2);
            if (logicFormatPtr->mpls.label0 > MPLS_LABEL_MAX_CNS)
                return GT_BAD_PARAM;
            if (logicFormatPtr->mpls.label1 > MPLS_LABEL_MAX_CNS)
                return GT_BAD_PARAM;
            if (logicFormatPtr->mpls.label2 > MPLS_LABEL_MAX_CNS)
                return GT_BAD_PARAM;
            if (logicFormatPtr->mpls.numOfLabels > MPLS_LABELS_MAX_CNS)
                return GT_BAD_PARAM;
            if (logicFormatPtr->mpls.numOfLabels < MPLS_LABELS_MAX_CNS)
            {
                if (logicFormatPtr->mpls.protocolAboveMPLS > MPLS_PROTOCOL_ABOVE_MAX_CNS)
                    return GT_BAD_PARAM;
            }
            break;

        case CPSS_DXCH_TTI_KEY_ETH_E:
            if (logicFormatPtr->eth.common.pclId != 3)
                return GT_BAD_PARAM;
            srcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->eth.common.srcIsTrunk);
            srcPortTrunk = logicFormatPtr->eth.common.srcPortTrunk;
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->eth.common.vid);
            isTagged = BOOL2BIT_MAC(logicFormatPtr->eth.common.isTagged);
            dsaSrcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->eth.common.dsaSrcIsTrunk);
            dsaSrcPortTrunk = logicFormatPtr->eth.common.dsaSrcPortTrunk;
            dsaSrcDevice = logicFormatPtr->eth.common.dsaSrcDevice;

            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->eth.up0);
            if (logicFormatPtr->eth.cfi0 > ETH_CFI_MAX_CNS)
                return GT_BAD_PARAM;
            if (logicFormatPtr->eth.isVlan1Exists)
            {
                PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->eth.vid1);
                PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->eth.up1);
                if (logicFormatPtr->eth.cfi1 > ETH_CFI_MAX_CNS)
                    return GT_BAD_PARAM;
            }

            break;

        default:
            /* do nothing */
            break;
        }
    }

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:

        /* handle word 0 (bits 0-31) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,10,logicFormatPtr->ipv4.common.pclId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],10,8,logicFormatPtr->ipv4.common.srcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],18,1,BOOL2BIT_MAC(logicFormatPtr->ipv4.common.srcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],19,12,logicFormatPtr->ipv4.common.vid);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,BOOL2BIT_MAC(logicFormatPtr->ipv4.common.isTagged));

        /* handle word 1 (bits 32-63) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,8,logicFormatPtr->ipv4.common.mac.arEther[5]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],8,8,logicFormatPtr->ipv4.common.mac.arEther[4]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,8,logicFormatPtr->ipv4.common.mac.arEther[3]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],24,8,logicFormatPtr->ipv4.common.mac.arEther[2]);

        /* handle word 2 (bits 64-95) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,8,logicFormatPtr->ipv4.common.mac.arEther[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],8,8,logicFormatPtr->ipv4.common.mac.arEther[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,7,logicFormatPtr->ipv4.common.dsaSrcDevice);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,3,logicFormatPtr->ipv4.tunneltype);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],26,6,logicFormatPtr->ipv4.srcIp.arIP[3]);

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,2,(logicFormatPtr->ipv4.srcIp.arIP[3] >> 6));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],2,8,logicFormatPtr->ipv4.srcIp.arIP[2]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],10,8,logicFormatPtr->ipv4.srcIp.arIP[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],18,8,logicFormatPtr->ipv4.srcIp.arIP[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],26,6,logicFormatPtr->ipv4.destIp.arIP[3]);

        /* handle word 4 (bits 128-159) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],0,2,(logicFormatPtr->ipv4.destIp.arIP[3] >> 6));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],2,8,logicFormatPtr->ipv4.destIp.arIP[2]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],10,8,logicFormatPtr->ipv4.destIp.arIP[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],18,8,logicFormatPtr->ipv4.destIp.arIP[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],26,1,BOOL2BIT_MAC(logicFormatPtr->ipv4.isArp));

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->ipv4.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->ipv4.common.dsaSrcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:

        /* handle word 0 (bits 0-31) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,10,logicFormatPtr->mpls.common.pclId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],10,8,logicFormatPtr->mpls.common.srcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],18,1,BOOL2BIT_MAC(logicFormatPtr->mpls.common.srcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],19,12,logicFormatPtr->mpls.common.vid);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,BOOL2BIT_MAC(logicFormatPtr->mpls.common.isTagged));

        /* handle word 1 (bits 32-63) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,8,logicFormatPtr->mpls.common.mac.arEther[5]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],8,8,logicFormatPtr->mpls.common.mac.arEther[4]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,8,logicFormatPtr->mpls.common.mac.arEther[3]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],24,8,logicFormatPtr->mpls.common.mac.arEther[2]);

        /* handle word 2 (bits 64-95) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,8,logicFormatPtr->mpls.common.mac.arEther[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],8,8,logicFormatPtr->mpls.common.mac.arEther[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,7,logicFormatPtr->mpls.common.dsaSrcDevice);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,3,logicFormatPtr->mpls.exp0);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],26,3,logicFormatPtr->mpls.exp1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],29,3,logicFormatPtr->mpls.label0);

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,17,(logicFormatPtr->mpls.label0 >> 3));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],17,15,logicFormatPtr->mpls.label1);

        /* handle word 4 (bits 128-159) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],0,5,(logicFormatPtr->mpls.label1 >> 15));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],5,3,logicFormatPtr->mpls.exp2);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],8,20,logicFormatPtr->mpls.label2);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],28,2,logicFormatPtr->mpls.numOfLabels);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],30,2,logicFormatPtr->mpls.protocolAboveMPLS);

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->mpls.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->mpls.common.dsaSrcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:

        /* handle word 0 (bits 0-31) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,10,logicFormatPtr->eth.common.pclId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],10,8,logicFormatPtr->eth.common.srcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],18,1,BOOL2BIT_MAC(logicFormatPtr->eth.common.srcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],19,12,logicFormatPtr->eth.common.vid);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,BOOL2BIT_MAC(logicFormatPtr->eth.common.isTagged));

        /* handle word 1 (bits 32-63) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,8,logicFormatPtr->eth.common.mac.arEther[5]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],8,8,logicFormatPtr->eth.common.mac.arEther[4]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,8,logicFormatPtr->eth.common.mac.arEther[3]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],24,8,logicFormatPtr->eth.common.mac.arEther[2]);

        /* handle word 2 (bits 64-95) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,8,logicFormatPtr->eth.common.mac.arEther[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],8,8,logicFormatPtr->eth.common.mac.arEther[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,7,logicFormatPtr->eth.common.dsaSrcDevice);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,3,logicFormatPtr->eth.up0);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],26,1,logicFormatPtr->eth.cfi0);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],27,1,BOOL2BIT_MAC(logicFormatPtr->eth.isVlan1Exists));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],28,4,(logicFormatPtr->eth.vid1 >> 4));

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,8,logicFormatPtr->eth.vid1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],8,3,logicFormatPtr->eth.up1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],11,1,logicFormatPtr->eth.cfi1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],12,16,logicFormatPtr->eth.etherType);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],28,1,BOOL2BIT_MAC(logicFormatPtr->eth.macToMe));
         /* handle word 4 (bits 128-159) */
        /* reserved */

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->eth.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->eth.common.dsaSrcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */
        break;

    default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

static GT_STATUS prvTtiIpv4HwGet
(
    IN  CPSS_DXCH_TTI_IPV4_RULE_STC        *ipv4StcPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    CPSS_DXCH_TTI_RULE_UNT             logicFormat;
    GT_STATUS                           rc;

    logicFormat.ipv4 = *ipv4StcPtr;

    rc = ttiConfigLogic2HwFormat(0, CPSS_DXCH_TTI_KEY_IPV4_E, GT_FALSE,
                                    &logicFormat, hwFormatArray);

    return rc;
}

/* maximum value for QOS profile field */
#define QOS_PROFILE_MAX_CNS             127

/* maximum value for match counter index field */
#define MATCH_COUNTER_INDEX_MAX_CNS     16383

/* maximum value for LTT index field */
#define LTT_INDEX_MAX_CNS               32767

/* maximum value for VRF ID field */
#define VRF_ID_MAX_CNS                  4095

/* maximum value for policer index field */
#define POLICER_INDEX_MAX_CNS           1023

/* maximum value for QOS profile field */
#define QOS_PROFILE_MAX_CNS             127

/* maximum value for source id field */
#define SOURCE_ID_MAX_CNS               31


static GT_STATUS ttiActionLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_STC           *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    GT_U32      pktCommand;         /* tti packet forwarding command (hw format) */
    GT_U32      redirectCommand;    /* where to redirect the packet (hw format) */
    GT_U32      userDefinedCpuCode; /* user defined cpu code (hw format) */
    GT_U32      passengerPktType;   /* passenger packet type (hw format) */
    GT_U32      vlanPrecedence;     /* VLAN assignment precedence mode (hw format) */
    GT_U32      qosPrecedence;      /* QoS precedence mode (hw format) */
    GT_U32      modifyDSCP;         /* modify DSCP mode (hw format) */
    GT_U32      modifyUP;           /* modify user priority mode (hw format) */
    GT_U32      vlanCmd;            /* vlan command applied to packets matching the TTI (hw format) */
    GT_U32      qosTrustMode;       /* qos profile marking (hw format) */

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TTI_ACTION_SIZE_CNS);

    /* to avoid warnings */
    devNum = devNum;

    /* translate tunnel termination action forwarding command parameter */
    switch (logicFormatPtr->command)
    {
    case CPSS_PACKET_CMD_FORWARD_E:
        pktCommand = 0;
        break;
    case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
        pktCommand = 1;
        break;
    case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
        pktCommand = 2;
        break;
    case CPSS_PACKET_CMD_DROP_HARD_E:
        pktCommand = 3;
        break;
    case CPSS_PACKET_CMD_DROP_SOFT_E:
        pktCommand = 4;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        if ((logicFormatPtr->userDefinedCpuCode >= CPSS_NET_FIRST_USER_DEFINED_E) &&
            (logicFormatPtr->userDefinedCpuCode <= CPSS_NET_LAST_USER_DEFINED_E))
        {
#ifdef CHX_FAMILY
            GT_STATUS rc;
            rc = prvCpssDxChNetIfCpuToDsaCode(logicFormatPtr->userDefinedCpuCode,
                                         (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT*)(GT_U32)&userDefinedCpuCode);
            if (rc != GT_OK)
                return rc;
#else
            userDefinedCpuCode = PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E +
                                 ((logicFormatPtr->userDefinedCpuCode - CPSS_NET_FIRST_USER_DEFINED_E) % 64);
#endif /* (defined CHX_FAMILY) */
        }
        else
            return GT_BAD_PARAM;
    }
    else
        userDefinedCpuCode = 0;

    /* translate QoS precedence from action parameter */
    switch (logicFormatPtr->qosPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        qosPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        qosPrecedence = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if (logicFormatPtr->qosProfile > QOS_PROFILE_MAX_CNS)
        return GT_BAD_PARAM;

     /* translate tunnel termination action modify DSCP parameter */
    switch (logicFormatPtr->modifyDscpEnable)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
        modifyDSCP = 0;
        break;
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        modifyDSCP = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
        modifyDSCP = 2;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate tunnel termination action modify User Priority parameter */
    switch (logicFormatPtr->modifyUpEnable)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
        modifyUP = 0;
        break;
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        modifyUP = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
        modifyUP = 2;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate tunnel termination action redirect command parameter */
    switch (logicFormatPtr->redirectCommand)
    {
    case CPSS_DXCH_TTI_NO_REDIRECT_E:
        redirectCommand = 0;
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        redirectCommand = 1;
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
        redirectCommand = 2;
        if (logicFormatPtr->routerLookupPtr > LTT_INDEX_MAX_CNS)
            return GT_BAD_PARAM;
        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        redirectCommand = 4;
        if (logicFormatPtr->vrfId > VRF_ID_MAX_CNS)
            return GT_BAD_PARAM;
        break;
    default:
        return GT_BAD_PARAM;
    }

    switch (logicFormatPtr->egressInterface.type)
    {
    case CPSS_INTERFACE_PORT_E:
     case CPSS_INTERFACE_TRUNK_E:
        break;
    case CPSS_INTERFACE_VIDX_E:
        if (logicFormatPtr->egressInterface.vidx > VIDX_INTERFACE_MAX_CNS)
            return GT_BAD_PARAM;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate VLAN precedence from action parameter */
    switch (logicFormatPtr->vlanPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        vlanPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        vlanPrecedence = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate VLAN command from action parameter */
    switch (logicFormatPtr->vlanCmd)
    {
    case CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E:
        vlanCmd = 0;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
        vlanCmd = 1;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E:
        vlanCmd = 2;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
        vlanCmd = 3;
        break;
    default:
        return GT_BAD_PARAM;
    }

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->vlanId);

    if (logicFormatPtr->policerIndex > POLICER_INDEX_MAX_CNS)
        return GT_BAD_PARAM;

    /* translate tunnel termination action qos trust mode parameter */
    switch (logicFormatPtr->qosTrustMode)
    {
    case CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E:
        qosTrustMode = 0;
        break;
    case CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E:
        qosTrustMode = 1;
        break;
    case CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E:
        qosTrustMode = 2;
        break;
    case CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E:
        qosTrustMode = 3;
        break;
    case CPSS_DXCH_TTI_QOS_UNTRUST_E:
        qosTrustMode = 4;
        break;
    case CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E:
        qosTrustMode = 5;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate passenger packet type from action parameter */
    switch (logicFormatPtr->passengerPacketType)
    {
    case CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E:
        passengerPktType = 0;
        break;
    case CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E:
        passengerPktType = 1;
        break;
    case CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E:
        passengerPktType = 2;
        break;
    case CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E:
        passengerPktType = 3;
        break;
    default:
        return GT_BAD_PARAM;
    }

    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->up);

    if (logicFormatPtr->sourceId >SOURCE_ID_MAX_CNS)
        return GT_BAD_PARAM;


    /* handle word 0 (bits 0-31) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,3,pktCommand);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],3,8,userDefinedCpuCode);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],11,1,BOOL2BIT_MAC(logicFormatPtr->mirrorToIngressAnalyzerEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],12,1,qosPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],14,7,logicFormatPtr->qosProfile);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],21,2,modifyDSCP);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],23,2,modifyUP);

    /* handle word 1 (bits 32-63) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,2,redirectCommand); /* sets only 2 bits. the last bit handled later */

    switch (redirectCommand)
    {
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,1,0);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],3,6,logicFormatPtr-> egressInterface.devPort.portNum);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],9,5,logicFormatPtr->egressInterface.devPort.devNum);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],14,1,0);
            break;
        case CPSS_INTERFACE_TRUNK_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,1,1);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],3,11,logicFormatPtr->egressInterface.trunkId);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],14,1,0);
            break;
        case CPSS_INTERFACE_VIDX_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,12,logicFormatPtr->egressInterface.vidx);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],14,1,1);
            break;
        default:
            return GT_BAD_PARAM;
        }
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,13,logicFormatPtr->routerLookupPtr);
        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,12,logicFormatPtr->vrfId);
        break;
    default:
        /* do noting */
        break;
    }

    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],15,1,vlanPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,1,BOOL2BIT_MAC(logicFormatPtr->nestedVlanEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],17,2,vlanCmd);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],19,12,logicFormatPtr->vlanId);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],31,1,BOOL2BIT_MAC(logicFormatPtr->remapDSCP));

    /* handle word 2 (bits 64-95) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,1,BOOL2BIT_MAC(logicFormatPtr->bindToPolicer));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],1,8,logicFormatPtr->policerIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],9,1,BOOL2BIT_MAC(logicFormatPtr->vntl2Echo));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],10,1,BOOL2BIT_MAC(logicFormatPtr->tunnelStart));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],11,10,logicFormatPtr->tunnelStartPtr);
    /* bit 21 is reserved */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],22,3,qosTrustMode);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],25,2,passengerPktType);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],27,1,BOOL2BIT_MAC(logicFormatPtr->copyTtlFromTunnelHeader));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],28,1,BOOL2BIT_MAC(logicFormatPtr->bridgeBypass));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],29,3,logicFormatPtr->up);

    /* handle word 3 (bits 96-127) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,1,BOOL2BIT_MAC(logicFormatPtr->tunnelTerminate));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],1,1,BOOL2BIT_MAC(logicFormatPtr->actionStop));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],10,4,(logicFormatPtr->policerIndex >> 8));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],14,3,(logicFormatPtr->tunnelStartPtr >> 10));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],17,1,BOOL2BIT_MAC(logicFormatPtr->sourceIdSetEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],18,5,logicFormatPtr->sourceId);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],23,1,(redirectCommand >> 2));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],24,2,(logicFormatPtr->routerLookupPtr >> 13));

    return GT_OK;
}



UTF_SUIT_BEGIN_TESTS_MAC(cpssFormatConvert)
UTF_SUIT_DECLARE_TEST_MAC(prvCpssFormat1)
UTF_SUIT_DECLARE_TEST_MAC(prvCpssFormat2)
UTF_SUIT_DECLARE_TEST_MAC(prvCpssFormat3)
UTF_SUIT_DECLARE_TEST_MAC(prvCpssFormat4)
UTF_SUIT_DECLARE_TEST_MAC(prvCpssFormat5)
UTF_SUIT_END_TESTS_MAC(cpssFormatConvert)

