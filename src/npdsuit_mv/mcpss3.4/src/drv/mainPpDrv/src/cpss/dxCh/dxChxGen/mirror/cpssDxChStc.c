/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPmStc.c
*
* DESCRIPTION:
*       CPSS DxCh Sampling To CPU (STC) API implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChStc.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>

/*******************************************************************************
* prvCpssDxChStcBusyWait
*
* DESCRIPTION:
*       Local busy wait function, for Control STC, to check that the previous
*       action was finished.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       portGroupId        - the portGroupId , to support multi-port-groups device
*       regAddr       - Ingress STC Control Access Register address
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChStcBusyWait
(
    IN GT_U8                                devNum,
    IN GT_U32                               portGroupId,
    IN GT_U32                               regAddr
)
{
    /* wait for bit 0 to clear */
    return prvCpssDxChPortGroupBusyWait(devNum,portGroupId,regAddr,0,GT_FALSE);
}

/*******************************************************************************
* prvCpssDxChStcAccessIngrTblSet
*
* DESCRIPTION:
*               Local auxiliary function
*               Write into HW statistical rate limit table
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*       accessAction  - read/write table entry
*       ingrStcLimitPtr  - pointer for new sFlow sample threshold
*                       may be NULL if not needed
*       ingrStcSampledCountPtr - pointer for current number of sFlow sampled packets
*
* OUTPUTS:
*       ingrStcLimitPtr   - pointer for current sFlow sample threshold
*                        may be NULL if not needed
*       ingrStcSampledCountPtr - pointer for current number of sFlow sampled packets
*       ingrStcCounterPtr - pointer for current value of countdown sample threshold counter
*       ingValRdyPtr      - pointer for the new value existance
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/

static GT_STATUS prvCpssDxChStcAccessIngrTblSet
(
    IN GT_U8                                devNum,
    IN GT_U8                                portNum,
    IN PRV_CPSS_DXCH_ACCESS_ACTION_ENT      accessAction,
    INOUT GT_U32                            *ingrStcLimitPtr,
    INOUT GT_U32                            *ingrStcSampledCountPtr,
    OUT GT_U32                              *ingrStcCounterPtr,
    OUT GT_U32                              *ingValRdyPtr
)
{
    GT_U32      ingStcTblAccessCtrlReg;
    GT_U32      ingStcTblWord0ValueReg;
    GT_U32      ingStcTblWord1ValueReg; /* read only */
    GT_U32      ingStcTblWord2ValueReg;
    GT_U32      controlValue, dataValue;
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    ingStcTblAccessCtrlReg = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
        ingStcTblAccessCtrlReg;
    ingStcTblWord0ValueReg = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
        ingStcTblWord0ValueReg;
    ingStcTblWord1ValueReg = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
        ingStcTblWord1ValueReg;
    ingStcTblWord2ValueReg = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
        ingStcTblWord2ValueReg;

    rc = prvCpssDxChStcBusyWait(devNum,portGroupId, ingStcTblAccessCtrlReg);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set the read/write trigger bit */
    controlValue = 1;

    if (accessAction == PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E)
    {
        if ((ingrStcLimitPtr == NULL) && (ingrStcSampledCountPtr == NULL))
            return GT_FAIL;

        controlValue |= (1 << 1);
        controlValue |= (localPort << 2);

        if (ingrStcLimitPtr != NULL)
        {
            controlValue |= (1 << 7);

            dataValue = *ingrStcLimitPtr;
            dataValue |= (1 << 30); /* IngressSTCNewLimValRdy */
            rc  = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,ingStcTblWord0ValueReg,
                                              dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if (ingrStcSampledCountPtr != NULL)
        {
            controlValue |= (1 << 8);

            dataValue = *ingrStcSampledCountPtr;
            rc  = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,ingStcTblWord2ValueReg,
                                              dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* non-direct table --- do the write action */
        rc  = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,ingStcTblAccessCtrlReg,
                                          controlValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* busy wait for the control reg to show "write was done" */
        rc = prvCpssDxChStcBusyWait(devNum, portGroupId,ingStcTblAccessCtrlReg);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    else
    {
        if ((ingrStcLimitPtr == NULL) && (ingrStcSampledCountPtr == NULL) &&
            (ingrStcCounterPtr == NULL))
            return GT_FAIL;

        controlValue |= (localPort << 2);

        /* non-direct table --- do the read action */
        rc  = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,ingStcTblAccessCtrlReg,
                                          controlValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* busy wait for the control reg to show "read was done" */
        rc = prvCpssDxChStcBusyWait(devNum, portGroupId,ingStcTblAccessCtrlReg);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (ingrStcLimitPtr != NULL)
        {
            rc  = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,ingStcTblWord0ValueReg,
                                             &dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            *ingrStcLimitPtr = dataValue & 0x3FFFFFFF;

            if(ingValRdyPtr != NULL)
            {
                *ingValRdyPtr    = (dataValue >> 30);
            }

        }

        if (ingrStcCounterPtr != NULL)
        {
            rc  = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,ingStcTblWord1ValueReg,
                                             &dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            *ingrStcCounterPtr = dataValue & 0x3FFFFFFF;
        }

        if (ingrStcSampledCountPtr != NULL)
        {
            rc  = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,ingStcTblWord2ValueReg,
                                             &dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            *ingrStcSampledCountPtr = dataValue & 0xFFFF;
        }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChStcIngressCountModeSet
*
* DESCRIPTION:
*       Set the type of packets subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       mode      - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E - All packets without
*                                                         any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcIngressCountModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_STC_COUNT_MODE_ENT    mode
)
{
    GT_U32      hwValue;        /* value to write to register */
    GT_U32      regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_DXCH_STC_COUNT_ALL_PACKETS_E:
            hwValue = 0;
            break;

        case CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E:
            hwValue = 1;
            break;

        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, hwValue);
}

/*******************************************************************************
* cpssDxChStcIngressCountModeGet
*
* DESCRIPTION:
*       Get the type of packets that are subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       modePtr   - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E = All packets without
*                                                           any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcIngressCountModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_STC_COUNT_MODE_ENT    *modePtr
)
{
    GT_U32      hwValue;        /* value to write to register   */
    GT_U32      regAddr;        /* register address             */
    GT_STATUS   rc;             /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* get Ingress STC Configuration register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &hwValue);

    *modePtr = (hwValue == 0) ? CPSS_DXCH_STC_COUNT_ALL_PACKETS_E :
                                CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E;

    return rc;
}

