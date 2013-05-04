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
* cpssDxChTti.c
*
* DESCRIPTION:
*       CPSS tunnel termination implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>

/* the size of tti macToMe table entry in words */
#define TTI_MAC2ME_SIZE_CNS             4

/* the size of tti rule key/mask in words */
#define TTI_RULE_SIZE_CNS               6

/* the size of tti action in words */
#define TTI_ACTION_SIZE_CNS             4

/* the size of tti action type 2 in words */
#define TTI_ACTION_TYPE_2_SIZE_CNS      5

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

/* maximum value for QOS profile field */
#define QOS_PROFILE_MAX_CNS             127

/* maximum value for LTT index field */

/* maximum value for VRF ID field */
#define VRF_ID_MAX_CNS                  4095

/* maximum value for QOS profile field */
#define QOS_PROFILE_MAX_CNS             127

/* maximum value for source id field */
#define SOURCE_ID_MAX_CNS               31

/* maximum value for IPCL index */
#define IPCL_INDEX_MAX_CNS              8191

/* maximum value for central counter index */
#define CENTRAL_COUNTER_MAX_CNS         16383

/* maximum value for DP field */
#define DP_MAX_CNS                      1

/* maximum value for MIM ISID field */
#define MIM_ISID_MAX_CNS                16777215

/* maximum value for MIM iRES field */
#define MIM_IRES_MAX_CNS                3

/* maximum value for time to live (TTL) field */
#define TTL_MAX_CNS                     255

/*******************************************************************************
* ttiConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       keyType           - type of the tunnel; valid options:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
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
static GT_STATUS ttiConfigLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             convertMask,
    IN  CPSS_DXCH_TTI_RULE_UNT             *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    GT_BOOL                 srcIsTrunk      = GT_FALSE;
    GT_U32                  srcPortTrunk    = GT_FALSE;
    GT_BOOL                 isTagged        = GT_FALSE;
    GT_BOOL                 dsaSrcIsTrunk   = GT_FALSE;
    GT_U8                   dsaSrcPortTrunk = 0;
    GT_U8                   dsaSrcDevice    = 0;
    GT_U32                  sourcePortGroupId = 0;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TTI_RULE_SIZE_CNS);

    /* check ipv4/mpls/eth configuration pararmeters only for config, not for mask */
    if (convertMask == GT_FALSE)
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            if (logicFormatPtr->ipv4.common.pclId >= BIT_10)
                return GT_BAD_PARAM;
            srcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->ipv4.common.srcIsTrunk);
            srcPortTrunk = logicFormatPtr->ipv4.common.srcPortTrunk;
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->ipv4.common.vid);
            isTagged = BOOL2BIT_MAC(logicFormatPtr->ipv4.common.isTagged);
            dsaSrcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->ipv4.common.dsaSrcIsTrunk);
            dsaSrcPortTrunk = logicFormatPtr->ipv4.common.dsaSrcPortTrunk;
            dsaSrcDevice = logicFormatPtr->ipv4.common.dsaSrcDevice;
            sourcePortGroupId = logicFormatPtr->ipv4.common.sourcePortGroupId;

            if (logicFormatPtr->ipv4.tunneltype > IPV4_TUNNEL_TYPE_MAX_CNS)
                return GT_BAD_PARAM;
            break;

        case CPSS_DXCH_TTI_KEY_MPLS_E:
            if (logicFormatPtr->mpls.common.pclId >= BIT_10)
                return GT_BAD_PARAM;
            srcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->mpls.common.srcIsTrunk);
            srcPortTrunk = logicFormatPtr->mpls.common.srcPortTrunk;
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->mpls.common.vid);
            isTagged = BOOL2BIT_MAC(logicFormatPtr->mpls.common.isTagged);
            dsaSrcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->mpls.common.dsaSrcIsTrunk);
            dsaSrcPortTrunk = logicFormatPtr->mpls.common.dsaSrcPortTrunk;
            dsaSrcDevice = logicFormatPtr->mpls.common.dsaSrcDevice;
            sourcePortGroupId = logicFormatPtr->mpls.common.sourcePortGroupId;

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
            if (logicFormatPtr->eth.common.pclId >= BIT_10)
                return GT_BAD_PARAM;
            srcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->eth.common.srcIsTrunk);
            srcPortTrunk = logicFormatPtr->eth.common.srcPortTrunk;
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->eth.common.vid);
            isTagged = BOOL2BIT_MAC(logicFormatPtr->eth.common.isTagged);
            dsaSrcIsTrunk = BOOL2BIT_MAC(logicFormatPtr->eth.common.dsaSrcIsTrunk);
            dsaSrcPortTrunk = logicFormatPtr->eth.common.dsaSrcPortTrunk;
            dsaSrcDevice = logicFormatPtr->eth.common.dsaSrcDevice;
            if (! PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
                sourcePortGroupId = logicFormatPtr->eth.common.sourcePortGroupId;

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
            /* fields for xCat; Lion; xCat2 */
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                if (logicFormatPtr->eth.srcId > SOURCE_ID_MAX_CNS)
                    return GT_BAD_PARAM;
                if (logicFormatPtr->eth.dsaQosProfile > QOS_PROFILE_MAX_CNS)
                    return GT_BAD_PARAM;
            }

            break;

        case CPSS_DXCH_TTI_KEY_MIM_E:
            PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                 CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

            /* MIM tunnel type is supported by xCat and Lion*/
            if (logicFormatPtr->eth.common.pclId >= BIT_10)
                return GT_BAD_PARAM;
            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mim.bUp);
            if (logicFormatPtr->mim.bDp > DP_MAX_CNS)
                return GT_BAD_PARAM;
            if (logicFormatPtr->mim.iSid > MIM_ISID_MAX_CNS)
                return GT_BAD_PARAM;
            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->mim.iUp);
            if (logicFormatPtr->mim.iDp > DP_MAX_CNS)
                return GT_BAD_PARAM;
            if (logicFormatPtr->mim.iRes1 > MIM_IRES_MAX_CNS)
                return GT_BAD_PARAM;
            if (logicFormatPtr->mim.iRes2 > MIM_IRES_MAX_CNS)
                return GT_BAD_PARAM;
            break;

        }

        /* check common params */
        if (sourcePortGroupId >= BIT_2)
        {
            /* 2 bits for source port group id */
            return GT_BAD_PARAM;
        }

        switch (srcIsTrunk)
        {
        case 0: /* src is port */
            if(srcPortTrunk >= BIT_6)
            {
                /* 6 bits for the port number */
                return GT_BAD_PARAM;
            }
            break;
        case 1: /* src is trunk */
            if(srcPortTrunk >= BIT_7)
            {
                /* 7 bits for the trunkId */
                return GT_BAD_PARAM;
            }
            break;
        }

        if (isTagged)
        {
            switch (dsaSrcIsTrunk)
            {
            case 0: /* src is port */
                if(dsaSrcPortTrunk >= BIT_6)
                {
                    /* 6 bits for the port number */
                    return GT_BAD_PARAM;
                }
                break;
            case 1: /* src is trunk */
                if(dsaSrcPortTrunk >= BIT_7)
                {
                    /* 7 bits for the trunkId */
                    return GT_BAD_PARAM;
                }
                break;
            }

            if(dsaSrcDevice >= BIT_7)
            {
                /* 7 bits for the dsaSrcDevice */
                return GT_BAD_PARAM;
            }
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
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],29,2,logicFormatPtr->ipv4.common.sourcePortGroupId);
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
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],29,2,logicFormatPtr->mpls.common.sourcePortGroupId);
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
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],28,4,logicFormatPtr->eth.vid1);

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,8,(logicFormatPtr->eth.vid1 >> 4));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],8,3,logicFormatPtr->eth.up1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],11,1,logicFormatPtr->eth.cfi1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],12,16,logicFormatPtr->eth.etherType);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],28,1,BOOL2BIT_MAC(logicFormatPtr->eth.macToMe));
         /* handle word 4 (bits 128-159) */
        /* reserved */

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->eth.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->eth.common.dsaSrcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],8,5,logicFormatPtr->eth.srcId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],13,7,logicFormatPtr->eth.dsaQosProfile);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],29,2,logicFormatPtr->eth.common.sourcePortGroupId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:

        /* handle word 0 (bits 0-31) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,10,logicFormatPtr->mim.common.pclId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],10,8,logicFormatPtr->mim.common.srcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],18,1,BOOL2BIT_MAC(logicFormatPtr->mim.common.srcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],19,12,logicFormatPtr->mim.common.vid);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,BOOL2BIT_MAC(logicFormatPtr->mim.common.isTagged));

        /* handle word 1 (bits 32-63) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,8,logicFormatPtr->mim.common.mac.arEther[5]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],8,8,logicFormatPtr->mim.common.mac.arEther[4]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,8,logicFormatPtr->mim.common.mac.arEther[3]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],24,8,logicFormatPtr->mim.common.mac.arEther[2]);

        /* handle word 2 (bits 64-95) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,8,logicFormatPtr->mim.common.mac.arEther[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],8,8,logicFormatPtr->mim.common.mac.arEther[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,7,logicFormatPtr->mim.common.dsaSrcDevice);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,3,logicFormatPtr->mim.bUp);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],26,1,logicFormatPtr->mim.bDp);
        /* note bit 27 is reserved */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],28,4,(logicFormatPtr->mim.iSid & 0xf));

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,20,(logicFormatPtr->mim.iSid >> 4));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],20,3,logicFormatPtr->mim.iUp);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],23,1,logicFormatPtr->mim.iDp);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],24,2,logicFormatPtr->mim.iRes1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],26,2,logicFormatPtr->mim.iRes2);
         /* handle word 4 (bits 128-159) */
        /* reserved */

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->mim.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->mim.common.dsaSrcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],29,2,logicFormatPtr->mim.common.sourcePortGroupId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */
        break;

    default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* ttiConfigHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       keyType           - type of the tunnel; valid options:
