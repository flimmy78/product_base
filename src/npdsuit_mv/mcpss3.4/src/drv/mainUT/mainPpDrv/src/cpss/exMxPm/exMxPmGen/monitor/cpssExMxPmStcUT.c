/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmStcUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmStc, that provides
*       CPSS ExMxPm Sampling To CPU (STC) API implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/monitor/cpssExMxPmStc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define STC_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define STC_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcIngressCountModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_EXMXPM_STC_COUNT_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcIngressCountModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E /
                         CPSS_EXMXPM_STC_COUNT_NON_DROPPED_PACKETS_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmStcIngressCountModeGet with
    non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call with out of range mode [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev; 

    CPSS_EXMXPM_STC_COUNT_MODE_ENT mode    = CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E;
    CPSS_EXMXPM_STC_COUNT_MODE_ENT modeGet = CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with mode [CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E / 
                                CPSS_EXMXPM_STC_COUNT_NON_DROPPED_PACKETS_E].
           Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E] */
        mode = CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E;

        st = cpssExMxPmStcIngressCountModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
           1.2. Call cpssExMxPmStcIngressCountModeGet with non-NULL modePtr.
           Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmStcIngressCountModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmStcIngressCountModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_STC_COUNT_NON_DROPPED_PACKETS_E] */
        mode = CPSS_EXMXPM_STC_COUNT_NON_DROPPED_PACKETS_E;

        st = cpssExMxPmStcIngressCountModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
           1.2. Call cpssExMxPmStcIngressCountModeGet with non-NULL modePtr.
           Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmStcIngressCountModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmStcIngressCountModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "get another mode than was set: %d", dev);

        /*
           1.3. Call with out of range mode [0x5AAAAAA5]
           Expected: GT_BAD_PARAM.
        */
        mode = STC_INVALID_ENUM_CNS;

        st = cpssExMxPmStcIngressCountModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcIngressCountModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcIngressCountModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcIngressCountModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_EXMXPM_STC_COUNT_MODE_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcIngressCountModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev; 

    CPSS_EXMXPM_STC_COUNT_MODE_ENT mode = CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with non-null modePtr.
           Expected: GT_OK.
        */
        st = cpssExMxPmStcIngressCountModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
           1.2. Call with modePtr [NULL].
           Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmStcIngressCountModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcIngressCountModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcIngressCountModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcReloadModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_STC_TYPE_ENT                stcType,
    IN  CPSS_EXMXPM_STC_COUNT_RELOAD_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcReloadModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                            CPSS_EXMXPM_STC_EGRESS_E], 
                   mode [CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E /
                         CPSS_EXMXPM_STC_COUNT_RELOAD_TRIGGERED_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmStcReloadModeGet with the same stcType and non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call with out of range stcType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range mode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev; 

    CPSS_EXMXPM_STC_TYPE_ENT              stcType = CPSS_EXMXPM_STC_INGRESS_E;
    CPSS_EXMXPM_STC_COUNT_RELOAD_MODE_ENT mode    = CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E;
    CPSS_EXMXPM_STC_COUNT_RELOAD_MODE_ENT modeGet = CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                                   CPSS_EXMXPM_STC_EGRESS_E], 
                          mode [CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E /
                               CPSS_EXMXPM_STC_COUNT_RELOAD_TRIGGERED_E].
           Expected: GT_OK.
        */

        /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
        stcType = CPSS_EXMXPM_STC_INGRESS_E;
        mode    = CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E;

        st = cpssExMxPmStcReloadModeSet(dev, stcType, mode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, stcType, mode);

        /*
            1.2. Call cpssExMxPmStcReloadModeGet with the same stcType and non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmStcReloadModeGet(dev, stcType, &modeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmStcReloadModeGet: %d, %d", dev, stcType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "get another mode than was set: %d", dev);

        /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;
        mode    = CPSS_EXMXPM_STC_COUNT_RELOAD_TRIGGERED_E;

        st = cpssExMxPmStcReloadModeSet(dev, stcType, mode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, stcType, mode);

        /*
           1.2. Call cpssExMxPmStcReloadModeGet with the same stcType and non-NULL modePtr.
           Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmStcReloadModeGet(dev, stcType, &modeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmStcReloadModeGet: %d, %d", dev, stcType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "get another mode than was set: %d", dev);

        /*
            1.3. Call with out of range stcType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        stcType = STC_INVALID_ENUM_CNS;
        
        st = cpssExMxPmStcReloadModeSet(dev, stcType, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, stcType);

        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        /*
            1.4. Call with out of range mode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mode = STC_INVALID_ENUM_CNS;

        st = cpssExMxPmStcReloadModeSet(dev, stcType, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, mode = %d", dev, mode);
    }

    stcType = CPSS_EXMXPM_STC_INGRESS_E;
    mode    = CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcReloadModeSet(dev, stcType, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcReloadModeSet(dev, stcType, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcReloadModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_STC_TYPE_ENT                stcType,
    OUT CPSS_EXMXPM_STC_COUNT_RELOAD_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcReloadModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                            CPSS_EXMXPM_STC_EGRESS_E], 
                   non-null modePtr.
    Expected: GT_OK.
    1.2. Call with out of range stcType [0x5AAAAAA5] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with modePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev; 

    CPSS_EXMXPM_STC_TYPE_ENT              stcType = CPSS_EXMXPM_STC_INGRESS_E;
    CPSS_EXMXPM_STC_COUNT_RELOAD_MODE_ENT mode    = CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                                   CPSS_EXMXPM_STC_EGRESS_E],
                          non-null modePtr.
           Expected: GT_OK.
        */

        /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        st = cpssExMxPmStcReloadModeGet(dev, stcType, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stcType);

        /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = cpssExMxPmStcReloadModeGet(dev, stcType, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stcType);

        /*
           1.2. Call with out of range stcType [0x5AAAAAA5] 
                          and other parameters from 1.1.
           Expected: GT_BAD_PARAM.
        */
        stcType = STC_INVALID_ENUM_CNS;

        st = cpssExMxPmStcReloadModeGet(dev, stcType, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, stcType);

        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        /*
           1.3. Call with modePtr [NULL] and other parameters from 1.1.
           Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmStcReloadModeGet(dev, stcType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    stcType = CPSS_EXMXPM_STC_INGRESS_E;
    mode    = CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcReloadModeGet(dev, stcType, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcReloadModeGet(dev, stcType, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_STC_TYPE_ENT    stcType,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                            CPSS_EXMXPM_STC_EGRESS_E], 
                   enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmStcEnableGet with the same stcType
                                              and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with out of range stcType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev; 

    CPSS_EXMXPM_STC_TYPE_ENT    stcType   = CPSS_EXMXPM_STC_INGRESS_E;
    GT_BOOL                     enable    = GT_TRUE;
    GT_BOOL                     enableGet = GT_TRUE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                                   CPSS_EXMXPM_STC_EGRESS_E], 
                          enable [GT_TRUE / GT_FALSE].
           Expected: GT_OK.
        */

        /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
        stcType = CPSS_EXMXPM_STC_INGRESS_E;
        enable  = GT_TRUE;

        st = cpssExMxPmStcEnableSet(dev, stcType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, stcType, enable);

        /*
           1.2. Call cpssExMxPmStcEnableGet with non-NULL enablePtr
                                                 and same stcType.
           Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmStcEnableGet(dev, stcType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                                     "cpssExMxPmStcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                     "get another enable than was set: %d", dev);

        /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;
        enable  = GT_FALSE;

        st = cpssExMxPmStcEnableSet(dev, stcType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, stcType, enable);

        /*
           1.2. Call cpssExMxPmStcEnableGet with non-NULL enablePtr
                                                 and same stcType.
           Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmStcEnableGet(dev, stcType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                                     "cpssExMxPmStcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                     "get another enable than was set: %d", dev);

        /*
           1.3. Call with out of range stcType [0x5AAAAAA5]
                          and other parameters from 1.1.
           Expected: GT_BAD_PARAM.
        */
        stcType = STC_INVALID_ENUM_CNS;
        
        st = cpssExMxPmStcEnableSet(dev, stcType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, stcType);
    }

    stcType = CPSS_EXMXPM_STC_INGRESS_E;
    enable  = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcEnableSet(dev, stcType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcEnableSet(dev, stcType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcEnableGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_STC_TYPE_ENT    stcType,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                            CPSS_EXMXPM_STC_EGRESS_E],
                   non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range stcType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev; 

    CPSS_EXMXPM_STC_TYPE_ENT    stcType = CPSS_EXMXPM_STC_INGRESS_E;
    GT_BOOL                     enable  = GT_TRUE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                                   CPSS_EXMXPM_STC_EGRESS_E],
                          non-null enablePtr.
           Expected: GT_OK.
        */

        /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        st = cpssExMxPmStcEnableGet(dev, stcType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stcType);

        /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = cpssExMxPmStcEnableGet(dev, stcType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stcType);

        /*
           1.2. Call with out of range stcType [0x5AAAAAA5]
                          and other parameters from 1.1.
           Expected: GT_BAD_PARAM.
        */
        stcType = STC_INVALID_ENUM_CNS;

        st = cpssExMxPmStcEnableGet(dev, stcType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, stcType);

        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        /*
           1.3. Call with enablePtr [NULL] and other parameters from 1.1.
           Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmStcEnableGet(dev, stcType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    stcType = CPSS_EXMXPM_STC_INGRESS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcEnableGet(dev, stcType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcEnableGet(dev, stcType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcPortLimitSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMXPM_STC_TYPE_ENT    stcType,
    IN  GT_U32                      limit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcPortLimitSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1 Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                             CPSS_EXMXPM_STC_EGRESS_E],
                    limit [0 / 0xFFFF].
    Expected: GT_OK.
    1.1.2 Call cpssExMxPmStcPortLimitGet with same stcType
                                              and non-null limitPtr.
    Expected: GT_OK and same limit.
    1.1.3. Call with out of range stcType [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with out of range limit [0x40000000]
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                     and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = STC_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_STC_TYPE_ENT stcType  = CPSS_EXMXPM_STC_INGRESS_E;
    GT_U32                   limit    = 0;
    GT_U32                   limitGet = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
               1.1.1 Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                                        CPSS_EXMXPM_STC_EGRESS_E],
                               limit [0 / 0xFFFF].
               Expected: GT_OK.
            */

            /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
            stcType = CPSS_EXMXPM_STC_EGRESS_E;
            limit   = 0;

            st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stcType, limit);

            /*
                1.1.2 Call cpssExMxPmStcPortLimitGet with same stcType
                                                          and non-null limitPtr.
                Expected: GT_OK and same limit.
            */
            st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limitGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                                   "cpssExMxPmStcPortLimitGet: %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(limit, limitGet,
                                   "get another limit than was set: %d", dev);

            /*  Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
            stcType = CPSS_EXMXPM_STC_EGRESS_E;
            limit   = 0xFFFF;

            st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stcType, limit);

            /*
                1.1.2 Call cpssExMxPmStcPortLimitGet with same stcType
                                                          and non-null limitPtr.
                Expected: GT_OK and same limit.
            */
            st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limitGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                                   "cpssExMxPmStcPortLimitGet: %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(limit, limitGet,
                                   "get another limit than was set: %d", dev);

            /*
                1.1.3. Call with out of range stcType [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            stcType = STC_INVALID_ENUM_CNS;
            
            st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

            stcType = CPSS_EXMXPM_STC_EGRESS_E;

            /*
               1.1.4. Call with out of range limit [0x40000000]
                                and other parameters from 1.1.
               Expected: NOT GT_OK.
            */
            limit = 0x40000000;

            st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, limit = %d", dev, port, limit);
        }

        stcType = CPSS_EXMXPM_STC_INGRESS_E;
        limit   = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /*
            1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                             and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        port = CPSS_CPU_PORT_NUM_CNS;

        stcType = CPSS_EXMXPM_STC_INGRESS_E;
        
        st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

        /*
            1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                             and other parameters from 1.1.
            Expected: GT_OK.
        */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);
    }

    stcType = CPSS_EXMXPM_STC_INGRESS_E;
    limit   = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = STC_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcPortLimitSet(dev, port, stcType, limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcPortLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMXPM_STC_TYPE_ENT    stcType,
    OUT GT_U32                      *limitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcPortLimitGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                              CPSS_EXMXPM_STC_EGRESS_E]
                     and non-null limitPtr.
    Expected: GT_OK.
    1.1.2. Call with out of range stcType [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with limitPtr [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                     and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = STC_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_STC_TYPE_ENT stcType = CPSS_EXMXPM_STC_INGRESS_E;
    GT_U32                   limit   = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
               1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E / 
                                         CPSS_EXMXPM_STC_EGRESS_E]
                                and non-null limitPtr.
               Expected: GT_OK.
            */

            /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
            stcType = CPSS_EXMXPM_STC_INGRESS_E;

            st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

            /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
            stcType = CPSS_EXMXPM_STC_EGRESS_E;

            st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

            /*
               1.1.2. Call with out of range stcType [0x5AAAAAA5]
                                and other parameters from 1.1.
               Expected: GT_BAD_PARAM.
            */
            stcType = STC_INVALID_ENUM_CNS;

            st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

            stcType = CPSS_EXMXPM_STC_EGRESS_E;

            /*
                1.1.3. Call with limitPtr [NULL]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmStcPortLimitGet(dev, port, stcType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, limitPtr = NULL", dev, port);
        }

        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /*
           1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                            and other parameters from 1.1.
           Expected: GT_BAD_PARAM.
        */
        port = CPSS_CPU_PORT_NUM_CNS;

        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

        /*
           1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                            and other parameters from 1.1.
           Expected: GT_OK.
        */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);
    }

    stcType = CPSS_EXMXPM_STC_EGRESS_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = STC_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcPortLimitGet(dev, port, stcType, &limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcPortReadyForNewLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMXPM_STC_TYPE_ENT    stcType,
    OUT GT_BOOL                     *isReadyPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcPortReadyForNewLimitGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                              CPSS_EXMXPM_STC_EGRESS_E]
                     and non-null isReadyPtr.
    Expected: GT_OK.
    1.1.2. Call with out of range stcType [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with isReadyPtr [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                     and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = STC_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_STC_TYPE_ENT stcType = CPSS_EXMXPM_STC_INGRESS_E;
    GT_BOOL                  isReady = GT_TRUE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
               1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                                         CPSS_EXMXPM_STC_EGRESS_E]
                                and non-null isReadyPtr.
               Expected: GT_OK.
            */

            /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
            stcType = CPSS_EXMXPM_STC_INGRESS_E;

            st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

            /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
            stcType = CPSS_EXMXPM_STC_EGRESS_E;

            st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

            /*
               1.1.2. Call with out of range stcType [0x5AAAAAA5]
                                and other parameters from 1.1.
               Expected: GT_BAD_PARAM.
            */
            stcType = STC_INVALID_ENUM_CNS;

            st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

            stcType = CPSS_EXMXPM_STC_INGRESS_E;

            /*
               1.1.3. Call with isReadyPtr [NULL] and other parameters from 1.1.
               Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, isReadyPtr = NULL", dev, port);
        }

        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /*
           1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                            and other parameters from 1.1.
           Expected: GT_BAD_PARAM.
        */
        port = CPSS_CPU_PORT_NUM_CNS;

        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

        /*
           1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                            and other parameters from 1.1.
           Expected: GT_OK.
        */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);
    }

    stcType = CPSS_EXMXPM_STC_INGRESS_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = STC_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcPortReadyForNewLimitGet(dev, port, stcType, &isReady);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcPortCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMXPM_STC_TYPE_ENT    stcType,
    OUT GT_U32                      *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcPortCntrGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                              CPSS_EXMXPM_STC_EGRESS_E]
                     and non-null cntrPtr.
    Expected: GT_OK.
    1.1.2. Call with out of range stcType [0x5AAAAAA5]
                     and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with cntrPtr [NULL]
                     and other parameters from  1.1.
    Expected: GT_BAD_PTR.
    1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                     and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = STC_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_STC_TYPE_ENT stcType = CPSS_EXMXPM_STC_INGRESS_E;
    GT_U32                   cntr    = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
               1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                                         CPSS_EXMXPM_STC_EGRESS_E]
                                and non-null cntrPtr.
               Expected: GT_OK.
            */

            /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
            stcType = CPSS_EXMXPM_STC_INGRESS_E;

            st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

            /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
            stcType = CPSS_EXMXPM_STC_EGRESS_E;

            st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

            /*
               1.1.2. Call with out of range stcType [0x5AAAAAA5]
                                and other params same as in 1.1.
               Expected: GT_BAD_PARAM.
            */
            stcType = STC_INVALID_ENUM_CNS;

            st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

            stcType = CPSS_EXMXPM_STC_EGRESS_E;

            /*
               1.1.3. Call with cntrPtr [NULL] 
                                and other parameters from  1.1.
               Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmStcPortCntrGet(dev, port, stcType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cntrPtr = NULL", dev, port);
        }

        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /*
           1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                            and other parameters from 1.1.
           Expected: GT_BAD_PARAM.
        */
        port = CPSS_CPU_PORT_NUM_CNS;

        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

        /*
           1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                            and other parameters from 1.1.
           Expected: GT_OK.
        */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);
    }

    stcType = CPSS_EXMXPM_STC_INGRESS_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = STC_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcPortCntrGet(dev, port, stcType, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcPortSampledPacketsCntrSet 
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMXPM_STC_TYPE_ENT    stcType,
    IN  GT_U32                      cntr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcPortSampledPacketsCntrSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                              CPSS_EXMXPM_STC_EGRESS_E],
                     cntr [0 / 0xFFFF].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmStcPortSampledPacketsCntrGet
                     with same stcType and non-null cntrPtr.
    Expected: GT_OK and same cntr.
    1.1.3. Call with out of range stcType [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with out of range cntr [65536]
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                     and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = STC_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_STC_TYPE_ENT stcType = CPSS_EXMXPM_STC_INGRESS_E;
    GT_U32                   cntr    = 0;
    GT_U32                   cntrGet = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
               1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                                         CPSS_EXMXPM_STC_EGRESS_E],
                                cntr [0 / 0xFFFF].
               Expected: GT_OK.
            */

            /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
            stcType = CPSS_EXMXPM_STC_INGRESS_E;
            cntr    = 0;

            st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stcType, cntr);

            /*
               1.1.2. Call cpssExMxPmStcPortSampledPacketsCntrGet with same stcType
                                                                       and non-null cntrPtr.
               Expected: GT_OK and same cntr.
            */
            st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntrGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmStcPortSampledPacketsCntrGet: %d, %d, %d", dev, port, stcType);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(cntr, cntrGet,
                       "get another cntr than was set: %d", dev);

            /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
            stcType = CPSS_EXMXPM_STC_EGRESS_E;
            cntr    = 0xFFFF;

            st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stcType, cntr);

            /*
               1.1.2. Call cpssExMxPmStcPortSampledPacketsCntrGet with same stcType
                                                                       and non-null cntrPtr.
               Expected: GT_OK and same cntr.
            */
            st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntrGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmStcPortSampledPacketsCntrGet: %d, %d, %d", dev, port, stcType);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(cntr, cntrGet,
                                   "get another cntr than was set: %d", dev);

            /*
               1.1.3. Call with out of range stcType [0x5AAAAAA5]
                                and other parameters from 1.1.
               Expected: GT_BAD_PARAM.
            */
            stcType = STC_INVALID_ENUM_CNS;
            
            st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

            stcType = CPSS_EXMXPM_STC_INGRESS_E;

            /*
               1.1.4. Call with cntr [65536]
                                and other parameters from 1.1.
               Expected: GT_OK.
            */
            cntr    = 65536;

            st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cntr = %d", dev, port, cntr);
        }

        stcType = CPSS_EXMXPM_STC_EGRESS_E;
        cntr    = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /*
            1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                             and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        port = CPSS_CPU_PORT_NUM_CNS;

        stcType = CPSS_EXMXPM_STC_INGRESS_E;
        
        st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

        /*
            1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                             and other parameters from 1.1.
            Expected: GT_OK.
        */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;
        
        st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);
    }

    stcType = CPSS_EXMXPM_STC_EGRESS_E;
    cntr    = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = STC_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcPortSampledPacketsCntrSet(dev, port, stcType, cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmStcPortSampledPacketsCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_EXMXPM_STC_TYPE_ENT    stcType,
    OUT GT_U32                      *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmStcPortSampledPacketsCntrGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                              CPSS_EXMXPM_STC_EGRESS_E]
                     and non-null cntrPtr.
    Expected: GT_OK.
    1.1.2. Call with out of range stcType [0x5AAAAAA5]
                     and non-null cntrPtr.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with cntrPtr [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                     and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = STC_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_STC_TYPE_ENT stcType = CPSS_EXMXPM_STC_INGRESS_E;
    GT_U32                   cntr    = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
               1.1.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E /
                                         CPSS_EXMXPM_STC_EGRESS_E]
                                and non-null cntrPtr.
               Expected: GT_OK.
            */

            /* Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] */
            stcType = CPSS_EXMXPM_STC_INGRESS_E;

            st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

            /* Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] */
            stcType = CPSS_EXMXPM_STC_EGRESS_E;

            st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);

            /*
               1.1.2. Call with out of range stcType [0x5AAAAAA5]
                                and non-null cntrPtr.
               Expected: GT_BAD_PARAM.
            */
            stcType = STC_INVALID_ENUM_CNS;

            st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

            stcType = CPSS_EXMXPM_STC_INGRESS_E;

            /*
                1.1.3. Call with cntrPtr [NULL]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cntrPtr = NULL", dev, port);
        }

        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /*
           1.4.1. Call with stcType [CPSS_EXMXPM_STC_INGRESS_E] (CPU port is NOT supported)
                            and other parameters from 1.1.
           Expected: GT_BAD_PARAM.
        */
        port = CPSS_CPU_PORT_NUM_CNS;

        stcType = CPSS_EXMXPM_STC_INGRESS_E;

        st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stcType);

        /*
           1.4.2. Call with stcType [CPSS_EXMXPM_STC_EGRESS_E] (CPU port is supported)
                            and other parameters from 1.1.
           Expected: GT_OK.
        */
        stcType = CPSS_EXMXPM_STC_EGRESS_E;

        st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stcType);
    }

    stcType = CPSS_EXMXPM_STC_INGRESS_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = STC_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmStcPortSampledPacketsCntrGet(dev, port, stcType, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmStc suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmStc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcIngressCountModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcIngressCountModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcReloadModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcReloadModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcPortLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcPortLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcPortReadyForNewLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcPortCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcPortSampledPacketsCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmStcPortSampledPacketsCntrGet) 
UTF_SUIT_END_TESTS_MAC(cpssExMxPmStc)

