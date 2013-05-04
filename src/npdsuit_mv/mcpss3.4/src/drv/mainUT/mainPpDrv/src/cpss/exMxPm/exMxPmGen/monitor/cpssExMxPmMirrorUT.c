/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmMirrorUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmMirror, that provides
*       CPSS ExMxPm Mirror APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/monitor/cpssExMxPmMirror.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define MIRROR_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define MIRROR_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define MIRROR_TESTED_VLAN_ID_CNS  100


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorStatEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_EXMXPM_MIRROR_TYPE_ENT  mirrorType,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorStatEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E],
                   enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmMirrorStatEnableGet with the same mirrorType
                                                 and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with out of range mirrorType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    GT_BOOL                     enable     = GT_TRUE;
    GT_BOOL                     enableGet  = GT_TRUE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E],
                           enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
        enable     = GT_FALSE;

        st = cpssExMxPmMirrorStatEnableSet(dev, mirrorType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrorType, enable);

        /*
            1.2. Call cpssExMxPmMirrorStatEnableGet with the same mirrorType
                                                         and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorStatEnableGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;
        enable     = GT_TRUE;

        st = cpssExMxPmMirrorStatEnableSet(dev, mirrorType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrorType, enable);

        /*
            1.2. Call cpssExMxPmMirrorStatEnableGet with the same mirrorType
                                                         and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorStatEnableGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;

        st = cpssExMxPmMirrorStatEnableSet(dev, mirrorType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrorType, enable);

        /*
            1.2. Call cpssExMxPmMirrorStatEnableGet with the same mirrorType
                                                         and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorStatEnableGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /*
            1.3. Call with out of range mirrorType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorStatEnableSet(dev, mirrorType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorType);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    enable     = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorStatEnableSet(dev, mirrorType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorStatEnableSet(dev, mirrorType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorStatEnableGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorStatEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                   and non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range mirrorType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    GT_BOOL                     enable     = GT_TRUE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                           and non-null enablePtr.
            Expected: GT_OK.
        */

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);


        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;

        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call with out of range mirrorType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorType);

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        /*
            1.3. Call with enablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorStatEnableGet(dev, mirrorType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorStatRatioSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType,
    IN  GT_U32                      ratio
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorStatRatioSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E],
                   ratio [0 / 10 / 2047].
    Expected: GT_OK.
    1.2. Call cpssExMxPmMirrorStatRatioGet with the same mirrorType
                                                and non-null ratioPtr.
    Expected: GT_OK and the same ratio.
    1.3. Call with out of range mirrorType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range ratio [2048]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    GT_U32                      ratio      = 0;
    GT_U32                      ratioGet   = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E],
                           ratio [0 / 10 / 2047].
            Expected: GT_OK.
        */

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
        ratio      = 0;

        st = cpssExMxPmMirrorStatRatioSet(dev, mirrorType, ratio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrorType, ratio);

        /*
            1.2. Call cpssExMxPmMirrorStatRatioGet with the same mirrorType
                                                        and non-null ratioPtr.
            Expected: GT_OK and the same ratio.
        */
        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratioGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorStatRatioGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ratio, ratioGet,
                   "get another ratio than was set: %d", dev);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;
        ratio      = 10;

        st = cpssExMxPmMirrorStatRatioSet(dev, mirrorType, ratio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrorType, ratio);

        /*
            1.2. Call cpssExMxPmMirrorStatRatioGet with the same mirrorType
                                                        and non-NULL ratioPtr.
            Expected: GT_OK and the same ratio.
        */
        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratioGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorStatRatioGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ratio, ratioGet,
                   "get another ratio than was set: %d", dev);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;
        ratio      = 2047;

        st = cpssExMxPmMirrorStatRatioSet(dev, mirrorType, ratio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrorType, ratio);

        /*
            1.2. Call cpssExMxPmMirrorStatRatioGet with the same mirrorType
                                                        and non-NULL ratioPtr.
            Expected: GT_OK and the same ratio.
        */
        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratioGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorStatRatioGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ratio, ratioGet,
                   "get another ratio than was set: %d", dev);

        /*
            1.3. Call with out of range mirrorType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorStatRatioSet(dev, mirrorType, ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorType);

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        /*
            1.4. Call with out of range ratio [2048]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ratio = 2048;

        st = cpssExMxPmMirrorStatRatioSet(dev, mirrorType, ratio);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ratio = %d", dev, ratio);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    ratio      = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorStatRatioSet(dev, mirrorType, ratio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorStatRatioSet(dev, mirrorType, ratio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorStatRatioGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType,
    OUT GT_U32                      *ratioPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorStatRatioGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                   and non-null ratio.
    Expected: GT_OK.
    1.2. Call with out of range mirrorType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with ratioPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    GT_U32                      ratio      = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                           and non-null ratioPtr.
            Expected: GT_OK.
        */

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;

        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call with out of range mirrorType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorType);

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        /*
            1.3. Call with ratioPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ratioPtr = NULL", dev);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorStatRatioGet(dev, mirrorType, &ratio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorPortEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       mirrPort,
    IN  CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorPortEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                 CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E],
                     enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmMirrorPortEnableGet with the same mirrorType
                                                   and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.1.3. Call with out of range mirrorType [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] (not supported)
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] (CPU port is NOT supported)
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4.2. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] (CPU port is supported)
                     and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = MIRROR_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    GT_BOOL                     enable     = GT_FALSE;
    GT_BOOL                     enableGet  = GT_FALSE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                             CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E],
                                 enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with mirrorType[CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] */
            mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
            enable     = GT_FALSE;

            st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, mirrorType, enable);

            /*
                1.1.2. Call cpssExMxPmMirrorPortEnableGet with the same mirrorType
                                                               and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmMirrorPortEnableGet: %d, %d, %d", dev, port, mirrorType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with mirrorType[CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] */
            mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;
            enable     = GT_TRUE;

            st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, mirrorType, enable);

            /*
                1.1.2. Call cpssExMxPmMirrorPortEnableGet with the same mirrorType
                                                               and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmMirrorPortEnableGet: %d, %d, %d", dev, port, mirrorType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range mirrorType [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            mirrorType = MIRROR_INVALID_ENUM_CNS;

            st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mirrorType);

            /*
                1.1.4. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] (not supported)
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;

            st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mirrorType);
        }

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;
        enable     = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /*
            1.4.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] (CPU port is NOT supported)
                             and other parameters from 1.1.
            Expected: GT_OK.
        */
        port = CPSS_CPU_PORT_NUM_CNS;

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mirrorType);

        /*
            1.4.2. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] (CPU port is supported)
                             and other parameters from 1.1.
            Expected: GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

        st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mirrorType);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;
    enable     = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = MIRROR_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorPortEnableSet(dev, port, mirrorType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorPortEnableGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       mirrPort,
    IN  CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorPortEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                 CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E]
                     and non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range mirrorType [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] (not supported)
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call with enablePtr [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] (CPU port is NOT supported)
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4.2. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] (CPU port is supported)
                     and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = MIRROR_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                             CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E]
                                 and non-null enablePtr.
                Expected: GT_OK.
            */

            /* Call with mirrorType[CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] */
            mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

            st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mirrorType);

            /* Call with mirrorType[CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] */
            mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

            st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mirrorType);

            /*
                1.1.2. Call with out of range mirrorType [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            mirrorType = MIRROR_INVALID_ENUM_CNS;

            st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mirrorType);

            /*
                1.1.3. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] (not supported)
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;

            st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mirrorType);

            mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

            /*
                1.1.4. Call with enablePtr [NULL] and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /*
            1.4.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] (CPU port is NOT supported)
                             and other parameters from 1.1.
            Expected: GT_OK.
        */
        port = CPSS_CPU_PORT_NUM_CNS;

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mirrorType);

        /*
            1.4.2. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] (CPU port is supported)
                             and other parameters from 1.1.
            Expected: GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

        st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mirrorType);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = MIRROR_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorPortEnableGet(dev, port, mirrorType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorDestInterfaceSet
(
    IN GT_U8                         devNum,
    IN CPSS_EXMXPM_MIRROR_TYPE_ENT   mirrorType,
    IN GT_U32                        pclAnalyzerNum,
    IN CPSS_EXMXPM_OUTLIF_INFO_STC   *interfaceInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorDestInterfaceSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E],
                   pclAnalyzerNum (not relevant),
                   and interfaceInfoPtr{outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                        interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                             devPort->devNum[dev],
                                                             devPort->portNum[0]}}.}
                                        outlifPointer{arpPtr[0]}
    Expected: GT_OK.
    1.2. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E],
                   pclAnalyzerNum (not relevant),
                   and interfaceInfoPtr {outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                         interfaceInfo {type [CPSS_INTERFACE_TRUNK_E],
                                                             trunkId[2]}}.
                                         outlifPointer{arpPtr[0]}
    Expected: GT_OK.
    1.3. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                   pclAnalyzerNum [3],
                   and interfaceInfoPtr {outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                         interfaceInfo {type [CPSS_INTERFACE_VID_E],
                                                        vlanId[100]}}.
                                         outlifPointer{arpPtr[0]}
    Expected: GT_OK.
    1.4. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E]
                   pclAnalyzerNum (not relevant),
                   and valid interfaceInfoPtr {outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                               interfaceInfo {type [CPSS_INTERFACE_PORT_E],
                                                                devNum[dev],
                                                                portNum[0]}}.
                                               outlifPointer{arpPtr[0]}
    Expected: GT_OK.
    1.5. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E]
                   pclAnalyzerNum (not relevant),
                   and valid interfaceInfoPtr {outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                               interfaceInfo {type [CPSS_INTERFACE_PORT_E],
                                                                devNum[dev],
                                                                portNum[0]}}.
                                               outlifPointer{arpPtr[0]}
    Expected: GT_OK.
    1.6. Call cpssExMxPmMirrorDestInterfaceGet with the same mirrorType as in 1.1., 1.2., 1.3., 1.4., 1.5.
                                                    and non-null interfaceInfoPtr.
    Expected: GT_OK and the same interfaceInfoPtr as in 1.1., 1.2., 1.3., 1.4., 1.5.
    1.7. Call with out of range mirrorType [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range interfaceInfoPtr->type [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with out of range interfaceInfoPtr->outlifType.type [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with interfaceInfoPtr->interfaceInfo.outlifType [CPSS_INTERFACE_VIDX_E] (not supported)
                    and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.11. Call with out of range interfaceInfoPtr->interfaceInfo.type [0x5AAAAAA5]
                    and other parameters the same as in 1.5.
    Expected: GT_BAD_PARAM.
    1.12. Call with out of range interfaceInfoPtr->interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.13. Call with out of range interfaceInfoPtr->interfaceInfo.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.14. Call with out of range interfaceInfoPtr->interfaceInfo.trunkId [256]
                    and other parameters the same as in 1.2.
    Expected: NOT GT_OK.
    1.15. Call with out of range interfaceInfoPtr->interfaceInfo.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                    and other parameters the same as in 1.3.
    Expected: GT_OUT_OF_RANGE.
    1.16. Call with out of range interfaceInfoPtr->interfaceInfo.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.17. Call with out of range interfaceInfoPtr->interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other parameters the same as in 1.5.
    Expected: NOT GT_OK.
    1.18. Call with out of range interfaceInfoPtr->interfaceInfo.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other parameters the same as in 1.5.
    Expected: NOT GT_OK.
    1.19 Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E],
                   pclAnalyzerNum [0 / 8] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.20 Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E],
                   pclAnalyzerNum [0 / 8] (not relevant) and other params from 1.1.
    Expected: GT_OK.
    1.21. Call with interfaceInfoPtr [NULL]
                    and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_TYPE_ENT   mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    CPSS_EXMXPM_OUTLIF_INFO_STC   interfaceInfo;
    CPSS_EXMXPM_OUTLIF_INFO_STC   interfaceInfoGet;
    GT_U32                        pclAnalyzerNum = 0;
    GT_U32                        pclAnalyzerNumGet = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E]
                           pclAnalyzerNum (not relevant),
                           and interfaceInfoPtr{outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                                interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                                     devPort->devNum[dev],
                                                                     devPort->portNum[0]}}.}
                                                outlifPointer{arpPtr[0]}
            Expected: GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
        interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        interfaceInfo.outlifPointer.arpPtr          = 0;
        interfaceInfo.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
        interfaceInfo.interfaceInfo.devPort.devNum  = 0;
        interfaceInfo.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.6. Call cpssExMxPmMirrorDestInterfaceGet with the same mirrorType as in 1.1., 1.2., 1.3., 1.4., 1.5.
                                                            and non-null interfaceInfoPtr.
            Expected: GT_OK and the same interfaceInfoPtr as in 1.1., 1.2., 1.3., 1.4., 1.5.
        */
        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNumGet, &interfaceInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorDestInterfaceGet: %d, %d", dev, mirrorType);

        /* Verifying struct values */
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.outlifType, interfaceInfoGet.outlifType,
                   "get another interfaceInfoPtr->outlifType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.type, interfaceInfoGet.interfaceInfo.type,
                   "get another interfaceInfoPtr->interfaceInfo.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.devPort.devNum, interfaceInfoGet.interfaceInfo.devPort.devNum,
                   "get another interfaceInfoPtr->interfaceInfo.devPort.devNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.devPort.portNum, interfaceInfoGet.interfaceInfo.devPort.portNum,
                   "get another interfaceInfoPtr->interfaceInfo.devPort.portNum than was set: %d", dev);

        /*
            1.2. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E],
                           pclAnalyzerNum (not relevant),
                           and interfaceInfoPtr {outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                 interfaceInfo {type [CPSS_INTERFACE_TRUNK_E],
                                                                     trunkId[2]}}.
                                                 outlifPointer{arpPtr[0]}
            Expected: GT_OK.
        */

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;
        interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        interfaceInfo.outlifPointer.arpPtr  = 0;
        interfaceInfo.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        interfaceInfo.interfaceInfo.trunkId = 2;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.6. Call cpssExMxPmMirrorDestInterfaceGet with the same mirrorType as in 1.1., 1.2., 1.3., 1.4., 1.5.
                                                            and non-null interfaceInfoPtr.
            Expected: GT_OK and the same interfaceInfoPtr as in 1.1., 1.2., 1.3., 1.4., 1.5.
        */
        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNumGet, &interfaceInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorDestInterfaceGet: %d, %d", dev, mirrorType);

        /* Verifying struct values */
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.outlifType, interfaceInfoGet.outlifType,
                   "get another interfaceInfoPtr->outlifType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.type, interfaceInfoGet.interfaceInfo.type,
                   "get another interfaceInfoPtr->interfaceInfo.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.trunkId, interfaceInfoGet.interfaceInfo.trunkId,
                   "get another interfaceInfoPtr->interfaceInfo.trunkId than was set: %d", dev);

        /*
            1.3. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                           pclAnalyzerNum [3],
                           and interfaceInfoPtr {outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                 interfaceInfo {type [CPSS_INTERFACE_VID_E],
                                                                vlanId[100]}}.
                                                 outlifPointer{arpPtr[0]}
            Expected: GT_OK.
        */

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;
        pclAnalyzerNum = 3;
        interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        interfaceInfo.outlifPointer.arpPtr = 0;
        interfaceInfo.interfaceInfo.type   = CPSS_INTERFACE_VID_E;
        interfaceInfo.interfaceInfo.vlanId = MIRROR_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.6. Call cpssExMxPmMirrorDestInterfaceGet with the same mirrorType as in 1.1., 1.2., 1.3., 1.4., 1.5.
                                                            and non-null interfaceInfoPtr.
            Expected: GT_OK and the same interfaceInfoPtr as in 1.1., 1.2., 1.3., 1.4., 1.5.
        */
        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorDestInterfaceGet: %d, %d", dev, mirrorType);

        /* Verifying struct values */
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.outlifType, interfaceInfoGet.outlifType,
                   "get another interfaceInfoPtr->outlifType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.type, interfaceInfoGet.interfaceInfo.type,
                   "get another interfaceInfoPtr->interfaceInfo.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.vlanId, interfaceInfoGet.interfaceInfo.vlanId,
                   "get another interfaceInfoPtr->interfaceInfo.vlanId than was set: %d", dev);

        /*
            1.4. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E]
                           pclAnalyzerNum (not relevant),
                           and valid interfaceInfoPtr {outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                       interfaceInfo {type [CPSS_INTERFACE_PORT_E],
                                                                        devNum[dev],
                                                                        portNum[0]}}.
                                                 outlifPointer{arpPtr[0]}
            Expected: GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
        interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        interfaceInfo.outlifPointer.arpPtr          = 0;
        interfaceInfo.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
        interfaceInfo.interfaceInfo.devPort.devNum  = 127;
        interfaceInfo.interfaceInfo.devPort.portNum = 8;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.6. Call cpssExMxPmMirrorDestInterfaceGet with the same mirrorType as in 1.1., 1.2., 1.3., 1.4., 1.5.
                                                                and non-null interfaceInfoPtr.
            Expected: GT_OK and the same interfaceInfoPtr as in 1.1., 1.2., 1.3., 1.4., 1.5.
        */
        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNumGet, &interfaceInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorDestInterfaceGet: %d, %d", dev, mirrorType);

        /* Verifying struct values */
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.outlifType, interfaceInfoGet.outlifType,
                   "get another interfaceInfoPtr->outlifType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.type, interfaceInfoGet.interfaceInfo.type,
                   "get another interfaceInfoPtr->interfaceInfo.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.devNum, interfaceInfoGet.interfaceInfo.devNum,
                   "get another interfaceInfoPtr->interfaceInfo.devNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.devPort.portNum, interfaceInfoGet.interfaceInfo.devPort.portNum,
                   "get another interfaceInfoPtr->interfaceInfo.devPort.portNum than was set: %d", dev);

        /*
            1.5. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E]
                           pclAnalyzerNum (not relevant),
                           and valid interfaceInfoPtr {outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                       interfaceInfo {type [CPSS_INTERFACE_PORT_E],
                                                                        devNum[dev],
                                                                        portNum[0]}}.
                                                 outlifPointer{ditPtr[0]}
            Expected: GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;
        interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        interfaceInfo.outlifPointer.arpPtr          = 0;
        interfaceInfo.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
        interfaceInfo.interfaceInfo.devPort.devNum  = 63;
        interfaceInfo.interfaceInfo.devPort.portNum = 18;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.6. Call cpssExMxPmMirrorDestInterfaceGet with the same mirrorType as in 1.1., 1.2., 1.3., 1.4., 1.5.
                                                                and non-null interfaceInfoPtr.
            Expected: GT_OK and the same interfaceInfoPtr as in 1.1., 1.2., 1.3., 1.4., 1.5.
        */
        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNumGet, &interfaceInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorDestInterfaceGet: %d, %d", dev, mirrorType);

        /* Verifying struct values */
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.outlifType, interfaceInfoGet.outlifType,
                   "get another interfaceInfoPtr->outlifType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.type, interfaceInfoGet.interfaceInfo.type,
                   "get another interfaceInfoPtr->interfaceInfo.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.devNum, interfaceInfoGet.interfaceInfo.devNum,
                   "get another interfaceInfoPtr->interfaceInfo.devNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interfaceInfo.interfaceInfo.devPort.portNum, interfaceInfoGet.interfaceInfo.devPort.portNum,
                   "get another interfaceInfoPtr->interfaceInfo.devPort.portNum than was set: %d", dev);

        /*
            1.7. Call with out of range mirrorType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorType);

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        /*
            1.8. Call with out of range interfaceInfoPtr->type [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        interfaceInfo.outlifType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, interfaceInfoPtr->type = %d",
                                     dev, interfaceInfo.outlifType);

        interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;

        /*
            1.9. Call with out of range interfaceInfoPtr->interfaceInfo.outlifType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        interfaceInfo.interfaceInfo.type = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, interfaceInfoPtr->interfaceInfo.outlifType = %d",
                                     dev, interfaceInfo.interfaceInfo.type);

        /*
            1.10. Call with interfaceInfoPtr->interfaceInfo.outlifType [CPSS_INTERFACE_VIDX_E] (not supported)
                            and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        interfaceInfo.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->interfaceInfo.outlifType = %d",
                                         dev, interfaceInfo.interfaceInfo.type);

        interfaceInfo.interfaceInfo.type = CPSS_INTERFACE_VID_E;

        /*
            1.11. Call with out of range interfaceInfoPtr->interfaceInfo.type [0x5AAAAAA5]
                            and other parameters the same as in 1.5.
            Expected: GT_BAD_PARAM.
        */

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

        interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;

        interfaceInfo.interfaceInfo.type = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, interfaceInfoPtr->interfaceInfo.type = %d",
                                     dev, interfaceInfo.interfaceInfo.type);

        interfaceInfo.interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
            1.12. Call with out of range interfaceInfoPtr->interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;

        interfaceInfo.interfaceInfo.type           = CPSS_INTERFACE_PORT_E;
        interfaceInfo.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->interfaceInfo.devPort.devNum = %d",
                                         dev, interfaceInfo.interfaceInfo.devPort.devNum);

        interfaceInfo.interfaceInfo.devPort.devNum = dev;

        /*
            1.13. Call with out of range interfaceInfoPtr->interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        interfaceInfo.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo->interfaceInfo.devPort.portNum = %d",
                                         dev, interfaceInfo.interfaceInfo.devPort.portNum);

        interfaceInfo.interfaceInfo.devPort.portNum = 0;

        /*
            1.14. Call with out of range interfaceInfoPtr->interfaceInfo.trunkId [256]
                            and other parameters the same as in 1.2.
            Expected: NOT GT_OK.
        */
        interfaceInfo.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        interfaceInfo.interfaceInfo.trunkId = 256;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->interfaceInfo.trunkId = %d",
                                         dev, interfaceInfo.interfaceInfo.trunkId);

        interfaceInfo.interfaceInfo.trunkId = 2;

        /*
            1.15. Call with out of range interfaceInfoPtr->interfaceInfo.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters the same as in 1.3.
            Expected: GT_OUT_OF_RANGE.
        */
        interfaceInfo.interfaceInfo.type   = CPSS_INTERFACE_VID_E;
        interfaceInfo.interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, interfaceInfo->interfaceInfo.vlanId = %d",
                                     dev, interfaceInfo.interfaceInfo.vlanId);

        /*
            1.16. Call with out of range interfaceInfoPtr->interfaceInfo.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        interfaceInfo.interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        interfaceInfo.interfaceInfo.vlanId = MIRROR_TESTED_VLAN_ID_CNS;

        /*
            1.17. Call with out of range interfaceInfoPtr->interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other parameters the same as in 1.5.
            Expected: NOT GT_OK.
        */
        interfaceInfo.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo->interfaceInfo.devPort.devNum = %d", dev,
                                         interfaceInfo.interfaceInfo.devPort.devNum);

        interfaceInfo.interfaceInfo.devPort.devNum = dev;

        /*
            1.18. Call with out of range interfaceInfoPtr->interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other parameters the same as in 1.5.
            Expected: NOT GT_OK.
        */
        interfaceInfo.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo->interfaceInfo.devPort.portNum = %d",
                                         dev, interfaceInfo.interfaceInfo.devPort.portNum);

        interfaceInfo.interfaceInfo.devPort.portNum = 0;

        /*
            1.19 Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E], pclAnalyzerNum [0 / 8] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;
        pclAnalyzerNum = 0;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pclAnalyzerNum = %d", dev, pclAnalyzerNum);

        pclAnalyzerNum = 8;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pclAnalyzerNum = %d", dev, pclAnalyzerNum);

        pclAnalyzerNum = 1;

        /*
            1.20 Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E],
                           pclAnalyzerNum [0 / 8] (not relevant) and other params from 1.1.
            Expected: GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
        pclAnalyzerNum = 0;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, pclAnalyzerNum = %d", dev, pclAnalyzerNum);

        pclAnalyzerNum = 8;

        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, pclAnalyzerNum = %d", dev, pclAnalyzerNum);

        pclAnalyzerNum = 1;

        /*
            1.21. Call with interfaceInfoPtr [NULL]
                            and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, interfaceInfoPtr = NULL", dev);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    pclAnalyzerNum = 1;
    interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    interfaceInfo.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    interfaceInfo.interfaceInfo.devPort.devNum  = dev;
    interfaceInfo.interfaceInfo.devPort.portNum = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorDestInterfaceSet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorDestInterfaceGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_EXMXPM_MIRROR_TYPE_ENT   mirrorType,
    IN  GT_U32                        pclAnalyzerNum,
    OUT CPSS_EXMXPM_OUTLIF_INFO_STC   *interfaceInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorDestInterfaceGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                   pclAnalyzerNum [1] (relevant just for CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E),
                   and non-null interfaceInfoPtr.
    Expected: GT_OK.
    1.2. Call with out of range mirrorType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E],
                   pclAnalyzerNum [0 / 8] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.5 Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E],
                   pclAnalyzerNum [0 / 8] (not relevant) and other params from 1.1.
    Expected: GT_OK.
    1.6. Call with interfaceInfoPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    CPSS_EXMXPM_OUTLIF_INFO_STC interfaceInfo;
    GT_U32                      pclAnalyzerNum = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                           and non-null interfaceInfoPtr.
            Expected: GT_OK.
        */

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;
        pclAnalyzerNum = 1;

        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call with out of range mirrorType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorType);

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
        /*
            1.4. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E], pclAnalyzerNum [0 / 8] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;
        pclAnalyzerNum = 0;

        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pclAnalyzerNum = %d", dev, pclAnalyzerNum);

        pclAnalyzerNum = 8;

        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pclAnalyzerNum = %d", dev, pclAnalyzerNum);

        pclAnalyzerNum = 1;

        /*
            1.5 Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E],
                           pclAnalyzerNum [0 / 8] (not relevant) and other params from 1.1.
            Expected: GT_OK.
        */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
        pclAnalyzerNum = 0;

        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, pclAnalyzerNum = %d", dev, pclAnalyzerNum);

        pclAnalyzerNum = 8;

        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, pclAnalyzerNum = %d", dev, pclAnalyzerNum);

        pclAnalyzerNum = 1;

        /*
            1.6. Call with interfaceInfoPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d,  interfaceInfoPtr = NULL", dev);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    pclAnalyzerNum = 1;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorDestInterfaceGet(dev, mirrorType, pclAnalyzerNum, &interfaceInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorQosSet
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_MIRROR_QOS_TYPE_ENT  mirrorQosType,
    IN CPSS_EXMXPM_MIRROR_QOS_STC       *monitorQosConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorQosSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mirrorQosType [CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_E /
                                  CPSS_EXMXPM_MIRROR_QOS_TYPE_EGRESS_E /
                                  CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_POLICY_E]
                   and monitorQosConfigPtr {qosEnable [GT_FALSE / GT_TRUE],
                                            tc [0 / 3 / 7],
                                            dp [CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E / CPSS_DP_RED_E]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmMirrorQosGet with the same mirrorType
                                          and non-null monitorQosConfigPtr.
    Expected: GT_OK and the same monitorQosConfigPtr.
    1.3. Call with out of range mirrorQosType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range monitorQosConfigPtr->tc [CPSS_TC_RANGE_CNS=8]
                   and other parameters from 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.5. Call with out of range monitorQosConfigPtr->dp [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with monitorQosConfigPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_QOS_TYPE_ENT mirrorType = CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_E;
    CPSS_EXMXPM_MIRROR_QOS_STC      monitorQosConfig;
    CPSS_EXMXPM_MIRROR_QOS_STC      monitorQosConfigGet;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mirrorQosType [CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_E /
                                          CPSS_EXMXPM_MIRROR_QOS_TYPE_EGRESS_E /
                                          CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_POLICY_E]
                           and monitorQosConfigPtr {qosEnable [GT_FALSE / GT_TRUE],
                                                    tc [0 / 3 / 7],
                                                    dp [CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E / CPSS_DP_RED_E]}.
            Expected: GT_OK.
        */

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_E;

        monitorQosConfig.qosEnable = GT_FALSE;
        monitorQosConfig.tc        = 0;
        monitorQosConfig.dp        = CPSS_DP_GREEN_E;

        st = cpssExMxPmMirrorQosSet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call cpssExMxPmMirrorQosGet with the same mirrorType
                                                  and non-null monitorQosConfigPtr.
            Expected: GT_OK and the same monitorQosConfigPtr.
        */
        st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorQosGet: %d, %d", dev, mirrorType);

        /* Verifying struct values */
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.qosEnable, monitorQosConfigGet.qosEnable,
                   "get another monitorQosConfig->qosEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.tc, monitorQosConfigGet.tc,
                   "get another monitorQosConfig->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.dp, monitorQosConfigGet.dp,
                   "get another monitorQosConfig->dp than was set: %d", dev);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_QOS_TYPE_EGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_QOS_TYPE_EGRESS_E;

        monitorQosConfig.qosEnable = GT_TRUE;
        monitorQosConfig.tc        = 3;
        monitorQosConfig.dp        = CPSS_DP_YELLOW_E;

        st = cpssExMxPmMirrorQosSet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call cpssExMxPmMirrorQosGet with the same mirrorType
                                                  and non-null monitorQosConfigPtr.
            Expected: GT_OK and the same monitorQosConfigPtr.
        */
        st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorQosGet: %d, %d", dev, mirrorType);

        /* Verifying struct values */
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.qosEnable, monitorQosConfigGet.qosEnable,
                   "get another monitorQosConfig->qosEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.tc, monitorQosConfigGet.tc,
                   "get another monitorQosConfig->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.dp, monitorQosConfigGet.dp,
                   "get another monitorQosConfig->dp than was set: %d", dev);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_POLICY_E] */

        mirrorType = CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_POLICY_E;

        monitorQosConfig.tc = 7;
        monitorQosConfig.dp = CPSS_DP_RED_E;

        st = cpssExMxPmMirrorQosSet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call cpssExMxPmMirrorQosGet with the same mirrorType
                                                  and non-null monitorQosConfigPtr.
            Expected: GT_OK and the same monitorQosConfigPtr.
        */
        st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorQosGet: %d, %d", dev, mirrorType);

        /* Verifying struct values */
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.qosEnable, monitorQosConfigGet.qosEnable,
                   "get another monitorQosConfig->qosEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.tc, monitorQosConfigGet.tc,
                   "get another monitorQosConfig->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(monitorQosConfig.dp, monitorQosConfigGet.dp,
                   "get another monitorQosConfig->dp than was set: %d", dev);

        /*
            1.3. Call with out of range mirrorType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorQosSet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorType);

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        /*
            1.4. Call with out of range monitorQosConfigPtr->tc [CPSS_TC_RANGE_CNS=8]
                           and other parameters from 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        monitorQosConfig.tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmMirrorQosSet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, monitorQosConfigPtr->tc = %d",
                                     dev, monitorQosConfig.tc);

        monitorQosConfig.tc = 0;

        /*
            1.5. Call with out of range monitorQosConfigPtr->dp [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        monitorQosConfig.dp = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorQosSet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, monitorQosConfigPtr->dp = %d",
                                     dev, monitorQosConfig.dp);

        monitorQosConfig.dp = CPSS_DP_GREEN_E;

        /*
            1.6. Call with monitorQosConfigPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMirrorQosSet(dev, mirrorType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, monitorQosConfigPtr = NULL", dev);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_QOS_TYPE_INGRESS_E;

    monitorQosConfig.qosEnable = GT_TRUE;
    monitorQosConfig.tc        = 0;
    monitorQosConfig.dp        = CPSS_DP_GREEN_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorQosSet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorQosSet(dev, mirrorType, &monitorQosConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorQosGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_EXMXPM_MIRROR_TYPE_ENT  mirrorType,
    OUT CPSS_EXMXPM_MIRROR_QOS_STC   *monitorQosConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorQosGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                               CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                   and non-null monitorQosConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range mirrorType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with monitorQosConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_TYPE_ENT     mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    CPSS_EXMXPM_MIRROR_QOS_STC      monitorQosConfig;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E /
                                       CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E]
                           and non-null monitorQosConfigPtr.
            Expected: GT_OK.
        */

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E;

        st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /* Call with mirrorType [CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E] */
        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E;

        st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call with out of range mirrorType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorType = MIRROR_INVALID_ENUM_CNS;

        st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorType);

        mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

        /*
            1.3. Call with monitorQosConfigPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMirrorQosGet(dev, mirrorType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, monitorQosConfigPtr = NULL", dev);
    }

    mirrorType = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorQosGet(dev, mirrorType, &monitorQosConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorVlanTagInfoSet
(
    IN GT_U8                                    devNum,
    IN CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC     *infoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorVlanTagInfoSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with infoPtr {vpt [0 / 3 / 7],
                            cfiBit [0 / 1 / 0]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmMirrorVlanTagInfoGet with the same mirrorType
                                                  and non-null infoPtr.
    Expected: GT_OK and the same infoPtr.
    1.3. Call with out of range infoPtr->vpt [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range infoPtr->cfiBit [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with infoPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC    info;
    CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC    infoGet;
    int mirrorType = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with infoPtr {vpt [0 / 3 / 7],
                                    cfiBit [0 / 1 / 0]}.
            Expected: GT_OK.
        */

        /* Call with info.vpt = 0 */
        info.vpt       = 0;
        info.cfiBit    = 0;

        st = cpssExMxPmMirrorVlanTagInfoSet(dev, &info);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call cpssExMxPmMirrorVlanTagInfoGet with the same mirrorType
                                                          and non-null infoPtr.
            Expected: GT_OK and the same infoPtr.
        */
        st = cpssExMxPmMirrorVlanTagInfoGet(dev, &infoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorVlanTagInfoGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(info.vpt, infoGet.vpt,
                   "get another infoPtr->vpt than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(info.cfiBit, infoGet.cfiBit,
                   "get another infoPtr->cfiBit than was set: %d", dev);

        /* Call with info.vpt = 3 */
        info.vpt       = 3;
        info.cfiBit    = 1;

        st = cpssExMxPmMirrorVlanTagInfoSet(dev, &info);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call cpssExMxPmMirrorVlanTagInfoGet with the same mirrorType
                                                          and non-null infoPtr.
            Expected: GT_OK and the same infoPtr.
        */
        st = cpssExMxPmMirrorVlanTagInfoGet(dev, &infoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorVlanTagInfoGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(info.vpt, infoGet.vpt,
                   "get another infoPtr->vpt than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(info.cfiBit, infoGet.cfiBit,
                   "get another infoPtr->cfiBit than was set: %d", dev);

        /* Call with info.vpt = 7 */
        info.vpt       = 7;
        info.cfiBit    = 0;

        st = cpssExMxPmMirrorVlanTagInfoSet(dev, &info);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrorType);

        /*
            1.2. Call cpssExMxPmMirrorVlanTagInfoGet with the same mirrorType
                                                          and non-null infoPtr.
            Expected: GT_OK and the same infoPtr.
        */
        st = cpssExMxPmMirrorVlanTagInfoGet(dev, &infoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmMirrorVlanTagInfoGet: %d, %d", dev, mirrorType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(info.vpt, infoGet.vpt,
                   "get another infoPtr->vpt than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(info.cfiBit, infoGet.cfiBit,
                   "get another infoPtr->cfiBit than was set: %d", dev);

        /*
            1.3. Call with out of range infoPtr->vpt [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        info.vpt = 8;

        st = cpssExMxPmMirrorVlanTagInfoSet(dev,  &info);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, infoPtr->vpt = %d", dev, info.vpt);

        info.vpt = 0;

        /*
            1.4. Call with out of range infoPtr->cfiBit [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        info.cfiBit = 2;

        st = cpssExMxPmMirrorVlanTagInfoSet(dev,  &info);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, infoPtr->cfiBit = %d", dev, info.cfiBit);

        info.cfiBit = 0;

        /*
            1.5. Call with infoPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMirrorVlanTagInfoSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, infoPtr = NULL", dev);
    }


    info.vpt       = 0;
    info.cfiBit    = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorVlanTagInfoSet(dev,  &info);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorVlanTagInfoSet(dev,  &info);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmMirrorVlanTagInfoGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC    *infoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmMirrorVlanTagInfoGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null infoPtr.
    Expected: GT_OK.
    1.2. Call with infoPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC    info;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null infoPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmMirrorVlanTagInfoGet(dev,  &info);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call with infoPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmMirrorVlanTagInfoGet(dev,  NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, infoPtr = NULL", dev);
    }

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmMirrorVlanTagInfoGet(dev,  &info);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmMirrorVlanTagInfoGet(dev,  &info);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmMirror suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmMirror)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorStatEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorStatEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorStatRatioSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorStatRatioGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorPortEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorPortEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorDestInterfaceSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorDestInterfaceGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorQosSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorQosGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorVlanTagInfoSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmMirrorVlanTagInfoGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmMirror)
