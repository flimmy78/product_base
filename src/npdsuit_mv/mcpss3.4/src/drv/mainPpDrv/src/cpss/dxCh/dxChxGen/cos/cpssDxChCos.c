/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCos.c
*
* DESCRIPTION:
*       CPSS DXCh Cos facility implementation.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>

/*******************************************************************************
* Local Macros
*******************************************************************************/

/* check the MAC QoS Table index range */
#define PRV_CPSS_MAX_MAC_QOS_ENTRIES_CHECK_MAC(_index) \
        if (((_index) > 7) || ((_index) == 0)) return GT_BAD_PARAM

/* convert modify attribute from SW enum to HW value */
#define PRV_CPSS_ATTR_MODIFY_TO_HW_CNV_MAC(_hwValue, _swValue)    \
    switch(_swValue)                                                           \
    {                                                                          \
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E: _hwValue = 0; break;\
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:        _hwValue = 1; break;\
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:       _hwValue = 2; break;\
        default: return GT_BAD_PARAM;                                          \
    }

/*******************************************************************************
* cpssDxChCosProfileEntrySet
*
* DESCRIPTION:
*       Configures the Profile Table Entry and
*       Initial Drop Precedence (DP) for Policer.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number.
*       profileIndex  - index of a profile in the profile table
*                       Ch values (0..71); Ch2 and above values (0..127).
*       cosPtr        - Pointer to new CoS values for packet
*                       (dp = CPSS_DP_YELLOW_E is not applicable).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or cos.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 devices Initial DP supports 3 levels (Yellow, Red, Green),
*       but QoS profile entry supports only 2 levels by the following way:
*        - The function set value 0 for Green DP
*        - The function set value 1 for both Yellow and Red DPs.
*
*******************************************************************************/
GT_STATUS cpssDxChCosProfileEntrySet
(
     IN  GT_U8                      devNum,
     IN  GT_U32                     profileIndex,
     IN  CPSS_DXCH_COS_PROFILE_STC  *cosPtr
)
{
    GT_U32      tc;         /* Traffic Class of the Profile */
    GT_U32      up;         /* User Priority of the Profile */
    GT_U32      dp;         /* Drop Precedence of the Profile */
    GT_U32      dscp;       /* dscp of the profile */
    GT_U32      regData;    /* the data to be written/read to/from register */
    GT_U32      regAddr;    /* register address */
    GT_U32      fieldOffset;/* offset of the field in the register */
    GT_U32      regOffset;  /* the offset of the register */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, profileIndex);
    CPSS_NULL_PTR_CHECK_MAC(cosPtr);
    PRV_CPSS_DXCH_COS_CHECK_TX_QUEUES_NUM_MAC(devNum, cosPtr->trafficClass);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(cosPtr->userPriority);
    PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(cosPtr->dscp);

    tc = cosPtr->trafficClass;
    up = cosPtr->userPriority;
    dscp = cosPtr->dscp;

    if (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if (0 == PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum)) 
        {
            PRV_CPSS_DXCH_COS_CHECK_DP_MAC(cosPtr->dropPrecedence);
        }
        else
        {
            /* DxCh3 */
            PRV_CPSS_DXCH3_COS_CHECK_DP_MAC(cosPtr->dropPrecedence);
        }
        dp = (cosPtr->dropPrecedence == CPSS_DP_GREEN_E) ? 0 : 1;
    }
    else
    {
        /* LION */
        PRV_CPSS_DXCH3_COS_DP_CONVERT_MAC(cosPtr->dropPrecedence, dp);
    }

    regData = (tc << 11) | (dp << 9) | (up << 6) | dscp;

    /* writing to the entry to QoS Profile Register */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
                                         profileIndex,
                                         0, /*word num */
                                         0, /* start bit */
                                         14, /* length */
                                         regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (0 != PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        /* updating <Dp> in Initial DP table, for Ingress stage #0 */
        PRV_CPSS_DXCH3_COS_DP_CONVERT_MAC(cosPtr->dropPrecedence, dp);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerInitialDPReg;
        regOffset = (profileIndex / 16) * 4;
        regAddr += regOffset;
        fieldOffset = (profileIndex % 16) * 2;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 2, dp);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* check if Ingress stage 1, exists */
        if (PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.iplrSecondStageSupported)
        {
            /* updating <Dp> in Initial DP table, for Ingress stage #1 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[1].policerInitialDPReg;
            regAddr += regOffset;
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 2, dp);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* check if the device is xCat, lion or xCat2 */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.qosProfile2DpTable;

            /* adding the offset of the register                    */
            /* 0x0F000340 + 4*n: where 0=<n<=7 */
            /* each register contains 16 entries of 2 bits each      */
            regAddr += regOffset;

            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 2, dp);
        }
    }
    else
    {
        /* updating <Dp> in RemarkTable */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_REMARKING_E,
                                             profileIndex,
                                             0,
                                             7,
                                             1,
                                             dp);
    }

    if (rc != GT_OK)
    {
        return rc;
    }


    /* write the exp to the QoSProfile to EXP table - DxCh2 and above */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(cosPtr->exp);

        /* get register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.qosProfile2ExpTable;

        /* adding the offset of the register                    */
        /* Offset Formula: 0x07800300 + Entry<n>*0x4 (0<=n<16)  */
        /* each register contains 8 entries of 3 bits each      */
        regAddr +=  (profileIndex / 8) * 4;

        fieldOffset = (profileIndex % 8) * 3;

        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,fieldOffset,3,cosPtr->exp);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChCosProfileEntryGet
*
* DESCRIPTION:
*       Get Profile Table Entry configuration and
*       initial Drop Precedence (DP) for Policer.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number.
*       profileIndex  - index of a profile in the profile table
*                       Ch values (0..71); Ch2 and above values (0..127).
*
* OUTPUTS:
*       cosPtr        - Pointer to new CoS values for packet.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or cos.
*       GT_TIMEOUT      - after max number of retries checking if PP ready.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 devices Initial DP supports 3 levels (Yellow, Red, Green),
*       but QoS profile entry supports only 2 levels.
*       Therefore DP value is taken from the Initial DP table.
*
*******************************************************************************/
GT_STATUS cpssDxChCosProfileEntryGet
(
     IN  GT_U8                      devNum,
     IN  GT_U32                     profileIndex,
     OUT CPSS_DXCH_COS_PROFILE_STC  *cosPtr
)
{
    GT_U32      dp;         /* Drop Precedence of the Profile */
    GT_U32      regData;    /* the data to be written/read to/from register */
    GT_U32      regOffset;  /* offset in the register to read data from */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, profileIndex);
    CPSS_NULL_PTR_CHECK_MAC(cosPtr);

    /* reading the entry to QoS Profile Register */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
                                        profileIndex,
                                        0, /*word num */
                                        0, /* start bit */
                                        14, /* length */
                                        &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    cosPtr->trafficClass = (regData >> 11) & 0x7; /* 3 bits */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
        CPSS_PP_FAMILY_CHEETAH3_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerInitialDPReg;
        regAddr +=  (profileIndex / 16) * 4;
        regOffset = (profileIndex % 16) * 2;
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, regOffset, 2, &dp);
        if (rc != GT_OK)
        {
            return rc;
        }

        PRV_CPSS_DXCH3_COS_DP_TO_SW_CONVERT_MAC(dp, cosPtr->dropPrecedence);
    }
    else
    {
        dp = (regData >> 9) & 0x1;                    /* 1 bits */
        cosPtr->dropPrecedence = (dp==0) ? CPSS_DP_GREEN_E : CPSS_DP_RED_E;
    }

    cosPtr->userPriority = (regData >> 6) & 0x7;  /* 3 bits */
    cosPtr->dscp = regData & 0x3F;                /* 6 bits */

    /* read the exp from the QoSProfile to EXP table  - DxCh2 and above */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* get register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.qosProfile2ExpTable;

        /* adding the offset of the register                    */
        /* Offset Formula: 0x07800300 + Entry<n>*0x4 (0<=n<16)  */
        /* each register contains 8 entries of 3 bits each      */
        regAddr +=  (profileIndex / 8) * 4;

        regOffset = (profileIndex % 8) * 3;

        rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,regOffset,3,&cosPtr->exp);
    }
    else
        cosPtr->exp = 0;

    return rc;
}

