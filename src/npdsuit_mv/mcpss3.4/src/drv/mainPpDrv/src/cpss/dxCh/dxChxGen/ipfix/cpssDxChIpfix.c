/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpfix.c
*
* DESCRIPTION:
*       CPSS APIs for IP Flow Information Export (IPFIX).
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/ipfix/cpssDxChIpfix.h>
#include <cpss/dxCh/dxCh3/policer/private/prvCpssDxCh3Policer.h>

/* Policer timer 6 is the IPfix nano timer. */
#define PRV_CPSS_DXCH_IPFIX_NANO_TIMER_CNS          6

/* Policer timer 7 is the second timer [31:0] */
#define PRV_CPSS_DXCH_IPFIX_SECOND_LSB_TIMER_CNS    7

/* Policer timer 8 is the second timer [63:32] */
#define PRV_CPSS_DXCH_IPFIX_SECOND_MSB_TIMER_CNS    8

/*******************************************************************************
* cpssDxChIpfixEntrySet
*
* DESCRIPTION:
*       Sets IPFIX entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of IPFIX Entry.
*                         Range: see datasheet for specific device.
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In order to set IPFIX entry under traffic perform the following
*       algorithm(none-zero setting of IPFIX Counters while enabled\still
*       under traffic may cause data coherency problems):
*           - disconnect entry from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entry back to traffic
*       This API address the same memory area (counters place) as can be set
*       by "cpssDxCh3PolicerBillingEntrySet".
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixEntrySet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_IPFIX_ENTRY_STC           *ipfixEntryPtr
)
{
    return cpssDxChIpfixPortGroupEntrySet(devNum,
                                          CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                          stage,
                                          entryIndex,
                                          ipfixEntryPtr);
}

/*******************************************************************************
* cpssDxChIpfixEntryGet
*
* DESCRIPTION:
*       Gets IPFIX entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex  - index of IPFIX Entry.
*                     Range: see datasheet for specific device.
*       reset       - reset flag (please refer to COMMENTS section for further
*                     explanations):
*                     GT_TRUE  - performing read and reset atomic operation.
*                     GT_FALSE - performing read entry operation only.
*
* OUTPUTS:
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       When counters are read in clear on read mode (reset == GT_TRUE), The
*       following actions are done on the different IPFIX entry fields:
*       - The <Packet Counter>, <Byte Counter>, and <Dropped Packet Counter>
*         fields are cleared.
*       - The <Timestamp> field is updated to the current time.
*       - The <Last Sampled Value> is cleared.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  GT_BOOL                             reset,
    OUT CPSS_DXCH_IPFIX_ENTRY_STC           *ipfixEntryPtr
)
{
    return cpssDxChIpfixPortGroupEntryGet(devNum,
                                          CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                          stage,
                                          entryIndex,
                                          reset,
                                          ipfixEntryPtr);
}

/*******************************************************************************
* cpssDxChIpfixDropCountModeSet
*
* DESCRIPTION:
*       Configures IPFIX drop counting mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode    - drop counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixDropCountModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT      mode
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    switch (mode)
    {
        case CPSS_DXCH_IPFIX_DROP_COUNT_MODE_METER_ONLY_E:
            fieldValue = 0;
            break;
        case CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ALL_E:
            fieldValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixControl;

    /* Set IPFIX Control register, <Enable dropped packets counting> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 1, fieldValue);
}

/*******************************************************************************
* cpssDxChIpfixDropCountModeGet
*
* DESCRIPTION:
*       Gets IPFIX drop counting mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr - pointer to drop counting mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixDropCountModeGet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    OUT CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT     *modePtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value  */
    GT_STATUS   rc;             /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixControl;

    /* Get IPFIX Control register, <Enable dropped packets counting> field */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 8, 1, &fieldValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    *modePtr = ( fieldValue == 0 ) ? CPSS_DXCH_IPFIX_DROP_COUNT_MODE_METER_ONLY_E :
                                     CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ALL_E ;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpfixCpuCodeSet
*
* DESCRIPTION:
*       Sets the 6 most significant bits of the CPU Code in case of mirroring.
*       The two least significant bits are taken from the IPfix entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       cpuCode - A cpu code to set the 6 most significant bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or cpu code.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixCpuCodeSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
)
{
    GT_U32  regAddr;                                    /* register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;  /* DSA code */
    GT_STATUS   rc;                                     /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode,&dsaCpuCode);
    if( rc != GT_OK )
    {
        return rc;
    }

    /* Since this CPU code is expected to set only the 6 MSb, while the 2 LSb  */
    /* are to be set by the IPFIX entry <CPU sub code> field, the 2 LSb should */
    /* be unset, thus enabling a range for 4 consecutive CPU codes.            */
    if( (dsaCpuCode & 0x3) != 0x0 )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixControl;

    /* Set IPFIX Control register, <ipfix cpu code > field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 6, (((GT_U32)dsaCpuCode) >> 2));
}

/*******************************************************************************
* cpssDxChIpfixCpuCodeGet
*
* DESCRIPTION:
*       Gets the 6 most significant bits of the CPU Code in case of mirroring.
*       The two least significant bits are taken from the IPfix entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       cpuCodePtr  - pointer to a cpu code to set the 6 most significant bits.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixCpuCodeGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT CPSS_NET_RX_CPU_CODE_ENT        *cpuCodePtr
)
{
    GT_U32  regAddr;                                    /* register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;  /* DSA code */
    GT_U32  tempCode;                                   /* dsa 6 LSb    */
    GT_STATUS   rc;                                     /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixControl;

    /* Get IPFIX Control register, <ipfix cpu code> field */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 6, &tempCode);
    if( rc != GT_OK )
    {
        return rc;
    }

    dsaCpuCode = (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)(tempCode << 2);

    return prvCpssDxChNetIfDsaToCpuCode(dsaCpuCode,cpuCodePtr);
}