*                               CPSS_DXCH_TTI_KEY_IP_V4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       hwFormatArray     - tunnel termination configuration in HW format (6 words).
*
* OUTPUTS:
*       logicFormatPtr    - points to tti configuration in logic format
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
static GT_STATUS ttiConfigHw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_DXCH_TTI_RULE_UNT              *logicFormatPtr
)
{
    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:

        logicFormatPtr->ipv4.common.pclId        = U32_GET_FIELD_MAC(hwFormatArray[0],0,10);
        logicFormatPtr->ipv4.common.srcPortTrunk = U32_GET_FIELD_MAC(hwFormatArray[0],10,8);
        logicFormatPtr->ipv4.common.srcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],18,1));
        logicFormatPtr->ipv4.common.vid          = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],19,12);
        logicFormatPtr->ipv4.common.isTagged     = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],31,1));

        logicFormatPtr->ipv4.common.mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],0,8);
        logicFormatPtr->ipv4.common.mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],8,8);
        logicFormatPtr->ipv4.common.mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],16,8);
        logicFormatPtr->ipv4.common.mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],24,8);

        logicFormatPtr->ipv4.common.mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],0,8);
        logicFormatPtr->ipv4.common.mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],8,8);
        logicFormatPtr->ipv4.common.dsaSrcDevice   = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],16,7);
        logicFormatPtr->ipv4.tunneltype            = U32_GET_FIELD_MAC(hwFormatArray[2],23,3);
        logicFormatPtr->ipv4.srcIp.arIP[3]         = (GT_U8)(U32_GET_FIELD_MAC(hwFormatArray[2],26,6) |
                                                            (U32_GET_FIELD_MAC(hwFormatArray[3],0,2) << 6));

        logicFormatPtr->ipv4.srcIp.arIP[2]  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[3],2,8);
        logicFormatPtr->ipv4.srcIp.arIP[1]  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[3],10,8);
        logicFormatPtr->ipv4.srcIp.arIP[0]  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[3],18,8);
        logicFormatPtr->ipv4.destIp.arIP[3] = (GT_U8)(U32_GET_FIELD_MAC(hwFormatArray[3],26,6) |
                                                     (U32_GET_FIELD_MAC(hwFormatArray[4],0,2) << 6));

        logicFormatPtr->ipv4.destIp.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[4],2,8);
        logicFormatPtr->ipv4.destIp.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[4],10,8);
        logicFormatPtr->ipv4.destIp.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[4],18,8);
        logicFormatPtr->ipv4.isArp          = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],26,1));

        logicFormatPtr->ipv4.common.dsaSrcPortTrunk = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[5],0,7);
        logicFormatPtr->ipv4.common.dsaSrcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[5],7,1));
        logicFormatPtr->ipv4.common.sourcePortGroupId = (GT_U32)U32_GET_FIELD_MAC(hwFormatArray[5],29,2);

        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:

        logicFormatPtr->mpls.common.pclId        = U32_GET_FIELD_MAC(hwFormatArray[0],0,10);
        logicFormatPtr->mpls.common.srcPortTrunk = U32_GET_FIELD_MAC(hwFormatArray[0],10,8);
        logicFormatPtr->mpls.common.srcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],18,1));
        logicFormatPtr->mpls.common.vid          = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],19,12);
        logicFormatPtr->mpls.common.isTagged     = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],31,1));

        logicFormatPtr->mpls.common.mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],0,8);
        logicFormatPtr->mpls.common.mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],8,8);
        logicFormatPtr->mpls.common.mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],16,8);
        logicFormatPtr->mpls.common.mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],24,8);

        logicFormatPtr->mpls.common.mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],0,8);
        logicFormatPtr->mpls.common.mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],8,8);
        logicFormatPtr->mpls.common.dsaSrcDevice   = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],16,7);
        logicFormatPtr->mpls.exp0                  = U32_GET_FIELD_MAC(hwFormatArray[2],23,3);
        logicFormatPtr->mpls.exp1                  = U32_GET_FIELD_MAC(hwFormatArray[2],26,3);
        logicFormatPtr->mpls.label0                = U32_GET_FIELD_MAC(hwFormatArray[2],29,3) |
                                                     (U32_GET_FIELD_MAC(hwFormatArray[3],0,17) << 3);

        logicFormatPtr->mpls.label1 = U32_GET_FIELD_MAC(hwFormatArray[3],17,15) |
                                      (U32_GET_FIELD_MAC(hwFormatArray[4],0,5) << 15);

        logicFormatPtr->mpls.exp2              = U32_GET_FIELD_MAC(hwFormatArray[4],5,3);
        logicFormatPtr->mpls.label2            = U32_GET_FIELD_MAC(hwFormatArray[4],8,20);
        logicFormatPtr->mpls.numOfLabels       = U32_GET_FIELD_MAC(hwFormatArray[4],28,2);
        logicFormatPtr->mpls.protocolAboveMPLS = U32_GET_FIELD_MAC(hwFormatArray[4],30,2);

        logicFormatPtr->mpls.common.dsaSrcPortTrunk = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[5],0,7);
        logicFormatPtr->mpls.common.dsaSrcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[5],7,1));
        logicFormatPtr->mpls.common.sourcePortGroupId = (GT_U32)U32_GET_FIELD_MAC(hwFormatArray[5],29,2);

        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:

        logicFormatPtr->eth.common.pclId         = U32_GET_FIELD_MAC(hwFormatArray[0],0,10);
        logicFormatPtr->eth.common.srcPortTrunk = U32_GET_FIELD_MAC(hwFormatArray[0],10,8);
        logicFormatPtr->eth.common.srcIsTrunk    = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],18,1));
        logicFormatPtr->eth.common.vid       = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],19,12);
        logicFormatPtr->eth.common.isTagged      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],31,1));

        logicFormatPtr->eth.common.mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],0,8);
        logicFormatPtr->eth.common.mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],8,8);
        logicFormatPtr->eth.common.mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],16,8);
        logicFormatPtr->eth.common.mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],24,8);

        logicFormatPtr->eth.common.mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],0,8);
        logicFormatPtr->eth.common.mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],8,8);
        logicFormatPtr->eth.common.dsaSrcDevice   = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],16,7);

        logicFormatPtr->eth.up0           = U32_GET_FIELD_MAC(hwFormatArray[2],23,3);
        logicFormatPtr->eth.cfi0          = U32_GET_FIELD_MAC(hwFormatArray[2],26,1);
        logicFormatPtr->eth.isVlan1Exists = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],27,1));
        logicFormatPtr->eth.vid1          = (GT_U16)(U32_GET_FIELD_MAC(hwFormatArray[2],28,4) |
                                                    (U32_GET_FIELD_MAC(hwFormatArray[3],0,8) << 4));

        logicFormatPtr->eth.up1       = U32_GET_FIELD_MAC(hwFormatArray[3],8,3);
        logicFormatPtr->eth.cfi1      = U32_GET_FIELD_MAC(hwFormatArray[3],11,1);
        logicFormatPtr->eth.etherType = U32_GET_FIELD_MAC(hwFormatArray[3],12,16);
        logicFormatPtr->eth.macToMe   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],28,1));

        logicFormatPtr->eth.common.dsaSrcPortTrunk = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[5],0,7);
        logicFormatPtr->eth.common.dsaSrcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[5],7,1));
        logicFormatPtr->eth.srcId                  = U32_GET_FIELD_MAC(hwFormatArray[5],8,5);
        logicFormatPtr->eth.dsaQosProfile          = U32_GET_FIELD_MAC(hwFormatArray[5],13,7);
        logicFormatPtr->eth.common.sourcePortGroupId = (GT_U32)U32_GET_FIELD_MAC(hwFormatArray[5],29,2);

        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

        /* MIM tunnel type is supported by xCat and Lion */
        logicFormatPtr->mim.common.pclId         = U32_GET_FIELD_MAC(hwFormatArray[0],0,10);
        logicFormatPtr->mim.common.srcPortTrunk = U32_GET_FIELD_MAC(hwFormatArray[0],10,8);
        logicFormatPtr->mim.common.srcIsTrunk    = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],18,1));
        logicFormatPtr->mim.common.vid       = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],19,12);
        logicFormatPtr->mim.common.isTagged      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],31,1));

        logicFormatPtr->mim.common.mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],0,8);
        logicFormatPtr->mim.common.mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],8,8);
        logicFormatPtr->mim.common.mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],16,8);
        logicFormatPtr->mim.common.mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],24,8);

        logicFormatPtr->mim.common.mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],0,8);
        logicFormatPtr->mim.common.mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],8,8);
        logicFormatPtr->mim.common.dsaSrcDevice   = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],16,7);

        logicFormatPtr->mim.bUp   = U32_GET_FIELD_MAC(hwFormatArray[2],23,3);
        logicFormatPtr->mim.bDp   = U32_GET_FIELD_MAC(hwFormatArray[2],26,1);

        logicFormatPtr->mim.iSid  = U32_GET_FIELD_MAC(hwFormatArray[2],28,4) |
                                    (U32_GET_FIELD_MAC(hwFormatArray[3],0,20) << 4);

        logicFormatPtr->mim.iUp   = U32_GET_FIELD_MAC(hwFormatArray[3],20,3);
        logicFormatPtr->mim.iDp   = U32_GET_FIELD_MAC(hwFormatArray[3],23,1);
        logicFormatPtr->mim.iRes1 = U32_GET_FIELD_MAC(hwFormatArray[3],24,2);
        logicFormatPtr->mim.iRes2 = U32_GET_FIELD_MAC(hwFormatArray[3],26,2);

        logicFormatPtr->mim.common.dsaSrcPortTrunk = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[5],0,7);
        logicFormatPtr->mim.common.dsaSrcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[5],7,1));
        logicFormatPtr->mim.common.sourcePortGroupId = (GT_U32)U32_GET_FIELD_MAC(hwFormatArray[5],29,2);

        break;

    default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* ttiActionType1Logic2HwFormat