/*******************************************************************************
* cpssDxChCosDscpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet DSCP (or remapped DSCP) to a QoS Profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number.
*       dscp          - DSCP of a IP packet [0..63].
*       profileIndex  - profile index, which is assigned to a IP packet with
*                       the DSCP on ports with enabled trust L3 or trust L2-L3.
*                       Ch values (0..71); Ch2(and above) values (0..127).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, dscp or profileIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosDscpToProfileMapSet
(
    IN GT_U8    devNum,
    IN GT_U8    dscp,
    IN GT_U32   profileIndex
)
{

    GT_U32      bitOffset;          /* offset in the register */
    GT_U32      regAddr;            /* register address */
    GT_STATUS   rc;                 /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, profileIndex);
    PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(dscp);


    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable;
    /* adding the offset of the register
       DSCP<4n...4n+3>2QoSProfile Map Table (0<=n<16),
       (16 registers in steps of 0x4) */

    regAddr +=  (dscp / 4) * 4;

    bitOffset = (dscp % 4) * 8;

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 7, profileIndex);

    return rc;
}

/*******************************************************************************
* cpssDxChCosDscpToProfileMapGet
*
* DESCRIPTION:
*       Get the Mapping: packet DSCP (or remapped DSCP) to a QoS Profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number.
*       dscp          - DSCP of a IP packet [0..63]
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index, which is assigned to
*                         an IP packet with the DSCP on ports with enabled
*                         trust L3 or trust L2-L3.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, dscp or profileIndex.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosDscpToProfileMapGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    dscp,
    OUT GT_U32  *profileIndexPtr
)
{

    GT_U32      bitOffset;          /* offset in the register */
    GT_U32      hwValue;            /* HW value of field in the register*/
    GT_U32      regAddr;            /* register address */
    GT_STATUS   rc;                 /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(dscp);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable;
    /* adding the offset of the register
       DSCP<4n...4n+3>2QoSProfile Map Table (0<=n<16),
       (16 registers in steps of 0x4) */

    regAddr +=  (dscp / 4) * 4;

    bitOffset = (dscp % 4) * 8;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 7, &hwValue);

    *profileIndexPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChCosDscpMutationEntrySet
*
* DESCRIPTION:
*       Maps the packet DSCP to a new, mutated DSCP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number.
*       dscp     - DSCP of a IP packet [0..63]
*       newDscp  - new DSCP, which is assigned to a IP packet with the DSCP on
*          ports with enabled trust L3 or trust L2-L3 and enabled DSCP mutation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, dscp or newDscp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosDscpMutationEntrySet
(
    IN GT_U8   devNum,
    IN GT_U8   dscp,
    IN GT_U8   newDscp
)
{
    GT_U32      bitOffset;      /* offset in the register */
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(dscp);
    PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(newDscp);

    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.dscp2DscpMapTable;
    /* adding the offset of the register
       DSCP<4n...4n+3>2QoSProfile Map Table (0<=n<16),
       (16 registers in steps of 0x4) */

    regAddr +=  (dscp / 4) * 4;

    bitOffset = (dscp % 4) * 8;

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 6, newDscp);

    return rc;

}

/*******************************************************************************
* cpssDxChCosDscpMutationEntryGet
*
* DESCRIPTION:
*       Get the Mapping: packet DSCP to a new, mutated DSCP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number.
*       dscp     - DSCP of a IP packet [0..63]
*
* OUTPUTS:
*       newDscpPtr  - new DSCP, which is assigned to a IP packet with the DSCP on
*          ports with enabled trust L3 or trust L2-L3 and enabled DSCP mutation.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, dscp or newDscp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosDscpMutationEntryGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   dscp,
    OUT GT_U8  *newDscpPtr
)
{
    GT_U32      bitOffset;      /* offset in the register */
    GT_U32      regAddr;        /* register address */
    GT_U32      hwValue;        /* HW value of field in the register*/
    GT_STATUS   rc;             /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(dscp);
    CPSS_NULL_PTR_CHECK_MAC(newDscpPtr);

    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.dscp2DscpMapTable;
    /* adding the offset of the register
       DSCP<4n...4n+3>2QoSProfile Map Table (0<=n<16),
       (16 registers in steps of 0x4) */

    regAddr +=  (dscp / 4) * 4;

    bitOffset = (dscp % 4) * 8;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 6, &hwValue);

    *newDscpPtr = (GT_U8)hwValue;

    return rc;

}

/*******************************************************************************
* cpssDxChCosUpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet User Priority to a QoS Profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       up              - user priority of a VLAN or Marvell tagged
*                         packet [0..7].
*       profileIndex    - profile index, which is assigned to a VLAN or
*                         Marvell tagged packet with the up on ports with
*                         enabled trust L2 or trust L2-L3.
*                         Ch values (0..71); Ch2 values (0..127).
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, up or profileIndex.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosUpToProfileMapSet
(
    IN GT_U8  devNum,
    IN GT_U8  up,
    IN GT_U32 profileIndex
)
{
    GT_U32      bitOffset;          /* offset in the register */
    GT_U32      regAddr;            /* register address */
    GT_STATUS   rc;                 /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, profileIndex);


    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.up2QosProfileMapTable;

    /* adding the offset of the register
       UP<4n...4n+3>2QoSProfile Map Table (0<=n<2),
       (2 registers in steps of 0x4) */

    regAddr +=  (up / 4) * 4;

    bitOffset = (up % 4) * 8;

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 7, profileIndex);
    return rc;
}


/*******************************************************************************
* cpssDxChCosUpToProfileMapGet
*
* DESCRIPTION:
*       Returns the mapping of packet User Priority to a QoS Profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       up              - user priority of a VLAN or Marvell tagged
*                         packet [0..7].
*
* OUTPUTS:
*       profileIndexPtr - (pointer to)profile index, which is assigned to
*                         a VLAN or Marvell tagged packet with the up on
*                         ports with enabled trust L2 or trust L2-L3.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, up.
*       GT_BAD_PTR               - NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosUpToProfileMapGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   up,
    OUT GT_U32 *profileIndexPtr
)
{
    GT_U32      bitOffset;          /* offset in the register */
    GT_U32      hwValue;            /* HW value of field in the register*/
    GT_U32      regAddr;            /* register address */
    GT_STATUS   rc;                 /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);


    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.up2QosProfileMapTable;

    /* adding the offset of the register
       UP<4n...4n+3>2QoSProfile Map Table (0<=n<2),
       (2 registers in steps of 0x4) */

    regAddr +=  (up / 4) * 4;

    bitOffset = (up % 4) * 8;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 7, &hwValue);

    *profileIndexPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChCosUpCfiDeiToProfileMapSet
*
* DESCRIPTION:
*       Maps the UP Profile Index and packet's User Priority and CFI/DEI bit (Canonical Format
*       Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number.
*       upProfileIndex - the UP profile index (table selector)
*                        this parameter not relevant for ch1,2,3,xcat.
*                       see also API : cpssDxChCosPortUpProfileIndexSet
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*                         this parameter not relevant for ch1,2.
*       profileIndex    - QoS Profile index, which is assigned to a VLAN or DSA
*                         tagged packet with the UP on ports with enabled trust
*                         L2 or trust L2-L3. Range is [0..127].(for ch1 range is [0..71])
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum, up or cfiDeiBit.
*       GT_OUT_OF_RANGE - on profileIndex out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChCosUpCfiDeiToProfileMapSet
(
    IN GT_U8    devNum,
    IN GT_U32   upProfileIndex,
    IN GT_U8    up,
    IN GT_U8    cfiDeiBit,
    IN GT_U32   profileIndex
)
{
    GT_U32      bitOffset;          /* offset in the register */
    GT_U32      regAddr;            /* register address */
    GT_U32      cfiStep;        /* cfi parameter step in address */
    GT_U32      upProfileStep;  /* upProfileIndex parameter step in address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);

    /* check profileIndex bit */
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        PRV_CPSS_DXCH_CHECK_KEY_UP_PROFILE_INDEX_MAC(upProfileIndex);
    }
    else
    {
        upProfileIndex = 0;/* set parameter so next code will be generic */
    }

    /* Check the QoS profile Index range */
    if(profileIndex >= (GT_U32)PRV_CPSS_DXCH_QOS_PROFILE_MAX_MAC(devNum))
    {
        return GT_OUT_OF_RANGE;
    }

    /* check CFI bit */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        PRV_CPSS_DXCH3_CHECK_CFI_DEI_BIT_MAC(cfiDeiBit);
    }
    else
    {
        cfiDeiBit = 0;/* set parameter so next code will be generic */
    }

    /* Get the base address of UP2QosProfileMapTable register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.qosProfileRegs.up2QosProfileMapTable;

    /* Calculate the actual register address according to the CFI bit value */
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        cfiStep = 0x8;
        upProfileStep = 0x10;
    }
    else
    {
        cfiStep = 0x10;
        upProfileStep = 0; /* set parameter so next code will be generic */
    }

    /* consider the cfi parameter */
    regAddr += (cfiDeiBit * cfiStep);
    /* consider the up profile index parameter */
    regAddr += (upProfileIndex * upProfileStep);

    /* Add the UP<4n...4n+3>2QoSProfile Map Table(0<=n<2) register offset,
       2 registers in steps of 0x4 per CFI/DEI bit value
    */
    regAddr +=  (up / 4) * 4;
    bitOffset = (up % 4) * 8;

    /* Set the QoS Profile index */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 7, profileIndex);
}

/*******************************************************************************
* cpssDxChCosUpCfiDeiToProfileMapGet
*
* DESCRIPTION:
*       Get the Map from UP Profile Index and packet's User Priority and CFI/DEI bit (Canonical Format
*       Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number.
*       upProfileIndex - the UP profile index (table selector)
*                        this parameter not relevant for ch1,2,3,xcat.
*                       see also API : cpssDxChCosPortUpProfileIndexSet
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*                         this parameter not relevant for ch1,2.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to)QoS Profile index, which is assigned to a
*                         VLAN or DSA tagged packet with the UP on ports with
*                         enabled trust L2 or trust L2-L3. Range is [0..127]. (for ch1 range is [0..71])
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum, up or cfiDeiBit.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChCosUpCfiDeiToProfileMapGet
(
    IN  GT_U8   devNum,
    IN GT_U32   upProfileIndex,
    IN  GT_U8   up,
    IN  GT_U8   cfiDeiBit,
    OUT GT_U32 *profileIndexPtr
)
{
    GT_STATUS   rc ;            /* return value */
    GT_U32      bitOffset;      /* offset in the register */
    GT_U32      regAddr;        /* register address */
    GT_U32      cfiStep;        /* cfi parameter step in address */
    GT_U32      upProfileStep;  /* upProfileIndex parameter step in address */
    GT_U32      hwValue;        /* hw value read from register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    /* check profileIndex bit */
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        PRV_CPSS_DXCH_CHECK_KEY_UP_PROFILE_INDEX_MAC(upProfileIndex);
    }
    else
    {
        upProfileIndex = 0;/* set parameter so next code will be generic */
    }

    /* check CFI bit */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        PRV_CPSS_DXCH3_CHECK_CFI_DEI_BIT_MAC(cfiDeiBit);
    }
    else
    {
        cfiDeiBit = 0;/* set parameter so next code will be generic */
    }

    /* Get the base address of UP2QosProfileMapTable register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.qosProfileRegs.up2QosProfileMapTable;

    /* Calculate the actual register address according to the CFI bit value */
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        cfiStep = 0x8;
        upProfileStep = 0x10;
    }
    else
    {
        cfiStep = 0x10;
        upProfileStep = 0; /* set parameter so next code will be generic */
    }

    /* consider the cfi parameter */
    regAddr += (cfiDeiBit * cfiStep);
    /* consider the up profile index parameter */
    regAddr += (upProfileIndex * upProfileStep);


    /* Add the UP<4n...4n+3>2QoSProfile Map Table(0<=n<2) register offset,
       2 registers in steps of 0x4 per CFI/DEI bit value
    */
    regAddr +=  (up / 4) * 4;
    bitOffset = (up % 4) * 8;

    /* Get the QoS Profile index */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 7, &hwValue);
    *profileIndexPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChCosPortDpToCfiDeiMapEnableSet
*
* DESCRIPTION:
*       Enables or disables mapping of Drop Precedence to Drop Eligibility
*       Indicator bit.
*       When enabled on egress port the DEI(CFI) bit at the outgoing VLAN
*       tag is set according to the packet DP(after mapping).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       port            - egress port number including CPU port.
*       enable          - enable/disable DP-to-CFI/DEI mapping on the
*                         egress port:
*                             GT_TRUE  - DP-to-CFI/DEI mapping enable on port.
*                             GT_FALSE - DP-to-CFI/DEI mapping disable on port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChCosPortDpToCfiDeiMapEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;  /* the register address */
    GT_U32      regData;  /* data to be written to the register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    regData = (enable == GT_TRUE) ? 1 : 0;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

        /* Get the address of DP2CFI Enable Per Port register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            egrTxQConf.txQueueDpToCfiPerPortEnReg;

        /* Remap CPU Port Number to the offset in the register */
        if(CPSS_CPU_PORT_NUM_CNS == localPort)
        {
            localPort = 31;
        }

        /* Enable/Disables DP mapping to CFI/DEI value for current port */
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, localPort, 1, regData);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                distributor.dpToCfiRemapEn[OFFSET_TO_WORD_MAC(port)];

        return prvCpssDxChHwPpSetRegField(devNum, regAddr,
                OFFSET_TO_BIT_MAC(port) , 1, regData);
    }
}

