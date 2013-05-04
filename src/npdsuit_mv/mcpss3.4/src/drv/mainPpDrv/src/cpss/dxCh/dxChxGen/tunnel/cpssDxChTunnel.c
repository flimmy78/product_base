/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* cpssDxChTunnel.c
*
* DESCRIPTION:
*       CPSS tunnel implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>

/* the size of tunnel start entry in words */
#define TUNNEL_START_HW_SIZE_CNS       7

/* maximum index for tunnel start entry */
#define TUNNEL_START_MAX_INDEX_CNS   1023

/* the size of tunnel termination TCAM rule key/mask in words */
#define TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS   PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_LINE_RULE_SIZE_CNS

/* the size of tunnel termination TCAM action in words */
#define TUNNEL_TERMINATION_TCAM_ACTION_SIZE_CNS PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_LINE_ACTION_SIZE_CNS

/* maximum value for time to live (TTL) field */
#define TTL_MAX_CNS                 255

/* maximum value for MPLS label field */
#define MPLS_LABEL_MAX_CNS          1048575

#define PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(label)    \
    if((label) > MPLS_LABEL_MAX_CNS)                    \
        return GT_BAD_PARAM

/* maximum value for MPLS label stop bit field */
#define S_BIT_MAX_CNS               1

#define PRV_TUNNEL_START_MPLS_SBIT_CHECK_MAC(sBit)  \
    if((sBit) > S_BIT_MAX_CNS)                      \
        return GT_BAD_PARAM

/* maximum value for MPLS label exp field */
#define MPLS_EXP_MAX_CNS            7

#define PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(exp)    \
    if((exp) > MPLS_EXP_MAX_CNS)                    \
        return GT_BAD_PARAM

/* maximum value for trunk interface field */
#define TRUNK_INTERFACE_MAX_CNS     127

/* maximum value for vidx interface field */
#define VIDX_INTERFACE_MAX_CNS      4095

/* maximum value for QOS profile field */
#define QOS_PROFILE_MAX_CNS         127

/* maximum value for policer index field */
#define POLICER_INDEX_MAX_CNS       255

/* maximum value for tunnel termination match counter value */
#define MATCH_COUNTER_INDEX_MAX_CNS 31

/* macro to check line index */
#define LINE_INDEX_CHECK_MAC(devNum,index)                             \
    if(index >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart) \
        return GT_BAD_PARAM

#define PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(mode)                  \
    switch ((mode))                                                     \
    {                                                                   \
    case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E:                  \
    case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E:     \
        break;                                                          \
    default:                                                            \
        return GT_BAD_PARAM;                                            \
    }

#define PRV_TUNNEL_START_TTL_CHECK_MAC(ttl)     \
    if((ttl) > 255)                             \
        return GT_BAD_PARAM

#define PRV_TUNNEL_START_AUTO_TUNNEL_OFFSET_CHECK_MAC(offset)   \
    if((offset) > 12)                                           \
        return GT_BAD_PARAM

#define PRV_TUNNEL_START_CFI_CHECK_MAC(cfi)   \
    if((cfi) > 1)                             \
        return GT_BAD_PARAM

#define PRV_TUNNEL_START_CHECK_TTL_MODE_MAC(mode)                           \
        switch ((mode))                                                      \
        {                                                                    \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E:                   \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E:                     \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E:           \
            break;                                                           \
        default:                                                             \
            return GT_BAD_PARAM;                                             \
        }

#define PRV_TUNNEL_START_ISID_CHECK_MAC(iSid)       \
    if((iSid) > 16777215)                           \
        return GT_BAD_PARAM

#define PRV_TUNNEL_START_ITAG_RES_CHECK_MAC(iTagRes)    \
    if((iTagRes) > 15)                                     \
        return GT_BAD_PARAM

#define PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(mode,value) \
    switch ((mode))                                                         \
    {                                                                       \
    case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E:                      \
        (value) = 1;                                                        \
        break;                                                              \
    case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E:         \
        (value) = 0;                                                        \
        break;                                                              \
    default:  /* default set some value as the field must not be valid */   \
        (value) = 0;                                                        \
        break;                                                              \
    }

#define PRV_TUNNEL_START_CONVERT_TTL_MODE_TO_HW_FORMAT_MAC(mode,value)       \
        switch ((mode))                                                      \
        {                                                                    \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E:                   \
            (value) = 0;                                                     \
            break;                                                           \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E:                     \
            (value) = 1;                                                     \
            break;                                                           \
        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E:           \
            (value) = 2;                                                     \
            break;                                                           \
        default:  /* default set some value as the field must not be valid */\
            (value) = 0;                                                     \
            break;                                                           \
        }

