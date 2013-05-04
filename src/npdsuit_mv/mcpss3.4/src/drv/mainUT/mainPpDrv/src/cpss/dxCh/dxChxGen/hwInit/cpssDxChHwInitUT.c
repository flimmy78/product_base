/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChHwInitUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChHwInitUT, that provides
*       CPSS DXCH level basic Hw initialization functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChModuleConfig.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
/* get DxCh Trunk private types */
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah3.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsDxChXcat.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/**/
#include <cpss/extServices/cpssExtServices.h>
#include <gtOs/gtOsTimer.h>

/* Valid port num value used for testing */
#define HWINIT_VALID_PORTNUM_CNS         0

/* Valid period low bound range*/
#define HWINIT_PERIOD_LOW_BOUND_CNS         320

/* Valid period high bound range*/
#define HWINIT_PERIOD_HIGH_BOUND_CNS       5242560

/* maximal number of units in device */
#define MAX_UNIT_NUM_CNS                    35

/*
 * Typedef: struct HWINIT_ERRATA_BAD_ADDR_RANGE_STC
 *
 * Description: Describes restricted address range
 *
 * Fields:
 *      addrStart  - Range start offset
 *      addrEnd    - Range end offset
 */
typedef struct
{
    GT_U32      addrStart;
    GT_U32      addrEnd;
} HWINIT_ERRATA_BAD_ADDR_RANGE_STC;