/*******************************************************************************
* cpssDxChCosPortDpToCfiDeiMapEnableGet
*
* DESCRIPTION:
*       Gets status (Enabled/Disabled) of Drop Precedence mapping to
*       Drop Eligibility Indicator bit.
*       When enabled on egress port the DEI(CFI) bit at the outgoing VLAN
*       tag is set according to the packet DP(after mapping).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       port            - egress port number including CPU port.
*
* OUTPUTS:
*       enablePtr       - (pointer to) value of DP-to-CFI/DEI mapping on the
*                         egress port (enable/disable):
*                             GT_TRUE  - DP-to-CFI/DEI mapping enable on port.
*                             GT_FALSE - DP-to-CFI/DEI mapping disable on port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChCosPortDpToCfiDeiMapEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      regAddr;  /* the register address */
    GT_U32      hwValue;  /* HW value of field in the register*/
    GT_STATUS   rc;       /* return code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

        /* Get the address of DP2CFI Enable Per Port register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            egrTxQConf.txQueueDpToCfiPerPortEnReg;

        /* Remap CPU Port Number to the offset in the register */
        if(CPSS_CPU_PORT_NUM_CNS == localPort)
        {
            localPort = 31;
        }

        /* Get the DP-to-CFI/DEI mapping value for current port */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, localPort, 1, &hwValue);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                distributor.dpToCfiRemapEn[OFFSET_TO_WORD_MAC(port)];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr,
                OFFSET_TO_BIT_MAC(port) , 1, &hwValue);
    }

    *enablePtr = (hwValue == 1) ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChCosDpToCfiDeiMapSet
*
* DESCRIPTION:
*       Maps Drop Precedence to Drop Eligibility Indicator bit.
*       Ports that support S-Tags can be enabled to map the packet's DP
*       (derived from QoS Profile assignment) to the DEI bit(appeared
*       in the outgoing S-Tag) by cpssDxChCosPortDpToCfiDeiMapEnableSet().
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       dp              - Drop Precedence [Green, Red].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or dp.
*       GT_OUT_OF_RANGE - on cfiDeiBit out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Three DP levels are mapped to only two values of CFI/DEI bit.
*
*******************************************************************************/
GT_STATUS cpssDxChCosDpToCfiDeiMapSet
(
    IN GT_U8                devNum,
    IN CPSS_DP_LEVEL_ENT    dp,
    IN GT_U8                cfiDeiBit
)
{
    GT_U32      bitOffset;  /* offset in the register */
    GT_U32      regAddr;    /* register address       */
    GT_U32      hwValue;    /* HW value               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, dp, hwValue);

    if(cfiDeiBit > BIT_0)
    {
        return GT_OUT_OF_RANGE;
    }

    if (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        bitOffset = hwValue;

        /* Get the address of DP2CFI register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            egrTxQConf.txQueueDpToCfiReg;
    }
    else
    {
        bitOffset = (7 + hwValue);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.distributor.generalConfig;
    }

    /* Set the CFI/DEI bit */
    return prvCpssDxChHwPpSetRegField(
        devNum, regAddr, bitOffset, 1, (GT_U32)cfiDeiBit);
}

/*******************************************************************************
* cpssDxChCosDpToCfiDeiMapGet
*
* DESCRIPTION:
*       Gets Drop Precedence mapping to Drop Eligibility Indicator bit.
*       Ports that support S-Tags can be enabled to map the packet's DP
*       (derived from QoS Profile assignment) to the DEI bit(appeared
*       in the outgoing S-Tag) by cpssDxChCosPortDpToCfiDeiMapEnableSet().
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       dp              - Drop Precedence [Green, Red].
*
* OUTPUTS:
*       cfiDeiBitPtr    - (pointer to) value of CFI/DEI bit[0..1].
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or dp.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Three DP levels are mapped to only two values of CFI/DEI bit.
*
*******************************************************************************/
GT_STATUS cpssDxChCosDpToCfiDeiMapGet
(
    IN  GT_U8               devNum,
    IN  CPSS_DP_LEVEL_ENT   dp,
    OUT GT_U8               *cfiDeiBitPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      hwValue;    /* HW value of field in the register*/
    GT_U32      bitOffset;  /* offset in the register */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, dp, hwValue);
    CPSS_NULL_PTR_CHECK_MAC(cfiDeiBitPtr);

    if (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        bitOffset = hwValue;

        /* Get the address of DP2CFI register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            egrTxQConf.txQueueDpToCfiReg;
    }
    else
    {
        bitOffset = (7 + hwValue);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.distributor.generalConfig;
    }

    /* Get the HW value of CFI/DEI bit */
    rc = prvCpssDxChHwPpGetRegField(
        devNum, regAddr, bitOffset, 1, &hwValue);

    *cfiDeiBitPtr = (GT_U8)hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChCosExpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet MPLS exp to a QoS Profile.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - device number.
*       exp             - mpls exp [0..7].
*       profileIndex    - profile index, which is assigned to a VLAN or
*                         Marvell tagged packet with the up on ports with
*                         enabled trust L2 or trust L2-L3.
*                         Ch values (0..71); Ch2(and above) values (0..127).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, exp or profileIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosExpToProfileMapSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  exp,
    IN  GT_U32  profileIndex
)
{
    GT_U32      bitOffset;          /* offset in the register */
    GT_U32      regAddr;            /* register address */
    GT_STATUS   rc;                 /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(exp);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum,profileIndex);

    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable;

    /* adding the offset of the register                */
    /* EXP<4n...4n+3>2QoSProfileEntry*0x4 (0<=n<1)      */
    /* each register contains 4 entries of 8 bits each  */
    regAddr +=  (exp / 4) * 4;

    bitOffset = (exp % 4) * 8;

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,bitOffset,7,profileIndex);
    return rc;
}

/*******************************************************************************
* cpssDxChCosExpToProfileMapGet
*
* DESCRIPTION:
*       Returns the mapping of packet MPLS exp to a QoS Profile.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - device number.
*       exp             - mpls exp.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to)profile index, which is assigned to
*                         a VLAN or Marvell tagged packet with the up on
*                         ports with enabled trust L2 or trust L2-L3.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, exp or profileIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosExpToProfileMapGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  exp,
    OUT GT_U32  *profileIndexPtr
)
{
    GT_U32      bitOffset;          /* offset in the register */
    GT_U32      regAddr;            /* register address */
    GT_STATUS   rc;                 /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(exp);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable;

    /* adding the offset of the register                */
    /* EXP<4n...4n+3>2QoSProfileEntry*0x4 (0<=n<1)      */
    /* each register contains 4 entries of 8 bits each  */
    regAddr +=  (exp / 4) * 4;

    bitOffset = (exp % 4) * 8;

    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,bitOffset,7,profileIndexPtr);
    return rc;
}

