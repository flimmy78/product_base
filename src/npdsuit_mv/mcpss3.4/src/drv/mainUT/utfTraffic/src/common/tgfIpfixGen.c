/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfIpfixGen.c
*
* DESCRIPTION:
*       Generic API implementation for IPFIX
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/private/prvUtfHelpers.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfIpfixGen.h>

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChIpfixEntry
*
* DESCRIPTION:
*       Convert generic into device specific IPFIX entry
*
* INPUTS:
*       IpfixEntryPtr - (pointer to) policer entry
*
* OUTPUTS:
*       dxChEntryPtr - (pointer to) DxCh IPFIX entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIpfixEntry
(
    IN  PRV_TGF_IPFIX_ENTRY_STC    *ipfixEntryPtr,
    OUT CPSS_DXCH_IPFIX_ENTRY_STC  *dxChEntryPtr
)
{
    cpssOsMemSet(dxChEntryPtr, 0, sizeof(CPSS_DXCH_IPFIX_ENTRY_STC));

    /* convert entry info into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, timeStamp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, packetCount);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, byteCount);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, dropCounter);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, randomOffset);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, lastSampledValue);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, samplingWindow);

    switch(ipfixEntryPtr->samplingAction)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChEntryPtr->samplingAction, 
                                PRV_TGF_IPFIX_SAMPLING_ACTION_ALARM_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_ACTION_ALARM_E);
        PRV_TGF_SWITCH_CASE_MAC(dxChEntryPtr->samplingAction, 
                                PRV_TGF_IPFIX_SAMPLING_ACTION_MIRROR_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_ACTION_MIRROR_E);
        default:
            return GT_BAD_PARAM;
    }

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, logSamplingRange);

    switch(ipfixEntryPtr->randomFlag)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChEntryPtr->randomFlag, 
                                PRV_TGF_IPFIX_SAMPLING_DIST_DETERMINISTIC_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_DIST_DETERMINISTIC_E);
        PRV_TGF_SWITCH_CASE_MAC(dxChEntryPtr->randomFlag, 
                                PRV_TGF_IPFIX_SAMPLING_DIST_RANDOM_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_DIST_RANDOM_E);
        default:
            return GT_BAD_PARAM;
    }

    switch(ipfixEntryPtr->samplingMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChEntryPtr->samplingMode, 
                                PRV_TGF_IPFIX_SAMPLING_MODE_DISABLE_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_MODE_DISABLE_E);
        PRV_TGF_SWITCH_CASE_MAC(dxChEntryPtr->samplingMode, 
                                PRV_TGF_IPFIX_SAMPLING_MODE_PACKET_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_MODE_PACKET_E);
        PRV_TGF_SWITCH_CASE_MAC(dxChEntryPtr->samplingMode, 
                                PRV_TGF_IPFIX_SAMPLING_MODE_BYTE_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_MODE_BYTE_E);
        PRV_TGF_SWITCH_CASE_MAC(dxChEntryPtr->samplingMode, 
                                PRV_TGF_IPFIX_SAMPLING_MODE_TIME_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_MODE_TIME_E);
        default:
            return GT_BAD_PARAM;
    }

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, cpuSubCode);


    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericIpfixEntry
*
* DESCRIPTION:
*       Convert device specific IPFIX entry into generic
*
* INPUTS:
*       dxChEntryPtr - (pointer to) DxCh IPFIX entry parameters
*
* OUTPUTS:
*       IpfixEntryPtr - (pointer to) policer entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericIpfixEntry
(
    IN CPSS_DXCH_IPFIX_ENTRY_STC  *dxChEntryPtr,
    OUT PRV_TGF_IPFIX_ENTRY_STC    *ipfixEntryPtr
    
)
{
    cpssOsMemSet(ipfixEntryPtr, 0, sizeof(PRV_TGF_IPFIX_ENTRY_STC));

    /* convert entry info into generic format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, timeStamp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, packetCount);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, byteCount);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, dropCounter);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, randomOffset);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, lastSampledValue);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, samplingWindow);

    switch(dxChEntryPtr->samplingAction)
    {
        PRV_TGF_SWITCH_CASE_MAC(ipfixEntryPtr->samplingAction, 
                                PRV_TGF_IPFIX_SAMPLING_ACTION_ALARM_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_ACTION_ALARM_E);
        PRV_TGF_SWITCH_CASE_MAC(ipfixEntryPtr->samplingAction, 
                                PRV_TGF_IPFIX_SAMPLING_ACTION_MIRROR_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_ACTION_MIRROR_E);
        default:
            return GT_BAD_PARAM;
    }

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, logSamplingRange);

    switch(dxChEntryPtr->randomFlag)
    {
        PRV_TGF_SWITCH_CASE_MAC(ipfixEntryPtr->randomFlag, 
                                PRV_TGF_IPFIX_SAMPLING_DIST_DETERMINISTIC_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_DIST_DETERMINISTIC_E);
        PRV_TGF_SWITCH_CASE_MAC(ipfixEntryPtr->randomFlag, 
                                PRV_TGF_IPFIX_SAMPLING_DIST_RANDOM_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_DIST_RANDOM_E);
        default:
            return GT_BAD_PARAM;
    }

    switch(dxChEntryPtr->samplingMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(ipfixEntryPtr->samplingMode, 
                                PRV_TGF_IPFIX_SAMPLING_MODE_DISABLE_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_MODE_DISABLE_E);
        PRV_TGF_SWITCH_CASE_MAC(ipfixEntryPtr->samplingMode, 
                                PRV_TGF_IPFIX_SAMPLING_MODE_PACKET_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_MODE_PACKET_E);
        PRV_TGF_SWITCH_CASE_MAC(ipfixEntryPtr->samplingMode, 
                                PRV_TGF_IPFIX_SAMPLING_MODE_BYTE_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_MODE_BYTE_E);
        PRV_TGF_SWITCH_CASE_MAC(ipfixEntryPtr->samplingMode, 
                                PRV_TGF_IPFIX_SAMPLING_MODE_TIME_E, 
                                CPSS_DXCH_IPFIX_SAMPLING_MODE_TIME_E);
        default:
            return GT_BAD_PARAM;
    }

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, ipfixEntryPtr, cpuSubCode);


    return GT_OK;
}
#endif

/*******************************************************************************
* prvTgfIpfixEntrySet
*
* DESCRIPTION:
*       Sets IPFIX entry.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of IPFIX Entry.
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
*
*******************************************************************************/
GT_STATUS prvTgfIpfixEntrySet
(
    IN  GT_U8                             devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                            entryIndex,
    IN  PRV_TGF_IPFIX_ENTRY_STC           *ipfixEntryPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_IPFIX_ENTRY_STC           dxChEntry;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;

    /* convert policer entry into device specific format */
    rc = prvTgfConvertGenericToDxChIpfixEntry(ipfixEntryPtr, &dxChEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIpfixEntry FAILED, rc = [%d]", rc);

        return rc;
    }

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    rc = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerCountingWriteBackCacheFlush FAILED, rc = [%d]", rc);
    
        return rc;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxChIpfixEntrySet(devNum, dxChStage, entryIndex, &dxChEntry); 
    }
    else
    {
        rc = cpssDxChIpfixPortGroupEntrySet(devNum, currPortGroupsBmp, dxChStage, entryIndex, &dxChEntry);
    } 
    
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixEntrySet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixEntryGet
*
* DESCRIPTION:
*       Gets IPFIX entry.
*
*  INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex  - index of IPFIX Entry.
*       reset       - reset flag:
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
*
*******************************************************************************/
GT_STATUS prvTgfIpfixEntryGet
(
    IN  GT_U8                             devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                            entryIndex,
    IN  GT_BOOL                           reset,
    OUT PRV_TGF_IPFIX_ENTRY_STC           *ipfixEntryPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_IPFIX_ENTRY_STC           dxChEntry;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxChIpfixEntryGet(devNum, dxChStage, entryIndex, reset, &dxChEntry);
    }
    else
    {
        rc = cpssDxChIpfixPortGroupEntryGet(devNum, currPortGroupsBmp, dxChStage, entryIndex, reset, &dxChEntry);
    } 
    
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixEntryGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    /* convert policer entry into generic format */
    rc = prvTgfConvertDxChToGenericIpfixEntry(&dxChEntry, ipfixEntryPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertDxChToGenericIpfixEntry FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixDropCountModeSet
*
* DESCRIPTION:
*       Configures IPFIX drop counting mode.
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
GT_STATUS prvTgfIpfixDropCountModeSet
(
    IN GT_U8                                devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN PRV_TGF_IPFIX_DROP_COUNT_MODE_ENT    mode
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT dxChMode;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    switch(mode)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChMode, 
                                PRV_TGF_IPFIX_DROP_COUNT_MODE_METER_ONLY_E, 
                                CPSS_DXCH_IPFIX_DROP_COUNT_MODE_METER_ONLY_E);
        PRV_TGF_SWITCH_CASE_MAC(dxChMode, 
                                PRV_TGF_IPFIX_DROP_COUNT_MODE_ALL_E, 
                                CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ALL_E);
        default:
            return GT_BAD_PARAM;
    }
    
    /* call device specific API */
    rc = cpssDxChIpfixDropCountModeSet(devNum, dxChStage, dxChMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixDropCountModeSet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixDropCountModeGet
*
* DESCRIPTION:
*       Gets IPFIX drop counting mode.
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
GT_STATUS prvTgfIpfixDropCountModeGet
(
    IN GT_U8                                  devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    OUT PRV_TGF_IPFIX_DROP_COUNT_MODE_ENT     *modePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT dxChMode;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChIpfixDropCountModeGet(devNum, dxChStage, &dxChMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixDropCountModeGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    switch(dxChMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(*modePtr,
                                CPSS_DXCH_IPFIX_DROP_COUNT_MODE_METER_ONLY_E,
                                PRV_TGF_IPFIX_DROP_COUNT_MODE_METER_ONLY_E);
        PRV_TGF_SWITCH_CASE_MAC(*modePtr,
                                CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ALL_E,
                                PRV_TGF_IPFIX_DROP_COUNT_MODE_ALL_E);
        default:
            return GT_BAD_PARAM;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixCpuCodeSet
*
* DESCRIPTION:
*       Sets the 6 most significant bits of the CPU Code in case of mirroring. 
*       The two least significant bits are taken from the IPfix entry.
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
GT_STATUS prvTgfIpfixCpuCodeSet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN CPSS_NET_RX_CPU_CODE_ENT       cpuCode
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChIpfixCpuCodeSet(devNum, dxChStage, cpuCode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixCpuCodeSet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixCpuCodeGet
*
* DESCRIPTION:
*       Gets the 6 most significant bits of the CPU Code in case of mirroring. 
*       The two least significant bits are taken from the IPfix entry.
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
GT_STATUS prvTgfIpfixCpuCodeGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT CPSS_NET_RX_CPU_CODE_ENT      *cpuCodePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChIpfixCpuCodeGet(devNum, dxChStage, cpuCodePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixCpuCodeGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixAgingEnableSet
*
* DESCRIPTION:
*       Enables or disabled the activation of aging for IPfix.
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
GT_STATUS prvTgfIpfixAgingEnableSet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_BOOL                        enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChIpfixAgingEnableSet(devNum, dxChStage, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixAgingEnableSet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixAgingEnableGet
*
* DESCRIPTION:
*       Gets enabling status of aging for IPfix.
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
GT_STATUS prvTgfIpfixAgingEnableGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_BOOL                       *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChIpfixAgingEnableGet(devNum, dxChStage, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixAgingEnableGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixWraparoundConfSet
*
* DESCRIPTION:
*       Configures IPFIX wraparound parameters.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       confPtr   - pointer to wraparound configuration structure.
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
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixWraparoundConfSet
(
    IN GT_U8                                  devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    IN PRV_TGF_IPFIX_WRAPAROUND_CFG_STC       *confPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC  dxChConf, *dxChConfPtr;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    dxChConfPtr = &dxChConf;

    switch(confPtr->action)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChConfPtr->action, 
                                PRV_TGF_IPFIX_WRAPAROUND_ACTION_NONE_E, 
                                CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_NONE_E);
        PRV_TGF_SWITCH_CASE_MAC(dxChConfPtr->action, 
                                PRV_TGF_IPFIX_WRAPAROUND_ACTION_CLEAR_E, 
                                CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_CLEAR_E);
        default:
            return GT_BAD_PARAM;
    }
    
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChConfPtr, confPtr, dropThreshold);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChConfPtr, confPtr, packetThreshold);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChConfPtr, confPtr, byteThreshold);

    /* call device specific API */
    rc = cpssDxChIpfixWraparoundConfSet(devNum, dxChStage, &dxChConf);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixWraparoundConfSet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixWraparoundConfGet
*
* DESCRIPTION:
*       Get IPFIX wraparound configuration.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       confPtr   - pointer to wraparound configuration structure.
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
GT_STATUS prvTgfIpfixWraparoundConfGet
(
    IN GT_U8                                  devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    OUT PRV_TGF_IPFIX_WRAPAROUND_CFG_STC      *confPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC  dxChConf, *dxChConfPtr;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChIpfixWraparoundConfGet(devNum, dxChStage, &dxChConf);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixWraparoundConfGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    dxChConfPtr = &dxChConf;
    switch(dxChConfPtr->action)
    {
        PRV_TGF_SWITCH_CASE_MAC(confPtr->action,
                                CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_NONE_E,
                                PRV_TGF_IPFIX_WRAPAROUND_ACTION_NONE_E);
        PRV_TGF_SWITCH_CASE_MAC(confPtr->action,
                                CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_CLEAR_E,
                                PRV_TGF_IPFIX_WRAPAROUND_ACTION_CLEAR_E);
        default:
            return GT_BAD_PARAM;
    }
    
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChConfPtr, confPtr, dropThreshold);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChConfPtr, confPtr, packetThreshold);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChConfPtr, confPtr, byteThreshold);

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixWraparoundStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of wraparound entries indexes.
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
GT_STATUS prvTgfIpfixWraparoundStatusGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                         startIndex,
    IN GT_U32                         endIndex,
    IN GT_BOOL                        reset,
    OUT GT_U32                        *bmpPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxChIpfixWraparoundStatusGet(devNum, dxChStage, startIndex, endIndex, reset, bmpPtr); 
    }
    else
    {
        rc = cpssDxChIpfixPortGroupWraparoundStatusGet(devNum, currPortGroupsBmp, dxChStage, startIndex, endIndex, reset, bmpPtr); 
    } 
    
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixWraparoundStatusGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixAgingStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of aged entries indexes.
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
GT_STATUS prvTgfIpfixAgingStatusGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                         startIndex,
    IN GT_U32                         endIndex,
    IN GT_BOOL                        reset,
    OUT GT_U32                        *bmpPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxChIpfixAgingStatusGet(devNum, dxChStage, startIndex, endIndex, reset, bmpPtr);
    }
    else
    {
        rc = cpssDxChIpfixPortGroupAgingStatusGet(devNum, currPortGroupsBmp, dxChStage, startIndex, endIndex, reset, bmpPtr);
    }
    
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixAgingStatusGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixTimestampUploadSet
*
* DESCRIPTION:
*       Configures IPFIX timestamp upload per stages.
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
*       "prvTgfIpfixTimestampUploadTrigger".
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimestampUploadSet
(
    IN GT_U8                                      devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    IN PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_CONF_STC    *uploadPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC  dxChUpload, *dxChUploadPtr;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    dxChUploadPtr = &dxChUpload;

    switch(uploadPtr->uploadMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChUploadPtr->uploadMode, 
                                PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E, 
                                CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E);
        PRV_TGF_SWITCH_CASE_MAC(dxChUploadPtr->uploadMode, 
                                PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E, 
                                CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E);
        default:
            return GT_BAD_PARAM;
    }
    
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChUploadPtr, uploadPtr, timer.nanoSecondTimer);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChUploadPtr, uploadPtr, timer.secondTimer);
    
    /* call device specific API */
    rc = cpssDxChIpfixTimestampUploadSet(devNum, dxChStage, dxChUploadPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixTimestampUploadSet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixTimestampUploadGet
*
* DESCRIPTION:
*       Gets IPFIX timestamp upload configuration for a stage.
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
GT_STATUS prvTgfIpfixTimestampUploadGet
(
    IN GT_U8                                      devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    OUT PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_CONF_STC   *uploadPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC  dxChUpload, *dxChUploadPtr;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    dxChUploadPtr = &dxChUpload;
    /* call device specific API */
    rc = cpssDxChIpfixTimestampUploadGet(devNum, dxChStage, dxChUploadPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixTimestampUploadGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    switch(dxChUploadPtr->uploadMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(uploadPtr->uploadMode,
                                CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E,
                                PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E);
        PRV_TGF_SWITCH_CASE_MAC(uploadPtr->uploadMode,
                                CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E,
                                PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E);
        default:
            return GT_BAD_PARAM;
    }
    
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChUploadPtr, uploadPtr, timer.nanoSecondTimer);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChUploadPtr, uploadPtr, timer.secondTimer);
    
    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixTimestampUploadTrigger
*
* DESCRIPTION:
*       Triggers the IPFIX timestamp upload operation.
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
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimestampUploadTrigger
(
    IN GT_U8    devNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    /* call device specific API */
    rc = cpssDxChIpfixTimestampUploadTrigger(devNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixTimestampUploadTrigger FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixTimestampUploadStatusGet
*
* DESCRIPTION:
*       Gets IPFIX timestamp upload operation status.
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
GT_STATUS prvTgfIpfixTimestampUploadStatusGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *uploadStatusPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    /* call device specific API */
    rc = cpssDxChIpfixTimestampUploadStatusGet(devNum, uploadStatusPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixTimestampUploadStatusGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixTimerGet
*
* DESCRIPTION:
*       Gets IPFIX timer
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
GT_STATUS prvTgfIpfixTimerGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT PRV_TGF_IPFIX_TIMER_STC       *timerPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_IPFIX_TIMER_STC  dxChTimer, *dxChTimerPtr;

    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    dxChTimerPtr = &dxChTimer;
    /* call device specific API */
    rc = cpssDxChIpfixTimerGet(devNum, dxChStage, dxChTimerPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixTimerGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTimerPtr, timerPtr, nanoSecondTimer);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTimerPtr, timerPtr, secondTimer);

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixAlarmEventsGet
*
* DESCRIPTION:
*       Retrieves up to 16 IPFIX entries indexes where alarm events (due to 
*       sampling) occured.
*
* APPLICABLE DEVICES: DxChXcat and above.
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
GT_STATUS prvTgfIpfixAlarmEventsGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_U32                        eventsArr[PRV_TGF_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS],
    OUT GT_U32                        *eventsNumPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxChIpfixAlarmEventsGet(devNum, dxChStage, eventsArr, eventsNumPtr);
    }
    else
    {
        rc = cpssDxChIpfixPortGroupAlarmEventsGet(devNum, currPortGroupsBmp, dxChStage, eventsArr, eventsNumPtr);
    } 
    
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixAlarmEventsGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixTimestampToCpuEnableSet
*
* DESCRIPTION:
*       Configures all TO_CPU DSA tags to include a Timestamp.
*
* APPLICABLE DEVICES: DxChXcat and above.
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
GT_STATUS prvTgfIpfixTimestampToCpuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    
    /* call device specific API */
    rc = cpssDxChIpfixTimestampToCpuEnableSet(devNum, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixTimestampToCpuEnableSet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfIpfixTimestampToCpuEnableGet
*
* DESCRIPTION:
*       Gets enabling status of TO_CPU DSA tags including a timestamp.
*
* APPLICABLE DEVICES: DxChXcat and above.
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
GT_STATUS prvTgfIpfixTimestampToCpuEnableGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    
    /* call device specific API */
    rc = cpssDxChIpfixTimestampToCpuEnableGet(devNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpfixTimestampToCpuEnableGet FAILED, rc = [%d]", rc);
    
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}


