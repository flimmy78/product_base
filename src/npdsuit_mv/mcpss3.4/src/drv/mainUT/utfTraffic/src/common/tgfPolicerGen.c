/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfPolicerGen.c
*
* DESCRIPTION:
*       Generic API for Ingress Policing Engine API.
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
#include <common/tgfPolicerGen.h>

#ifdef ASIC_SIMULATION
/* #include <asicSimulation/SKernel/smain/smain.h> */
extern GT_STATUS skernelPolicerConformanceLevelForce(
    IN  GT_U32      dp
);
#endif

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxCh3/policer/private/prvCpssDxCh3Policer.h>
#endif /*CHX_FAMILY*/

/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChPolicerEntry
*
* DESCRIPTION:
*       Convert generic into device specific policer entry
*
* INPUTS:
*       policerEntryPtr - (pointer to) policer entry
*
* OUTPUTS:
*       dxChEntryPtr - (pointer to) DxCh policer entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChPolicerEntry
(
    IN  PRV_TGF_POLICER_ENTRY_STC    *policerEntryPtr,
    OUT CPSS_DXCH_POLICER_ENTRY_STC  *dxChEntryPtr
)
{
    /* convert entry info into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, policerEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, meterColorMode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, counterEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, counterSetIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, qosProfile);

    /* convert command into device specific format */
    switch (policerEntryPtr->cmd)
    {
        case PRV_TGF_POLICER_CMD_NONE_E:
            dxChEntryPtr->cmd = CPSS_DXCH_POLICER_CMD_NONE_E;
            break;

        case PRV_TGF_POLICER_CMD_DROP_RED_E:
            dxChEntryPtr->cmd = CPSS_DXCH_POLICER_CMD_DROP_RED_E;
            break;

        case PRV_TGF_POLICER_CMD_QOS_MARK_BY_ENTRY_E:
            dxChEntryPtr->cmd = CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E;
            break;

        case PRV_TGF_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E:
            dxChEntryPtr->cmd = CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyDscp into device specific format */
    dxChEntryPtr->modifyDscp = (GT_TRUE == policerEntryPtr->modifyQosParams.modifyDscp) ?
                                CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E :
                                CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;

    /* convert modifyUp into device specific format */
    dxChEntryPtr->modifyUp = (GT_TRUE == policerEntryPtr->modifyQosParams.modifyUp) ?
                              CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E :
                              CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;

    /* convert tbParams into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEntryPtr->tbParams),
                                   &(policerEntryPtr->tbParams.srTcmParams), cir);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEntryPtr->tbParams),
                                   &(policerEntryPtr->tbParams.srTcmParams), cbs);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericPolicerEntry
*
* DESCRIPTION:
*       Convert generic from device specific policer entry
*
* INPUTS:
*       dxChEntryPtr - (pointer to) DxCh policer entry parameters
*
* OUTPUTS:
*       policerEntryPtr - (pointer to) policer entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericPolicerEntry
(
    IN  CPSS_DXCH_POLICER_ENTRY_STC  *dxChEntryPtr,
    OUT PRV_TGF_POLICER_ENTRY_STC    *policerEntryPtr

)
{
    /* convert entry info from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, policerEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, meterColorMode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, counterEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, counterSetIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, qosProfile);

    /* convert command from device specific format */
    switch (dxChEntryPtr->cmd)
    {
        case CPSS_DXCH_POLICER_CMD_NONE_E:
            policerEntryPtr->cmd = PRV_TGF_POLICER_CMD_NONE_E;
            break;

        case CPSS_DXCH_POLICER_CMD_DROP_RED_E:
            policerEntryPtr->cmd = PRV_TGF_POLICER_CMD_DROP_RED_E;
            break;

        case CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E:
            policerEntryPtr->cmd = PRV_TGF_POLICER_CMD_QOS_MARK_BY_ENTRY_E;
            break;

        case CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E:
            policerEntryPtr->cmd = PRV_TGF_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyDscp into device specific format */
    switch (dxChEntryPtr->modifyDscp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            policerEntryPtr->modifyQosParams.modifyDscp = GT_TRUE;
            break;

        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            policerEntryPtr->modifyQosParams.modifyDscp = GT_FALSE;
            break;

        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyUp into device specific format */
    switch (dxChEntryPtr->modifyUp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            policerEntryPtr->modifyQosParams.modifyUp = GT_TRUE;
            break;

        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            policerEntryPtr->modifyQosParams.modifyUp = GT_FALSE;
            break;

        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tbParams into device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEntryPtr->tbParams),
                                   &(policerEntryPtr->tbParams.srTcmParams), cir);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEntryPtr->tbParams),
                                   &(policerEntryPtr->tbParams.srTcmParams), cbs);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxCh3PolicerMeteringEntry
*
* DESCRIPTION:
*       Convert generic into device specific policer entry
*
* INPUTS:
*       policerEntryPtr - (pointer to) policer entry
*
* OUTPUTS:
*       dxChEntryPtr - (pointer to) DxCh policer entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxCh3PolicerMeteringEntry
(
    IN  PRV_TGF_POLICER_ENTRY_STC             *policerEntryPtr,
    OUT CPSS_DXCH3_POLICER_METERING_ENTRY_STC *dxChEntryPtr
)
{
    /* convert entry info into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, countingEntryIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, meterColorMode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, modifyUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, modifyDscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, modifyDp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, qosProfile);

    /* convert mngCounterSet into device specific format */
    switch (policerEntryPtr->mngCounterSet)
    {
        case PRV_TGF_POLICER_MNG_CNTR_SET0_E:
            dxChEntryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET1_E:
            dxChEntryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET2_E:
            dxChEntryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_DISABLED_E:
            dxChEntryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert meterMode into device specific format */
    switch (policerEntryPtr->meterMode)
    {
        case PRV_TGF_POLICER_METER_MODE_SR_TCM_E:
            dxChEntryPtr->meterMode = CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;

            /* convert tbParams into device specific format */
            dxChEntryPtr->tokenBucketParams.srTcmParams.cir =
                  policerEntryPtr->tbParams.srTcmParams.cir;
            dxChEntryPtr->tokenBucketParams.srTcmParams.cbs =
                  policerEntryPtr->tbParams.srTcmParams.cbs;
            dxChEntryPtr->tokenBucketParams.srTcmParams.ebs =
                  policerEntryPtr->tbParams.srTcmParams.ebs;

            break;

        case PRV_TGF_POLICER_METER_MODE_TR_TCM_E:
            dxChEntryPtr->meterMode = CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E;

            /* convert tbParams into device specific format */
            dxChEntryPtr->tokenBucketParams.trTcmParams.cir =
                  policerEntryPtr->tbParams.trTcmParams.cir;
            dxChEntryPtr->tokenBucketParams.trTcmParams.cbs =
                  policerEntryPtr->tbParams.trTcmParams.cbs;
            dxChEntryPtr->tokenBucketParams.trTcmParams.pir =
                  policerEntryPtr->tbParams.trTcmParams.pir;
            dxChEntryPtr->tokenBucketParams.trTcmParams.pbs =
                  policerEntryPtr->tbParams.trTcmParams.pbs;

            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert yellowPcktCmd into device specific format */
    switch (policerEntryPtr->yellowPcktCmd)
    {
        case PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
            dxChEntryPtr->yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
            break;

        case PRV_TGF_POLICER_NON_CONFORM_CMD_DROP_E:
            dxChEntryPtr->yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
            break;

        case PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_E:
            dxChEntryPtr->yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E;
            break;

        case PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E:
            dxChEntryPtr->yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert redPcktCmd into device specific format */
    switch (policerEntryPtr->redPcktCmd)
    {
        case PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
            dxChEntryPtr->redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
            break;

        case PRV_TGF_POLICER_NON_CONFORM_CMD_DROP_E:
            dxChEntryPtr->redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
            break;

        case PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_E:
            dxChEntryPtr->redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E;
            break;

        case PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E:
            dxChEntryPtr->redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert remarkMode into device specific format */
    switch (policerEntryPtr->remarkMode)
    {
        case PRV_TGF_POLICER_REMARK_MODE_L2_E:
            dxChEntryPtr->remarkMode = CPSS_DXCH_POLICER_REMARK_MODE_L2_E;
            break;

        case PRV_TGF_POLICER_REMARK_MODE_L3_E:
            dxChEntryPtr->remarkMode = CPSS_DXCH_POLICER_REMARK_MODE_L3_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxCh3ToGenericPolicerEntry
*
* DESCRIPTION:
*       Convert generic from device specific policer entry
*
* INPUTS:
*       dxChEntryPtr - (pointer to) DxCh policer entry parameters
*
* OUTPUTS:
*       policerEntryPtr - (pointer to) policer entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxCh3ToGenericPolicerEntry
(
    IN  CPSS_DXCH3_POLICER_METERING_ENTRY_STC *dxChEntryPtr,
    OUT PRV_TGF_POLICER_ENTRY_STC             *policerEntryPtr
)
{
    /* convert entry info from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, countingEntryIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, meterColorMode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, modifyUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, modifyDscp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, modifyDp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEntryPtr, policerEntryPtr, qosProfile);

    /* convert mngCounterSet from device specific format */
    switch (dxChEntryPtr->mngCounterSet)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E:
            policerEntryPtr->mngCounterSet = PRV_TGF_POLICER_MNG_CNTR_SET0_E;
            break;

        case CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E:
            policerEntryPtr->mngCounterSet = PRV_TGF_POLICER_MNG_CNTR_SET1_E;
            break;

        case CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E:
            policerEntryPtr->mngCounterSet = PRV_TGF_POLICER_MNG_CNTR_SET2_E;
            break;

        case CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E:
            policerEntryPtr->mngCounterSet = PRV_TGF_POLICER_MNG_CNTR_DISABLED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert meterMode from device specific format */
    switch (dxChEntryPtr->meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            policerEntryPtr->meterMode = PRV_TGF_POLICER_METER_MODE_SR_TCM_E;

            /* convert tbParams from device specific format */
            policerEntryPtr->tbParams.srTcmParams.cir =
                  dxChEntryPtr->tokenBucketParams.srTcmParams.cir;
            policerEntryPtr->tbParams.srTcmParams.cbs =
                  dxChEntryPtr->tokenBucketParams.srTcmParams.cbs;
            policerEntryPtr->tbParams.srTcmParams.ebs =
                  dxChEntryPtr->tokenBucketParams.srTcmParams.ebs;

            break;

        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
            policerEntryPtr->meterMode = PRV_TGF_POLICER_METER_MODE_TR_TCM_E;

            /* convert tbParams from device specific format */
            policerEntryPtr->tbParams.trTcmParams.cir =
                  dxChEntryPtr->tokenBucketParams.trTcmParams.cir;
            policerEntryPtr->tbParams.trTcmParams.cbs =
                  dxChEntryPtr->tokenBucketParams.trTcmParams.cbs;
            policerEntryPtr->tbParams.trTcmParams.pir =
                  dxChEntryPtr->tokenBucketParams.trTcmParams.pir;
            policerEntryPtr->tbParams.trTcmParams.pbs =
                  dxChEntryPtr->tokenBucketParams.trTcmParams.pbs;

            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert yellowPcktCmd from device specific format */
    switch (dxChEntryPtr->yellowPcktCmd)
    {
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
            policerEntryPtr->yellowPcktCmd = PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
            break;

        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E:
            policerEntryPtr->yellowPcktCmd = PRV_TGF_POLICER_NON_CONFORM_CMD_DROP_E;
            break;

        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E:
            policerEntryPtr->yellowPcktCmd = PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_E;
            break;

        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E:
            policerEntryPtr->yellowPcktCmd = PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert redPcktCmd from device specific format */
    switch (dxChEntryPtr->redPcktCmd)
    {
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
            policerEntryPtr->redPcktCmd = PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
            break;

        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E:
            policerEntryPtr->redPcktCmd = PRV_TGF_POLICER_NON_CONFORM_CMD_DROP_E;
            break;

        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E:
            policerEntryPtr->redPcktCmd = PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_E;
            break;

        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E:
            policerEntryPtr->redPcktCmd = PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert remarkMode from device specific format */
    switch (dxChEntryPtr->remarkMode)
    {
        case CPSS_DXCH_POLICER_REMARK_MODE_L2_E:
            policerEntryPtr->remarkMode = PRV_TGF_POLICER_REMARK_MODE_L2_E;
            break;

        case CPSS_DXCH_POLICER_REMARK_MODE_L3_E:
            policerEntryPtr->remarkMode = PRV_TGF_POLICER_REMARK_MODE_L3_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChPolicerStage
*
* DESCRIPTION:
*       Convert generic into device specific policer stage
*
* INPUTS:
*       policerStage - (pointer to) policer stage
*
* OUTPUTS:
*       dxChStagePtr - (pointer to) DxCh policer stage
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToDxChPolicerStage
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT      policerStage,
    OUT CPSS_DXCH_POLICER_STAGE_TYPE_ENT    *dxChStagePtr
)
{
    /* convert command into device specific format */
    switch (policerStage)
    {
        case PRV_TGF_POLICER_STAGE_INGRESS_0_E:
            *dxChStagePtr = CPSS_DXCH_POLICER_STAGE_INGRESS_0_E;
            break;

        case PRV_TGF_POLICER_STAGE_INGRESS_1_E:
            *dxChStagePtr = CPSS_DXCH_POLICER_STAGE_INGRESS_1_E;
            break;

        case PRV_TGF_POLICER_STAGE_EGRESS_E:
            *dxChStagePtr = CPSS_DXCH_POLICER_STAGE_EGRESS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToExMxPmPolicerEntry
*
* DESCRIPTION:
*       Convert generic into device specific policer entry
*
* INPUTS:
*       policerEntryPtr - (pointer to) policer entry
*
* OUTPUTS:
*       exMxPmEntryPtr - (pointer to) ExMxPm policer entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmPolicerEntry
(
    IN  PRV_TGF_POLICER_ENTRY_STC               *policerEntryPtr,
    OUT CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  *exMxPmEntryPtr
)
{
    /* convert entry info into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEntryPtr, policerEntryPtr, dropRed);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEntryPtr, policerEntryPtr, meterColorMode);

    exMxPmEntryPtr->countingEntryIndex = policerEntryPtr->counterSetIndex;

    /* convert modifyQosParams into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyTc);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyDp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyDscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyExp);

    /* convert remarkMode into device specific format */
    switch (policerEntryPtr->remarkMode)
    {
        case PRV_TGF_POLICER_REMARK_MODE_L2_E:
            exMxPmEntryPtr->remarkMode = CPSS_EXMXPM_POLICER_REMARK_MODE_L2_E;
            break;

        case PRV_TGF_POLICER_REMARK_MODE_L3_E:
            exMxPmEntryPtr->remarkMode = CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert meterMode into device specific format */
    switch (policerEntryPtr->meterMode)
    {
        case PRV_TGF_POLICER_METER_MODE_SR_TCM_E:
            exMxPmEntryPtr->meterMode = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;

            /* convert tbParams into device specific format */
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.srTcmParams),
                                           &(policerEntryPtr->tbParams.srTcmParams), cir);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.srTcmParams),
                                           &(policerEntryPtr->tbParams.srTcmParams), cbs);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.srTcmParams),
                                           &(policerEntryPtr->tbParams.srTcmParams), ebs);

            break;

        case PRV_TGF_POLICER_METER_MODE_TR_TCM_E:
            exMxPmEntryPtr->meterMode = CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E;

            /* convert tbParams into device specific format */
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.trTcmParams),
                                           &(policerEntryPtr->tbParams.trTcmParams), cir);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.trTcmParams),
                                           &(policerEntryPtr->tbParams.trTcmParams), cbs);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.trTcmParams),
                                           &(policerEntryPtr->tbParams.trTcmParams), pir);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.trTcmParams),
                                           &(policerEntryPtr->tbParams.trTcmParams), pbs);

            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mngCounterSet into device specific format */
    switch (policerEntryPtr->mngCounterSet)
    {
        case PRV_TGF_POLICER_MNG_CNTR_SET0_E:
            exMxPmEntryPtr->mngCounterSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET1_E:
            exMxPmEntryPtr->mngCounterSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET2_E:
            exMxPmEntryPtr->mngCounterSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_DISABLED_E:
            exMxPmEntryPtr->mngCounterSet = CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericPolicerEntry
*
* DESCRIPTION:
*       Convert generic from device specific policer entry
*
* INPUTS:
*       exMxPmEntryPtr - (pointer to) ExMxPm policer entry parameters
*
* OUTPUTS:
*       policerEntryPtr - (pointer to) policer entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericPolicerEntry
(
    IN  CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  *exMxPmEntryPtr,
    OUT PRV_TGF_POLICER_ENTRY_STC               *policerEntryPtr
)
{
    /* convert entry info from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEntryPtr, policerEntryPtr, dropRed);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEntryPtr, policerEntryPtr, meterColorMode);

    policerEntryPtr->counterSetIndex = exMxPmEntryPtr->countingEntryIndex;

    /* convert modifyQosParams from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyTc);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyDp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyDscp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->modifyQosParams),
                                   &(policerEntryPtr->modifyQosParams), modifyExp);

    /* convert remarkMode from device specific format */
    switch (exMxPmEntryPtr->remarkMode)
    {
        case CPSS_EXMXPM_POLICER_REMARK_MODE_L2_E:
            policerEntryPtr->remarkMode = PRV_TGF_POLICER_REMARK_MODE_L2_E;
            break;

        case CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E:
            policerEntryPtr->remarkMode = PRV_TGF_POLICER_REMARK_MODE_L3_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert meterMode from device specific format */
    switch (exMxPmEntryPtr->meterMode)
    {
        case CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E:
            policerEntryPtr->meterMode = PRV_TGF_POLICER_METER_MODE_SR_TCM_E;

            /* convert tbParams from device specific format */
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.srTcmParams),
                                           &(policerEntryPtr->tbParams.srTcmParams), cir);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.srTcmParams),
                                           &(policerEntryPtr->tbParams.srTcmParams), cbs);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.srTcmParams),
                                           &(policerEntryPtr->tbParams.srTcmParams), ebs);

            break;

        case CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E:
            policerEntryPtr->meterMode = PRV_TGF_POLICER_METER_MODE_TR_TCM_E;

            /* convert tbParams from device specific format */
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.trTcmParams),
                                           &(policerEntryPtr->tbParams.trTcmParams), cir);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.trTcmParams),
                                           &(policerEntryPtr->tbParams.trTcmParams), cbs);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.trTcmParams),
                                           &(policerEntryPtr->tbParams.trTcmParams), pir);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEntryPtr->tokenBucketsParams.trTcmParams),
                                           &(policerEntryPtr->tbParams.trTcmParams), pbs);

            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mngCounterSet from device specific format */
    switch (exMxPmEntryPtr->mngCounterSet)
    {
        case CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E:
            policerEntryPtr->mngCounterSet = PRV_TGF_POLICER_MNG_CNTR_SET0_E;
            break;

        case CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E:
            policerEntryPtr->mngCounterSet = PRV_TGF_POLICER_MNG_CNTR_SET1_E;
            break;

        case CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E:
            policerEntryPtr->mngCounterSet = PRV_TGF_POLICER_MNG_CNTR_SET2_E;
            break;

        case CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E:
            policerEntryPtr->mngCounterSet = PRV_TGF_POLICER_MNG_CNTR_DISABLED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPolicerInit
*
* DESCRIPTION:
*       Init Traffic Conditioner facility on specified device
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - on illegal devNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerInit
(
    GT_VOID
)
{
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;


#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPolicerInit(devNum);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerInit FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPolicerInit(devNum);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPolicerInit FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables metering per device.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable          - Enable/disable metering:
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringEnableSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_BOOL                        enable
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerMeteringEnableSet(devNum, dxChStage, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerMeteringEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(enable);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringEnableGet
*
* DESCRIPTION:
*       Gets device metering status (Enable/Disable).
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr       - pointer to Enable/Disable metering
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringEnableGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_BOOL                        *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerMeteringEnableGet(devNum, dxChStage, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerMeteringEnableGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerBillingCountingEnableSet
*
* DESCRIPTION:
*       Enables or disables Billing Counting.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - enable/disable Billing Counting:
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingCountingEnableSet
(
    IN GT_BOOL  enable
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerBillingCountingEnableSet(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerBillingCountingEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(enable);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerBillingCountingEnableGet
*
* DESCRIPTION:
*       Gets device Billing Counting status (Enable/Disable).
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to Enable/Disable Billing Counting
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingCountingEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* call device specific API */
    rc = cpssDxCh3PolicerBillingCountingEnableGet(devNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerBillingCountingEnableGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerCountingModeSet
*
* DESCRIPTION:
*       Configures counting mode.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode    -  counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingModeSet
(
    IN GT_U8                             devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN PRV_TGF_POLICER_COUNTING_MODE_ENT mode
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_POLICER_COUNTING_MODE_ENT dxChMode;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert command into device specific format */
    switch (mode)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_COUNTING_DISABLE_E,
                                CPSS_DXCH_POLICER_COUNTING_DISABLE_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_COUNTING_BILLING_IPFIX_E,
                                CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_COUNTING_POLICY_E,
                                CPSS_DXCH_POLICER_COUNTING_POLICY_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_COUNTING_VLAN_E,
                                CPSS_DXCH_POLICER_COUNTING_VLAN_E);

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxChPolicerCountingModeSet(devNum, dxChStage, dxChMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerCountingModeSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(mode);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerCountingModeGet
*
* DESCRIPTION:
*       Gets the couning mode.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - pointer to Counting mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingModeGet
(
    IN  GT_U8                             devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    OUT PRV_TGF_POLICER_COUNTING_MODE_ENT *modePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    dxChStage;
    CPSS_DXCH_POLICER_COUNTING_MODE_ENT dxChCntrMode;

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChPolicerCountingModeGet(devNum, dxChStage, &dxChCntrMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerCountingModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert dxChCntrMode into device specific format */
    switch (dxChCntrMode)
    {
        case CPSS_DXCH_POLICER_COUNTING_DISABLE_E:
            *modePtr = PRV_TGF_POLICER_COUNTING_DISABLE_E;
            break;

        case CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E:
            *modePtr = PRV_TGF_POLICER_COUNTING_BILLING_IPFIX_E;
            break;

        case CPSS_DXCH_POLICER_COUNTING_POLICY_E:
            *modePtr = PRV_TGF_POLICER_COUNTING_POLICY_E;
            break;

        case CPSS_DXCH_POLICER_COUNTING_VLAN_E:
            *modePtr = PRV_TGF_POLICER_COUNTING_VLAN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(modePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerBillingCountersModeSet
*
* DESCRIPTION:
*      Sets the Billing Counters resolution.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrMode        - Billing Counters resolution: 1 Byte
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or cntrMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingCountersModeSet
(
    IN PRV_TGF_POLICER_BILLING_CNTR_MODE_ENT cntrMode
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT dxChCntrMode;

    /* convert cntrMode into device specific format */
    switch (cntrMode)
    {
        case PRV_TGF_POLICER_BILLING_CNTR_1_BYTE_E:
            dxChCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E;
            break;

        case PRV_TGF_POLICER_BILLING_CNTR_16_BYTES_E:
            dxChCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E;
            break;

        case PRV_TGF_POLICER_BILLING_CNTR_PACKET_E:
            dxChCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerBillingCountersModeSet(devNum, dxChCntrMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerBillingCountersModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(cntrMode);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerBillingCountersModeGet
*
* DESCRIPTION:
*      Gets the Billing Counters resolution.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       cntrModePtr     - pointer to the Billing Counters resolution
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingCountersModeGet
(
    IN GT_U8                                  devNum,
    OUT PRV_TGF_POLICER_BILLING_CNTR_MODE_ENT *cntrModePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT dxChCntrMode;

    CPSS_NULL_PTR_CHECK_MAC(cntrModePtr);

    /* call device specific API */
    rc = cpssDxCh3PolicerBillingCountersModeGet(devNum, &dxChCntrMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerBillingCountersModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert dxChCntrMode into device specific format */
    switch (dxChCntrMode)
    {
        case CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E:
            *cntrModePtr = PRV_TGF_POLICER_BILLING_CNTR_1_BYTE_E;
            break;

        case CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E:
            *cntrModePtr = PRV_TGF_POLICER_BILLING_CNTR_16_BYTES_E;
            break;

        case CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E:
            *cntrModePtr = PRV_TGF_POLICER_BILLING_CNTR_PACKET_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(cntrModePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPacketSizeModeSet
*
* DESCRIPTION:
*       Sets metered Packet Size Mode that metering and billing is done
*       according to.
*
* INPUTS:
*       devNum             - physical device number.
*       stage              - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       packetSize         - Type of packet size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or packetSize.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPacketSizeModeSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT packetSize
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerPacketSizeModeSet(devNum, dxChStage, packetSize);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerPacketSizeModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(packetSize);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPacketSizeModeGet
*
* DESCRIPTION:
*       Gets metered Packet Size Mode that metering and billing is done
*       according to.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       packetSizePtr   - pointer to the Type of packet size
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPacketSizeModeGet
(
    IN  GT_U8                             devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    OUT CPSS_POLICER_PACKET_SIZE_MODE_ENT *packetSizePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    CPSS_NULL_PTR_CHECK_MAC(packetSizePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerPacketSizeModeGet(devNum, dxChStage, packetSizePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerPacketSizeModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(packetSizePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeterResolutionSet
*
* DESCRIPTION:
*       Sets metering algorithm resolution
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       resolution      - packet/Byte based Meter resolution.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or resolution.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeterResolutionSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN PRV_TGF_POLICER_METER_RESOLUTION_ENT resolution
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        dxChStage;
    CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT dxChResolution;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert resolution into device specific format */
    switch (resolution)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChResolution,
                                PRV_TGF_POLICER_METER_RESOLUTION_BYTES_E,
                                CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChResolution,
                                CPSS_DXCH3_POLICER_METER_RESOLUTION_PACKETS_E,
                                PRV_TGF_POLICER_METER_RESOLUTION_PACKETS_E);

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerMeterResolutionSet(devNum, dxChStage, dxChResolution);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerMeterResolutionSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(resolution);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeterResolutionGet
*
* DESCRIPTION:
*       Gets metering algorithm resolution
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       resolutionPtr   - pointer to the Meter resolution: packet or Byte based.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeterResolutionGet
(
    IN  GT_U8                                devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    OUT PRV_TGF_POLICER_METER_RESOLUTION_ENT *resolutionPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        dxChStage;
    CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT dxChResolution;

    CPSS_NULL_PTR_CHECK_MAC(resolutionPtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerMeterResolutionGet(devNum, dxChStage, &dxChResolution);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerMeterResolutionGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert dxChResolution into device specific format */
    switch (dxChResolution)
    {
        case CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E:
            *resolutionPtr = PRV_TGF_POLICER_METER_RESOLUTION_BYTES_E;
            break;

        case CPSS_DXCH3_POLICER_METER_RESOLUTION_PACKETS_E:
            *resolutionPtr = PRV_TGF_POLICER_METER_RESOLUTION_PACKETS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(resolutionPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerDropTypeSet
*
* DESCRIPTION:
*       Sets the Policer out-of-profile drop command type.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       dropType        - Policer Drop Type: Soft or Hard.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, dropType or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerDropTypeSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN CPSS_DROP_MODE_TYPE_ENT        dropType
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerDropTypeSet(devNum, dxChStage, dropType);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerDropTypeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(dropType);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerDropTypeGet
*
* DESCRIPTION:
*       Gets the Policer out-of-profile drop command type.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*
* OUTPUTS:
*       dropTypePtr     - pointer to the Policer Drop Type: Soft or Hard.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerDropTypeGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT CPSS_DROP_MODE_TYPE_ENT        *dropTypePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    CPSS_NULL_PTR_CHECK_MAC(dropTypePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerDropTypeGet(devNum, dxChStage, dropTypePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerDropTypeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(dropTypePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerCountingColorModeSet
*
* DESCRIPTION:
*       Sets the Policer color counting mode.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - Color counting mode: Drop Precedence or
*                         Conformance Level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingColorModeSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN PRV_TGF_POLICER_COLOR_COUNT_MODE_ENT mode
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        dxChStage;
    CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT dxChMode;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert mode into device specific format */
    switch (mode)
    {
        case PRV_TGF_POLICER_COLOR_COUNT_CL_E:
            dxChMode = CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E;
            break;

        case PRV_TGF_POLICER_COLOR_COUNT_DP_E:
            dxChMode = CPSS_DXCH3_POLICER_COLOR_COUNT_DP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerCountingColorModeSet(devNum, dxChStage, dxChMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerCountingColorModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(mode);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerCountingColorModeGet
*
* DESCRIPTION:
*       Gets the Policer color counting mode.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - pointer to the color counting mode:
*                         Drop Precedence or Conformance Level.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingColorModeGet
(
    IN  GT_U8                                devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    OUT PRV_TGF_POLICER_COLOR_COUNT_MODE_ENT *modePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        dxChStage;
    CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT dxChCntrMode;

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerCountingColorModeGet(devNum, dxChStage, &dxChCntrMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerCountingColorModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert dxChCntrMode into device specific format */
    switch (dxChCntrMode)
    {
        case CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E:
            *modePtr = PRV_TGF_POLICER_COLOR_COUNT_CL_E;
            break;

        case CPSS_DXCH3_POLICER_COLOR_COUNT_DP_E:
            *modePtr = PRV_TGF_POLICER_COLOR_COUNT_DP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(modePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerManagementCntrsResolutionSet
*
* DESCRIPTION:
*       Sets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       cntrSet         - Management Counters Set [0..2].
*       cntrResolution  - Management Counters resolution: 1 or 16 Bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or
*                                     Management Counters Set or cntrResolution.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerManagementCntrsResolutionSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT          stage,
    IN PRV_TGF_POLICER_MNG_CNTR_SET_ENT        cntrSet,
    IN PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_ENT cntrResolution
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT           dxChStage;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT        dxChMngCntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT dxChCntrResolution;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert cntrSet into device specific format */
    switch (cntrSet)
    {
        case PRV_TGF_POLICER_MNG_CNTR_SET0_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET1_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET2_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_DISABLED_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert cntrResolution into device specific format */
    switch (cntrResolution)
    {
        case PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_1B_E:
            dxChCntrResolution = CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_1B_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_16B_E:
            dxChCntrResolution = CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_16B_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerManagementCntrsResolutionSet(devNum, dxChStage, dxChMngCntrSet, dxChCntrResolution);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerManagementCntrsResolutionSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(cntrSet);
    TGF_PARAM_NOT_USED(cntrResolution);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerManagementCntrsResolutionGet
*
* DESCRIPTION:
*       Gets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       cntrSet         - Management Counters Set [0..2].
*
* OUTPUTS:
*       cntrResolutionPtr   - pointer to the Management Counters
*                             resolution: 1 or 16 Bytes.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or  Mng Counters Set.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerManagementCntrsResolutionGet
(
    IN  GT_U8                                    devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT           stage,
    IN  PRV_TGF_POLICER_MNG_CNTR_SET_ENT         cntrSet,
    OUT PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_ENT  *cntrResolutionPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT           dxChStage;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT        dxChMngCntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT dxChCntrResolution;

    CPSS_NULL_PTR_CHECK_MAC(cntrResolutionPtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert cntrSet into device specific format */
    switch (cntrSet)
    {
        case PRV_TGF_POLICER_MNG_CNTR_SET0_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET1_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET2_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_DISABLED_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerManagementCntrsResolutionGet(devNum, dxChStage, dxChMngCntrSet, &dxChCntrResolution);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerManagementCntrsResolutionGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert dxChMngCntrSet into device specific format */
    switch (dxChCntrResolution)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_1B_E:
            *cntrResolutionPtr = PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_1B_E;
            break;

        case CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_16B_E:
            *cntrResolutionPtr = PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_16B_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(cntrSet);
    TGF_PARAM_NOT_USED(cntrResolutionPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPacketSizeModeForTunnelTermSet
*
* DESCRIPTION:
*      Sets size mode for metering and counting of tunnel terminated packets.
*
* INPUTS:
*       devNum              - physical device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       ttPacketSizeMode    - Tunnel Termination Packet Size Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or ttPacketSizeMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPacketSizeModeForTunnelTermSet
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT          stage,
    IN  PRV_TGF_POLICER_TT_PACKET_SIZE_MODE_ENT ttPacketSizeMode
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT           dxChStage;
    CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT dxChTtPacketSizeMode;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert mode into device specific format */
    switch (ttPacketSizeMode)
    {
        case PRV_TGF_POLICER_TT_PACKET_SIZE_REGULAR_ENT:
            dxChTtPacketSizeMode = CPSS_DXCH3_POLICER_TT_PACKET_SIZE_REGULAR_ENT;
            break;

        case PRV_TGF_POLICER_TT_PACKET_SIZE_PASSENGER_ENT:
            dxChTtPacketSizeMode = CPSS_DXCH3_POLICER_TT_PACKET_SIZE_PASSENGER_ENT;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerPacketSizeModeForTunnelTermSet(devNum, dxChStage, dxChTtPacketSizeMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerPacketSizeModeForTunnelTermSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(ttPacketSizeMode);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPacketSizeModeForTunnelTermGet
*
* DESCRIPTION:
*      Gets size mode for metering and counting of tunnel terminated packets.
*
* INPUTS:
*       devNum              - physical device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       ttPacketSizeModePtr     - pointer to the Tunnel Termination
*                                 Packet Size Mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPacketSizeModeForTunnelTermGet
(
    IN  GT_U8                                    devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT           stage,
    OUT PRV_TGF_POLICER_TT_PACKET_SIZE_MODE_ENT  *ttPacketSizeModePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT           dxChStage;
    CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT dxChTtPacketSizeMode;

    CPSS_NULL_PTR_CHECK_MAC(ttPacketSizeModePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerPacketSizeModeForTunnelTermGet(devNum, dxChStage, &dxChTtPacketSizeMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerPacketSizeModeForTunnelTermGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert dxChTtPacketSizeMode into device specific format */
    switch (dxChTtPacketSizeMode)
    {
        case CPSS_DXCH3_POLICER_TT_PACKET_SIZE_REGULAR_ENT:
            *ttPacketSizeModePtr = PRV_TGF_POLICER_TT_PACKET_SIZE_REGULAR_ENT;
            break;

        case CPSS_DXCH3_POLICER_TT_PACKET_SIZE_PASSENGER_ENT:
            *ttPacketSizeModePtr = PRV_TGF_POLICER_TT_PACKET_SIZE_PASSENGER_ENT;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(ttPacketSizeModePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshScanEnableSet
*
* DESCRIPTION:
*       Enables or disables the metering Auto Refresh Scan mechanism.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Enable/disable Auto Refresh Scan mechanism:
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshScanEnableSet
(
    IN GT_BOOL enable
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(enable);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshScanEnableGet
*
* DESCRIPTION:
*       Gets metering Auto Refresh Scan mechanism status (Enabled/Disabled).
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer on Auto Refresh Scan mechanism status
*                         (Enable/Disable) :
*                         GT_TRUE  - Auto Refresh scan mechanism is enabled.
*                         GT_FALSE - Auto Refresh scan mechanism is disabled.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshScanEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* call device specific API */
    rc = cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet(devNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshRangeSet
*
* DESCRIPTION:
*       Sets Metering Refresh Scan address range (Start and Stop addresses).
*
* INPUTS:
*       devNum              - physical device number.
*       startAddress        - beginning of the address range to be scanned
*                             by the Auto Refresh Scan mechanism [0..1023].
*       stopAddress         - end of the address range to be scanned by the
*                             Auto Refresh Scan mechanism [0..1023].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or illegal values
*                                     of startAddress and stopAddress.
*       GT_OUT_OF_RANGE             - on out of range of startAddress
*                                     or stopAddress.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshRangeSet
(
    IN GT_U32 startAddress,
    IN GT_U32 stopAddress
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerMeteringAutoRefreshRangeSet(devNum, startAddress, stopAddress);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerCountingColorModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(startAddress);
    TGF_PARAM_NOT_USED(stopAddress);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshRangeGet
*
* DESCRIPTION:
*       Gets Metering Refresh Scan address range (Start and Stop addresses).
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       startAddressPtr     - pointer to the beginning address of Refresh
*                             Scan address range.
*       stopAddressPtr      - pointer to the end address of Refresh Scan
*                             address range.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointers.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshRangeGet
(
    IN GT_U8   devNum,
    OUT GT_U32 *startAddressPtr,
    OUT GT_U32 *stopAddressPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;

    CPSS_NULL_PTR_CHECK_MAC(startAddressPtr);
    CPSS_NULL_PTR_CHECK_MAC(stopAddressPtr);

    /* call device specific API */
    rc = cpssDxCh3PolicerMeteringAutoRefreshRangeGet(devNum, startAddressPtr, stopAddressPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerMeteringAutoRefreshRangeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(startAddressPtr);
    TGF_PARAM_NOT_USED(stopAddressPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshIntervalSet
*
* DESCRIPTION:
*       Sets the time interval between two refresh access to metering table.
*
* INPUTS:
*       devNum          - physical device number.
*       interval        - time interval between refresh of two entries
*                         in micro seconds. Upon 270 Mhz core clock value
*                         of DxCh3, the range [0..15907286 microS].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_OUT_OF_RANGE             - on out of range of interval value.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshIntervalSet
(
    IN GT_U32 interval
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerMeteringAutoRefreshIntervalSet(devNum, interval);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerMeteringAutoRefreshIntervalSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(interval);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshIntervalGet
*
* DESCRIPTION:
*       Gets the time interval between two refresh access to metering table.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       intervalPtr     - pointer to the time interval between refresh of two
*                         entries in micro seconds. Upon 270 Mhz core clock
*                         value of DxCh3, the range [0..15907286 microS].
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshIntervalGet
(
    IN GT_U8                                devNum,
    OUT GT_U32                              *intervalPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;

    CPSS_NULL_PTR_CHECK_MAC(intervalPtr);

    /* call device specific API */
    rc = cpssDxCh3PolicerMeteringAutoRefreshIntervalGet(devNum, intervalPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerMeteringAutoRefreshIntervalGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(intervalPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMeteringEntryRefresh
*
* DESCRIPTION:
*       Refresh the Policer Metering Entry.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of Policer Metering Entry
*                         going to be refreshed [0..1023].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringEntryRefresh
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         entryIndex
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        if(usePortGroupsBmp == GT_FALSE)
        {
            rc = cpssDxCh3PolicerMeteringEntryRefresh(devNum, dxChStage, entryIndex);
        }
        else
        {
            rc = cpssDxChPolicerPortGroupMeteringEntryRefresh(devNum, currPortGroupsBmp, dxChStage, entryIndex);
        }

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupMeteringEntryRefresh FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(entryIndex);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPortMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables a port metering trigger for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       enable          - Enable/Disable per-port metering for packets arriving
*                         on this port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPortMeteringEnableSet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerPortMeteringEnableSet(devNum, dxChStage, portNum, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerPortMeteringEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPortMeteringEnableGet
*
* DESCRIPTION:
*       Gets port status (Enable/Disable) of metering for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*
* OUTPUTS:
*       enablePtr       - pointer on per-port metering status (Enable/Disable)
*                         for packets arriving on specified port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPortMeteringEnableGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U8                          portNum,
    OUT GT_BOOL                        *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerPortMeteringEnableGet(devNum, dxChStage, portNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerPacketSizeModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}


/*******************************************************************************
* prvTgfPolicerManagementCountersSet
*
* DESCRIPTION:
*       Sets the value of specified Management Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*       mngCntrPtr      - pointer to the Management Counters Entry must be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage or mngCntrType
*                                     or Management Counters Set number.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In data unit management counter only 32 bits are used for DxCh3 devices
*       and 42 bits are used for DxChXcat and above devices.
*******************************************************************************/
GT_STATUS prvTgfPolicerManagementCountersSet
(
    IN GT_U8                              devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN PRV_TGF_POLICER_MNG_CNTR_SET_ENT   mngCntrSet,
    IN PRV_TGF_POLICER_MNG_CNTR_TYPE_ENT  mngCntrType,
    IN PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC *mngCntrPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT      dxChStage;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT   dxChMngCntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT  dxChMngCntrType;
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC dxChMngCntr;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert mngCntrSet into device specific format */
    switch (mngCntrSet)
    {
        case PRV_TGF_POLICER_MNG_CNTR_SET0_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET1_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET2_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_DISABLED_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mngCntrType into device specific format */
    switch (mngCntrType)
    {
        case PRV_TGF_POLICER_MNG_CNTR_GREEN_E:
            dxChMngCntrType = CPSS_DXCH3_POLICER_MNG_CNTR_GREEN_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_YELLOW_E:
            dxChMngCntrType = CPSS_DXCH3_POLICER_MNG_CNTR_YELLOW_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_RED_E:
            dxChMngCntrType = CPSS_DXCH3_POLICER_MNG_CNTR_RED_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_DROP_E:
            dxChMngCntrType = CPSS_DXCH3_POLICER_MNG_CNTR_DROP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mngCntrPtr into device specific format */
    dxChMngCntr.duMngCntr.l[0] = mngCntrPtr->duMngCntr.l[0];
    dxChMngCntr.duMngCntr.l[1] = mngCntrPtr->duMngCntr.l[1];
    dxChMngCntr.packetMngCntr  = mngCntrPtr->packetMngCntr;

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxCh3PolicerManagementCountersSet(devNum, dxChStage,
                dxChMngCntrSet, dxChMngCntrType, &dxChMngCntr);
    }
    else
    {
        rc = cpssDxChPolicerPortGroupManagementCountersSet(devNum, currPortGroupsBmp, dxChStage,
                dxChMngCntrSet, dxChMngCntrType, &dxChMngCntr);
    }

    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupManagementCountersSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(mngCntrSet);
    TGF_PARAM_NOT_USED(mngCntrType);
    TGF_PARAM_NOT_USED(mngCntrPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerManagementCountersGet
*
* DESCRIPTION:
*       Gets the value of specified Management Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*
* OUTPUTS:
*       mngCntrPtr      - pointer to the requested Management Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage or mngCntrType
*                                     or Management Counters Set number.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In data unit management counter only 32 bits are used for DxCh3 devices
*       and 42 bits are used for DxChXcat and above devices.
*******************************************************************************/
GT_STATUS prvTgfPolicerManagementCountersGet
(
    IN GT_U8                               devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN  PRV_TGF_POLICER_MNG_CNTR_SET_ENT   mngCntrSet,
    IN  PRV_TGF_POLICER_MNG_CNTR_TYPE_ENT  mngCntrType,
    OUT PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC *mngCntrPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        dxChStage;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT     dxChMngCntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT    dxChMngCntrType;
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC   dxChMngCntr;

    CPSS_NULL_PTR_CHECK_MAC(mngCntrPtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert mngCntrSet into device specific format */
    switch (mngCntrSet)
    {
        case PRV_TGF_POLICER_MNG_CNTR_SET0_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET1_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_SET2_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_DISABLED_E:
            dxChMngCntrSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mngCntrType into device specific format */
    switch (mngCntrType)
    {
        case PRV_TGF_POLICER_MNG_CNTR_GREEN_E:
            dxChMngCntrType = CPSS_DXCH3_POLICER_MNG_CNTR_GREEN_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_YELLOW_E:
            dxChMngCntrType = CPSS_DXCH3_POLICER_MNG_CNTR_YELLOW_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_RED_E:
            dxChMngCntrType = CPSS_DXCH3_POLICER_MNG_CNTR_RED_E;
            break;

        case PRV_TGF_POLICER_MNG_CNTR_DROP_E:
            dxChMngCntrType = CPSS_DXCH3_POLICER_MNG_CNTR_DROP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxCh3PolicerManagementCountersGet(devNum, dxChStage,
                dxChMngCntrSet, dxChMngCntrType, &dxChMngCntr);
    }
    else
    {
        rc = cpssDxChPolicerPortGroupManagementCountersGet(devNum, currPortGroupsBmp, dxChStage,
                dxChMngCntrSet, dxChMngCntrType, &dxChMngCntr);
    }

    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupManagementCountersGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert mngCntrPtr from device specific format */
    mngCntrPtr->duMngCntr.l[0] = dxChMngCntr.duMngCntr.l[0];
    mngCntrPtr->duMngCntr.l[1] = dxChMngCntr.duMngCntr.l[1];
    mngCntrPtr->packetMngCntr  = dxChMngCntr.packetMngCntr;

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(mngCntrSet);
    TGF_PARAM_NOT_USED(mngCntrType);
    TGF_PARAM_NOT_USED(mngCntrPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEntrySet
*
* DESCRIPTION:
*      Set Policer Entry configuration
*
* INPUTS:
*       devNum     - device number
*       stage      - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                    Stage type is significant for DxChXcat and above devices
*                    and ignored by DxCh3.
*       entryIndex - policer entry index
*       entryPtr   - (pointer to) policer entry
*
* OUTPUTS:
*       tbParamsPtr - (pointer to) actual policer token bucket parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum, entryIndex or entry parameter
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEntrySet
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN  GT_U32                               entryIndex,
    IN  PRV_TGF_POLICER_ENTRY_STC           *entryPtr,
    OUT PRV_TGF_POLICER_METER_TB_PARAMS_UNT *tbParamsPtr
)
{
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_POLICER_ENTRY_STC             dxChEntry;
    CPSS_DXCH_POLICER_TB_PARAMS_STC         dxChTbParams;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        dxChStage;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC   dxCh3Entry;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  dxCh3TbParams;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC   exMxPmEntry;
    CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT  exMxPmTbParams;
    TGF_PARAM_NOT_USED(stage);
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    CPSS_NULL_PTR_CHECK_MAC(tbParamsPtr);

    /* get device family */
    rc = prvUtfDeviceFamilyGet(prvTgfDevNum, &devFamily);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    if (devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        TGF_PARAM_NOT_USED(stage);
        TGF_PARAM_NOT_USED(dxChStage);
        TGF_PARAM_NOT_USED(dxCh3Entry);
        TGF_PARAM_NOT_USED(dxCh3TbParams);

        /* convert policer entry into device specific format */
        rc = prvTgfConvertGenericToDxChPolicerEntry(entryPtr, &dxChEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerEntry FAILED, rc = [%d]", rc);

            return rc;
        }

        /* go over all active devices */
        while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
        {
            /* call device specific API */
            rc = cpssDxChPolicerEntrySet(devNum, entryIndex, &dxChEntry, &dxChTbParams);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerEntrySet FAILED, rc = [%d]", rc);

                rc1 = rc;
            }
        }

        /* convert tbParams from device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->srTcmParams),
                                       &(dxChEntry.tbParams), cir);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->srTcmParams),
                                       &(dxChEntry.tbParams), cbs);

        return rc1;
    }
    else
    {
        TGF_PARAM_NOT_USED(dxChEntry);
        TGF_PARAM_NOT_USED(dxChTbParams);

        /* clear dxCh3TbParams */
        cpssOsMemSet(&dxCh3TbParams, 0, sizeof(dxCh3TbParams));

        /* convert stage into device specific format */
        rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

            return rc;
        }

        /* convert policer entry into device specific format */
        rc = prvTgfConvertGenericToDxCh3PolicerMeteringEntry(entryPtr, &dxCh3Entry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerEntry FAILED, rc = [%d]", rc);

            return rc;
        }

        /* go over all active devices */
        while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
        {
            /* call device specific API */
            if(usePortGroupsBmp == GT_FALSE)
            {
                rc = cpssDxCh3PolicerMeteringEntrySet(devNum, dxChStage, entryIndex, &dxCh3Entry, &dxCh3TbParams);
            }
            else
            {
                rc = cpssDxChPolicerPortGroupMeteringEntrySet(devNum, currPortGroupsBmp, dxChStage, entryIndex, &dxCh3Entry, &dxCh3TbParams);
            }

            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupMeteringEntrySet FAILED, rc = [%d]", rc);

                rc1 = rc;
            }
        }

        /* convert tbParams from device specific format */
        if (CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E == dxCh3Entry.meterMode)
        {
            tbParamsPtr->srTcmParams.cir = dxCh3TbParams.srTcmParams.cir;
            tbParamsPtr->srTcmParams.cbs = dxCh3TbParams.srTcmParams.cbs;
            tbParamsPtr->srTcmParams.ebs = dxCh3TbParams.srTcmParams.ebs;
        }
        else
        {
            tbParamsPtr->trTcmParams.cir = dxCh3TbParams.trTcmParams.cir;
            tbParamsPtr->trTcmParams.cbs = dxCh3TbParams.trTcmParams.cbs;
            tbParamsPtr->trTcmParams.pir = dxCh3TbParams.trTcmParams.pir;
            tbParamsPtr->trTcmParams.pbs = dxCh3TbParams.trTcmParams.pbs;
        }

        return rc1;
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_NULL_PTR_CHECK_MAC(tbParamsPtr);

    /* clear dxCh3TbParams */
    cpssOsMemSet(&exMxPmTbParams, 0, sizeof(exMxPmTbParams));

    /* convert policer entry into device specific format */
    rc = prvTgfConvertGenericToExMxPmPolicerEntry(entryPtr, &exMxPmEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPolicerEntry FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPolicerMeteringEntrySet(devNum, entryIndex, CPSS_DIRECTION_INGRESS_E,
                                               &exMxPmEntry, &exMxPmTbParams);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPolicerMeteringEntrySet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    /* convert tbParams from device specific format */
    if (CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E == exMxPmEntry.meterMode)
    {
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->srTcmParams),
                                       &(exMxPmTbParams.srTcmParams), cir);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->srTcmParams),
                                       &(exMxPmTbParams.srTcmParams), cbs);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->srTcmParams),
                                       &(exMxPmTbParams.srTcmParams), ebs);
    }
    else
    {
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->trTcmParams),
                                       &(exMxPmTbParams.trTcmParams), cir);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->trTcmParams),
                                       &(exMxPmTbParams.trTcmParams), cbs);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->trTcmParams),
                                       &(exMxPmTbParams.trTcmParams), pir);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(tbParamsPtr->trTcmParams),
                                       &(exMxPmTbParams.trTcmParams), pbs);
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(entryIndex);
    TGF_PARAM_NOT_USED(entryPtr);
    TGF_PARAM_NOT_USED(tbParamsPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEntryGet
*
* DESCRIPTION:
*      Get Policer Entry parameters
*
* INPUTS:
*       devNum     - device number
*       stage      - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                    Stage type is significant for DxChXcat and above devices
*                    and ignored by DxCh3.
*       entryIndex - policer entry index
*
* OUTPUTS:
*       entryPtr - (pointer to) policer entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or entryIndex
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*       GT_TIMEOUT   - on max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEntryGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         entryIndex,
    OUT PRV_TGF_POLICER_ENTRY_STC     *entryPtr
)
{
    GT_STATUS rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_POLICER_ENTRY_STC           dxChEntry;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC dxCh3Entry;
    CPSS_PP_FAMILY_TYPE_ENT               devFamily;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT      dxChStage;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC   exMxPmEntry;
    TGF_PARAM_NOT_USED(stage);
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    /* get device family */
    rc = prvUtfDeviceFamilyGet(prvTgfDevNum, &devFamily);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);

        return rc;
    }

    if (devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        TGF_PARAM_NOT_USED(stage);
        TGF_PARAM_NOT_USED(dxChStage);
        TGF_PARAM_NOT_USED(dxCh3Entry);

        /* call device specific API */
        rc = cpssDxChPolicerEntryGet(devNum, entryIndex, &dxChEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerEntryGet FAILED, rc = [%d]", rc);

            return rc;
        }

        /* convert policer entry into device specific format */
        return prvTgfConvertDxChToGenericPolicerEntry(&dxChEntry, entryPtr);
    }
    else
    {
        TGF_PARAM_NOT_USED(dxChEntry);

        /* convert stage into device specific format */
        rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

            return rc;
        }

        /* reset entry */
        cpssOsMemSet(&dxCh3Entry, 0, sizeof(dxCh3Entry));

        /* call device specific API */
        if(usePortGroupsBmp == GT_FALSE)
        {
            rc = cpssDxCh3PolicerMeteringEntryGet(devNum, dxChStage, entryIndex, &dxCh3Entry);
        }
        else
        {
            rc = cpssDxChPolicerPortGroupMeteringEntryGet(devNum, currPortGroupsBmp, dxChStage, entryIndex, &dxCh3Entry);
        }

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupMeteringEntryGet FAILED, rc = [%d]", rc);

            return rc;
        }

        /* convert policer entry into device specific format */
        return prvTgfConvertDxCh3ToGenericPolicerEntry(&dxCh3Entry, entryPtr);
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    /* call device specific API */
    rc = cpssExMxPmPolicerMeteringEntryGet(devNum, CPSS_DIRECTION_INGRESS_E,
                                           entryIndex, &exMxPmEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerEntryGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert policer entry into device specific format */
    return prvTgfConvertExMxPmToGenericPolicerEntry(&exMxPmEntry, entryPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(entryIndex);
    TGF_PARAM_NOT_USED(entryPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*      Calculates Token Bucket parameters in the Application format without
*      HW update.
*
* INPUTS:
*       devNum          - device number
*       tbInParamsPtr   - pointer to Token bucket input parameters.
*       meterMode       - Meter mode (SrTCM or TrTCM).
*
* OUTPUTS:
*       tbOutParamsPtr  - pointer to Token bucket output paramters.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_BAD_PARAM                - on wrong devNum or meterMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEntryMeterParamsCalculate
(
    IN  GT_U8                                   devNum,
    IN  PRV_TGF_POLICER_METER_MODE_ENT       meterMode,
    IN  PRV_TGF_POLICER_METER_TB_PARAMS_UNT  *tbInParamsPtr,
    OUT PRV_TGF_POLICER_METER_TB_PARAMS_UNT  *tbOutParamsPtr
);

/*******************************************************************************
* prvTgfPolicerBillingEntrySet
*
* DESCRIPTION:
*       Sets Policer Billing Counters.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of Policer Billing Counters Entry.
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out upon counter reset by
*                                     indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingEntrySet
(
    IN GT_U8                              devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U32                             entryIndex,
    IN PRV_TGF_POLICER_BILLING_ENTRY_STC *billingCntrPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT      dxChStage;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC  dxChBillingCntrSet;

    /* clear dxChBillingCntrSet */
    cpssOsMemSet(&dxChBillingCntrSet, 0, sizeof(dxChBillingCntrSet));

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert billingCntrPtr into device specific format */
    dxChBillingCntrSet.greenCntr.l[0]  = billingCntrPtr->greenCntr.l[0];
    dxChBillingCntrSet.greenCntr.l[1]  = billingCntrPtr->greenCntr.l[1];
    dxChBillingCntrSet.yellowCntr.l[0] = billingCntrPtr->yellowCntr.l[0];
    dxChBillingCntrSet.yellowCntr.l[1] = billingCntrPtr->yellowCntr.l[1];
    dxChBillingCntrSet.redCntr.l[0]    = billingCntrPtr->redCntr.l[0];
    dxChBillingCntrSet.redCntr.l[1]    = billingCntrPtr->redCntr.l[1];

    /* convert billingCntrMode into device specific format */
    switch (billingCntrPtr->billingCntrMode)
    {
        case PRV_TGF_POLICER_BILLING_CNTR_1_BYTE_E:
            dxChBillingCntrSet.billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E;
            break;

        case PRV_TGF_POLICER_BILLING_CNTR_16_BYTES_E:
            dxChBillingCntrSet.billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E;
            break;

        case PRV_TGF_POLICER_BILLING_CNTR_PACKET_E:
            dxChBillingCntrSet.billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxCh3PolicerBillingEntrySet(devNum, dxChStage, entryIndex, &dxChBillingCntrSet);
    }
    else
    {
        rc = cpssDxChPolicerPortGroupBillingEntrySet(devNum, currPortGroupsBmp, dxChStage,
                entryIndex, &dxChBillingCntrSet);
    }

    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupBillingEntrySet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(entryIndex);
    TGF_PARAM_NOT_USED(billingCntrPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerBillingEntryGet
*
* DESCRIPTION:
*       Gets Policer Billing Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of Policer Billing Counters Entry.
*       reset           - reset flag:
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read counters operation only.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingEntryGet
(
    IN  GT_U8                              devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN  GT_U32                             entryIndex,
    IN  GT_BOOL                            reset,
    OUT PRV_TGF_POLICER_BILLING_ENTRY_STC *billingCntrPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT     dxChStage;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC dxChBillingCntrSet;

    CPSS_NULL_PTR_CHECK_MAC(billingCntrPtr);

    /* clear billingCntrPtr */
    cpssOsMemSet(billingCntrPtr, 0, sizeof(*billingCntrPtr));

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxCh3PolicerBillingEntryGet(devNum, dxChStage,
                entryIndex, reset, &dxChBillingCntrSet);
    }
    else
    {
        rc = cpssDxChPolicerPortGroupBillingEntryGet(devNum, currPortGroupsBmp, dxChStage,
                entryIndex, reset, &dxChBillingCntrSet);
    }

    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupBillingEntryGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert billingCntrPtr from device specific format */
    billingCntrPtr->greenCntr.l[0]  = dxChBillingCntrSet.greenCntr.l[0];
    billingCntrPtr->greenCntr.l[1]  = dxChBillingCntrSet.greenCntr.l[1];
    billingCntrPtr->yellowCntr.l[0] = dxChBillingCntrSet.yellowCntr.l[0];
    billingCntrPtr->yellowCntr.l[1] = dxChBillingCntrSet.yellowCntr.l[1];
    billingCntrPtr->redCntr.l[0]    = dxChBillingCntrSet.redCntr.l[0];
    billingCntrPtr->redCntr.l[1]    = dxChBillingCntrSet.redCntr.l[1];

    /* convert billingCntrMode from device specific format */
    switch (dxChBillingCntrSet.billingCntrMode)
    {
        case CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E:
            billingCntrPtr->billingCntrMode = PRV_TGF_POLICER_BILLING_CNTR_1_BYTE_E;
            break;

        case CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E:
            billingCntrPtr->billingCntrMode = PRV_TGF_POLICER_BILLING_CNTR_16_BYTES_E;
            break;

        case CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E:
            billingCntrPtr->billingCntrMode = PRV_TGF_POLICER_BILLING_CNTR_PACKET_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(entryIndex);
    TGF_PARAM_NOT_USED(reset);
    TGF_PARAM_NOT_USED(billingCntrPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEntryInvalidate
*
* DESCRIPTION:
*      Invalidate Policer Entry
*
* INPUTS:
*       devNum     - device number
*       entryIndex - policer entry index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or entryIndex
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEntryInvalidate
(
    IN  GT_U32                        entryIndex
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPolicerEntryInvalidate(devNum, entryIndex);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerEntryInvalidate FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(entryIndex);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    TGF_PARAM_NOT_USED(entryIndex);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Policer Relative Qos Remarking Entry.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - Policer Stage type: Ingress #0 or Ingress #1.
*       qosProfileIndex             - index of Qos Remarking Entry will be set.
*       yellowQosTableRemarkIndex   - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Yellow color. Index range is [0..127].
*       redQosTableRemarkIndex      - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Red color. Index range is [0..127].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or qosProfileIndex.
*       GT_OUT_OF_RANGE             - on yellowQosTableRemarkIndex and
*                                     redQosTableRemarkIndex out of range.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerQosRemarkingEntrySet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                         qosProfileIndex,
    IN GT_U32                         yellowQosTableRemarkIndex,
    IN GT_U32                         redQosTableRemarkIndex
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh3PolicerQosRemarkingEntrySet(devNum, dxChStage, qosProfileIndex,
                yellowQosTableRemarkIndex, redQosTableRemarkIndex);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerQosRemarkingEntrySet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(qosProfileIndex);
    TGF_PARAM_NOT_USED(yellowQosTableRemarkIndex);
    TGF_PARAM_NOT_USED(redQosTableRemarkIndex);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerQosRemarkingEntryGet
*
* DESCRIPTION:
*       Gets Policer Relative Qos Remarking Entry.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum              - device number.
*       stage               - Policer Stage type: Ingress #0 or Ingress #1.
*       qosProfileIndex     - index of requested Qos Remarking Entry.
*
* OUTPUTS:
*       yellowQosTableRemarkIndexPtr    - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         Out-Of-Profile packets with the
*                                         Yellow color. Index range is [0..127].
*       redQosTableRemarkIndexPtr       - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         Out-Of-Profile packets with the Red
*                                         color. Index range is [0..127].
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or qosProfileIndex.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerQosRemarkingEntryGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         qosProfileIndex,
    OUT GT_U32                         *yellowQosTableRemarkIndexPtr,
    OUT GT_U32                         *redQosTableRemarkIndexPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    CPSS_NULL_PTR_CHECK_MAC(yellowQosTableRemarkIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(redQosTableRemarkIndexPtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxCh3PolicerQosRemarkingEntryGet(devNum, dxChStage, qosProfileIndex,
            yellowQosTableRemarkIndexPtr, redQosTableRemarkIndexPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerQosRemarkingEntryGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(qosProfileIndex);
    TGF_PARAM_NOT_USED(yellowQosTableRemarkIndexPtr);
    TGF_PARAM_NOT_USED(redQosTableRemarkIndexPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEgressQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Egress Policer Re-Marking table Entry.
*
* INPUTS:
*       devNum              - device number.
*       remarkTableType     - Remark table type: DSCP, EXP or TC/UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*       qosParamPtr         - pointer to the Re-Marking Entry going to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkTableType,
*                                remarkParamValue or confLevel.
*
*       GT_BAD_PTR                               - on NULL pointer.
*       GT_OUT_OF_RANGE     - on QoS parameter out of range
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressQosRemarkingEntrySet
(
    IN PRV_TGF_POLICER_REMARK_TABLE_TYPE_ENT remarkTableType,
    IN GT_U32                                remarkParamValue,
    IN CPSS_DP_LEVEL_ENT                     confLevel,
    IN PRV_TGF_POLICER_QOS_PARAM_STC         *qosParamPtr
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT dxChRemarkTableType;
    CPSS_DXCH_POLICER_QOS_PARAM_STC         dxChQosParam;

    /* convert remarkTableType into device specific format */
    switch (remarkTableType)
    {
        case PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E:
            dxChRemarkTableType = CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_TC_UP_E;
            break;

        case PRV_TGF_POLICER_REMARK_TABLE_TYPE_DSCP_E:
            dxChRemarkTableType = CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_DSCP_E;
            break;

        case PRV_TGF_POLICER_REMARK_TABLE_TYPE_EXP_E:
            dxChRemarkTableType = CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_EXP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert dxChQosParam into device specific format */
    dxChQosParam.up   = qosParamPtr->up;
    dxChQosParam.dscp = qosParamPtr->dscp;
    dxChQosParam.exp  = qosParamPtr->exp;
    dxChQosParam.dp   = qosParamPtr->dp;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPolicerEgressQosRemarkingEntrySet(devNum, dxChRemarkTableType,
                remarkParamValue, confLevel, &dxChQosParam);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerCountingColorModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(remarkTableType);
    TGF_PARAM_NOT_USED(remarkParamValue);
    TGF_PARAM_NOT_USED(confLevel);
    TGF_PARAM_NOT_USED(qosParamPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEgressQosRemarkingEntryGet
*
* DESCRIPTION:
*       Gets Egress Policer Re-Marking table Entry.
*
* INPUTS:
*       devNum              - device number.
*       remarkTableType     - Remark table type: DSCP, EXP or TC/UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*
* OUTPUTS:
*       qosParamPtr         - pointer to the requested Re-Marking Entry.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkTableType,
*                                remarkParamValue or confLevel.
*
*       GT_BAD_PTR                               - on NULL pointer.
*       GT_BAD_STATE                            - on bad value in a entry.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressQosRemarkingEntryGet
(
    IN  GT_U8                                 devNum,
    IN  PRV_TGF_POLICER_REMARK_TABLE_TYPE_ENT remarkTableType,
    IN  GT_U32                                remarkParamValue,
    IN  CPSS_DP_LEVEL_ENT                     confLevel,
    OUT PRV_TGF_POLICER_QOS_PARAM_STC         *qosParamPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT dxChRemarkTableType;
    CPSS_DXCH_POLICER_QOS_PARAM_STC         dxChQosParam;

    CPSS_NULL_PTR_CHECK_MAC(qosParamPtr);

    /* convert remarkTableType into device specific format */
    switch (remarkTableType)
    {
        case PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E:
            dxChRemarkTableType = CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_TC_UP_E;
            break;

        case PRV_TGF_POLICER_REMARK_TABLE_TYPE_DSCP_E:
            dxChRemarkTableType = CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_DSCP_E;
            break;

        case PRV_TGF_POLICER_REMARK_TABLE_TYPE_EXP_E:
            dxChRemarkTableType = CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_EXP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxChPolicerEgressQosRemarkingEntryGet(devNum, dxChRemarkTableType,
            remarkParamValue, confLevel, &dxChQosParam);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerEgressQosRemarkingEntryGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert dxChQosParam from device specific format */
    qosParamPtr->up   = dxChQosParam.up;
    qosParamPtr->dscp = dxChQosParam.dscp;
    qosParamPtr->exp  = dxChQosParam.exp;
    qosParamPtr->dp   = dxChQosParam.dp;

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(remarkTableType);
    TGF_PARAM_NOT_USED(remarkParamValue);
    TGF_PARAM_NOT_USED(confLevel);
    TGF_PARAM_NOT_USED(qosParamPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerStageMeterModeSet
*
* DESCRIPTION:
*       Sets Policer Global stage mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - Policer meter mode: FLOW or PORT.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerStageMeterModeSet
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN  PRV_TGF_POLICER_STAGE_METER_MODE_ENT mode
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT       dxChStage;
    CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT dxChMode;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert mode into device specific format */
    switch (mode)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E,
                                PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                CPSS_DXCH_POLICER_STAGE_METER_MODE_FLOW_E,
                                PRV_TGF_POLICER_STAGE_METER_MODE_FLOW_E);

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPolicerStageMeterModeSet(devNum, dxChStage, dxChMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerStageMeterModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(mode);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerStageMeterModeGet
*
* DESCRIPTION:
*       Gets Policer Global stage mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - (pointer to) Policer meter mode: FLOW or PORT.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerStageMeterModeGet
(
    IN  GT_U8                                devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    OUT PRV_TGF_POLICER_STAGE_METER_MODE_ENT *modePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT       dxChStage;
    CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT dxChMode;

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChPolicerStageMeterModeGet(devNum, dxChStage, &dxChMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh3PolicerStageMeterModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert dxChResolution into device specific format */
    switch (dxChMode)
    {
        case CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E:
            *modePtr = PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E;
            break;

        case CPSS_DXCH_POLICER_STAGE_METER_MODE_FLOW_E:
            *modePtr = PRV_TGF_POLICER_STAGE_METER_MODE_FLOW_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(modePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPortStormTypeIndexSet
*
* DESCRIPTION:
*       Associates policing profile with source/target port and storm rate type.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       stormType       - storm type
*       index           - policer index (0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, stormType ot port.
*       GT_OUT_OF_RANGE             - on out of range of index.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPortStormTypeIndexSet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN GT_U8                                portNum,
    IN PRV_TGF_POLICER_STORM_TYPE_ENT       stormType,
    IN GT_U32                               index
)
{

#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT       dxChStage;
    CPSS_DXCH_POLICER_STORM_TYPE_ENT       dxChStormType;


    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert command into device specific format */
    switch (stormType)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChStormType,
                                PRV_TGF_POLICER_STORM_TYPE_UC_KNOWN_E,
                                CPSS_DXCH_POLICER_STORM_TYPE_UC_KNOWN_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChStormType,
                                PRV_TGF_POLICER_STORM_TYPE_UC_UNKNOWN_E,
                                CPSS_DXCH_POLICER_STORM_TYPE_UC_UNKNOWN_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChStormType,
                                PRV_TGF_POLICER_STORM_TYPE_MC_UNREGISTERED_E,
                                CPSS_DXCH_POLICER_STORM_TYPE_MC_UNREGISTERED_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChStormType,
                                PRV_TGF_POLICER_STORM_TYPE_MC_REGISTERED_E,
                                CPSS_DXCH_POLICER_STORM_TYPE_MC_REGISTERED_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChStormType,
                                PRV_TGF_POLICER_STORM_TYPE_BC_E,
                                CPSS_DXCH_POLICER_STORM_TYPE_BC_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChStormType,
                                PRV_TGF_POLICER_STORM_TYPE_TCP_SYN_E,
                                CPSS_DXCH_POLICER_STORM_TYPE_TCP_SYN_E);
        
        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxChPolicerPortStormTypeIndexSet(devNum, dxChStage, portNum, 
                                              dxChStormType, index);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortStormTypeIndexSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(stormType);
    TGF_PARAM_NOT_USED(index);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEgressL2RemarkModelSet
*
* DESCRIPTION:
*       Sets Egress Policer L2 packets remarking model.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       model           - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table:
*                         User Priority or Traffic Class.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressL2RemarkModelSet
(
    IN PRV_TGF_POLICER_L2_REMARK_MODEL_ENT model
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT dxChModel;

    /* convert model into device specific format */
    switch (model)
    {
        case PRV_TGF_POLICER_L2_REMARK_MODEL_TC_E:
            dxChModel = CPSS_DXCH_POLICER_L2_REMARK_MODEL_TC_E;
            break;

        case PRV_TGF_POLICER_L2_REMARK_MODEL_UP_E:
            dxChModel = CPSS_DXCH_POLICER_L2_REMARK_MODEL_UP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPolicerEgressL2RemarkModelSet(devNum, dxChModel);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerEgressL2RemarkModelSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(model);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEgressL2RemarkModelGet
*
* DESCRIPTION:
*       Gets Egress Policer L2 packets remarking model.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modelPtr        - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressL2RemarkModelGet
(
    IN  GT_U8                                    devNum,
    OUT PRV_TGF_POLICER_L2_REMARK_MODEL_ENT    *modelPtr
);

/*******************************************************************************
* prvTgfPolicerEgressQosUpdateEnableSet
*
* DESCRIPTION:
*       The function enables or disables QoS remarking of conforming packets.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - Enable/disable Qos update for conforming packets:
*                 GT_TRUE  - Remark Qos parameters of conforming packets.
*                 GT_FALSE - Keep incoming Qos parameters of conforming packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressQosUpdateEnableSet
(
    IN GT_BOOL  enable
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPolicerEgressQosUpdateEnableSet(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerEgressQosUpdateEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(enable);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerEgressQosUpdateEnableGet
*
* DESCRIPTION:
*       The function get QoS remarking status of conforming packets
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       enablePtr  - Enable/disable Qos update for conforming packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressQosUpdateEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* prvTgfPolicerVlanCntrSet
*
* DESCRIPTION:
*      Sets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*       cntrValue       - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled VLAN Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries) or stage.
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCntrSet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U16                         vid,
    IN  GT_U32                         cntrValue
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChPolicerVlanCntrSet(devNum, dxChStage, vid, cntrValue);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerVlanCntrSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(vid);
    TGF_PARAM_NOT_USED(cntrValue);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerVlanCntrGet
*
* DESCRIPTION:
*      Gets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*
* OUTPUTS:
*       cntrValuePtr    - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled VLAN Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries) or stage.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCntrGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U16                         vid,
    OUT GT_U32                         *cntrValuePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChPolicerVlanCntrGet(devNum, dxChStage, vid, cntrValuePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerVlanCntrGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(vid);
    TGF_PARAM_NOT_USED(cntrValuePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerVlanCountingPacketCmdTriggerSet
*
* DESCRIPTION:
*       Enables or disables VLAN counting triggering according to the
*       specified packet command.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       cmdTrigger      - The packet command trigger.
*       enable          - Enable/Disable VLAN Counting according to the packet
*                         command trigger
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_SUPPORTED         - on not supported egress direction
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCountingPacketCmdTriggerSet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN CPSS_PACKET_CMD_ENT            cmdTrigger,
    IN GT_BOOL                        enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChPolicerVlanCountingPacketCmdTriggerSet(devNum, dxChStage,
            cmdTrigger, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerVlanCountingPacketCmdTriggerSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(cmdTrigger);
    TGF_PARAM_NOT_USED(enable);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerVlanCountingPacketCmdTriggerGet
*
* DESCRIPTION:
*       Gets VLAN counting triggering status (Enable/Disable) according to the
*       specified packet command.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       cmdTrigger      - The packet command trigger.
*
* OUTPUTS:
*       enablePtr       - Pointer to Enable/Disable VLAN Counting according
*                         to the packet command trigger:
*                         GT_TRUE  - Enable VLAN counting on packet with
*                                    triggered packet command.
*                         GT_FALSE - Disable VLAN counting on packet with
*                                    triggered packet command.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCountingPacketCmdTriggerGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  CPSS_PACKET_CMD_ENT            cmdTrigger,
    OUT GT_BOOL                        *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChPolicerVlanCountingPacketCmdTriggerGet(devNum, dxChStage,
            cmdTrigger, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerVlanCountingPacketCmdTriggerGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(cmdTrigger);
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}


/*******************************************************************************
* prvTgfPolicerPolicyCntrSet
*
* DESCRIPTION:
*      Sets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*       cntrValue       - packets counter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or index (index range is limited by
*                         max number of Policer Policy counters).
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPolicyCntrSet
(
    IN GT_U8                           devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         index,
    IN  GT_U32                         cntrValue
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxChPolicerPolicyCntrSet(devNum, dxChStage, index, cntrValue);
    }
    else
    {
        rc = cpssDxChPolicerPortGroupPolicyCntrSet(devNum, currPortGroupsBmp, dxChStage, index, cntrValue);
    }

    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupPolicyCntrSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(cntrValue);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPolicyCntrGet
*
* DESCRIPTION:
*      Gets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*
* OUTPUTS:
*       cntrValuePtr    - Pointer to the packet counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or index.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_TIMEOUT               - on time out.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPolicyCntrGet
(
    IN GT_U8                           devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         index,
    OUT GT_U32                         *cntrValuePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    if(usePortGroupsBmp == GT_FALSE)
    {
        rc = cpssDxChPolicerPolicyCntrGet(devNum, dxChStage, index, cntrValuePtr);
    }
    else
    {
        rc = cpssDxChPolicerPortGroupPolicyCntrGet(devNum, currPortGroupsBmp, dxChStage, index, cntrValuePtr);
    }

    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortGroupPolicyCntrGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(cntrValuePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerCountersSet
*
* DESCRIPTION:
*      Set Policer Counters. To reset counters use zero values.
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index
*       countersPtr     - (pointer to) counters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or counterSetIndex
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountersSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterSetIndex,
    IN  PRV_TGF_POLICER_COUNTERS_STC *countersPtr
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_POLICER_COUNTERS_STC dxChCounters;

    /* convert countersPtr into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChCounters), countersPtr, outOfProfileBytesCnt);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChCounters), countersPtr, inProfileBytesCnt);

    /* call device specific API */
    return cpssDxChPolicerCountersSet(devNum, counterSetIndex, &dxChCounters);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssExMxPmPolicerPolicyCntrSet(devNum, CPSS_DIRECTION_INGRESS_E, counterSetIndex,
                                        countersPtr->inProfileBytesCnt);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPolicerPolicyCntrSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(counterSetIndex);
    TGF_PARAM_NOT_USED(countersPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerCountersGet
*
* DESCRIPTION:
*      Get Policer Counters
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index
*
* OUTPUTS:
*       countersPtr - (pointer to) counters
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or counterSetIndex
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*       GT_TIMEOUT   - on max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountersGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterSetIndex,
    OUT PRV_TGF_POLICER_COUNTERS_STC *countersPtr
)
{
    GT_STATUS rc;
#ifdef CHX_FAMILY
    CPSS_DXCH_POLICER_COUNTERS_STC dxChCounters;
    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    /* call device specific API */
    rc = cpssDxChPolicerCountersGet(devNum, counterSetIndex, &dxChCounters);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerCountersGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert countersPtr from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChCounters), countersPtr, outOfProfileBytesCnt);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChCounters), countersPtr, inProfileBytesCnt);

    return GT_OK;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    /* call device specific API */
    rc = cpssExMxPmPolicerPolicyCntrGet(devNum, CPSS_DIRECTION_INGRESS_E, counterSetIndex,
                                        &(countersPtr->inProfileBytesCnt));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPolicerPolicyCntrGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(counterSetIndex);
    TGF_PARAM_NOT_USED(countersPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMemorySizeModeSet
*
* DESCRIPTION:
*      Sets internal table sizes and the way they are shared between the Ingress
*      policers.
*
* INPUTS:
*       devNum          - device number.
*       mode            - The mode in which internal tables are shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong input parameters.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMemorySizeModeSet
(
    IN GT_U8                                devNum,
    IN PRV_TGF_POLICER_MEMORY_CTRL_MODE_ENT mode
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT dxChMode;

    /* convert command into device specific format */
    switch (mode)
    {
        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_4_E,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_4_E);

        PRV_TGF_SWITCH_CASE_MAC(dxChMode,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_5_E,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_5_E);
        
        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxChPolicerMemorySizeModeSet(devNum, dxChMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerMemorySizeModeSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(mode);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerMemorySizeModeGet
*
* DESCRIPTION:
*      Gets internal table sizes and the way they are shared between the Ingress
*      policers.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       modePtr         - The mode in which internal tables are shared.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMemorySizeModeGet
(
    IN  GT_U8                                      devNum,
    OUT PRV_TGF_POLICER_MEMORY_CTRL_MODE_ENT     *modePtr
)
{

#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT dxChMode;

    /* call device specific API */
    rc = cpssDxChPolicerMemorySizeModeGet(devNum, &dxChMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerMemorySizeModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert command into device specific format */
    switch (dxChMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(*modePtr,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E);

        PRV_TGF_SWITCH_CASE_MAC(*modePtr,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E);

        PRV_TGF_SWITCH_CASE_MAC(*modePtr,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E);

        PRV_TGF_SWITCH_CASE_MAC(*modePtr,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E);

        PRV_TGF_SWITCH_CASE_MAC(*modePtr,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_4_E,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_4_E);

        PRV_TGF_SWITCH_CASE_MAC(*modePtr,
                                CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_5_E,
                                PRV_TGF_POLICER_MEMORY_CTRL_MODE_5_E);
        
        default:
            return GT_BAD_STATE;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(modePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerCountingWriteBackCacheFlush
*
* DESCRIPTION:
*       Flush internal Write Back Cache (WBC) of counting entries.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or stage.
*       GT_HW_ERROR              - on Hardware error.
*       GT_TIMEOUT               - on time out.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingWriteBackCacheFlush
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage
)
{
#ifdef CHX_FAMILY
    GT_U8     devNum;
    GT_STATUS rc, rc1 = GT_OK;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, dxChStage);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerCountingWriteBackCacheFlush FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerConformanceLevelForce
*
* DESCRIPTION:
*    force the conformance level for the packets entering the policer
*       (traffic cond)
* INPUTS:
*       dp -  conformance level (drop precedence) - green/yellow/red
*
* RETURNS:
*       GT_OK - success, GT_FAIL otherwise
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfPolicerConformanceLevelForce(
    IN  GT_U32      dp
)
{
#ifdef ASIC_SIMULATION
    GT_STATUS rc;

    /* set the conformance level */
    rc = skernelPolicerConformanceLevelForce(dp);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: skernelPolicerConformanceLevelForce FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif

#if !(defined ASIC_SIMULATION)
    TGF_PARAM_NOT_USED(dp);

    return GT_OK;
#endif /* !(defined CHX_FAMILY) */
}

#ifdef CHX_FAMILY
static GT_STATUS prvTgfDxChPlrStageCheck
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     dxChStage
)
{
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum,dxChStage);
    return GT_OK;
}
#endif /* CHX_FAMILY */

/*******************************************************************************
* prvTgfPolicerStageCheck
*
* DESCRIPTION:
*    check if the device supports this policer stage
* INPUTS:
*       stage -  policer stage to check
*
* RETURNS:
*       GT_TRUE - the device supports the stage
*       GT_FALSE - the device not supports the stage
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfPolicerStageCheck(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    GT_U8     devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfPolicerStageCheck FAILED, rc = [%d]", rc);
        return GT_FALSE;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        if(GT_OK != prvTgfDxChPlrStageCheck(devNum,dxChStage))
        {
            /* the device not support the state */
            return GT_FALSE;
        }
    }

    return GT_TRUE;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(stage);

    return GT_FALSE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPolicerPortModeAddressSelectSet
*
* DESCRIPTION:
*       Configure Metering Address calculation type.
*       Relevant when stage mode is CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E. 
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       type        - Address select type: Full or Compressed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or type.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPortModeAddressSelectSet
(
    IN  GT_U8                                               devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT                      stage,
    IN  PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT      type
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT      dxChType;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT                      dxChStage;

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert command into device specific format */
    dxChType = (type == PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E) ?
        CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E : 
        CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E;

    /* call device specific API */
    rc = cpssDxChPolicerPortModeAddressSelectSet(devNum, dxChStage, dxChType);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortModeAddressSelectSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(type);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}


/*******************************************************************************
* prvTgfPolicerPortModeAddressSelectGet
*
* DESCRIPTION:
*       Get Metering Address calculation type.
*       Relevant when stage mode is CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E. 
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       typePtr     - (pointer to) Address select type: Full or Compressed.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPortModeAddressSelectGet
(
    IN  GT_U8                                               devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT                      stage,
    OUT PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT      *typePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT      dxChType;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT                      dxChStage;


    CPSS_NULL_PTR_CHECK_MAC(typePtr);

    /* convert stage into device specific format */
    rc = prvTgfConvertGenericToDxChPolicerStage(stage, &dxChStage);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicerStage FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChPolicerPortModeAddressSelectGet(devNum, dxChStage, &dxChType);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPolicerPortModeAddressSelectGet FAILED, rc = [%d]", rc);

        return rc;
    }


    /* convert command into device specific format */
    *typePtr = (dxChType == CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E) ?
        PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E :
        PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E;

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(stage);
    TGF_PARAM_NOT_USED(typePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}