/*******************************************************************************
* cpssDxChCosPortQosConfigSet
*
* DESCRIPTION:
*       Configures the port's QoS attributes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number.
*       port            - logical port number, CPU port.
*       portQosCfgPtr   - Pointer to QoS related configuration of a port.
*                         portQosCfgPtr->qosProfileId -
*                                                   [0..71] for Ch,
*                                                   [0..127] for Ch2 and above;
*                         portQosCfgPtr->enableModifyDscp and
*                         portQosCfgPtr->enableModifyUp
*                         only [CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
*                              CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, port or portQosCfgPtr
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosPortQosConfigSet
(
    IN GT_U8                devNum,
    IN GT_U8                port,
    IN CPSS_QOS_ENTRY_STC  *portQosCfgPtr
)
{
    GT_U8         modifyDscp;   /* 1 bit that specifies if dscp modification
                                is enabled or disabled */
    GT_U8         modifyUp;     /* 1 bit that specifies if up modification
                                is enabled or disabled */
    GT_U8         precedence;   /* port precedence */
    GT_U32        qosProfileId; /* Port QoS Profile index */
    GT_U32        qosWord;      /* entry word which contains the QoS params */
    GT_U32        modifyDscpOffset; /* offset of modifyDscp in qosWord */
    GT_U32        modifyUpOffset;   /* offset of modifyUp in qosWord */
    GT_U32        precedenceOffset; /* offset of port precedence in qosWord */
    GT_U32        profileIdOffset; /* offset of qosProfileId in qosWord */
    GT_U32        regData;          /* data to be written or read to/from register*/
    GT_U32        regMask;          /* Qos data fields mask */
    GT_STATUS     rc;               /* return code            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(portQosCfgPtr);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, portQosCfgPtr->qosProfileId);

    /* getting QoS Configuration fields */
    qosProfileId = portQosCfgPtr->qosProfileId;
    PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(precedence, portQosCfgPtr->assignPrecedence)

    switch(portQosCfgPtr->enableModifyDscp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            modifyDscp = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            modifyDscp = 1;
            break;
        default:
           return GT_BAD_PARAM;
    }

    switch(portQosCfgPtr->enableModifyUp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            modifyUp = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            modifyUp = 1;
            break;
        default:
           return GT_BAD_PARAM;

    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        qosWord          = 1;
        modifyDscpOffset = 5;
        modifyUpOffset   = 4;
        precedenceOffset = 3;
        profileIdOffset  = 12;
        regMask = 0xFFF80FC7; /* mask entry, zero bits: 3,4,5 and 12-18 */
    }
    else
    {
        qosWord          = 0;
        modifyDscpOffset = 8;
        modifyUpOffset   = 7;
        precedenceOffset = 13;
        profileIdOffset  = 25;
        regMask = 0x01FFDE7F; /* mask entry, zero bits: 7,8,13 and 25-31 */
    }

    /* read entry from Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        qosWord,
                                        0,
                                        32,
                                        &regData);

    /* update entry with QoS parameters*/
    regData &= regMask;

    regData |= (modifyUp << modifyUpOffset);

    regData |= (modifyDscp << modifyDscpOffset);

    regData |= (precedence << precedenceOffset);

    regData |= (qosProfileId << profileIdOffset);

    /* write entry to Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         qosWord,
                                         0,
                                         32,
                                         regData);


    return rc;
}

/*******************************************************************************
* cpssDxChCosPortQosConfigGet
*
* DESCRIPTION:
*       Get the port's QoS attributes configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number.
*       port            - logical port number, CPU port.
*
* OUTPUTS:
*       portQosCfgPtr   - Pointer to QoS related configuration of a port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, port or portQosCfgPtr
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosPortQosConfigGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                port,
    OUT CPSS_QOS_ENTRY_STC  *portQosCfgPtr

)
{
    GT_U32        modifyDscp;   /* 1 bit that specifies if dscp modification
                                is enabled or disabled */
    GT_U32        modifyUp;     /* 1 bit that specifies if up modification
                                is enabled or disabled */
    GT_U32        precedence;   /* port precedence */
    GT_U32        qosWord;      /* entry word which contains the QoS params */
    GT_U32        modifyDscpOffset; /* offset of modifyDscp in qosWord */
    GT_U32        modifyUpOffset;   /* offset of modifyUp in qosWord */
    GT_U32        precedenceOffset; /* offset of port precedence in qosWord */
    GT_U32        profileIdOffset;  /* offset of qos Profile Id in qosWord */
    GT_U32        regData;          /* data to be written or read to/from register*/
    GT_STATUS     rc;               /* return code            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(portQosCfgPtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        qosWord          = 1;
        modifyDscpOffset = 5;
        modifyUpOffset   = 4;
        precedenceOffset = 3;
        profileIdOffset  = 12;
    }
    else
    {
        qosWord          = 0;
        modifyDscpOffset = 8;
        modifyUpOffset   = 7;
        precedenceOffset = 13;
        profileIdOffset  = 25;
    }

    /* read entry from Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        qosWord,
                                        0,
                                        32,
                                        &regData);

    /*  entry with QoS parameters bits: 7,8,13 and 25-31 */
    modifyUp     = (regData >> modifyUpOffset) & 0x1;
    modifyDscp   = (regData >> modifyDscpOffset) & 0x1;
    precedence   = (regData >> precedenceOffset) & 0x1;
    portQosCfgPtr->qosProfileId = (GT_U8 )((regData >> profileIdOffset) & 0x7F);

    /* Getting QoS Configuration fields */
    PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(portQosCfgPtr->assignPrecedence, precedence);

    portQosCfgPtr->enableModifyDscp = (modifyDscp == 1) ?
        CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E : CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;

    portQosCfgPtr->enableModifyUp = (modifyUp == 1) ?
        CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E : CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;

    return rc;
}

/*******************************************************************************
* cpssDxChCosMacQosEntrySet
*
* DESCRIPTION:
*       Set the QoS Attribute of the MAC QoS Table Entr
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number.
*       entryIdx       - Index of a MAC QoS table's entry. Valid values 1 - 7.
*       macQosCfgPtr   - Pointer to QoS related configuration of a MAC QoS table's entry.
*                        macQosCfgPtr->qosProfileId -
*                                                   [0..71] for Ch and
*                                                   [0..127] for Ch2 and above;
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, entryIndex or macQosCfgPtr
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS cpssDxChCosMacQosEntrySet
(
    IN GT_U8               devNum,
    IN GT_U32              entryIdx,
    IN CPSS_QOS_ENTRY_STC *macQosCfgPtr
)
{
    GT_U32      regData;            /* data to be written to register */
    GT_U32      hwValue;            /* HW value of field */
    GT_U32      regAddr;            /* register address */
    GT_STATUS   rc;                 /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MAX_MAC_QOS_ENTRIES_CHECK_MAC(entryIdx);
    CPSS_NULL_PTR_CHECK_MAC(macQosCfgPtr);

    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, macQosCfgPtr->qosProfileId);

    regData = macQosCfgPtr->qosProfileId;

    /* convert Enable Modify UP to HW value */
    PRV_CPSS_ATTR_MODIFY_TO_HW_CNV_MAC(hwValue, macQosCfgPtr->enableModifyUp);
    regData |= (hwValue << 7);

    /* convert Enable Modify DSCP to HW value */
    PRV_CPSS_ATTR_MODIFY_TO_HW_CNV_MAC(hwValue, macQosCfgPtr->enableModifyDscp);
    regData |= (hwValue << 9);

    /* convert MAC QoS Precedence to HW value */
    PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(hwValue, macQosCfgPtr->assignPrecedence);
    regData |= (hwValue << 11);

    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.macQosTable;

    regAddr += (entryIdx - 1) * 4;

    /* write to MAC QoS Register */
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChCosMacQosEntryGet
*
* DESCRIPTION:
*       Returns the QoS Attribute of the MAC QoS Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number.
*       entryIdx       - Index of a MAC QoS table's entry. Valid values 1 - 7.
*
* OUTPUTS:
*       macQosCfgPtr  - Pointer to QoS related configuration of a MAC QoS
*                        table's entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, entryIndex or macQosCfgPtr.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS cpssDxChCosMacQosEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIdx,
    OUT CPSS_QOS_ENTRY_STC *macQosCfgPtr
)
{
    GT_U32      regData;            /* data to be written to register */
    GT_U32      regAddr;            /* register address */
    GT_STATUS   rc;                 /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macQosCfgPtr);
    PRV_CPSS_MAX_MAC_QOS_ENTRIES_CHECK_MAC(entryIdx);

    /* getting the base address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.qosProfileRegs.macQosTable;
    regAddr += (entryIdx - 1)* 4;

    /* Read the MAC QoS Register */
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regData);

    macQosCfgPtr->qosProfileId = regData & 0x7f; /* 7 bits */

    macQosCfgPtr->assignPrecedence =
        (((regData >> 11) & 0x1) == 0) ? CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E :
                                         CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

    switch((regData >>  9) & 0x3) /* 2 bits */
    {
        case 0:
            macQosCfgPtr->enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            macQosCfgPtr->enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            macQosCfgPtr->enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
    }

    switch((regData >>  7) & 0x3) /* 2 bits */
    {
        case 0:
            macQosCfgPtr->enableModifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            macQosCfgPtr->enableModifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            macQosCfgPtr->enableModifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChCosMacQosConflictResolutionSet
*
* DESCRIPTION:
*       Configure QoS Marking Conflict Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       macQosResolvType    - Enum holding two attributes for selecting
*                             the SA command or the DA command
*                             (0 - DA command, 1 - SA caommand).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or macQosResolvType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosMacQosConflictResolutionSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_QOS_RESOLVE_ENT     macQosResolvType
)
{
    GT_U32      regData;    /* data to be written to register */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return code            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (macQosResolvType)
    {
        case CPSS_MAC_QOS_GET_FROM_DA_E:
            regData = 1;
            break;
        case CPSS_MAC_QOS_GET_FROM_SA_E:
            regData = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* write field <MACQosConflictMode> in Bridge Global Register0 */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 22, 1, regData);
    return rc;

}