/*******************************************************************************
* cpssDxChIpfixAgingEnableSet
*
* DESCRIPTION:
*       Enables or disabled the activation of aging for IPfix.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable  - GT_TRUE for enable, GT_FALSE for disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixAgingEnableSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_BOOL                          enable
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    fieldValue = ( enable == GT_FALSE ) ? 0 : 1;

    /* Set Policer Control register, <Enable IPfix aging> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 22, 1, fieldValue);
}

/*******************************************************************************
* cpssDxChIpfixAgingEnableGet
*
* DESCRIPTION:
*       Gets enabling status of aging for IPfix.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr   - pointer to: GT_TRUE for enable, GT_FALSE for disable.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixAgingEnableGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_BOOL                         *enablePtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value  */
    GT_STATUS   rc;             /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Get Policer Control register, <Enable IPfix aging> field */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 22, 1, &fieldValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    *enablePtr = ( fieldValue == 0 ) ? GT_FALSE : GT_TRUE ;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpfixWraparoundConfSet
*
* DESCRIPTION:
*       Configures IPFIX wraparound parameters.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       confPtr - pointer to wraparound configuration structure.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, action or threshold.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Since there is a HW limitation that wraparound byte threshold 18 LSBs 
*       must be set to 0, the function ignores the 18 LSBs input value and set
*       them to 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixWraparoundConfSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC       *confPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value  */
    GT_STATUS   rc;             /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(confPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    switch (confPtr->action)
    {
        case CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_NONE_E:
            fieldValue = 0;
            break;
        case CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_CLEAR_E:
            fieldValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* check counters thresholds validity */
    if( confPtr->dropThreshold >= BIT_30 ||
        confPtr->packetThreshold >= BIT_30 ||
        confPtr->byteThreshold.l[1] >= BIT_4 ||
        ((confPtr->byteThreshold.l[1] == 0) && (confPtr->byteThreshold.l[0] < BIT_18)) )
    {
        return GT_OUT_OF_RANGE;
    }

    /* Byte threshold 18lsb must be 0. */
    confPtr->byteThreshold.l[0] &= 0xFFFC0000 ;
    
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixControl;

    /* Set IPFIX Control register, <IPfix Wrap Around Action> field */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 9, 1, fieldValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixDroppedWaThreshold;

    /* Set IPFIX dropped packet count WA threshold register */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 30, confPtr->dropThreshold);
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixPacketWaThreshold;

    /* Set IPFIX packet count WA threshold register */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 30, confPtr->packetThreshold);
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixByteWaThresholdLsb;

    /* Set PLR IPFIX byte count WA threshold LSB register */
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, confPtr->byteThreshold.l[0]);
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixByteWaThresholdMsb;

    /* Set PLR IPFIX byte count WA threshold MSB register */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 4, confPtr->byteThreshold.l[1]);
}

/*******************************************************************************
* cpssDxChIpfixWraparoundConfGet
*
* DESCRIPTION:
*       Get IPFIX wraparound configuration.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       confPtr - pointer to wraparound configuration structure.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixWraparoundConfGet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    OUT CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC      *confPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value  */
    GT_STATUS   rc;             /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(confPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixControl;

    /* Get IPFIX Control register, <IPfix Wrap Around Action> field */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 9, 1, &fieldValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    confPtr->action = (fieldValue == 0) ? CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_NONE_E :
                                          CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_CLEAR_E;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixDroppedWaThreshold;

    /* Get IPFIX dropped packet count WA threshold register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 30, &(confPtr->dropThreshold));
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixPacketWaThreshold;

    /* Get IPFIX packet count WA threshold register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 30, &(confPtr->packetThreshold));
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixByteWaThresholdLsb;

    /* Get PLR IPFIX byte count WA threshold LSB register */
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &(confPtr->byteThreshold.l[0]));
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixByteWaThresholdMsb;

    /* Get PLR IPFIX byte count WA threshold MSB register */
    return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 4, &(confPtr->byteThreshold.l[1]));
}