*
* DESCRIPTION:
*       Converts a given tti action type 1 from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*
*       devNum            - device number
*       logicFormatPtr    - points to tti action in logic format
*
* OUTPUTS:
*       hwFormatArray     - tti action in hardware format (4 words)
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
static GT_STATUS ttiActionType1Logic2HwFormat
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
    GT_U32      routerLookupPtr = 0;/* pointer to the Router Lookup Translation Table entry (hw format) */
    GT_STATUS   rc;                 /* return status */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TTI_ACTION_SIZE_CNS);

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
            rc = prvCpssDxChNetIfCpuToDsaCode(logicFormatPtr->userDefinedCpuCode,
                                         (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT*)(GT_U32)&userDefinedCpuCode);
            if (rc != GT_OK)
            {
                return rc;
            }
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
    case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E:
        modifyDSCP = 0;
        break;
    case CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E:
        modifyDSCP = 1;
        break;
    case CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E:
        modifyDSCP = 2;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate tunnel termination action modify User Priority parameter */
    switch (logicFormatPtr->modifyUpEnable)
    {
    case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E:
        modifyUP = 0;
        break;
    case CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E:
        modifyUP = 1;
        break;
    case CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E:
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
        routerLookupPtr = logicFormatPtr->routerLookupPtr;
        if (logicFormatPtr->routerLookupPtr > (fineTuningPtr->tableSize.routerAndTunnelTermTcam * 4))
            return GT_BAD_PARAM;
        /* FEr#2018: Limited number of Policy-based routes */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E))
        {
            switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
            {
            /* DxCh3 devices support index 0,1,2,3…(max IP TCAM row) only */
            case CPSS_PP_FAMILY_CHEETAH3_E:
                if (logicFormatPtr->routerLookupPtr >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
                    return GT_NOT_SUPPORTED;
                break;
            default:
                /* Do nothing */
                break;
            }
        }
        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        redirectCommand = 4;
        if (logicFormatPtr->vrfId > VRF_ID_MAX_CNS)
            return GT_BAD_PARAM;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if(logicFormatPtr->redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            if(logicFormatPtr->egressInterface.devPort.portNum >= BIT_6)
            {
                /* 6 bits for the port number */
                return GT_BAD_PARAM;
            }
            break;
         case CPSS_INTERFACE_TRUNK_E:
            if(logicFormatPtr->egressInterface.trunkId >= BIT_7)
            {
                /* 7 bits for the trunkId */
                return GT_BAD_PARAM;
            }
            break;
        case CPSS_INTERFACE_VIDX_E:
            if (logicFormatPtr->egressInterface.vidx > VIDX_INTERFACE_MAX_CNS)
                return GT_BAD_PARAM;
            break;
        default:
            return GT_BAD_PARAM;
        }
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

    if (logicFormatPtr->policerIndex >= BIT_12)
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

    if (logicFormatPtr->tunnelStartPtr >= fineTuningPtr->tableSize.tunnelStart)
        return GT_BAD_PARAM;

    if (logicFormatPtr->counterIndex >= BIT_14)
        return GT_BAD_PARAM;

    /* handle word 0 (bits 0-31) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,3,pktCommand);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],3,8,userDefinedCpuCode);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],11,1,BOOL2BIT_MAC(logicFormatPtr->mirrorToIngressAnalyzerEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],12,1,qosPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],14,7,logicFormatPtr->qosProfile);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],21,2,modifyDSCP);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],23,2,modifyUP);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],25,1,BOOL2BIT_MAC(logicFormatPtr->activateCounter));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],26,6,(logicFormatPtr->counterIndex));
    /* handle word 1 (bits 32-63) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,2,redirectCommand); /* sets only 2 bits. the last bit handled later */

    switch (redirectCommand)
    {
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,1,0);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],3,6,logicFormatPtr->egressInterface.devPort.portNum);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],9,5,logicFormatPtr->egressInterface.devPort.devNum);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],14,1,0);
            break;
        case CPSS_INTERFACE_TRUNK_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,1,1);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],3,7,logicFormatPtr->egressInterface.trunkId);
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
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,13,routerLookupPtr);
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
    if (redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],10,1,BOOL2BIT_MAC(logicFormatPtr->tunnelStart));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],11,10,logicFormatPtr->tunnelStartPtr);
    }
    else
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],10,1,0);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],11,10,0);
    }
    /* bit 21 is reserved */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],22,3,qosTrustMode);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],25,2,passengerPktType);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],27,1,BOOL2BIT_MAC(logicFormatPtr->copyTtlFromTunnelHeader));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],28,1,BOOL2BIT_MAC(logicFormatPtr->bridgeBypass));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],29,3,logicFormatPtr->up);

    /* handle word 3 (bits 96-127) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,1,BOOL2BIT_MAC(logicFormatPtr->tunnelTerminate));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],1,1,BOOL2BIT_MAC(logicFormatPtr->actionStop));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],2,8,(logicFormatPtr->counterIndex >> 6));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],10,4,(logicFormatPtr->policerIndex >> 8));
    if (redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],14,3,(logicFormatPtr->tunnelStartPtr >> 10));
    }
    else
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],14,3,0);
    }
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],17,1,BOOL2BIT_MAC(logicFormatPtr->sourceIdSetEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],18,5,logicFormatPtr->sourceId);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],23,1,(redirectCommand >> 2));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],24,2,(routerLookupPtr >> 13));

    return GT_OK;
}

/*******************************************************************************
* ttiActionType1Hw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti action type 1 from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       hwFormatArray     - tti action in hardware format (4 words)
*
* OUTPUTS:
*       logicFormatPtr    - points to tti action in logic format
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
static GT_STATUS ttiActionType1Hw2LogicFormat
(
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_DXCH_TTI_ACTION_STC            *logicFormatPtr
)
{
    GT_STATUS rc = GT_OK;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],0,3))
    {
        case 0:
            logicFormatPtr->command = CPSS_PACKET_CMD_FORWARD_E;
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

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        rc = prvCpssDxChNetIfDsaToCpuCode((U32_GET_FIELD_MAC(hwFormatArray[0],3,8)),
                                   &logicFormatPtr->userDefinedCpuCode);
        if(rc != GT_OK)
            return rc;
    }
    else
        logicFormatPtr->userDefinedCpuCode = 0;

    logicFormatPtr->mirrorToIngressAnalyzerEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],11,1));


    switch (U32_GET_FIELD_MAC(hwFormatArray[0],12,1))
    {
        case 0:
            logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            break;
        case 1:
            logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    logicFormatPtr->qosProfile = U32_GET_FIELD_MAC(hwFormatArray[0],14,7);

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],21,2))
    {
        case 0:
            logicFormatPtr->modifyDscpEnable = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
            break;
        case 1:
            logicFormatPtr->modifyDscpEnable = CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E;
            break;
        case 2:
            logicFormatPtr->modifyDscpEnable = CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],23,2))
    {
        case 0:
            logicFormatPtr->modifyUpEnable = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E;
            break;
        case 1:
            logicFormatPtr->modifyUpEnable = CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E;
            break;
        case 2:
            logicFormatPtr->modifyUpEnable = CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],25,1))
    {
        case 0:
            logicFormatPtr->activateCounter = GT_FALSE;
            break;
        case 1:
            logicFormatPtr->activateCounter = GT_TRUE;
            break;
        default:
            return GT_BAD_STATE;
    }

    logicFormatPtr->counterIndex = U32_GET_FIELD_MAC(hwFormatArray[0],26,6) |
                                  (U32_GET_FIELD_MAC(hwFormatArray[3],2,8) << 6);
             ;
    switch (U32_GET_FIELD_MAC(hwFormatArray[1],0,2) |
            (U32_GET_FIELD_MAC(hwFormatArray[3],23,1) << 2))
    {
    case 0:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;
        break;
    case 1:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;
        if (U32_GET_FIELD_MAC(hwFormatArray[1],14,1) == 0) /* unicast */
        {
            if (U32_GET_FIELD_MAC(hwFormatArray[1],2,1) == 0) /* port */
            {
                logicFormatPtr->egressInterface.devPort.portNum = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],3,6);
                logicFormatPtr->egressInterface.devPort.devNum  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],9,5);
                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_PORT_E;
            }
            else /* trunk */
            {
                logicFormatPtr->egressInterface.trunkId = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[1],3,7);
                logicFormatPtr->egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
            }
        }
        else /* multicast */
        {
            logicFormatPtr->egressInterface.vidx = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[1],3,11);
            logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VIDX_E;
        }
        break;
    case 2:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
        logicFormatPtr->routerLookupPtr = U32_GET_FIELD_MAC(hwFormatArray[1],2,13) |
                                         (U32_GET_FIELD_MAC(hwFormatArray[3],24,2) << 13);
        break;
    case 4:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_VRF_ID_ASSIGN_E;
        logicFormatPtr->vrfId           = U32_GET_FIELD_MAC(hwFormatArray[1],2,12);
        break;
    default:
        return GT_BAD_STATE;
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[1],15,1))
    {
        case 0:
            logicFormatPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            break;
        case 1:
            logicFormatPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    logicFormatPtr->nestedVlanEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],16,1));

    switch (U32_GET_FIELD_MAC(hwFormatArray[1],17,2))
    {
        case 0:
            logicFormatPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E;
            break;
        case 1:
            logicFormatPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;
        case 2:
            logicFormatPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E;
            break;
        case 3:
            logicFormatPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    logicFormatPtr->vlanId         = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[1],19,12);
    logicFormatPtr->remapDSCP      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],31,1));
    logicFormatPtr->bindToPolicer  = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],0,1));
    logicFormatPtr->policerIndex   = U32_GET_FIELD_MAC(hwFormatArray[2],1,8) |
                                    (U32_GET_FIELD_MAC(hwFormatArray[3],10,4) << 8);
    logicFormatPtr->vntl2Echo      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],9,1));
    logicFormatPtr->tunnelStart    = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],10,1));
    logicFormatPtr->tunnelStartPtr = U32_GET_FIELD_MAC(hwFormatArray[2],11,10) |
                                    (U32_GET_FIELD_MAC(hwFormatArray[3],14,3) << 10);

    switch (U32_GET_FIELD_MAC(hwFormatArray[2],22,3))
    {
        case 0:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E;
            break;
         case 1:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E;
            break;
         case 2:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E;
            break;
         case 3:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E;
            break;
         case 4:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_UNTRUST_E;
            break;
         case 5:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[2],25,2))
    {
         case 0:
            logicFormatPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV4_E;
            break;
         case 1:
            logicFormatPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV6_E;
            break;
         case 2:
            logicFormatPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E;
            break;
         case 3:
            logicFormatPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E;
            break;
         default:
            return GT_BAD_STATE;
    }

    logicFormatPtr->copyTtlFromTunnelHeader = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],27,1));
    logicFormatPtr->bridgeBypass             = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],28,1));
    logicFormatPtr->up                       = U32_GET_FIELD_MAC(hwFormatArray[2],29,3);
    logicFormatPtr->tunnelTerminate         = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],0,1));
    logicFormatPtr->actionStop           = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],1,1));
    logicFormatPtr->sourceIdSetEnable       = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],17,1));
    logicFormatPtr->sourceId             = U32_GET_FIELD_MAC(hwFormatArray[3],18,5);

    return GT_OK;
}