/*******************************************************************************
* cpssDxChCosMacQosConflictResolutionGet
*
* DESCRIPTION:
*       Get Configured QoS Marking Conflict Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*
* OUTPUTS:
*       macQosResolvTypePtr - Pointer Enum holding two optional attributes for
*                             selecting the SA command or the DA
*                             command (0 - DA command, 1 - SA caommand).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or macQosResolvType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosMacQosConflictResolutionGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_QOS_RESOLVE_ENT    *macQosResolvTypePtr
)
{
    GT_U32      regData;    /* data to be written to register */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return code            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macQosResolvTypePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* Read field <MACQosConflictMode> in Bridge Global Register0 */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 22, 1, &regData);

    *macQosResolvTypePtr =
        (regData == 1) ? CPSS_MAC_QOS_GET_FROM_DA_E : CPSS_MAC_QOS_GET_FROM_SA_E;


    return rc;

}

/*******************************************************************************
* cpssDxChCosRemarkTblEntrySet
*
* DESCRIPTION:
*       Sets new profile index for out profile packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       profileIndex    - profile index in the QoS Profile Table
*                         Ch values (0..71); Ch2(and above) values (0..127).
*       newProfileIndex - new profile index, which is assigned for out-profile
*                         packets according to Traffic Conditioner command.
*                         Ch values (0..71); Ch2(and above) values (0..127).
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, profileIndex or newProfileIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosRemarkTblEntrySet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U32   newProfileIndex
)
{
    GT_STATUS   rc;                 /* return code            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, profileIndex);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, newProfileIndex);


    /* updating data with new profile index */
    /* writing the entry to QoS Remark Register */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_REMARKING_E,
                                         profileIndex,
                                         0,
                                         0,
                                         7,
                                         newProfileIndex);
    return rc;

}

/*******************************************************************************
* cpssDxChCosRemarkTblEntryGet
*
* DESCRIPTION:
*       Get new profile index setting for out profile packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       profileIndex    - profile index in the QoS Profile Table
*                         Ch values (0..71); Ch2(and above) values (0..127).
*
* OUTPUTS:
*       newProfileIndexPtr - (pointer to) new profile index, which will be
*                            assigned for out-profile packets according to
*                            Traffic Conditioner command.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, profileIndex
*       GT_BAD_PTR               - NULL == newProfileIndexPtr
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosRemarkTblEntryGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    OUT GT_U32  *newProfileIndexPtr
)
{
    GT_STATUS   rc;                 /* return code            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, profileIndex);
    CPSS_NULL_PTR_CHECK_MAC(newProfileIndexPtr);


    /* Read the entry to QoS Remark Register */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_TABLE_REMARKING_E,
                                       profileIndex,
                                       0,
                                       0,
                                       7,
                                       newProfileIndexPtr);
    return rc;

}

/*******************************************************************************
* cpssDxChPortDefaultUPSet
*
* DESCRIPTION:
*       Set default user priority (VPT) for untagged packet to a given port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - packet processor device number.
*       port      - logical port number, CPU port.
*       defaultUserPrio  - default user priority (VPT) [0..7].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortDefaultUPSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U8    defaultUserPrio
)
{
    GT_U32      pup;        /* User Priority for port */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;         /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(defaultUserPrio);

    /* update PUP for port */
    pup = defaultUserPrio;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startBit  = 0;
    }
    else
    {
        startBit  = 12;
    }

    /* write data to Ports VLAN and QoS Configuration Table, word1 */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         1,
                                         startBit,
                                         3,
                                         pup);

    return rc;
}

/*******************************************************************************
* cpssDxChPortDefaultUPGet
*
* DESCRIPTION:
*       Get default user priority (VPT) for untagged packet to a given port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - packet processor device number.
*       port      - logical port number, CPU port.
*
* OUTPUTS:
*       defaultUserPrioPtr  - (pointer to) default user priority (VPT).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortDefaultUPGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_U8    *defaultUserPrioPtr
)
{
    GT_U32      pup;        /* User Priority for port */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;         /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(defaultUserPrioPtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startBit  = 0;
    }
    else
    {
        startBit  = 12;
    }

    /* write data to Ports VLAN and QoS Configuration Table, word1 */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         1,
                                         startBit,
                                         3,
                                         &pup);

    /* update PUP for port */
    *defaultUserPrioPtr = (GT_U8)pup;

    return rc;
}