/*******************************************************************************
* tunnelStartConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel start configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - physical device number
*       tunnelType        - type of the tunnel; valid options:
*                           CPSS_TUNNEL_X_OVER_IPV4_E
*                           CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_X_OVER_MPLS_E
*                           CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       logicFormatPtr    - points to tunnel start configuration
*
* OUTPUTS:
*       hwFormatArray     - The configuration in HW format (6 words).
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
static GT_STATUS tunnelStartConfigLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *logicFormatPtr,
    OUT GT_U32                              *hwFormatArray
)
{
    GT_U32      upMarkMode;         /* UP marking mode in hw format   */
    GT_U32      dscpMarkMode;       /* DSCP marking mode in hw format */
    GT_U32      exp1MarkMode;       /* EXP1 marking mode in hw format */
    GT_U32      exp2MarkMode;       /* EXP2 marking mode in hw format */
    GT_U32      exp3MarkMode;       /* EXP3 marking mode in hw format */
    GT_U32      ttlMode;            /* TTL mode in hw format          */
    GT_U32      iUpMarkMode;        /* iUP marking mode in hw format  */
    GT_U32      iDpMarkMode;        /* iDP marking mode in hw format  */

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TUNNEL_START_HW_SIZE_CNS);

    /********************/
    /* check parameters */
    /********************/

    switch (tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        /********************************/
        /* check IPv4 config parameters */
        /********************************/
        if (logicFormatPtr->ipv4Cfg.tagEnable == GT_TRUE)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->ipv4Cfg.vlanId);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv4Cfg.upMarkMode);
            if (logicFormatPtr->ipv4Cfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->ipv4Cfg.up);
        }
        PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->ipv4Cfg.dscpMarkMode);
        if (logicFormatPtr->ipv4Cfg.dscpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(logicFormatPtr->ipv4Cfg.dscp);
        PRV_TUNNEL_START_TTL_CHECK_MAC(logicFormatPtr->ipv4Cfg.ttl);
        if (logicFormatPtr->ipv4Cfg.autoTunnel == GT_TRUE)
            PRV_TUNNEL_START_AUTO_TUNNEL_OFFSET_CHECK_MAC(logicFormatPtr->ipv4Cfg.autoTunnelOffset);
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
            PRV_TUNNEL_START_CFI_CHECK_MAC(logicFormatPtr->ipv4Cfg.cfi);
        break;

    case CPSS_TUNNEL_X_OVER_MPLS_E:
        /********************************/
        /* check MPLS config parameters */
        /********************************/
        if (logicFormatPtr->mplsCfg.tagEnable == GT_TRUE)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->mplsCfg.vlanId);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.upMarkMode);
            if (logicFormatPtr->mplsCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mplsCfg.up);
        }
        PRV_TUNNEL_START_TTL_CHECK_MAC(logicFormatPtr->mplsCfg.ttl);
        if ((logicFormatPtr->mplsCfg.ttl == 0)
                && PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            PRV_TUNNEL_START_CHECK_TTL_MODE_MAC(
                logicFormatPtr->mplsCfg.ttlMode);
        }
        switch (logicFormatPtr->mplsCfg.numLabels)
        {
        case 3:
        case 2:
        case 1:
             /* 3 labels mode is supported for xCat and Lion */
            if (logicFormatPtr->mplsCfg.numLabels >= 3)
            {
                if(! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
                    return GT_BAD_PARAM;
                PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label3);
                PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp3MarkMode);
                if (logicFormatPtr->mplsCfg.exp3MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                    PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp3);
            }
            if (logicFormatPtr->mplsCfg.numLabels >= 2)
            {
                PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label2);
                PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp2MarkMode);
                if (logicFormatPtr->mplsCfg.exp2MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                    PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp2);
            }
            PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label1);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mplsCfg.exp1MarkMode);
            if (logicFormatPtr->mplsCfg.exp1MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp1);
            break;
        default:
            return GT_BAD_PARAM;
        }
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
            PRV_TUNNEL_START_CFI_CHECK_MAC(logicFormatPtr->mplsCfg.cfi);

        /* FEr#2785: The passenger Ethernet packet CRC cannot be retained in some cases */
         if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                                    PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E))
         {
             if (logicFormatPtr->mplsCfg.retainCRC == GT_TRUE)
                 return GT_NOT_SUPPORTED;
         }

        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        /*******************************/
        /* check MIM config parameters */
        /*******************************/
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

        if (logicFormatPtr->mimCfg.tagEnable == GT_TRUE)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->mimCfg.vlanId);
            PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.upMarkMode);
            if (logicFormatPtr->mimCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mimCfg.up);
        }
        PRV_TUNNEL_START_ISID_CHECK_MAC(logicFormatPtr->mimCfg.iSid);
        PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.iUpMarkMode);
        if (logicFormatPtr->mimCfg.iUpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mimCfg.iUp);
        PRV_TUNNEL_START_QOS_MARK_MODE_CHECK_MAC(logicFormatPtr->mimCfg.iDpMarkMode);
        if (logicFormatPtr->mimCfg.iDpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mimCfg.iDp);
        PRV_TUNNEL_START_ITAG_RES_CHECK_MAC(logicFormatPtr->mimCfg.iTagReserved);

        /* FEr#2785: The passenger Ethernet packet CRC cannot be retained in some cases */
         if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                                    PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E))
         {
             if (logicFormatPtr->mimCfg.retainCrc == GT_TRUE)
                 return GT_NOT_SUPPORTED;
         }

        break;
    default:
        return GT_BAD_PARAM;
    }

    /*****************************************/
    /* convert paramteres to hardware format */
    /*****************************************/

    switch (tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->ipv4Cfg.upMarkMode,upMarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->ipv4Cfg.dscpMarkMode,dscpMarkMode);

        /* prepare hw format */
        hwFormatArray[0] = 0x1 | /* X-over-IPv4 tunnel */
            ((upMarkMode & 0x1) << 2) |
            ((logicFormatPtr->ipv4Cfg.up & 0x7) << 3) |
            (((logicFormatPtr->ipv4Cfg.tagEnable == GT_TRUE) ? 1 : 0) << 6) |
            ((logicFormatPtr->ipv4Cfg.vlanId & 0xfff) << 7) |
            ((logicFormatPtr->ipv4Cfg.ttl & 0xff) << 19) |
            ((logicFormatPtr->ipv4Cfg.cfi & 0x1) << 27);

        hwFormatArray[1] = GT_HW_MAC_LOW32((&logicFormatPtr->ipv4Cfg.macDa));

        hwFormatArray[2] = GT_HW_MAC_HIGH16((&logicFormatPtr->ipv4Cfg.macDa));

        hwFormatArray[3] = (logicFormatPtr->ipv4Cfg.autoTunnelOffset & 0xf) |
            (((logicFormatPtr->ipv4Cfg.autoTunnel == GT_TRUE) ? 1 : 0) << 4) |
            (((tunnelType == CPSS_TUNNEL_X_OVER_GRE_IPV4_E) ? 1 : 0) << 5) |
            ((logicFormatPtr->ipv4Cfg.dscp & 0x3f) << 6) |
            ((dscpMarkMode & 0x1) << 12) |
            (((logicFormatPtr->ipv4Cfg.dontFragmentFlag == GT_TRUE) ? 1 : 0) << 13);

        hwFormatArray[4] = (logicFormatPtr->ipv4Cfg.destIp.arIP[0] << 24) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[1] << 16) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[2] << 8) |
            (logicFormatPtr->ipv4Cfg.destIp.arIP[3]);

        hwFormatArray[5] = (logicFormatPtr->ipv4Cfg.srcIp.arIP[0] << 24) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[1] << 16) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[2] << 8) |
            (logicFormatPtr->ipv4Cfg.srcIp.arIP[3]);

        break;

    case CPSS_TUNNEL_X_OVER_MPLS_E:
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.upMarkMode,upMarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp1MarkMode,exp1MarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp2MarkMode,exp2MarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.exp3MarkMode,exp3MarkMode);
        PRV_TUNNEL_START_CONVERT_TTL_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mplsCfg.ttlMode,ttlMode);

        /* prepare hw format */
        hwFormatArray[0] = 0x0 | /* X-over-MPLS tunnel */
            ((upMarkMode & 0x1) << 2) |
            ((logicFormatPtr->mplsCfg.up & 0x7) << 3) |
            (((logicFormatPtr->mplsCfg.tagEnable == GT_TRUE) ? 1 : 0) << 6) |
            ((logicFormatPtr->mplsCfg.vlanId & 0xfff) << 7) |
            ((logicFormatPtr->mplsCfg.ttl & 0xff) << 19) |
            ((logicFormatPtr->mplsCfg.cfi & 0x1) << 27);

        hwFormatArray[1] = GT_HW_MAC_LOW32((&logicFormatPtr->mplsCfg.macDa));

        hwFormatArray[2] = GT_HW_MAC_HIGH16((&logicFormatPtr->mplsCfg.macDa));

        hwFormatArray[3] = ((logicFormatPtr->mplsCfg.label1 & 0xfffff) << 0) |
            ((logicFormatPtr->mplsCfg.exp1 & 0x7) << 20) |
            ((exp1MarkMode & 0x1) << 23) |
            (((logicFormatPtr->mplsCfg.numLabels - 1) & 0x1) << 24) |
            (((logicFormatPtr->mplsCfg.retainCRC == GT_TRUE) ? 1 : 0) << 25) |
            ((((logicFormatPtr->mplsCfg.numLabels - 1) >> 1) & 0x1) << 26) |
            (((logicFormatPtr->mplsCfg.setSBit == GT_TRUE) ? 1 : 0) << 27) |
            ((ttlMode & 0x3) << 28);

        hwFormatArray[4] = ((logicFormatPtr->mplsCfg.label2 & 0xfffff) << 0) |
            ((logicFormatPtr->mplsCfg.exp2 & 0x7) << 20) |
            (exp2MarkMode << 23);

        hwFormatArray[5] = ((logicFormatPtr->mplsCfg.label3 & 0xfffff) << 0) |
            ((logicFormatPtr->mplsCfg.exp3 & 0x7) << 20) |
            (exp3MarkMode << 23);

        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.upMarkMode,upMarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.iUpMarkMode,iUpMarkMode);
        PRV_TUNNEL_START_CONVERT_QOS_MARK_MODE_TO_HW_FORMAT_MAC(logicFormatPtr->mimCfg.iDpMarkMode,iDpMarkMode);

        /* prepare hw format */
        hwFormatArray[0] = 0x2 | /* MIM tunnel */
            ((upMarkMode & 0x1) << 2) |
            ((logicFormatPtr->mimCfg.up & 0x7) << 3) |
            (((logicFormatPtr->mimCfg.tagEnable == GT_TRUE) ? 1 : 0) << 6) |
            ((logicFormatPtr->mimCfg.vlanId & 0xfff) << 7);

        hwFormatArray[1] = GT_HW_MAC_LOW32((&logicFormatPtr->mimCfg.macDa));

        hwFormatArray[2] = GT_HW_MAC_HIGH16((&logicFormatPtr->mimCfg.macDa));

        hwFormatArray[3] = ((logicFormatPtr->mimCfg.iSid & 0xffffff) << 0) |
            ((iUpMarkMode & 0x1) << 24) |
            (((logicFormatPtr->mimCfg.retainCrc == GT_TRUE) ? 1 : 0) << 25) |
            (((logicFormatPtr->mimCfg.iUp) & 0x7) << 26) |
            ((logicFormatPtr->mimCfg.iDp & 0x1) << 29) |
            ((iDpMarkMode & 0x1) << 30);

        hwFormatArray[4] = ((logicFormatPtr->mimCfg.iTagReserved & 0xf) << 0);

        hwFormatArray[5] = 0;

        break;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}