/*******************************************************************************
* ttiActionType2Logic2HwFormat
*
* DESCRIPTION:
*       Converts a given tti action type 2 from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*
*       devNum            - device number
*       logicFormatPtr    - points to tti action in logic format
*
* OUTPUTS:
*       hwFormatArray     - tti action in hardware format (5 words)
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
static GT_STATUS ttiActionType2Logic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_2_STC         *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    GT_U32      ttPassengerPktType = 0;         /* tt passenger packet type (hw format) */
    GT_U32      tsPassengerPacketType = 0;      /* ts passenger packet type (hw format) */
    GT_U32      mplsCommand = 0;                /* mpls command (hw format) */
    GT_U32      pktCommand = 0;                 /* tti packet forwarding command (hw format) */
    GT_U32      redirectCommand = 0;            /* where to redirect the packet (hw format) */
    GT_U32      userDefinedCpuCode = 0;         /* user defined cpu code (hw format) */
    GT_U32      tag0VlanPrecedence = 0;         /* VLAN assignment precedence mode (hw format) */
    GT_U32      qosPrecedence = 0;              /* QoS precedence mode (hw format) */
    GT_U32      modifyDSCP = 0;                 /* modify DSCP mode (hw format) */
    GT_U32      modifyTag0Up = 0;               /* modify user priority mode (hw format) */
    GT_U32      tag1UpCommand = 0;              /* tag 1 up command (hw format) */
    GT_U32      tag0VlanCmd = 0;                /* tag 0 vlan command applied to packets matching the TTI (hw format) */
    GT_U32      tag1VlanCmd = 0;                /* tag 1 vlan command applied to packets matching the TTI (hw format) */
    GT_U32      pcl0OverrideConfigIndex = 0;    /* pcl 0 Override Config Index (hw format)   */
    GT_U32      pcl0_1OverrideConfigIndex = 0;  /* pcl 0-1 Override Config Index (hw format) */
    GT_U32      pcl1OverrideConfigIndex = 0;    /* pcl 1 Override Config Index (hw format)   */
    GT_U32      routerLttPtr = 0;               /* pointer to the Router Lookup Translation Table entry (hw format) */
    GT_U32      tempRouterLttPtr = 0;           /* pointer to the Router Lookup Translation Table entry (hw format) */
    GT_STATUS   rc;                             /* return status */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TTI_ACTION_TYPE_2_SIZE_CNS);

    /********************************************************/
    /* check params and translate params to hardware format */
    /********************************************************/

    /* check and convert tt passenger packet type */
    switch (logicFormatPtr->ttPassengerPacketType)
    {
    case CPSS_DXCH_TTI_PASSENGER_IPV4V6_E:
        ttPassengerPktType = 0;
        break;
    case CPSS_DXCH_TTI_PASSENGER_MPLS_E:
        ttPassengerPktType = 1;
        break;
    case CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E:
        ttPassengerPktType = 2;
        break;
    case CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E:
        ttPassengerPktType = 3;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* in MPLS LSR functionality, tt passenger type must be MPLS */
    if (logicFormatPtr->tunnelTerminate == GT_FALSE)
    {
        if (logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_POP1_CMD_E ||
            logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_POP2_CMD_E ||
            logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E)
        {
            if (logicFormatPtr->ttPassengerPacketType != CPSS_DXCH_TTI_PASSENGER_MPLS_E)
                return GT_BAD_PARAM;
        }
    }

    /* check and convert ts passenger type */
    if (logicFormatPtr->redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E &&
        logicFormatPtr->tunnelStart == GT_TRUE)
    {
        if ((logicFormatPtr->tunnelTerminate == GT_FALSE) &&
            (logicFormatPtr->mplsCommand != CPSS_DXCH_TTI_MPLS_NOP_CMD_E))
        {
            if (logicFormatPtr->tsPassengerPacketType != CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E)
                return GT_BAD_PARAM;
        }

        /* modifyMacDa == GT_TRUE requires that tunnelStart == GT_FALSE*/
        if (logicFormatPtr->modifyMacDa == GT_TRUE)
        {
            return GT_BAD_PARAM;
        }

        switch (logicFormatPtr->tsPassengerPacketType)
        {
        case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
            tsPassengerPacketType = 0;
            break;
        case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
            tsPassengerPacketType = 1;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    /* check and convert mpls command */
    switch (logicFormatPtr->mplsCommand)
    {
    case CPSS_DXCH_TTI_MPLS_NOP_CMD_E:
        mplsCommand = 0;
        break;
    case CPSS_DXCH_TTI_MPLS_SWAP_CMD_E:
        mplsCommand = 1;
        break;
    case CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E:
        mplsCommand = 2;
        break;
    case CPSS_DXCH_TTI_MPLS_POP1_CMD_E:
        mplsCommand = 3;
        break;
    case CPSS_DXCH_TTI_MPLS_POP2_CMD_E:
        mplsCommand = 4;
        break;
    case CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E:
        mplsCommand = 7;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if (logicFormatPtr->mplsCommand != CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
    {
        if (logicFormatPtr->mplsTtl > TTL_MAX_CNS)
            return GT_BAD_PARAM;
    }

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
        if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
            return GT_NOT_APPLICABLE_DEVICE;
        redirectCommand = 2;
        if(logicFormatPtr->routerLttPtr >= BIT_15)
        {
            return GT_OUT_OF_RANGE;
        }
        routerLttPtr = logicFormatPtr->routerLttPtr;
        if (logicFormatPtr->routerLttPtr > (fineTuningPtr->tableSize.routerAndTunnelTermTcam * 4))
            return GT_BAD_PARAM;

        /* FEr#2018: Limited number of Policy-based routes */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E))
        {
            /* xCat devices support index 0,4,8,12…(max IP TCAM row * 4) only */
            if ((logicFormatPtr->routerLttPtr % 4) != 0)
                return GT_NOT_SUPPORTED;
        }

        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
            return GT_NOT_APPLICABLE_DEVICE;
        redirectCommand = 4;
        if (logicFormatPtr->vrfId > VRF_ID_MAX_CNS)
            return GT_BAD_PARAM;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if(logicFormatPtr->redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            if(logicFormatPtr->egressInterface.devPort.portNum >= BIT_6)
            {
                /* 6 bits for the port number */
                return GT_BAD_PARAM;
            }
            break;
         case CPSS_INTERFACE_TRUNK_E:
            if(logicFormatPtr->egressInterface.trunkId >= BIT_7)
            {
                /* 7 bits for the trunkId */
                return GT_BAD_PARAM;
            }
            break;
        case CPSS_INTERFACE_VIDX_E:
            if (logicFormatPtr->egressInterface.vidx > VIDX_INTERFACE_MAX_CNS)
                return GT_BAD_PARAM;
            break;
        default:
            return GT_BAD_PARAM;
        }

        if (logicFormatPtr->tunnelStart == GT_TRUE)
        {
            if (logicFormatPtr->tunnelStartPtr >= fineTuningPtr->tableSize.tunnelStart)
                return GT_BAD_PARAM;
        }
        else
        {
            if (logicFormatPtr->arpPtr >= (fineTuningPtr->tableSize.routerArp))
                return GT_BAD_PARAM;
        }

    }

    if (logicFormatPtr->sourceIdSetEnable == GT_TRUE)
    {
        if (logicFormatPtr->sourceId > SOURCE_ID_MAX_CNS)
            return GT_BAD_PARAM;

    }

    /* translate tag 0 VLAN command from action parameter */
    switch (logicFormatPtr->tag0VlanCmd)
    {
    case CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E:
        tag0VlanCmd = 0;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
        tag0VlanCmd = 1;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E:
        tag0VlanCmd = 2;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
        tag0VlanCmd = 3;
        break;
    default:
        return GT_BAD_PARAM;
    }

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->tag0VlanId);

    /* translate tag 1 VLAN command from action parameter */
    switch (logicFormatPtr->tag1VlanCmd)
    {
    case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
        tag1VlanCmd = 0;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
        tag1VlanCmd = 1;
        break;
    default:
        return GT_BAD_PARAM;
    }

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->tag1VlanId);

    /* translate VLAN precedence from action parameter */
    switch (logicFormatPtr->tag0VlanPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        tag0VlanPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        tag0VlanPrecedence = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if (logicFormatPtr->bindToPolicerMeter == GT_TRUE ||
        logicFormatPtr->bindToPolicer == GT_TRUE)
    {
        if (logicFormatPtr->policerIndex >= BIT_12)
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

    if (logicFormatPtr->keepPreviousQoS == GT_FALSE)
    {
        if (logicFormatPtr->qosProfile > QOS_PROFILE_MAX_CNS)
            return GT_BAD_PARAM;
    }

    /* translate tunnel termination action modify User Priority parameter */
    switch (logicFormatPtr->modifyTag0Up)
    {
    case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E:
        modifyTag0Up = 0;
        break;
    case CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E:
        modifyTag0Up = 1;
        break;
    case CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E:
        modifyTag0Up = 2;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate tunnel termination tag1 up command parameter */
    switch (logicFormatPtr->tag1UpCommand)
    {
    case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E:
        tag1UpCommand = 0;
        break;
    case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E:
        tag1UpCommand = 1;
        break;
    case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E:
        tag1UpCommand = 2;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* translate tunnel termination action modify DSCP parameter */
    switch (logicFormatPtr->modifyDscp)
    {
    case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E:
        modifyDSCP = 0;
        break;
    case CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E:
        modifyDSCP = 1;
        break;
    case CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E:
        modifyDSCP = 2;
        break;
    default:
        return GT_BAD_PARAM;
    }

    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->tag0Up);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->tag1Up);

    if (logicFormatPtr->hashMaskIndex >= BIT_4)
    {
        return GT_BAD_PARAM;
    }

    if (logicFormatPtr->redirectCommand != CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        switch (logicFormatPtr->pcl0OverrideConfigIndex)
        {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            pcl0OverrideConfigIndex = 0;
            break;
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            pcl0OverrideConfigIndex = 1;
            break;
        default:
            return GT_BAD_PARAM;
        }

        switch (logicFormatPtr->pcl0_1OverrideConfigIndex)
        {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            pcl0_1OverrideConfigIndex = 0;
            break;
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            pcl0_1OverrideConfigIndex = 1;
            break;
        default:
            return GT_BAD_PARAM;
        }

        switch (logicFormatPtr->pcl1OverrideConfigIndex)
        {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            pcl1OverrideConfigIndex = 0;
            break;
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            pcl1OverrideConfigIndex = 1;
            break;
        default:
            return GT_BAD_PARAM;
        }

        if (logicFormatPtr->iPclConfigIndex > IPCL_INDEX_MAX_CNS)
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
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
            return GT_BAD_PARAM;
    }
    else
        userDefinedCpuCode = 0;

    if (logicFormatPtr->bindToCentralCounter == GT_TRUE)
    {
        if (logicFormatPtr->centralCounterIndex > CENTRAL_COUNTER_MAX_CNS)
            return GT_BAD_PARAM;
    }

    /* handle word 0 (bits 0-31) and also some of word 1 (bits 32-63) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,3,pktCommand);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],3,8,userDefinedCpuCode);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],11,1,BOOL2BIT_MAC(logicFormatPtr->mirrorToIngressAnalyzerEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],12,3,redirectCommand);

    switch (redirectCommand)
    {
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,1,0); /* target is not trunk */
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],16,6,logicFormatPtr->egressInterface.devPort.portNum);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],22,5,logicFormatPtr->egressInterface.devPort.devNum);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],27,1,0); /* target not VIDX */
            break;
        case CPSS_INTERFACE_TRUNK_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,1,1); /* target is trunk */
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],16,7,logicFormatPtr->egressInterface.trunkId);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],27,1,0); /* target not VIDX */
            break;
        case CPSS_INTERFACE_VIDX_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,12,logicFormatPtr->egressInterface.vidx);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],27,1,1); /* target is VIDX */
            break;
        default:
            return GT_BAD_PARAM;
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],28,1,BOOL2BIT_MAC(logicFormatPtr->tunnelStart));
        if (logicFormatPtr->tunnelStart == GT_TRUE)
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],29,3,logicFormatPtr->tunnelStartPtr & 0x7);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,10,logicFormatPtr->tunnelStartPtr >> 3);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],10,1,tsPassengerPacketType);
        }
        else
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],29,3,logicFormatPtr->arpPtr & 0x7);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,11,logicFormatPtr->arpPtr >> 3);
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],11,1,BOOL2BIT_MAC(logicFormatPtr->vntl2Echo));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->modifyMacDa));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],23,1,BOOL2BIT_MAC(logicFormatPtr->modifyMacSa));
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
       /* LTT entry is row based however in this field the LTT is treated as column based.
          Bits [11:0] indicate row while bits [14:13] indicate column, bit 12 is not used.
          The Formula for translating the LTT entry to column based is as follow:
          [11:0] << 2 + [14:13]   (Note: bit 12 not used). */
        tempRouterLttPtr = (((routerLttPtr & 0x3FFC) >> 2) | ((routerLttPtr & 0x3) << 13));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,15,tempRouterLttPtr);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,logicFormatPtr->iPclConfigIndex & 0x1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,12,logicFormatPtr->iPclConfigIndex >> 1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->ResetSrcPortGroupId));
        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,12,logicFormatPtr->vrfId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,logicFormatPtr->iPclConfigIndex & 0x1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,12,logicFormatPtr->iPclConfigIndex >> 1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->ResetSrcPortGroupId));
        break;
    case CPSS_DXCH_TTI_NO_REDIRECT_E:
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,1,BOOL2BIT_MAC(logicFormatPtr->multiPortGroupTtiEnable));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,logicFormatPtr->iPclConfigIndex & 0x1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,12,logicFormatPtr->iPclConfigIndex >> 1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->ResetSrcPortGroupId));
    default:
        /* do noting */
        break;
    }

    /* handle word 1 (bits 32-63) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],12,1,BOOL2BIT_MAC(logicFormatPtr->bindToCentralCounter));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],13,14,logicFormatPtr->centralCounterIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],27,1,BOOL2BIT_MAC(logicFormatPtr->bindToPolicerMeter));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],28,1,BOOL2BIT_MAC(logicFormatPtr->bindToPolicer));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],29,3,logicFormatPtr->policerIndex & 0x7);

    /* handle word 2 (bits 64-95) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,9,logicFormatPtr->policerIndex >> 3);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],9,1,BOOL2BIT_MAC(logicFormatPtr->sourceIdSetEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],10,5,logicFormatPtr->sourceId);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],15,1,BOOL2BIT_MAC(logicFormatPtr->actionStop));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,1,BOOL2BIT_MAC(logicFormatPtr->bridgeBypass));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],17,1,BOOL2BIT_MAC(logicFormatPtr->ingressPipeBypass));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],18,1,pcl1OverrideConfigIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],19,1,pcl0_1OverrideConfigIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],20,1,pcl0OverrideConfigIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],21,1,tag0VlanPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],22,1,BOOL2BIT_MAC(logicFormatPtr->nestedVlanEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,2,tag0VlanCmd);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],25,7,logicFormatPtr->tag0VlanId & 0x7F);

    /* handle word 3 (bits 96-127) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,5,logicFormatPtr->tag0VlanId >> 7);
    if (logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],5,1,tag1VlanCmd);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],6,12,logicFormatPtr->tag1VlanId);
    }
    else
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],5,8,logicFormatPtr->mplsTtl);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],13,1,BOOL2BIT_MAC(logicFormatPtr->enableDecrementTtl));
    }
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],18,1,qosPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],19,7,logicFormatPtr->qosProfile);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],26,2,modifyDSCP);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],28,2,modifyTag0Up);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],30,1,BOOL2BIT_MAC(logicFormatPtr->keepPreviousQoS));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],31,1,BOOL2BIT_MAC(logicFormatPtr->trustUp));

    /* handle word 4 (bits 128-159) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],0,1,BOOL2BIT_MAC(logicFormatPtr->trustDscp));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],1,1,BOOL2BIT_MAC(logicFormatPtr->trustExp));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],2,1,BOOL2BIT_MAC(logicFormatPtr->remapDSCP));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],3,3,logicFormatPtr->tag0Up);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],6,2,tag1UpCommand);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],8,3,logicFormatPtr->tag1Up);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],11,2,ttPassengerPktType);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],13,1,BOOL2BIT_MAC(logicFormatPtr->copyTtlFromTunnelHeader));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],14,1,BOOL2BIT_MAC(logicFormatPtr->tunnelTerminate));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],15,3,mplsCommand);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],18,4,logicFormatPtr->hashMaskIndex & 0xF);

    return GT_OK;
}

/*******************************************************************************
* ttiActionType2Hw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti action type 2 from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       hwFormatArray     - tti action in hardware format (5 words)
*
* OUTPUTS:
*       logicFormatPtr    - points to tti action in logic format
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
static GT_STATUS ttiActionType2Hw2LogicFormat
(
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_DXCH_TTI_ACTION_2_STC          *logicFormatPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    tempRouterLttPtr=0;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],0,3))
    {
        case 0:
            logicFormatPtr->command = CPSS_PACKET_CMD_FORWARD_E;
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

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        rc = prvCpssDxChNetIfDsaToCpuCode((U32_GET_FIELD_MAC(hwFormatArray[0],3,8)),
                                   &logicFormatPtr->userDefinedCpuCode);
        if(rc != GT_OK)
            return rc;
    }

    logicFormatPtr->mirrorToIngressAnalyzerEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],11,1));

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],12,3))
    {
    case 0:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;
        logicFormatPtr->multiPortGroupTtiEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],15,1));
        logicFormatPtr->iPclConfigIndex = U32_GET_FIELD_MAC(hwFormatArray[0],31,1) |
                                          (U32_GET_FIELD_MAC(hwFormatArray[1],0,12) << 1);
        break;
    case 1:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;
        if (U32_GET_FIELD_MAC(hwFormatArray[0],27,1) == 0) /* target is not VIDX */
        {
            if (U32_GET_FIELD_MAC(hwFormatArray[0],15,1) == 0) /* target is not trunk */
            {
                logicFormatPtr->egressInterface.devPort.portNum = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[0],16,6);
                logicFormatPtr->egressInterface.devPort.devNum  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[0],22,5);
                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_PORT_E;
            }
            else /* target is trunk */
            {
                logicFormatPtr->egressInterface.trunkId = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],16,7);
                logicFormatPtr->egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
            }
        }
        else /* target is VIDX */
        {
            logicFormatPtr->egressInterface.vidx = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],15,12);
            logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VIDX_E;
        }

        logicFormatPtr->tunnelStart = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],28,1));
        if (logicFormatPtr->tunnelStart == GT_TRUE)
        {
            logicFormatPtr->tunnelStartPtr = U32_GET_FIELD_MAC(hwFormatArray[0],29,3) |
                                             (U32_GET_FIELD_MAC(hwFormatArray[1],0,10) << 3);
            switch (U32_GET_FIELD_MAC(hwFormatArray[1],11,1))
            {
            case 0:
                logicFormatPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;
                break;
            case 1:
                logicFormatPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
                break;
            default:
                return GT_BAD_STATE;
            }
        }
        else
        {
            logicFormatPtr->arpPtr = U32_GET_FIELD_MAC(hwFormatArray[0],29,3) |
                                     (U32_GET_FIELD_MAC(hwFormatArray[1],0,11) << 3);
        }
        logicFormatPtr->vntl2Echo = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],11,1));
        logicFormatPtr->modifyMacDa = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],22,1));
        logicFormatPtr->modifyMacSa = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],23,1));
        break;
    case 2:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
        /* LTT entry is row based however in this field the LTT is treated as column based.
          Bits [11:0] indicate row while bits [14:13] indicate column, bit 12 is not used.
          The Formula for translating the LTT entry to column based is as follow:
          [11:0] << 2 + [14:13]   (Note: bit 12 not used). */
        tempRouterLttPtr = U32_GET_FIELD_MAC(hwFormatArray[0],15,15);
        logicFormatPtr->routerLttPtr = (((tempRouterLttPtr & 0xFFF) << 2) |
                                        ((tempRouterLttPtr & 0x6000) >> 13));

        logicFormatPtr->iPclConfigIndex = U32_GET_FIELD_MAC(hwFormatArray[0],31,1) |
                                          (U32_GET_FIELD_MAC(hwFormatArray[1],0,12) << 1);
        break;
    case 4:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_VRF_ID_ASSIGN_E;
        logicFormatPtr->vrfId           = U32_GET_FIELD_MAC(hwFormatArray[0],15,12);
        logicFormatPtr->iPclConfigIndex = U32_GET_FIELD_MAC(hwFormatArray[0],31,1) |
                                          (U32_GET_FIELD_MAC(hwFormatArray[1],0,12) << 1);
        break;
    default:
        return GT_BAD_STATE;
    }

    logicFormatPtr->bindToCentralCounter = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],12,1));
    if (logicFormatPtr->bindToCentralCounter == GT_TRUE)
    {
        logicFormatPtr->centralCounterIndex   = U32_GET_FIELD_MAC(hwFormatArray[1],13,14);
    }

    logicFormatPtr->bindToPolicerMeter = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],27,1));
    logicFormatPtr->bindToPolicer = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],28,1));
    if ((logicFormatPtr->bindToPolicerMeter == GT_TRUE) ||
        (logicFormatPtr->bindToPolicer == GT_TRUE))
    {
        logicFormatPtr->policerIndex = U32_GET_FIELD_MAC(hwFormatArray[1],29,3) |
                                       (U32_GET_FIELD_MAC(hwFormatArray[2],0,9) << 3);
    }

    logicFormatPtr->sourceIdSetEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],9,1));
    if (logicFormatPtr->sourceIdSetEnable == GT_TRUE)
    {
        logicFormatPtr->sourceId   = U32_GET_FIELD_MAC(hwFormatArray[2],10,5);
    }

    logicFormatPtr->actionStop        = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],15,1));
    logicFormatPtr->bridgeBypass      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],16,1));
    logicFormatPtr->ingressPipeBypass = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],17,1));

    if (logicFormatPtr->redirectCommand != CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        switch (U32_GET_FIELD_MAC(hwFormatArray[2],18,1))
        {
        case 0:
            logicFormatPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;
        case 1:
            logicFormatPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;
        default:
            return GT_BAD_STATE;
        }

        switch (U32_GET_FIELD_MAC(hwFormatArray[2],19,1))
        {
        case 0:
            logicFormatPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;
        case 1:
            logicFormatPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;
        default:
            return GT_BAD_STATE;
        }

        switch (U32_GET_FIELD_MAC(hwFormatArray[2],20,1))
        {
        case 0:
            logicFormatPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;
        case 1:
            logicFormatPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;
        default:
            return GT_BAD_STATE;
        }
        logicFormatPtr->ResetSrcPortGroupId = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],22,1));
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[2],21,1))
    {
    case 0:
        logicFormatPtr->tag0VlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        break;
    case 1:
        logicFormatPtr->tag0VlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    logicFormatPtr->nestedVlanEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],22,1));

    switch (U32_GET_FIELD_MAC(hwFormatArray[2],23,2))
    {
    case 0:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E;
        break;
    case 1:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
        break;
    case 2:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E;
        break;
    case 3:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    logicFormatPtr->tag0VlanId = (GT_U16)(U32_GET_FIELD_MAC(hwFormatArray[2],25,7) |
                                          (U32_GET_FIELD_MAC(hwFormatArray[3],0,5) << 7));

    switch (U32_GET_FIELD_MAC(hwFormatArray[4],15,3))
    {
    case 0:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_NOP_CMD_E;
        break;
    case 1:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_SWAP_CMD_E;
        break;
    case 2:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E;
        break;
    case 3:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP1_CMD_E;
        break;
    case 4:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP2_CMD_E;
        break;
    case 7:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    if (logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
    {
        switch (U32_GET_FIELD_MAC(hwFormatArray[3],5,1))
        {
        case 0:
            logicFormatPtr->tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;
        case 1:
            logicFormatPtr->tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
            break;
        default:
            return GT_BAD_STATE;
        }
        logicFormatPtr->tag1VlanId = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[3],6,12);
    }
    else
    {
        if (logicFormatPtr->mplsCommand != CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
        {
            logicFormatPtr->mplsTtl = U32_GET_FIELD_MAC(hwFormatArray[3],5,8);
        }
        logicFormatPtr->enableDecrementTtl = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],13,1));
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[3],18,1))
    {
    case 0:
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        break;
    case 1:
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    logicFormatPtr->keepPreviousQoS = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],30,1));
    if (logicFormatPtr->keepPreviousQoS == GT_FALSE)
    {
        logicFormatPtr->qosProfile   = U32_GET_FIELD_MAC(hwFormatArray[3],19,7);
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[3],26,2))
    {
    case 0:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
        break;
    case 1:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E;
        break;
    case 2:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[3],28,2))
    {
    case 0:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E;
        break;
    case 1:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E;
        break;
    case 2:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    logicFormatPtr->trustUp   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],31,1));
    logicFormatPtr->trustDscp = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],0,1));
    logicFormatPtr->trustExp  = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],1,1));
    logicFormatPtr->remapDSCP = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],2,1));
    logicFormatPtr->tag0Up    = U32_GET_FIELD_MAC(hwFormatArray[4],3,3);

    switch (U32_GET_FIELD_MAC(hwFormatArray[4],6,2))
    {
    case 0:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E;
        break;
    case 1:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E;
        break;
    case 2:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    logicFormatPtr->tag1Up    = U32_GET_FIELD_MAC(hwFormatArray[4],8,3);

    switch (U32_GET_FIELD_MAC(hwFormatArray[4],11,2))
    {
    case 0:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV4V6_E;
        break;
    case 1:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_MPLS_E;
        break;
    case 2:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E;
        break;
    case 3:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    logicFormatPtr->copyTtlFromTunnelHeader = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],13,1));
    logicFormatPtr->tunnelTerminate         = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],14,1));
    logicFormatPtr->hashMaskIndex           = U32_GET_FIELD_MAC(hwFormatArray[4],18,4);

    return GT_OK;
}