/* check if device supports Link Loss Skip */
#define IS_LINK_LOSS_SUPPORT_DEV_MAC(_dev) \
((PRV_CPSS_PP_MAC(_dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) || \
 ((CPSS_PP_FAMILY_CHEETAH3_E == PRV_CPSS_PP_MAC(_dev)->devFamily) && (1 == PRV_CPSS_DXCH_PP_MAC(_dev)->hwInfo.mg.metalFix)) \
)

/* check if device supports PEX Init Skip */
#define IS_PEX_SKIP_SUPPORT_DEV_MAC(_dev) \
((PRV_CPSS_PP_MAC(_dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)  && \
 (PRV_CPSS_PP_MAC(_dev)->devFamily != CPSS_PP_FAMILY_CHEETAH2_E) && \
 (PRV_CPSS_PP_MAC(_dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwAuDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwAuDescSizeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with descSizePtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32     descSize;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChHwAuDescSizeGet(dev, &descSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with descSizePtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwAuDescSizeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwAuDescSizeGet(dev, &descSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwAuDescSizeGet(dev, &descSize);
    /* the devType is not used yet */
    /*UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);*/
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwCoreClockGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *coreClkDbPtr,
    OUT GT_U32  *coreClkHwPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwCoreClockGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with coreClkDbPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with coreClkHwPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32     coreClkDb;
    GT_U32     coreClkHw;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChHwCoreClockGet(dev, &coreClkDb, &coreClkHw);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with coreClkDbPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwCoreClockGet(dev, NULL, &coreClkHw);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with coreClkHbPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwCoreClockGet(dev, &coreClkDb, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwCoreClockGet(dev, &coreClkDb, &coreClkHw);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwCoreClockGet(dev, &coreClkDb, &coreClkHw);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwInterruptCoalescingSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U32   period,
    IN GT_BOOL  linkChangeOverride
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwInterruptCoalescingSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with enable [GT_TRUE / GT_FALSE],
                    period [HWINIT_PERIOD_LOW_BOUND_CNS /
                            (HWINIT_PERIOD_HIGH_BOUND_CNS +
                             HWINIT_PERIOD_LOW_BOUND_CNS) / 2   /
                             HWINIT_PERIOD_HIGH_BOUND_CNS],
                    linkChangeOverride [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChHwInterruptCoalescingGet with the same params.
    Expected: GT_OK and the same values.
    1.3. Call with enable [GT_FALSE],
                   period [HWINIT_PERIOD_HIGH_BOUND_CNS + 1]
                   (ignored when enable [GT_FALSE]),
                   linkChangeOverride [GT_FALSE].
    Expected: GT_OK.
    1.4. Call with enable [GT_TRUE],
                   period [HWINIT_PERIOD_HIGH_BOUND_CNS + 1]
                   linkChangeOverride [GT_FALSE].
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL  enable = GT_TRUE;
    GT_U32   period = 0;
    GT_BOOL  linkChangeOverride = GT_TRUE;

    GT_BOOL  enableGet = GT_FALSE;
    GT_U32   periodGet = 1;
    GT_BOOL  linkChangeOverrideGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE / GT_FALSE],
                            period [HWINIT_PERIOD_LOW_BOUND_CNS /
                                    (HWINIT_PERIOD_HIGH_BOUND_CNS +
                                     HWINIT_PERIOD_LOW_BOUND_CNS) / 2   /
                                     HWINIT_PERIOD_HIGH_BOUND_CNS],
                            linkChangeOverride [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        /* call with
                    enable = GT_TRUE;
                    period = HWINIT_PERIOD_LOW_BOUND_CNS;
                    linkChangeOverride = GT_TRUE;
        */

        enable = GT_TRUE;
        period = HWINIT_PERIOD_LOW_BOUND_CNS;
        linkChangeOverride = GT_TRUE;

        st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChHwInterruptCoalescingGet with the same params.
            Expected: GT_OK and the same values.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, &enableGet, &periodGet,
                                                &linkChangeOverrideGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if(GT_OK ==st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "get another enable than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(period, periodGet,
                 "get another period than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(linkChangeOverride, linkChangeOverrideGet,
                 "get another linkChangeOverride than was set: dev = %d", dev);
        }

        /* call with
                    enable = GT_TRUE;
                    period = HWINIT_PERIOD_LOW_BOUND_CNS;
                    linkChangeOverride = GT_FALSE;
        */

        enable = GT_TRUE;
        period = HWINIT_PERIOD_LOW_BOUND_CNS;
        linkChangeOverride = GT_FALSE;

        st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChHwInterruptCoalescingGet with the same params.
            Expected: GT_OK and the same values.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, &enableGet, &periodGet,
                                                &linkChangeOverrideGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if(GT_OK ==st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "get another enable than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(period, periodGet,
                 "get another period than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(linkChangeOverride, linkChangeOverrideGet,
                 "get another linkChangeOverride than was set: dev = %d", dev);
        }

        /* call with
                    enable = GT_TRUE;
                    period = (HWINIT_PERIOD_HIGH_BOUND_CNS + HWINIT_PERIOD_LOW_BOUND_CNS) / 2;
                    linkChangeOverride = GT_TRUE;
        */

        enable = GT_TRUE;
        period = (HWINIT_PERIOD_HIGH_BOUND_CNS + HWINIT_PERIOD_LOW_BOUND_CNS) / 2;
        linkChangeOverride = GT_TRUE;

        st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChHwInterruptCoalescingGet with the same params.
            Expected: GT_OK and the same values.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, &enableGet, &periodGet,
                                                &linkChangeOverrideGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if(GT_OK ==st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "get another enable than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(period, periodGet,
                 "get another period than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(linkChangeOverride, linkChangeOverrideGet,
                 "get another linkChangeOverride than was set: dev = %d", dev);
        }

        /* call with
                    enable = GT_TRUE;
                    period = HWINIT_PERIOD_HIGH_BOUND_CNS;
                    linkChangeOverride = GT_TRUE;
        */

        enable = GT_TRUE;
        period = HWINIT_PERIOD_HIGH_BOUND_CNS;
        linkChangeOverride = GT_TRUE;

        st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChHwInterruptCoalescingGet with the same params.
            Expected: GT_OK and the same values.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, &enableGet, &periodGet,
                                                &linkChangeOverrideGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if(GT_OK ==st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "get another enable than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(period, periodGet,
                 "get another period than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(linkChangeOverride, linkChangeOverrideGet,
                 "get another linkChangeOverride than was set: dev = %d", dev);
        }

        /* call with
                    enable = GT_FALSE;
                    period = 0;
                    linkChangeOverride = GT_TRUE;
        */

        enable = GT_FALSE;
        period = 0;
        linkChangeOverride = GT_TRUE;

        st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChHwInterruptCoalescingGet with the same params.
            Expected: GT_OK and the same values.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, &enableGet, &periodGet,
                                                &linkChangeOverrideGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if(GT_OK ==st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "get another enable than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(period, periodGet,
                 "get another period than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(linkChangeOverride, linkChangeOverrideGet,
                 "get another linkChangeOverride than was set: dev = %d", dev);
        }
        /*
            1.3. Call with enable [GT_FALSE],
                           period [HWINIT_PERIOD_HIGH_BOUND_CNS + 1]
                           (ignored when enable [GT_FALSE]),
                           linkChangeOverride [GT_FALSE].
            Expected: GT_OK.
        */

        enable = GT_FALSE;
        period = HWINIT_PERIOD_HIGH_BOUND_CNS+1;
        linkChangeOverride = GT_FALSE;

        st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.4. Call with enable [GT_TRUE],
                           period [HWINIT_PERIOD_HIGH_BOUND_CNS + 1]
                           linkChangeOverride [GT_FALSE].
            Expected: GT_OUT_OF_RANGE.
        */

        enable = GT_TRUE;
        period = HWINIT_PERIOD_HIGH_BOUND_CNS+1;
        linkChangeOverride = GT_FALSE;

        st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwInterruptCoalescingSet(dev, enable, period, linkChangeOverride);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwInterruptCoalescingGet
(
    IN GT_U8        devNum,
    OUT GT_BOOL     *enablePtr,
    OUT GT_U32      *periodPtr,
    OUT GT_BOOL     *linkChangeOverridePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwInterruptCoalescingGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null pointers.
    Expected: GT_OK.
    1.2. Call with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with wrong periodPtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call with wrong linkChangeOverridePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL  enable = GT_TRUE;
    GT_U32   period = 0;
    GT_BOOL  linkChangeOverride = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null pointers.
            Expected: GT_OK.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, &enable, &period,
                                                &linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, NULL, &period,
                                                &linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with wrong periodPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, &enable, NULL,
                                                &linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.4. Call with wrong linkChangeOverridePtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChHwInterruptCoalescingGet(dev, &enable, &period, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwInterruptCoalescingGet(dev, &enable, &period,
                                                &linkChangeOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwInterruptCoalescingGet(dev, &enable, &period,
                                            &linkChangeOverride);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwPpPhase1Init
(
    IN      CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   *ppPhase1ParamsPtr,
    OUT     CPSS_PP_DEVICE_TYPE                 *deviceTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwPpPhase1Init)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with wrong ppPhase1ParamsPtr [NULL].
    Expected: GT_BAD_PTR.
    1.2. Call with wrong deviceTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   ppPhase1Params;
    CPSS_PP_DEVICE_TYPE                 deviceType;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with wrong ppPhase1ParamsPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChHwPpPhase1Init(NULL, &deviceType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.2. Call with wrong deviceTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChHwPpPhase1Init(&ppPhase1Params, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwPpPhase1Init(&ppPhase1Params, &deviceType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwPpPhase2Init
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PP_PHASE2_INIT_INFO_STC       *ppPhase2ParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwPpPhase2Init)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with wrong ppPhase2ParamsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PP_PHASE2_INIT_INFO_STC   ppPhase2Params;

    /* initialize mac entry */
    cpssOsMemSet(&ppPhase2Params,0,sizeof(ppPhase2Params));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with wrong ppPhase2ParamsPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChHwPpPhase2Init(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwPpPhase2Init(dev, &ppPhase2Params);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwPpPhase2Init(dev, &ppPhase2Params);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwPpSoftResetTrigger
(
    IN  GT_U8        devNum,
)
*/
#if 0
/* don't execute this test on HW. The API will reset PEX registers
   and device will be not manageable!!!!! */
UTF_TEST_CASE_MAC(cpssDxChHwPpSoftResetTrigger)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null pointers.
            Expected: GT_OK.
        */

        st = cpssDxChHwPpSoftResetTrigger(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwPpSoftResetTrigger(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwPpSoftResetTrigger(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#endif
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwPpStartInit
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     loadFromEeprom,
    IN  CPSS_REG_VALUE_INFO_STC     *initDataListPtr,
    IN  GT_U32                      initDataListLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwPpStartInit)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL                     loadFromEeprom = GT_TRUE;
    CPSS_REG_VALUE_INFO_STC     initDataList;
    GT_U32                      initDataListLen = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null pointers.
            Expected: GT_OK.
        */
        loadFromEeprom = GT_TRUE;

        st = cpssDxChHwPpStartInit(dev, loadFromEeprom, &initDataList, initDataListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        loadFromEeprom = GT_FALSE;

        st = cpssDxChHwPpStartInit(dev, loadFromEeprom, &initDataList, initDataListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1. Call with wrong initDataListPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChHwPpStartInit(dev, loadFromEeprom, NULL, initDataListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwPpStartInit(dev, loadFromEeprom, &initDataList, initDataListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwPpStartInit(dev, loadFromEeprom, &initDataList, initDataListLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwRxBufAlignmentGet
(
    IN  CPSS_PP_DEVICE_TYPE devType,
    OUT GT_U32              *byteAlignmentPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwRxBufAlignmentGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with byteAlignmentPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32     byteAlignment;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChHwRxBufAlignmentGet(dev, &byteAlignment);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with byteAlignmentPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwRxBufAlignmentGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwRxBufAlignmentGet(dev, &byteAlignment);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwRxBufAlignmentGet(dev, &byteAlignment);
    /* the devType is not used yet */
    /*UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);*/
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwRxDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwRxDescSizeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with descSizePtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32     descSize;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChHwRxDescSizeGet(dev, &descSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with descSizePtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwRxDescSizeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwRxDescSizeGet(dev, &descSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwRxDescSizeGet(dev, &descSize);
    /* the devType is not used yet */
    /*UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);*/
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwTxDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwTxDescSizeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with descSizePtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32     descSize;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChHwTxDescSizeGet(dev, &descSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with descSizePtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwTxDescSizeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwTxDescSizeGet(dev, &descSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwTxDescSizeGet(dev, &descSize);
    /* the devType is not used yet */
    /*UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);*/
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgHwDevNumSet
(
    IN GT_U8    devNum,
    IN GT_U8    hwDevNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgHwDevNumSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct hwDevNum [0 / 15 / 31].
    Expected: GT_OK.
    1.2. Call with wrong hwDevNum [BIT_5].
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       hwDevNum = 0;
    GT_U8       hwDevNumGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct hwDevNum [0 / 15 / 31].
            Expected: GT_OK.
        */

        hwDevNum = 0;
        st = cpssDxChCfgHwDevNumSet(dev, hwDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        /*
            Call cpssDxChCfgHwDevNumGet with the same params.
            Expected: GT_OK and the same values.
        */

        st = cpssDxChCfgHwDevNumGet(dev, &hwDevNumGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if(GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(hwDevNum, hwDevNumGet,
                 "get another hwDevNum than was set: dev = %d", dev);
        }


        hwDevNum = 15;
        st = cpssDxChCfgHwDevNumSet(dev, hwDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        /*
            Call cpssDxChCfgHwDevNumGet with the same params.
            Expected: GT_OK and the same values.
        */

        st = cpssDxChCfgHwDevNumGet(dev, &hwDevNumGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if(GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(hwDevNum, hwDevNumGet,
                 "get another hwDevNum than was set: dev = %d", dev);
        }

        hwDevNum = 31;
        st = cpssDxChCfgHwDevNumSet(dev, hwDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        /*
            Call cpssDxChCfgHwDevNumGet with the same params.
            Expected: GT_OK and the same values.
        */

        st = cpssDxChCfgHwDevNumGet(dev, &hwDevNumGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if(GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(hwDevNum, hwDevNumGet,
                 "get another hwDevNum than was set: dev = %d", dev);
        }

        /*
            1.2. Call with wrong hwDevNum [BIT_5].
            Expected: GT_OUT_OF_RANGE.
        */

        hwDevNum = BIT_5;

        st = cpssDxChCfgHwDevNumSet(dev, hwDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgHwDevNumSet(dev, hwDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgHwDevNumSet(dev, hwDevNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwPpImplementWaInit
(
    IN GT_U8    devNum,
    IN GT_U32                       numOfWa,
    IN CPSS_DXCH_IMPLEMENT_WA_ENT   waArr[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwPpImplementWaInit)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct params.
    Expected: GT_OK.
    1.2. Call with wrong waArrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32                       numOfWa = 0;
    CPSS_DXCH_IMPLEMENT_WA_ENT   waArr[4] = {0};

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct params.
            Expected: GT_OK or GT_BAD_STATE
        */

        numOfWa = 0;

        if (CPSS_PP_FAMILY_DXCH_XCAT_E == PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            /* the cpss implementation was done only for XCAT devices */
            waArr[numOfWa++] = CPSS_DXCH_IMPLEMENT_WA_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E;
        }
        waArr[numOfWa++] = CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FIFO_E;
        waArr[numOfWa++] = CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_PADDING_E;
        waArr[numOfWa++] = CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E;

        /* for some WA this API need to be called before relate library already initialized, */
        /* thus GT_BAD_STATE is ok. If this WA already initialized the function will return GT_OK */
        /* No need to check device type, all WA can be initialized for all devices */
        st = cpssDxChHwPpImplementWaInit(dev, numOfWa, waArr, 0);
        if (st != GT_OK && st != GT_BAD_STATE)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,"expected rc is GT_OK or GT_BAD_STATE in %d", dev);
        }


        /*
            1.2. Call with wrong waArrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwPpImplementWaInit(dev, numOfWa, NULL, 0);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwPpImplementWaInit(dev, numOfWa, waArr, 0);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwPpImplementWaInit(dev, numOfWa, waArr, 0);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgHwDevNumGet
(
    IN GT_U8    devNum,
    OUT GT_U8   *hwDevNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgHwDevNumGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with hwDevNumPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       hwDevNum;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChCfgHwDevNumGet(dev, &hwDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with hwDevNumPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCfgHwDevNumGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgHwDevNumGet(dev, &hwDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgHwDevNumGet(dev, &hwDevNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*        Check the following restricted ranges :
*
*           XCAT A1 ranges:
*
*           1.  0x02900000 - 0x0290FFFF   (Unit 5)
*               0x02C00000 - 0x02CFFFFF
*           2.  0x07E40000 - 0x07FC0000     (Unit 15)
*           3.  0x08800000 - 0x08801700     (Unit 17)
*               0x08801C00 - 0x08803F00
*           4.  0x09000000 - 0x092FFFF0     (Unit 18)
*               0x09380000 - 0x097FFFFF
*           5.  0x098007FF - 0x09800C00     (Unit 19)
*               0x098017FF - 0x09801C00
*               0x098027FF - 0x09802C00
*               0x098037FF - 0x09803C00
*               0x098047FF - 0x09804C00
*               0x098057FF - 0x09805C00
*               0x098067FF - 0x09806C00
*               0x098077FF - 0x09807C00
*               0x098087FF - 0x09808C00
*               0x098097FF - 0x09809C00
*               0x0980F7FF - 0x0980FC00
*           6.  0x0A807000 - 0x0A80F800 (Unit 21)
*
*
*           Lion B0 ranges (only for port groups 1,2,3):
*
*           1.  0x03800000 - 0x0387FFFC
*           2.  0x07000000 - 0x0707FFFC
*           3.  0x0A000000 - 0x0A7FFFFC
*           4.  0x10800000 - 0x10FFFFFC
*           only for port group 2 and 3
*           5.  0x11800000 - 0x11FFFFFC
*/
UTF_TEST_CASE_MAC(cpssDxChCfgHwCheckBadAddrWa)
{
/*
    1.1. Call with restricted addresses.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      singleRanges;
    GT_U32      i;
    GT_U32      numOfWords;
    GT_U32      data;
    GT_U32      maxNumOfWordsToWrite = 0x10000;
    GT_U32      *dataPtr;
    CPSS_DXCH_IMPLEMENT_WA_ENT          waFromCpss[1];
    HWINIT_ERRATA_BAD_ADDR_RANGE_STC    badAddrsArray[25];
    GT_U32      portGroupId;
    GT_U32      portGroupsBmp;
    GT_U32      allRanges;
    GT_STATUS   expectedStatus;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);


    dataPtr = (GT_U32 *)cpssOsMalloc(maxNumOfWordsToWrite);
    cpssOsMemSet(dataPtr,0,maxNumOfWordsToWrite);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            /* All units single ranges */

            /* Unit 20 */
            badAddrsArray[0].addrStart = 0x0A000000;
            badAddrsArray[0].addrEnd = 0x0A7FFFFC;

            /* Unit 33 */
            badAddrsArray[1].addrStart = 0x10800000;
            badAddrsArray[1].addrEnd = 0x10FFFFFC;


            /* Unit 35 */
            badAddrsArray[2].addrStart = 0x11800000;
#ifdef ASIC_SIMULATION
            /* An existing address */
            badAddrsArray[2].addrEnd = 0x118F0000;
#else
            badAddrsArray[2].addrEnd = 0x118FFFFC;
#endif /*ASIC_SIMULATION*/

            /* Unit 7 */
            badAddrsArray[3].addrStart = 0x03800000;
            badAddrsArray[3].addrEnd = 0x0387FFFC;

            /* Unit 14 */
            badAddrsArray[4].addrStart = 0x07000000;
            badAddrsArray[4].addrEnd = 0x0707FFFC;

            /* Two or more units ranges */

            /* includes units 14 and 20 */
            badAddrsArray[5].addrStart = 0x0707FFF0;
            badAddrsArray[5].addrEnd = 0x0A00000C;

            /* includes units 7 and 14 */
            badAddrsArray[6].addrStart = 0x0387FFF0;
            badAddrsArray[6].addrEnd = 0x0700000C;

            /* includes ranges 13 and 14
               (where unit 13 is ok)  */
            badAddrsArray[7].addrStart = 0x06FFFF0C;
            badAddrsArray[7].addrEnd = 0x070000FC;

            singleRanges = 5;
            allRanges = 8;
            portGroupsBmp = 0xE;
        }
        else if (CPSS_PP_FAMILY_DXCH_XCAT_E == PRV_CPSS_PP_MAC(dev)->devFamily) 
        {
            /* XCAT */

            /* All units single ranges */

            badAddrsArray[0].addrStart = 0x02900000;
            badAddrsArray[0].addrEnd = 0x0290FFFC;

            badAddrsArray[1].addrStart = 0x02C00000;
            badAddrsArray[1].addrEnd = 0x02CFFFFC;

            badAddrsArray[2].addrStart = 0x07E40000;
            badAddrsArray[2].addrEnd = 0x07FC0000;

            badAddrsArray[3].addrStart = 0x08800000;
            badAddrsArray[3].addrEnd = 0x08801700;

            badAddrsArray[4].addrStart = 0x08801C00;
            badAddrsArray[4].addrEnd = 0x08803F00;

            badAddrsArray[5].addrStart = 0x09000000;
            badAddrsArray[5].addrEnd = 0x092FFFF0;

            badAddrsArray[6].addrStart = 0x09380000;
            badAddrsArray[6].addrEnd = 0x097FFFFC;

            badAddrsArray[7].addrStart = 0x09800800;
            badAddrsArray[7].addrEnd = 0x09800C00;

            badAddrsArray[8].addrStart = 0x09801800;
            badAddrsArray[8].addrEnd = 0x09801C00;

            badAddrsArray[9].addrStart = 0x09802800;
            badAddrsArray[9].addrEnd = 0x09802C00;

            badAddrsArray[10].addrStart = 0x09803800;
            badAddrsArray[10].addrEnd = 0x09803C00;

            badAddrsArray[11].addrStart = 0x09804800;
            badAddrsArray[11].addrEnd = 0x09804C00;

            badAddrsArray[12].addrStart = 0x09805800;
            badAddrsArray[12].addrEnd = 0x09805C00;

            badAddrsArray[13].addrStart = 0x09806800;
            badAddrsArray[13].addrEnd = 0x09806C00;

            badAddrsArray[14].addrStart = 0x09807800;
            badAddrsArray[14].addrEnd = 0x09807C00;

            badAddrsArray[15].addrStart = 0x09808800;
            badAddrsArray[15].addrEnd = 0x09808C00;

            badAddrsArray[16].addrStart = 0x09809800;
            badAddrsArray[16].addrEnd = 0x09809C00;

            badAddrsArray[17].addrStart = 0x0980F800;
            badAddrsArray[17].addrEnd = 0x0980FC00;

            badAddrsArray[18].addrStart = 0x0A807000;
            badAddrsArray[18].addrEnd = 0x0A80F800;

            /* Single unit two or more ranges */

            /* includes two ranges: 0x02900000-0x0290FFFF and 0x02C00000-0x02CFFFFF */
            badAddrsArray[19].addrStart = 0x028FFFF0;
            badAddrsArray[19].addrEnd = 0x02D0000A;

            /* includes two ranges: 0x08800000-0x08801700 and 0x08801C00-0x08803F00 */
            badAddrsArray[20].addrStart = 0x08801600;
            badAddrsArray[20].addrEnd = 0x08801CF0;

            /* includes three ranges: 0x09800800-0x09800C00, 0x09801800-0x09801C00 and
               0x09802800-0x09802C00 */
            badAddrsArray[21].addrStart = 0x098007F0;
            badAddrsArray[21].addrEnd = 0x09802C04;

            /* Two or more units ranges */

            /* includes ranges 17 and 18 */
            badAddrsArray[22].addrStart = 0x08803C00;
            badAddrsArray[22].addrEnd = 0x09800004;

            /* includes ranges 5 and 6 */
            badAddrsArray[23].addrStart = 0x092FFF00;
            badAddrsArray[23].addrEnd = 0x0938000C;

            /* includes ranges 16, 17 and 18 */
            badAddrsArray[24].addrStart = 0x087FF000;
            badAddrsArray[24].addrEnd = 0x09000010;

            singleRanges = 19;
            allRanges = 25;
            portGroupsBmp = 0x1;
        }
        else
        {
            /* XCAT2 */

            /* All units single ranges */

            badAddrsArray[0].addrStart = 0x01B400FC;
            badAddrsArray[0].addrEnd = 0x01B40FFC;

            badAddrsArray[1].addrStart = 0x03F01000;
            badAddrsArray[1].addrEnd = 0x03F017FC;

            badAddrsArray[2].addrStart = 0x06200100;
            badAddrsArray[2].addrEnd = 0x06200F00;

            badAddrsArray[3].addrStart = 0x07E4EFF0;
            badAddrsArray[3].addrEnd = 0x07E4FFFC;

            badAddrsArray[4].addrStart = 0x0980EFFC;
            badAddrsArray[4].addrEnd = 0x0980FFF0;

            badAddrsArray[5].addrStart = 0x0D0014F0;
            badAddrsArray[5].addrEnd = 0x0D0015F0;

            badAddrsArray[6].addrStart = 0x0A808000;
            badAddrsArray[6].addrEnd = 0x0A809000;

            badAddrsArray[7].addrStart = 0x0C721000;
            badAddrsArray[7].addrEnd = 0x0C721060;

            /* Single unit two or more ranges */

            /* includes two ranges: 0x0C000600-0x0C0008FC and 0x0C001800-0x0C001FFC */
            badAddrsArray[8].addrStart = 0x0C0008F0;
            badAddrsArray[8].addrEnd = 0x0C00180C;

            /* includes two ranges: 0x0E000400-0x0E0010FC and 0x0E002000-0x0E0027FC */
            badAddrsArray[9].addrStart = 0x0E0010F0;
            badAddrsArray[9].addrEnd = 0x0E00200C;

            singleRanges = 8;
            allRanges = 10;
            portGroupsBmp = 0x1;
        }

        /* enable Check Address WA */
        waFromCpss[0] = CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E;
        st = cpssDxChHwPpImplementWaInit(dev,1,&waFromCpss[0],0);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        if(PRV_CPSS_DXCH_PP_MAC(dev)->errata.
            info_PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E.unitRangesPtr[0] == NULL)
        {
            /* The WA is not initialized */
            return;
        }

        /* 1.1. Go over all active port groups from the bitmap */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(dev, portGroupsBmp, portGroupId)
        {

            /* Read/Write one restricted address, write single ranges */
            for (i = 0; i < singleRanges; i++)
            {
                if ((portGroupId == 1) && (badAddrsArray[i].addrStart == 0x11800000))
                {
                    /* In Lion B0 in portGroup 1 unit 35 don't have
                       restricted addresses */
                    expectedStatus = GT_OK;
                }
                else
                {
                    expectedStatus = GT_BAD_PARAM;
                }

                st = prvCpssDrvHwPpPortGroupReadRegister(dev, portGroupId,
                                                         badAddrsArray[i].addrStart,
                                                         &data);
                UTF_VERIFY_EQUAL3_PARAM_MAC(expectedStatus, st, dev, portGroupId,
                                            badAddrsArray[i].addrStart);

                st = prvCpssDrvHwPpPortGroupWriteRegister(dev, portGroupId,
                                                          badAddrsArray[i].addrEnd,
                                                          0);
                UTF_VERIFY_EQUAL3_PARAM_MAC(expectedStatus, st, dev, portGroupId,
                                            badAddrsArray[i].addrEnd);


                /* calculate the number of words to write */
                numOfWords = ((badAddrsArray[i].addrEnd - badAddrsArray[i].addrStart) / 4) + 1;

                if (numOfWords > maxNumOfWordsToWrite)
                {
                    numOfWords = maxNumOfWordsToWrite;
                }

                st = prvCpssDrvHwPpPortGroupWriteRam(dev, portGroupId,
                                                     badAddrsArray[i].addrStart,
                                                     numOfWords, dataPtr);

                UTF_VERIFY_EQUAL3_PARAM_MAC(expectedStatus, st, dev, portGroupId,
                                            badAddrsArray[i].addrStart);
            }

            /* Write ranges that include two or more ranges or units */
            for (i = singleRanges; i < allRanges; i++)
            {
                /* calculate the number of words to write */
                numOfWords = ((badAddrsArray[i].addrEnd - badAddrsArray[i].addrStart) / 4) + 1;

                if (numOfWords > maxNumOfWordsToWrite)
                {
                    numOfWords = maxNumOfWordsToWrite;
                }

                st = prvCpssDrvHwPpPortGroupWriteRam(dev, portGroupId,
                                                     badAddrsArray[i].addrStart,
                                                     numOfWords, dataPtr);
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)


        /* Disable Address Checking WA */
        st = prvCpssDrvAddrCheckWaBind(dev,NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwPpInitStageGet
(
    IN  GT_U8                      dev,
    OUT CPSS_HW_PP_INIT_STAGE_ENT  *initStagePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwPpInitStageGet)
{
/*
    ITERATE_DEVICES(All DxCh Devices)
    1.1. Call with not null initStagePtr.
    Expected: GT_OK.
    1.2. Call api with wrong initStagePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                      dev;
    CPSS_HW_PP_INIT_STAGE_ENT  initStage;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null initStagePtr.
            Expected: GT_OK.
        */
        st = cpssDxChHwPpInitStageGet(dev, &initStage);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong initStagePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwPpInitStageGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                   "%d, initStagePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwPpInitStageGet(dev, &initStage);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwPpInitStageGet(dev, &initStage);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwPpSoftResetSkipParamGet
(
    IN  GT_U8                           dev,
    IN  CPSS_HW_PP_RESET_SKIP_TYPE_ENT  skipType,
    OUT GT_BOOL                         *skipEnablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwPpSoftResetSkipParamGet)
{
/*
    ITERATE_DEVICES(All DxCh Devices)
    1.1. Call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E /
                            CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E /
                            CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E /
                            CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E /
                            CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E],
    Expected: GT_OK.
    1.2. Call api with wrong skipType [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.3. Call api with wrong skipEnablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                           dev;
    CPSS_HW_PP_RESET_SKIP_TYPE_ENT  skipType = CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E;
    GT_BOOL                         skipEnable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E /
                                    CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E /
                                    CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E /
                                    CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E /
                                    CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E],
            Expected: GT_OK.
        */

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E;

        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E;

        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E;

        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E;

        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnable);
        if (IS_PEX_SKIP_SUPPORT_DEV_MAC(dev))
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E;

        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnable);
        if (IS_LINK_LOSS_SUPPORT_DEV_MAC(dev))
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);

        /*
            1.2. Call api with wrong skipType [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChHwPpSoftResetSkipParamGet
                            (dev, skipType, &skipEnable),
                            skipType);

        /*
            1.3. Call api with wrong skipEnablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                   "%d, skipEnablePtr = NULL", dev);
    }

    /* restore correct values */
    skipType = CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChHwPpSoftResetSkipParamSet
(
    IN  GT_U8                          dev,
    IN  CPSS_HW_PP_RESET_SKIP_TYPE_ENT skipType,
    IN  GT_BOOL                        skipEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChHwPpSoftResetSkipParamSet)
{
/*
    ITERATE_DEVICES(All DxCh Devices)
    1.1. Call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E /
                            CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E /
                            CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E /
                            CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E /
                            CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E],
                   skipEnable[GT_TRUE / GT_FALSE / GT_TRUE / GT_TRUE / GT_TRUE],
    Expected: GT_OK.
    1.2. Call cpssDxChHwPpSoftResetSkipParamGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong skipType [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                          dev;
    CPSS_HW_PP_RESET_SKIP_TYPE_ENT skipType = CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E;
    GT_BOOL                        skipEnable = GT_FALSE;
    GT_BOOL                        skipEnableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E /
                                    CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E /
                                    CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E /
                                    CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E /
                                    CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E],
                           skipEnable[GT_TRUE / GT_FALSE / GT_TRUE / GT_TRUE / GT_TRUE],
            Expected: GT_OK.
        */

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E],
                     skipEnable[GT_TRUE] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E;
        skipEnable = GT_TRUE;

        st = cpssDxChHwPpSoftResetSkipParamSet(dev, skipType, skipEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChHwPpSoftResetSkipParamGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChHwPpSoftResetSkipParamGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(skipEnable, skipEnableGet,
                       "got another skipEnable then was set: %d", dev);

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E],
                     skipEnable[GT_FALSE] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E;
        skipEnable = GT_FALSE;

        st = cpssDxChHwPpSoftResetSkipParamSet(dev, skipType, skipEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call get func again, 1.2. */
        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChHwPpSoftResetSkipParamGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(skipEnable, skipEnableGet,
                       "got another skipEnable then was set: %d", dev);

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E],
                     skipEnable[GT_TRUE] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E;
        skipEnable = GT_TRUE;

        st = cpssDxChHwPpSoftResetSkipParamSet(dev, skipType, skipEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call get func again, 1.2. */
        st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChHwPpSoftResetSkipParamGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(skipEnable, skipEnableGet,
                       "got another skipEnable then was set: %d", dev);

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E],
                     skipEnable[GT_TRUE] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E;
        skipEnable = GT_TRUE;

        st = cpssDxChHwPpSoftResetSkipParamSet(dev, skipType, skipEnable);
        if (IS_PEX_SKIP_SUPPORT_DEV_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.2. */
            st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChHwPpSoftResetSkipParamGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(skipEnable, skipEnableGet,
                           "got another skipEnable then was set: %d", dev);
        }
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);

        /* call with skipType[CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E],
                     skipEnable[GT_TRUE] */
        skipType = CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E;
        skipEnable = GT_TRUE;

        st = cpssDxChHwPpSoftResetSkipParamSet(dev, skipType, skipEnable);

        if (IS_LINK_LOSS_SUPPORT_DEV_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.2. */
            st = cpssDxChHwPpSoftResetSkipParamGet(dev, skipType, &skipEnableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChHwPpSoftResetSkipParamGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(skipEnable, skipEnableGet,
                           "got another skipEnable then was set: %d", dev);

        }
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);


        /*
            1.3. Call api with wrong skipType [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChHwPpSoftResetSkipParamSet
                            (dev, skipType, skipEnable),
                            skipType);
    }

    /* restore correct values */
    skipType = CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E;
    skipEnable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChHwPpSoftResetSkipParamSet(dev, skipType, skipEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChHwPpSoftResetSkipParamSet(dev, skipType, skipEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChHwInit suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChHwInit)

    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwAuDescSizeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwCoreClockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwInterruptCoalescingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwInterruptCoalescingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwPpPhase1Init)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwPpPhase2Init)
#if 0
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwPpSoftResetTrigger)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwPpStartInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwRxBufAlignmentGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwRxDescSizeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwTxDescSizeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgHwDevNumSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwPpImplementWaInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgHwDevNumGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgHwCheckBadAddrWa)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwPpInitStageGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwPpSoftResetSkipParamGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChHwPpSoftResetSkipParamSet)

UTF_SUIT_END_TESTS_MAC(cpssDxChHwInit)



GT_VOID  checkBadAddrWa
(
    void
)
{
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      data;
    CPSS_DXCH_IMPLEMENT_WA_ENT          waFromCpss[1];
    GT_U32      portGroupId;
    GT_U32      portGroupsBmp;
    GT_U32      unitArray[MAX_UNIT_NUM_CNS];    /* Array of units that are used */
    GT_U32      numOfUnits;         /* total number of units in the device */
    GT_U32      unitId;             /* unit index in the array of units */
    GT_U32      regAddr;            /* register address  */
    GT_U32      unitAddrRange;      /* the last address in the unit */


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(dev)->devFamily)
        {

            /* XCAT units db initialization  */
            /* Unit Id is defined by bits 23-28 */
            numOfUnits = 24;

            unitArray[0] =  0x0;    /* 0x00000000   */
            unitArray[1] =  0x3;    /* 0x01800000   */
            unitArray[2] =  0x4;    /* 0x02000000   */
            unitArray[3] =  0x5;    /* 0x02800000   */
            unitArray[4] =  0x6;    /* 0x03000000   */
            unitArray[5] =  0x7;    /* 0x03800000   */
            unitArray[6] =  0x8;    /* 0x04000000   */
            unitArray[7] =  0xC;    /* 0x06000000   */
            unitArray[8] =  0xD;    /* 0x06800000   */
            unitArray[9] =  0xE;    /* 0x07000000   */
            unitArray[10] = 0xF;    /* 0x07800000   */
            unitArray[11] = 0x10;   /* 0x08000000   */
            unitArray[12] = 0x13;   /* 0x09800000   */
            unitArray[13] = 0x14;   /* 0x0A000000   */
            unitArray[14] = 0x15;   /* 0x0A800000   */
            unitArray[15] = 0x16;   /* 0x0B000000   */
            unitArray[16] = 0x17;   /* 0x0B800000   */
            unitArray[17] = 0x18;   /* 0x0C000000   */
            unitArray[18] = 0x19;   /* 0x0C800000   */
            unitArray[19] = 0x1A;   /* 0x0D000000   */
            unitArray[20] = 0x1C;   /* 0x0E000000   */
            unitArray[21] = 0x1D;   /* 0x0E800000   */
            unitArray[22] = 0x1E;   /* 0x0F000000   */
            unitArray[23] = 0x1F;   /* 0x0F800000   */

            portGroupsBmp = 0x1;
        }

        else if (CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            /* Lion units db initialization */
            /* Unit Id is defined by bits 23-28 */
            numOfUnits = 26;

            unitArray[0] =  0x0;    /* 0x00000000   */
            unitArray[1] =  0x1;    /* 0x00800000   */
            unitArray[2] =  0x2;    /* 0x01000000   */
            unitArray[3] =  0x4;    /* 0x02000000   */
            unitArray[4] =  0x5;    /* 0x02800000   */
            unitArray[5] =  0x6;    /* 0x03000000   */
            unitArray[6] =  0x7;    /* 0x03800000   */
            unitArray[7] =  0x8;    /* 0x04000000   */
            unitArray[8] =  0xC;    /* 0x06000000   */
            unitArray[9] =  0xE;    /* 0x06800000   */
            unitArray[10] = 0xF;    /* 0x07000000   */
            unitArray[11] = 0x10;    /* 0x07800000   */
            unitArray[12] = 0x11;   /* 0x08000000   */
            unitArray[13] = 0x12;   /* 0x08800000   */
            unitArray[14] = 0x13;   /* 0x09000000   */
            unitArray[15] = 0x15;   /* 0x09800000   */
            unitArray[16] = 0x16;   /* 0x0A800000   */
            unitArray[17] = 0x17;   /* 0x0B000000   */
            unitArray[18] = 0x18;   /* 0x0B800000   */
            unitArray[19] = 0x19;   /* 0x0C000000   */
            unitArray[20] = 0x1A;   /* 0x0C800000   */
            unitArray[21] = 0x1B;   /* 0x0D000000   */
            unitArray[22] = 0x1C;   /* 0x0D800000   */
            unitArray[23] = 0x1D;   /* 0x0F000000   */
            unitArray[24] = 0x1E;   /* 0x0F800000   */
            unitArray[25] = 0x1F;   /* 0x0D000000   */

            portGroupsBmp = 0xF;
        }
        else
        {
            /* XCAT units db initialization  */
            /* Unit Id is defined by bits 23-28 */
            numOfUnits = 25;

            unitArray[0] =  0x0;    /* 0x00000000   */
            unitArray[1] =  0x3;    /* 0x01800000   */
            unitArray[2] =  0x4;    /* 0x02000000   */
            unitArray[3] =  0x5;    /* 0x02800000   */
            unitArray[4] =  0x6;    /* 0x03000000   */
            unitArray[5] =  0x7;    /* 0x03800000   */
            unitArray[6] =  0x8;    /* 0x04000000   */
            unitArray[7] =  0xC;    /* 0x06000000   */
            unitArray[8] =  0xF;    /* 0x07800000   */
            unitArray[9] =  0x10;   /* 0x08000000   */
            unitArray[10] = 0x11;   /* 0x08800000   */
            unitArray[11] = 0x12;   /* 0x09000000   */
            unitArray[12] = 0x13;   /* 0x09800000   */
            unitArray[13] = 0x14;   /* 0x0A000000   */
            unitArray[14] = 0x15;   /* 0x0A800000   */
            unitArray[15] = 0x16;   /* 0x0B000000   */
            unitArray[16] = 0x17;   /* 0x0B800000   */
            unitArray[17] = 0x18;   /* 0x0C000000   */
            unitArray[18] = 0x19;   /* 0x0C800000   */
            unitArray[19] = 0x1A;   /* 0x0D000000   */
            unitArray[20] = 0x1B;   /* 0x0D800000   */
            unitArray[21] = 0x1C;   /* 0x0E000000   */
            unitArray[22] = 0x1D;   /* 0x0E800000   */
            unitArray[23] = 0x1E;   /* 0x0F000000   */
            unitArray[24] = 0x1F;   /* 0x0F800000   */

            portGroupsBmp = 0x1;
        }

        /* enable Check Address WA */
        waFromCpss[0] = CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E;
        st = cpssDxChHwPpImplementWaInit(dev,1,&waFromCpss[0],0);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        if(PRV_CPSS_DXCH_PP_MAC(dev)->errata.
            info_PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E.unitRangesPtr == NULL)
        {
            /* The WA is not initialized */
            return;
        }

        /* 1.1. Go over all active port groups from the bitmap */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(dev, portGroupsBmp, portGroupId)
        {
            for (unitId = 0; unitId < numOfUnits; unitId++)
            {
              cpssOsPrintf("------------------------------\n");
              cpssOsPrintf("unit = %d\n", unitArray[unitId]);
              cpssOsPrintf("------------------------------\n");

                /* The first register address in the unit */
                regAddr = (unitArray[unitId] << 23);

                /* calculate the memory range to check */
                unitAddrRange = regAddr + 0x7FFFFF;

                for (regAddr = regAddr; regAddr < unitAddrRange; regAddr +=4 /*64*/)
                {
                    /*cpssOsPrintSync("regAddr = 0x%x\n", regAddr);*/

                    st = prvCpssDrvHwPpPortGroupReadRegister(dev, portGroupId,
                                                             regAddr,
                                                             &data);

                    /* GT_BAD_PARAM status - indicates a restricted address,
                       GT_OK status - indicates valid address */
                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_FAIL, st, dev, portGroupId,
                                                regAddr);

                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_NOT_INITIALIZED, st, dev, portGroupId,
                                                regAddr);

                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_HW_ERROR, st, dev, portGroupId,
                                                regAddr);
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)


        /* Disable Address Checking WA */
        st = prvCpssDrvAddrCheckWaBind(dev,NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    }
}


GT_STATUS   calcReadWriteTime
(
    IN GT_U8    dev,
    IN GT_U32   startRegAddr,
    IN GT_U32   numOfCycles
)
{
    GT_U32      startSeconds;
    GT_U32      startNanoSeconds;
    GT_U32      endSeconds;
    GT_U32      endNanoSeconds;
    GT_U32      seconds;
    GT_U32      nanoseconds;
    GT_U32      i;
    GT_U32      regAddr;
    GT_U32      data;
    GT_STATUS   rc;

    /* Reading cycles */

    regAddr = startRegAddr;

    rc = cpssOsTimeRT(&startSeconds, &startNanoSeconds);
    if (rc != GT_OK)
    {
        return rc;
    }


    for (i = 0; i < numOfCycles; i++)
    {
       rc = prvCpssDrvHwPpReadRegister(dev, regAddr, &data);
       if (rc != GT_OK)
       {
           return rc;
       }

       regAddr += 4;
    }

    rc = cpssOsTimeRT(&endSeconds, &endNanoSeconds);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintSync("------- Reading ---------\n");

    cpssOsPrintSync("Start Time:seconds = %d, nanoseconds = %d\n", startSeconds, startNanoSeconds);
    cpssOsPrintSync("End Time:seconds = %d, nanoseconds = %d\n", endSeconds, endNanoSeconds);

    seconds = endSeconds - startSeconds;
    if (seconds == 0)
    {
        nanoseconds = endNanoSeconds - startNanoSeconds;
    }
    else
        nanoseconds = 0;

    cpssOsPrintSync("Reading %d cycles:seconds = %d, nanoseconds = %d\n", numOfCycles, seconds, nanoseconds);

    return GT_OK;
}