/*******************************************************************************
* cpssDxChPortModifyUPSet
*
* DESCRIPTION:
*       Enable/Disable overriding a tagged packet's User Priority by
*       other assignment mechanisms.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - packet processor device number.
*       port        - logical port, CPU port.
*       upOverrideEnable - GT_TRUE, enable overriding a tagged packet's
*                                   User Priority by other assignment
*                                   mechanisms.
*                          GT_FALSE for disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortModifyUPSet
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL upOverrideEnable
)
{
    GT_U32      regData;
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    regData = (upOverrideEnable == GT_TRUE) ? 1 : 0;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startWord = 1;
        startBit  = 4;
    }
    else
    {
        startWord = 0;
        startBit  = 7;
    }

    /* write entry to Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         startWord,
                                         startBit,
                                         1,
                                         regData);


    return rc;
}


/*******************************************************************************
* cpssDxChPortModifyUPGet
*
* DESCRIPTION:
*       Get Enable/Disable overriding a tagged packet's User Priority by
*       other assignment mechanisms setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - packet processor device number.
*       port        - logical port, CPU port.
*
* OUTPUTS:
*       upOverrideEnablePtr - GT_TRUE, enable overriding a tagged packet's
*                                   User Priority by other assignment
*                                   mechanisms.
*                          GT_FALSE for disable.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortModifyUPGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL *upOverrideEnablePtr
)
{
    GT_U32      regData;
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(upOverrideEnablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startWord = 1;
        startBit  = 4;
    }
    else
    {
        startWord = 0;
        startBit  = 7;
    }

    /* write entry to Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,
                                         startWord,
                                         startBit,
                                         1,
                                         &regData);

    *upOverrideEnablePtr = (regData == 1) ? GT_TRUE : GT_FALSE;

    return rc;
}


/*******************************************************************************
* cpssDxChCosPortQosTrustModeSet
*
* DESCRIPTION:
*       Configures port's Trust Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       port                - logical port number, CPU port.
*       portQosTrustMode    - QoS trust mode of a port.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, port or trustMode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosPortQosTrustModeSet
(
    IN GT_U8                         devNum,
    IN GT_U8                         port,
    IN CPSS_QOS_PORT_TRUST_MODE_ENT  portQosTrustMode
)
{
    GT_U32      regData;   /* the data to be written to rgister */
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_STATUS   rc;        /* return code */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

     /* 2 bits */
    switch(portQosTrustMode)
    {
       case CPSS_QOS_PORT_NO_TRUST_E:
           regData = 0;
           break;
       case CPSS_QOS_PORT_TRUST_L2_E:
           regData = 1;
           break;
       case CPSS_QOS_PORT_TRUST_L3_E:
           regData = 2;
           break;
       case CPSS_QOS_PORT_TRUST_L2_L3_E:
           regData = 3;
           break;
       default:
           return GT_BAD_PARAM;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startWord = 1;
        startBit  = 7;
    }
    else
    {
        startWord = 0;
        startBit  = 10;
    }

    /* write data to Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        startWord,
                                        startBit,
                                        2,
                                        regData);
    return rc;
}

/*******************************************************************************
* cpssDxChCosPortQosTrustModeGet
*
* DESCRIPTION:
*       Get Configured port's Trust Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       port                - logical port number, CPU port.
*
* OUTPUTS:
*       portQosTrustModePtr  - QoS trust mode of a port.
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, port or trustMode.
*       GT_TIMEOUT      - after max number of retries checking if PP ready.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosPortQosTrustModeGet
(
    IN  GT_U8                         devNum,
    IN  GT_U8                         port,
    OUT CPSS_QOS_PORT_TRUST_MODE_ENT  *portQosTrustModePtr
)
{
    GT_U32      regData;   /* the data to be written to rgister */
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_STATUS   rc;        /* return code */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(portQosTrustModePtr);


    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startWord = 1;
        startBit  = 7;
    }
    else
    {
        startWord = 0;
        startBit  = 10;
    }

    /* write data to Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        startWord,
                                        startBit,
                                        2,
                                        &regData);

    switch(regData)
    {
       case 0:
           *portQosTrustModePtr = CPSS_QOS_PORT_NO_TRUST_E;
           break;
       case 1:
           *portQosTrustModePtr = CPSS_QOS_PORT_TRUST_L2_E;
           break;
       case 2:
           *portQosTrustModePtr = CPSS_QOS_PORT_TRUST_L3_E;
           break;
       case 3:
           *portQosTrustModePtr = CPSS_QOS_PORT_TRUST_L2_L3_E;
           break;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChCosPortReMapDSCPSet
*
* DESCRIPTION:
*       Enable/Disable DSCP-to-DSCP re-mapping.
*       Relevant for L2-L3 or L3 trust modes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       port                - logical port number, CPU port.
*       enableDscpMutation  - GT_TRUE: Packet's DSCP is remapped.
*                             GT_FALSE: Packet's DSCP is not remapped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosPortReMapDSCPSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        port,
    IN GT_BOOL                      enableDscpMutation
)
{
    GT_U32      regData;    /* the data to be written to rgister */
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;         /* return code */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);


    regData = (enableDscpMutation == GT_TRUE) ? 1 : 0;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startWord = 1;
        startBit  = 6;
    }
    else
    {
        startWord = 0;
        startBit  = 9;
    }

    /* write data to Ports VLAN and QoS Configuration Table */
    /* <QosTrustMode> and <ReMapDSCP> fields */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        startWord,
                                        startBit,
                                        1,
                                        regData);
    return rc;
}

/*******************************************************************************
* cpssDxChCosPortReMapDSCPGet
*
* DESCRIPTION:
*       Get status (Enabled/Disabled) for DSCP-to-DSCP re-mapping.
*       Relevant for L2-L3 or L3 trust modes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       port                - logical port number, CPU port.
*
* OUTPUTS:
*       enableDscpMutationPtr   - GT_TRUE: Packet's DSCP is remapped.
*                                 GT_FALSE: Packet's DSCP is not remapped.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_TIMEOUT      - after max number of retries checking if PP ready.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosPortReMapDSCPGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      port,
    OUT GT_BOOL   *enableDscpMutationPtr
)
{
    GT_U32      regData;    /* the data to be written to rgister */
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;         /* return code */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enableDscpMutationPtr);


    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startWord = 1;
        startBit  = 6;
    }
    else
    {
        startWord = 0;
        startBit  = 9;
    }

    /* write data to Ports VLAN and QoS Configuration Table */
    /* <QosTrustMode> and <ReMapDSCP> fields */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        startWord,
                                        startBit,
                                        1,
                                        &regData);

     *enableDscpMutationPtr = (regData == 1) ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChCosPortVlanQoSCfgEntryGet
*
* DESCRIPTION:
*       Read an entry from HW from Ports VLAN and QoS Configuration Table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum  - device number
*        port    - logical port number, CPU port
*
* OUTPUTS:
*        entryPtr  - pointer to memory where will be placed next entry.
*                     size 32 Bytes.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum or port.
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosPortVlanQoSCfgEntryGet
(
    IN    GT_U8     devNum,
    IN    GT_U8     port,
    OUT   GT_U32   *entryPtr
)
{
    GT_STATUS       rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    /* Read entry from Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                   port,
                                   entryPtr);
    return rc;
}

/*******************************************************************************
* cpssDxChCosRemarkEntryGet
*
* DESCRIPTION:
*       Read an entry from HW Policers QoS Remarking and Initial DP Table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*        devNum  - device number
*        index   - index of entry [0..71] for Ch and [0..127] for Ch2 and above.
*
* OUTPUTS:
*        entryPtr  - pointer to memory where will be placed next entry. size 4 Bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or entry index.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosRemarkEntryGet
(
    IN    GT_U8     devNum,
    IN    GT_U32    index,
    OUT   GT_U32   *entryPtr
)
{
    GT_STATUS       rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, index);


    /* Policers QoS Remarking and Initial DP Table */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_TABLE_REMARKING_E,
                                   index,
                                   entryPtr);
    return rc;
}

/*******************************************************************************
* cpssDxChCosQoSProfileEntryGet
*
* DESCRIPTION:
*       Read an entry from HW QoS Profile Table on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum  - device number
*        index   - index of entry [0..71] for Ch and [0..127] for Ch2 and above.
*
* OUTPUTS:
*        entryPtr  - pointer to memory where will be placed next entry. size 4 Bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or entry index.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosQoSProfileEntryGet
(
    IN    GT_U8     devNum,
    IN    GT_U32    index,
    OUT   GT_U32   *entryPtr
)
{
    GT_STATUS       rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, index);

    /* Read an entry from QoS Profile Table */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
                                   index,
                                   entryPtr);
    return rc;
}

/*******************************************************************************
* cpssDxChCosTrustDsaTagQosModeSet
*
* DESCRIPTION:
*       Set trust DSA tag QoS mode. To enable end to end QoS in cascade system,
*       the DSA carries QoS profile assigned to the packet in previous device.
*       When the mode is set to GT_TRUE and the packet's DSA tag contains QoS
*       profile, the QoS profile assigned to the packet is assigned from the
*       DSA tag.
*       Relevant for cascading port only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number.
*       port    -  physical port number, CPU port.
*       enable  - trust DSA tag QoS mode.
*                 GT_FALSE - The QoS profile isn't assigned from DSA tag.
*                 GT_TRUE  - If the packets DSA tag contains a QoS profile,
*                     it is assigned to the packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosTrustDsaTagQosModeSet
(
    IN GT_U8      devNum,
    IN GT_U8      port,
    IN GT_BOOL    enable
)
{
    GT_U32 regData;    /* register data */
    GT_U32 startWord;  /* the table word at which the field starts */
    GT_U32 startBit;   /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    regData = (enable == GT_TRUE) ? 1 : 0;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startWord = 1;
        startBit  = 10;
    }
    else
    {
        startWord = 0;
        startBit  = 12;
    }

    /* write data to Ports VLAN and QoS Configuration Table */
    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                           port,
                                           startWord,
                                           startBit,
                                           1,
                                           regData);
}