/*******************************************************************************
* cpssDxChStcReloadModeSet
*
* DESCRIPTION:
*       Set the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       mode      - Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,STC type or mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcReloadModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT                  stcType,
    IN  CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT     mode
)
{
    GT_U32      hwValue;        /* value to write to register */
    GT_U32      regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* get register address according to STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* get Ingress STC Configuration register address */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
            break;

        case CPSS_DXCH_STC_EGRESS_E:
            /* get Statistical and CPU-Triggered Egress Mirroring to
               Analyzer Port Configuration register address */
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                    statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
            }
            break;

        default:
            return GT_BAD_PARAM;
    }

    switch (mode)
    {
        case CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E:
            hwValue = 0;
            break;

        case CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E:
            hwValue = 1;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 1, 1, hwValue);
}

/*******************************************************************************
* cpssDxChStcReloadModeGet
*
* DESCRIPTION:
*       Get the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       modePtr   - (pointer to) Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcReloadModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT                  stcType,
    OUT CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT     *modePtr
)
{
    GT_U32      hwValue;        /* value to write to register   */
    GT_U32      regAddr;        /* register address             */
    GT_STATUS   rc;             /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* get register address according to STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* get Ingress STC Configuration register address */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
            break;

        case CPSS_DXCH_STC_EGRESS_E:
            /* get Statistical and CPU-Triggered Egress Mirroring to
               Analyzer Port Configuration register address */
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                    statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
            }
            break;

        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 1, 1, &hwValue);

    *modePtr = (hwValue == 0) ? CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E :
                                CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E;

    return rc;
}

/*******************************************************************************
* cpssDxChStcEnableSet
*
* DESCRIPTION:
*       Global Enable/Disable Sampling To CPU (STC).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       enable    - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_BOOL                     enable
)
{
    GT_U32      hwValue;        /* value to write to register   */
    GT_U32      regAddr;        /* register address             */
    GT_U32      bitOffset;      /* bit offset inside register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* get Ingress STC Configuration register address */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
            bitOffset = 2;
            break;

        case CPSS_DXCH_STC_EGRESS_E:
            /* get Statistical and CPU-Triggered Egress Mirroring to
               Analyzer Port Configuration register address */
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                    statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
            }
            bitOffset = 0;
            break;

        default:
            return GT_BAD_PARAM;
    }

    hwValue = BOOL2BIT_MAC(enable);

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, bitOffset, 1, hwValue);
}

