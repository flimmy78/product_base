/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmMplsUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmMpls, that provides
*       CPSS ExMxPm MPLS Switching API.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/mpls/cpssExMxPmMpls.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define MPLS_INVALID_ENUM_CNS    0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsLabelSpaceModeSet
(
    IN GT_U8                                  devNum,
    IN CPSS_EXMXPM_MPLS_LABEL_SPACE_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsLabelSpaceModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [ CPSS_EXMXPM_MPLS_PER_PLATFORM_E /
                          CPSS_EXMXPM_MPLS_PER_INTERFACE_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsLabelSpaceModeGet with not NULL mode.
    Expected: GT_OK and the same mode as was set.
    1.3. Call with mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MPLS_LABEL_SPACE_MODE_ENT  mode    = CPSS_EXMXPM_MPLS_PER_PLATFORM_E;
    CPSS_EXMXPM_MPLS_LABEL_SPACE_MODE_ENT  modeGet = CPSS_EXMXPM_MPLS_PER_PLATFORM_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_EXMXPM_MPLS_PER_PLATFORM_E / CPSS_EXMXPM_MPLS_PER_INTERFACE_E].
            Expected: GT_OK.
        */
        /* iterate with mode = CPSS_EXMXPM_MPLS_PER_PLATFORM_E */
        mode = CPSS_EXMXPM_MPLS_PER_PLATFORM_E;

        st = cpssExMxPmMplsLabelSpaceModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmMplsLabelSpaceModeGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmMplsLabelSpaceModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsLabelSpaceModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode then was set: %d", dev);

        /* iterate with mode = CPSS_EXMXPM_MPLS_PER_INTERFACE_E */
        mode = CPSS_EXMXPM_MPLS_PER_INTERFACE_E;

        st = cpssExMxPmMplsLabelSpaceModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmMplsLabelSpaceModeGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmMplsLabelSpaceModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsLabelSpaceModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode then was set: %d", dev);


        /*
            1.3. Call with mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsLabelSpaceModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_MPLS_PER_PLATFORM_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsLabelSpaceModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsLabelSpaceModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsLabelSpaceModeGet
(
    IN  GT_U8                                  devNum,
    OUT CPSS_EXMXPM_MPLS_LABEL_SPACE_MODE_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsLabelSpaceModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MPLS_LABEL_SPACE_MODE_ENT mode;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmMplsLabelSpaceModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsLabelSpaceModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsLabelSpaceModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsLabelSpaceModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsEnableGet with not NULL enable.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssExMxPmMplsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmMplsEnableGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmMplsEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable then was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssExMxPmMplsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmMplsEnableGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmMplsEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable then was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsEnableGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmMplsEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsInterfaceEntrySet
(
    IN GT_U8                                 devNum,
    IN GT_U32                                index,
    IN CPSS_EXMXPM_MPLS_INTERFACE_ENTRY_STC  *mplsInterfaceEntryStcPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsInterfaceEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / moduleCfgPtr->mplsCfg.interfaceEntriesNum],
                   mplsInterfaceEntryStcPtr {valid [GT_FALSE / GT_TRUE],
                                             minLabel[16 / 100],
                                             maxLabel[100 / 4095],
                                             baseIndex[100],
                                             ecmpQosSize[0 / 7],
                                             nextHopRouteMethod[CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_ECMP_E /
                                                                CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_REGULAR_E] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsInterfaceEntryGet with not NULL mplsInterfaceEntryStcPtr.
    Expected: GT_OK and the same mplsInterfaceEntryStc as was set.
    1.3. Call with mplsInterfaceEntryStcPtr->nextHopRouteMethod[MPLS_INVALID_ENUM_CNS].
    Expected: GT_BAD_PARAM.
    1.4. Call with mplsInterfaceEntryStcPtr->ecmpQosSize[8].
    Expected: NOT GT_OK.
    1.5. Call with mplsInterfaceEntryStcPtr->minLabel [15] (out of range)
    Expected: NOT GT_OK.
    1.6. Call with mplsInterfaceEntryStcPtr->minLabel [18],
                   mplsInterfaceEntryStcPtr->maxLabel [16] (minLabel > maxLabel not supported)
    Expected: NOT GT_OK.
    1.7. Call with index [moduleCfgPtr->mplsCfg.interfaceEntriesNum+1] (out of range).
    Expected: NOT GT_OK.
    1.8. Call with mplsInterfaceEntryStcPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    GT_U32      index = 0;

    CPSS_EXMXPM_MPLS_INTERFACE_ENTRY_STC    mplsInterfaceEntryStc;
    CPSS_EXMXPM_MPLS_INTERFACE_ENTRY_STC    mplsInterfaceEntryStcGet;


    cpssOsBzero((GT_VOID*) &(mplsInterfaceEntryStc), sizeof(mplsInterfaceEntryStc));
    cpssOsBzero((GT_VOID*) &(mplsInterfaceEntryStcGet), sizeof(mplsInterfaceEntryStcGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / moduleCfgPtr->mplsCfg.interfaceEntriesNum],
                           mplsInterfaceEntryStcPtr {valid [GT_FALSE / GT_TRUE],
                                                     minLabel[16 / 100],
                                                     maxLabel[100 / 4095],
                                                     baseIndex[100],
                                                     ecmpQosSize[0 / 7],
                                                     nextHopRouteMethod[CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_ECMP_E /
                                                                        CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_REGULAR_E] }.
            Expected: GT_OK.
        */
        /* mplsInterfaceEntryStc.valid = GT_FALSE */
        index = 0;

        mplsInterfaceEntryStc.valid              = GT_FALSE;
        mplsInterfaceEntryStc.minLabel           = 16;
        mplsInterfaceEntryStc.maxLabel           = 100;
        mplsInterfaceEntryStc.baseIndex          = 100;
        mplsInterfaceEntryStc.ecmpQosSize        = 0;
        mplsInterfaceEntryStc.nextHopRouteMethod = CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_ECMP_E;

        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call cpssExMxPmMplsInterfaceEntryGet with not NULL mplsInterfaceEntryStcPtr.
            Expected: GT_OK and the same mplsInterfaceEntryStc as was set.
        */
        st = cpssExMxPmMplsInterfaceEntryGet(dev, index, &mplsInterfaceEntryStcGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsInterfaceEntryGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mplsInterfaceEntryStc, (GT_VOID*) &mplsInterfaceEntryStcGet, sizeof(mplsInterfaceEntryStc) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another mplsInterfaceEntryStc than was set: %d", dev);

        /* mplsInterfaceEntryStc.valid = GT_TRUE */

        /* legal index is [ 0 <= index < interfaceEntriesNum ] */
        index = (PRV_CPSS_EXMXPM_DEV_MODULE_CFG_MAC(dev)->mplsCfg.interfaceEntriesNum)-1;

        mplsInterfaceEntryStc.valid              = GT_TRUE;
        mplsInterfaceEntryStc.minLabel           = 100;
        mplsInterfaceEntryStc.maxLabel           = 4095;
        mplsInterfaceEntryStc.baseIndex          = 100;
        mplsInterfaceEntryStc.ecmpQosSize        = 7;
        mplsInterfaceEntryStc.nextHopRouteMethod = CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_REGULAR_E;

        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call cpssExMxPmMplsInterfaceEntryGet with not NULL mplsInterfaceEntryStcPtr.
            Expected: GT_OK and the same mplsInterfaceEntryStc as was set.
        */
        st = cpssExMxPmMplsInterfaceEntryGet(dev, index, &mplsInterfaceEntryStcGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsInterfaceEntryGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mplsInterfaceEntryStc, (GT_VOID*) &mplsInterfaceEntryStcGet, sizeof(mplsInterfaceEntryStc) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another mplsInterfaceEntryStc than was set: %d", dev);

        /*
            1.3. Call with mplsInterfaceEntryStcPtr->nextHopRouteMethod[MPLS_INVALID_ENUM_CNS].
            Expected: NOT GT_OK.
        */
        mplsInterfaceEntryStc.nextHopRouteMethod = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, mplsInterfaceEntryStcPtr->nextHopRouteMethod = %d",
                                    dev, mplsInterfaceEntryStc.nextHopRouteMethod);

        mplsInterfaceEntryStc.nextHopRouteMethod = CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_REGULAR_E;

        /*
            1.4. Call with mplsInterfaceEntryStcPtr->ecmpQosSize[8].
            Expected: NOT GT_OK.
        */
        mplsInterfaceEntryStc.ecmpQosSize = 8;

        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mplsInterfaceEntryStcPtr->ecmpQosSize = %d",
                                         dev, mplsInterfaceEntryStc.ecmpQosSize);

        mplsInterfaceEntryStc.ecmpQosSize = 7;

        /*
            1.5. Call with mplsInterfaceEntryStcPtr->minLabel [15] (out of range)
            Expected: NOT GT_OK.
        */
        mplsInterfaceEntryStc.minLabel = 15;

        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mplsInterfaceEntryStcPtr->minLabel = %d",
                                         dev, mplsInterfaceEntryStc.minLabel);

        mplsInterfaceEntryStc.minLabel = 18;

        /*
            1.6. Call with mplsInterfaceEntryStcPtr->minLabel [18],
                           mplsInterfaceEntryStcPtr->maxLabel [16] (minLabel > maxLabel not supported)
            Expected: NOT GT_OK.
        */
        mplsInterfaceEntryStc.minLabel = 18;
        mplsInterfaceEntryStc.maxLabel = 16;

        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, mplsInterfaceEntryStcPtr->minLabel = %d, mplsInterfaceEntryStcPtr->maxLabel = %d",
                                         dev, mplsInterfaceEntryStc.minLabel, mplsInterfaceEntryStc.maxLabel);

        mplsInterfaceEntryStc.maxLabel = 100;

        /*
            1.7. Call with index [moduleCfgPtr->mplsCfg.interfaceEntriesNum+1] (out of range).
            Expected: NOT GT_OK.
        */
        index = PRV_CPSS_EXMXPM_DEV_MODULE_CFG_MAC(dev)->mplsCfg.interfaceEntriesNum;

        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.8. Call with mplsInterfaceEntryStcPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mplsInterfaceEntryStcPtr = NULL", dev);
    }

    index = 0;

    mplsInterfaceEntryStc.valid              = GT_FALSE;
    mplsInterfaceEntryStc.minLabel           = 0;
    mplsInterfaceEntryStc.maxLabel           = 10;
    mplsInterfaceEntryStc.baseIndex          = 100;
    mplsInterfaceEntryStc.ecmpQosSize        = 0;
    mplsInterfaceEntryStc.nextHopRouteMethod = CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_ECMP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsInterfaceEntrySet(dev, index, &mplsInterfaceEntryStc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsInterfaceEntryGet
(
    IN  GT_U8                                 devNum,
    IN  GT_U32                                index,
    OUT CPSS_EXMXPM_MPLS_INTERFACE_ENTRY_STC  *mplsInterfaceEntryStcPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsInterfaceEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / moduleCfgPtr->mplsCfg.interfaceEntriesNum], not NULL mplsInterfaceEntryStcPtr.
    Expected: GT_OK.
    1.2. Call with mplsInterfaceEntryStcPtr[NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with index [moduleCfgPtr->mplsCfg.interfaceEntriesNum+1] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      index = 0;

    CPSS_EXMXPM_MPLS_INTERFACE_ENTRY_STC    mplsInterfaceEntryStc;


    cpssOsBzero((GT_VOID*) &(mplsInterfaceEntryStc), sizeof(mplsInterfaceEntryStc));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / moduleCfgPtr->mplsCfg.interfaceEntriesNum], not NULL mplsInterfaceEntryStcPtr.
            Expected: GT_OK.
        */
        index = 0;

        st = cpssExMxPmMplsInterfaceEntryGet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* legal index is [ 0 <= index < interfaceEntriesNum ] */
        index = (PRV_CPSS_EXMXPM_DEV_MODULE_CFG_MAC(dev)->mplsCfg.interfaceEntriesNum)-1;

        st = cpssExMxPmMplsInterfaceEntryGet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with mplsInterfaceEntryStcPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsInterfaceEntryGet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mplsInterfaceEntryStcPtr = NULL", dev);

        /*
            1.3. Call with index [moduleCfgPtr->mplsCfg.interfaceEntriesNum+1] (out of range).
            Expected: NOT GT_OK.
        */
        index = PRV_CPSS_EXMXPM_DEV_MODULE_CFG_MAC(dev)->mplsCfg.interfaceEntriesNum;

        st = cpssExMxPmMplsInterfaceEntryGet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsInterfaceEntryGet(dev, index, &mplsInterfaceEntryStc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsInterfaceEntryGet(dev, index, &mplsInterfaceEntryStc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsInvalidEntryCmdSet
(
    IN GT_U8                               devNum,
    IN CPSS_PACKET_CMD_ENT                 cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsInvalidEntryCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E / CPSS_PACKET_CMD_DROP_HARD_E]
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsInvalidEntryCmdGet with not NULL cmdPtr.
    Expected: GT_OK and the same cmd as was set.
    1.3. Call with cmd[CPSS_PACKET_CMD_FORWARD_E] not supported
    Expected: NOT GT_OK.
    1.4. Call with cmd[MPLS_INVALID_ENUM_CNS] out of range
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT     cmd    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    CPSS_PACKET_CMD_ENT     cmdGet = CPSS_PACKET_CMD_TRAP_TO_CPU_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E / CPSS_PACKET_CMD_DROP_HARD_E]
            Expected: GT_OK.
        */
        /* iterate with cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E */
        cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmMplsInvalidEntryCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmMplsInvalidEntryCmdGet with not NULL cmdPtr.
            Expected: GT_OK and the same cmd as was set.
        */
        st = cpssExMxPmMplsInvalidEntryCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsInvalidEntryCmdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet, "got another cmd than was set: %d", dev);

        /* iterate with cmd = CPSS_PACKET_CMD_DROP_HARD_E */
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmMplsInvalidEntryCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmMplsInvalidEntryCmdGet with not NULL cmdPtr.
            Expected: GT_OK and the same cmd as was set.
        */
        st = cpssExMxPmMplsInvalidEntryCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsInvalidEntryCmdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet, "got another cmd than was set: %d", dev);

        /*
            1.3. Call with cmd[CPSS_PACKET_CMD_FORWARD_E] not supported
            Expected: NOT GT_OK.
        */
        cmd = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmMplsInvalidEntryCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.4. Call with cmd[MPLS_INVALID_ENUM_CNS] out of range
            Expected: GT_BAD_PARAM.
        */
        cmd = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsInvalidEntryCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cmd);
    }

    cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsInvalidEntryCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsInvalidEntryCmdSet(dev, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsInvalidEntryCmdGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_PACKET_CMD_ENT                 *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsInvalidEntryCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with cmdPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT     cmd;


    cmd = CPSS_PACKET_CMD_NONE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL cmdPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmMplsInvalidEntryCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with cmdPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsInvalidEntryCmdGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsInvalidEntryCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsInvalidEntryCmdGet(dev, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsSpecialRouterTriggerEnableSet
(
    IN GT_U8                                                devNum,
    IN CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TYPE_ENT     routerTriggerType,
    IN GT_BOOL                                              enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsSpecialRouterTriggerEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with routerTriggerType [CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E /
                                      CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_SOFT_DROP_E],
                   enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsSpecialRouterTriggerEnableGet with not NULL enablePtr
                and the same param as in 1.1.
    Expected: GT_OK and enable as was set.
    1.3. Call with routerTriggerType [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TYPE_ENT     routerTriggerType;
    GT_BOOL                                              enable;
    GT_BOOL                                              enableGet;


    enable            = GT_FALSE;
    enableGet         = GT_FALSE;
    routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerTriggerType [CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E /
                                              CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_SOFT_DROP_E],
                           enable[GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E */
        routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E;
        enable            = GT_FALSE;

        st = cpssExMxPmMplsSpecialRouterTriggerEnableSet(dev, routerTriggerType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, routerTriggerType, enable);

        /*
            1.2. Call cpssExMxPmMplsSpecialRouterTriggerEnableGet with not NULL enablePtr
                        and the same param as in 1.1.
            Expected: GT_OK and enable as was set.
        */
        st = cpssExMxPmMplsSpecialRouterTriggerEnableGet(dev, routerTriggerType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmMplsSpecialRouterTriggerEnableGet: %d, %d",
                                    dev, routerTriggerType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /* iterate with routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_SOFT_DROP_E */
        routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_SOFT_DROP_E;
        enable            = GT_TRUE;

        st = cpssExMxPmMplsSpecialRouterTriggerEnableSet(dev, routerTriggerType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, routerTriggerType, enable);

        /*
            1.2. Call cpssExMxPmMplsSpecialRouterTriggerEnableGet with not NULL enablePtr
                        and the same param as in 1.1.
            Expected: GT_OK and enable as was set.
        */
        st = cpssExMxPmMplsSpecialRouterTriggerEnableGet(dev, routerTriggerType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmMplsSpecialRouterTriggerEnableGet: %d, %d",
                                    dev, routerTriggerType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /*
            1.3. Call with routerTriggerType [0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routerTriggerType = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsSpecialRouterTriggerEnableSet(dev, routerTriggerType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, routerTriggerType);
    }

    routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E;
    enable            = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsSpecialRouterTriggerEnableSet(dev, routerTriggerType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsSpecialRouterTriggerEnableSet(dev, routerTriggerType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsSpecialRouterTriggerEnableGet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TYPE_ENT    routerTriggerType,
    OUT GT_BOOL                                             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsSpecialRouterTriggerEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with routerTriggerType [CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E /
                                      CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_SOFT_DROP_E],
                   and not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL];
    Expected: GT_BAD_PTR.
    1.3. Call with routerTriggerType [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TYPE_ENT    routerTriggerType;
    GT_BOOL                                             enable;


    enable            = GT_FALSE;
    routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerTriggerType [CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E /
                                              CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_SOFT_DROP_E],
                           and not NULL enablePtr.
            Expected: GT_OK.
        */
        /* iterate with routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E */
        routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E;

        st = cpssExMxPmMplsSpecialRouterTriggerEnableGet(dev, routerTriggerType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerTriggerType);

        /* iterate with routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_SOFT_DROP_E */
        routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_SOFT_DROP_E;

        st = cpssExMxPmMplsSpecialRouterTriggerEnableGet(dev, routerTriggerType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerTriggerType);

        /*
            1.2. Call with enablePtr [NULL];
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsSpecialRouterTriggerEnableGet(dev, routerTriggerType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);

        /*
            1.3. Call with routerTriggerType [0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routerTriggerType = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsSpecialRouterTriggerEnableGet(dev, routerTriggerType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, routerTriggerType);
    }

    routerTriggerType = CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TRAP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsSpecialRouterTriggerEnableGet(dev, routerTriggerType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsSpecialRouterTriggerEnableGet(dev, routerTriggerType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet
(
    IN GT_U8   devNum,
    IN GT_U32  baseIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with baseIndex[0 / 100].
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet with not NULL baseIndexPtr
    Expected: GT_OK and tha same baseIndex as was set.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  baseIndex    = 0;
    GT_U32  baseIndexGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with baseIndex[0 / 100].
            Expected: GT_OK.
        */
        /* iterate with baseIndex = 0 */
        baseIndex = 0;

        st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet(dev, baseIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, baseIndex);

        /*
            1.2. Call cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet with not NULL baseIndexPtr
            Expected: GT_OK and tha same baseIndex as was set.
        */
        st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet(dev, &baseIndexGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(baseIndex, baseIndexGet, "got another baseIndex than was set: %d", dev);

        /* iterate with baseIndex = 100 */
        baseIndex = PRV_CPSS_EXMXPM_DEV_MODULE_CFG_MAC(dev)->mplsCfg.nhlfeNum - 16;

        st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet(dev, baseIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, baseIndex);

        /*
            1.2. Call cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet with not NULL baseIndexPtr
            Expected: GT_OK and tha same baseIndex as was set.
        */
        st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet(dev, &baseIndexGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(baseIndex, baseIndexGet, "got another baseIndex than wasset: %d", dev);
    }

    baseIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet(dev, baseIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet(dev, baseIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *baseIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL baseIndexPtr.
    Expected: GT_OK.
    1.2. Call with baseIndexPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      baseIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL baseIndexPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet(dev, &baseIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with baseIndexPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, baseIndexPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet(dev, &baseIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet(dev, &baseIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsNhlfEntriesWrite
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              nhlfEntryBaseIndex,
    IN  GT_U32                              numOfNhlfEntries,
    IN  CPSS_EXMXPM_MPLS_NHLFE_STC          *nhlfEntriesArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsNhlfEntriesWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with nhlfEntryBaseIndex[0],
                   numOfNhlfEntries[0],
                   nhlfEntriesArrayPtr{nhlfeCommonParams{
                                                         mplsLabel[0],
                                                         mplsCmd[CPSS_EXMXPM_MPLS_NOP_CMD_E],
                                                         outlifConfig{
                                                                        outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                        interfaceInfo{
                                                                                        type[CPSS_INTERFACE_TRUNK_E],
                                                                                        trunkId[100]},
                                                                        outlifPointer{arpPtr [100] }
                                                                      },
                                                         nextHopVlanId[100],
                                                         ageRefresh[GT_FALSE / GT_TRUE],
                                                         mtuProfileIndex[0 / 15],
                                                         counterSetIndex[CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E /
                                                                         CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_3_E],
                                                         mirrorToIngressAnalyzer[GT_FALSE / GT_TRUE],
                                                         ttl[0 / 100],
                                                         ttlMode[CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E /
                                                                 CPSS_EXMXPM_MPLS_TTL_DECREMENTED_BY_ENTRY_E],
                                                         bypassTtlExceptionCheckEnable[GT_FALSE / GT_TRUE],
                                                         cpuCodeIndex[0],
                                                         packetCmd[CPSS_CMD_DROP_HARD_E /
                                                                   CPSS_CMD_DROP_SOFT_E],
                                                        }
                                       qosParamsModify{
                                                        modifyTc[GT_FALSE / GT_TRUE],
                                                        modifyUp[GT_FALSE / GT_TRUE],
                                                        modifyDp[GT_FALSE / GT_TRUE],
                                                        modifyDscp[GT_FALSE / GT_TRUE],
                                                        modifyExp[GT_FALSE / GT_TRUE] }
                                       qosParams{
                                                tc[0 / 7],
                                                dp[CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E],
                                                up[0 / 7],
                                                dscp[0 / 63],
                                                exp[0 / 7] } }
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsNhlfEntriesRead with not NULL nhlfEntriesArrayPtr and other params from 1.1.
    Expected: GT_OK and the same nhlfEntriesArray as was set.
    1.3. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.mplsCmd[0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifType[0x5AAAAAA5] out of range
              and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.type [0x5AAAAAA5] out of range
              and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.mtuProfileIndex [16] out of range
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.counterSetIndex [0x5AAAAAA5] out of range
              and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.ttlMode [0x5AAAAAA5] out of range
              and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd [0x5AAAAAA5] out of range
              and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd [CPSS_PACKET_CMD_BRIDGE_E] not supported
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.11. Call with nhlfEntriesArrayPtr->qosParams.tc [8] out of range
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.12. Call with nhlfEntriesArrayPtr->qosParams.dp [0x5AAAAAA5] out of range
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with nhlfEntriesArrayPtr->qosParams.up [8] out of range
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.14. Call with nhlfEntriesArrayPtr->qosParams.dscp [64] out of range
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.15. Call with nhlfEntriesArrayPtr->qosParams.exp [8] out of range
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.16. Call with nhlfEntriesArrayPtr [NULL].
              and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                              nhlfEntryBaseIndex;
    GT_U32                              numOfNhlfEntries;
    CPSS_EXMXPM_MPLS_NHLFE_STC          nhlfEntriesArray;
    CPSS_EXMXPM_MPLS_NHLFE_STC          nhlfEntriesArrayGet;


    numOfNhlfEntries   = 0;
    nhlfEntryBaseIndex = 0;

    cpssOsBzero((GT_VOID*) &nhlfEntriesArray, sizeof(nhlfEntriesArray));
    cpssOsBzero((GT_VOID*) &nhlfEntriesArrayGet, sizeof(nhlfEntriesArrayGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with nhlfEntryBaseIndex[0 / 1],
                           numOfNhlfEntries[1],
                           nhlfEntriesArrayPtr{nhlfeCommonParams{
                                                                 mplsLabel[0],
                                                                 mplsCmd[CPSS_EXMXPM_MPLS_NOP_CMD_E / CPSS_EXMXPM_MPLS_PUSH_CMD_E],
                                                                 outlifConfig{
                                                                                outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                                interfaceInfo{
                                                                                                type[CPSS_INTERFACE_TRUNK_E],
                                                                                                trunkId[100]},
                                                                                outlifPointer{arpPtr [100] }
                                                                              },
                                                                 nextHopVlanId[100 / 4095],
                                                                 ageRefresh[GT_FALSE / GT_TRUE],
                                                                 mtuProfileIndex[0 / 15],
                                                                 counterSetIndex[CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E /
                                                                                 CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_3_E],
                                                                 mirrorToIngressAnalyzer[GT_FALSE / GT_TRUE],
                                                                 ttl[0 / 100],
                                                                 ttlMode[CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E /
                                                                         CPSS_EXMXPM_MPLS_TTL_DECREMENTED_BY_ENTRY_E],
                                                                 bypassTtlExceptionCheckEnable[GT_FALSE / GT_TRUE],
                                                                 cpuCodeIndex[0],
                                                                 packetCmd[CPSS_CMD_DROP_HARD_E /
                                                                           CPSS_CMD_DROP_SOFT_E],
                                                                }
                                               qosParamsModify{
                                                                modifyTc[GT_FALSE / GT_TRUE],
                                                                modifyUp[GT_FALSE / GT_TRUE],
                                                                modifyDp[GT_FALSE / GT_TRUE],
                                                                modifyDscp[GT_FALSE / GT_TRUE],
                                                                modifyExp[GT_FALSE / GT_TRUE] }
                                               qosParams{
                                                        tc[0 / 7],
                                                        dp[CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E],
                                                        up[0 / 7],
                                                        dscp[0 / 63],
                                                        exp[0 / 7] } }
            Expected: GT_OK.
        */
        /* iterate with nhlfEntryBaseIndex = 0 */
        nhlfEntryBaseIndex = 0;
        numOfNhlfEntries   = 1;

        nhlfEntriesArray.nhlfeCommonParams.mplsLabel = 0;
        nhlfEntriesArray.nhlfeCommonParams.mplsCmd   = CPSS_EXMXPM_MPLS_SWAP_CMD_E;


        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId = 2;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr = 0;

        nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId = 100;
        nhlfEntriesArray.nhlfeCommonParams.ageRefresh = GT_FALSE;
        nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = 15;
        nhlfEntriesArray.nhlfeCommonParams.counterSetIndex = CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E;
        nhlfEntriesArray.nhlfeCommonParams.mirrorToIngressAnalyzer = GT_FALSE;
        nhlfEntriesArray.nhlfeCommonParams.ttl = 0;
        nhlfEntriesArray.nhlfeCommonParams.ttlMode = CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E;
        nhlfEntriesArray.nhlfeCommonParams.bypassTtlExceptionCheckEnable = GT_FALSE;
        nhlfEntriesArray.nhlfeCommonParams.cpuCodeIndex = 0;
        nhlfEntriesArray.nhlfeCommonParams.packetCmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        nhlfEntriesArray.qosParamsModify.modifyTc   = GT_FALSE;
        nhlfEntriesArray.qosParamsModify.modifyUp   = GT_FALSE;
        nhlfEntriesArray.qosParamsModify.modifyDp   = GT_FALSE;
        nhlfEntriesArray.qosParamsModify.modifyDscp = GT_FALSE;
        nhlfEntriesArray.qosParamsModify.modifyExp  = GT_FALSE;

        nhlfEntriesArray.qosParams.tc   = 0;
        nhlfEntriesArray.qosParams.dp   = CPSS_DP_GREEN_E;
        nhlfEntriesArray.qosParams.up   = 0;
        nhlfEntriesArray.qosParams.dscp = 0;
        nhlfEntriesArray.qosParams.exp  = 0;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, nhlfEntryBaseIndex, numOfNhlfEntries);

        /*
            1.2. Call cpssExMxPmMplsNhlfEntriesRead with not NULL nhlfEntriesArrayPtr and other params from 1.1.
            Expected: GT_OK and the same nhlfEntriesArray as was set.
        */
        st = cpssExMxPmMplsNhlfEntriesRead(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArrayGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmMplsNhlfEntriesRead: %d, %d, %d",
                                    dev, nhlfEntryBaseIndex, numOfNhlfEntries);

       /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mplsLabel, nhlfEntriesArrayGet.nhlfeCommonParams.mplsLabel,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mplsLabel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mplsCmd, nhlfEntriesArrayGet.nhlfeCommonParams.mplsCmd,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mplsCmd than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.outlifType,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.interfaceInfo.type,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId, nhlfEntriesArrayGet.nhlfeCommonParams.nextHopVlanId,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.nextHopVlanId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ageRefresh, nhlfEntriesArrayGet.nhlfeCommonParams.ageRefresh,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ageRefresh than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex, nhlfEntriesArrayGet.nhlfeCommonParams.mtuProfileIndex,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mtuProfileIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.counterSetIndex, nhlfEntriesArrayGet.nhlfeCommonParams.counterSetIndex,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.counterSetIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mirrorToIngressAnalyzer, nhlfEntriesArrayGet.nhlfeCommonParams.mirrorToIngressAnalyzer,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mirrorToIngressAnalyzer than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ttl, nhlfEntriesArrayGet.nhlfeCommonParams.ttl,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ttl than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ttlMode, nhlfEntriesArrayGet.nhlfeCommonParams.ttlMode,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ttlMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.bypassTtlExceptionCheckEnable, nhlfEntriesArrayGet.nhlfeCommonParams.bypassTtlExceptionCheckEnable,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.bypassTtlExceptionCheckEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.cpuCodeIndex, nhlfEntriesArrayGet.nhlfeCommonParams.cpuCodeIndex,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.cpuCodeIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.packetCmd, nhlfEntriesArrayGet.nhlfeCommonParams.packetCmd,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyTc, nhlfEntriesArrayGet.qosParamsModify.modifyTc,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyUp, nhlfEntriesArrayGet.qosParamsModify.modifyUp,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyUp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyDp, nhlfEntriesArrayGet.qosParamsModify.modifyDp,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyDscp, nhlfEntriesArrayGet.qosParamsModify.modifyDscp,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyDscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyExp, nhlfEntriesArrayGet.qosParamsModify.modifyExp,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyExp than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.tc, nhlfEntriesArrayGet.qosParams.tc,
                   "got another nhlfEntriesArrayPtr->qosParams.tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.dp, nhlfEntriesArrayGet.qosParams.dp,
                   "got another nhlfEntriesArrayPtr->qosParams.dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.up, nhlfEntriesArrayGet.qosParams.up,
                   "got another nhlfEntriesArrayPtr->qosParams.up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.dscp, nhlfEntriesArrayGet.qosParams.dscp,
                   "got another nhlfEntriesArrayPtr->qosParams.dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.exp, nhlfEntriesArrayGet.qosParams.exp,
                   "got another nhlfEntriesArrayPtr->qosParams.exp than was set: %d", dev);



        /* iterate with nhlfEntryBaseIndex = 1 */
        nhlfEntryBaseIndex = 1;
        numOfNhlfEntries   = 1;

        nhlfEntriesArray.nhlfeCommonParams.mplsLabel = 0;
        nhlfEntriesArray.nhlfeCommonParams.mplsCmd   = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId = 100;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr = 100;

        nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId = 4095;
        nhlfEntriesArray.nhlfeCommonParams.ageRefresh = GT_TRUE;
        nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = 0;
        nhlfEntriesArray.nhlfeCommonParams.counterSetIndex = CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_3_E;
        nhlfEntriesArray.nhlfeCommonParams.mirrorToIngressAnalyzer = GT_TRUE;
        nhlfEntriesArray.nhlfeCommonParams.ttl = 255;
        nhlfEntriesArray.nhlfeCommonParams.ttlMode = CPSS_EXMXPM_MPLS_TTL_DECREMENTED_BY_ENTRY_E;
        nhlfEntriesArray.nhlfeCommonParams.bypassTtlExceptionCheckEnable = GT_FALSE;
        nhlfEntriesArray.nhlfeCommonParams.cpuCodeIndex = 3;
        nhlfEntriesArray.nhlfeCommonParams.packetCmd  = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        nhlfEntriesArray.qosParamsModify.modifyTc   = GT_TRUE;
        nhlfEntriesArray.qosParamsModify.modifyUp   = GT_TRUE;
        nhlfEntriesArray.qosParamsModify.modifyDp   = GT_TRUE;
        nhlfEntriesArray.qosParamsModify.modifyDscp = GT_TRUE;
        nhlfEntriesArray.qosParamsModify.modifyExp  = GT_TRUE;

        nhlfEntriesArray.qosParams.tc   = 7;
        nhlfEntriesArray.qosParams.dp   = CPSS_DP_YELLOW_E;
        nhlfEntriesArray.qosParams.up   = 7;
        nhlfEntriesArray.qosParams.dscp = 63;
        nhlfEntriesArray.qosParams.exp  = 7;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, nhlfEntryBaseIndex, numOfNhlfEntries);

        /*
            1.2. Call cpssExMxPmMplsNhlfEntriesRead with not NULL nhlfEntriesArrayPtr and other params from 1.1.
            Expected: GT_OK and the same nhlfEntriesArray as was set.
        */
        st = cpssExMxPmMplsNhlfEntriesRead(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArrayGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmMplsNhlfEntriesRead: %d, %d, %d",
                                    dev, nhlfEntryBaseIndex, numOfNhlfEntries);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mplsLabel, nhlfEntriesArrayGet.nhlfeCommonParams.mplsLabel,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mplsLabel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mplsCmd, nhlfEntriesArrayGet.nhlfeCommonParams.mplsCmd,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mplsCmd than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.outlifType,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.interfaceInfo.type,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId, nhlfEntriesArrayGet.nhlfeCommonParams.nextHopVlanId,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.nextHopVlanId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ageRefresh, nhlfEntriesArrayGet.nhlfeCommonParams.ageRefresh,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ageRefresh than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex, nhlfEntriesArrayGet.nhlfeCommonParams.mtuProfileIndex,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mtuProfileIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.counterSetIndex, nhlfEntriesArrayGet.nhlfeCommonParams.counterSetIndex,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.counterSetIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mirrorToIngressAnalyzer, nhlfEntriesArrayGet.nhlfeCommonParams.mirrorToIngressAnalyzer,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mirrorToIngressAnalyzer than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ttl, nhlfEntriesArrayGet.nhlfeCommonParams.ttl,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ttl than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ttlMode, nhlfEntriesArrayGet.nhlfeCommonParams.ttlMode,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ttlMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.bypassTtlExceptionCheckEnable, nhlfEntriesArrayGet.nhlfeCommonParams.bypassTtlExceptionCheckEnable,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.bypassTtlExceptionCheckEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.cpuCodeIndex, nhlfEntriesArrayGet.nhlfeCommonParams.cpuCodeIndex,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.cpuCodeIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.packetCmd, nhlfEntriesArrayGet.nhlfeCommonParams.packetCmd,
                   "got another nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyTc, nhlfEntriesArrayGet.qosParamsModify.modifyTc,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyUp, nhlfEntriesArrayGet.qosParamsModify.modifyUp,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyUp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyDp, nhlfEntriesArrayGet.qosParamsModify.modifyDp,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyDscp, nhlfEntriesArrayGet.qosParamsModify.modifyDscp,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyDscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyExp, nhlfEntriesArrayGet.qosParamsModify.modifyExp,
                   "got another nhlfEntriesArrayPtr->qosParamsModify.modifyExp than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.tc, nhlfEntriesArrayGet.qosParams.tc,
                   "got another nhlfEntriesArrayPtr->qosParams.tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.dp, nhlfEntriesArrayGet.qosParams.dp,
                   "got another nhlfEntriesArrayPtr->qosParams.dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.up, nhlfEntriesArrayGet.qosParams.up,
                   "got another nhlfEntriesArrayPtr->qosParams.up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.dscp, nhlfEntriesArrayGet.qosParams.dscp,
                   "got another nhlfEntriesArrayPtr->qosParams.dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.exp, nhlfEntriesArrayGet.qosParams.exp,
                   "got another nhlfEntriesArrayPtr->qosParams.exp than was set: %d", dev);



        /*
            1.3. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.mplsCmd[0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nhlfEntriesArray.nhlfeCommonParams.mplsCmd = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nhlfEntriesArrayPtr->nhlfeCommonParams.mplsCmd = %d",
                                    dev, nhlfEntriesArray.nhlfeCommonParams.mplsCmd);

        nhlfEntriesArray.nhlfeCommonParams.mplsCmd = CPSS_EXMXPM_MPLS_NOP_CMD_E;

        /*
            1.4. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifType[0x5AAAAAA5] out of range
                      and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifType = %d",
                                    dev, nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType);

        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType = CPSS_EXMXPM_MPLS_NOP_CMD_E;

        /*
            1.5. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.type [0x5AAAAAA5] out of range
                      and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.type = %d",
                                    dev, nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type);

        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type = CPSS_EXMXPM_MPLS_NOP_CMD_E;

        /*
            1.6. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.mtuProfileIndex [16] out of range
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = 16;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nhlfEntriesArrayPtr->nhlfeCommonParams.mtuProfileIndex = %d",
                                        dev, nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex);

        nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = 15;

        /*
            1.7. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.counterSetIndex [0x5AAAAAA5] out of range
                      and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nhlfEntriesArray.nhlfeCommonParams.counterSetIndex = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nhlfEntriesArrayPtr->nhlfeCommonParams.counterSetIndex = %d",
                                     dev, nhlfEntriesArray.nhlfeCommonParams.counterSetIndex);

        nhlfEntriesArray.nhlfeCommonParams.counterSetIndex = CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_3_E;

        /*
            1.8. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.ttlMode [0x5AAAAAA5] out of range
                      and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nhlfEntriesArray.nhlfeCommonParams.ttlMode = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nhlfEntriesArrayPtr->nhlfeCommonParams.ttlMode = %d",
                                     dev, nhlfEntriesArray.nhlfeCommonParams.ttlMode);

        nhlfEntriesArray.nhlfeCommonParams.ttlMode = CPSS_EXMXPM_MPLS_TTL_DECREMENTED_BY_ENTRY_E;

        /*
            1.9. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd [0x5AAAAAA5] out of range
                      and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nhlfEntriesArray.nhlfeCommonParams.packetCmd = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd = %d",
                                     dev, nhlfEntriesArray.nhlfeCommonParams.packetCmd);

        nhlfEntriesArray.nhlfeCommonParams.packetCmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        /*
            1.10. Call with nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd [CPSS_PACKET_CMD_BRIDGE_E] not supported
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        nhlfEntriesArray.nhlfeCommonParams.packetCmd = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd = %d",
                                         dev, nhlfEntriesArray.nhlfeCommonParams.packetCmd);

        nhlfEntriesArray.nhlfeCommonParams.packetCmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        /*
            1.11. Call with nhlfEntriesArrayPtr->qosParams.tc [8] out of range
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        nhlfEntriesArray.qosParams.tc = 8;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nhlfEntriesArrayPtr->qosParams.tc = %d",
                                         dev, nhlfEntriesArray.qosParams.tc);

        nhlfEntriesArray.qosParams.tc = 7;

        /*
            1.12. Call with nhlfEntriesArrayPtr->qosParams.dp [0x5AAAAAA5] out of range
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        nhlfEntriesArray.qosParams.dp = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nhlfEntriesArrayPtr->qosParams.dp = %d",
                                         dev, nhlfEntriesArray.qosParams.dp);

        nhlfEntriesArray.qosParams.dp = CPSS_DP_GREEN_E;

        /*
            1.13. Call with nhlfEntriesArrayPtr->qosParams.up [8] out of range
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        nhlfEntriesArray.qosParams.up = 8;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nhlfEntriesArrayPtr->qosParams.up = %d",
                                         dev, nhlfEntriesArray.qosParams.up);

        nhlfEntriesArray.qosParams.up = 7;

        /*
            1.14. Call with nhlfEntriesArrayPtr->qosParams.dscp [64] out of range
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        nhlfEntriesArray.qosParams.dscp = 64;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nhlfEntriesArrayPtr->qosParams.dscp = %d",
                                         dev, nhlfEntriesArray.qosParams.dscp);

        nhlfEntriesArray.qosParams.dscp = 63;

        /*
            1.15. Call with nhlfEntriesArrayPtr->qosParams.exp [8] out of range
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        nhlfEntriesArray.qosParams.exp = 8;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nhlfEntriesArrayPtr->qosParams.exp = %d",
                                         dev, nhlfEntriesArray.qosParams.exp);

        nhlfEntriesArray.qosParams.exp = 7;

        /*
            1.16. Call with nhlfEntriesArrayPtr [NULL].
                      and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nhlfEntriesArrayPtr = NULL", dev);
    }

    nhlfEntryBaseIndex = 0;
    numOfNhlfEntries   = 1;

    nhlfEntriesArray.nhlfeCommonParams.mplsLabel = 0;
    nhlfEntriesArray.nhlfeCommonParams.mplsCmd   = CPSS_EXMXPM_MPLS_NOP_CMD_E;

    nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
    nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId = 100;
    nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr = 100;

    nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId = 100;
    nhlfEntriesArray.nhlfeCommonParams.ageRefresh = GT_FALSE;
    nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = 15;
    nhlfEntriesArray.nhlfeCommonParams.counterSetIndex = CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E;
    nhlfEntriesArray.nhlfeCommonParams.mirrorToIngressAnalyzer = GT_FALSE;
    nhlfEntriesArray.nhlfeCommonParams.ttl = 0;
    nhlfEntriesArray.nhlfeCommonParams.ttlMode = CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E;
    nhlfEntriesArray.nhlfeCommonParams.bypassTtlExceptionCheckEnable = GT_FALSE;
    nhlfEntriesArray.nhlfeCommonParams.cpuCodeIndex = 0;
    nhlfEntriesArray.nhlfeCommonParams.packetCmd = CPSS_PACKET_CMD_DROP_HARD_E;

    nhlfEntriesArray.qosParamsModify.modifyTc   = GT_FALSE;
    nhlfEntriesArray.qosParamsModify.modifyUp   = GT_FALSE;
    nhlfEntriesArray.qosParamsModify.modifyDp   = GT_FALSE;
    nhlfEntriesArray.qosParamsModify.modifyDscp = GT_FALSE;
    nhlfEntriesArray.qosParamsModify.modifyExp  = GT_FALSE;

    nhlfEntriesArray.qosParams.tc   = 0;
    nhlfEntriesArray.qosParams.dp   = CPSS_DP_GREEN_E;
    nhlfEntriesArray.qosParams.up   = 0;
    nhlfEntriesArray.qosParams.dscp = 0;
    nhlfEntriesArray.qosParams.exp  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsNhlfEntriesRead
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              nhlfEntryBaseIndex,
    IN  GT_U32                              numOfNhlfEntries,
    OUT CPSS_EXMXPM_MPLS_NHLFE_STC          *nhlfEntriesArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsNhlfEntriesRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmMplsNhlfEntriesWrite with
                   nhlfEntryBaseIndex[0],
                   numOfNhlfEntries[0],
                   nhlfEntriesArrayPtr{nhlfeCommonParams{
                                                         mplsLabel[0],
                                                         mplsCmd[CPSS_EXMXPM_MPLS_NOP_CMD_E],
                                                         outlifConfig{
                                                                        outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                        interfaceInfo{
                                                                                        type[CPSS_INTERFACE_TRUNK_E],
                                                                                        trunkId[100]},
                                                                        outlifPointer{arpPtr [100] }
                                                                      },
                                                         nextHopVlanId[100],
                                                         ageRefresh[GT_FALSE],
                                                         mtuProfileIndex[0],
                                                         counterSetIndex[CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E],
                                                         mirrorToIngressAnalyzer[GT_FALSE],
                                                         ttl[0],
                                                         ttlMode[CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E],
                                                         bypassTtlExceptionCheckEnable[GT_FALSE],
                                                         cpuCodeIndex[0],
                                                         packetCmd[CPSS_CMD_DROP_HARD_E],}
                                       qosParamsModify{
                                                        modifyTc[GT_FALSE],
                                                        modifyUp[GT_FALSE],
                                                        modifyDp[GT_FALSE],
                                                        modifyDscp[GT_FALSE],
                                                        modifyExp[GT_FALSE] }
                                       qosParams{
                                                tc[0],
                                                dp[CPSS_DP_GREEN_E],
                                                up[0],
                                                dscp[0],
                                                exp[0] } }
    Expected: GT_OK.
    1.2. Call with nhlfEntryBaseIndex[0],
                   numOfNhlfEntries[0],
                   not NULL nhlfEntriesArrayPtr.
    Expected: GT_OK.
    1.3. Call with nhlfEntriesArrayPtr[NULL] and other params from 1.2.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                              nhlfEntryBaseIndex;
    GT_U32                              numOfNhlfEntries;
    CPSS_EXMXPM_MPLS_NHLFE_STC          nhlfEntriesArray;


    numOfNhlfEntries   = 0;
    nhlfEntryBaseIndex = 0;

    cpssOsBzero((GT_VOID*) &nhlfEntriesArray, sizeof(nhlfEntriesArray));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
               1.1. Call cpssExMxPmMplsNhlfEntriesWrite with
                           nhlfEntryBaseIndex[0],
                           numOfNhlfEntries[0],
                           nhlfEntriesArrayPtr{nhlfeCommonParams{
                                                                 mplsLabel[0],
                                                                 mplsCmd[CPSS_EXMXPM_MPLS_NOP_CMD_E],
                                                                 outlifConfig{
                                                                                outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                                interfaceInfo{
                                                                                                type[CPSS_INTERFACE_TRUNK_E],
                                                                                                trunkId[100]},
                                                                                outlifPointer{arpPtr [100] }
                                                                              },
                                                                 nextHopVlanId[100],
                                                                 ageRefresh[GT_FALSE],
                                                                 mtuProfileIndex[0],
                                                                 counterSetIndex[CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E],
                                                                 mirrorToIngressAnalyzer[GT_FALSE],
                                                                 ttl[0],
                                                                 ttlMode[CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E],
                                                                 bypassTtlExceptionCheckEnable[GT_FALSE],
                                                                 cpuCodeIndex[0],
                                                                 packetCmd[CPSS_CMD_DROP_HARD_E],}
                                               qosParamsModify{
                                                                modifyTc[GT_FALSE],
                                                                modifyUp[GT_FALSE],
                                                                modifyDp[GT_FALSE],
                                                                modifyDscp[GT_FALSE],
                                                                modifyExp[GT_FALSE] }
                                               qosParams{
                                                        tc[0],
                                                        dp[CPSS_DP_GREEN_E],
                                                        up[0],
                                                        dscp[0],
                                                        exp[0] } }
            Expected: GT_OK.
        */
        nhlfEntryBaseIndex = 0;
        numOfNhlfEntries   = 1;

        nhlfEntriesArray.nhlfeCommonParams.mplsLabel = 0;
        nhlfEntriesArray.nhlfeCommonParams.mplsCmd   = CPSS_EXMXPM_MPLS_NOP_CMD_E;

        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId = 100;
        nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr = 100;

        nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId = 100;
        nhlfEntriesArray.nhlfeCommonParams.ageRefresh = GT_FALSE;
        nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = 15;
        nhlfEntriesArray.nhlfeCommonParams.counterSetIndex = CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E;
        nhlfEntriesArray.nhlfeCommonParams.mirrorToIngressAnalyzer = GT_FALSE;
        nhlfEntriesArray.nhlfeCommonParams.ttl = 0;
        nhlfEntriesArray.nhlfeCommonParams.ttlMode = CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E;
        nhlfEntriesArray.nhlfeCommonParams.bypassTtlExceptionCheckEnable = GT_FALSE;
        nhlfEntriesArray.nhlfeCommonParams.cpuCodeIndex = 0;
        nhlfEntriesArray.nhlfeCommonParams.packetCmd = CPSS_PACKET_CMD_DROP_HARD_E;

        nhlfEntriesArray.qosParamsModify.modifyTc   = GT_FALSE;
        nhlfEntriesArray.qosParamsModify.modifyUp   = GT_FALSE;
        nhlfEntriesArray.qosParamsModify.modifyDp   = GT_FALSE;
        nhlfEntriesArray.qosParamsModify.modifyDscp = GT_FALSE;
        nhlfEntriesArray.qosParamsModify.modifyExp  = GT_FALSE;

        nhlfEntriesArray.qosParams.tc   = 0;
        nhlfEntriesArray.qosParams.dp   = CPSS_DP_GREEN_E;
        nhlfEntriesArray.qosParams.up   = 0;
        nhlfEntriesArray.qosParams.dscp = 0;
        nhlfEntriesArray.qosParams.exp  = 0;

        st = cpssExMxPmMplsNhlfEntriesWrite(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, nhlfEntryBaseIndex, numOfNhlfEntries);

        /*
            1.2. Call with nhlfEntryBaseIndex[0],
                           numOfNhlfEntries[0],
                           not NULL nhlfEntriesArrayPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmMplsNhlfEntriesRead(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, nhlfEntryBaseIndex, numOfNhlfEntries);

        /*
            1.3. Call with nhlfEntriesArrayPtr[NULL] and other params from 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsNhlfEntriesRead(dev, nhlfEntryBaseIndex, numOfNhlfEntries, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nhlfEntriesArrayPtr = NULL", dev);
    }

    numOfNhlfEntries   = 1;
    nhlfEntryBaseIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsNhlfEntriesRead(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsNhlfEntriesRead(dev, nhlfEntryBaseIndex, numOfNhlfEntries, &nhlfEntriesArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsExceptionCmdSet
(
    IN GT_U8                                devNum,
    IN CPSS_EXMXPM_MPLS_EXCEPTION_TYPE_ENT  exceptionType,
    IN CPSS_PACKET_CMD_ENT                  exceptionCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsExceptionCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with exceptionType[CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E /
                                 CPSS_EXMXPM_MPLS_OUTGOING_TTL_EXCP_E],
                   exceptionCmd[CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                CPSS_PACKET_CMD_DROP_HARD_E],
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsExceptionCmdGet with not NULL exceptionCmdPtr and other param from 1.1.
    Expectdd: GT_OK and the same exceptionCmd as was set. (for 1.1. and 1.3.)
    1.3. Call with exceptionType[CPSS_EXMXPM_MPLS_MTU_EXCP_E],
                   exceptionCmd[CPSS_PACKET_CMD_ROUTE_E],
    Expected: GT_OK.
    1.4. Call with exceptionType[CPSS_EXMXPM_MPLS_MTU_EXCP_E],
                   exceptionCmd[CPSS_PACKET_CMD_BRIDGE_E], not supported
    Expected: NOT GT_OK.

    1.5. Call with exceptionType[MPLS_INVALID_ENUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with exceptionCmd[MPLS_INVALID_ENUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MPLS_EXCEPTION_TYPE_ENT  exceptionType;
    CPSS_PACKET_CMD_ENT                  exceptionCmd;
    CPSS_PACKET_CMD_ENT                  exceptionCmdGet;


    exceptionType   = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;
    exceptionCmd    = CPSS_PACKET_CMD_BRIDGE_E;
    exceptionCmdGet = CPSS_PACKET_CMD_BRIDGE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exceptionType[CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E /
                                         CPSS_EXMXPM_MPLS_OUTGOING_TTL_EXCP_E],
                           exceptionCmd[CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                        CPSS_PACKET_CMD_DROP_HARD_E],
            Expected: GT_OK.
        */
        /* iterate with exceptionType = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E */
        exceptionType = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;
        exceptionCmd  = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmMplsExceptionCmdSet(dev, exceptionType, exceptionCmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, exceptionCmd);

        /*
            1.2. Call cpssExMxPmMplsExceptionCmdGet with not NULL exceptionCmdPtr and other param from 1.1.
            Expectdd: GT_OK and the same exceptionCmd as was set. (for 1.1. and 1.3.)
        */
        st = cpssExMxPmMplsExceptionCmdGet(dev, exceptionType, &exceptionCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmMplsExceptionCmdGet: %d, %d", dev, exceptionType);
        UTF_VERIFY_EQUAL1_STRING_MAC(exceptionCmd, exceptionCmdGet, "got another exceptionCmd than was set: %d", dev);

        /* iterate with exceptionType = CPSS_EXMXPM_MPLS_OUTGOING_TTL_EXCP_E */
        exceptionType = CPSS_EXMXPM_MPLS_OUTGOING_TTL_EXCP_E;
        exceptionCmd  = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmMplsExceptionCmdSet(dev, exceptionType, exceptionCmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, exceptionCmd);

        /*
            1.2. Call cpssExMxPmMplsExceptionCmdGet with not NULL exceptionCmdPtr and other param from 1.1.
            Expectdd: GT_OK and the same exceptionCmd as was set. (for 1.1. and 1.3.)
        */
        st = cpssExMxPmMplsExceptionCmdGet(dev, exceptionType, &exceptionCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmMplsExceptionCmdGet: %d, %d", dev, exceptionType);
        UTF_VERIFY_EQUAL1_STRING_MAC(exceptionCmd, exceptionCmdGet, "got another exceptionCmd than was set: %d", dev);

        /*
            1.3. Call with exceptionType[CPSS_EXMXPM_MPLS_MTU_EXCP_E],
                           exceptionCmd[CPSS_PACKET_CMD_ROUTE_E],
            Expected: GT_OK.
        */
        exceptionType = CPSS_EXMXPM_MPLS_MTU_EXCP_E;
        exceptionCmd  = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmMplsExceptionCmdSet(dev, exceptionType, exceptionCmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, exceptionCmd);

        /*
            1.2. Call cpssExMxPmMplsExceptionCmdGet with not NULL exceptionCmdPtr and other param from 1.1.
            Expectdd: GT_OK and the same exceptionCmd as was set. (for 1.1. and 1.3.)
        */
        st = cpssExMxPmMplsExceptionCmdGet(dev, exceptionType, &exceptionCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmMplsExceptionCmdGet: %d, %d", dev, exceptionType);
        UTF_VERIFY_EQUAL1_STRING_MAC(exceptionCmd, exceptionCmdGet, "got another exceptionCmd than was set: %d", dev);

        /*
            1.4. Call with exceptionType[CPSS_EXMXPM_MPLS_MTU_EXCP_E],
                           exceptionCmd[CPSS_PACKET_CMD_BRIDGE_E], not supported
            Expected: NOT GT_OK.
        */
        exceptionType = CPSS_EXMXPM_MPLS_MTU_EXCP_E;
        exceptionCmd  = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmMplsExceptionCmdSet(dev, exceptionType, exceptionCmd);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, exceptionCmd);

        exceptionType = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;
        exceptionCmd  = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /*
            1.5. Call with exceptionType[MPLS_INVALID_ENUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        exceptionType = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsExceptionCmdSet(dev, exceptionType, exceptionCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, exceptionType);

        exceptionType = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;

        /*
            1.6. Call with exceptionCmd[MPLS_INVALID_ENUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        exceptionCmd = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsExceptionCmdSet(dev, exceptionType, exceptionCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, exceptionCmd = %d", dev, exceptionCmd);
    }

    exceptionType = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;
    exceptionCmd  = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsExceptionCmdSet(dev, exceptionType, exceptionCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsExceptionCmdSet(dev, exceptionType, exceptionCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsExceptionCmdGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_EXMXPM_MPLS_EXCEPTION_TYPE_ENT  exceptionType,
    OUT CPSS_PACKET_CMD_ENT                  *exceptionCmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsExceptionCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with exceptionType[CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E],
                   not NULL exceptionCmdPtr,
    Expected: GT_OK.
    1.2. Call with exceptionType[0x5AAAAAA5],
                   and other param from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with exceptionCmdPtr[NULL],
                   and other param from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MPLS_EXCEPTION_TYPE_ENT  exceptionType;
    CPSS_PACKET_CMD_ENT                  exceptionCmd;


    exceptionType   = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;
    exceptionCmd    = CPSS_PACKET_CMD_BRIDGE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exceptionType[CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E],
                           not NULL exceptionCmdPtr,
            Expected: GT_OK.
        */
        exceptionType = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;

        st = cpssExMxPmMplsExceptionCmdGet(dev, exceptionType, &exceptionCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceptionType);

        /*
            1.2. Call with exceptionType[0x5AAAAAA5],
                           and other param from 1.1.
            Expected: GT_BAD_PARAM.
        */
        exceptionType = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsExceptionCmdGet(dev, exceptionType, &exceptionCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, exceptionType);

        exceptionType = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;

        /*
            1.3. Call with exceptionCmdPtr[NULL],
                           and other param from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsExceptionCmdGet(dev, exceptionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, exceptionCmd = NULL", dev);
    }

    exceptionType = CPSS_EXMXPM_MPLS_INCOMING_TTL_EXCP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsExceptionCmdGet(dev, exceptionType, &exceptionCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsExceptionCmdGet(dev, exceptionType, &exceptionCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsFastRerouteEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsFastRerouteEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmMplsFastRerouteEnableGet with not NULL enable.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssExMxPmMplsFastRerouteEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmMplsFastRerouteEnableGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmMplsFastRerouteEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable then was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssExMxPmMplsFastRerouteEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmMplsFastRerouteEnableGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmMplsFastRerouteEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmMplsEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable then was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsFastRerouteEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsFastRerouteEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsFastRerouteEnableGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsFastRerouteEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmMplsFastRerouteEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsFastRerouteEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsFastRerouteEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsFastRerouteEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsFastRerouteEntrySet
(
    IN   GT_U8                                  devNum,
    IN   GT_U32                                 index,
    IN   CPSS_EXMXPM_MPLS_FAST_REROUTE_STC      *frrEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsFastRerouteEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0],
                   frrEntryPtr { outLif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                          interfaceInfo{ type[CPSS_INTERFACE_TRUNK_E],
                                                         trunkId[100]},
                                          outlifPointer{arpPtr [100] } }
                                 mapCommand [CPSS_EXMXPM_MPLS_FRR_COMMAND_DONT_MAP_E] }
    Expected: GT_OK.
    1.2. Call with cpssExMxPmMplsFastRerouteEntryGet with not NULL frrEntryPtr and other params asin 1.1.
    Expected: GT_OK and the same params as was set (1.1. and 1.3.)
    1.3. Call with index [2047],
                   frrEntryPtr { outLif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_DIT_E],
                                          interfaceInfo{ type[CPSS_INTERFACE_VIDX_E],
                                                         vidx[100]},
                                          outlifPointer{ditPtr [100] } }
                                 mapCommand [CPSS_EXMXPM_MPLS_FRR_COMMAND_MAP_E] }
    Expected: GT_OK.
    1.4. Call with index [2048] and other params as in 1.3.
    Expected: NOT GT_OK.
    1.5. Call with frrEntryPtr { outLif { outlifType[0x5AAAAAA5] }} and other params as in 1.3.
    Expected: GT_BAD_PARAM.
    1.6. Call with frrEntryPtr { outLif { interfaceInfo{ type[0x5AAAAAA5] }}} and other params as in 1.3.
    Expected: GT_BAD_PARAM.
    1.7. Call with frrEntryPtr [NULL]and other params as in 1.3.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                                 index = 0;
    CPSS_EXMXPM_MPLS_FAST_REROUTE_STC      frrEntry;
    CPSS_EXMXPM_MPLS_FAST_REROUTE_STC      frrEntryGet;


    cpssOsBzero((GT_VOID*) &frrEntry, sizeof(frrEntry));
    cpssOsBzero((GT_VOID*) &frrEntryGet, sizeof(frrEntryGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0],
                           frrEntryPtr { outLif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                  interfaceInfo{ type[CPSS_INTERFACE_TRUNK_E],
                                                                 trunkId[100]},
                                                  outlifPointer{arpPtr [100] } }
                                         mapCommand [CPSS_EXMXPM_MPLS_FRR_COMMAND_DONT_MAP_E] }
            Expected: GT_OK.
        */
        index = 0;
        frrEntry.outLif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        frrEntry.outLif.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        frrEntry.outLif.interfaceInfo.trunkId = 100;
        frrEntry.outLif.outlifPointer.arpPtr = 100;
        frrEntry.mapCommand = CPSS_EXMXPM_MPLS_FRR_COMMAND_DONT_MAP_E;

        st = cpssExMxPmMplsFastRerouteEntrySet(dev, index, &frrEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with cpssExMxPmMplsFastRerouteEntryGet with not NULL frrEntryPtr and other params asin 1.1.
            Expected: GT_OK and the same params as was set (1.1. and 1.3.)
        */
        st = cpssExMxPmMplsFastRerouteEntryGet(dev, index, &frrEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmMplsFastRerouteEntryGet: %d, %d", dev, index);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.outLif.outlifType, frrEntryGet.outLif.outlifType,
                                     "got another frrEntry.outLif.outlifType thab was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.outLif.interfaceInfo.type, frrEntryGet.outLif.interfaceInfo.type,
                                     "got another frrEntry.outLif.interfaceInfo.type thab was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.outLif.interfaceInfo.trunkId, frrEntryGet.outLif.interfaceInfo.trunkId,
                                     "got another frrEntry.outLif.interfaceInfo.trunkId thab was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.outLif.outlifPointer.arpPtr, frrEntryGet.outLif.outlifPointer.arpPtr,
                                     "got another frrEntry.outLif.outlifPointer.arpPtr thab was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.mapCommand, frrEntryGet.mapCommand,
                                     "got another frrEntry.mapCommand thab was set: %d", dev);

        /*
            1.3. Call with index [2047],
                           frrEntryPtr { outLif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_DIT_E],
                                                  interfaceInfo{ type[CPSS_INTERFACE_VIDX_E],
                                                                 vidx[100]},
                                                  outlifPointer{ditPtr [100] } }
                                         mapCommand [CPSS_EXMXPM_MPLS_FRR_COMMAND_MAP_E] }
            Expected: GT_OK.
        */
        index = 2047;
        frrEntry.outLif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
        frrEntry.outLif.interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        frrEntry.outLif.interfaceInfo.fabricVidx = 100;
        frrEntry.outLif.outlifPointer.ditPtr = 100;
        frrEntry.mapCommand = CPSS_EXMXPM_MPLS_FRR_COMMAND_MAP_E;

        st = cpssExMxPmMplsFastRerouteEntrySet(dev, index, &frrEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with cpssExMxPmMplsFastRerouteEntryGet with not NULL frrEntryPtr and other params asin 1.1.
            Expected: GT_OK and the same params as was set (1.1. and 1.3.)
        */
        st = cpssExMxPmMplsFastRerouteEntryGet(dev, index, &frrEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmMplsFastRerouteEntryGet: %d, %d", dev, index);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.outLif.outlifType, frrEntryGet.outLif.outlifType,
                                     "got another frrEntry.outLif.outlifType thab was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.outLif.interfaceInfo.type, frrEntryGet.outLif.interfaceInfo.type,
                                     "got another frrEntry.outLif.interfaceInfo.type thab was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.outLif.interfaceInfo.vidx, frrEntryGet.outLif.interfaceInfo.vidx,
                                     "got another frrEntry.outLif.interfaceInfo.vidx thab was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.outLif.outlifPointer.ditPtr, frrEntryGet.outLif.outlifPointer.ditPtr,
                                     "got another frrEntry.outLif.outlifPointer.ditPtr thab was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(frrEntry.mapCommand, frrEntryGet.mapCommand,
                                     "got another frrEntry.mapCommand thab was set: %d", dev);

        /*
            1.4. Call with index [2048] and other params as in 1.3.
            Expected: NOT GT_OK.
        */
        index = 2048;

        st = cpssExMxPmMplsFastRerouteEntrySet(dev, index, &frrEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 2047;

        /*
            1.5. Call with frrEntryPtr { outLif { outlifType[0x5AAAAAA5] }} and other params as in 1.3.
            Expected: GT_BAD_PARAM.
        */
        frrEntry.outLif.outlifType = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsFastRerouteEntrySet(dev, index, &frrEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, frrEntryPtr->outLif.outlifType = %d", dev, frrEntry.outLif.outlifType);

        frrEntry.outLif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;

        /*
            1.6. Call with frrEntryPtr { outLif { interfaceInfo{ type[0x5AAAAAA5] }}} and other params as in 1.3.
            Expected: GT_BAD_PARAM.
        */
        frrEntry.outLif.interfaceInfo.type = MPLS_INVALID_ENUM_CNS;

        st = cpssExMxPmMplsFastRerouteEntrySet(dev, index, &frrEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, frrEntryPtr->outLif.interfaceInfo.type = %d", dev, frrEntry.outLif.interfaceInfo.type);

        frrEntry.outLif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.7. Call with frrEntryPtr [NULL]and other params as in 1.3.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsFastRerouteEntrySet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, frrEntryPtr = NULL", dev);
    }

    index = 0;
    frrEntry.outLif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    frrEntry.outLif.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
    frrEntry.outLif.interfaceInfo.trunkId = 100;
    frrEntry.outLif.outlifPointer.arpPtr = 100;
    frrEntry.mapCommand = CPSS_EXMXPM_MPLS_FRR_COMMAND_DONT_MAP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsFastRerouteEntrySet(dev, index, &frrEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsFastRerouteEntrySet(dev, index, &frrEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMplsFastRerouteEntryGet
(
    IN   GT_U8                                  devNum,
    IN   GT_U32                                 index,
    OUT  CPSS_EXMXPM_MPLS_FAST_REROUTE_STC      *frrEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsFastRerouteEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 2047] and not NULL frrEntryPtr.
    expected: GT_OK.
    1.2. Call with index [2048]
    Expected: NOT GT_OK
    1.3. Call with frrEntryPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                                 index = 0;
    CPSS_EXMXPM_MPLS_FAST_REROUTE_STC      frrEntry;

    cpssOsBzero((GT_VOID*) &frrEntry, sizeof(frrEntry));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 2047] and not NULL frrEntryPtr.
            expected: GT_OK.
        */
        /* iterate with index = 0 */
        index = 0;

        st = cpssExMxPmMplsFastRerouteEntryGet(dev, index, &frrEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* iterate with index = 2047 */
        index = 2047;

        st = cpssExMxPmMplsFastRerouteEntryGet(dev, index, &frrEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with index [2048]
            Expected: NOT GT_OK
        */
        index = 2048;

        st = cpssExMxPmMplsFastRerouteEntryGet(dev, index, &frrEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with frrEntryPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMplsFastRerouteEntryGet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, frrEntryPtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMplsFastRerouteEntryGet(dev, index, &frrEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMplsFastRerouteEntryGet(dev, index, &frrEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill NHLF table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmMplsFillNhlfTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in NHLF table.
         Call cpssExMxPmMplsNhlfEntriesWrite with nhlfEntryBaseIndex[0..numEntries - 1],
                                               numOfNhlfEntries[1],
                                               nhlfEntriesArrayPtr{nhlfeCommonParams{
                                                                                     mplsLabel[0],
                                                                                     mplsCmd[CPSS_EXMXPM_MPLS_NOP_CMD_E],
                                                                                     outlifConfig{
                                                                                                    outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                                                    interfaceInfo{
                                                                                                                    type[CPSS_INTERFACE_TRUNK_E],
                                                                                                                    trunkId[100]},
                                                                                                    outlifPointer{arpPtr [100] }
                                                                                                  },
                                                                                     nextHopVlanId[100],
                                                                                     ageRefresh[GT_FALSE],
                                                                                     mtuProfileIndex[0],
                                                                                     counterSetIndex[CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E],
                                                                                     mirrorToIngressAnalyzer[GT_FALSE],
                                                                                     ttl[0],
                                                                                     ttlMode[CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E],
                                                                                     bypassTtlExceptionCheckEnable[GT_FALSE],
                                                                                     cpuCodeIndex[0],
                                                                                     packetCmd[CPSS_CMD_DROP_HARD_E],
                                                                                    }
                                                                   qosParamsModify{
                                                                                    modifyTc[GT_FALSE],
                                                                                    modifyUp[GT_FALSE],
                                                                                    modifyDp[GT_FALSE],
                                                                                    modifyDscp[GT_FALSE],
                                                                                    modifyExp[GT_FALSE] }
                                                                   qosParams{
                                                                            tc[0],
                                                                            dp[CPSS_DP_GREEN_E],
                                                                            up[0],
                                                                            dscp[0],
                                                                            exp[0] } }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmMplsNhlfEntriesWrite with nhlfEntryBaseIndex[numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in NHLF table and compare with original.
         Call cpssExMxPmMplsNhlfEntriesRead with not NULL nhlfEntriesArrayPtr and other params from 1.2.
    Expected: GT_OK and the same nhlfEntriesArray as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmMplsNhlfEntriesRead with nhlfEntryBaseIndex[numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries       = 0;
    GT_U32      iTemp            = 0;
    GT_U32      numOfNhlfEntries = 0;

    CPSS_EXMXPM_MPLS_NHLFE_STC  nhlfEntriesArray;
    CPSS_EXMXPM_MPLS_NHLFE_STC  nhlfEntriesArrayGet;


    cpssOsBzero((GT_VOID*) &nhlfEntriesArray, sizeof(nhlfEntriesArray));
    cpssOsBzero((GT_VOID*) &nhlfEntriesArrayGet, sizeof(nhlfEntriesArrayGet));

    /* Fill the entry for NHLF table */
    numOfNhlfEntries = 1;

    nhlfEntriesArray.nhlfeCommonParams.mplsLabel = 0;
    nhlfEntriesArray.nhlfeCommonParams.mplsCmd   = CPSS_EXMXPM_MPLS_SWAP_CMD_E;

    nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
    nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId = 2;
    nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr = 0;

    nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId = 100;
    nhlfEntriesArray.nhlfeCommonParams.ageRefresh = GT_FALSE;
    nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = 15;
    nhlfEntriesArray.nhlfeCommonParams.counterSetIndex = CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_0_E;
    nhlfEntriesArray.nhlfeCommonParams.mirrorToIngressAnalyzer = GT_FALSE;
    nhlfEntriesArray.nhlfeCommonParams.ttl = 0;
    nhlfEntriesArray.nhlfeCommonParams.ttlMode = CPSS_EXMXPM_MPLS_TTL_NOT_DECREMENTED_E;
    nhlfEntriesArray.nhlfeCommonParams.bypassTtlExceptionCheckEnable = GT_FALSE;
    nhlfEntriesArray.nhlfeCommonParams.cpuCodeIndex = 0;
    nhlfEntriesArray.nhlfeCommonParams.packetCmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

    nhlfEntriesArray.qosParamsModify.modifyTc   = GT_FALSE;
    nhlfEntriesArray.qosParamsModify.modifyUp   = GT_FALSE;
    nhlfEntriesArray.qosParamsModify.modifyDp   = GT_FALSE;
    nhlfEntriesArray.qosParamsModify.modifyDscp = GT_FALSE;
    nhlfEntriesArray.qosParamsModify.modifyExp  = GT_FALSE;

    nhlfEntriesArray.qosParams.tc   = 0;
    nhlfEntriesArray.qosParams.dp   = CPSS_DP_GREEN_E;
    nhlfEntriesArray.qosParams.up   = 0;
    nhlfEntriesArray.qosParams.dscp = 0;
    nhlfEntriesArray.qosParams.exp  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_DEV_MODULE_CFG_MAC(dev)->mplsCfg.nhlfeNum;

        /* 1.2. Fill all entries in NHLF table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = iTemp % 15;
            nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId   = (GT_U16)(iTemp % 4095);
            nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId = (GT_U16)(iTemp % 255);

            st = cpssExMxPmMplsNhlfEntriesWrite(dev, iTemp, numOfNhlfEntries, &nhlfEntriesArray);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmMplsNhlfEntriesWrite: %d, %d, %d", dev, iTemp, numOfNhlfEntries);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmMplsNhlfEntriesWrite(dev, numEntries, numOfNhlfEntries, &nhlfEntriesArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmMplsNhlfEntriesWrite: %d, %d, %d", dev, iTemp, numOfNhlfEntries);

        /* 1.4. Read all entries in NHLF table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex = iTemp % 15;
            nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId   = (GT_U16)(iTemp % 4095);
            nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId = (GT_U16)(iTemp % 255);

            st = cpssExMxPmMplsNhlfEntriesRead(dev, iTemp, numOfNhlfEntries, &nhlfEntriesArrayGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmMplsNhlfEntriesRead: %d, %d, %d",
                                        dev, iTemp, numOfNhlfEntries);

           /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mplsLabel, nhlfEntriesArrayGet.nhlfeCommonParams.mplsLabel,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mplsLabel than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mplsCmd, nhlfEntriesArrayGet.nhlfeCommonParams.mplsCmd,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mplsCmd than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifType, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.outlifType,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.type, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.interfaceInfo.type,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr, nhlfEntriesArrayGet.nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.nextHopVlanId, nhlfEntriesArrayGet.nhlfeCommonParams.nextHopVlanId,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.nextHopVlanId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ageRefresh, nhlfEntriesArrayGet.nhlfeCommonParams.ageRefresh,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ageRefresh than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mtuProfileIndex, nhlfEntriesArrayGet.nhlfeCommonParams.mtuProfileIndex,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mtuProfileIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.counterSetIndex, nhlfEntriesArrayGet.nhlfeCommonParams.counterSetIndex,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.counterSetIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.mirrorToIngressAnalyzer, nhlfEntriesArrayGet.nhlfeCommonParams.mirrorToIngressAnalyzer,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.mirrorToIngressAnalyzer than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ttl, nhlfEntriesArrayGet.nhlfeCommonParams.ttl,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ttl than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.ttlMode, nhlfEntriesArrayGet.nhlfeCommonParams.ttlMode,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.ttlMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.bypassTtlExceptionCheckEnable, nhlfEntriesArrayGet.nhlfeCommonParams.bypassTtlExceptionCheckEnable,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.bypassTtlExceptionCheckEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.cpuCodeIndex, nhlfEntriesArrayGet.nhlfeCommonParams.cpuCodeIndex,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.cpuCodeIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.nhlfeCommonParams.packetCmd, nhlfEntriesArrayGet.nhlfeCommonParams.packetCmd,
                       "got another nhlfEntriesArrayPtr->nhlfeCommonParams.packetCmd than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyTc, nhlfEntriesArrayGet.qosParamsModify.modifyTc,
                       "got another nhlfEntriesArrayPtr->qosParamsModify.modifyTc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyUp, nhlfEntriesArrayGet.qosParamsModify.modifyUp,
                       "got another nhlfEntriesArrayPtr->qosParamsModify.modifyUp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyDp, nhlfEntriesArrayGet.qosParamsModify.modifyDp,
                       "got another nhlfEntriesArrayPtr->qosParamsModify.modifyDp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyDscp, nhlfEntriesArrayGet.qosParamsModify.modifyDscp,
                       "got another nhlfEntriesArrayPtr->qosParamsModify.modifyDscp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParamsModify.modifyExp, nhlfEntriesArrayGet.qosParamsModify.modifyExp,
                       "got another nhlfEntriesArrayPtr->qosParamsModify.modifyExp than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.tc, nhlfEntriesArrayGet.qosParams.tc,
                       "got another nhlfEntriesArrayPtr->qosParams.tc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.dp, nhlfEntriesArrayGet.qosParams.dp,
                       "got another nhlfEntriesArrayPtr->qosParams.dp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.up, nhlfEntriesArrayGet.qosParams.up,
                       "got another nhlfEntriesArrayPtr->qosParams.up than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.dscp, nhlfEntriesArrayGet.qosParams.dscp,
                       "got another nhlfEntriesArrayPtr->qosParams.dscp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(nhlfEntriesArray.qosParams.exp, nhlfEntriesArrayGet.qosParams.exp,
                       "got another nhlfEntriesArrayPtr->qosParams.exp than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmMplsNhlfEntriesRead(dev, numEntries, numOfNhlfEntries, &nhlfEntriesArrayGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmMplsNhlfEntriesRead: %d, %d, %d",
                                         dev, iTemp, numOfNhlfEntries);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmMpls suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmMpls)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsLabelSpaceModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsLabelSpaceModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsInterfaceEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsInterfaceEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsInvalidEntryCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsInvalidEntryCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsSpecialRouterTriggerEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsSpecialRouterTriggerEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsNhlfEntriesWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsNhlfEntriesRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsExceptionCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsExceptionCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsFastRerouteEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsFastRerouteEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsFastRerouteEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsFastRerouteEntryGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMplsFillNhlfTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmMpls)