/*******************************************************************************
* prvCpssDxChIpfixGetBitmap
*
* DESCRIPTION:
*       Utility function to get bitmap or part of it.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       startIndex      - the first bits mapbit.
*       endIndex        - the last bits mapbit.
*       memStart        - address of the bitmap start.
*       reset           - clear the bits after read:
*                         GT_TRUE - clear,
*                         GT_FALSE - don't clear.
*
* OUTPUTS:
*       outPtr      - pointer to a buffer containig the bitmap data.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChIpfixGetBitmap
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               startIndex,
    IN GT_U32               endIndex,
    IN GT_U32               memStart,
    IN GT_BOOL              reset,
    OUT GT_U32              *outPtr
)
{
    GT_U32  i,j;
    GT_U32  memData;        /* address of the complete HW bitmap */
    GT_U32  startWord;      /* the word in the HW bitmap with contains the "startIndex" bit */
    GT_U32  endWord;        /* the word in the HW bitmap with contains the "endIndex" bit */
    GT_U32  startBitInWord; /* "startIndex" bit place in "startWord" */
    GT_U32  endBitInWord;   /* "endIndex" bit place in "endWord" */
    GT_STATUS rc;           /* return code */
    GT_U32  portGroupId;    /* the port group Id - support multi-port-groups device */

    CPSS_NULL_PTR_CHECK_MAC(outPtr);

    startWord = startIndex / 32;
    endWord   = endIndex / 32;
    startBitInWord = startIndex % 32;
    endBitInWord   = endIndex % 32;

    for( j = 0 ; j <= (endWord - startWord) ; j++ )
    {
        outPtr[j] = 0;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        j = 0;
        /* Only if startWord is different from endWord the following for loop is done. */
        /* During each iteration consecutive 32 bits are read but only relevant ones   */
        /* are set to 0 in case reset flag is enabled.                                 */
        for( i = startWord ; i < endWord ; i++ )
        {
            /* Get bits from the first word starting at startBitInWord place. */
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                     (memStart + i*4), &memData);
            if (rc != GT_OK)
            {
                return rc;
            }
            outPtr[j] |= (memData >> startBitInWord);

            if( reset == GT_TRUE )
            {
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                        (memStart + i*4),
                                                        startBitInWord, 32 - startBitInWord, 0);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            /* In case startBitInWord is not 0 bits should be read from the adjacent word. */
            if( startBitInWord != 0 )
            {
                rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                         (memStart + i*4 + 4), &memData);
                if (rc != GT_OK)
                {
                    return rc;
                }
                outPtr[j] |= (memData << (32 - startBitInWord));

                if( reset == GT_TRUE )
                {
                    if( i == (endWord-1))
                    {
                        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                                (memStart + i*4 + 4),
                                                                0, endBitInWord + 1, 0);
                    }
                    else
                    {
                        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                                (memStart + i*4 + 4),
                                                                0, startBitInWord, 0);
                    }

                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
            j++;
        }

        /* Reading bits from the endWord (which is the only word in case  startIndex and */
        /* endIndex are in the same HW word) in case endBitInWord >= startBitInWord,     */
        /* otherwise the bits from endWord were alreay read in the for loop.             */
        if( endBitInWord >= startBitInWord )
        {
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                     (memStart + i*4), &memData);
            if (rc != GT_OK)
            {
                return rc;
            }

            outPtr[j] |= (memData >> startBitInWord);

            if( reset == GT_TRUE )
            {
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                        (memStart + i*4),
                                                        startBitInWord,
                                                        endBitInWord - startBitInWord + 1,
                                                        0);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpfixWraparoundStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of wraparound entries indexes.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex  - the first entry index to get wraparound indication on.
*       endIndex    - the last entry index to get wraparound indication on.
*       reset       - clear the bits after read:
*                     GT_TRUE - clear,
*                     GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr     - pointer to a bitmap indication wraparound.
*                     each bit indicate: 0 - no WA occured, 1 - WA occured.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixWraparoundStatusGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                           startIndex,
    IN GT_U32                           endIndex,
    IN GT_BOOL                          reset,
    OUT GT_U32                          *bmpPtr
)
{
    return cpssDxChIpfixPortGroupWraparoundStatusGet(devNum,
                                                     CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                     stage,
                                                     startIndex,
                                                     endIndex,
                                                     reset,
                                                     bmpPtr);
}

/*******************************************************************************
* cpssDxChIpfixAgingStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of aged entries indexes.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex  - the first entry index to get aging indication on.
*       endIndex    - the last entry index to get aging indication on.
*       reset       - clear the bits after read:
*                     GT_TRUE - clear,
*                     GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication aging.
*                     each bit indicate: 0 - no packet recieved on flow (aged).
*                                        1 - packet recieved on flow.
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixAgingStatusGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                           startIndex,
    IN GT_U32                           endIndex,
    IN GT_BOOL                          reset,
    OUT GT_U32                          *bmpPtr
)
{
    return cpssDxChIpfixPortGroupAgingStatusGet(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                stage,
                                                startIndex,
                                                endIndex,
                                                reset,
                                                bmpPtr);
}

/*******************************************************************************
* cpssDxChIpfixTimestampUploadSet
*
* DESCRIPTION:
*       Configures IPFIX timestamp upload per stages.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       uploadPtr   - pointer to timestamp upload configuration of a stage.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or upload mode.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Actual upload operation is triggered by
*       "cpssDxChIpfixTimestampUploadTrigger".
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampUploadSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC    *uploadPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value  */
    GT_U32      nanoSecHwVal;   /* hw value of nanoseconds */
    GT_STATUS   rc;             /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(uploadPtr);

    if( uploadPtr->timer.nanoSecondTimer >= BIT_30)
    {
        return GT_OUT_OF_RANGE;
    }

    switch(uploadPtr->uploadMode)
    {
        case CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E:
            fieldValue = 0;
            break;
        case CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E:
            fieldValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixControl;

    /* Set IPFIX Control register, <time stamp upload mode> field */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 7, 1, fieldValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* Check if TOD nanoseconds errata is enabled */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E))
    {
        /* nanoseconds value should be multiplies of 20ns */
        nanoSecHwVal = (uploadPtr->timer.nanoSecondTimer / 20) * 20;
    }
    else
    {
        nanoSecHwVal = uploadPtr->timer.nanoSecondTimer;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixNanoTimerStampUpload;

    /* Set PLR IPFIX nano timer stamp upload */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 30, nanoSecHwVal);
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixSecLsbTimerStampUpload;

    /* Set PLR IPFIX Seconds LSb timer stamp upload */
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, uploadPtr->timer.secondTimer.l[0]);
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixSecMsbTimerStampUpload;

    /* Set PLR IPFIX Seconds MSb timer stamp upload */
    return prvCpssDrvHwPpWriteRegister(devNum, regAddr, uploadPtr->timer.secondTimer.l[1]);
}

