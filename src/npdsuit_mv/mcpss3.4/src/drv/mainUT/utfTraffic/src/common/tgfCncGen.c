/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCncGen.c
*
* DESCRIPTION:
*       Generic API implementation for Cnc
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssMath.h>

#include <utf/private/prvUtfHelpers.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <common/tgfCommon.h>
#include <common/tgfCncGen.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* Functions to convert PRV_TGF_CNC_ to/from CPSS_DXCH_CNC_ */

static void cnvCNC_CLIENT_ENT_tgf2dxch
(
    IN  PRV_TGF_CNC_CLIENT_ENT   tgfCncClient,
    OUT CPSS_DXCH_CNC_CLIENT_ENT *dxchCncClientPtr
)
{
    switch (tgfCncClient)
    {
        case PRV_TGF_CNC_CLIENT_L2L3_INGRESS_VLAN_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
            break;
        case PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E;
            break;
        case PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E;
            break;
        case PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E;
            break;
        case PRV_TGF_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E;
            break;
        case PRV_TGF_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E;
            break;
        case PRV_TGF_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;
            break;
        case PRV_TGF_CNC_CLIENT_EGRESS_PCL_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E;
            break;
        case PRV_TGF_CNC_CLIENT_ARP_TABLE_ACCESS_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E;
            break;
        case PRV_TGF_CNC_CLIENT_TUNNEL_START_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E;
            break;
        case PRV_TGF_CNC_CLIENT_TTI_E:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_TTI_E;
            break;
        default:
            *dxchCncClientPtr = CPSS_DXCH_CNC_CLIENT_LAST_E;
            break;
    }
}


static void cnvCNC_BYTE_COUNT_MODE_ENT_tgf2dxch
(
    IN  PRV_TGF_CNC_BYTE_COUNT_MODE_ENT   tgfCncByteCountMode,
    OUT CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT *dxchCncByteCountModePtr
)
{
    switch (tgfCncByteCountMode)
    {
        case PRV_TGF_CNC_BYTE_COUNT_MODE_L2_E:
            *dxchCncByteCountModePtr = CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E;
            break;
        case PRV_TGF_CNC_BYTE_COUNT_MODE_L3_E:
            *dxchCncByteCountModePtr = CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E;
            break;
        default:
            /* wrong value */
            *dxchCncByteCountModePtr = CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E + 1;
            break;
    }
}

static void cnvCNC_BYTE_COUNT_MODE_ENT_dxch2tgf
(
    IN  CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT dxchCncByteCountMode,
    OUT PRV_TGF_CNC_BYTE_COUNT_MODE_ENT   *tgfCncByteCountModePtr
)
{
    switch (dxchCncByteCountMode)
    {
        case CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E:
            *tgfCncByteCountModePtr = PRV_TGF_CNC_BYTE_COUNT_MODE_L2_E;
            break;
        case CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E:
            *tgfCncByteCountModePtr = PRV_TGF_CNC_BYTE_COUNT_MODE_L3_E;
            break;
        default:
            /* wrong value */
            *tgfCncByteCountModePtr = PRV_TGF_CNC_BYTE_COUNT_MODE_L3_E + 1;
            break;
    }
}

static void cnvCNC_EGRESS_DROP_COUNT_ENT_tgf2dxch
(
    IN  PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_ENT   tgfCncEgrDropCountMode,
    OUT CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT *dxchCncEgrDropCountModePtr
)
{
    switch (tgfCncEgrDropCountMode)
    {
        case PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E:
            *dxchCncEgrDropCountModePtr =
                CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;
            break;
        case PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E:
            *dxchCncEgrDropCountModePtr =
                CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E;
            break;
        case PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E:
            *dxchCncEgrDropCountModePtr =
                CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E;
            break;
        default:
            /* wrong value */
            *dxchCncEgrDropCountModePtr =
                CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E + 1;
            break;
    }
}

static void cnvCNC_EGRESS_DROP_COUNT_ENT_dxch2tgf
(
    IN  CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT dxchCncEgrDropCountMode,
    OUT PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_ENT   *tgfCncEgrDropCountModePtr
)
{
    switch (dxchCncEgrDropCountMode)
    {
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E:
            *tgfCncEgrDropCountModePtr =
                PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;
            break;
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E:
            *tgfCncEgrDropCountModePtr =
                PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E;
            break;
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E:
            *tgfCncEgrDropCountModePtr =
                PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E;
            break;
        default:
            /* wrong value */
            *tgfCncEgrDropCountModePtr =
                PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E + 1;
            break;
    }
}