/*******************************************************************************
* ttiActionLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tti action from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       actionType        - type of the action to use
*       logicFormatPtr    - points to tti action in logic format
*
* OUTPUTS:
*       hwFormatArray     - tti action in hardware format (4 words)
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
static GT_STATUS ttiActionLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT           *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    switch (actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            return ttiActionType1Logic2HwFormat(devNum,
                                                &logicFormatPtr->type1,
                                                hwFormatArray);
        }
        else
            return GT_BAD_PARAM;
    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            return ttiActionType2Logic2HwFormat(devNum,
                                                &logicFormatPtr->type2,
                                                hwFormatArray);
        }
        else
            return GT_BAD_PARAM;
    default:
        break;
    }
    return GT_BAD_PARAM;

}

/*******************************************************************************
* ttiActionHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti action type 2 from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       hwFormatArray     - tti action in hardware format (4 words)
*       actionType        - type of the action to use
*
* OUTPUTS:
*       logicFormatPtr    - points to tti action in logic format
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
static GT_STATUS ttiActionHw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *logicFormatPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    switch (actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            return ttiActionType1Hw2LogicFormat(hwFormatArray,
                                                &logicFormatPtr->type1);
        }
        else
            return GT_BAD_PARAM;
    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            return ttiActionType2Hw2LogicFormat(hwFormatArray,
                                                &logicFormatPtr->type2);
        }
        else
            return GT_BAD_PARAM;
    default:
        break;
    }

    return GT_BAD_PARAM;
}

/*******************************************************************************
* cpssDxChTtiMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table (0..7)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr
)
{
    return cpssDxChTtiPortGroupMacToMeSet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,entryIndex,valuePtr,maskPtr);
}


/*******************************************************************************
* cpssDxChTtiMacToMeGet
*
* DESCRIPTION:
*       This function gets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table (0..7).
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *maskPtr
)
{
    return cpssDxChTtiPortGroupMacToMeGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,entryIndex,valuePtr,maskPtr);
}

/*******************************************************************************
* cpssDxChTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       enable        - GT_TRUE: enable TTI lookup
*                       GT_FALSE: disable TTI lookup
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
GT_STATUS cpssDxChTtiPortLookupEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             enable
)
{

    GT_U32      value;      /* value to write */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

     /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    /* DxCh3 */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            bit = 49;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            bit = 50;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            bit = 51;
            break;
        default:
            if ((keyType == CPSS_DXCH_TTI_KEY_MIM_E) && (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                return GT_NOT_SUPPORTED;
            else
                return GT_BAD_PARAM;
        }
    }
    else /* xCat; Lion; xCat2 */
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            bit = 51;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            bit = 55;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            bit = 57;
            break;
        case CPSS_DXCH_TTI_KEY_MIM_E:
            bit = 60;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         (bit / 32),    /* word   */
                                         (bit % 32),    /* offset */
                                         1,             /* length */
                                         value);

    return rc;

}