/*******************************************************************************
* cpssDxChIpfixTimestampUploadGet
*
* DESCRIPTION:
*       Gets IPFIX timestamp upload configuration for a stage.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       uploadPtr   - pointer to timestamp upload configuration of a stage.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampUploadGet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    OUT CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC   *uploadPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value  */
    GT_STATUS   rc;             /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(uploadPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixControl;

    /* Get IPFIX Control register, <time stamp upload mode> field */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 7, 1, &fieldValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    uploadPtr->uploadMode = (fieldValue == 0) ?
                            CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E:
                            CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E;


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixNanoTimerStampUpload;

    /* Get PLR IPFIX nano timer stamp upload */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 30, &(uploadPtr->timer.nanoSecondTimer));
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixSecLsbTimerStampUpload;

    /* Get PLR IPFIX Seconds LSb timer stamp upload */
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &(uploadPtr->timer.secondTimer.l[0]));
    if (GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixSecMsbTimerStampUpload;

    /* Get PLR IPFIX Seconds MSb timer stamp upload */
    return prvCpssDrvHwPpReadRegister(devNum, regAddr, &(uploadPtr->timer.secondTimer.l[1]));
}

/*******************************************************************************
* cpssDxChIpfixTimestampUploadTrigger
*
* DESCRIPTION:
*       Triggers the IPFIX timestamp upload operation.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_BAD_STATE                - if previous upload operation is not
*                                     finished yet.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       CPSS_DXCH_POLICER_STAGE_INGRESS_0_E triggering controls
*       CPSS_DXCH_POLICER_STAGE_INGRESS_1 & CPSS_DXCH_POLICER_STAGE_EGRESS_E
*       as well, all uploads are done simultenously.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampUploadTrigger
(
    IN GT_U8    devNum
)
{
    GT_U32      regAddr;            /* register address */
    GT_BOOL     prevUploadStatus;   /* previous upload state */
    GT_STATUS   rc;                 /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    /* Read triggering field to check if previous operation already done.    */
    rc = cpssDxChIpfixTimestampUploadStatusGet(devNum, &prevUploadStatus);
    if (GT_OK != rc)
    {
        return rc;
    }

    if ( GT_FALSE == prevUploadStatus)
    {
        return GT_BAD_STATE;
    }

    /* trigger the upload */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[CPSS_DXCH_POLICER_STAGE_INGRESS_0_E].ipfixControl;

    /* Set IPFIX Control register, <time stamp trig> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 6, 1, 1);
}

/*******************************************************************************
* cpssDxChIpfixTimestampUploadStatusGet
*
* DESCRIPTION:
*       Gets IPFIX timestamp upload operation status.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       uploadStatusPtr - pointer to indication whether upload was done.
*                         GT_TRUE: upload is finished.
*                         GT_FALSE: upload still in progess.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampUploadStatusGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *uploadStatusPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(uploadStatusPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[CPSS_DXCH_POLICER_STAGE_INGRESS_0_E].ipfixControl;

    /* check that all port groups are ready */
    rc = prvCpssDxChPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                regAddr,6,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        rc = GT_OK;
        /* not all port groups has the bit with value 0 */
        *uploadStatusPtr = GT_FALSE;
    }
    else
    {
        *uploadStatusPtr = GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChIpfixTimerGet
*
* DESCRIPTION:
*       Gets IPFIX timer
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       timerPtr    - pointer to IPFIX timer.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimerGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT CPSS_DXCH_IPFIX_TIMER_STC       *timerPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(timerPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTimerTbl;

    /* Get IPFIX nano timer */
    rc = prvCpssDrvHwPpGetRegField(devNum,
                                   regAddr + (PRV_CPSS_DXCH_IPFIX_NANO_TIMER_CNS * 4),
                                   0, 30, &(timerPtr->nanoSecondTimer));
    if (GT_OK != rc)
    {
        return rc;
    }

    /* Get IPFIX second LSb timer */
    rc = prvCpssDrvHwPpReadRegister(devNum,
                                   regAddr + (PRV_CPSS_DXCH_IPFIX_SECOND_LSB_TIMER_CNS * 4),
                                   &(timerPtr->secondTimer.l[0]));
    if (GT_OK != rc)
    {
        return rc;
    }

    /* Get IPFIX second MSb timer */
    rc = prvCpssDrvHwPpReadRegister(devNum,
                                   regAddr + (PRV_CPSS_DXCH_IPFIX_SECOND_MSB_TIMER_CNS * 4),
                                   &(timerPtr->secondTimer.l[1]));
    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpfixAlarmEventsGet
*
* DESCRIPTION:
*       Retrieves up to 16 IPFIX entries indexes where alarm events (due to
*       sampling) occured.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum       - physical device number.
*       stage        - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
*
* OUTPUTS:
*       eventsArr    - array of alarm events, each valid element contains
*                      the index of IPFIX entry which caused the alarm.
*       eventsNumPtr - The number of valid entries in eventsPtr list.
*                      Range: 0..15
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixAlarmEventsGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_U32                          eventsArr[CPSS_DXCH_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS],
    OUT GT_U32                          *eventsNumPtr
)
{
    return cpssDxChIpfixPortGroupAlarmEventsGet(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                stage,
                                                eventsArr,
                                                eventsNumPtr);
}

/*******************************************************************************
* cpssDxChIpfixPortGroupEntrySet
*
* DESCRIPTION:
*       Sets IPFIX entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of IPFIX Entry.
*                         Range: see datasheet for specific device.
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In order to set IPFIX entry under traffic perform the following
*       algorithm(none-zero setting of IPFIX Counters while enabled\still
*       under traffic may cause data coherency problems):
*           - disconnect entry from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entry back to traffic
*       This API address the same memory area (counters place) as can be set
*       by "cpssDxCh3PolicerBillingEntrySet".
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_IPFIX_ENTRY_STC           *ipfixEntryPtr
)
{
    GT_STATUS rc;                   /* return code */
    GT_U32    regAddr;                /* register address */
    GT_U32    hwData[8];              /* HW data */
    GT_U32    samplingActionHwVal;    /* HW value of samplingAction */
    GT_U32    randomFlagHwVal;        /* HW value of randomFlag */
    GT_U32    samplingModeHwVal;      /* HW value of samplingMode */
    GT_U32    portGroupId;            /* port group id */
    GT_U32    wordAddr;               /* entry word's address */
    GT_U16    portGroupNum;           /* number of port groups      */
    GT_U16    portGroupIndex;         /* index of port group in BMP */
    GT_U64    averageByteCounter;     /* byte counter value to write into HW */
    GT_U64    packetCounter;          /* packet counter value received into API */
    GT_U64    averagePacketCounter;   /* packet counter value to write into HW */
    GT_U64    dropCounter;            /* drop counter value received into API */
    GT_U64    averageDropCounter;     /* drop counter value to write into HW */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(ipfixEntryPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entry */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,entryIndex);

    /* check params validity */

    if( ipfixEntryPtr->dropCounter >= BIT_30 ||
        ipfixEntryPtr->packetCount >= BIT_30 ||
        ipfixEntryPtr->byteCount.l[1] >= BIT_4 ||
        ipfixEntryPtr->samplingWindow.l[1] >= BIT_4 ||
        ipfixEntryPtr->cpuSubCode >= BIT_2 ||
        ipfixEntryPtr->timeStamp >= BIT_16 ||
        ipfixEntryPtr->lastSampledValue.l[1] >= BIT_4)
    {
        return GT_OUT_OF_RANGE;
    }


    switch (ipfixEntryPtr->samplingAction)
    {
        case CPSS_DXCH_IPFIX_SAMPLING_ACTION_ALARM_E:
            samplingActionHwVal = 0;
            break;
        case CPSS_DXCH_IPFIX_SAMPLING_ACTION_MIRROR_E:
            /* On the egress, sampling can only trigger Alarm events, */
            /* since mirroring to the CPU is not supported from the egress pipe. */
            if( stage == CPSS_DXCH_POLICER_STAGE_EGRESS_E )
            {
                return GT_BAD_PARAM;
            }
            samplingActionHwVal = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch (ipfixEntryPtr->randomFlag)
    {
        case CPSS_DXCH_IPFIX_SAMPLING_DIST_DETERMINISTIC_E:
            randomFlagHwVal = 0;
            break;
        case CPSS_DXCH_IPFIX_SAMPLING_DIST_RANDOM_E:
            randomFlagHwVal = 1;
            if( ipfixEntryPtr->logSamplingRange >= BIT_6 )
            {
                return GT_BAD_PARAM;
            }

            break;
        default:
            return GT_BAD_PARAM;
    }

    switch (ipfixEntryPtr->samplingMode)
    {
        case CPSS_DXCH_IPFIX_SAMPLING_MODE_DISABLE_E:
            samplingModeHwVal = 0;
            break;
        case CPSS_DXCH_IPFIX_SAMPLING_MODE_PACKET_E:
            samplingModeHwVal = 1;
            break;
        case CPSS_DXCH_IPFIX_SAMPLING_MODE_BYTE_E:
            samplingModeHwVal = 2;
            break;
        case CPSS_DXCH_IPFIX_SAMPLING_MODE_TIME_E:
            samplingModeHwVal = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* calculate number of port groups */
    portGroupNum = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        portGroupNum ++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    if (portGroupNum == 0)
    {
        /* nothing to do */
        return GT_OK;
    }

    packetCounter.l[0] = ipfixEntryPtr->packetCount;
    packetCounter.l[1] = 0;
    dropCounter.l[0] = ipfixEntryPtr->dropCounter;
    dropCounter.l[1] = 0;


    /* Check if Policer Counting memory errata is enabled */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E))
    {
        /* check that the previous action is finished in each portGroup, do busy wait */
        rc = prvCpssDxCh3PolicerAccessControlBusyWait(devNum, portGroupsBmp, stage);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Get Ingress Policer Table Access Data 0 Register Address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerTblAccessDataReg;

    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerCountingTbl + (entryIndex * 0x20);
    }

    /* loop on all port groups to set the counter value */
    portGroupIndex = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* Nullify the hwData */
        cpssOsMemSet(hwData, 0, sizeof(hwData));

        /* calculate average value for byte counter */
        prvCpssDxChPortGroupCounterAverageValueGet(portGroupNum, portGroupIndex,
                                                   ipfixEntryPtr->byteCount,
                                                   &averageByteCounter);

        /* calculate average value for packet counter */
        prvCpssDxChPortGroupCounterAverageValueGet(portGroupNum, portGroupIndex,
                                                   packetCounter,
                                                   &averagePacketCounter);

        /* calculate average value for drop counter */
        prvCpssDxChPortGroupCounterAverageValueGet(portGroupNum, portGroupIndex,
                                                   dropCounter,
                                                   &averageDropCounter);

        /************************/
        /* Create HW entry data */
        /************************/
        hwData[0] = averageByteCounter.l[0];
        hwData[1] = averageByteCounter.l[1] |
                    ((averagePacketCounter.l[0] & 0xFFFFFFF) << 4);
        hwData[2] = (averagePacketCounter.l[0] >> 28) |
                    (ipfixEntryPtr->timeStamp << 2) |
                    (samplingModeHwVal << 18) |
                    (randomFlagHwVal << 20) |
                    ((ipfixEntryPtr->logSamplingRange & 0x3F) << 21) |
                    (samplingActionHwVal << 27) |
                    (ipfixEntryPtr->cpuSubCode << 28) |
                    ((ipfixEntryPtr->samplingWindow.l[0] & 0x3) << 30);
        hwData[3] = (ipfixEntryPtr->samplingWindow.l[0]  >> 2) |
                    ((ipfixEntryPtr->samplingWindow.l[1] & 0x3) << 30);
        hwData[4] = (ipfixEntryPtr->samplingWindow.l[1]  >> 2) |
                    ((ipfixEntryPtr->lastSampledValue.l[0] & 0x3FFFFFFF) << 2);
        hwData[5] = (ipfixEntryPtr->lastSampledValue.l[0] >> 30) |
                    (ipfixEntryPtr->lastSampledValue.l[1] << 2) |
                    ((ipfixEntryPtr->randomOffset & 0x3FFFFFF) << 6);
        hwData[6] = (ipfixEntryPtr->randomOffset >> 26) |
                    ((averageDropCounter.l[0] & 0x3FFFFFF) << 6);
        hwData[7] = (averageDropCounter.l[0] >> 26);

        /* In the last word (8), 5th bit shouild be set as Counter mode is IPFIX */
        hwData[7] |= ( 1 << 4);


        portGroupIndex++;

        /* Check if Policer Counting memory errata is enabled */
         if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                  PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E))
         {
             /* Indirect access to counting memory should be used */

             rc = prvCpssDrvHwPpPortGroupWriteRam(
                 devNum, portGroupId, regAddr, 8/* words*/, hwData);
             if (rc != GT_OK)
             {
                 return rc;
             }

             rc = prvCpssDxCh3PolicerInternalTableAccess(devNum,
                                 portGroupId,
                                 stage,
                                 entryIndex,
                                 PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_WRITE_E);
             if (rc != GT_OK)
             {
                 return rc;
             }

         }
         else
         {

             /* set first 7 words of IPFIX entry */
             rc = prvCpssDrvHwPpPortGroupWriteRam(
                 devNum, portGroupId, regAddr, 7/* words*/, hwData);
             if (rc != GT_OK)
             {
                 return rc;
             }

             /* verify that all words were written before write last one.
                because last one triggers write of whole entry */
             GT_SYNC;

             /* set last word */
             wordAddr = regAddr + 0x1C;
             rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, wordAddr, hwData[7]);
             if (rc != GT_OK)
             {
                 return rc;
             }
         }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpfixPortGroupEntryGet
*
* DESCRIPTION:
*       Gets IPFIX entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of IPFIX Entry.
*                         Range: see datasheet for specific device.
*       reset           - reset flag (please refer to COMMENTS section for further
*                         explanations):
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read entry operation only.
*
* OUTPUTS:
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       When counters are read in clear on read mode (reset == GT_TRUE), The
*       following actions are done on the different IPFIX entry fields:
*       - The <Packet Counter>, <Byte Counter>, and <Dropped Packet Counter>
*         fields are cleared.
*       - The <Timestamp> field is updated to the current time.
*       - The <Last Sampled Value> is cleared.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  GT_BOOL                             reset,
    OUT CPSS_DXCH_IPFIX_ENTRY_STC           *ipfixEntryPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value   */
    GT_U32      samplingActionHwVal;    /* HW value of samplingAction */
    GT_U32      randomFlagHwVal;        /* HW value of randomFlag */
    GT_U32      samplingModeHwVal;      /* HW value of samplingMode */
    GT_U32      portGroupId;            /* port group id */
    GT_U32      firstActivePortGroupId; /* first active port group id */
    GT_STATUS   rc;                     /* return code */

    PRV_CPSS_DXCH3_POLICER_ACCESS_CTRL_CMD_ENT  accessAction; /* access action */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(ipfixEntryPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entry */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,entryIndex);

    /* Define Policer Table Indirect Access triggered action */
    accessAction = (reset == GT_TRUE) ?
                    PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_RESET_E :
                    PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* Perform indirect access to the Policer Table */
        rc = prvCpssDxCh3PolicerInternalTableAccess(devNum,
                                                    portGroupId,
                                                    stage,
                                                    entryIndex,
                                                    accessAction);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        firstActivePortGroupId);

    /* Get Ingress Policer Table Access Data 0 Register Address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;

    /* Get Ingress Policer Table Access Data 7 Register */
    regAddr += 28;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, firstActivePortGroupId,
                                             regAddr, &regValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* Check that counter mode is IPFIX and not Billing */
    if (((regValue >> 4) & 0x1) == 0x0 )
    {
        return GT_BAD_STATE;
    }

    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                regAddr,0,4,
                                                &ipfixEntryPtr->dropCounter,
                                                NULL);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->dropCounter = (ipfixEntryPtr->dropCounter << 26);

    /* Set the register address back to the register 0 */
    regAddr -= 28;

    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                regAddr,0,32,
                                                NULL,
                                                &ipfixEntryPtr->byteCount);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* Get Ingress Policer Table Access Data 1 Register */
    regAddr += 4;

    rc = prvCpssDxChPortGroupsBmpMultiCounterSummary(devNum,
                                                     portGroupsBmp, regAddr,
                                                     0,4,
                                                     &regValue,/*byteCount:4MSbit*/
                                                     4,28,
                                                     &ipfixEntryPtr->packetCount,/*packetCount:28LSbit*/
                                                     0,0,NULL);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->byteCount.l[1] += regValue;

    /* Get Ingress Policer Table Access Data 2 Register */
    regAddr += 4;

    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                regAddr,0,2,
                                                &regValue,
                                                NULL);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->packetCount += (regValue << 28);


    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, firstActivePortGroupId,
                                             regAddr, &regValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->timeStamp = U32_GET_FIELD_MAC(regValue,2,16);

    samplingModeHwVal = U32_GET_FIELD_MAC(regValue,18,2);
    switch(samplingModeHwVal)
    {
        case 0: ipfixEntryPtr->samplingMode = CPSS_DXCH_IPFIX_SAMPLING_MODE_DISABLE_E;
                break;
        case 1: ipfixEntryPtr->samplingMode = CPSS_DXCH_IPFIX_SAMPLING_MODE_PACKET_E;
                break;
        case 2: ipfixEntryPtr->samplingMode = CPSS_DXCH_IPFIX_SAMPLING_MODE_BYTE_E;
                break;
        case 3: ipfixEntryPtr->samplingMode = CPSS_DXCH_IPFIX_SAMPLING_MODE_TIME_E;
                break;
        default:
                return GT_FAIL;
    }

    randomFlagHwVal = U32_GET_FIELD_MAC(regValue,20,1);
    ipfixEntryPtr->randomFlag = (randomFlagHwVal == 0) ?
                                 CPSS_DXCH_IPFIX_SAMPLING_DIST_DETERMINISTIC_E:
                                 CPSS_DXCH_IPFIX_SAMPLING_DIST_RANDOM_E;

    ipfixEntryPtr->logSamplingRange = U32_GET_FIELD_MAC(regValue,21,6);

    samplingActionHwVal = U32_GET_FIELD_MAC(regValue,27,1);
    ipfixEntryPtr->samplingAction = (samplingActionHwVal == 0) ?
                                     CPSS_DXCH_IPFIX_SAMPLING_ACTION_ALARM_E:
                                     CPSS_DXCH_IPFIX_SAMPLING_ACTION_MIRROR_E;

    ipfixEntryPtr->cpuSubCode = U32_GET_FIELD_MAC(regValue,28,2);
    ipfixEntryPtr->samplingWindow.l[0] = U32_GET_FIELD_MAC(regValue,30,2);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                             regAddr, &regValue);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* for timeStamp use the highest value from all port groups */
        if( ipfixEntryPtr->timeStamp < U32_GET_FIELD_MAC(regValue,2,16) )
        {
            ipfixEntryPtr->timeStamp = U32_GET_FIELD_MAC(regValue,2,16);
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    /* Get Ingress Policer Table Access Data 3 Register */
    regAddr += 4;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, firstActivePortGroupId,
                                             regAddr, &regValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->samplingWindow.l[0] |= ((regValue & 0x3FFFFFFF) << 2);
    ipfixEntryPtr->samplingWindow.l[1] = U32_GET_FIELD_MAC(regValue,30,2);

    /* Get Ingress Policer Table Access Data 4 Register */
    regAddr += 4;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, firstActivePortGroupId,
                                             regAddr, &regValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->samplingWindow.l[1] |= ((regValue & 0x3) << 2);
    ipfixEntryPtr->lastSampledValue.l[0] = U32_GET_FIELD_MAC(regValue,2,30);

    /* Get Ingress Policer Table Access Data 5 Register */
    regAddr += 4;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, firstActivePortGroupId,
                                             regAddr, &regValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->lastSampledValue.l[0] |= ((regValue & 0x3) << 30);
    ipfixEntryPtr->lastSampledValue.l[1] = U32_GET_FIELD_MAC(regValue,2,4);
    ipfixEntryPtr->randomOffset = U32_GET_FIELD_MAC(regValue,6,26);

    /* Get Ingress Policer Table Access Data 6 Register */
    regAddr += 4;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, firstActivePortGroupId,
                                             regAddr, &regValue);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->randomOffset |= ((regValue & 0x3F) << 26);

    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                regAddr,6,26,
                                                &regValue,
                                                NULL);
    if (GT_OK != rc)
    {
        return rc;
    }

    ipfixEntryPtr->dropCounter += regValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpfixPortGroupWraparoundStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of wraparound entries indexes.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex      - the first entry index to get wraparound indication on.