/*******************************************************************************
* tunnelStartConfigHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tunnel start configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - physical device number
*       hwFormatArray   - The configuration in HW format (6 words).
*
* OUTPUTS:
*       tunnelTypePtr   - points to tunnel type; valid values:
*                         CPSS_TUNNEL_X_OVER_IPV4_E,
*                         CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                         CPSS_TUNNEL_X_OVER_MPLS_E
*                         CPSS_TUNNEL_MAC_IN_MAC_E (CPSS_TUNNEL_MAC_IN_MAC_E xCat; Lion; xCat2)
*       logicFormatPtr  - points to tunnel start configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameters.
*       GT_BAD_PTR      - on NULL pointer
*       GT_BAD_STATE    - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelStartConfigHw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *logicFormatPtr
)
{
    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    /* get tunnel type */
    switch (hwFormatArray[0] & 0x3)
    {
    case 0:
        *tunnelTypePtr = CPSS_TUNNEL_X_OVER_MPLS_E;
        break;
    case 1:
        if (((hwFormatArray[3] >> 5) & 0x1) == 0)
            *tunnelTypePtr = CPSS_TUNNEL_X_OVER_IPV4_E;
        else
            *tunnelTypePtr = CPSS_TUNNEL_X_OVER_GRE_IPV4_E;
        break;
    case 2:
        /* MIM is supported xCat and Lion */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            *tunnelTypePtr = CPSS_TUNNEL_MAC_IN_MAC_E;
        else
            return GT_BAD_STATE;
        break;
    default:
        return GT_BAD_STATE;
    }

    /* convert to logical format */
    switch (*tunnelTypePtr)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        if (((hwFormatArray[0] >> 6) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.tagEnable = GT_TRUE;
        else
            logicFormatPtr->ipv4Cfg.tagEnable = GT_FALSE;

        if (logicFormatPtr->ipv4Cfg.tagEnable == GT_TRUE)
        {
            if (((hwFormatArray[0] >> 2) & 0x1) == 1)
                logicFormatPtr->ipv4Cfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->ipv4Cfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

            if (logicFormatPtr->ipv4Cfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->ipv4Cfg.up = ((hwFormatArray[0] >> 3) & 0x7);

            logicFormatPtr->ipv4Cfg.vlanId = (GT_U16)((hwFormatArray[0] >> 7) & 0xfff);
        }

        logicFormatPtr->ipv4Cfg.ttl = ((hwFormatArray[0] >> 19) & 0xff);

        /* cfi bit is relevant only for DxCh3; xCat; Lion */
        if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
            logicFormatPtr->ipv4Cfg.cfi = ((hwFormatArray[0] >> 27) & 0x1);

        logicFormatPtr->ipv4Cfg.macDa.arEther[5] = (GT_U8)((hwFormatArray[1] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  8) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 24) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[1] = (GT_U8)((hwFormatArray[2] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  8) & 0xff);

        if (((hwFormatArray[3] >> 4) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.autoTunnel = GT_TRUE;
        else
            logicFormatPtr->ipv4Cfg.autoTunnel = GT_FALSE;

        if (logicFormatPtr->ipv4Cfg.autoTunnel == GT_TRUE)
        {
            logicFormatPtr->ipv4Cfg.autoTunnelOffset = ((hwFormatArray[3]) & 0xf);
        }

        if (((hwFormatArray[3] >> 12) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        else
            logicFormatPtr->ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

        if (logicFormatPtr->ipv4Cfg.dscpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
        {
            logicFormatPtr->ipv4Cfg.dscp = ((hwFormatArray[3] >> 6) & 0x3f);
        }

        if (((hwFormatArray[3] >> 13) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.dontFragmentFlag = GT_TRUE;
        else
            logicFormatPtr->ipv4Cfg.dontFragmentFlag = GT_FALSE;

        logicFormatPtr->ipv4Cfg.destIp.arIP[3] = (GT_U8)((hwFormatArray[4] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[2] = (GT_U8)((hwFormatArray[4] >>  8) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[1] = (GT_U8)((hwFormatArray[4] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[0] = (GT_U8)((hwFormatArray[4] >> 24) & 0xff);

        logicFormatPtr->ipv4Cfg.srcIp.arIP[3] = (GT_U8)((hwFormatArray[5] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[2] = (GT_U8)((hwFormatArray[5] >>  8) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[1] = (GT_U8)((hwFormatArray[5] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[0] = (GT_U8)((hwFormatArray[5] >> 24) & 0xff);

        break;

    case CPSS_TUNNEL_X_OVER_MPLS_E:
        if (((hwFormatArray[0] >> 6) & 0x1) == 1)
            logicFormatPtr->mplsCfg.tagEnable = GT_TRUE;
        else
            logicFormatPtr->mplsCfg.tagEnable = GT_FALSE;

        if (logicFormatPtr->mplsCfg.tagEnable == GT_TRUE)
        {
            if (((hwFormatArray[0] >> 2) & 0x1) == 1)
                logicFormatPtr->mplsCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mplsCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

            if (logicFormatPtr->mplsCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            {
                logicFormatPtr->mplsCfg.up = ((hwFormatArray[0] >> 3) & 0x7);
            }

            logicFormatPtr->mplsCfg.vlanId = (GT_U16)((hwFormatArray[0] >> 7) & 0xfff);
        }

        logicFormatPtr->mplsCfg.ttl = ((hwFormatArray[0] >> 19) & 0xff);

        if ((logicFormatPtr->mplsCfg.ttl == 0)
            && PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            switch ((hwFormatArray[3] >> 28) & 0x3)
            {
            case 0:
                logicFormatPtr->mplsCfg.ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E;
                break;
            case 1:
                logicFormatPtr->mplsCfg.ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E;
                break;
            case 2:
                logicFormatPtr->mplsCfg.ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E;
                break;
            default:
                return GT_BAD_STATE;
            }
        }

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            if (((hwFormatArray[3] >> 27) & 0x1) == 1)
                logicFormatPtr->mplsCfg.setSBit = GT_TRUE;
            else
                logicFormatPtr->mplsCfg.setSBit = GT_FALSE;
        }

        /* cfi bit is relevant only for DxCh3; xCat; Lion */
        if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
        {
            logicFormatPtr->mplsCfg.cfi = ((hwFormatArray[0] >> 27) & 0x1);
        }

        logicFormatPtr->mplsCfg.macDa.arEther[5] = (GT_U8)((hwFormatArray[1] >>  0) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  8) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 16) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 24) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[1] = (GT_U8)((hwFormatArray[2] >>  0) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  8) & 0xff);

        logicFormatPtr->mplsCfg.numLabels = (((hwFormatArray[3] >> 24) & 0x1) |
            (((hwFormatArray[3] >> 26) & 0x1) << 1)) + 1;

        switch (logicFormatPtr->mplsCfg.numLabels)
        {
        case 3:
            /* 3 labels mode is supported for xCat and Lion */
            if(! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
                return GT_BAD_STATE;
            logicFormatPtr->mplsCfg.label3 = ((hwFormatArray[5]) & 0xfffff);
            if (((hwFormatArray[5] >> 23) & 0x1) == 1)
                logicFormatPtr->mplsCfg.exp3MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mplsCfg.exp3MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;
            if (logicFormatPtr->mplsCfg.exp3MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->mplsCfg.exp3 = ((hwFormatArray[5] >> 20) & 0x7);
            /* note fall through to check other labels too */
        case 2:
            logicFormatPtr->mplsCfg.label2 = ((hwFormatArray[4]) & 0xfffff);
            if (((hwFormatArray[4] >> 23) & 0x1) == 1)
                logicFormatPtr->mplsCfg.exp2MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mplsCfg.exp2MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;
            if (logicFormatPtr->mplsCfg.exp2MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->mplsCfg.exp2 = ((hwFormatArray[4] >> 20) & 0x7);
            /* note fall through to check other labels too */
        case 1:
            logicFormatPtr->mplsCfg.label1 = ((hwFormatArray[3]) & 0xfffff);
            if (((hwFormatArray[3] >> 23) & 0x1) == 1)
                logicFormatPtr->mplsCfg.exp1MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mplsCfg.exp1MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;
            if (logicFormatPtr->mplsCfg.exp1MarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->mplsCfg.exp1 = ((hwFormatArray[3] >> 20) & 0x7);
            break;
        default:
            return GT_BAD_STATE;
        }

        if (((hwFormatArray[3] >> 25) & 0x1) == 1)
            logicFormatPtr->mplsCfg.retainCRC = GT_TRUE;
        else
            logicFormatPtr->mplsCfg.retainCRC = GT_FALSE;

        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        if (((hwFormatArray[0] >> 6) & 0x1) == 1)
            logicFormatPtr->mimCfg.tagEnable = GT_TRUE;
        else
            logicFormatPtr->mimCfg.tagEnable = GT_FALSE;

        if (logicFormatPtr->mimCfg.tagEnable == GT_TRUE)
        {
            if (((hwFormatArray[0] >> 2) & 0x1) == 1)
                logicFormatPtr->mimCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            else
                logicFormatPtr->mimCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

            if (logicFormatPtr->mimCfg.upMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
                logicFormatPtr->mimCfg.up = ((hwFormatArray[0] >> 3) & 0x7);

            logicFormatPtr->mimCfg.vlanId = (GT_U16)((hwFormatArray[0] >> 7) & 0xfff);
        }

        logicFormatPtr->mimCfg.macDa.arEther[5] = (GT_U8)((hwFormatArray[1] >>  0) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  8) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 16) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 24) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[1] = (GT_U8)((hwFormatArray[2] >>  0) & 0xff);
        logicFormatPtr->mimCfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  8) & 0xff);

        logicFormatPtr->mimCfg.iSid = ((hwFormatArray[3] >> 0) & 0xffffff);

        if (((hwFormatArray[3] >> 25) & 0x1) == 1)
            logicFormatPtr->mimCfg.retainCrc = GT_TRUE;
        else
            logicFormatPtr->mimCfg.retainCrc = GT_FALSE;

        if (((hwFormatArray[3] >> 24) & 0x1) == 1)
            logicFormatPtr->mimCfg.iUpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        else
            logicFormatPtr->mimCfg.iUpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

        if (logicFormatPtr->mimCfg.iUpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            logicFormatPtr->mimCfg.iUp = ((hwFormatArray[3] >> 26) & 0x7);

        if (((hwFormatArray[3] >> 30) & 0x1) == 1)
            logicFormatPtr->mimCfg.iDpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        else
            logicFormatPtr->mimCfg.iDpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;

        if (logicFormatPtr->mimCfg.iDpMarkMode == CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E)
            logicFormatPtr->mimCfg.iDp = ((hwFormatArray[3] >> 29) & 0x1);

        logicFormatPtr->mimCfg.iTagReserved = ((hwFormatArray[4] >> 0) & 0xf);

        break;

    default:
        return GT_BAD_STATE;
    }

    return GT_OK;
}


/*******************************************************************************
* tunnelTermConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
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
    GT_U32      ipv4TunnelTypeShift[] = {11,13,12,14}; /* ipv4 tunnel type to word 2 of hwFormatArray */

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
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default:
        return GT_BAD_PARAM;
    }

    /* check ipv4/mpls configuration pararmeters only for config, not for mask */
    if (convertMask == GT_FALSE)
    {
        /* check ipv4 parameters */
        if (isXoverMplsTunnel == GT_FALSE)
        {
            if(logicFormatPtr->ipv4Cfg.srcDev >= BIT_5) return GT_OUT_OF_RANGE;

            switch (logicFormatPtr->ipv4Cfg.srcIsTrunk)
            {
            case 0: /* src is not trunk */

                if(logicFormatPtr->ipv4Cfg.srcPortTrunk >= BIT_6) return GT_OUT_OF_RANGE;

                break;
            case 1: /* src is trunk */
                if (logicFormatPtr->ipv4Cfg.srcPortTrunk > TRUNK_INTERFACE_MAX_CNS)
                    return GT_BAD_PARAM;
                break;
            default:
                return GT_BAD_PARAM;
            }
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->ipv4Cfg.vid);
        }
        /* check mpls parameters */
        else
        {
            if(logicFormatPtr->mplsCfg.srcDev >= BIT_5) return GT_OUT_OF_RANGE;

            switch (logicFormatPtr->mplsCfg.srcIsTrunk)
            {
            case 0: /* src is not trunk */
                if(logicFormatPtr->mplsCfg.srcPortTrunk >= BIT_6) return GT_OUT_OF_RANGE;

                break;
            case 1: /* src is trunk */
                if (logicFormatPtr->mplsCfg.srcPortTrunk > TRUNK_INTERFACE_MAX_CNS)
                    return GT_BAD_PARAM;
                break;
            default:
                return GT_BAD_PARAM;
            }
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->mplsCfg.vid);
            PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label1);
            PRV_TUNNEL_START_MPLS_SBIT_CHECK_MAC(logicFormatPtr->mplsCfg.sBit1);
            PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp1);
            PRV_TUNNEL_START_MPLS_LABEL_CHECK_MAC(logicFormatPtr->mplsCfg.label2);
            PRV_TUNNEL_START_MPLS_SBIT_CHECK_MAC(logicFormatPtr->mplsCfg.sBit2);
            PRV_TUNNEL_START_MPLS_EXP_CHECK_MAC(logicFormatPtr->mplsCfg.exp2);
        }
    }

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
        hwFormatArray[2] |= (0x1 << ipv4TunnelTypeShift[tunnelType]);

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


/*******************************************************************************
* tunnelTermConfigHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       hwFormatArray     - tunnel termination configuration in HW format (6 words).
*
* OUTPUTS:
*       tunnelTypePtr     - points to tunnel type; valid options:
*                           CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                           CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                           CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                           CPSS_TUNNEL_X_OVER_MPLS_E
*       logicFormatPtr    - points to tunnel termination configuration in
*                           logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelTermConfigHw2LogicFormat
(
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *logicFormatPtr
)
{
    GT_BOOL     isXoverMplsTunnel;  /* if tunnel type is X-over-MPLS tunnel */

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    /* first check that config is for tunnel termination */
    if (((hwFormatArray[3] >> 31) & 0x1) != 1)
    {
        return GT_BAD_STATE;
    }

    /* get tunnel type */
    if (((hwFormatArray[2] >> 10) & 0x1) == 1)
    {
        isXoverMplsTunnel = GT_TRUE;
        *tunnelTypePtr = CPSS_TUNNEL_X_OVER_MPLS_E;
    }
    else
    {
        isXoverMplsTunnel = GT_FALSE;
        switch ((hwFormatArray[2] >> 11) & 0xf)
        {
        case 0x1:
            *tunnelTypePtr = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
            break;
        case 0x2:
            *tunnelTypePtr = CPSS_TUNNEL_IPV6_OVER_IPV4_E;
            break;
        case 0x4:
            *tunnelTypePtr = CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E;
            break;
        case 0x8:
            *tunnelTypePtr = CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E;
            break;
        default:
            return GT_BAD_STATE;
        }
    }

    /* get X-over-IPv4/GRE_IPv4 tunnel termination configuration */
    if (isXoverMplsTunnel == GT_FALSE)
    {
        logicFormatPtr->ipv4Cfg.srcPortTrunk = ((hwFormatArray[0]) & 0x7f);

        if (((hwFormatArray[0] >> 7) & 0x1) == 1)
            logicFormatPtr->ipv4Cfg.srcIsTrunk = GT_TRUE;
        else
            logicFormatPtr->ipv4Cfg.srcIsTrunk = GT_FALSE;

        logicFormatPtr->ipv4Cfg.srcDev = (GT_U8)((hwFormatArray[0] >> 8) & 0x1f);

        logicFormatPtr->ipv4Cfg.vid = (GT_U16)((hwFormatArray[0] >> 14) & 0xfff);

        logicFormatPtr->ipv4Cfg.macDa.arEther[5] =
            (GT_U8)(((hwFormatArray[1] & 0x3) << 6) | ((hwFormatArray[0] >> 26) & 0x3f));
        logicFormatPtr->ipv4Cfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  2) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 10) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 18) & 0xff);
        logicFormatPtr->ipv4Cfg.macDa.arEther[1] =
            (GT_U8)(((hwFormatArray[2] & 0x3) << 6) | ((hwFormatArray[1] >> 26) & 0x3f));
        logicFormatPtr->ipv4Cfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  2) & 0xff);

        logicFormatPtr->ipv4Cfg.srcIp.arIP[3] = (GT_U8)((hwFormatArray[2] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[2] = (GT_U8)((hwFormatArray[2] >> 24) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[1] = (GT_U8)((hwFormatArray[3] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.srcIp.arIP[0] = (GT_U8)((hwFormatArray[3] >>  8) & 0xff);

        logicFormatPtr->ipv4Cfg.destIp.arIP[3] = (GT_U8)((hwFormatArray[4] >>  0) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[2] = (GT_U8)((hwFormatArray[4] >>  8) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[1] = (GT_U8)((hwFormatArray[4] >> 16) & 0xff);
        logicFormatPtr->ipv4Cfg.destIp.arIP[0] = (GT_U8)((hwFormatArray[4] >> 24) & 0xff);
    }
    /* get X-over-MPLS tunnel termination configuration */
    else
    {
        logicFormatPtr->mplsCfg.srcPortTrunk = ((hwFormatArray[0]) & 0x7f);

        if (((hwFormatArray[0] >> 7) & 0x1) == 1)
            logicFormatPtr->mplsCfg.srcIsTrunk = GT_TRUE;
        else
            logicFormatPtr->mplsCfg.srcIsTrunk = GT_FALSE;

        logicFormatPtr->mplsCfg.srcDev = (GT_U8)((hwFormatArray[0] >> 8) & 0x1f);

        logicFormatPtr->mplsCfg.vid = (GT_U16)((hwFormatArray[0] >> 14) & 0xfff);

        logicFormatPtr->mplsCfg.macDa.arEther[5] =
            (GT_U8)(((hwFormatArray[1] & 0x3) << 6) | ((hwFormatArray[0] >> 26) & 0x3f));
        logicFormatPtr->mplsCfg.macDa.arEther[4] = (GT_U8)((hwFormatArray[1] >>  2) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[3] = (GT_U8)((hwFormatArray[1] >> 10) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[2] = (GT_U8)((hwFormatArray[1] >> 18) & 0xff);
        logicFormatPtr->mplsCfg.macDa.arEther[1] =
            (GT_U8)(((hwFormatArray[2] & 0x3) << 6) | ((hwFormatArray[1] >> 26) & 0x3f));
        logicFormatPtr->mplsCfg.macDa.arEther[0] = (GT_U8)((hwFormatArray[2] >>  2) & 0xff);

        logicFormatPtr->mplsCfg.exp2 = ((hwFormatArray[2] >> 12) & 0x7);

        logicFormatPtr->mplsCfg.sBit2 = ((hwFormatArray[2] >> 15) & 0x1);

        logicFormatPtr->mplsCfg.label2 =
            (((hwFormatArray[3] & 0xf) << 16) | ((hwFormatArray[2] >> 16) & 0xffff));

        logicFormatPtr->mplsCfg.exp1 = ((hwFormatArray[2] >> 4) & 0x7);

        logicFormatPtr->mplsCfg.sBit1 = ((hwFormatArray[3] >> 7) & 0x1);

        logicFormatPtr->mplsCfg.label1 = ((hwFormatArray[3] >> 8) & 0xfffff);
    }

    return GT_OK;
}


/*******************************************************************************
* tunnelTermActionLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination action from logic format to
*       hardware format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       tunnelType        - type of the tunnel; valid options:
*                           CPSS_TUNNEL_IP_OVER_X_E
*                           CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       logicFormatPtr    - points to tunnel termination action in logic format
*
* OUTPUTS:
*       hwFormatArray     - tunnel termination action in hardware format (4 words)
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelTermActionLogic2HwFormat
(
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *logicFormatPtr,
    OUT GT_U32                              *hwFormatArray
)
{
    GT_STATUS   rc=GT_OK;
    GT_32       pktCommand;         /* tunnel termination packet forwarding */
                                    /* command (hw format) */
    GT_U32      userDefinedCpuCode; /* user defined cpu code (hw format) */
    GT_U32      passengerPktType;   /* passenger packet type (hw format) */
    GT_U32      redirect;           /* if to redirect packet */
    GT_U32      egressInterface;    /* egress interface (hw format) */
    GT_U32      vlanPrecedence;     /* VLAN assignment precedence mode (hw format) */
    GT_U32      qosMode;            /* QoS mode (hw format) */
    GT_U32      qosPrecedence;      /* QoS precedence mode (hw format) */
    GT_U32      modifyUP;           /* modift user priority mode (hw format) */
    GT_U32      modifyDSCP;         /* modify DSCP mode (hw format) */

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TUNNEL_TERMINATION_TCAM_ACTION_SIZE_CNS);

    /* check parameters that are not checked later on */

    if ((tunnelType != CPSS_TUNNEL_IP_OVER_X_E) &&
        (tunnelType != CPSS_TUNNEL_ETHERNET_OVER_MPLS_E))
    {
        return GT_BAD_PARAM;
    }

    switch (logicFormatPtr->egressInterface.type)
    {
    case CPSS_INTERFACE_VIDX_E:
        if (logicFormatPtr->egressInterface.vidx > VIDX_INTERFACE_MAX_CNS)
            return GT_BAD_PARAM;
        break;
    case CPSS_INTERFACE_TRUNK_E:
        if (logicFormatPtr->egressInterface.trunkId > TRUNK_INTERFACE_MAX_CNS)
            return GT_BAD_PARAM;
        break;
    case CPSS_INTERFACE_PORT_E:
         /* check that devNum value takes only 5 bits and portNum only 6 bits
           according to HW action format */
         if((logicFormatPtr->egressInterface.devPort.devNum >= BIT_5) ||
            (logicFormatPtr->egressInterface.devPort.portNum >= BIT_6))
         {
             return GT_BAD_PARAM;
         }
        break;
    default:
        return GT_BAD_PARAM;
    }

    if (logicFormatPtr->tunnelStartIdx > TUNNEL_START_MAX_INDEX_CNS)
        return GT_BAD_PARAM;

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->vlanId);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->defaultUP);
    if (logicFormatPtr->qosProfile > QOS_PROFILE_MAX_CNS)
        return GT_BAD_PARAM;
    if (logicFormatPtr->policerIndex > POLICER_INDEX_MAX_CNS)
        return GT_BAD_PARAM;
    if (logicFormatPtr->matchCounterIndex > MATCH_COUNTER_INDEX_MAX_CNS)
        return GT_BAD_PARAM;

    /* translate tunnel termination action forwarding command parameter */
    switch (logicFormatPtr->command)
    {
    case CPSS_PACKET_CMD_ROUTE_E:
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

    /* translate tunnel termination action modify DSCP parameter */
    switch (logicFormatPtr->modifyDSCP)
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
    switch (logicFormatPtr->modifyUP)
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

    /* get redirect command from tunnel type */
    if (tunnelType == CPSS_TUNNEL_ETHERNET_OVER_MPLS_E)
        redirect = 1;
    else
        redirect = 0;

    /* translate egress interface to hw format from action output interface */
    switch (logicFormatPtr->egressInterface.type)
    {
    case CPSS_INTERFACE_VIDX_E:
        egressInterface = (0x1 << 12) | /* use vidx */
            (logicFormatPtr->egressInterface.vidx & 0x3ff);
        break;
    case CPSS_INTERFACE_TRUNK_E:
        egressInterface = 0x1 | /* trunk type */
            ((logicFormatPtr->egressInterface.trunkId & 0x7f) << 1);
        break;
    case CPSS_INTERFACE_PORT_E:
        egressInterface = ((logicFormatPtr->egressInterface.devPort.portNum & 0x3f) << 1) |
            ((logicFormatPtr->egressInterface.devPort.devNum & 0x1f) << 7);
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate QoS mode from action QoS mode parameter */
    switch (logicFormatPtr->qosMode)
    {
    case CPSS_DXCH_TUNNEL_QOS_KEEP_PREVIOUS_E:
        qosMode = 0;
        break;
    case CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_E:
        qosMode = 1;
        break;
    case CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_DSCP_E:
        qosMode = 2;
        break;
    case CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E:
        qosMode = 3;
        break;
    case CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E:
        qosMode = 4;
        break;
    case CPSS_DXCH_TUNNEL_QOS_TRUST_MPLS_EXP_E:
        qosMode = 5;
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

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        if ((logicFormatPtr->userDefinedCpuCode >= CPSS_NET_FIRST_USER_DEFINED_E) &&
            (logicFormatPtr->userDefinedCpuCode <= CPSS_NET_LAST_USER_DEFINED_E))
        {
            rc = prvCpssDxChNetIfCpuToDsaCode(logicFormatPtr->userDefinedCpuCode,
                                         (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT*)(GT_U32)&userDefinedCpuCode);
            if(rc != GT_OK)
                return rc;
        }
        else
            return GT_BAD_PARAM;
    }
    else
        userDefinedCpuCode = 0;

    /* now prepare hardware format */
    hwFormatArray[0] = (pktCommand & 0x7) |
        ((userDefinedCpuCode & 0xff) << 3) |
        (((logicFormatPtr->mirrorToIngressAnalyzerEnable == GT_TRUE) ? 1 : 0) << 11) |
        ((qosPrecedence & 0x1) << 12) |
        ((logicFormatPtr->qosProfile & 0x7f) << 14) |
        ((modifyDSCP & 0x3) << 21) |
        ((modifyUP & 0x3) << 23) |
        (((logicFormatPtr->matchCounterEnable == GT_TRUE) ? 1 : 0) << 25) |
        ((logicFormatPtr->matchCounterIndex & 0x1f) << 26);

    hwFormatArray[1] = (redirect & 0x3) |
        ((egressInterface & 0x1fff) << 2) |
        ((vlanPrecedence & 0x1) << 15) |
        (((logicFormatPtr->nestedVlanEnable == GT_TRUE) ? 1 : 0) << 16) |
        ((logicFormatPtr->vlanId & 0xfff) << 19) |
        (((logicFormatPtr->remapDSCP == GT_TRUE) ? 1 : 0) << 31);

    hwFormatArray[2] = ((logicFormatPtr->policerEnable == GT_TRUE) ? 1 : 0) |
        ((logicFormatPtr->policerIndex & 0xff) << 1) |
        (((logicFormatPtr->isTunnelStart == GT_TRUE) ? 1 : 0) << 10) |
        ((logicFormatPtr->tunnelStartIdx & 0x3ff) << 11) |
        ((qosMode & 0x7) << 22) |
        ((passengerPktType & 0x3) << 25) |
        (((logicFormatPtr->copyTtlFromTunnelHeader == GT_TRUE) ? 1 : 0) << 27) |
        ((0x1) << 28) | /* this bit is reserved and must be set to 1 */
        ((logicFormatPtr->defaultUP & 0x7) << 29);

    hwFormatArray[3] = 0;

    return GT_OK;
}


/*******************************************************************************
* tunnelTermActionHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination action from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       hwFormatArray     - tunnel termination action in hardware format (4 words)
*
* OUTPUTS:
*       tunnelTypePtr     - points to type of the tunnel; valid options:
*                           CPSS_TUNNEL_IP_OVER_X_E
*                           CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       logicFormatPtr    - points to tunnel termination action in logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tunnelTermActionHw2LogicFormat
(
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *logicFormatPtr
)
{
    GT_STATUS rc = GT_OK;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    /* get tunnel type */
    switch (hwFormatArray[1] & 0x3)
    {
        case 0:
            *tunnelTypePtr = CPSS_TUNNEL_IP_OVER_X_E;
            break;
        case 1:
            *tunnelTypePtr = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch (hwFormatArray[0] & 0x7)
    {
        case 0:
            logicFormatPtr->command = CPSS_PACKET_CMD_ROUTE_E;
            break;
        case 1:
            logicFormatPtr->command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;
        case 2:
            logicFormatPtr->command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            logicFormatPtr->command = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            logicFormatPtr->command = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    rc = prvCpssDxChNetIfDsaToCpuCode(((hwFormatArray[0] >> 3) & 0xff),
                                 &logicFormatPtr->userDefinedCpuCode);
    if(rc != GT_OK)
        return rc;

    if (((hwFormatArray[0] >> 11) & 0x1) == 1)
        logicFormatPtr->mirrorToIngressAnalyzerEnable = GT_TRUE;
    else
        logicFormatPtr->mirrorToIngressAnalyzerEnable = GT_FALSE;

    if (((hwFormatArray[0] >> 12) & 0x1) == 1)
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    else
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    logicFormatPtr->qosProfile = ((hwFormatArray[0] >> 14) & 0x7f);

    switch ((hwFormatArray[0] >> 21) & 0x3)
    {
        case 0:
            logicFormatPtr->modifyDSCP = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            logicFormatPtr->modifyDSCP = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            logicFormatPtr->modifyDSCP = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch ((hwFormatArray[0] >> 23) & 0x3)
    {
        case 0:
            logicFormatPtr->modifyUP = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            logicFormatPtr->modifyUP = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            logicFormatPtr->modifyUP = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    if (((hwFormatArray[0] >> 25) & 0x1) == 1)
        logicFormatPtr->matchCounterEnable = GT_TRUE;
    else
        logicFormatPtr->matchCounterEnable = GT_FALSE;

    logicFormatPtr->matchCounterIndex = ((hwFormatArray[0] >> 26) & 0x1f);

    /* if egress interface is vidx */
    if (((hwFormatArray[1] >> 14) & 0x1) == 1)
    {
        logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VIDX_E;
        logicFormatPtr->egressInterface.vidx =
            (GT_U16)((hwFormatArray[1] >> 2) & 0xfff);
    }
    /* if egress interface is trunk or {dev,port} */
    else
    {
        /* if egress interface is trunk */
        if (((hwFormatArray[1] >> 2) & 0x1) == 1)
        {
            logicFormatPtr->egressInterface.type = CPSS_INTERFACE_TRUNK_E;
            logicFormatPtr->egressInterface.trunkId =
                (GT_TRUNK_ID)((hwFormatArray[1] >> 3) & 0x7f);
        }
        /* if egress interface {dev,port} */
        else
        {
            logicFormatPtr->egressInterface.type = CPSS_INTERFACE_PORT_E;
            logicFormatPtr->egressInterface.devPort.devNum =
                (GT_U8)((hwFormatArray[1] >> 9) & 0x1f);
            logicFormatPtr->egressInterface.devPort.portNum =
                (GT_U8)((hwFormatArray[1] >> 3) & 0x3f);
        }
    }

    if (((hwFormatArray[1] >> 15) & 0x1) == 1)
        logicFormatPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    else
        logicFormatPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    if (((hwFormatArray[1] >> 16) & 0x1) == 1)
        logicFormatPtr->nestedVlanEnable = GT_TRUE;
    else
        logicFormatPtr->nestedVlanEnable = GT_FALSE;

    logicFormatPtr->vlanId = (GT_U16)((hwFormatArray[1] >> 19) & 0xfff);

    if (((hwFormatArray[1] >> 31) & 0x1) == 1)
        logicFormatPtr->remapDSCP = GT_TRUE;
    else
        logicFormatPtr->remapDSCP = GT_FALSE;

    if (((hwFormatArray[2]) & 0x1) == 1)
        logicFormatPtr->policerEnable = GT_TRUE;
    else
        logicFormatPtr->policerEnable = GT_FALSE;

    logicFormatPtr->policerIndex = ((hwFormatArray[2] >> 1) & 0xff);

    if (((hwFormatArray[2] >> 10) & 0x1) == 1)
        logicFormatPtr->isTunnelStart = GT_TRUE;
    else
        logicFormatPtr->isTunnelStart = GT_FALSE;

    logicFormatPtr->tunnelStartIdx = ((hwFormatArray[2] >> 11) & 0x3ff);

    switch ((hwFormatArray[2] >> 22) & 0x7)
    {
        case 0:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_KEEP_PREVIOUS_E;
            break;
        case 1:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_E;
            break;
        case 2:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_DSCP_E;
            break;
        case 3:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E;
            break;
        case 4:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E;
            break;
        case 5:
            logicFormatPtr->qosMode = CPSS_DXCH_TUNNEL_QOS_TRUST_MPLS_EXP_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch ((hwFormatArray[2] >> 25) & 0x3)
    {
        case 0:
            logicFormatPtr->passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E;
            break;
        case 1:
            logicFormatPtr->passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E;
            break;
        case 2:
            logicFormatPtr->passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E;
            break;
        case 3:
            logicFormatPtr->passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    if (((hwFormatArray[2] >> 27) & 0x1) == 1)
        logicFormatPtr->copyTtlFromTunnelHeader = GT_TRUE;
    else
        logicFormatPtr->copyTtlFromTunnelHeader = GT_FALSE;

    logicFormatPtr->defaultUP = ((hwFormatArray[2] >> 29) & 0x7);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table
*       tunnelType    - type of the tunnel; valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
{
    GT_U32      hwConfigArray[TUNNEL_START_HW_SIZE_CNS]; /* tunnel start config in hardware format */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    LINE_INDEX_CHECK_MAC(devNum,routerArpTunnelStartLineIndex);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);

    /* convert tunnel start entry to hw format while checking parameters */
    rc = tunnelStartConfigLogic2HwFormat(devNum,tunnelType,configPtr,hwConfigArray);
    if (rc != GT_OK)
        return rc;

      /* write the tunnel start entry to the hardware */
    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                    routerArpTunnelStartLineIndex,
                                    hwConfigArray);



    return rc;
}


/*******************************************************************************
* cpssDxChTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table
*
* OUTPUTS:
*       tunnelTypePtr - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
{
    GT_U32      hwConfigArray[TUNNEL_START_HW_SIZE_CNS]; /* tunnel start config in hardware format */
    GT_STATUS   rc;     /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    LINE_INDEX_CHECK_MAC(devNum,routerArpTunnelStartLineIndex);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);

     /* read tunnel start entry from the hardware */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                   routerArpTunnelStartLineIndex,
                                   hwConfigArray);


    if (rc != GT_OK)
        return rc;

    /* convert tunnel entry from hardware format to logic format */
    rc = tunnelStartConfigHw2LogicFormat(devNum,hwConfigArray,tunnelTypePtr,configPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartPortGroupEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table
*       tunnelType    - type of the tunnel; valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
{
    GT_U32      hwConfigArray[TUNNEL_START_HW_SIZE_CNS]; /* tunnel start config in hardware format */
    GT_STATUS   rc;         /* function return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    LINE_INDEX_CHECK_MAC(devNum,routerArpTunnelStartLineIndex);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);

    /* convert tunnel start entry to hw format while checking parameters */
    rc = tunnelStartConfigLogic2HwFormat(devNum,tunnelType,configPtr,hwConfigArray);
    if (rc != GT_OK)
        return rc;

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                                     portGroupId,
                                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                     routerArpTunnelStartLineIndex,
                                                     hwConfigArray);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }
    else
    {
        /* write the tunnel start entry to the hardware */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                        routerArpTunnelStartLineIndex,
                                        hwConfigArray);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartPortGroupEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table
*
* OUTPUTS:
*       tunnelTypePtr - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
{
    GT_U32      hwConfigArray[TUNNEL_START_HW_SIZE_CNS]; /* tunnel start config in hardware format */
    GT_STATUS   rc;     /* function return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    LINE_INDEX_CHECK_MAC(devNum,routerArpTunnelStartLineIndex);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* Get the first active port group */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                            portGroupId);

        /* read tunnel start entry from the hardware */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                                portGroupId,
                                                PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                routerArpTunnelStartLineIndex,
                                                hwConfigArray);
    }
    else
    {
        /* read tunnel start entry from the hardware */
        rc = prvCpssDxChReadTableEntry(devNum,
                                       PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                       routerArpTunnelStartLineIndex,
                                       hwConfigArray);
    }


    if (rc != GT_OK)
        return rc;

    /* convert tunnel entry from hardware format to logic format */
    rc = tunnelStartConfigHw2LogicFormat(devNum,hwConfigArray,tunnelTypePtr,configPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChIpv4TunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for ipv4 tunnel termination.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable ipv4 tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4TunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    IN  GT_BOOL     enable
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    /* prepare value to write */
    value = (enable == GT_TRUE) ? 1 : 0;

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         1,     /* word 1 */
                                         17,    /* offset */
                                         1,     /* length */
                                         value);

    return rc;
}


/*******************************************************************************
* cpssDxChIpv4TunnelTermPortGet
*
* DESCRIPTION:
*       Get ipv4 tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get ipv4 tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to ipv4 tunnel termination state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4TunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read hardware value */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        1,     /* word 1 */
                                        17,    /* offset */
                                        1,     /* length */
                                        &value);
    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChMplsTunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for mpls tunnel termination.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable mpls tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMplsTunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    IN  GT_BOOL     enable
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    /* prepare value to write */
    value = (enable == GT_TRUE) ? 1 : 0;

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         1,     /* word 1 */
                                         18,    /* offset */
                                         1,     /* length */
                                         value);

    return rc;
}


/*******************************************************************************
* cpssDxChMplsTunnelTermPortGet
*
* DESCRIPTION:
*       Get mpls tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get mpls tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to mpls tunnel termination state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMplsTunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read hardware value */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        1,     /* word 1 */
                                        18,    /* offset */
                                        1,     /* length */
                                        &value);
    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChTunnelTermEntrySet
*
* DESCRIPTION:
*       Set a tunnel termination entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM (0..1023)
*       tunnelType    - type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IP_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
{
    GT_STATUS   rc;                 /* function return code */
    GT_U32      hwTcamKeyArray[TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS];  /* TCAM key in hw format */
    GT_U32      hwTcamMaskArray[TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS]; /* TCAM mask in hw format */
    GT_U32      hwTcamActionArray[TUNNEL_TERMINATION_TCAM_ACTION_SIZE_CNS]; /* TCAM action (rule) in hw format */
    CPSS_TUNNEL_TYPE_ENT    configTunnelType;   /* tunnel type used to convert */
                                                /* tunnel config to hw format  */
    CPSS_TUNNEL_TYPE_ENT    actionTunnelType;   /* tunnel type used to convert */
                                                /* tunnel action to hw format  */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_CHECK_LINE_INDEX_MAC(devNum,routerTunnelTermTcamIndex);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);
    CPSS_NULL_PTR_CHECK_MAC(configMaskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);

    /* classify tunnel type according to the required by the function to */
    /* convert tunnel termination config to hardware format              */
    switch (tunnelType)
    {
    case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
        configTunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
        break;
    case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
        configTunnelType = CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E;
        break;
    case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
        configTunnelType = CPSS_TUNNEL_IPV6_OVER_IPV4_E;
        break;
    case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:
        configTunnelType = CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E;
        break;
    case CPSS_TUNNEL_IP_OVER_MPLS_E:
    case CPSS_TUNNEL_ETHERNET_OVER_MPLS_E:
        configTunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;
        break;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default:
        return GT_BAD_PARAM;
    }

    /* convert tunnel termination configuration to hardware format */
    rc = tunnelTermConfigLogic2HwFormat(configTunnelType,GT_FALSE,
                                        configPtr,hwTcamKeyArray);
    if (rc != GT_OK)
        return rc;

    /* convert tunnel termination configuration mask to hardware format */
    rc = tunnelTermConfigLogic2HwFormat(configTunnelType,GT_TRUE,
                                        configMaskPtr,hwTcamMaskArray);
    if (rc != GT_OK)
        return rc;

    /* classify tunnel type according to the required by the function to */
    /* convert tunnel termination action to hardware format              */
    /* No check for wrong cases is needed - done in the switch-case above*/
    if (tunnelType == CPSS_TUNNEL_ETHERNET_OVER_MPLS_E)
    {
        actionTunnelType = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;
    }
    else
    {
        actionTunnelType = CPSS_TUNNEL_IP_OVER_X_E;
    }

    /* convert tunnel termination action to hardware format */
    rc = tunnelTermActionLogic2HwFormat(actionTunnelType,actionPtr,hwTcamActionArray);
    if (rc != GT_OK)
        return rc;

    /* now write entry to the router / tunnel termination TCAM */
    rc = prvCpssDxChRouterTunnelTermTcamSetLine(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                routerTunnelTermTcamIndex,
                                                hwTcamKeyArray,
                                                hwTcamMaskArray,
                                                hwTcamActionArray);

    return rc;
}


/*******************************************************************************
* cpssDxChTunnelTermEntryGet
*
* DESCRIPTION:
*       Get tunnel termination entry from hardware at a given index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM (0..1023)
*
* OUTPUTS:
*       validPtr      - points to whether this tunnel termination entry is valid
*       tunnelTypePtr - points to the type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IP_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    OUT GT_BOOL                             *validPtr,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
{
    GT_U32      hwTcamKeyArray[TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS];      /* TCAM key in hw format            */
    GT_U32      hwTcamMaskArray[TUNNEL_TERMINATION_TCAM_RULE_SIZE_CNS];     /* TCAM mask in hw format           */
    GT_U32      hwTcamActionArray[TUNNEL_TERMINATION_TCAM_ACTION_SIZE_CNS]; /* TCAM action (rule) in hw format  */
    GT_U32      validArr[5];                                                /* TCAM line valid bits             */
    GT_U32      compareModeArr[5];                                          /* TCAM line compare mode           */
    CPSS_TUNNEL_TYPE_ENT    configTunnelType;       /* tunnel type got from config      */
    CPSS_TUNNEL_TYPE_ENT    configMaskTunnelType;   /* tunnel type got from config mask */
    CPSS_TUNNEL_TYPE_ENT    actionTunnelType;       /* tunnel type got from action      */
    GT_STATUS               rc;                     /* function return code             */
    GT_U32                  i;

    /* check pararmeters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_CHECK_LINE_INDEX_MAC(devNum,routerTunnelTermTcamIndex);
    CPSS_NULL_PTR_CHECK_MAC(configPtr);
    CPSS_NULL_PTR_CHECK_MAC(configMaskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    CPSS_NULL_PTR_CHECK_MAC(tunnelTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);


    /* read tunnel termination configuration and action from hardware */
    rc = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                routerTunnelTermTcamIndex,
                                                validArr,
                                                compareModeArr,
                                                hwTcamKeyArray,
                                                hwTcamMaskArray,
                                                hwTcamActionArray);
    if (rc != GT_OK)
        return rc;

    /* line holds valid tunnel termination entry if the following applies:
         - all entries are valid
         - the compare mode or all entries is row compare
         - keyArr[3] bit 31 must be 1 (to indicate TT entry and not IPv6 entry) */
    *validPtr = GT_TRUE;
    for (i = 0 ; i < 5 ; i++)
    {
        /* if entry is not valid or is single compare mode, whole line is not valid */
        if ((validArr[i] == 0) || (compareModeArr[i] == 0))
        {
            *validPtr = GT_FALSE;
            break;
        }
    }
    /* if whole line is valid, verify it is indeed TT entry and not IPv6 entry */
    if ((*validPtr == GT_TRUE) && (((hwTcamKeyArray[3] >> 31) & 0x1) != 1))
    {
        *validPtr = GT_FALSE;
    }
    /* if the entry is not valid, no need to continue to decode it */
    if (*validPtr == GT_FALSE)
        return GT_OK;

    /* convert tunnel termination configuration from hw format to logic format */
    rc = tunnelTermConfigHw2LogicFormat(hwTcamKeyArray,&configTunnelType,configPtr);
    if (rc != GT_OK)
        return rc;

    /* convert tunnel termination configuration mask from hw format to logic format */
    rc = tunnelTermConfigHw2LogicFormat(hwTcamMaskArray,&configMaskTunnelType,configMaskPtr);
    if (rc != GT_OK)
        return rc;

    /* convert tunnel termination action from hw format to logic format */
    rc = tunnelTermActionHw2LogicFormat(hwTcamActionArray,&actionTunnelType,actionPtr);
    if (rc != GT_OK)
        return rc;

    /* classify tunnel type */
    if ((configTunnelType == CPSS_TUNNEL_IPV4_OVER_IPV4_E) ||
        (configTunnelType == CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E) ||
        (configTunnelType == CPSS_TUNNEL_IPV6_OVER_IPV4_E) ||
        (configTunnelType == CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E))
    {
        *tunnelTypePtr = configTunnelType;
    }
    else if (configTunnelType == CPSS_TUNNEL_X_OVER_MPLS_E)
    {
        if (actionTunnelType == CPSS_TUNNEL_ETHERNET_OVER_MPLS_E)
        {
            *tunnelTypePtr = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;
        }
        else if (actionTunnelType == CPSS_TUNNEL_IP_OVER_X_E)
        {
            *tunnelTypePtr = CPSS_TUNNEL_IP_OVER_MPLS_E;
        }
        else
        {
            return GT_BAD_STATE;
        }
    }
    else
    {
        return GT_BAD_STATE;
    }

    return rc;
}


/*******************************************************************************
* cpssDxChTunnelTermEntryInvalidate
*
* DESCRIPTION:
*       Invalidate a tunnel termination entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM (0..1023)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntryInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTunnelTermTcamIndex
)
{
    GT_STATUS   rc;     /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_CHECK_LINE_INDEX_MAC(devNum,routerTunnelTermTcamIndex);

    /* now invalidate the router / tunnel termination TCAM entry */
    rc = prvCpssDxChRouterTunnelTermTcamInvalidateLine(devNum,
                                     CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                     routerTunnelTermTcamIndex);

    return rc;
}


/*******************************************************************************
* cpssDxChTunnelStartEgressFilteringSet
*
* DESCRIPTION:
*       Set globally whether to subject Tunnel Start packets to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgressFilteringSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    /* write enable value */
    value = (enable == GT_TRUE) ? 1 : 0;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,25,1,value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
            filterConfig.globalEnables;
        rc = prvCpssDxChHwPpSetRegField(devNum,regAddr,8,1,value);
    }

    return rc;
}


/*******************************************************************************
* cpssDxChTunnelStartEgressFilteringGet
*
* DESCRIPTION:
*       Get if Tunnel Start packets are globally subject to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgressFilteringGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* read register value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,25,1,&value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
            filterConfig.globalEnables;
        rc = prvCpssDxChHwPpGetRegField(devNum,regAddr,8,1,&value);
    }

    if (rc != GT_OK)
        return rc;

    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTaggingSet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTaggingSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* write enable value */
    value = (enable == GT_TRUE) ? 1 : 0;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,27,1,value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.generalConfig;
        rc = prvCpssDxChHwPpSetRegField(devNum,regAddr,12,1,value);
    }

    return rc;
}