/*******************************************************************************
* cpssDxChTtiPortLookupEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of  the
*       TTI lookup for the specified key type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup
*                       GT_TRUE: TTI lookup is enabled
*                       GT_FALSE: TTI lookup is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id, port or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortLookupEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* DxCh3 */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            bit = 49;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            bit = 50;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            bit = 51;
            break;
        default:
            if ((keyType == CPSS_DXCH_TTI_KEY_MIM_E) && (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                return GT_NOT_SUPPORTED;
            else
                return GT_BAD_PARAM;
        }
    }
    else /* xCat; Lion; xCat2 */
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            bit = 51;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            bit = 55;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            bit = 57;
            break;
        case CPSS_DXCH_TTI_KEY_MIM_E:
            bit = 60;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    /* read value from hardware table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         (bit / 32),    /* word   */
                                         (bit %32),     /* offset */
                                         1,             /* length */
                                         &value);
    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}
/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for tunneled packets
*                       GT_FALSE: disable IPv4 TTI lookup only for tunneled packets
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
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        bit = 54;
    }
    else /* xCat; Lion */
    {
        bit = 52;
    }

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         (bit / 32),    /* word   */
                                         (bit % 32),    /* offset */
                                         1,             /* length */
                                         value);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only tunneled packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       tunneled packets
*                       GT_TRUE: IPv4 TTI lookup only for tunneled packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for tunneled packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        bit = 54;
    }
    else /* xCat; Lion */
    {
        bit = 52;
    }

    /* read hardware value */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        (bit / 32),     /* word   */
                                        (bit % 32),     /* offset */
                                        1,              /* length */
                                        &value);

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for mac to me packets
*                       GT_FALSE: disable IPv4 TTI lookup only for mac to me packets
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
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E );

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        bit = 53;
    }
    else /* xCat; Lion */
    {
        bit = 54;
    }

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         (bit / 32),    /* word   */
                                         (bit % 32),    /* offset */
                                         1,             /* length */
                                         value);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: IPv4 TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E );

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        bit = 53;
    }
    else /* xCat; Lion */
    {
        bit = 54;
    }

    /* read hardware value */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        (bit / 32),     /* word   */
                                        (bit % 32),     /* offset */
                                        1,              /* length */
                                        &value);

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChTtiIpv4McEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for IPv4 multicast
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: enable TTI lookup for IPv4 MC
*                       GT_FALSE: disable TTI lookup for IPv4 MC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4McEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{

    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    /* write value */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,23,1,value);
    }
    else /* xCat; Lion */
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,5,1,value);
    }

    return rc;


}

/*******************************************************************************
* cpssDxChTtiIpv4McEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of TTI lookup for
*       IPv4 multicast (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup for IPv4 MC
*                       GT_TRUE: TTI lookup for IPv4 MC enabled
*                       GT_FALSE: TTI lookup for IPv4 MC disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4McEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      value = 0;  /* hardware write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read value */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,23,1,&value);
    }
    else /* xCat; Lion */
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
        rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,5,1,&value);
    }

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MPLS TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable MPLS TTI lookup only for mac to me packets
*                       GT_FALSE: disable MPLS TTI lookup only for mac to me packets
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
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        bit = 52;
    }
    else /* xCat; Lion */
    {
        bit = 56;
    }

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         (bit / 32),    /* word   */
                                         (bit % 32),    /* offset */
                                         1,             /* length */
                                         value);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MPLS TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: MPLS TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MPLS TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        bit = 52;
    }
    else /* xCat; Lion */
    {
        bit = 56;
    }

    /* read hardware value */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        (bit / 32),     /* word   */
                                        (bit % 32),     /* offset */
                                        1,              /* length */
                                        &value);

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MIM TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE:  enable MIM TTI lookup only for mac to me packets
*                       GT_FALSE: disable MIM TTI lookup only for mac to me packets
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
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableSet
(
    IN  GT_U8           devNum,
    IN  GT_U8           port,
    IN  GT_BOOL         enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      bit = 0;    /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    bit = 61;

    /* write value to hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         (bit / 32),    /* word   */
                                         (bit % 32),    /* offset */
                                         1,             /* length */
                                         value);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MIM TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MIM TTI lookup only for
*                       mac to me packets
*                       GT_TRUE:  MIM TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MIM TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_U8           port,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit = 0;    /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    bit = 61;

    /* read hardware value */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        (bit / 32),     /* word   */
                                        (bit % 32),     /* offset */
                                        1,              /* length */
                                        &value);

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       keyType           - TTI key type; valid values:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 means don't care bit (corresponding
*                           bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    return cpssDxChTtiPortGroupRuleSet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow,
        keyType,patternPtr,maskPtr,actionType,actionPtr);
}