/*******************************************************************************
* cpssDxChStcEnableGet
*
* DESCRIPTION:
*       Get the global status of Sampling To CPU (STC) (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       enablePtr - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcEnableGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *enablePtr
)
{

    GT_U32      hwValue;        /* value to write to register   */
    GT_U32      regAddr;        /* register address             */
    GT_U32      bitOffset;      /* bit offset inside register   */
    GT_STATUS   rc;             /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* get Ingress STC Configuration register address */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
            bitOffset = 2;
            break;

        case CPSS_DXCH_STC_EGRESS_E:
            /* get Statistical and CPU-Triggered Egress Mirroring to
               Analyzer Port Configuration register address */
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                    statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
            }
            bitOffset = 0;
            break;

        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, bitOffset, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChStcPortLimitSet
*
* DESCRIPTION:
*       Set Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       limit     - Count Down Limit (0 - 0x3FFFFFFF)
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - limit is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortLimitSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_U32                      limit
)
{
    GT_U32  hwValue;    /* value to write to register */
    GT_U32  regAddr;    /* register address           */
    GT_U32  regAddr2;   /* register address for Egress monitoring */
    GT_U32  bitOffset;  /* bit offset in the register */
    GT_BOOL mirrEnable; /* mirror enabling indication */
    GT_U32  mirrIndex;  /* mirror interface index */
    GT_STATUS   rc;     /* retrun code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    if (limit >= BIT_30)
    {
        return GT_OUT_OF_RANGE;
    }

    hwValue = (limit | (1 << 30));

    /* get register address according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* Indirect Table write */
            return prvCpssDxChStcAccessIngrTblSet(devNum, port,
                                             PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E,
                                             &limit,
                                             NULL, NULL, NULL);

        case CPSS_DXCH_STC_EGRESS_E:

            regAddr2 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trSrcSniff;
            bitOffset = port % 32;
    
            if(PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
            {
                if(port > 31)
                {
                    regAddr2 += 0x64;
                }
            }

            /* Get Egress mirroring state */
            rc = cpssDxChMirrorTxPortGet(devNum, port, &mirrEnable, &mirrIndex);
            if ( GT_OK != rc) 
            {
                return rc;
            }

            /* If Egress STC is enabled on port and Egress mirroring is not enabled on port, */
            /* enable Egress monitoring on port. */ 
            if( (limit > 0) && (GT_FALSE == mirrEnable) )
            {
                rc = prvCpssDrvHwPpSetRegField(devNum, regAddr2, bitOffset, 1, 1);
                if (GT_OK != rc)
                {
                    return rc;
                }
            }

            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word0 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord0[port];
                rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                     regAddr,hwValue);
            }
            else
            {
                rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            (GT_U32)port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            0,
                                            31,
                                            hwValue);
            }

            if (GT_OK != rc)
            {
                return rc;
            }

            /* If Egress STC is disabled on port and Egress mirroring is not enabled on port, */
            /* disable Egress monitoring on port. */ 
            if( (0 == limit) && (GT_FALSE == mirrEnable) )
            {
                rc = prvCpssDrvHwPpSetRegField(devNum, regAddr2, bitOffset, 1, 0);
                if (GT_OK != rc)
                {
                    return rc;
                }
            }
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChStcPortLimitGet
*
* DESCRIPTION:
*       Get Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       limitPtr  - pointer to Count Down Limit (0 - 0x3FFFFFFF)
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *limitPtr
)
{
    GT_U32                          regAddr;    /* register address           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(limitPtr);

    /* get register address according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* Indirect Table read */
            return prvCpssDxChStcAccessIngrTblSet(devNum, port,
                                             PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
                                             limitPtr,
                                             NULL, NULL, NULL);

        case CPSS_DXCH_STC_EGRESS_E:
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word0 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord0[port];
                return prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                        regAddr, 0, 30, limitPtr);
            }
            else
            {
                return prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            (GT_U32)port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            0,
                                            30,
                                            limitPtr);
            }

        default:
            return GT_BAD_PARAM;
    }

}

