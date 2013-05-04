/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPfc.c
*
* DESCRIPTION:
*       CPSS implementation for Priority Flow Control functionality.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>

/*******************************************************************************
* cpssDxChPortPfcEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC (Priority Flow Control) triggering or/and response
*       functionality.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       pfcEnable  - PFC enable option.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC enable option
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        If PFC response is enabled, the shaper’s baseline must be
*        at least 0x3FFFC0, see:
*        cpssDxChPortTxShaperBaselineSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcEnableSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_ENABLE_ENT pfcEnable
)
{
    GT_U32 value;         /* register value */
    GT_U32 regAddr;       /* register address */
    GT_STATUS rc;         /* function return value */
    GT_BOOL trigEnable;   /* enable PFC triggering*/
    GT_BOOL resposeEnable;/* enable PFC response */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    switch (pfcEnable)
    {
        case CPSS_DXCH_PORT_PFC_DISABLE_ALL_E:
            trigEnable = resposeEnable = GT_FALSE;
            break;
        case CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E:
            trigEnable = GT_TRUE;
            resposeEnable = GT_FALSE;
            break;
        case CPSS_DXCH_PORT_PFC_ENABLE_RESPONSE_ONLY_E:
            trigEnable = GT_FALSE;
            resposeEnable = GT_TRUE;
            break;
        case CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E:
            trigEnable = resposeEnable = GT_TRUE;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Set <PfcEn> field in the PFC Trigger Global Config reg. */
    value = BOOL2BIT_MAC(trigEnable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set Enable Priority Based FC in TTI Unit Global Config reg. */
    value = BOOL2BIT_MAC(resposeEnable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 24, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set FcResponseEnable in Flow Control Response Config reg. */
    value = BOOL2BIT_MAC(resposeEnable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.flowControl.flowControlConfig;
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcEnableGet
*
* DESCRIPTION:
*       Get the status of PFC (Priority Flow Control) triggering or/and response
*       functionality.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       pfcEnablePtr  - (pointer to) PFC enable option.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcEnableGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_ENABLE_ENT *pfcEnablePtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 trigValue,resposeValue; /* registers value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pfcEnablePtr);

    /* Get <PfcEn> field in the PFC Trigger Global Config reg. */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 1, &trigValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Get Enable Priority Based FC in TTI Unit Global Config reg. */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;
    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 24, 1, &resposeValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    *pfcEnablePtr = (trigValue == 0) ? 
        ((resposeValue == 0) ? 
            CPSS_DXCH_PORT_PFC_DISABLE_ALL_E : 
            CPSS_DXCH_PORT_PFC_ENABLE_RESPONSE_ONLY_E):
        ((resposeValue == 0) ? 
            CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E :
            CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcProfileIndexSet
*
* DESCRIPTION:
*       Binds a source port to a PFC profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileIndexSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   profileIndex
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 fieldOffset;/* the start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (profileIndex >= BIT_3)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcSourcePortProfile;

    regAddr += (portNum/8)*4;
    fieldOffset = (portNum%8)*3;

    return prvCpssDxChHwPpSetRegField(devNum, regAddr,fieldOffset, 3, profileIndex);
}

/*******************************************************************************
* cpssDxChPortPfcProfileIndexGet
*
* DESCRIPTION:
*       Gets the port's PFC profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *profileIndexPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcSourcePortProfile;

    regAddr += (portNum/8)*4;
    fieldOffset = (portNum%8)*3;

    return prvCpssDxChHwPpGetRegField(devNum, regAddr,fieldOffset, 3, profileIndexPtr);
}

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pfcProfileCfgPtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (profileIndex > 7 )
    {
        return GT_BAD_PARAM;
    }

    if (pfcProfileCfgPtr->xoffThreshold >= BIT_11 ||
        pfcProfileCfgPtr->xonThreshold >= BIT_11)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcProfileXoffThresholds;
    regAddr += profileIndex*0x20 + tcQueue*0x4;
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 11, pfcProfileCfgPtr->xoffThreshold);
    if (rc != GT_OK)
    {
        return rc;
    }
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcProfileXonThresholds;
    regAddr += profileIndex*0x20 + tcQueue*0x4;
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 11, pfcProfileCfgPtr->xonThreshold);

}

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pfcProfileCfgPtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (profileIndex > 7 )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcProfileXoffThresholds;
    regAddr += profileIndex*0x20 + tcQueue*0x4;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 11, &(pfcProfileCfgPtr->xoffThreshold));
    if (rc != GT_OK)
    {
        return rc;
    }
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcProfileXonThresholds;
    regAddr += profileIndex*0x20 + tcQueue*0x4;
    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 11, &(pfcProfileCfgPtr->xonThreshold));
}

/*******************************************************************************
* cpssDxChPortPfcCountingModeSet
*
* DESCRIPTION:
*       Sets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcCountMode - PFC counting mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCountingModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountMode
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    switch(pfcCountMode)
    {
        case CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 1, 1, value);
}

/*******************************************************************************
* cpssDxChPortPfcCountingModeGet
*
* DESCRIPTION:
*       Gets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       pfcCountModePtr - (pointer to) PFC counting mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCountingModeGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  *pfcCountModePtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pfcCountModePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 1, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *pfcCountModePtr = (CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT)value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable PFC global drop.
*                    GT_FALSE: Disable PFC global drop.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To configure drop threshold use cpssDxChPortPfcGlobalQueueConfigSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalDropEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    value = BOOL2BIT_MAC(enable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 9, 1, value);
}

/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableGet
*
* DESCRIPTION:
*       Gets the current status of PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of PFC functionality
*                     GT_TRUE:  PFC global drop enabled.
*                     GT_FALSE: PFC global drop disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalDropEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 9, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum        - device number.
*       tcQueue       - traffic class queue (0..7).
*       xoffThreshold - Xoff threshold (0..0x7FF).
*       dropThreshold - Drop threshold. When a global counter with given tcQueue
*               crosses up the dropThreshold the packets are dropped (0..0x7FF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. dropThreshold is used when PFC global drop is enabled.
*       See cpssDxChPortPfcGlobalDropEnableSet.
*       2. All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_U32   xoffThreshold,
    IN GT_U32   dropThreshold
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 value;     /* register value */
    GT_STATUS rc;     /* function return value */
    GT_U32 i;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (xoffThreshold >= BIT_11 || dropThreshold >= BIT_11)
    {
        return GT_OUT_OF_RANGE;
    }

    value = xoffThreshold | (dropThreshold << 11);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.pfcRegs.pfcGlobaGroupOfPortsThresholds;
    regAddr += tcQueue*0x4;

    /* set the value to all port groups*/
    for (i=0; i<4; i++)
    {
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 22, value);
        if (rc != GT_OK)
        {
            return rc;
        }
        regAddr += 0x20;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       xoffThresholdPtr - (pointer to) Xoff threshold.
*       dropThresholdPtr - (pointer to) Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *xoffThresholdPtr,
    OUT GT_U32   *dropThresholdPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 value;     /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(xoffThresholdPtr);
    CPSS_NULL_PTR_CHECK_MAC(dropThresholdPtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.pfcRegs.pfcGlobaGroupOfPortsThresholds;
    regAddr += tcQueue*0x4;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 22, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *xoffThresholdPtr = value & 0x7FF;
    *dropThresholdPtr = value >> 11;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableSet
*
* DESCRIPTION:
*       Enables mapping of PFC timer to priority queue for given scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile.
*       enable     - Determines whether PFC timer to Priority Queue map
*                    is used.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerMapEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_BOOL                                 enable

)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);

    value = BOOL2BIT_MAC(enable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.flowControl.schedulerProfileTcToPriorityMapEnable[profileSet];
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, value);
}

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableGet
*
* DESCRIPTION:
*       Get the status of PFS timer to priority queue mapping for given
*       scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile Set
*
* OUTPUTS:
*       enablePtr   - (pointer to) status of PFC timer to Priority Queue 
*                      mapping.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue. 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerMapEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT GT_BOOL                                *enablePtr

)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.flowControl.schedulerProfileTcToPriorityMapEnable[profileSet];
    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapSet
*
* DESCRIPTION:
*       Sets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcTimer     - PFC timer (0..7)
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_OUT_OF_RANGE          - on out of traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*      
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerToQueueMapSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    IN  GT_U32    tcQueue

)
{
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (pfcTimer >= 8)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.flowControl.pfcTimerToPriorityMapping[pfcTimer];
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 3, tcQueue);
}

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapGet
*
* DESCRIPTION:
*       Gets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       pfcTimer        - PFC timer (0..7).
*
* OUTPUTS:
*       tcQueuePtr      - (pointer to) traffic class queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*       
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerToQueueMapGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    OUT GT_U32    *tcQueuePtr

)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(tcQueuePtr);
    if (pfcTimer >= 8)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.flowControl.pfcTimerToPriorityMapping[pfcTimer];

    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 3, tcQueuePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioSet
*
* DESCRIPTION:
*       Sets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*       shaperToPortRateRatio - shaper rate to port speed ratio 
*                               in percentage (0..100).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range shaper rate to port speed ratio
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    IN  GT_U32                                  shaperToPortRateRatio

)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    if (shaperToPortRateRatio > 100)
    {
        return GT_OUT_OF_RANGE;
    }

    if (shaperToPortRateRatio == 0)
    {
        value = 0;
    }
    else
    {
        /* PFCFactor = ROUNDUP(shaperToPortRateRatio*128/100) - 1 */
        value = (shaperToPortRateRatio*128)/100 -
            (((shaperToPortRateRatio*128)%100 == 0) ? 1 :0);
    }


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.flowControl.tcProfilePfcFactor[tcQueue][profileSet];
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 7, value);
}

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioGet
*
* DESCRIPTION:
*       Gets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*
* OUTPUTS:
*       shaperToPortRateRatioPtr - (pointer to)shaper rate to port speed ratio 
*                               in percentage.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    OUT GT_U32                                 *shaperToPortRateRatioPtr

)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(shaperToPortRateRatioPtr);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.flowControl.tcProfilePfcFactor[tcQueue][profileSet];

    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 7, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (value == 0)
    {
        *shaperToPortRateRatioPtr = 0;
    }
    else
    {
        /* shaperToPortRateRatio = ROUNDDOWN(PFCFactor*100/128) + 1 */
        *shaperToPortRateRatioPtr = (value*100)/128 + 1;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPfcCascadeEnableSet
*
* DESCRIPTION:
*       Enable/Disable insertion of DSA tag for PFC frames 
*       transmitted on given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*       enable     - GT_TRUE - PFC packets are transmitted with a DSA tag.
*                    GT_FALSE - PFC packets are transmitted without DSA tag.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPfcCascadeEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 fieldOffset;/* the start bit number in the register */
    GT_U32 value;      /* register value */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    value = BOOL2BIT_MAC(enable);

#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif

    
        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
            fieldOffset = 15;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
            fieldOffset = 3;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            /* set PFC Cascade Port Enable */
            rc =  prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                                fieldOffset, 1, value);
            if (rc != GT_OK) 
            {
                    return rc;
            }
        }
#ifdef __AX_PLATFORM__
    }
#endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcForwardEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of PFC frames to the ingress 
*       pipeline of a specified port. Processing of unknown MAC control frames 
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - device number.
*       portNum - physical port number (CPU port not supported)
*       enable  - GT_TRUE:  forward PFC frames to the ingress pipe,
*                 GT_FALSE: do not forward PFC frames to the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcForwardEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    value = BOOL2BIT_MAC(enable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters1;
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 1, 1, value);
}


/*******************************************************************************
* cpssDxChPortPfcForwardEnableGet
*
* DESCRIPTION:
*       Get status of PFC frames forwarding
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr   - current forward PFC frames status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcForwardEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters1;
    if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 1, 1, &value)) != GT_OK)
        return rc;
    
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