static void cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch
(
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT   tgfCncCounterFormat,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT *dxchCncCounterFormatPtr
)
{
    switch (tgfCncCounterFormat)
    {
        case PRV_TGF_CNC_COUNTER_FORMAT_MODE_0_E:
            *dxchCncCounterFormatPtr =
                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;
            break;
        case PRV_TGF_CNC_COUNTER_FORMAT_MODE_1_E:
            *dxchCncCounterFormatPtr =
                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;
            break;
        case PRV_TGF_CNC_COUNTER_FORMAT_MODE_2_E:
            *dxchCncCounterFormatPtr =
                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;
            break;
        default:
            /* wrong value */
            *dxchCncCounterFormatPtr =
                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E + 1;
            break;
    }
}

static void cnvCNC_COUNTER_FORMAT_ENT_dxch2tgf
(
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT dxchCncCounterFormat,
    OUT PRV_TGF_CNC_COUNTER_FORMAT_ENT   *tgfCncCounterFormatPtr
)
{
    switch (dxchCncCounterFormat)
    {
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E:
            *tgfCncCounterFormatPtr =
                PRV_TGF_CNC_COUNTER_FORMAT_MODE_0_E;
            break;
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E:
            *tgfCncCounterFormatPtr =
                PRV_TGF_CNC_COUNTER_FORMAT_MODE_1_E;
            break;
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E:
            *tgfCncCounterFormatPtr =
                PRV_TGF_CNC_COUNTER_FORMAT_MODE_2_E;
            break;
        default:
            /* wrong value */
            *tgfCncCounterFormatPtr =
                PRV_TGF_CNC_COUNTER_FORMAT_MODE_2_E + 1;
            break;
    }
}

static void cnvCNC_CNC_COUNTER_STC_tgf2dxch
(
    IN  PRV_TGF_CNC_COUNTER_STC       *tgfCncCounrerPtr,
    OUT CPSS_DXCH_CNC_COUNTER_STC     *dxchCncCounrerPtr
)
{
    dxchCncCounrerPtr->byteCount   = tgfCncCounrerPtr->byteCount;
    dxchCncCounrerPtr->packetCount = tgfCncCounrerPtr->packetCount;
}

static void cnvCNC_CNC_COUNTER_STC_dxch2tgf
(
    IN  CPSS_DXCH_CNC_COUNTER_STC     *dxchCncCounrerPtr,
    OUT PRV_TGF_CNC_COUNTER_STC       *tgfCncCounrerPtr
)
{
    tgfCncCounrerPtr->byteCount   = dxchCncCounrerPtr->byteCount;
    tgfCncCounrerPtr->packetCount = dxchCncCounrerPtr->packetCount;
}

static void cnvCNC_COUNTING_ENABLE_UNIT_ENT_tgf2dxch
(
    IN  PRV_TGF_CNC_COUNTING_ENABLE_UNIT_ENT   tgfCncCounterEnUnit,
    OUT CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT *dxchCncCounterEnUnitPtr
)
{
    switch (tgfCncCounterEnUnit)
    {
        case PRV_TGF_CNC_COUNTING_ENABLE_UNIT_TTI_E:
            *dxchCncCounterEnUnitPtr =
                CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E;
            break;
        case PRV_TGF_CNC_COUNTING_ENABLE_UNIT_PCL_E:
            *dxchCncCounterEnUnitPtr =
                CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E;
            break;
        default:
            /* wrong value */
            *dxchCncCounterEnUnitPtr =
                CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E + 1;
            break;
    }
}

static void cnvCNC_EGRESS_QUEUE_CLIENT_MODE_ENT_tgf2dxch
(
    IN  PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT   tgfCncEgrQueClientMode,
    OUT CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT *dxchCncEgrQueClientModePtr
)
{
    switch (tgfCncEgrQueClientMode)
    {
        case PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E:
            *dxchCncEgrQueClientModePtr =
                CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E;
            break;
        case PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E:
            *dxchCncEgrQueClientModePtr =
                CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E;
            break;
        default:
            /* wrong value */
            *dxchCncEgrQueClientModePtr =
                CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E + 1;
            break;
    }
}

static void cnvCNC_EGRESS_QUEUE_CLIENT_MODE_ENT_dxch2tgf
(
    IN  CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT dxchCncEgrQueClientMode,
    OUT PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT   *tgfCncEgrQueClientModePtr
)
{
    switch (dxchCncEgrQueClientMode)
    {
        case CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E:
            *tgfCncEgrQueClientModePtr =
                PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E;
            break;
        case CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E:
            *tgfCncEgrQueClientModePtr =
                PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E;
            break;
        default:
            /* wrong value */
            *tgfCncEgrQueClientModePtr =
                PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E + 1;
            break;
    }
}

