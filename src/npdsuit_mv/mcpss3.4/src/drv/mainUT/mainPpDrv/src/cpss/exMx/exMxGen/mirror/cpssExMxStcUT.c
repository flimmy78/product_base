/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxStcUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxStc, that provides
*       CPSS ExMx Sampling To CPU (STC) API implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/mirror/cpssExMxStc.h>


#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define MIRROR_STC_INVALID_ENUM_CNS     0x5AAAAAA5

/* Tests use this value as default valid value for port */
#define EXMX_STC_VALID_PHY_PORT_CNS     0

/* Macro to define is device of 98Ex136 type       */
/* devType  - type of device (CPSS_PP_DEVICE_TYPE) */
#define IS_98EX136_DEV_MAC(devNum)                                         \
                 ((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX136_CNS) || \
                 (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX136DI_CNS))

/* Macro to define is device of 98Ex135 type       */
/* devType  - type of device (CPSS_PP_DEVICE_TYPE) */
#define IS_98EX135_DEV_MAC(devNum)                                          \
                 (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX135D_CNS)

/* Macro to define is device of TIGER type       */
/* devType  - type of device (CPSS_PP_DEVICE_TYPE) */
#define IS_TIGER_DEV_MAC(devNum)                                            \
                 (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_TIGER_E)
                
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcReloadModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMX_STC_TYPE_ENT                  stcType,
    IN  CPSS_EXMX_STC_COUNT_RELOAD_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcReloadModeSet)
{
/*
    ITERATE_DEVICES (Only ExMxTg devices and 98EX135)
    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                     CPSS_EXMX_STC_EGRESS_E],
                            mode [CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E /
                                  CPSS_EXMX_STC_COUNT_RELOAD_TRIGGERED_E]
    Expected: GT_OK.
    1.2. Call cpssExMxStcReloadModeGet with non-NULL modeGet
              and other parameters from 1.1.
    Expected: GT_OK and mode the same as written.
    1.3. Call function with out of range stcType [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with out of range mode [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;

    CPSS_EXMX_STC_TYPE_ENT              stcType = CPSS_EXMX_STC_INGRESS_E;
    CPSS_EXMX_STC_COUNT_RELOAD_MODE_ENT mode    = CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E;
    CPSS_EXMX_STC_COUNT_RELOAD_MODE_ENT modeGet = CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_TIGER_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /*
                1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                                 CPSS_EXMX_STC_EGRESS_E],
                                        mode [CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E /
                                              CPSS_EXMX_STC_COUNT_RELOAD_TRIGGERED_E]
                Expected: GT_OK.
            */
            /* call with stcType = CPSS_EXMX_STC_INGRESS_E */
            stcType = CPSS_EXMX_STC_INGRESS_E;
            mode    = CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E;

            st = cpssExMxStcReloadModeSet(dev, stcType, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, stcType, mode);

            /*
                1.2. Call cpssExMxStcReloadModeGet with non-NULL modeGet
                          and other parameters from 1.1.
                Expected: GT_OK and mode the same as written.
            */
            st = cpssExMxStcReloadModeGet(dev, stcType, &modeGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxStcReloadModeSet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode then was set: %d", dev);

            /* call with stcType = CPSS_EXMX_STC_EGRESS_E */
            stcType = CPSS_EXMX_STC_EGRESS_E;
            mode    = CPSS_EXMX_STC_COUNT_RELOAD_TRIGGERED_E;

            st = cpssExMxStcReloadModeSet(dev, stcType, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, stcType, mode);

            /*
                1.2. Call cpssExMxStcReloadModeGet with non-NULL modeGet
                          and other parameters from 1.1.
                Expected: GT_OK and mode the same as written.
            */
            st = cpssExMxStcReloadModeGet(dev, stcType, &modeGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxStcReloadModeSet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode then was set: %d", dev);

            /*
                1.3. Call function with out of range stcType [0x5AAAAAA5]
                          and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            stcType = MIRROR_STC_INVALID_ENUM_CNS;

            st = cpssExMxStcReloadModeSet(dev, stcType, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, stcType);

            stcType = CPSS_EXMX_STC_INGRESS_E;

            /*
                1.4. Call function with out of range mode [0x5AAAAAA5]
                          and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            mode = MIRROR_STC_INVALID_ENUM_CNS;

            st = cpssExMxStcReloadModeSet(dev, stcType, mode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, mode = %d", dev, mode);
        }
    }

    stcType = CPSS_EXMX_STC_INGRESS_E;
    mode    = CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcReloadModeSet(dev, stcType, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcReloadModeSet(dev, stcType, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcReloadModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMX_STC_TYPE_ENT                  stcType,
    OUT CPSS_EXMX_STC_COUNT_RELOAD_MODE_ENT     *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcReloadModeGet)
{
/*
    ITERATE_DEVICES (Only ExMxTg devices and 98EX135)
    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                     CPSS_EXMX_STC_EGRESS_E],
                       and non NULL mode
    Expected: GT_OK.
    1.2. Call function with out of range stcType [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with mode [NULL]
              and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;

    CPSS_EXMX_STC_TYPE_ENT                  stcType = CPSS_EXMX_STC_INGRESS_E;
    CPSS_EXMX_STC_COUNT_RELOAD_MODE_ENT     mode    = CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_TIGER_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /*
                1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                                 CPSS_EXMX_STC_EGRESS_E],
                                   and non NULL mode
                Expected: GT_OK.
            */
            /* call with stcType = CPSS_EXMX_STC_INGRESS_E */
            stcType = CPSS_EXMX_STC_INGRESS_E;

            st = cpssExMxStcReloadModeGet(dev, stcType, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stcType);

            /* call with stcType = CPSS_EXMX_STC_EGRESS_E */
            stcType = CPSS_EXMX_STC_EGRESS_E;

            st = cpssExMxStcReloadModeGet(dev, stcType, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stcType);

            /*
                1.2. Call function with out of range stcType [0x5AAAAAA5]
                          and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            stcType = MIRROR_STC_INVALID_ENUM_CNS;

            st = cpssExMxStcReloadModeGet(dev, stcType, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, stcType);

            stcType = CPSS_EXMX_STC_EGRESS_E;

            /*
                1.3. Call function with mode [NULL]
                          and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxStcReloadModeGet(dev, stcType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mode = NULL", dev);
        }
    }

    stcType = CPSS_EXMX_STC_INGRESS_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcReloadModeGet(dev, stcType, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcReloadModeGet(dev, stcType, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMX_STC_TYPE_ENT      stcType,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcEnableSet)
{
/*
    ITERATE_DEVICES (Only ExMxTg devices and 98EX135)
    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                     CPSS_EXMX_STC_EGRESS_E],
                            enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK.
    1.2. Call cpssExMxStcEnableGet with non-NULL enableGet
              and other parameters from 1.1.
    Expected: GT_OK and enable the same as written.
    1.3. Call function with out of range stcType [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;

    CPSS_EXMX_STC_TYPE_ENT  stcType   = CPSS_EXMX_STC_INGRESS_E;
    GT_BOOL                 enable    = GT_TRUE;
    GT_BOOL                 enableGet = GT_TRUE;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_TIGER_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /*
                1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                                 CPSS_EXMX_STC_EGRESS_E],
                                        enable [GT_TRUE / GT_FALSE]
                Expected: GT_OK.
            */
            /* call with stcType = CPSS_EXMX_STC_INGRESS_E */
            stcType = CPSS_EXMX_STC_INGRESS_E;
            enable  = GT_TRUE;

            st = cpssExMxStcEnableSet(dev, stcType, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, stcType, enable);

            /*
                1.2. Call cpssExMxStcEnableGet with non-NULL enableGet
                          and other parameters from 1.1.
                Expected: GT_OK and enable the same as written.
            */
            st = cpssExMxStcEnableGet(dev, stcType, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxStcEnableGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

            /* call with stcType = CPSS_EXMX_STC_EGRESS_E */
            stcType = CPSS_EXMX_STC_EGRESS_E;
            enable  = GT_FALSE;

            st = cpssExMxStcEnableSet(dev, stcType, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, stcType, enable);

            /*
                1.2. Call cpssExMxStcEnableGet with non-NULL enableGet
                          and other parameters from 1.1.
                Expected: GT_OK and enable the same as written.
            */
            st = cpssExMxStcEnableGet(dev, stcType, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxStcEnableGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

            /*
                1.3. Call function with out of range stcType [0x5AAAAAA5]
                          and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            stcType = MIRROR_STC_INVALID_ENUM_CNS;

            st = cpssExMxStcEnableSet(dev, stcType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, stcType);
        }
    }

    stcType = CPSS_EXMX_STC_INGRESS_E;
    enable  = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcEnableSet(dev, stcType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcEnableSet(dev, stcType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcEnableGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMX_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcEnableGet)
{
/*
    ITERATE_DEVICES (Only ExMxTg devices and 98EX135)
    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                     CPSS_EXMX_STC_EGRESS_E],
                       and non NULL enable
    Expected: GT_OK.
    1.2. Call function with out of range stcType [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with enable [NULL]
              and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;

    CPSS_EXMX_STC_TYPE_ENT  stcType = CPSS_EXMX_STC_INGRESS_E;
    GT_BOOL                 enable  = GT_FALSE;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_TIGER_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /*
                1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                                 CPSS_EXMX_STC_EGRESS_E],
                                   and non NULL enable
                Expected: GT_OK.
            */
            /* call with stcType = CPSS_EXMX_STC_INGRESS_E */
            stcType = CPSS_EXMX_STC_INGRESS_E;

            st = cpssExMxStcEnableGet(dev, stcType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stcType);

            /* call with stcType = CPSS_EXMX_STC_EGRESS_E */
            stcType = CPSS_EXMX_STC_EGRESS_E;

            st = cpssExMxStcEnableGet(dev, stcType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stcType);

            /*
                1.2. Call function with out of range stcType [0x5AAAAAA5]
                          and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            stcType = MIRROR_STC_INVALID_ENUM_CNS;

            st = cpssExMxStcEnableGet(dev, stcType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, stcType);

            stcType = CPSS_EXMX_STC_INGRESS_E;

            /*
                1.3. Call function with enable [NULL]
                          and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxStcEnableGet(dev, stcType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enable = NULL", dev);
        }
    }

    stcType = CPSS_EXMX_STC_INGRESS_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcEnableGet(dev, stcType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcEnableGet(dev, stcType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcPortSampleValueSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMX_STC_TYPE_ENT      stcType,
    IN  GT_U32                      sampleVal
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcPortSampleValueSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (Only ExMxTg devices and 98EX135)
    1.1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                     CPSS_EXMX_STC_EGRESS_E],
                            sampleVal [0 / 0xFFFFFFFF]
    Expected: GT_OK.
    1.1.2. Call cpssExMxStcPortSampleValueGet with non-NULL sampleValGet
              and other parameters from 1.1.
    Expected: GT_OK and sampleVal the same as written.
    1.1.3. Call function with out of range stcType [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;
    GT_U8       port;

    CPSS_EXMX_STC_TYPE_ENT  stcType      = CPSS_EXMX_STC_INGRESS_E;
    GT_U32                  sampleVal    = 0;
    GT_U32                  sampleValGet = 0;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_TIGER_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1. For all active devices go over all available physical ports. */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
            {
                /*
                    1.1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                                     CPSS_EXMX_STC_EGRESS_E],
                                            sampleVal [0 / 0xFFFFFFFF]
                    Expected: GT_OK.
                */
                /* call with stcType = CPSS_EXMX_STC_INGRESS_E */
                stcType   = CPSS_EXMX_STC_INGRESS_E;
                sampleVal = 0;

                st = cpssExMxStcPortSampleValueSet(dev, port, stcType, sampleVal);
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stcType, sampleVal);
                /*
                    1.1.2. Call cpssExMxStcPortSampleValueGet with non-NULL sampleValGet
                              and other parameters from 1.1.
                    Expected: GT_OK and sampleVal the same as written.
                */
                st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleValGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxStcPortSampleValueGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(sampleVal, sampleValGet, "got another sampleVal than was set: %d, %d", dev, port);

                /* call with stcType = CPSS_EXMX_STC_EGRESS_E */
                stcType   = CPSS_EXMX_STC_EGRESS_E;
                sampleVal = 0xFFFFFFFF;

                st = cpssExMxStcPortSampleValueSet(dev, port, stcType, sampleVal);
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stcType, sampleVal);

                /*
                    1.1.2. Call cpssExMxStcPortSampleValueGet with non-NULL sampleValGet
                              and other parameters from 1.1.
                    Expected: GT_OK and sampleVal the same as written.
                */
                st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleValGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxStcPortSampleValueGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(sampleVal, sampleValGet, "got another sampleVal than was set: %d, %d", dev, port);

                /*
                    1.1.3. Call function with out of range stcType [0x5AAAAAA5]
                              and other parameters from 1.1.
                    Expected: GT_BAD_PARAM.
                */
                stcType = MIRROR_STC_INVALID_ENUM_CNS;

                st = cpssExMxStcPortSampleValueSet(dev, port, stcType, sampleVal);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);
            }

            stcType   = CPSS_EXMX_STC_INGRESS_E;
            sampleVal = 0;

            /* 1.2. Check that function handles CPU port    */
            port = CPSS_CPU_PORT_NUM_CNS;

            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.3. Go over non configured ports */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. Call function for each non configured port.   */
                st = cpssExMxStcPortSampleValueSet(dev, port, stcType, sampleVal);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }

            /* 1.4. For active device check that function returns GT_BAD_PARAM  */
            /* for out of bound value for port number.                          */
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxStcPortSampleValueSet(dev, port, stcType, sampleVal);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    stcType   = CPSS_EXMX_STC_INGRESS_E;
    sampleVal = 0;

    port = EXMX_STC_VALID_PHY_PORT_CNS;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcPortSampleValueSet(dev, port, stcType, sampleVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcPortSampleValueSet(dev, port, stcType, sampleVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcPortSampleValueGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMX_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *sampleValPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcPortSampleValueGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (Only ExMxTg devices and 98EX135)
    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                     CPSS_EXMX_STC_EGRESS_E],
                       and non NULL sampleValPtr
    Expected: GT_OK.
    1.2. Call function with out of range stcType [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with sampleValPtr [NULL]
              and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;
    GT_U8       port;

    CPSS_EXMX_STC_TYPE_ENT  stcType      = CPSS_EXMX_STC_INGRESS_E;
    GT_U32                  sampleVal    = 0;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_TIGER_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1. For all active devices go over all available physical ports. */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
            {
                /*
                    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                                     CPSS_EXMX_STC_EGRESS_E],
                                       and non NULL sampleValPtr
                    Expected: GT_OK.
                */
                /* call with stcType = CPSS_EXMX_STC_INGRESS_E */
                stcType = CPSS_EXMX_STC_INGRESS_E;

                st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleVal);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

                /* call with stcType = CPSS_EXMX_STC_INGRESS_E */
                stcType = CPSS_EXMX_STC_INGRESS_E;

                st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleVal);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

                /*
                    1.2. Call function with out of range stcType [0x5AAAAAA5]
                              and other parameters from 1.1.
                    Expected: GT_BAD_PARAM.
                */
                stcType = MIRROR_STC_INVALID_ENUM_CNS;

                st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleVal);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

                stcType = CPSS_EXMX_STC_INGRESS_E;

                /*
                    1.3. Call function with sampleValPtr [NULL]
                              and other parameters from 1.1.
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxStcPortSampleValueGet(dev, port, stcType, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, sampleValPtr = %d", dev, port);
            }

            stcType = CPSS_EXMX_STC_INGRESS_E;

            /* 1.2. Check that function handles CPU port    */
            port = CPSS_CPU_PORT_NUM_CNS;

            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.3. Go over non configured ports */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. Call function for each non configured port.   */
                st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleVal);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }

            /* 1.4. For active device check that function returns GT_BAD_PARAM  */
            /* for out of bound value for port number.                          */
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleVal);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    stcType = CPSS_EXMX_STC_INGRESS_E;

    port = EXMX_STC_VALID_PHY_PORT_CNS;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcPortSampleValueGet(dev, port, stcType, &sampleVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcPortCountdownCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMX_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcPortCountdownCntrGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (Only ExMxTg devices and 98EX135)
    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                     CPSS_EXMX_STC_EGRESS_E],
                       and non NULL cntrPtr
    Expected: GT_OK.
    1.2. Call function with out of range stcType [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with cntrPtr [NULL]
              and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;
    GT_U8       port;

    CPSS_EXMX_STC_TYPE_ENT  stcType = CPSS_EXMX_STC_INGRESS_E;
    GT_U32                  cntr    = 0;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_TIGER_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1. For all active devices go over all available physical ports. */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
            {
                /*
                    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                                     CPSS_EXMX_STC_EGRESS_E],
                                       and non NULL cntrPtr
                    Expected: GT_OK.
                */
                /* call with stcType = CPSS_EXMX_STC_INGRESS_E*/
                stcType = CPSS_EXMX_STC_INGRESS_E;

                st = cpssExMxStcPortCountdownCntrGet(dev, port, stcType, &cntr);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

                /* call with stcType = CPSS_EXMX_STC_EGRESS_E*/
                stcType = CPSS_EXMX_STC_EGRESS_E;

                st = cpssExMxStcPortCountdownCntrGet(dev, port, stcType, &cntr);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

                /*
                    1.2. Call function with out of range stcType [0x5AAAAAA5]
                              and other parameters from 1.1.
                    Expected: GT_BAD_PARAM.
                */
                stcType = MIRROR_STC_INVALID_ENUM_CNS;

                st = cpssExMxStcPortCountdownCntrGet(dev, port, stcType, &cntr);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

                stcType = CPSS_EXMX_STC_INGRESS_E;

                /*
                    1.3. Call function with cntrPtr [NULL]
                              and other parameters from 1.1.
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxStcPortCountdownCntrGet(dev, port, stcType, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cntrPtr = NULL", dev, port);
            }

            stcType = CPSS_EXMX_STC_INGRESS_E;

            /* 1.2. Check that function handles CPU port    */
            port = CPSS_CPU_PORT_NUM_CNS;

            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.3. Go over non configured ports */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. Call function for each non configured port.   */
                st = cpssExMxStcPortCountdownCntrGet(dev, port, stcType, &cntr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }

            /* 1.4. For active device check that function returns GT_BAD_PARAM  */
            /* for out of bound value for port number.                          */
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxStcPortCountdownCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    stcType = CPSS_EXMX_STC_INGRESS_E;

    port = EXMX_STC_VALID_PHY_PORT_CNS;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcPortCountdownCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcPortCountdownCntrGet(dev, port, stcType, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcPortSampledPacketsCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMX_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcPortSampledPacketsCntrGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (Only ExMxTg devices and 98EX135)
    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                     CPSS_EXMX_STC_EGRESS_E],
                       and non NULL cntrPtr
    Expected: GT_OK.
    1.2. Call function with out of range stcType [0x5AAAAAA5]
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with cntrPtr [NULL]
              and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;
    GT_U8       port;

    CPSS_EXMX_STC_TYPE_ENT  stcType = CPSS_EXMX_STC_INGRESS_E;
    GT_U32                  cntr    = 0;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_TIGER_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1. For all active devices go over all available physical ports. */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
            {
                /*
                    1.1. Call function with stcType [CPSS_EXMX_STC_INGRESS_E /
                                                     CPSS_EXMX_STC_EGRESS_E],
                                       and non NULL cntrPtr
                    Expected: GT_OK.
                */
                /* call with stcType = CPSS_EXMX_STC_INGRESS_E*/
                stcType = CPSS_EXMX_STC_INGRESS_E;

                st = cpssExMxStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

                /* call with stcType = CPSS_EXMX_STC_EGRESS_E*/
                stcType = CPSS_EXMX_STC_EGRESS_E;

                st = cpssExMxStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

                /*
                    1.2. Call function with out of range stcType [0x5AAAAAA5]
                              and other parameters from 1.1.
                    Expected: GT_BAD_PARAM.
                */
                stcType = MIRROR_STC_INVALID_ENUM_CNS;

                st = cpssExMxStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

                stcType = CPSS_EXMX_STC_INGRESS_E;

                /*
                    1.3. Call function with cntrPtr [NULL]
                              and other parameters from 1.1.
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxStcPortSampledPacketsCntrGet(dev, port, stcType, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cntrPtr = NULL", dev, port);
            }

            stcType = CPSS_EXMX_STC_INGRESS_E;

            /* 1.2. Check that function handles CPU port    */
            port = CPSS_CPU_PORT_NUM_CNS;

            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.3. Go over non configured ports */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. Call function for each non configured port.   */
                st = cpssExMxStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }

            /* 1.4. For active device check that function returns GT_BAD_PARAM  */
            /* for out of bound value for port number.                          */
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    stcType = CPSS_EXMX_STC_INGRESS_E;

    port    = EXMX_STC_VALID_PHY_PORT_CNS;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcIngressFifoFullLevelSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      treshold
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcIngressFifoFullLevelSet)
{
/*
    ITERATE_DEVICES (Only Ex136 and Ex135 devices)
    1.1. Call function with treshold [0 / 31],
    Expected: GT_OK.
    1.2. Call cpssExMxStcIngressFifoFullLevelGet with non-NULL tresholdGet
              and other parameters from 1.1.
    Expected: GT_OK and tresholdGet the same as written.
    1.3. Call function with out of range treshold [32]
              and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;

    GT_U32  treshold    = 0;
    GT_U32  tresholdGet = 0;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_98EX136_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /*
                1.1. Call function with treshold [0 / 31],
                Expected: GT_OK.
            */
            /* call with treshold = 0 */
            treshold = 0;

            st = cpssExMxStcIngressFifoFullLevelSet(dev, treshold);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, treshold);

            /*
                1.2. Call cpssExMxStcIngressFifoFullLevelGet with non-NULL tresholdGet
                          and other parameters from 1.1.
                Expected: GT_OK and tresholdGet the same as written.
            */
            st = cpssExMxStcIngressFifoFullLevelGet(dev, &tresholdGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxStcIngressFifoFullLevelGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(treshold, tresholdGet, "got another treshold than was set: %d", dev);

            /* call with treshold = 31 */
            treshold = 31;

            st = cpssExMxStcIngressFifoFullLevelSet(dev, treshold);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, treshold);

            /*
                1.2. Call cpssExMxStcIngressFifoFullLevelGet with non-NULL tresholdGet
                          and other parameters from 1.1.
                Expected: GT_OK and tresholdGet the same as written.
            */
            st = cpssExMxStcIngressFifoFullLevelGet(dev, &tresholdGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxStcIngressFifoFullLevelGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(treshold, tresholdGet, "got another treshold than was set: %d", dev);

            /*
                1.3. Call function with out of range treshold [32]
                          and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            treshold = 32;

            st = cpssExMxStcIngressFifoFullLevelSet(dev, treshold);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, treshold);
        }
    }

    treshold = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcIngressFifoFullLevelSet(dev, treshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcIngressFifoFullLevelSet(dev, treshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcIngressFifoFullLevelGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *tresholdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcIngressFifoFullLevelGet)
{
/*
    ITERATE_DEVICES (Only Ex136 and Ex135 devices)
    1.1. Call function with non NULL tresholdPtr
    Expected: GT_OK.
    1.2. Call function with tresholdPtr [NULL]
              and other parameter from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;

    GT_U32      treshold = 0;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_98EX136_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /*
                1.1. Call function with non NULL tresholdPtr
                Expected: GT_OK.
            */
            st = cpssExMxStcIngressFifoFullLevelGet(dev, &treshold);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call function with tresholdPtr [NULL]
                          and other parameter from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxStcIngressFifoFullLevelGet(dev, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tresholdPtr = NULL", dev);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcIngressFifoFullLevelGet(dev, &treshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcIngressFifoFullLevelGet(dev, &treshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS   cpssExMxStcPortIngressFifoFullStatusGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *emptyStatusPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcPortIngressFifoFullStatusGet)
{
/*
    ITERATE_DEVICES (Only Ex136 and Ex135 devices)
    1.1. Call function with non NULL emptyStatusPtr
    Expected: GT_OK.
    1.2. Call function with emptyStatusPtr [NULL]
              and other parameter from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;

    GT_BOOL     emptyStatus = GT_FALSE;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_98EX136_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /*
                1.1. Call function with non NULL emptyStatusPtr
                Expected: GT_OK.
            */
            st = cpssExMxStcPortIngressFifoFullStatusGet(dev, &emptyStatus);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call function with emptyStatusPtr [NULL]
                          and other parameter from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxStcPortIngressFifoFullStatusGet(dev, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, emptyStatusPtr = NULL", dev);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcPortIngressFifoFullStatusGet(dev, &emptyStatus);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcPortIngressFifoFullStatusGet(dev, &emptyStatus);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxStcIngressFifoStatusGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *statusPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxStcIngressFifoStatusGet)
{
/*
    ITERATE_DEVICES (Only Ex136 and Ex135 devices)
    1.1. Call function with non NULL statusPtr
    Expected: GT_OK.
    1.2. Call function with statusPtr [NULL]
              and other parameter from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8   	dev;

    GT_U32      status = 0;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if ((IS_98EX136_DEV_MAC(dev)) || (IS_98EX135_DEV_MAC(dev)))
        {
            /*
                1.1. Call function with non NULL statusPtr
                Expected: GT_OK.
            */
            st = cpssExMxStcIngressFifoStatusGet(dev, &status);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call function with statusPtr [NULL]
                          and other parameter from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxStcIngressFifoStatusGet(dev, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, statusPtr = NULL", dev);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        st = cpssExMxStcIngressFifoStatusGet(dev, &status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxStcIngressFifoStatusGet(dev, &status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxStc suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxStc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcReloadModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcReloadModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcPortSampleValueSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcPortSampleValueGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcPortCountdownCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcPortSampledPacketsCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcIngressFifoFullLevelSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcIngressFifoFullLevelGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcPortIngressFifoFullStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxStcIngressFifoStatusGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxStc)