*       endIndex        - the last entry index to get wraparound indication on.
*       reset           - clear the bits after read:
*                         GT_TRUE - clear,
*                         GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication wraparound.
*                     each bit indicate: 0 - no WA occured, 1 - WA occured.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupWraparoundStatusGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                           startIndex,
    IN GT_U32                           endIndex,
    IN GT_BOOL                          reset,
    OUT GT_U32                          *bmpPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(bmpPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entries */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,startIndex);
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,endIndex);

    if( startIndex > endIndex )
    {
        return GT_BAD_PARAM;
    }

    return prvCpssDxChIpfixGetBitmap(devNum, portGroupsBmp, startIndex, endIndex,
                                     PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixWrapAroundAlertTbl,
                                     reset, bmpPtr);
}

/*******************************************************************************
* cpssDxChIpfixPortGroupAgingStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of aged entries indexes.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex      - the first entry index to get aging indication on.
*       endIndex        - the last entry index to get aging indication on.
*       reset           - clear the bits after read:
*                         GT_TRUE - clear,
*                         GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication aging.
*                     each bit indicate: 0 - no packet recieved on flow (aged).
*                                        1 - packet recieved on flow.
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupAgingStatusGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                           startIndex,
    IN GT_U32                           endIndex,
    IN GT_BOOL                          reset,
    OUT GT_U32                          *bmpPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(bmpPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entries */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,startIndex);
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,endIndex);

    if( startIndex > endIndex )
    {
        return GT_BAD_PARAM;
    }

    return prvCpssDxChIpfixGetBitmap(devNum, portGroupsBmp, startIndex, endIndex,
                                     PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixAgingAlertTbl,
                                     reset, bmpPtr);
}