/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTaggingGet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTaggingGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read register value */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,27,1,&value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.generalConfig;
        rc = prvCpssDxChHwPpGetRegField(devNum,regAddr,12,1,&value);
    }

    if (rc != GT_OK)
        return rc;

    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTagModeSet
*
* DESCRIPTION:
*       Set the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*       tagMode     - tunnel start ethernet-over-x vlan tag mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTagModeSet

(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    switch (tagMode)
    {
    case CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E:
        data = 0;
        break;
    case CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E:
        data = 1;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;

        return prvCpssDrvHwPpSetRegField(devNum,regAddr,26,1,data);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.generalConfig;
        return prvCpssDxChHwPpSetRegField(devNum,regAddr,11,1,data);
    }

}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTagModeGet
*
* DESCRIPTION:
*       Get the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       tagModePtr  - pointer to tunnel start ethernet-over-x vlan tag mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_STATE             - on bad state.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTagModeGet

(
    IN  GT_U8                                                devNum,
    OUT CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT *tagModePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_STATUS rc;                /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(tagModePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,26,1,&data);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.generalConfig;
        rc = prvCpssDxChHwPpGetRegField(devNum,regAddr,11,1,&data);
    }

    if (rc != GT_OK)
        return rc;

    switch (data)
    {
    case 0:
        *tagModePtr = CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E;
        break;
    case 1:
        *tagModePtr = CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChTunnelStartPassengerVlanTranslationEnableSet
*
* DESCRIPTION:
*       Controls Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPassengerVlanTranslationEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    data = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 10 , 1, data);

}