/*******************************************************************************
* cpssDxChTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the rule in the TCAM
*       keyType           - TTI key type; valid values:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 means don't care bit (corresponding
*                           bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    return cpssDxChTtiPortGroupRuleGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow,keyType,
        patternPtr,maskPtr,actionType,actionPtr);
}

/*******************************************************************************
* cpssDxChTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       routerTtiTcamRow - Index of the rule in the TCAM
*       actionType        - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    return cpssDxChTtiPortGroupRuleActionUpdate(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow,
        actionType,actionPtr);
}

/*******************************************************************************
* cpssDxChTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  GT_BOOL                             valid
)
{
    return cpssDxChTtiPortGroupRuleValidStatusSet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow,valid);
}

/*******************************************************************************
* cpssDxChTtiRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    OUT GT_BOOL                             *validPtr
)
{
    return cpssDxChTtiPortGroupRuleValidStatusGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow,validPtr);
}

/*******************************************************************************
* cpssDxChTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
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
GT_STATUS cpssDxChTtiMacModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
{
    return cpssDxChTtiPortGroupMacModeSet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,keyType,macMode);
}

/*******************************************************************************
* cpssDxChTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E (
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; Lion; xCat2)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
{
    return cpssDxChTtiPortGroupMacModeGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,keyType,macModePtr);
}

/*******************************************************************************
* cpssDxChTtiPclIdSet
*
* DESCRIPTION:
*       This function sets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E
*       pclId         - PCL ID value (10 bits)
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
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPclIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_U32                          pclId
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      offset;         /* offset in register */
    GT_STATUS   rc;             /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(pclId >= BIT_10)
        return GT_BAD_PARAM;

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        offset  = 0;
        break;
    case CPSS_DXCH_TTI_KEY_MPLS_E:
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        offset  = 10;
        break;
    case CPSS_DXCH_TTI_KEY_ETH_E:
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        offset  = 20;
        break;
    case CPSS_DXCH_TTI_KEY_MIM_E:
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig1;
        offset  = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* write value */
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,10,pclId);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPclIdGet
*
* DESCRIPTION:
*       This function gets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E
*
* OUTPUTS:
*       pclIdPtr      - (points to) PCL ID value (10 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPclIdGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    OUT GT_U32                          *pclIdPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in register */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(pclIdPtr);

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        offset  = 0;
        break;
    case CPSS_DXCH_TTI_KEY_MPLS_E:
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        offset  = 10;
        break;
    case CPSS_DXCH_TTI_KEY_ETH_E:
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        offset  = 20;
        break;
    case CPSS_DXCH_TTI_KEY_MIM_E:
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig1;
        offset  = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* read value */
    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,offset,10,pclIdPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiIpv4GreEthTypeSet
*
* DESCRIPTION:
*       This function sets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_DXCH_TTI_IPV4_GRE0_E
*                           CPSS_DXCH_TTI_IPV4_GRE1_E
*       ethType       - Ethernet type value (range 16 bits)
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
GT_STATUS cpssDxChTtiIpv4GreEthTypeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT     greTunnelType,
    IN  GT_U32                              ethType
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in register */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    switch (greTunnelType)
    {
    case CPSS_DXCH_TTI_IPV4_GRE0_E:
        offset = 0;
        break;
    case CPSS_DXCH_TTI_IPV4_GRE1_E:
        offset = 16;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* write value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vntReg.vntGreEtherTypeConfReg;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,16,ethType);
    }
    else /* xCat; Lion */
    {
        /* write value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiIpv4GreEthertype;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,16,ethType);
    }

    return rc;
}


/*******************************************************************************
* cpssDxChTtiIpv4GreEthTypeGet
*
* DESCRIPTION:
*       This function gets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_DXCH_TTI_IPV4_GRE0_E
*                           CPSS_DXCH_TTI_IPV4_GRE1_E
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or GRE tunnel type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4GreEthTypeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT     greTunnelType,
    OUT GT_U32                              *ethTypePtr

)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in register */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(ethTypePtr);

    switch (greTunnelType)
    {
    case CPSS_DXCH_TTI_IPV4_GRE0_E:
        offset = 0;
        break;
    case CPSS_DXCH_TTI_IPV4_GRE1_E:
        offset = 16;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* read value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vntReg.vntGreEtherTypeConfReg;
    }
    else /* xCat; Lion */
    {
        /* read value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiIpv4GreEthertype;
    }

    return prvCpssDrvHwPpGetRegField(devNum,regAddr,offset,16,ethTypePtr);
}

/*******************************************************************************
* cpssDxChTtiMimEthTypeSet
*
* DESCRIPTION:
*       This function sets the MIM Ethernet type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       ethType       - Ethernet type value (range 16 bits)
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
*   In xCat and Lion we have 2 registers used for ethertype configuration.
*   One for ethertype identification of incoming tunneled packets in TTI,
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration. These registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMimEthTypeSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          ethType
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;    /* register address */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if (ethType >= BIT_16)
        return GT_BAD_PARAM;

    /* write value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.specialEthertypes;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,16,16,ethType);
    if(rc != GT_OK)
        return rc;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltIEthertype;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,0,16,ethType);
    if(rc != GT_OK)
        return rc;

    return rc;

}

/*******************************************************************************
* cpssDxChTtiMimEthTypeGet
*
* DESCRIPTION:
*       This function gets the MIM Ethernet type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In xCat and Lion we have 2 registers used for ethertype configuration.
*   One for ethertype identification of incoming tunneled packets in TTI,
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration. These registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMimEthTypeGet
(
    IN  GT_U8           devNum,
    OUT GT_U32          *ethTypePtr
)
{
    GT_U32      regAddr;    /* register address */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(ethTypePtr);

    /* we are reading only specialEthertypes register because it reflects the
       value in the hdrAltIEthertype as well */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.specialEthertypes;
    return prvCpssDrvHwPpGetRegField(devNum,regAddr,16,16,ethTypePtr);
}

/*******************************************************************************
* cpssDxChTtiMplsEthTypeSet
*
* DESCRIPTION:
*       This function sets the MPLS Ethernet type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       ucMcSet       - whether to set it for unicast packets or multicast.
*       ethType       - Ethernet type value (range 16 bits)
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
*   In xCat and Lion we have 2 registers used for ethertype configuration.
*   One for ethertype identification of incoming tunneled packets in TTI,
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration. These registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMplsEthTypeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    IN  GT_U32                          ethType
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if (ethType >= BIT_16)
        return GT_BAD_PARAM;

    switch (ucMcSet)
    {
    case CPSS_IP_UNICAST_E:
        offset = 0;
        break;
    case CPSS_IP_MULTICAST_E:
        offset = 16;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* write value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.mplsEthertypes;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,16,ethType);
    if(rc != GT_OK)
        return rc;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltMplsEthertypes;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,16,ethType);
    if(rc != GT_OK)
        return rc;

    return rc;

}

/*******************************************************************************
* cpssDxChTtiMplsEthTypeGet
*
* DESCRIPTION:
*       This function gets the MPLS Ethernet type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       ucMcSet       - whether to get it for unicast packets or multicast.
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In xCat and Lion we have 2 registers used for ethertype configuration.
*   One for ethertype identification of incoming tunneled packets in TTI,
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration. These registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMplsEthTypeGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    OUT GT_U32                          *ethTypePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(ethTypePtr);

    switch (ucMcSet)
    {
    case CPSS_IP_UNICAST_E:
        offset = 0;
        break;
    case CPSS_IP_MULTICAST_E:
        offset = 16;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* we are reading only mplsEthertypes register because it reflects the
       value in the hdrAltMplsEthertypes as well */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.mplsEthertypes;
    return prvCpssDrvHwPpGetRegField(devNum,regAddr,offset,16,ethTypePtr);
}