/*******************************************************************************
* cpssDxChIpfixPortGroupAlarmEventsGet
*
* DESCRIPTION:
*       Retrieves up to 16 IPFIX entries indexes where alarm events (due to
*       sampling) occured.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
*
* OUTPUTS:
*       eventsArr    - array of alarm events, each valid element contains
*                      the index of IPFIX entry which caused the alarm.
*       eventsNumPtr - The number of valid entries in eventsPtr list.
*                      Range: 0..15
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupAlarmEventsGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_U32                          eventsArr[CPSS_DXCH_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS],
    OUT GT_U32                          *eventsNumPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register address */
    GT_STATUS   rc;             /* return code */
    GT_U32      portGroupId;    /* the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(eventsArr);
    CPSS_NULL_PTR_CHECK_MAC(eventsNumPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    *eventsNumPtr = 0;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixSampleEntriesLog1;

        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regData);
        if (GT_OK != rc)
        {
            return rc;
        }

        if( U32_GET_FIELD_MAC(regData,15,1) == 0x1 )
        {
            eventsArr[*eventsNumPtr] = U32_GET_FIELD_MAC(regData,0,15);
            (*eventsNumPtr)++;
        }

        if( U32_GET_FIELD_MAC(regData,31,1) == 0x1 )
        {
            eventsArr[*eventsNumPtr] = U32_GET_FIELD_MAC(regData,16,15);
            (*eventsNumPtr)++;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].ipfixSampleEntriesLog0;

        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regData);
        if (GT_OK != rc)
        {
            return rc;
        }

        if( U32_GET_FIELD_MAC(regData,15,1) == 0x1 )
        {
            eventsArr[*eventsNumPtr] = U32_GET_FIELD_MAC(regData,0,15);
            (*eventsNumPtr)++;
        }

        if( U32_GET_FIELD_MAC(regData,31,1) == 0x1 )
        {
            eventsArr[*eventsNumPtr] = U32_GET_FIELD_MAC(regData,16,15);
            (*eventsNumPtr)++;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpfixTimestampToCpuEnableSet
*
* DESCRIPTION:
*       Configures all TO_CPU DSA tags to include a Timestamp.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - GT_TRUE: TO_CPU DSA tag contains a Timestamp.
*                 GT_FALSE: TO_CPU DSA tag does not contain a Timestamp.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampToCpuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regData;    /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    regData = ( enable == GT_FALSE ) ? 0 : 1;

    /* Set Header Alteration Global Configuration Register, */
    /* <Enable IPFIX Timestamping To CPU> field.            */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 12, 1, regData);
}

/*******************************************************************************
* cpssDxChIpfixTimestampToCpuEnableGet
*
* DESCRIPTION:
*       Gets enabling status of TO_CPU DSA tags including a Timestamp.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE: TO_CPU DSA tag contains a Timestamp.
*                    GT_FALSE: TO_CPU DSA tag does not contain a Timestamp.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampToCpuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regData;    /* register data */
    GT_STATUS   rc;             /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    /* Get Header Alteration Global Configuration Register, */
    /* <Enable IPFIX Timestamping To CPU> field.            */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 12, 1, &regData);
    if (GT_OK != rc)
    {
        return rc;
    }

    *enablePtr = ( regData == 0 ) ? GT_FALSE : GT_TRUE ;

    return GT_OK;
}