/*******************************************************************************
* cpssDxChTunnelStartPassengerVlanTranslationEnableGet
*
* DESCRIPTION:
*       Gets the Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points enable state.
*                     GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPassengerVlanTranslationEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;         /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 10, 1, &value);

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChTunnelTermExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
)
{
    GT_STATUS   rc;         /* function return code */
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in the register */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    /* find register offset according to the exception type */
    switch (exceptionType)
    {
    case CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E:
        offset = 19;
        break;
    case CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E:
        offset = 20;
        break;
    case CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E:
        offset = 21;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* note the commands for all exception types are the same  */
    /* get the value to write according to command             */
    switch (command)
    {
    case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
        value = 0;
        break;
    case CPSS_PACKET_CMD_DROP_HARD_E:
        value = 1;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* write command value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,1,value);

    return rc;
}


/*******************************************************************************
* cpssDxChTunnelCtrlTtExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PTR  - on NULL pointer
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelCtrlTtExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_STATUS   rc;         /* function return code */
    GT_U32      value;      /* register value */
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in the register */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    /* find register offset according to the exception type */
    switch (exceptionType)
    {
    case CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E:
        offset = 19;
        break;
    case CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E:
        offset = 20;
        break;
    case CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E:
        offset = 21;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* read register value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,offset,1,&value);

    if (rc != GT_OK)
        return rc;

    *commandPtr = (value == 0) ? CPSS_PACKET_CMD_TRAP_TO_CPU_E : CPSS_PACKET_CMD_DROP_HARD_E;

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
*
* DESCRIPTION:
*       This feature allows overriding the <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      bitOffset;  /* offset of nit from start of register */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    /* prepare value to write */
    value = (enable == GT_TRUE) ? 1 : 0;

     /* write register value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.trunkNumConfigReg[port >> 2];
    bitOffset = (port & 0x3) + 28;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 1, value);

    return rc;

}

/*******************************************************************************
* cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
*
* DESCRIPTION:
*       The function gets status of the feature which allows overriding the
*       <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable additional offset to tunnel total length
*                       GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      bitOffset;  /* offset of nit from start of register */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

     /* read register value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.trunkNumConfigReg[port >> 2];
    bitOffset = (port & 0x3) + 28;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 1, &value);

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
*
* DESCRIPTION:
*       This API sets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum            - device number
*       additionToLength  - Ip tunnel total length offset (6 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range values
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              additionToLength
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);

    if(additionToLength >= BIT_6)
        return GT_OUT_OF_RANGE;

    /* write register value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCpidReg1;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,26,6,additionToLength);

    return rc;
}

/*******************************************************************************
* cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
*
* DESCRIPTION:
*       This API gets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       additionToLengthPtr  - (pointer to) Ip tunnel total length offset (6 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
(
    IN  GT_U8                               devNum,
    OUT GT_U32                              *additionToLengthPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(additionToLengthPtr);

    /* read register value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCpidReg1;
    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,26,6,additionToLengthPtr);

    return rc;
}