/*******************************************************************************
* cpssDxChCosTrustDsaTagQosModeGet
*
* DESCRIPTION:
*       Get trust DSA tag QoS mode. To enable end to end QoS in cascade system,
*       the DSA carries QoS profile assigned to the packet in previous device.
*       When the mode is  GT_TRUE and the packet's DSA tag contains QoS
*       profile, the QoS profile assigned to the packet is assigned from the
*       DSA tag.
*       Relevant for cascading port only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       port       -  physical port number, CPU port.
*
* OUTPUTS:
*       enablePtr  - trust DSA tag QoS mode.
*                    GT_FALSE - The QoS profile isn't assigned from DSA tag.
*                    GT_TRUE  - If the packets DSA tag contains a QoS profile,
*                               it is assigned to the packet.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_TIMEOUT      - after max number of retries checking if PP ready.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosTrustDsaTagQosModeGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      port,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32 regData;     /* register data */
    GT_U32 startWord;   /* the table word at which the field starts */
    GT_U32 startBit;    /* the word's bit at which the field starts */
    GT_STATUS rc;       /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);


    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion and xCat2 */
        startWord = 1;
        startBit  = 10;
    }
    else
    {
        startWord = 0;
        startBit  = 12;
    }

    /* read data to Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,
                                        startWord,
                                        startBit,
                                        1,
                                        &regData);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (regData == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCosTrustExpModeSet
*
* DESCRIPTION:
*       Enable/Disable trust EXP mode for MPLS packets.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number.
*       port    - port number, CPU port.
*       enable  - trust the MPLS header's EXP field.
*                 GT_FALSE - QoS profile from Exp To Qos Profile table
*                            not assigned to the MPLS packet.
*                 GT_TRUE  - QoS profile from Exp To Qos Profile table
*                            assigned to the MPLS packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosTrustExpModeSet
(
    IN GT_U8      devNum,
    IN GT_U8      port,
    IN GT_BOOL    enable
)
{
    GT_U32 regData;    /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    regData = (enable == GT_FALSE) ? 0 : 1;

    /* write data to Ports VLAN and QoS Configuration Table */
    return prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
        port,
        1 /*startWord*/,
        9 /*startBit*/,
        1,
        regData);
}

/*******************************************************************************
* cpssDxChCosTrustExpModeGet
*
* DESCRIPTION:
*       Get Enable/Disable state of trust EXP mode for MPLS packets.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum     - device number.
*       port       - port number, CPU port.
*
* OUTPUTS:
*       enablePtr  - (pointer to) trust the MPLS header's EXP field.
*                    GT_FALSE - QoS profile from Exp To Qos Profile table
*                               not assigned to the MPLS packet.
*                    GT_TRUE  - QoS profile from Exp To Qos Profile table
*                               assigned to the MPLS packet.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_TIMEOUT               - after max number of retries checking if PP ready.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosTrustExpModeGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      port,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32 regData;     /* register data */
    GT_STATUS rc;       /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read data to Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChReadTableEntryField(
        devNum,
        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
        port,
        1 /*startWord*/,
        9 /*startBit*/,
        1,
        &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (regData == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCosL2TrustModeVlanTagSelectSet
*
* DESCRIPTION:
*       Select type of VLAN tag (either Tag0 or Tag1) for Trust L2 Qos mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       port            - port number, CPU port.
*       vlanTagType     - Vlan Tag Type (tag0 or tag1)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port or vlanTagType.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosL2TrustModeVlanTagSelectSet
(
    IN GT_U8                     devNum,
    IN GT_U8                     port,
    IN CPSS_VLAN_TAG_TYPE_ENT    vlanTagType
)
{
    GT_U32 regData;    /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    switch (vlanTagType)
    {
        case CPSS_VLAN_TAG0_E:
            regData = 0;
            break;
        case CPSS_VLAN_TAG1_E:
            regData = 1;
            break;
        default: return GT_BAD_PARAM;
    }

    /* write data to Ports VLAN and QoS Configuration Table */
    return prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
        port,
        2 /*startWord*/,
        1 /*startBit*/,
        1,
        regData);
}

/*******************************************************************************
* cpssDxChCosL2TrustModeVlanTagSelectGet
*
* DESCRIPTION:
*       Get Selected type of VLAN tag (either Tag0 or Tag1) for Trust L2 Qos mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum     - device number.
*       port       - port number, CPU port.
*
* OUTPUTS:
*       vlanTagTypePtr  - (pointer to)Vlan Tag Type (tag0 or tag1)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCosL2TrustModeVlanTagSelectGet
(
    IN  GT_U8                     devNum,
    IN  GT_U8                     port,
    OUT CPSS_VLAN_TAG_TYPE_ENT    *vlanTagTypePtr
)
{
    GT_U32 regData;     /* register data */
    GT_STATUS rc;       /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(vlanTagTypePtr);

    /* read data to Ports VLAN and QoS Configuration Table */
    rc = prvCpssDxChReadTableEntryField(
        devNum,
        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
        port,
        2 /*startWord*/,
        1 /*startBit*/,
        1,
        &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *vlanTagTypePtr =
        (regData == 0)
            ? CPSS_VLAN_TAG0_E
            : CPSS_VLAN_TAG1_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCosPortUpProfileIndexSet
*
* DESCRIPTION:
*       Set per ingress port and per UP (user priority) the 'UP profile index' (table selector).
*       See also API cpssDxChCosUpCfiDeiToProfileMapSet
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - ingress port number including CPU port.
*       up              - user priority of a VLAN or DSA tagged packet [0..7].
*       upProfileIndex  - the UP profile index (table selector) , value 0..1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - wrong devNum, portNum or up
*       GT_OUT_OF_RANGE    - upProfileIndex  > 1
*       GT_HW_ERROR        - on HW error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChCosPortUpProfileIndexSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U8    up,
    IN GT_U32   upProfileIndex
)
{
    GT_U32  regAddr;  /* the register address */
    GT_U32  bitIndex; /* bit index in register to write to */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);
    PRV_CPSS_DXCH_CHECK_DATA_UP_PROFILE_INDEX_MAC(upProfileIndex);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* Get the register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.
                qos.cfiUpToQoSProfileMappingTableSelector[localPort / 4];

    bitIndex =  ((localPort % 4) * 8) + up;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                    bitIndex, 1, upProfileIndex);

}

/*******************************************************************************
* cpssDxChCosPortUpProfileIndexGet
*
* DESCRIPTION:
*       Get per ingress port and per UP (user priority) the 'UP profile index' (table selector).
*       See also API cpssDxChCosUpCfiDeiToProfileMapSet
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - ingress port number including CPU port.
*       up              - user priority of a VLAN or DSA tagged packet [0..7].
*
* OUTPUTS:
*       upProfileIndexPtr - (pointer to)the UP profile index (table selector) , value 0..1
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, portNum or up.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChCosPortUpProfileIndexGet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U8    up,
    OUT GT_U32  *upProfileIndexPtr
)
{
    GT_U32  regAddr;  /* the register address */
    GT_U32  bitIndex; /* bit index in register to read from */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);
    CPSS_NULL_PTR_CHECK_MAC(upProfileIndexPtr);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* Get the register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.
                qos.cfiUpToQoSProfileMappingTableSelector[localPort / 4];

    bitIndex =  ((localPort % 4) * 8) + up;

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr,
                    bitIndex, 1, upProfileIndexPtr);
}