/*******************************************************************************
* cpssDxChStcPortReadyForNewLimitGet
*
* DESCRIPTION:
*       Check whether Sampling to CPU is ready to get new STC Limit Value per
*       port.
*       The function cpssDxChStcPortLimitSet sets new limit value.
*       But only after device finishes handling of new limit value the
*       cpssDxChStcPortLimitSet may be called once more.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       isReadyPtr - (pointer to) Is Ready value
*                    GT_TRUE = Sampling to CPU ready to get new STC Limit Value
*                    GT_FALSE = Sampling to CPU handles STC Limit Value yet and
*                               is not ready to get new value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortReadyForNewLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *isReadyPtr
)
{

    GT_U32                          regAddr;    /* register address           */
    GT_U32                          hwValue;    /* value to read from register */
    GT_U32                          limit = 0;
    GT_STATUS                       rc;         /* return code                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(isReadyPtr);

    /* get register address according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* Indirect Table read */
            rc = prvCpssDxChStcAccessIngrTblSet(devNum, port,
                                             PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
                                             &limit,
                                             NULL, NULL, &hwValue);
            *isReadyPtr = BIT2BOOL_MAC(1 - hwValue);

            return rc;

        case CPSS_DXCH_STC_EGRESS_E:
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word0 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord0[port];
                rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                        regAddr, 30, 1, &hwValue);
            }
            else
            {
                rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            (GT_U32)port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            30,
                                            1,
                                            &hwValue);
            }
            break;

        default:
            return GT_BAD_PARAM;
    }


    *isReadyPtr = BIT2BOOL_MAC(1 - hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChStcPortCountdownCntrGet
*
* DESCRIPTION:
*       Get STC Countdown Counter per port.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Count down counter. This is the number of
*                   packets left to send/receive in order that a packet will be
*                   sampled to CPU and a new value will be loaded.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortCountdownCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
)
{
    GT_U32                          regAddr;    /* register address           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(cntrPtr);

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* Indirect Table read */
            return prvCpssDxChStcAccessIngrTblSet(devNum, port,
                                             PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
                                             NULL,
                                             NULL, cntrPtr, NULL);

        case CPSS_DXCH_STC_EGRESS_E:
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word1 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord1[port];
                return  prvCpssDrvHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                         regAddr, 0, 30, cntrPtr);
            }
            else
            {
                return prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            (GT_U32)port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            32,
                                            30,
                                            cntrPtr);
            }

        default:
            return GT_BAD_PARAM;
    }

}

/*******************************************************************************
* cpssDxChStcPortSampledPacketsCntrSet
*
* DESCRIPTION:
*       Set the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       cntr      - STC Sampled to CPU packet's counter (0 - 0xFFFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_OUT_OF_RANGE          - on wrong cntr
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortSampledPacketsCntrSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_U32                      cntr
)
{
    GT_U32                          regAddr;    /* register address           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    if (cntr >= BIT_16)
    {
        return GT_OUT_OF_RANGE;
    }

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* Indirect Table write */
            return prvCpssDxChStcAccessIngrTblSet(devNum, port,
                                             PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E,
                                             NULL, &cntr, NULL, NULL);

        case CPSS_DXCH_STC_EGRESS_E:
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word2 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord2[port];
                return prvCpssDrvHwPpPortGroupSetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                         regAddr, 0, 16, cntr);
            }
            else
            {
                return prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            (GT_U32)port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            64,
                                            16,
                                            cntr);
            }

        default:
            return GT_BAD_PARAM;
    }

}

/*******************************************************************************
* cpssDxChStcPortSampledPacketsCntrGet
*
* DESCRIPTION:
*       Get the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Sampled to CPU packet's counter (0 - 0xFFFF).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortSampledPacketsCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
)
{

    GT_U32                          regAddr;    /* register address           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(cntrPtr);

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* Indirect Table read */
            return prvCpssDxChStcAccessIngrTblSet(devNum, port,
                                             PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
                                             NULL, cntrPtr, NULL, NULL);

        case CPSS_DXCH_STC_EGRESS_E:
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word2 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord2[port];
                return  prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                        regAddr, 0, 16, cntrPtr);
            }
            else
            {
                return prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            (GT_U32)port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            64,
                                            16,
                                            cntrPtr);
            }
        default:
            return GT_BAD_PARAM;
    }
}