#endif /* CHX_FAMILY */

/******************************************************************************\
 *                           Private declarations                             *
\******************************************************************************/

/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/


/*******************************************************************************
* prvTgfCncBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* INPUTS:
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockClientEnableSet
(
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    IN  GT_BOOL                   updateEnable
)
{
#ifdef CHX_FAMILY

    GT_U8                    devNum  = 0;
    GT_STATUS                rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncBlockClientEnableSet(
            devNum, blockNum,
            dxchClient, updateEnable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncBlockClientEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(updateEnable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* INPUTS:
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockClientEnableGet
(
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncBlockClientEnableGet(
        devNum, blockNum, dxchClient, updateEnablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncBlockClientEnableGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(updateEnablePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* INPUTS:
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*       indexRangesBmp   - the counter index ranges bitmap
*                         DxCh3 and DxChXcat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         The Lion devices has 64 ranges (512 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockClientRangesSet
(
    IN  GT_U32                      blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT      client,
    IN  GT_U64                      indexRangesBmp
)
{
#ifdef CHX_FAMILY

    GT_U8                    devNum  = 0;
    GT_STATUS                rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncBlockClientRangesSet(
            devNum, blockNum,
            dxchClient, indexRangesBmp);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncBlockClientRangesSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(indexRangesBmp);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* INPUTS:
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) the counter index ranges bitmap
*                         DxCh3 and DxChXcat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         The Lion devices has 64 ranges (512 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockClientRangesGet
(
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_U64                    *indexRangesBmpPtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncBlockClientRangesGet(
        devNum, blockNum, dxchClient, indexRangesBmpPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncBlockClientRangesGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(indexRangesBmpPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortClientEnableSet
*
* DESCRIPTION:
*   The function sets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* INPUTS:
*       portIndex        - port index in UTF array
*       client           - CNC client
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortClientEnableSet
(
    IN  GT_U8                     portIndex,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    IN  GT_BOOL                   enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS                rc;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);


    /* call device specific API */
    rc = cpssDxChCncPortClientEnableSet(
        prvTgfDevsArray[portIndex], prvTgfPortsArray[portIndex],
        dxchClient, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortClientEnableSet FAILED, rc = [%d]", rc);
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portIndex);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(enable);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortClientEnableGet
*
* DESCRIPTION:
*   The function gets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* INPUTS:
*       portIndex        - port index in UTF array
*       client           - CNC client
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortClientEnableGet
(
    IN  GT_U8                     portIndex,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_BOOL                   *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                rc;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);


    /* call device specific API */
    rc = cpssDxChCncPortClientEnableGet(
        prvTgfDevsArray[portIndex], prvTgfPortsArray[portIndex],
        dxchClient, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortClientEnableGet FAILED, rc = [%d]", rc);
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portIndex);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncIngressVlanPassDropFromCpuCountEnableSet
*
* DESCRIPTION:
*   The function enables or disables counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* INPUTS:
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncIngressVlanPassDropFromCpuCountEnableSet
(
    IN  GT_BOOL enable
)
{
#ifdef CHX_FAMILY

    GT_U8                    devNum  = 0;
    GT_STATUS                rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet(
            devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncIngressVlanPassDropFromCpuCountEnableGet
*
* DESCRIPTION:
*   The function gets status of counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* INPUTS:
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncIngressVlanPassDropFromCpuCountEnableGet
(
    OUT GT_BOOL *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(
        devNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncClientByteCountModeSet
*
* DESCRIPTION:
*   The function sets byte count counter mode for CNC client.
*
* INPUTS:
*       client           - CNC client
*       countMode        - count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncClientByteCountModeSet
(
    IN  PRV_TGF_CNC_CLIENT_ENT            client,
    IN  PRV_TGF_CNC_BYTE_COUNT_MODE_ENT   countMode
)
{
#ifdef CHX_FAMILY

    GT_U8                             devNum  = 0;
    GT_STATUS                         rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT          dxchClient;
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT dxchCncByteCountMode;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);

    cnvCNC_BYTE_COUNT_MODE_ENT_tgf2dxch(countMode, &dxchCncByteCountMode);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncClientByteCountModeSet(
            devNum, dxchClient, dxchCncByteCountMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncClientByteCountModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(countMode);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncClientByteCountModeGet
*
* DESCRIPTION:
*   The function gets byte count counter mode for CNC client.
*
* INPUTS:
*       client           - CNC client
*
* OUTPUTS:
*       countModePtr     - (pointer to) count mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncClientByteCountModeGet
(
    IN  PRV_TGF_CNC_CLIENT_ENT            client,
    OUT PRV_TGF_CNC_BYTE_COUNT_MODE_ENT   *countModePtr
)
{
#ifdef CHX_FAMILY
    GT_U8                             devNum  = 0;
    GT_STATUS                         rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT          dxchClient;
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT dxchCncByteCountMode;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncClientByteCountModeGet(
        devNum, dxchClient, &dxchCncByteCountMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncClientByteCountModeGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }
    cnvCNC_BYTE_COUNT_MODE_ENT_dxch2tgf(dxchCncByteCountMode, countModePtr);
    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(countModePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncEgressVlanDropCountModeSet
*
* DESCRIPTION:
*   The function sets Egress VLAN Drop counting mode.
*
* INPUTS:
*       mode             - Egress VLAN Drop counting mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncEgressVlanDropCountModeSet
(
    IN  PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_ENT   mode
)
{
#ifdef CHX_FAMILY

    GT_U8                                    devNum  = 0;
    GT_STATUS                                rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT dxchEgrDropCountMode;

    cnvCNC_EGRESS_DROP_COUNT_ENT_tgf2dxch(mode, &dxchEgrDropCountMode);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncEgressVlanDropCountModeSet(
            devNum, dxchEgrDropCountMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncEgressVlanDropCountModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(mode);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncEgressVlanDropCountModeGet
*
* DESCRIPTION:
*   The function gets Egress VLAN Drop counting mode.
*
*
* INPUTS:
*
* OUTPUTS:
*       modePtr             - (pointer to) Egress VLAN Drop counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - on reserved value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncEgressVlanDropCountModeGet
(
    OUT PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_ENT   *modePtr
)
{
#ifdef CHX_FAMILY
    GT_U8                                    devNum  = 0;
    GT_STATUS                                rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT dxchEgrDropCountMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncEgressVlanDropCountModeGet(
        devNum, &dxchEgrDropCountMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncEgressVlanDropCountModeGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    cnvCNC_EGRESS_DROP_COUNT_ENT_dxch2tgf(dxchEgrDropCountMode, modePtr);
    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(modePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncCounterClearByReadEnableSet
*
* DESCRIPTION:
*   The function enable/disables clear by read mode of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* INPUTS:
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterClearByReadEnableSet
(
    IN  GT_BOOL  enable
)
{
#ifdef CHX_FAMILY

    GT_U8                    devNum  = 0;
    GT_STATUS                rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncCounterClearByReadEnableSet(
            devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterClearByReadEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncCounterClearByReadEnableGet
*
* DESCRIPTION:
*   The function gets clear by read mode status of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* INPUTS:
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterClearByReadEnableGet
(
    OUT GT_BOOL  *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncCounterClearByReadEnableGet(
        devNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterClearByReadEnableGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncCounterClearByReadValueSet
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* INPUTS:
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterClearByReadValueSet
(
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    IN  PRV_TGF_CNC_COUNTER_STC           *counterPtr
)
{
#ifdef CHX_FAMILY

    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCounter;

    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCounterFormat);
    cnvCNC_CNC_COUNTER_STC_tgf2dxch(counterPtr, &dxchCounter);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncCounterClearByReadValueSet(
            devNum, dxchCounterFormat, &dxchCounter);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterClearByReadValueSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(format);
    TGF_PARAM_NOT_USED(counterPtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncCounterClearByReadValueGet
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
*
* INPUTS:
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterClearByReadValueGet
(
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT PRV_TGF_CNC_COUNTER_STC           *counterPtr
)
{
#ifdef CHX_FAMILY
    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCounter;

    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCounterFormat);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncCounterClearByReadValueGet(
        devNum, dxchCounterFormat, &dxchCounter);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterClearByReadValueGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    cnvCNC_CNC_COUNTER_STC_dxch2tgf(&dxchCounter, counterPtr);
    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(format);
    TGF_PARAM_NOT_USED(counterPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncCounterWraparoundEnableSet
*
* DESCRIPTION:
*   The function enables/disables wraparound for all CNC counters
*
* INPUTS:
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterWraparoundEnableSet
(
    IN  GT_BOOL  enable
)
{
#ifdef CHX_FAMILY

    GT_U8                    devNum  = 0;
    GT_STATUS                rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncCounterWraparoundEnableSet(
            devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterWraparoundEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncCounterWraparoundEnableGet
*
* DESCRIPTION:
*   The function gets status of wraparound for all CNC counters
*
* INPUTS:
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterWraparoundEnableGet
(
    OUT GT_BOOL  *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncCounterWraparoundEnableGet(
        devNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterWraparoundEnableGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* INPUTS:
*       blockNum     - CNC block number
*                      valid range see in datasheet of specific device.
*       indexNumPtr  - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr  - (pointer to) actual size of array of indexes
*       indexesArr[] - (pointer to) array of indexes of counters wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterWraparoundIndexesGet
(
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncCounterWraparoundIndexesGet(
        devNum, blockNum, indexNumPtr, indexesArr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterWraparoundIndexesGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(indexNumPtr);
    TGF_PARAM_NOT_USED(indexesArr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* INPUTS:
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterSet
(
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    IN  PRV_TGF_CNC_COUNTER_STC           *counterPtr
)
{
#ifdef CHX_FAMILY

    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCounter;

    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCounterFormat);
    cnvCNC_CNC_COUNTER_STC_tgf2dxch(counterPtr, &dxchCounter);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncCounterSet(
            devNum, blockNum, index, dxchCounterFormat, &dxchCounter);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }

        /* set the value to first device only, 0 to all others */
        cpssOsMemSet(&dxchCounter, 0, sizeof(dxchCounter));
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(format);
    TGF_PARAM_NOT_USED(counterPtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* INPUTS:
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterGet
(
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT PRV_TGF_CNC_COUNTER_STC           *counterPtr
)
{
#ifdef CHX_FAMILY

    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCounter;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCounterSum;

    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCounterFormat);

    cpssOsMemSet(&dxchCounterSum, 0, sizeof(dxchCounterSum));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncCounterGet(
            devNum, blockNum, index, dxchCounterFormat, &dxchCounter);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterGet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }

        /* sum over all devices */
        dxchCounterSum.packetCount =
            prvCpssMathAdd64(
                dxchCounterSum.packetCount, dxchCounter.packetCount);
        dxchCounterSum.byteCount =
            prvCpssMathAdd64(
                dxchCounterSum.byteCount, dxchCounter.byteCount);
    }

    cnvCNC_CNC_COUNTER_STC_dxch2tgf(&dxchCounterSum, counterPtr);
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(format);
    TGF_PARAM_NOT_USED(counterPtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   prvTgfCncBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by prvTgfCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChCncBlockUploadTrigger(
        devNum, blockNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncBlockUploadTrigger FAILED, rc = [%d]", rc);

    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(blockNum);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       inProcessBlocksBmpPtr    - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *inProcessBlocksBmpPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChCncBlockUploadInProcessGet(
        devNum, inProcessBlocksBmpPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncBlockUploadInProcessGet FAILED, rc = [%d]", rc);

    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(inProcessBlocksBmpPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by prvTgfCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the prvTgfCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       prvTgfCncUploadedBlockGet one more time to get rest of the block.
*
* INPUTS:
*       devNum                - device number
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by prvTgfCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS prvTgfCncUploadedBlockGet
(
    IN  GT_U8                                devNum,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT    PRV_TGF_CNC_COUNTER_STC           *counterValuesPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                           rc = GT_OK;
    GT_U32                              numOfCounterValues;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCncCounterFormat;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCncCounter;
    GT_U32                              counterAmount;


    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCncCounterFormat);

    for (numOfCounterValues = 0, counterAmount = 1;
          (numOfCounterValues < (*numOfCounterValuesPtr));
          counterValuesPtr ++, numOfCounterValues ++)
    {
        /* call device specific API */
        rc = cpssDxChCncUploadedBlockGet(
            devNum, &counterAmount, dxchCncCounterFormat, &dxchCncCounter);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncBlockUploadInProcessGet FAILED, rc = [%d]", rc);
            break;
        }

        if (counterAmount == 0)
        {
            break;
        }

        cnvCNC_CNC_COUNTER_STC_dxch2tgf(&dxchCncCounter, counterValuesPtr);
    }

    *numOfCounterValuesPtr = numOfCounterValues;
    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(numOfCounterValuesPtr);
    TGF_PARAM_NOT_USED(format);
    TGF_PARAM_NOT_USED(counterValuesPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncCountingEnableSet
*
* DESCRIPTION:
*   The function enables counting on selected cnc unit.
*
* INPUTS:
*       cncUnit - selected unit for enable\disable counting
*       enable  - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCountingEnableSet
(
    IN  PRV_TGF_CNC_COUNTING_ENABLE_UNIT_ENT    cncUnit,
    IN  GT_BOOL                                 enable
)
{
#ifdef CHX_FAMILY
    GT_U8                                  devNum  = 0;
    GT_STATUS                              rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT dxchCncUnit;

    cnvCNC_COUNTING_ENABLE_UNIT_ENT_tgf2dxch(cncUnit, &dxchCncUnit);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncCountingEnableSet(
            devNum, dxchCncUnit, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCountingEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(cncUnit);
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncCountingEnableGet
*
* DESCRIPTION:
*   The function gets enable counting status on selected cnc unit.
*
* INPUTS:
*       cncUnit - selected unit for enable\disable counting
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable
*                     GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCountingEnableGet
(
    IN  PRV_TGF_CNC_COUNTING_ENABLE_UNIT_ENT    cncUnit,
    OUT GT_BOOL                                 *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_U8                                  devNum  = 0;
    GT_STATUS                              rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT dxchCncUnit;

    cnvCNC_COUNTING_ENABLE_UNIT_ENT_tgf2dxch(cncUnit, &dxchCncUnit);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncCountingEnableGet(
        devNum, dxchCncUnit, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCountingEnableGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(cncUnit);
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* INPUTS:
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterFormatSet
(
    IN  GT_U32                            blockNum,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format
)
{
#ifdef CHX_FAMILY

    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;

    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCounterFormat);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncCounterFormatSet(
            devNum, blockNum, dxchCounterFormat);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterFormatSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(format);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* INPUTS:
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterFormatGet
(
    IN  GT_U32                            blockNum,
    OUT PRV_TGF_CNC_COUNTER_FORMAT_ENT    *formatPtr
)
{
#ifdef CHX_FAMILY
    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncCounterFormatGet(
        devNum, blockNum, &dxchCounterFormat);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncCounterFormatGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    cnvCNC_COUNTER_FORMAT_ENT_dxch2tgf(dxchCounterFormat, formatPtr);
    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(formatPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncEgressQueueClientModeSet
*
* DESCRIPTION:
*   The function sets Egress Queue client counting mode
*
* INPUTS:
*       mode            - Egress Queue client counting mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncEgressQueueClientModeSet
(
    IN  PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT   mode
)
{
#ifdef CHX_FAMILY

    GT_U8                                      devNum  = 0;
    GT_STATUS                                  rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT dxchCncEgrQueClientMode;

    cnvCNC_EGRESS_QUEUE_CLIENT_MODE_ENT_tgf2dxch(
        mode, &dxchCncEgrQueClientMode);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncEgressQueueClientModeSet(
            devNum, dxchCncEgrQueClientMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncEgressQueueClientModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(mode);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncEgressQueueClientModeGet
*
* DESCRIPTION:
*   The function gets Egress Queue client counting mode
*
* INPUTS:
*
* OUTPUTS:
*       modePtr         - (pointer to) Egress Queue client counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncEgressQueueClientModeGet
(
    OUT PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT   *modePtr
)
{
#ifdef CHX_FAMILY
    GT_U8                                      devNum  = 0;
    GT_STATUS                                  rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT dxchCncEgrQueClientMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncEgressQueueClientModeGet(
        devNum, &dxchCncEgrQueClientMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncEgressQueueClientModeGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }
    cnvCNC_EGRESS_QUEUE_CLIENT_MODE_ENT_dxch2tgf(
        dxchCncEgrQueClientMode, modePtr);

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(modePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncCpuAccessStrictPriorityEnableSet
*
* DESCRIPTION:
*   The function enables strict priority of CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* INPUTS:
*       enable          - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCpuAccessStrictPriorityEnableSet
(
    IN  GT_BOOL   enable
)
{
#ifdef CHX_FAMILY

    GT_U8                    devNum  = 0;
    GT_STATUS                rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncCpuAccessStrictPriorityEnableSet(
            devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssDxChCncCpuAccessStrictPriorityEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncCpuAccessStrictPriorityEnableGet
*
* DESCRIPTION:
*   The function gets enable status of strict priority of
*   CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* INPUTS:
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCpuAccessStrictPriorityEnableGet
(
    OUT GT_BOOL   *enablePtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncCpuAccessStrictPriorityEnableGet(
        devNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: cpssDxChCncCpuAccessStrictPriorityEnableGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enablePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortGroupBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* INPUTS:
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockClientEnableSet
(
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    IN  GT_BOOL                   updateEnable
)
{
#ifdef CHX_FAMILY

    GT_U8                    devNum  = 0;
    GT_STATUS                rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncPortGroupBlockClientEnableSet(
            devNum, portGroupsBmp, blockNum,
            dxchClient, updateEnable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupBlockClientEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(updateEnable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncPortGroupBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockClientEnableGet
(
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncPortGroupBlockClientEnableGet(
        devNum, portGroupsBmp, blockNum, dxchClient, updateEnablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupBlockClientEnableGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(updateEnablePtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortGroupBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*       indexRangesBmp   - bitmap of counter index ranges
*                         DxCh3 and DxChXcat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         The Lion devices has 64 ranges (512 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockClientRangesSet
(
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT      client,
    IN  GT_U64                      indexRangesBmp
)
{
#ifdef CHX_FAMILY

    GT_U8                    devNum  = 0;
    GT_STATUS                rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncPortGroupBlockClientRangesSet(
            devNum, portGroupsBmp, blockNum,
            dxchClient, indexRangesBmp);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncGroupBlockClientRangesSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(indexRangesBmp);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncPortGroupBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* INPUTS:
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap counter index ranges
*                         DxCh3 and DxChXcat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         The Lion devices has 64 ranges (512 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockClientRangesGet
(
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_U64                    *indexRangesBmpPtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_CLIENT_ENT dxchClient;

    cnvCNC_CLIENT_ENT_tgf2dxch(client, &dxchClient);


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncPortGroupBlockClientRangesGet(
        devNum, portGroupsBmp, blockNum, dxchClient, indexRangesBmpPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupBlockClientRangesGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(client);
    TGF_PARAM_NOT_USED(indexRangesBmpPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortGroupCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       indexNumPtr     - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr     - (pointer to) actual size of array of indexes
*       portGroupIdArr[]- (pointer to) array of port group Ids of
*                         counters wrapped around.
*                         The NULL pointer supported.
*       indexesArr[]    - (pointer to) array of indexes of counters
*                         wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterWraparoundIndexesGet
(
    IN    GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN    GT_U32                  blockNum,
    INOUT GT_U32                  *indexNumPtr,
    OUT   GT_U32                  portGroupIdArr[],
    OUT   GT_U32                  indexesArr[]
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncPortGroupCounterWraparoundIndexesGet(
        devNum, portGroupsBmp,
        blockNum, indexNumPtr, portGroupIdArr, indexesArr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupCounterWraparoundIndexesGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(indexNumPtr);
    TGF_PARAM_NOT_USED(portGroupIdArr);
    TGF_PARAM_NOT_USED(indexesArr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortGroupCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterSet
(
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    IN  PRV_TGF_CNC_COUNTER_STC           *counterPtr
)
{
#ifdef CHX_FAMILY

    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCounter;

    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCounterFormat);
    cnvCNC_CNC_COUNTER_STC_tgf2dxch(counterPtr, &dxchCounter);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncPortGroupCounterSet(
            devNum, portGroupsBmp,
            blockNum, index, dxchCounterFormat, &dxchCounter);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupCounterSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }

        /* set the value to first device only, 0 to all others */
        cpssOsMemSet(&dxchCounter, 0, sizeof(dxchCounter));
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(format);
    TGF_PARAM_NOT_USED(counterPtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncPortGroupCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* INPUTS:
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterGet
(
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT PRV_TGF_CNC_COUNTER_STC           *counterPtr
)
{
#ifdef CHX_FAMILY

    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCounter;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCounterSum;

    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCounterFormat);

    cpssOsMemSet(&dxchCounterSum, 0, sizeof(dxchCounterSum));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncPortGroupCounterGet(
            devNum, portGroupsBmp,
            blockNum, index, dxchCounterFormat, &dxchCounter);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupCounterGet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }

        /* sum over all devices */
        dxchCounterSum.packetCount =
            prvCpssMathAdd64(
                dxchCounterSum.packetCount, dxchCounter.packetCount);
        dxchCounterSum.byteCount =
            prvCpssMathAdd64(
                dxchCounterSum.byteCount, dxchCounter.byteCount);
    }

    cnvCNC_CNC_COUNTER_STC_dxch2tgf(&dxchCounterSum, counterPtr);
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(format);
    TGF_PARAM_NOT_USED(counterPtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncPortGroupBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   prvTgfCncPortGroupBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by prvTgfCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChCncPortGroupBlockUploadTrigger(
        devNum, portGroupsBmp, blockNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupBlockUploadTrigger FAILED, rc = [%d]", rc);

    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortGroupBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          Checks all active port groups of the bitmap.
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    OUT GT_U32                      *inProcessBlocksBmpPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChCncPortGroupBlockUploadInProcessGet(
        devNum, portGroupsBmp, inProcessBlocksBmpPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupBlockUploadInProcessGet FAILED, rc = [%d]", rc);

    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(inProcessBlocksBmpPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortGroupUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by prvTgfCncPortGroupBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the prvTgfCncPortGroupUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       prvTgfCncPortGroupUploadedBlockGet one more time to get rest of the block.
*
* INPUTS:
*       devNum                - device number
*       portGroupsBmp         - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by prvTgfCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_PORT_GROUPS_BMP                portGroupsBmp,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT    PRV_TGF_CNC_COUNTER_STC           *counterValuesPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                           rc = GT_OK;
    GT_U32                              numOfCounterValues;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCncCounterFormat;
    CPSS_DXCH_CNC_COUNTER_STC           dxchCncCounter;
    GT_U32                              counterAmount;


    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCncCounterFormat);

    for (numOfCounterValues = 0, counterAmount = 1;
          (numOfCounterValues < (*numOfCounterValuesPtr));
          counterValuesPtr ++, numOfCounterValues ++)
    {
        /* call device specific API */
        rc = cpssDxChCncPortGroupUploadedBlockGet(
            devNum, portGroupsBmp,
            &counterAmount, dxchCncCounterFormat, &dxchCncCounter);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupBlockUploadInProcessGet FAILED, rc = [%d]", rc);
            break;
        }

        if (counterAmount == 0)
        {
            break;
        }

        cnvCNC_CNC_COUNTER_STC_dxch2tgf(&dxchCncCounter, counterValuesPtr);
    }

    *numOfCounterValuesPtr = numOfCounterValues;
    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(numOfCounterValuesPtr);
    TGF_PARAM_NOT_USED(format);
    TGF_PARAM_NOT_USED(counterValuesPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncPortGroupCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterFormatSet
(
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format
)
{
#ifdef CHX_FAMILY

    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;

    cnvCNC_COUNTER_FORMAT_ENT_tgf2dxch(format, &dxchCounterFormat);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChCncPortGroupCounterFormatSet(
            devNum, portGroupsBmp, blockNum, dxchCounterFormat);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupCounterFormatSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
    return rc1;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(format);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCncPortGroupCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterFormatGet
(
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    OUT PRV_TGF_CNC_COUNTER_FORMAT_ENT    *formatPtr
)
{
#ifdef CHX_FAMILY
    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT    dxchCounterFormat;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCncPortGroupCounterFormatGet(
        devNum, portGroupsBmp, blockNum, &dxchCounterFormat);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCncPortGroupCounterFormatGet FAILED, rc = [%d]", rc);

        rc1 = rc;
    }

    cnvCNC_COUNTER_FORMAT_ENT_dxch2tgf(dxchCounterFormat, formatPtr);
    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(blockNum);
    TGF_PARAM_NOT_USED(formatPtr);

    return GT_NOT_APPLICABLE_DEVICE;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/******************************************************************************\
 *                       API for default settings                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfCncFineTuningBlockAmountGet
*
* DESCRIPTION:
*   The function gets Amount of Cnc Blocks per device.
*   if Cnc not supported - 0 returned.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       amount of CnC Blocks per Device or per Port Group
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCncFineTuningBlockAmountGet
(
    GT_VOID
)
{
#ifdef CHX_FAMILY
    GT_U8                               devNum  = 0;
    GT_STATUS                           rc;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return PRV_CPSS_DXCH_PP_MAC(devNum)->
            fineTuning.tableSize.cncBlocks;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    return 0;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return 0;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCncFineTuningBlockSizeGet
*
* DESCRIPTION:
*   The function gets Amount of Cnc Blocks per device.
*   if Cnc not supported - 0 returned.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       amount of CnC Blocks per Device or per Port Group
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCncFineTuningBlockSizeGet
(
    GT_VOID
)
{
#ifdef CHX_FAMILY
    GT_U8                               devNum  = 0;
    GT_STATUS                           rc;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return PRV_CPSS_DXCH_PP_MAC(devNum)->
            fineTuning.tableSize.cncBlockNumEntries;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    return 0;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return 0;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