/*******************************************************************************
* cpssDxChTtiExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
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
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E - (APPLICABLE DEVICES: xCat; Lion; xCat2)
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E - (APPLICABLE DEVICES: xCat; Lion; xCat2)
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
)
{
    GT_STATUS   rc = GT_OK; /* function return code */
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in the register */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


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

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* find register offset according to the exception type */
        switch (exceptionType)
        {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
            offset = 19;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
            offset = 20;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
            offset = 21;
            break;
        default:
            if (((exceptionType == CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E) ||
                (exceptionType == CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E)) &&
                (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                return GT_NOT_SUPPORTED;
            else
                return GT_BAD_PARAM;
        }
        /* write command value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,1,value);
    }
    else /* xCat; Lion */
    {
        /* find register offset according to the exception type */
        switch (exceptionType)
        {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
            offset = 4;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
            offset = 9;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
            offset = 6;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E:
            offset = 7;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E:
            offset = 8;
            break;
        default:
            return GT_BAD_PARAM;
        }
        /* write command value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,1,value);
    }

    return rc;
}


/*******************************************************************************
* cpssDxChTtiExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Commands for the different exceptions are:
*
**       CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E - (APPLICABLE DEVICES: xCat; Lion; xCat2)
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E - (APPLICABLE DEVICES: xCat; Lion; xCat2)
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
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
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* find register offset according to the exception type */
        switch (exceptionType)
        {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
            offset = 19;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
            offset = 20;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
            offset = 21;
            break;
        default:
            if (((exceptionType == CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E) ||
                (exceptionType == CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E)) &&
                (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                return GT_NOT_SUPPORTED;
            else
                return GT_BAD_PARAM;
        }
        /* read register value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,offset,1,&value);
    }
    else /* xCat; Lion */
    {
        /* find register offset according to the exception type */
        switch (exceptionType)
        {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
            offset = 4;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
            offset = 9;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
            offset = 6;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E:
            offset = 7;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E:
            offset = 8;
            break;
        default:
            return GT_BAD_PARAM;
        }
        /* read register value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
        rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,offset,1,&value);
    }

    if (rc != GT_OK)
        return rc;

    *commandPtr = (value == 0) ? CPSS_PACKET_CMD_TRAP_TO_CPU_E : CPSS_PACKET_CMD_DROP_HARD_E;

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortGroupMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table (0..7)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr
)
{
    GT_U32      hwMacToMeArray[TTI_MAC2ME_SIZE_CNS]; /* macToMe config in hardware format */
    GT_U32      portGroupId;                         /* port group Id                     */
    GT_U32      rc;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(valuePtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* clear output data */
    cpssOsMemSet(hwMacToMeArray, 0, sizeof(GT_U32) * TTI_MAC2ME_SIZE_CNS);

    hwMacToMeArray[0] = (GT_HW_MAC_LOW16(&(valuePtr->mac)) << 16) | ((valuePtr->vlanId & 0xfff) << 4);
    hwMacToMeArray[1] = GT_HW_MAC_HIGH32(&(valuePtr->mac));
    hwMacToMeArray[2] = (GT_HW_MAC_LOW16(&(maskPtr->mac)) << 16) | ((maskPtr->vlanId & 0xfff) << 4);
    hwMacToMeArray[3] = GT_HW_MAC_HIGH32(&(maskPtr->mac));

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* write mac to me entry to the hardware */
        rc =  prvCpssDxChPortGroupWriteTableEntry(
            devNum,portGroupId,PRV_CPSS_DXCH3_TABLE_MAC2ME_E,entryIndex,hwMacToMeArray);

        if (rc != GT_OK)
            return rc;
    }

    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupMacToMeGet
*
* DESCRIPTION:
*       This function gets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table (0..7).
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *maskPtr
)
{
    GT_U32      hwMacToMeArray[TTI_MAC2ME_SIZE_CNS]; /* macToMe config in hardware format */
    GT_U32      portGroupId;                         /* port group Id                     */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(valuePtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* read mac to me entry from the hardware */
    if (prvCpssDxChPortGroupReadTableEntry(
        devNum,portGroupId,PRV_CPSS_DXCH3_TABLE_MAC2ME_E,entryIndex,hwMacToMeArray)!= GT_OK)
    return GT_HW_ERROR;

    valuePtr->mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[0], 16, 8);
    valuePtr->mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[0], 24, 8);
    valuePtr->mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[1],  0, 8);
    valuePtr->mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[1],  8, 8);
    valuePtr->mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[1], 16, 8);
    valuePtr->mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[1], 24, 8);
    valuePtr->vlanId = (GT_U16)U32_GET_FIELD_MAC(hwMacToMeArray[0], 4, 12);

    maskPtr->mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[2], 16, 8);
    maskPtr->mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[2], 24, 8);
    maskPtr->mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[3],  0, 8);
    maskPtr->mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[3],  8, 8);
    maskPtr->mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[3], 16, 8);
    maskPtr->mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[3], 24, 8);
    maskPtr->vlanId = (GT_U16)U32_GET_FIELD_MAC(hwMacToMeArray[2], 4, 12);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
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
GT_STATUS cpssDxChTtiPortGroupMacModeSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
{
    GT_U32      value;          /* value to write       */
    GT_U32      regAddr;        /* register address     */
    GT_U32      offset;         /* offset in register   */
    GT_STATUS   rc;             /* function return code */
    GT_U32      portGroupId;    /* port group Id        */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* prepare value to write */
    switch (macMode)
    {
    case CPSS_DXCH_TTI_MAC_MODE_DA_E:
        value = 0;
        break;
    case CPSS_DXCH_TTI_MAC_MODE_SA_E:
        value = 1;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            offset = 25;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            offset = 24;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            offset = 26;
            break;
        default:
            if ((keyType == CPSS_DXCH_TTI_KEY_MIM_E) && (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                return GT_NOT_SUPPORTED;
            else
                return GT_BAD_PARAM;
        }

        /* write value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum,portGroupId,regAddr,offset,1,value);

            if (rc != GT_OK)
                return rc;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

    }
    else /* xCat; Lion; xCat2 */
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            offset = 0;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            offset = 1;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            offset = 2;
            break;
        case CPSS_DXCH_TTI_KEY_MIM_E:
            offset = 3;
            break;
        default:
            return GT_BAD_PARAM;
        }

        /* write value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,offset,1,value);

            if (rc != GT_OK)
                return rc;

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; Lion; xCat2)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
{
    GT_U32      value;      /* value to write       */
    GT_U32      regAddr;    /* register address     */
    GT_U32      offset;     /* offset in register   */
    GT_STATUS   rc;         /* function return code */
    GT_U32      portGroupId;/* port group Id        */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(macModePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            offset = 25;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            offset = 24;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            offset = 26;
            break;
        default:
            if ((keyType == CPSS_DXCH_TTI_KEY_MIM_E) && (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                return GT_NOT_SUPPORTED;
            else
                return GT_BAD_PARAM;
        }

        /* read value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum,portGroupId,regAddr,offset,1,&value);
    }
    else /* xCat; Lion; xCat2 */
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            offset = 0;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            offset = 1;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            offset = 2;
            break;
        case CPSS_DXCH_TTI_KEY_MIM_E:
            offset = 3;
            break;
        default:
            return GT_BAD_PARAM;
        }

        /* read value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum,portGroupId,regAddr,offset,1,&value);
    }

    if (rc != GT_OK)
        return rc;

    switch (value)
    {
    case 0:
        *macModePtr = CPSS_DXCH_TTI_MAC_MODE_DA_E;
        break;
    case 1:
        *macModePtr = CPSS_DXCH_TTI_MAC_MODE_SA_E;
        break;
    default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       keyType           - TTI key type; valid values:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 means don't care bit (corresponding
*                           bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS   rc;                                 /* function return code    */
    GT_U32      hwTtiKeyArray[TTI_RULE_SIZE_CNS];   /* TTI key in hw format    */
    GT_U32      hwTtiMaskArray[TTI_RULE_SIZE_CNS];  /* TTI mask in hw format   */
    GT_U32      *hwTtiActionPtr;                    /* TTI action in hw format */
    GT_U32      portGroupId;                        /* port group Id           */


    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(patternPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,routerTtiTcamRow);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* convert tti configuration to hardware format */
    rc = ttiConfigLogic2HwFormat(devNum,
                                 keyType,
                                 GT_FALSE,
                                 patternPtr,
                                 hwTtiKeyArray);
    if (rc != GT_OK)
        return rc;

    /* convert tti configuration mask to hardware format */
    rc = ttiConfigLogic2HwFormat(devNum,
                                 keyType,
                                 GT_TRUE,
                                 maskPtr,
                                 hwTtiMaskArray);
    if (rc != GT_OK)
        return rc;

    /* allocate memory for TTI action hw format array */

    if (actionType == CPSS_DXCH_TTI_ACTION_TYPE1_ENT)
        hwTtiActionPtr = cpssOsMalloc(TTI_ACTION_SIZE_CNS * sizeof(GT_U32));
    else if (actionType == CPSS_DXCH_TTI_ACTION_TYPE2_ENT)
        hwTtiActionPtr = cpssOsMalloc(TTI_ACTION_TYPE_2_SIZE_CNS * sizeof(GT_U32));
    else
        return GT_BAD_PARAM;

    if (hwTtiActionPtr == NULL)
        return GT_OUT_OF_CPU_MEM;

    /* convert tti action to hardware format */
    rc = ttiActionLogic2HwFormat(devNum,
                                 actionType,
                                 actionPtr,
                                 hwTtiActionPtr);
    if (rc != GT_OK)
    {
        cpssOsFree(hwTtiActionPtr);
        return rc;
    }


    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* now write entry to the router / tunnel termination TCAM */
        rc = prvCpssDxChRouterTunnelTermTcamSetLine(
            devNum,portGroupId,routerTtiTcamRow,hwTtiKeyArray,
            hwTtiMaskArray,hwTtiActionPtr);

        if (rc != GT_OK)
        {
            cpssOsFree(hwTtiActionPtr);
            return rc;
        }
    }
     PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
           devNum, portGroupsBmp, portGroupId)

    cpssOsFree(hwTtiActionPtr);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the rule in the TCAM
*       keyType           - TTI key type; valid values:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 means don't care bit (corresponding
*                           bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{

    GT_STATUS   rc;                                                           /* function return code    */
    GT_U32      *hwTtiActionPtr;                                              /* TTI action in hw format */
    GT_U32      validArr[5];                                                  /* TCAM line valid bits    */
    GT_U32      compareModeArr[5];                                            /* TCAM line compare mode  */
    GT_U32      portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;                                                  /* port group Id           */
    GT_U32      hwTtiKeyArray[TTI_RULE_SIZE_CNS];                             /*  TTI key in hw format  */
    GT_U32      hwTtiMaskArray[TTI_RULE_SIZE_CNS];                            /*  TTI mask in hw format */
    /* check pararmeters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(patternPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,routerTtiTcamRow);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

     /* zero out hw format */
    cpssOsMemSet(hwTtiKeyArray,0,sizeof(hwTtiKeyArray));
    cpssOsMemSet(hwTtiMaskArray,0,sizeof(hwTtiMaskArray));

    /* allo cate memory for TTI action hw format array */
    if (actionType == CPSS_DXCH_TTI_ACTION_TYPE1_ENT)
        hwTtiActionPtr = cpssOsMalloc(TTI_ACTION_SIZE_CNS * sizeof(GT_U32));
    else if (actionType == CPSS_DXCH_TTI_ACTION_TYPE2_ENT)
        hwTtiActionPtr = cpssOsMalloc(TTI_ACTION_TYPE_2_SIZE_CNS * sizeof(GT_U32));
    else
        return GT_BAD_PARAM;

    if (hwTtiActionPtr == NULL)
        return GT_OUT_OF_CPU_MEM;
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {

        rc = prvCpssDxChRouterTunnelTermTcamMultiPortGroupsGetLine(devNum,
                                                                   portGroupsBmp,
                                                                   routerTtiTcamRow,
                                                                   &validArr[0],
                                                                   &compareModeArr[0],
                                                                   hwTtiKeyArray,
                                                                   hwTtiMaskArray,
                                                                   hwTtiActionPtr);
    }
    else
    {
        /* read tunnel termination configuration and action from hardware */
        rc = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    portGroupId,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    hwTtiKeyArray,
                                                    hwTtiMaskArray,
                                                    hwTtiActionPtr);
    }
    if (rc != GT_OK)
    {
        cpssOsFree(hwTtiActionPtr);
        return rc;
    }

    /* convert tti configuration from hw format to logic format */
    rc = ttiConfigHw2LogicFormat(devNum,keyType,hwTtiKeyArray,patternPtr);
    if (rc != GT_OK)
    {
        cpssOsFree(hwTtiActionPtr);
        return rc;
    }

    /* convert tti configuration mask from hw format to logic format */
    rc = ttiConfigHw2LogicFormat(devNum,keyType,hwTtiMaskArray,maskPtr);
    if (rc != GT_OK)
    {
        cpssOsFree(hwTtiActionPtr);
        return rc;
    }

    /* convert tti action from hw format to logic format */
    rc = ttiActionHw2LogicFormat(devNum,hwTtiActionPtr,actionType,actionPtr);

    cpssOsFree(hwTtiActionPtr);
    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow - Index of the rule in the TCAM
*       actionType       - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS   rc;                 /* return status                  */
    GT_U32      *hwTtiActionPtr;    /* TTI action (rule) in hw format */
    GT_U32      portGroupId;        /* port group Id                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,routerTtiTcamRow);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* allocate memory for TTI action hw format array */
    if (actionType == CPSS_DXCH_TTI_ACTION_TYPE1_ENT)
        hwTtiActionPtr = cpssOsMalloc(TTI_ACTION_SIZE_CNS * sizeof(GT_U32));
    else if (actionType == CPSS_DXCH_TTI_ACTION_TYPE2_ENT)
        hwTtiActionPtr = cpssOsMalloc(TTI_ACTION_TYPE_2_SIZE_CNS * sizeof(GT_U32));
    else
        return GT_BAD_PARAM;

    if (hwTtiActionPtr == NULL)
        return GT_OUT_OF_CPU_MEM;

    /* convert tti action to hardware format */
    rc = ttiActionLogic2HwFormat(devNum,actionType,actionPtr,hwTtiActionPtr);
    if (rc != GT_OK)
    {
        cpssOsFree(hwTtiActionPtr);
        return rc;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc =  prvCpssDxChPortGroupWriteTableEntry(
            devNum,portGroupId,PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
            routerTtiTcamRow,hwTtiActionPtr);

        if (rc != GT_OK)
        {
            cpssOsFree(hwTtiActionPtr);
            return rc;
        }
    }
     PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
           devNum, portGroupsBmp, portGroupId)

     cpssOsFree(hwTtiActionPtr);

     return GT_OK;

}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  GT_BOOL                             valid
)
{
    GT_U32      keyArr[6];          /* TCAM key in hw format            */
    GT_U32      maskArr[6];         /* TCAM mask in hw format           */
    GT_U32      actionArr[6];       /* TCAM action (rule) in hw format  */
    GT_U32      validArr[5];        /* TCAM line valid bits             */
    GT_U32      compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32      portGroupId;        /* port group Id                    */

    GT_STATUS   rc = GT_OK;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,routerTtiTcamRow);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if (valid == GT_TRUE)
    {
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            /* read the current line from TCAM */
            rc = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                        portGroupId,
                                                        routerTtiTcamRow,
                                                        validArr,       /* dummy */
                                                        compareModeArr, /* dummy */
                                                        keyArr,
                                                        maskArr,
                                                        actionArr);
            if (rc != GT_OK)
                return rc;

            /* write the line (note it also set the line as valid) */
            rc = prvCpssDxChRouterTunnelTermTcamSetLine(devNum,
                                                        portGroupId,
                                                        routerTtiTcamRow,
                                                        keyArr,
                                                        maskArr,
                                                        actionArr);
            if (rc != GT_OK)
                return rc;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
           devNum, portGroupsBmp, portGroupId)
    }
    else /* (valid == GT_FALSE) */
    {
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            /* just invalidate the line */
            rc = prvCpssDxChRouterTunnelTermTcamInvalidateLine(
                devNum,portGroupId,routerTtiTcamRow);

            if (rc != GT_OK)
                return rc;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    OUT GT_BOOL                             *validPtr
)
{
    GT_U32      keyArr[6];          /* TCAM key in hw format            */
    GT_U32      maskArr[6];         /* TCAM mask in hw format           */
    GT_U32      actionArr[6];       /* TCAM action (rule) in hw format  */
    GT_U32      validArr[5];        /* TCAM line valid bits             */
    GT_U32      compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32      portGroupId;        /* port group Id                    */
    GT_STATUS   rc;                 /* function return code             */
    GT_U32      i;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,routerTtiTcamRow);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    rc = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                portGroupId,
                                                routerTtiTcamRow,
                                                validArr,
                                                compareModeArr,
                                                keyArr,
                                                maskArr,
                                                actionArr);
    if (rc != GT_OK)
        return rc;

    /* line holds valid TTI entry if the following applies:
         - all entries are valid
         - the compare mode or all entries is row compare
         - keyArr[5] bit 31 must be 1 (to indicate TTI entry and not IPv6 entry) */
    *validPtr = GT_TRUE;
    for (i = 0 ; i < 4; i++)
    {
        /* if entry is not valid or is single compare mode, whole line is not valid */
        if ((validArr[i] == 0) || (compareModeArr[i] == 0))
        {
            *validPtr = GT_FALSE;
            break;
        }
    }
    /* if whole line is valid, verify it is indeed TTI entry and not IPv6 entry */
    if ((*validPtr == GT_TRUE) && (((keyArr[5] >> 31) & 0x1) != 1))
    {
        *validPtr = GT_FALSE;
    }

    return GT_OK;
}
