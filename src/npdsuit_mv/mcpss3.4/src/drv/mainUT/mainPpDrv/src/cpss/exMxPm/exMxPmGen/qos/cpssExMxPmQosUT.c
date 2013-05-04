/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmQosUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmQos, that provides
*       CPSS ExMxPm Quality of Service  API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/exMxPm/exMxPmGen/qos/cpssExMxPmQos.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for virtual port id */
#define QOS_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define QOS_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosDsaTagQosProfileToQosParamMapSet
(   
    IN GT_U8                        devNum,
    IN GT_U32                       profileIndex,
    IN CPSS_EXMXPM_QOS_PARAM_STC    *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosDsaTagQosProfileToQosParamMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 127],
                   qosParam {tc [0 / 7],
                             dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                             up [0 / 7],
                             dscp [0 / 63],
                             exp [0 / 7] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosDsaTagQosProfileToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
    Expected: GT_OK and the same values as written.
    1.3. Check out of range profileIndex. Call with profileIndex [128] and other parameters from 1.1. 
    Expected: non GT_OK.
    1.4. Call with out of range qosParam.tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Check invalid enum. Call with qosParam.dp [0x5AAAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range qosParam.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.7. Call with out of range qosParam.dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
    1.8. Call with out-of-range qosParam.exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.9. Call with null qosParamPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8                     profile = 0;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;
    CPSS_EXMXPM_QOS_PARAM_STC qosParamGet;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with profileIndex [0 / 127],
                           qosParam {tc [0 / 7],
                                     dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                     up [0 / 7],
                                     dscp [0 / 63],
                                     exp [0 / 7] }.
            Expected: GT_OK.
        */

        /* Call with profileIndex [0] */
        profile = 0;

        qosParam.tc   = 0;
        qosParam.dp   = CPSS_DP_GREEN_E;
        qosParam.up   = 0;
        qosParam.dscp = 0;
        qosParam.exp  = 0;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /*
            1.2. Call cpssExMxPmQosDsaTagQosProfileToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
            Expected: GT_OK and the same values as written.
        */
        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(dev, profile, &qosParamGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosDsaTagQosProfileToQosParamMapGet: %d, %d", dev, profile);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                       "get another qosParamPtr->dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* Call with profileIndex [127] */
        profile = 127;

        qosParam.tc   = 7;
        qosParam.dp   = CPSS_DP_RED_E;
        qosParam.up   = 7;
        qosParam.dscp = 63;
        qosParam.exp  = 7;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /*
            1.2. Call cpssExMxPmQosDsaTagQosProfileToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
            Expected: GT_OK and the same values as written.
        */
        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(dev, profile, &qosParamGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosDsaTagQosProfileToQosParamMapGet: %d, %d", dev, profile);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                       "get another qosParamPtr->dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* 
            1.3. Check out of range profileIndex. Call with profileIndex [128] and other parameters from 1.1. 
            Expected: non GT_OK.
        */
        profile = 128;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        profile = 0;

        /*
            1.4. Call with out of range qosParam.tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParam->tc = %d", dev, qosParam.tc);

        qosParam.tc = 0;

        /* 
            1.5. Check invalid enum. Call with qosParam.dp [0x5AAAAAA5], other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        qosParam.dp = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, qosParam->dp = %d", dev, qosParam.dp);

        qosParam.dp = CPSS_DP_GREEN_E;

        /*
            1.6. Call with out of range qosParam.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        qosParam.up = CPSS_USER_PRIORITY_RANGE_CNS;
        
        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->up = %d", dev, qosParam.up);

        qosParam.up = 0;

        /*
            1.7. Call with out of range qosParam.dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        qosParam.dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParam->dscp= %d", dev, qosParam.dscp);

        qosParam.dscp = 0;

        /*
            1.8. Call with out-of-range qosParam.exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        qosParam.exp = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->exp = %d", dev, qosParam.exp);

        qosParam.exp = 0;
        
        /* 
            1.9. Call with null qosParamPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    profile = 0;

    qosParam.tc   = 0;
    qosParam.dp   = CPSS_DP_GREEN_E;
    qosParam.up   = 0;
    qosParam.dscp = 0;
    qosParam.exp  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(dev, profile, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosDsaTagQosProfileToQosParamMapGet
(   
    IN  GT_U8                       devNum,
    IN  GT_U32                      profileIndex,
    OUT CPSS_EXMXPM_QOS_PARAM_STC   *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosDsaTagQosProfileToQosParamMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 127], non NULL qosParam.
    Expected: GT_OK.
    1.2. Check out of range profileIndex. Call with profileIndex [128] and other parameters from 1.1. 
    Expected: non GT_OK.
    1.3. Check for null pointer. Call with qosParamPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8                     profile = 0;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with profileIndex [0 / 127], non NULL qosParam.
            Expected: GT_OK.
        */

        /* Call with profileIndex [0] */
        profile = 0;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(dev, profile, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /* Call with profileIndex [127] */
        profile = 127;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(dev, profile, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /* 
            1.2. Check out of range profileIndex. Call with profileIndex [128] and other parameters from 1.1. 
            Expected: non GT_OK.
        */
        profile = 128;

        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(dev, profile, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        profile = 0;
        
        /* 
            1.3. Check for null pointer. Call with qosParamPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(dev, profile, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    profile = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(dev, profile, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(dev, profile, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosMplsExpToDpMappingTableSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       exp,
    IN CPSS_DP_LEVEL_ENT            dp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosMplsExpToDpMappingTableSet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with exp [0 / 7], dp [CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosMplsExpToDpMappingTableGet with not NULL dpPtr
              and the same exp as in 1.1.
    Expected: GT_OK and the same dp as was set.
    1.3. Call with exp[8]
    Expected: NOT GT_OK.
    1.4. Call with dp [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32              exp   = 0;
    CPSS_DP_LEVEL_ENT   dp    = CPSS_DP_GREEN_E;
    CPSS_DP_LEVEL_ENT   dpGet = CPSS_DP_GREEN_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exp [0 / 7], dp [CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E].
            Expected: GT_OK.
        */
        /* iterate with exp = 0 */
        exp = 0;
        dp  = CPSS_DP_GREEN_E;

        st = cpssExMxPmQosMplsExpToDpMappingTableSet(dev, exp, dp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exp, dp);

        /*
            1.2. Call cpssExMxPmQosMplsExpToDpMappingTableGet with not NULL dpPtr and the same exp as in 1.1.
            Expected: GT_OK and the same dpPtr as was set.
        */
        st = cpssExMxPmQosMplsExpToDpMappingTableGet(dev, exp, &dpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosMplsExpToDpMappingTableGet: %d, %d", dev, exp);
        UTF_VERIFY_EQUAL1_STRING_MAC(dp, dpGet, "get another dp than was set: %d, %d", dev);

        /* iterate with exp = 7 */
        exp = 7;
        dp  = CPSS_DP_YELLOW_E;

        st = cpssExMxPmQosMplsExpToDpMappingTableSet(dev, exp, dp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exp, dp);

        /*
            1.2. Call cpssExMxPmQosMplsExpToDpMappingTableGet with not NULL dpPtr and the same exp as in 1.1.
            Expected: GT_OK and the same dpPtr as was set.
        */
        st = cpssExMxPmQosMplsExpToDpMappingTableGet(dev, exp, &dpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosMplsExpToDpMappingTableGet: %d, %d", dev, exp);
        UTF_VERIFY_EQUAL1_STRING_MAC(dp, dpGet, "get another dp than was set: %d, %d", dev);

        /*
            1.3. Call with exp[8]
            Expected: NOT GT_OK.
        */
        exp = 8;

        st = cpssExMxPmQosMplsExpToDpMappingTableSet(dev, exp, dp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        exp = 0;

        /*
            1.4. Call with dp [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        dp = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosMplsExpToDpMappingTableSet(dev, exp, dp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, exp, dp);
    }

    exp = 0;
    dp  = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosMplsExpToDpMappingTableSet(dev, exp, dp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosMplsExpToDpMappingTableSet(dev, exp, dp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosMplsExpToDpMappingTableGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      exp,
    OUT CPSS_DP_LEVEL_ENT           *dpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosMplsExpToDpMappingTableGet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with exp [0 / 7], not NULL dpPtr.
    Expected: GT_OK.
    1.2. Call with exp[8]
    Expected: NOT GT_OK.
    1.4. Call with dpPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32              exp = 0;
    CPSS_DP_LEVEL_ENT   dp  = CPSS_DP_GREEN_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exp [0 / 7], not NULL dpPtr.
            Expected: GT_OK.
        */
        exp = 0;

        st = cpssExMxPmQosMplsExpToDpMappingTableGet(dev, exp, &dp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        exp = 7;

        st = cpssExMxPmQosMplsExpToDpMappingTableGet(dev, exp, &dp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        /*
            1.2. Call with exp[8]
            Expected: NOT GT_OK.
        */
        exp = 8;

        st = cpssExMxPmQosMplsExpToDpMappingTableGet(dev, exp, &dp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        exp = 0;

        /*
            1.4. Call with dpPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosMplsExpToDpMappingTableGet(dev, exp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dpPtr = NULL", dev);
    }

    exp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosMplsExpToDpMappingTableGet(dev, exp, &dp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosMplsExpToDpMappingTableGet(dev, exp, &dp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosDscpToQosParamMapSet
(   
    IN GT_U8                        devNum,
    IN GT_U32                       dscp,    
    IN CPSS_EXMXPM_QOS_PARAM_STC   *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosDscpToQosParamMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dscp [0 / 63],
                   qosParam {tc [0 / 7],
                             dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                             up [0 / 7],
                             dscp [0 / 63],
                             exp [0 / 7] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosDscpToQosParamMapGet
    with non-NULL qosParamPtr, other params same as in 1.1.
    Expected: GT_OK and the same values as written (except for qosParam.dscp).
    1.3. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
    1.4. Call with out of range qosParam.tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Check invalid enum. Call with qosParam.dp [0x5AAAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range qosParam.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.7. Call with out of range qosParam.dscp [CPSS_DSCP_RANGE_CNS = 64] (should be ignored) and other parameters from 1.1.
    Expected: GT_OK.
    1.8. Call with out-of-range qosParam.exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.9. Call with null qosParamPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    dscp = 0;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;
    CPSS_EXMXPM_QOS_PARAM_STC qosParamGet;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with dscp [0 / 63],
                           qosParam {tc [0 / 7],
                                     dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                     up [0 / 7],
                                     dscp [0 / 63],
                                     exp [0 / 7] }.
            Expected: GT_OK.
        */

        /* Call with dscp [0] */
        dscp = 0;

        qosParam.tc   = 0;
        qosParam.dp   = CPSS_DP_GREEN_E;
        qosParam.up   = 0;
        qosParam.exp  = 0;

        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /*
            1.2. Call cpssExMxPmQosDscpToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
            Expected: GT_OK and the same values as written (except for qosParam.dscp).
        */
        st = cpssExMxPmQosDscpToQosParamMapGet(dev, dscp, &qosParamGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosDscpToQosParamMapGet: %d, %d", dev, dscp);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* Call with dscp [63] */
        dscp = 63;

        qosParam.tc   = 7;
        qosParam.dp   = CPSS_DP_RED_E;
        qosParam.up   = 7;
        qosParam.exp  = 7;

        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /*
            1.2. Call cpssExMxPmQosDscpToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
            Expected: GT_OK and the same values as written (except for qosParam.dscp).
        */
        st = cpssExMxPmQosDscpToQosParamMapGet(dev, dscp, &qosParamGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosDscpToQosParamMapGet: %d, %d", dev, dscp);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* 
            1.3. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        dscp = 0;

        /*
            1.4. Call with out of range qosParam.tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParam->tc = %d", dev, qosParam.tc);

        qosParam.tc = 0;

        /* 
            1.5. Check invalid enum. Call with qosParam.dp [0x5AAAAAA5], other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        qosParam.dp = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, qosParam->dp = %d", dev, qosParam.dp);

        qosParam.dp = CPSS_DP_GREEN_E;

        /*
            1.6. Call with out of range qosParam.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        qosParam.up = CPSS_USER_PRIORITY_RANGE_CNS;
        
        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->up = %d", dev, qosParam.up);

        qosParam.up = 0;

        /*
            1.7. Call with out of range qosParam.dscp [CPSS_DSCP_RANGE_CNS = 64] (should be ignored) and other parameters from 1.1.
            Expected: GT_OK.
        */
        qosParam.dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParam->dscp= %d", dev, qosParam.dscp);

        qosParam.dscp = 0;

        /*
            1.8. Call with out-of-range qosParam.exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        qosParam.exp = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->exp = %d", dev, qosParam.exp);

        qosParam.exp = 0;
        
        /* 
            1.9. Call with null qosParamPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    dscp = 0;

    qosParam.tc  = 0;
    qosParam.dp  = CPSS_DP_GREEN_E;
    qosParam.up  = 0;
    qosParam.exp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosDscpToQosParamMapSet(dev, dscp, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosDscpToQosParamMapGet
(   
    IN GT_U8                        devNum,
    IN GT_U32                       dscp,    
    OUT CPSS_EXMXPM_QOS_PARAM_STC   *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosDscpToQosParamMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dscp [0 / 63], non NULL qosParam.
    Expected: GT_OK.
    1.2. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
    1.3. Check for null pointer. Call with qosParamPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8                     dscp = 0;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with dscp [0 / 63], non NULL qosParam.
            Expected: GT_OK.
        */

        /* Call with dscp [0] */
        dscp = 0;

        st = cpssExMxPmQosDscpToQosParamMapGet(dev, dscp, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* Call with dscp [63] */
        dscp = 63;

        st = cpssExMxPmQosDscpToQosParamMapGet(dev, dscp, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* 
            1.2. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosDscpToQosParamMapGet(dev, dscp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        dscp = 0;
        
        /* 
            1.3. Check for null pointer. Call with qosParamPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosDscpToQosParamMapGet(dev, dscp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    dscp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosDscpToQosParamMapGet(dev, dscp, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosDscpToQosParamMapGet(dev, dscp, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosExpToQosParamMapSet
(   
    IN GT_U8                        devNum,
    IN GT_U32                       exp,
    IN CPSS_EXMXPM_QOS_PARAM_STC    *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosExpToQosParamMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with exp [0 / 7],
                   qosParam {tc [0 / 7],
                             dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                             up [0 / 7],
                             dscp [0 / 63],
                             exp [0 / 7] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosExpToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
    Expected: GT_OK and the same values as written (except for qosParam. exp).
    1.3. Check out of range exp. Call with exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.4. Call with out-of-range qosParam.tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Check invalid enum. Call with qosParam.dp [0x5AAAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.
    1.6. Call with out-of-range qosParam.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.7. Call with out-of-range qosParam.dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
    1.8. Call with out-of-range qosParam.exp [CPSS_EXP_RANGE_CNS = 8] (should be ignored) and other parameters from 1.1.
    Expected: GT_OK.
    1.9. Call with null qosParamPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    exp = 0;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;
    CPSS_EXMXPM_QOS_PARAM_STC qosParamGet;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with exp [0 / 7],
                           qosParam {tc [0 / 7],
                                     dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                     up [0 / 7],
                                     dscp [0 / 63],
                                     exp [0 / 7] }.
            Expected: GT_OK.
        */

        /* Call with exp [0] */
        exp = 0;

        qosParam.tc   = 0;
        qosParam.dp   = CPSS_DP_GREEN_E;
        qosParam.up   = 0;
        qosParam.dscp = 0;

        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        /*
            1.2. Call cpssExMxPmQosExpToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
            Expected: GT_OK and the same values as written (except for qosParam. exp).
        */
        st = cpssExMxPmQosExpToQosParamMapGet(dev, exp, &qosParamGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosExpToQosParamMapGet: %d, %d", dev, exp);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                       "get another qosParamPtr->dscp than was set: %d", dev);

        /* Call with exp [7] */
        exp = 7;

        qosParam.tc   = 7;
        qosParam.dp   = CPSS_DP_RED_E;
        qosParam.up   = 7;
        qosParam.dscp = 63;

        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        /*
            1.2. Call cpssExMxPmQosExpToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
            Expected: GT_OK and the same values as written (except for qosParam. exp).
        */
        st = cpssExMxPmQosExpToQosParamMapGet(dev, exp, &qosParamGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosExpToQosParamMapGet: %d, %d", dev, exp);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                       "get another qosParamPtr->dscp than was set: %d", dev);

        /* 
            1.3. Check out of range exp. Call with exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        exp = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        exp = 0;

        /*
            1.4. Call with out of range qosParam.tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParam->tc = %d", dev, qosParam.tc);

        qosParam.tc = 0;

        /* 
            1.5. Check invalid enum. Call with qosParam.dp [0x5AAAAAA5], other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        qosParam.dp = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, qosParam->dp = %d", dev, qosParam.dp);

        qosParam.dp = CPSS_DP_GREEN_E;

        /*
            1.6. Call with out of range qosParam.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        qosParam.up = CPSS_USER_PRIORITY_RANGE_CNS;
        
        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->up = %d", dev, qosParam.up);

        qosParam.up = 0;

        /*
            1.7. Call with out of range qosParam.dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: not GT_OK.
        */
        qosParam.dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParam->dscp= %d", dev, qosParam.dscp);

        qosParam.dscp = 0;

        /*
            1.8. Call with out-of-range qosParam.exp [CPSS_EXP_RANGE_CNS = 8] (should be ignored) and other parameters from 1.1.
            Expected: GT_OK.
        */
        qosParam.exp = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->exp = %d", dev, qosParam.exp);

        qosParam.exp = 0;
        
        /* 
            1.9. Call with null qosParamPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    exp = 0;

    qosParam.tc   = 0;
    qosParam.dp   = CPSS_DP_GREEN_E;
    qosParam.up   = 0;
    qosParam.dscp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosExpToQosParamMapSet(dev, exp, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosExpToQosParamMapGet
(   
    IN  GT_U8                       devNum,
    IN  GT_U32                      exp,
    OUT CPSS_EXMXPM_QOS_PARAM_STC   *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosExpToQosParamMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with exp [0 / 7], non NULL qosParam.
    Expected: GT_OK.
    1.2. Check out of range exp. Call with exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.3. Check for null pointer. Call with qosParamPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8                     exp = 0;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with exp [0 / 7], non NULL qosParam.
            Expected: GT_OK.
        */

        /* Call with exp [0] */
        exp = 0;

        st = cpssExMxPmQosExpToQosParamMapGet(dev, exp, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        /* Call with exp [7] */
        exp = 7;

        st = cpssExMxPmQosExpToQosParamMapGet(dev, exp, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        /* 
            1.2. Check out of range exp. Call with exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        exp = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmQosExpToQosParamMapGet(dev, exp, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, exp);

        exp = 0;
        
        /* 
            1.3. Check for null pointer. Call with qosParamPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosExpToQosParamMapGet(dev, exp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    exp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosExpToQosParamMapGet(dev, exp, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosExpToQosParamMapGet(dev, exp, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosUpToQosParamMapSet
(   
    IN GT_U8                        devNum,
    IN GT_U32                       up,
    IN CPSS_EXMXPM_QOS_PARAM_STC    *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosUpToQosParamMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with up [0 / 7],
                   qosParam {tc [0 / 7],
                             dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                             up [0 / 7],
                             dscp [0 / 63],
                             exp [0 / 7] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosUpToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
    Expected: GT_OK and the same values as written (except for qosParam. up).
    1.3. Check out of range up. Call with up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.4. Call with out-of-range qosParam.tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Check invalid enum. Call with qosParam.dp [0x5AAAAAA5], other params same as in 1.1
    Expected: GT_BAD_PARAM.
    1.6. Call with out-of-range qosParam.dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
    1.7. Call with out-of-range qosParam.exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.8. Call with null qosParamPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    up = 0;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;
    CPSS_EXMXPM_QOS_PARAM_STC qosParamGet;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with dscp [0 / 63],
                           qosParam {tc [0 / 7],
                                     dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                     up [0 / 7],
                                     dscp [0 / 63],
                                     exp [0 / 7] }.
            Expected: GT_OK.
        */

        /* Call with up [0] */
        up = 0;

        qosParam.tc   = 0;
        qosParam.dp   = CPSS_DP_GREEN_E;
        qosParam.up   = 0;
        qosParam.dscp = 0;
        qosParam.exp  = 0;

        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        /*
            1.2. Call cpssExMxPmQosUpToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
            Expected: GT_OK and the same values as written (except for qosParam.dscp).
        */
        st = cpssExMxPmQosUpToQosParamMapGet(dev, up, &qosParamGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUpToQosParamMapGet: %d, %d", dev, up);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* Call with up [7] */
        up = 7;

        qosParam.tc   = 7;
        qosParam.dp   = CPSS_DP_RED_E;
        qosParam.up   = 7;
        qosParam.dscp = 63;
        qosParam.exp  = 7;

        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        /*
            1.2. Call cpssExMxPmQosUpToQosParamMapGet with non-NULL qosParamPtr, other params same as in 1.1.
            Expected: GT_OK and the same values as written (except for qosParam.dscp).
        */
        st = cpssExMxPmQosUpToQosParamMapGet(dev, up, &qosParamGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUpToQosParamMapGet: %d, %d", dev, up);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* 
            1.3. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        up = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        up = 0;

        /*
            1.4. Call with out of range qosParam.tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParam->tc = %d", dev, qosParam.tc);

        qosParam.tc = 0;

        /* 
            1.5. Check invalid enum. Call with qosParam.dp [0x5AAAAAA5], other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        qosParam.dp = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, qosParam->dp = %d", dev, qosParam.dp);

        qosParam.dp = CPSS_DP_GREEN_E;

        /*
            1.6. Call with out of range qosParam.dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParam->dscp= %d", dev, qosParam.dscp);

        qosParam.dscp = 0;

        /*
            1.7. Call with out-of-range qosParam.exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        qosParam.exp = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->exp = %d", dev, qosParam.exp);

        qosParam.exp = 0;
        
        /* 
            1.8. Call with null qosParamPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    up = 0;

    qosParam.tc  = 0;
    qosParam.dp  = CPSS_DP_GREEN_E;
    qosParam.up  = 0;
    qosParam.exp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosUpToQosParamMapSet(dev, up, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosUpToQosParamMapGet
(   
    IN  GT_U8                       devNum,
    IN  GT_U32                      up,
    OUT CPSS_EXMXPM_QOS_PARAM_STC   *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosUpToQosParamMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with up [0 / 7], non NULL qosParam.
    Expected: GT_OK.
    1.2. Check out of range up. Call with up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.3. Check for null pointer. Call with qosParamPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8                     up = 0;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with up [0 / 7], non NULL qosParam.
            Expected: GT_OK.
        */

        /* Call with up [0] */
        up = 0;

        st = cpssExMxPmQosUpToQosParamMapGet(dev, up, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        /* Call with up [7] */
        up = 7;

        st = cpssExMxPmQosUpToQosParamMapGet(dev, up, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        /* 
            1.2. Check out of range up. Call with up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        up = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmQosUpToQosParamMapGet(dev, up, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        up = 0;
        
        /* 
            1.3. Check for null pointer. Call with qosParamPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosUpToQosParamMapGet(dev, up, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    up = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosUpToQosParamMapGet(dev, up, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosUpToQosParamMapGet(dev, up, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosDscpToDscpMapSet
(   
    IN GT_U8    devNum,
    IN GT_U32   dscp,    
    IN GT_U32   newDscp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosDscpToDscpMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dscp [0 / 63 / 5],
                   newDscp [63 / 0 / 5].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosDscpToDscpMapGet with non-NULL newDscpPtr, other params same as in 1.1.
    Expected: GT_OK and the same value as written.
    1.3. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
    1.4. Check out of range newDscp. Call with newDscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32    dscp    = 0;    
    GT_U32    newDscp = 0;
    GT_U32    dscpGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with dscp [0 / 63 / 5],
                          newDscp [63 / 0 / 5].
            Expected: GT_OK.
        */

        /* Call with dscp [0], newDscp [63] */
        dscp    = 0;
        newDscp = 63;
        
        st = cpssExMxPmQosDscpToDscpMapSet(dev, dscp, newDscp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, dscp, newDscp);

        /*
            1.2. Call cpssExMxPmQosDscpToDscpMapGet with non-NULL newDscpPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, &dscpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosDscpToDscpMapGet: %d, %d", dev, dscp);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(newDscp, dscpGet,
                       "get another newDscp than was set: %d, %d", dev, dscp);

        /* Call with dscp [63], newDscp [0] */
        dscp    = 63;
        newDscp = 0;
        
        st = cpssExMxPmQosDscpToDscpMapSet(dev, dscp, newDscp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, dscp, newDscp);

        /*
            1.2. Call cpssExMxPmQosDscpToDscpMapGet with non-NULL newDscpPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, &dscpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosDscpToDscpMapGet: %d, %d", dev, dscp);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(newDscp, dscpGet,
                       "get another newDscp than was set: %d, %d", dev, dscp);

        /* Call with dscp [5], newDscp [5] */
        dscp    = 5;
        newDscp = 5;
        
        st = cpssExMxPmQosDscpToDscpMapSet(dev, dscp, newDscp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, dscp, newDscp);

        /*
            1.2. Call cpssExMxPmQosDscpToDscpMapGet with non-NULL newDscpPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, &dscpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosDscpToDscpMapGet: %d, %d", dev, dscp);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(newDscp, dscpGet,
                       "get another newDscp than was set: %d, %d", dev, dscp);

        /* 
            1.3. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        dscp = CPSS_DSCP_RANGE_CNS;
        
        st = cpssExMxPmQosDscpToDscpMapSet(dev, dscp, newDscp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        dscp = 0;

        /* 
            1.4. Check out of range newDscp. Call with newDscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        newDscp = CPSS_DSCP_RANGE_CNS;
        
        st = cpssExMxPmQosDscpToDscpMapSet(dev, dscp, newDscp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, newDscp = %d", dev, newDscp);
    }

    dscp    = 0;
    newDscp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosDscpToDscpMapSet(dev, dscp, newDscp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosDscpToDscpMapSet(dev, dscp, newDscp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosDscpToDscpMapGet
(   
    IN  GT_U8    devNum,
    IN  GT_U32   dscp,    
    OUT GT_U32   *newDscpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosDscpToDscpMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dscp [0 / 63], non-NULL newDscpPtr.
    Expected: GT_OK.
    1.2. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
    1.3. Check for null pointer. Call with newDscpPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  dscp    = 0;
    GT_U32  newDscp = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with dscp [0 / 63], non-NULL newDscpPtr.
            Expected: GT_OK.
        */

        /* Call with dscp [0] */
        dscp = 0;

        st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, &newDscp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* Call with dscp [63] */
        dscp = 63;

        st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, &newDscp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* 
            1.2. Check out of range dscp. Call with dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, &newDscp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        dscp = 0;

        /*
            1.3. Check for null pointer. Call with newDscpPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, newDscpPtr = NULL", dev);
    }

    dscp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, &newDscp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosDscpToDscpMapGet(dev, dscp, &newDscp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosUpToUpMapSet
(   
    IN GT_U8    devNum,
    IN GT_U32   up,    
    IN GT_U32   newUp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosUpToUpMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with up [0 / 7 / 5], newUp [7 / 0 / 5].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosUpToUpMapGet with non-NULL newUpPtr, other params same as in 1.1.
    Expected: GT_OK and the same value as written.
    1.3. Check out of range up. Call with up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.4. Check out of range newUp. Call with newUp [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32     up    = 0;    
    GT_U32     newUp = 0;
    GT_U32     upGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with up [0 / 7 / 5], newUp [7 / 0 / 5].
            Expected: GT_OK.
        */

        /* Call with up [0], newUp [7] */
        up    = 0;
        newUp = 7;
        
        st = cpssExMxPmQosUpToUpMapSet(dev, up, newUp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, up, newUp);

        /*
            1.2. Call cpssExMxPmQosUpToUpMapGet with non-NULL newUpPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosUpToUpMapGet(dev, up, &upGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUpToUpMapGet: %d, %d", dev, up);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(newUp, upGet,
                       "get another newDscp than was set: %d, %d", dev, up);

        /* Call with up [7], newUp [0] */
        up    = 7;
        newUp = 0;
        
        st = cpssExMxPmQosUpToUpMapSet(dev, up, newUp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, up, newUp);

        /*
            1.2. Call cpssExMxPmQosUpToUpMapGet with non-NULL newUpPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosUpToUpMapGet(dev, up, &upGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUpToUpMapGet: %d, %d", dev, up);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(newUp, upGet,
                       "get another newDscp than was set: %d, %d", dev, up);

        /* Call with up [5], newUp [5] */
        up    = 5;
        newUp = 5;
        
        st = cpssExMxPmQosUpToUpMapSet(dev, up, newUp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, up, newUp);

        /*
            1.2. Call cpssExMxPmQosUpToUpMapGet with non-NULL newUpPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosUpToUpMapGet(dev, up, &upGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUpToUpMapGet: %d, %d", dev, up);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(newUp, upGet,
                       "get another newDscp than was set: %d, %d", dev, up);

        /* 
            1.3. Check out of range up. Call with up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        up = CPSS_USER_PRIORITY_RANGE_CNS;
        
        st = cpssExMxPmQosUpToUpMapSet(dev, up, newUp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        up = 0;

        /* 
            1.4. Check out of range newUp. Call with newUp [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        newUp = CPSS_USER_PRIORITY_RANGE_CNS;
        
        st = cpssExMxPmQosUpToUpMapSet(dev, up, newUp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, newDscp = %d", dev, newUp);
    }

    up    = 0;
    newUp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosUpToUpMapSet(dev, up, newUp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosUpToUpMapSet(dev, up, newUp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosUpToUpMapGet
(   
    IN  GT_U8   devNum,
    IN  GT_U32  up,    
    OUT GT_U32  *newUpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosUpToUpMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with up [0 / 7], non-NULL newUpPtr.
    Expected: GT_OK.
    1.2. Check out of range up. Call with up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
    1.3. Check for null pointer. Call with newUpPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32       up    = 0;
    GT_U32       newUp = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with up [0 / 7], non-NULL newUpPtr.
            Expected: GT_OK.
        */

        /* Call with up [0] */
        up = 0;

        st = cpssExMxPmQosUpToUpMapGet(dev, up, &newUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        /* Call with up [7] */
        up = 7;

        st = cpssExMxPmQosUpToUpMapGet(dev, up, &newUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        /* 
            1.2. Check out of range up. Call with up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
            Expected: non GT_OK.
        */
        up = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmQosUpToUpMapGet(dev, up, &newUp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        up = 0;

        /*
            1.3. Check for null pointer. Call with newUpPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosUpToUpMapGet(dev, up, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, newUpPtr = NULL", dev);
    }

    up = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosUpToUpMapGet(dev, up, &newUp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosUpToUpMapGet(dev, up, &newUp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosCfiToDpMapSet
(   
    IN GT_U8                devNum,
    IN GT_U32               cfi,    
    IN CPSS_DP_LEVEL_ENT    dp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosCfiToDpMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cfi [0 / 1], dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosCfiToDpMapGet with non-NULL dpPtr, other params same as in 1.1.
    Expected: GT_OK and the same value as written.
    1.3. Call with invalid cfi [2] and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with invalid dp [0x5AAAAAA5] and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32            cfi   = 0;
    CPSS_DP_LEVEL_ENT dp    = CPSS_DP_GREEN_E;
    CPSS_DP_LEVEL_ENT dpGet = CPSS_DP_GREEN_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with cfi [0 / 1], dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E].
           sExpected: GT_OK.
        */

        /* Call with cfi [0], dp [CPSS_DP_GREEN_E] */
        cfi = 0;
        dp  = CPSS_DP_GREEN_E;
        
        st = cpssExMxPmQosCfiToDpMapSet(dev, cfi, dp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cfi, dp);

        /*
            1.2. Call cpssExMxPmQosCfiToDpMapGet with non-NULL dpPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosCfiToDpMapGet(dev, cfi, &dpGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosCfiToDpMapGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dp, dpGet,
                       "get another dp than was set: %d", dev);

        /* Call with cfi [1], dp [CPSS_DP_RED_E] */
        cfi = 1;
        dp  = CPSS_DP_RED_E;
        
        st = cpssExMxPmQosCfiToDpMapSet(dev, cfi, dp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cfi, dp);

        /*
            1.2. Call cpssExMxPmQosCfiToDpMapGet with non-NULL dpPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosCfiToDpMapGet(dev, cfi, &dpGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosCfiToDpMapGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dp, dpGet,
                       "get another dp than was set: %d", dev);

        /*
            1.3. Call with invalid cfi [2] and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cfi = 2;

        st = cpssExMxPmQosCfiToDpMapSet(dev, cfi, dp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cfi);

        cfi = 0;

        /*
            1.4. Call with invalid dp [0x5AAAAAA5] and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dp = QOS_INVALID_ENUM_CNS;
        
        st = cpssExMxPmQosCfiToDpMapSet(dev, cfi, dp);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dp = %d", dev, dp);
    }

    cfi = 0;
    dp  = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosCfiToDpMapSet(dev, cfi, dp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosCfiToDpMapSet(dev, cfi, dp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosCfiToDpMapGet
(   
    IN  GT_U8                devNum,
    IN  GT_U32               cfi,    
    OUT CPSS_DP_LEVEL_ENT    *dpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosCfiToDpMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cfi [0 / 1], non-NULL dpPtr.
    Expected: GT_OK.
    1.2. Call with invalid cfi [2] and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with dpPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32            cfi = 0;
    CPSS_DP_LEVEL_ENT dp  = CPSS_DP_GREEN_E;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
           1.1. Call with cfi [0 / 1], non-NULL dpPtr.
           Expected: GT_OK.
        */

        /* Call with cfi [0] */
        cfi = 0;
        
        st = cpssExMxPmQosCfiToDpMapGet(dev, cfi, &dp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cfi);

        /* Call with cfi [1] */
        cfi = 1;
        
        st = cpssExMxPmQosCfiToDpMapGet(dev, cfi, &dp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cfi);

        /*
            1.2. Call with invalid cfi [2] and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cfi = 2;

        st = cpssExMxPmQosCfiToDpMapGet(dev, cfi, &dp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cfi);

        cfi = 0;

        /*
            1.3. Call with dpPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosCfiToDpMapGet(dev, cfi, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dpPtr = NULL", dev);
    }

    cfi = 0;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosCfiToDpMapGet(dev, cfi, &dp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosCfiToDpMapGet(dev, cfi, &dp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortCfiToDpMapEnableSet
(   
    IN GT_U8      devNum,
    IN GT_U8      port,    
    IN GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortCfiToDpMapEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortCfiToDpMapEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same value as written.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmQosPortCfiToDpMapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortCfiToDpMapEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortCfiToDpMapEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmQosPortCfiToDpMapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortCfiToDpMapEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortCfiToDpMapEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortCfiToDpMapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortCfiToDpMapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortCfiToDpMapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortCfiToDpMapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortCfiToDpMapEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortCfiToDpMapEnableGet
(   
    IN GT_U8      devNum,
    IN GT_U8      port,    
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortCfiToDpMapEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
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
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortCfiToDpMapEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortTrustEnableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        port,
    IN CPSS_EXMXPM_QOS_PARAM_ENT    trustParamType,
    IN GT_BOOL                      enable 
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortTrustEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with trustParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                                     CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                                     CPSS_EXMXPM_QOS_PARAM_EXP_E],
                     enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.3. Call with trustParamType [CPSS_EXMXPM_QOS_PARAM_DP_E /
                                     CPSS_EXMXPM_QOS_PARAM_TC_E] (ignored)
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call with invalid trustParamType [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_QOS_PARAM_ENT   trustParamType = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;
    GT_BOOL                     enable         = GT_FALSE;


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
                1.1.1. Call with trustParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                                                 CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                                                 CPSS_EXMXPM_QOS_PARAM_EXP_E],
                                 enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE], trustParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E] */
            enable         = GT_FALSE;
            trustParamType = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trustParamType, enable);

            /* Call with enable [GT_FALSE], trustParamType [CPSS_EXMXPM_QOS_PARAM_DSCP_E] */
            trustParamType = CPSS_EXMXPM_QOS_PARAM_DSCP_E;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trustParamType, enable);

            /* Call with enable [GT_FALSE], trustParamType [CPSS_EXMXPM_QOS_PARAM_EXP_E] */
            trustParamType = CPSS_EXMXPM_QOS_PARAM_EXP_E;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trustParamType, enable);


            /* Call with enable [GT_TRUE], trustParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E] */
            enable         = GT_TRUE;
            trustParamType = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trustParamType, enable);

            /* Call with enable [GT_TRUE], trustParamType [CPSS_EXMXPM_QOS_PARAM_DSCP_E] */
            trustParamType = CPSS_EXMXPM_QOS_PARAM_DSCP_E;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trustParamType, enable);

            /* Call with enable [GT_TRUE], trustParamType [CPSS_EXMXPM_QOS_PARAM_EXP_E] */
            trustParamType = CPSS_EXMXPM_QOS_PARAM_EXP_E;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trustParamType, enable);

            /*
                1.1.3. Call with trustParamType [CPSS_EXMXPM_QOS_PARAM_DP_E /
                                                 CPSS_EXMXPM_QOS_PARAM_TC_E] (ignored)
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */

            /* Call with  trustParamType [CPSS_EXMXPM_QOS_PARAM_DP_E] */
            trustParamType = CPSS_EXMXPM_QOS_PARAM_DP_E;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trustParamType);

            /* Call with  trustParamType [CPSS_EXMXPM_QOS_PARAM_TC_E] */
            trustParamType = CPSS_EXMXPM_QOS_PARAM_TC_E;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trustParamType);

            trustParamType = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

            /* 
                1.1.4. Call with invalid trustParamType [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            trustParamType = QOS_INVALID_ENUM_CNS;

            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trustParamType);
        }

        enable         = GT_TRUE;
        trustParamType = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable         = GT_TRUE;
    trustParamType = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortTrustEnableSet(dev, port, trustParamType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultTcSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U32   tc
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultTcSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with tc [0 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortDefaultTcGet with non-NULL tcPtr.
    Expected: GT_OK and the same value as written.
    1.1.3. Call with out-of-range tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_U32   tc    = 0;
    GT_U32   tcGet = 0;

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
                1.1.1. Call with tc [0 / 7].
                Expected: GT_OK.
            */

            /* Call with tc [0] */
            tc = 0;

            st = cpssExMxPmQosPortDefaultTcSet(dev, port, tc);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultTcGet with non-NULL tcPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tcGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultTcGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(tc, tcGet,
                       "get another tc than was set: %d, %d", dev, port);

            /* Call with tc [7] */
            tc = 7;

            st = cpssExMxPmQosPortDefaultTcSet(dev, port, tc);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultTcGet with non-NULL tcPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tcGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultTcGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(tc, tcGet,
                       "get another tc than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out-of-range tc [CPSS_TC_RANGE_CNS = 8] and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPmQosPortDefaultTcSet(dev, port, tc);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);
        }

        tc = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultTcSet(dev, port, tc);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultTcSet(dev, port, tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultTcSet(dev, port, tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultTcSet(dev, port, tc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultTcSet(dev, port, tc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultTcGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_U32  *tcPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultTcGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL tcPtr.
    Expected: GT_OK.
    1.1.2. Call with tcPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_U32   tc = 0;
    
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
                1.1.1. Call with non-NULL tcPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tc);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with tcPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tc);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, tcPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tc);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultTcGet(dev, port, &tc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultUpSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U32   up
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultUpSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with up [0 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortDefaultUpGet with non-NULL upPtr.
    Expected: GT_OK and the same value as written.
    1.1.3. Call with out-of-range up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_U32   up    = 0;
    GT_U32   upGet = 0;

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
                1.1.1. Call with up [0 / 7].
                Expected: GT_OK.
            */

            /* Call with up [0] */
            up = 0;

            st = cpssExMxPmQosPortDefaultUpSet(dev, port, up);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, up);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultUpGet with non-NULL upPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultUpGet(dev, port, &upGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultUpGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(up, upGet,
                       "get another up than was set: %d, %d", dev, port);

            /* Call with up [7] */
            up = 7;

            st = cpssExMxPmQosPortDefaultUpSet(dev, port, up);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, up);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultUpGet with non-NULL upPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultUpGet(dev, port, &upGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultUpGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(up, upGet,
                       "get another up than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out-of-range up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters from 1.1.
                Expected: non GT_OK.
            */
            up = CPSS_USER_PRIORITY_RANGE_CNS;

            st = cpssExMxPmQosPortDefaultUpSet(dev, port, up);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, up);
        }

        up = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultUpSet(dev, port, up);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultUpSet(dev, port, up);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultUpSet(dev, port, up);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    up = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultUpSet(dev, port, up);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultUpSet(dev, port, up);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultUpGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_U32   *upPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultUpGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL upPtr.
    Expected: GT_OK.
    1.1.2. Call with upPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_U32   up = 0;
    
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
                1.1.1. Call with non-NULL upPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortDefaultUpGet(dev, port, &up);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with upPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortDefaultUpGet(dev, port, &up);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, upPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultUpGet(dev, port, &up);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultUpGet(dev, port, &up);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultUpGet(dev, port, &up);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultUpGet(dev, port, &up);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultUpGet(dev, port, &up);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultDscpSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U32   dscp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultDscpSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with dscp [0 / 63].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortDefaultDscpGet
    with non-NULL dscpPtr.
    Expected: GT_OK and the same value as written.
    1.1.3. Call with out of range dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_U32   dscp    = 0;
    GT_U32   dscpGet = 0;

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
                1.1.1. Call with dscp [0 / 7].
                Expected: GT_OK.
            */

            /* Call with dscp [0] */
            dscp = 0;

            st = cpssExMxPmQosPortDefaultDscpSet(dev, port, dscp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dscp);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultDscpGet with non-NULL dscpPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscpGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultDscpGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(dscp, dscpGet,
                       "get another dscp than was set: %d, %d", dev, port);

            /* Call with dscp [63] */
            dscp = 63;

            st = cpssExMxPmQosPortDefaultDscpSet(dev, port, dscp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dscp);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultDscpGet with non-NULL dscpPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscpGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultDscpGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(dscp, dscpGet,
                       "get another dscp than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out-of-range dscp [PSS_DSCP_RANGE_CNS = 64] and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            dscp = CPSS_DSCP_RANGE_CNS;

            st = cpssExMxPmQosPortDefaultDscpSet(dev, port, dscp);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dscp);
        }

        dscp = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultDscpSet(dev, port, dscp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultDscpSet(dev, port, dscp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultDscpSet(dev, port, dscp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    dscp = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultDscpSet(dev, port, dscp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultDscpSet(dev, port, dscp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultDscpGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_U32   *dscpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultDscpGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL dscpPtr.
    Expected: GT_OK.
    1.1.2. Call with dscpPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_U32   dscp = 0;
    
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
                1.1.1. Call with non-NULL dscpPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with dscpPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, dscpPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultDscpGet(dev, port, &dscp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultDpSet
(
    IN GT_U8                devNum,
    IN GT_U8                port,
    IN CPSS_DP_LEVEL_ENT    dp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultDpSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortDefaultDpGet with non-NULL dpPtr.
    Expected: GT_OK and the same value as written.
    1.1.3. Call with out of range dp [0x5AAAAAA5] and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    CPSS_DP_LEVEL_ENT   dp    = CPSS_DP_GREEN_E;
    CPSS_DP_LEVEL_ENT   dpGet = CPSS_DP_GREEN_E;

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
                1.1.1. Call with dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E].
                Expected: GT_OK.
            */

            /* Call with dp [CPSS_DP_GREEN_E] */
            dp = CPSS_DP_GREEN_E;

            st = cpssExMxPmQosPortDefaultDpSet(dev, port, dp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dp);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultDpGet with non-NULL dpPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dpGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultDpGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(dp, dpGet,
                       "get another dp than was set: %d, %d", dev, port);

            /* Call with dp [CPSS_DP_RED_E] */
            dp = CPSS_DP_RED_E;

            st = cpssExMxPmQosPortDefaultDpSet(dev, port, dp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dp);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultDpGet with non-NULL dpPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dpGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultDpGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(dp, dpGet,
                       "get another dp than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range dp [0x5AAAAAA5] and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            dp = QOS_INVALID_ENUM_CNS;

            st = cpssExMxPmQosPortDefaultDpSet(dev, port, dp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, dp);
        }

        dp = CPSS_DP_GREEN_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultDpSet(dev, port, dp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultDpSet(dev, port, dp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultDpSet(dev, port, dp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    dp = CPSS_DP_GREEN_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultDpSet(dev, port, dp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultDpSet(dev, port, dp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultDpGet
(
    IN  GT_U8               devNum,
    IN  GT_U8               port,
    OUT CPSS_DP_LEVEL_ENT   *dpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultDpGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL dpPtr.
    Expected: GT_OK.
    1.1.2. Call with dpPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    CPSS_DP_LEVEL_ENT   dp = CPSS_DP_GREEN_E;
    
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
                1.1.1. Call with non-NULL dpPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with dpPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, dpPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultDpGet(dev, port, &dp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultExpSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U32   exp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultExpSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with exp [0 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortDefaultExpGet with non-NULL expPtr.
    Expected: GT_OK and the same value as written.
    1.1.3. Call with out-of-range exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_U32   exp    = 0;
    GT_U32   expGet = 0;

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
                1.1.1. Call with exp [0 / 7].
                Expected: GT_OK.
            */

            /* Call with exp [0] */
            exp = 0;

            st = cpssExMxPmQosPortDefaultExpSet(dev, port, exp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, exp);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultExpGet with non-NULL expPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultExpGet(dev, port, &expGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultExpGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(exp, expGet,
                       "get another exp than was set: %d, %d", dev, port);

            /* Call with exp [7] */
            exp = 7;

            st = cpssExMxPmQosPortDefaultExpSet(dev, port, exp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, exp);

            /*
                1.1.2. Call cpssExMxPmQosPortDefaultExpGet with non-NULL expPtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDefaultExpGet(dev, port, &expGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDefaultExpGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(exp, expGet,
                       "get another exp than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out-of-range exp [CPSS_EXP_RANGE_CNS = 8] and other parameters from 1.1.
                Expected: non GT_OK.
            */
            exp = CPSS_EXP_RANGE_CNS;

            st = cpssExMxPmQosPortDefaultExpSet(dev, port, exp);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, exp);
        }

        exp = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultExpSet(dev, port, exp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultExpSet(dev, port, exp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultExpSet(dev, port, exp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    exp = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultExpSet(dev, port, exp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultExpSet(dev, port, exp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDefaultExpGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_U32   *expPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDefaultExpGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL expPtr.
    Expected: GT_OK.
    1.1.2. Call with expPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_U32   exp = 0;
    
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
                1.1.1. Call with non-NULL expPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortDefaultExpGet(dev, port, &exp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with expPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortDefaultExpGet(dev, port, &exp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, expPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDefaultExpGet(dev, port, &exp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDefaultExpGet(dev, port, &exp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDefaultExpGet(dev, port, &exp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDefaultExpGet(dev, port, &exp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDefaultExpGet(dev, port, &exp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortQosPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_U8                                        port,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  qosPrecedence
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortQosPrecedenceSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortQosPrecedenceGet with non-NULL qosPrecedencePtr.
    Expected: GT_OK and the same value as written.
    1.1.3. Call with invalid qosPrecedence [0x5AAAAAA5] and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   qosPrecedence    = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   qosPrecedenceGet = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

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
                1.1.1. Call with qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E].
                Expected: GT_OK.
            */

            /* Call with qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] */
            qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            st = cpssExMxPmQosPortQosPrecedenceSet(dev, port, qosPrecedence);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, qosPrecedence);

            /*
                1.1.2. Call cpssExMxPmQosPortQosPrecedenceGet with non-NULL qosPrecedencePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedenceGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortQosPrecedenceGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(qosPrecedence, qosPrecedenceGet,
                       "get another qosPrecedence than was set: %d, %d", dev, port);

            /* Call with qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E] */
            qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

            st = cpssExMxPmQosPortQosPrecedenceSet(dev, port, qosPrecedence);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, qosPrecedence);

            /*
                1.1.2. Call cpssExMxPmQosPortQosPrecedenceGet with non-NULL qosPrecedencePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedenceGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortQosPrecedenceGet: %d, %d ", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(qosPrecedence, qosPrecedenceGet,
                       "get another qosPrecedence than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with invalid qosPrecedence [0x5AAAAAA5] and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            qosPrecedence = QOS_INVALID_ENUM_CNS;

            st = cpssExMxPmQosPortQosPrecedenceSet(dev, port, qosPrecedence);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, qosPrecedence);
        }

        qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortQosPrecedenceSet(dev, port, qosPrecedence);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortQosPrecedenceSet(dev, port, qosPrecedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortQosPrecedenceSet(dev, port, qosPrecedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortQosPrecedenceSet(dev, port, qosPrecedence);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortQosPrecedenceSet(dev, port, qosPrecedence);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortQosPrecedenceGet
(
    IN  GT_U8                                        devNum,
    IN  GT_U8                                        port,
    OUT CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  *qosPrecedencePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortQosPrecedenceGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL qosPrecedencePtr. Expected: GT_OK.
    1.1.2. Call with qosPrecedencePtr [NULL], other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    
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
                1.1.1. Call with non-NULL qosPrecedencePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedence);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with qosPrecedencePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedence);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, qosPrecedencePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedence);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedence);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortQosPrecedenceGet(dev, port, &qosPrecedence);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortModifyQoSParamEnableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        port,
    IN CPSS_EXMXPM_QOS_PARAM_ENT    param, 
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortModifyQoSParamEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with param [CPSS_EXMXPM_QOS_PARAM_UP_E /
                            CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                            CPSS_EXMXPM_QOS_PARAM_EXP_E],
                     enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortModifyQoSParamEnableGet with non-NULL enablePtr, other params same as in 1.1.
    Expected: GT_OK and the same value as written.
    1.1.3. Call with param [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                            CPSS_EXMXPM_QOS_PARAM_DP_E /
                            CPSS_EXMXPM_QOS_PARAM_TC_E] (not allowed), enable [GT_TRUE].
    Expected: non GT_OK.
    1.1.4. Call with invalid param [0x5AAAAAA5] and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_QOS_PARAM_ENT   param     = CPSS_EXMXPM_QOS_PARAM_UP_E;
    GT_BOOL                     enable    = GT_FALSE;
    GT_BOOL                     enableGet = GT_FALSE;

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
                1.1.1. Call with param [CPSS_EXMXPM_QOS_PARAM_UP_E /
                                        CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                                        CPSS_EXMXPM_QOS_PARAM_EXP_E],
                                 enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE], param [CPSS_EXMXPM_QOS_PARAM_UP_E] */
            enable = GT_FALSE;
            param  = CPSS_EXMXPM_QOS_PARAM_UP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, param, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortModifyQoSParamEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortModifyQoSParamEnableGet: %d, %d, %d", dev, port, param);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_FALSE], param [CPSS_EXMXPM_QOS_PARAM_DSCP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_DSCP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, param, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortModifyQoSParamEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortModifyQoSParamEnableGet: %d, %d, %d", dev, port, param);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_FALSE], param [CPSS_EXMXPM_QOS_PARAM_EXP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_EXP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, param, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortModifyQoSParamEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortModifyQoSParamEnableGet: %d, %d, %d", dev, port, param);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE], param [CPSS_EXMXPM_QOS_PARAM_UP_E] */
            enable = GT_TRUE;
            param  = CPSS_EXMXPM_QOS_PARAM_UP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, param, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortModifyQoSParamEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortModifyQoSParamEnableGet: %d, %d, %d", dev, port, param);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE], param [CPSS_EXMXPM_QOS_PARAM_DSCP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_DSCP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, param, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortModifyQoSParamEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortModifyQoSParamEnableGet: %d, %d, %d", dev, port, param);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE], param [CPSS_EXMXPM_QOS_PARAM_EXP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_EXP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, param, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortModifyQoSParamEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortModifyQoSParamEnableGet: %d, %d, %d", dev, port, param);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with param [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                                        CPSS_EXMXPM_QOS_PARAM_DP_E /
                                        CPSS_EXMXPM_QOS_PARAM_TC_E] (not allowed),
                                 enable [GT_TRUE].
                Expected: NOT GT_OK.
            */

            /* Call with  param [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E] */
            param = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            /* Call with  param [CPSS_EXMXPM_QOS_PARAM_DP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_DP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            /* Call with  param [CPSS_EXMXPM_QOS_PARAM_TC_E] */
            param = CPSS_EXMXPM_QOS_PARAM_TC_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            param = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

            /* 
                1.1.4. Call with invalid param [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            param = QOS_INVALID_ENUM_CNS;

            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, param);
        }

        enable = GT_TRUE;
        param  = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;
    param  = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortModifyQoSParamEnableSet(dev, port, param, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortModifyQoSParamEnableGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        port,
    IN  CPSS_EXMXPM_QOS_PARAM_ENT    param, 
    OUT GT_BOOL                      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortModifyQoSParamEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with param [CPSS_EXMXPM_QOS_PARAM_UP_E /
                            CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                            CPSS_EXMXPM_QOS_PARAM_EXP_E],
                     non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with param [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                            CPSS_EXMXPM_QOS_PARAM_DP_E /
                            CPSS_EXMXPM_QOS_PARAM_TC_E] (not allowed),
                     non-NULL enablePtr.
    Expected: non GT_OK.
    1.1.3. Call with invalid param [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with enablePtr [NULL],
                     other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_QOS_PARAM_ENT   param = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;
    GT_BOOL                     enable         = GT_FALSE;
    
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
                1.1.1. Call with param [CPSS_EXMXPM_QOS_PARAM_UP_E /
                                        CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                                        CPSS_EXMXPM_QOS_PARAM_EXP_E],
                                 non-NULL enablePtr.
                Expected: GT_OK.
            */

            /* Call with param [CPSS_EXMXPM_QOS_PARAM_UP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_UP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            /* Call with param [CPSS_EXMXPM_QOS_PARAM_DSCP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_DSCP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            /* Call with param [CPSS_EXMXPM_QOS_PARAM_EXP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_EXP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            /*
                1.1.2. Call with param [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                                        CPSS_EXMXPM_QOS_PARAM_DP_E /
                                        CPSS_EXMXPM_QOS_PARAM_TC_E] (not allowed)
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */

            /* Call with  param [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E] */
            param = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            /* Call with  param [CPSS_EXMXPM_QOS_PARAM_DP_E] */
            param = CPSS_EXMXPM_QOS_PARAM_DP_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            /* Call with  param [CPSS_EXMXPM_QOS_PARAM_TC_E] */
            param = CPSS_EXMXPM_QOS_PARAM_TC_E;

            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, param);

            param = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

            /* 
                1.1.3. Call with invalid param [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            param = QOS_INVALID_ENUM_CNS;

            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, param);

            param = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

            /* 
                1.1.4. Call with enablePtr [NULL],
                                 other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        param = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    param = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortModifyQoSParamEnableGet(dev, port, param, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortReMapDscpEnableSet
(
    IN GT_U8        devNum,
    IN GT_U8        port,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortReMapDscpEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortReMapDscpEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same value as written.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmQosPortReMapDscpEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortReMapDscpEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortReMapDscpEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmQosPortReMapDscpEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortReMapDscpEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortReMapDscpEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortReMapDscpEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortReMapDscpEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortReMapDscpEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortReMapDscpEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortReMapDscpEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortReMapDscpEnableGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        port,
    OUT GT_BOOL                      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortReMapDscpEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
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
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortReMapDscpEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortReMapUpEnableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        port,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortReMapUpEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortReMapUpEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same value as written.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmQosPortReMapUpEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortReMapUpEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortReMapUpEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmQosPortReMapUpEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortReMapUpEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortReMapUpEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortReMapUpEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortReMapUpEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortReMapUpEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortReMapUpEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortReMapUpEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortReMapUpEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortReMapUpEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
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
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortReMapUpEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosMacQosEntrySet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryIdx,
    IN CPSS_EXMXPM_QOS_ENTRY_STC       *macQosCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosMacQosEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIdx [1 / 7 / 8],
                   macQosCfgPtr {qosParamsModify {modifyTc [GT_FALSE / GT_TRUE],
                                                  modifyUp [GT_FALSE / GT_TRUE],
                                                  modifyDp [GT_FALSE / GT_TRUE],
                                                  modifyDscp [GT_FALSE / GT_TRUE],
                                                  modifyExp [GT_FALSE / GT_TRUE]},
                                 qosParams {tc [0 / 7],
                                            dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                            up [0 / 7],
                                            dscp [0 / 63],
                                            exp [0 / 7] },
                                 qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E] } .
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosMacQosEntryGet with entryIdx [1 / 7 / 8],
                                               non-NULL macQosCfgPtr.
    Expected: GT_OK and same values as written.
    1.3. Check out of range entryIdx. Call with entryIdx [0 / 9], other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call with out of range macQosCfgPtr->qosParams.tc [CPSS_TC_RANGE_CNS = 8],
                                macQosCfgPtr->modifyTc [GT_TRUE]
                                and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Check invalid enum. Call with macQosCfgPtr->qosParams.dp [0x5AAAAAA5],
                                       macQosCfgPtr->modifyDp [GT_TRUE],
                                       other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out-of-range macQosCfgPtr->qosParams.up [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                                macQosCfgPtr->modifyUp [GT_TRUE]
                                and other parameters from 1.1.
    Expected: non GT_OK.
    1.7. Call with out of range macQosCfgPtr->qosParams.dscp [CPSS_DSCP_RANGE_CNS = 64],
                                macQosCfgPtr->modifyDscp [GT_TRUE]
                                and other parameters from 1.1.
    Expected: non GT_OK.
    1.8. Call with out of range macQosCfgPtr->qosParams.exp [CPSS_EXP_RANGE_CNS = 8],
                                macQosCfgPtr->modifyExp [GT_TRUE]
                                and other parameters from 1.1.
    Expected: non GT_OK.
    1.9. Check invalid enum. Call with macQosCfgPtr->qosPrecedence [0x5AAAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with macQosCfgPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      entryIdx = 0;
    GT_BOOL                     isEqual  = GT_FALSE;
    CPSS_EXMXPM_QOS_ENTRY_STC   macQosCfg;
    CPSS_EXMXPM_QOS_ENTRY_STC   macQosCfgGet;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with entryIdx [1 / 7 / 8],
                           macQosCfgPtr {qosParamsModify {modifyTc [GT_FALSE / GT_TRUE],
                                                          modifyUp [GT_FALSE / GT_TRUE],
                                                          modifyDp [GT_FALSE / GT_TRUE],
                                                          modifyDscp [GT_FALSE / GT_TRUE],
                                                          modifyExp [GT_FALSE / GT_TRUE]},
                                         qosParams {tc [0 / 7],
                                                    dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                                    up [0 / 7],
                                                    dscp [0 / 63],
                                                    exp [0 / 7] },
                                         qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                        CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E] } .
            Expected: GT_OK.
        */

        /* Call with profileIndex [1] */
        entryIdx = 1;

        macQosCfg.qosParamsModify.modifyTc   = GT_FALSE;
        macQosCfg.qosParamsModify.modifyUp   = GT_FALSE;
        macQosCfg.qosParamsModify.modifyDp   = GT_FALSE;
        macQosCfg.qosParamsModify.modifyDscp = GT_FALSE;
        macQosCfg.qosParamsModify.modifyExp  = GT_FALSE;
        macQosCfg.qosParams.tc   = 0;
        macQosCfg.qosParams.dp   = CPSS_DP_GREEN_E;
        macQosCfg.qosParams.up   = 0;
        macQosCfg.qosParams.dscp = 0;
        macQosCfg.qosParams.exp = 0;
        macQosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /*
            1.2. Call cpssExMxPmQosMacQosEntryGet with entryIdx [1 / 7 / 8],
                                                       non-NULL macQosCfgPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosMacQosEntryGet: %d, %d", dev, entryIdx);
        
        /* Verifying macQosCfgPtr->qosParamsModify values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macQosCfg.qosParamsModify,
                               (GT_VOID*) &macQosCfg.qosParamsModify,
                                sizeof(macQosCfg.qosParamsModify))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macQosCfgPtr->qosParamsModify than was set: %d", dev);

        /* Verifying macQosCfgPtr->qosParams values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.tc, macQosCfgGet.qosParams.tc,
                       "get another macQosCfgPtr->qosParams.tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.dp, macQosCfgGet.qosParams.dp,
                       "get another macQosCfgPtr->qosParams.dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.up, macQosCfgGet.qosParams.up,
                       "get another macQosCfgPtr->qosParams.up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.dscp, macQosCfgGet.qosParams.dscp,
                       "get another macQosCfgPtr->qosParams.dscp than was set: %d", dev);        
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.exp, macQosCfgGet.qosParams.exp,
                       "get another macQosCfgPtr->qosParams.exp than was set: %d", dev);        

        /* Verifying macQosCfgPtr->qosPrecedence values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosPrecedence, macQosCfgGet.qosPrecedence,
                       "get another macQosCfgPtr->qosPrecedence than was set: %d", dev);

        /* Call with profileIndex [7] */
        entryIdx = 7;

        macQosCfg.qosParamsModify.modifyTc   = GT_TRUE;
        macQosCfg.qosParamsModify.modifyUp   = GT_TRUE;
        macQosCfg.qosParamsModify.modifyDp   = GT_TRUE;
        macQosCfg.qosParamsModify.modifyDscp = GT_TRUE;
        macQosCfg.qosParamsModify.modifyExp  = GT_TRUE;        
        macQosCfg.qosParams.tc   = 7;
        macQosCfg.qosParams.dp   = CPSS_DP_RED_E;
        macQosCfg.qosParams.up   = 7;
        macQosCfg.qosParams.dscp = 63;
        macQosCfg.qosParams.exp  = 7;        
        macQosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /*
            1.2. Call cpssExMxPmQosMacQosEntryGet with entryIdx [1 / 7 / 8],
                                                       non-NULL macQosCfgPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosMacQosEntryGet: %d, %d", dev, entryIdx);
        
        /* Verifying macQosCfgPtr->qosParamsModify values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macQosCfg.qosParamsModify,
                               (GT_VOID*) &macQosCfg.qosParamsModify,
                                sizeof(macQosCfg.qosParamsModify))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macQosCfgPtr->qosParamsModify than was set: %d", dev);

        /* Verifying macQosCfgPtr->qosParams values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.tc, macQosCfgGet.qosParams.tc,
                       "get another macQosCfgPtr->qosParams.tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.dp, macQosCfgGet.qosParams.dp,
                       "get another macQosCfgPtr->qosParams.dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.up, macQosCfgGet.qosParams.up,
                       "get another macQosCfgPtr->qosParams.up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.dscp, macQosCfgGet.qosParams.dscp,
                       "get another macQosCfgPtr->qosParams.dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.exp, macQosCfgGet.qosParams.exp,
                       "get another macQosCfgPtr->qosParams.exp than was set: %d", dev);        

        /* Verifying macQosCfgPtr->qosPrecedence values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosPrecedence, macQosCfgGet.qosPrecedence,
                       "get another macQosCfgPtr->qosPrecedence than was set: %d", dev);

        /* Call with profileIndex [8] */
        entryIdx = 8;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /*
            1.2. Call cpssExMxPmQosMacQosEntryGet with entryIdx [1 / 7 / 8],
                                                       non-NULL macQosCfgPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosMacQosEntryGet: %d, %d", dev, entryIdx);
        
        /* Verifying macQosCfgPtr->qosParamsModify values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macQosCfg.qosParamsModify,
                               (GT_VOID*) &macQosCfg.qosParamsModify,
                                sizeof(macQosCfg.qosParamsModify))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macQosCfgPtr->qosParamsModify than was set: %d", dev);

        /* Verifying macQosCfgPtr->qosParams values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.tc, macQosCfgGet.qosParams.tc,
                       "get another macQosCfgPtr->qosParams.tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.dp, macQosCfgGet.qosParams.dp,
                       "get another macQosCfgPtr->qosParams.dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.up, macQosCfgGet.qosParams.up,
                       "get another macQosCfgPtr->qosParams.up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.dscp, macQosCfgGet.qosParams.dscp,
                       "get another macQosCfgPtr->qosParams.dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.exp, macQosCfgGet.qosParams.exp,
                       "get another macQosCfgPtr->qosParams.exp than was set: %d", dev);        

        /* Verifying macQosCfgPtr->qosPrecedence values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosPrecedence, macQosCfgGet.qosPrecedence,
                       "get another macQosCfgPtr->qosPrecedence than was set: %d", dev);

        /* 
            1.3. Check out of range entryIdx. Call with entryIdx [0 / 9], other params same as in 1.1.
            Expected: non GT_OK.
        */

        /* Call with entryIdx [0] */
        entryIdx = 0;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /* Call with entryIdx [9] */
        entryIdx = 9;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        entryIdx = 1;

        /*
            1.4. Call with out of range macQosCfgPtr->qosParams.tc [CPSS_TC_RANGE_CNS = 8],
                                        macQosCfgPtr->modifyTc [GT_TRUE]
                                        and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        macQosCfg.qosParams.tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macQosCfg->qosParams.tc = %d", dev, macQosCfg.qosParams.tc);

        macQosCfg.qosParams.tc = 0;

        /* 
            1.5. Check invalid enum. Call with macQosCfgPtr->qosParams.dp [0x5AAAAAA5],
                                               macQosCfgPtr->modifyDp [GT_TRUE],
                                               other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        macQosCfg.qosParams.dp = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, macQosCfg->qosParams.dp = %d", dev, macQosCfg.qosParams.dp);

        macQosCfg.qosParams.dp = CPSS_DP_GREEN_E;

        /*
            1.6. Call with out-of-range macQosCfgPtr->qosParams.up [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                                        macQosCfgPtr->modifyUp [GT_TRUE]
                                        and other parameters from 1.1.
            Expected: non GT_OK.
        */
        macQosCfg.qosParams.up = CPSS_USER_PRIORITY_RANGE_CNS;
        
        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macQosCfg->qosParams.up = %d", dev, macQosCfg.qosParams.up);

        macQosCfg.qosParams.up = 0;

        /*
            1.7. Call with out of range macQosCfgPtr->qosParams.dscp [CPSS_DSCP_RANGE_CNS = 64],
                                        macQosCfgPtr->modifyDscp [GT_TRUE]
                                        and other parameters from 1.1.
            Expected: non GT_OK.
        */
        macQosCfg.qosParams.dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macQosCfgPtr->qosParams.dscp= %d", dev, macQosCfg.qosParams.dscp);

        macQosCfg.qosParams.dscp = 0;

        /*
            1.9. Check invalid enum. Call with macQosCfgPtr->qosPrecedence [0x5AAAAAA5], other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        macQosCfg.qosPrecedence = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, macQosCfgPtr->qosPrecedence = %d", dev, macQosCfg.qosPrecedence);

        macQosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        /* 
            1.10. Call with macQosCfgPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macQosCfgPtr = NULL", dev);
    }

    entryIdx = 1;

    macQosCfg.qosParamsModify.modifyTc   = GT_FALSE;
    macQosCfg.qosParamsModify.modifyUp   = GT_FALSE;
    macQosCfg.qosParamsModify.modifyDp   = GT_FALSE;
    macQosCfg.qosParamsModify.modifyDscp = GT_FALSE;
    macQosCfg.qosParamsModify.modifyExp  = GT_FALSE;
    macQosCfg.qosParams.tc   = 0;
    macQosCfg.qosParams.dp   = CPSS_DP_GREEN_E;
    macQosCfg.qosParams.up   = 0;
    macQosCfg.qosParams.dscp = 0;
    macQosCfg.qosParams.exp = 0;
    macQosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    
    macQosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosMacQosEntrySet(dev, entryIdx, &macQosCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosMacQosEntryGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      entryIdx,
    OUT CPSS_EXMXPM_QOS_ENTRY_STC   *macQosCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosMacQosEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIdx [1 / 7 / 8], non-NULL macQosCfgPtr.
    Expected: GT_OK.
    1.2. Check out-of-range entryIdx. Call with entryIdx [0 / 9], other params same as in 1.1.
    Expected: non GT_OK.
    1.3. Call with macQosCfgPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      entryIdx = 0;
    CPSS_EXMXPM_QOS_ENTRY_STC   macQosCfg;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with entryIdx [1 / 7 / 8], non-NULL macQosCfgPtr.
            Expected: GT_OK.
        */

        /* Call with profileIndex [1] */
        entryIdx = 1;

        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /* Call with profileIndex [7] */
        entryIdx = 7;

        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /* Call with profileIndex [8] */
        entryIdx = 8;

        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /* 
            1.3. Check out of range entryIdx. Call with entryIdx [0 / 9], other params same as in 1.1.
            Expected: non GT_OK.
        */

        /* Call with entryIdx [0] */
        entryIdx = 0;

        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /* Call with entryIdx [9] */
        entryIdx = 9;

        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        entryIdx = 1;

        /* 
            1.3. Call with macQosCfgPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macQosCfgPtr = NULL", dev);
    }

    entryIdx = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosMacQosEntryGet(dev, entryIdx, &macQosCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosMacQosConflictResolutionSet
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_QOS_MAC_RESOLVE_ENT  macQosResolvType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosMacQosConflictResolutionSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with macQosResolvType [CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_DA_E /
                                     CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_MAX_TC_SA_E /
                                     CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_MIN_TC_DA_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosMacQosConflictResolutionGet with non-NULL macQosResolvTypePtr, other param same as in 1.1.
    Expected: GT_OK and same value as written.
    1.3. Check invalid enum. Call with macQosResolvType [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_QOS_MAC_RESOLVE_ENT macQosResolvType    = CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_DA_E;
    CPSS_EXMXPM_QOS_MAC_RESOLVE_ENT macQosResolvTypeGet = CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_DA_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with macQosResolvType [CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_DA_E /
                                             CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_MAX_TC_SA_E /
                                             CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_MIN_TC_DA_E].
            Expected: GT_OK.
        */

        /* Call with macQosResolvType [CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_DA_E] */
        macQosResolvType = CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_DA_E;

        st = cpssExMxPmQosMacQosConflictResolutionSet(dev, macQosResolvType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, macQosResolvType);

        /*
            1.2. Call cpssExMxPmQosMacQosConflictResolutionGet with non-NULL macQosResolvTypePtr, other param same as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmQosMacQosConflictResolutionGet(dev, &macQosResolvTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosMacQosConflictResolutionGet: %d", dev);

        /* Verify values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosResolvType, macQosResolvTypeGet,
                       "get another macQosResolvType than was set: %d", dev);

        /* Call with macQosResolvType [CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_MAX_TC_SA_E] */
        macQosResolvType = CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_MAX_TC_SA_E;

        st = cpssExMxPmQosMacQosConflictResolutionSet(dev, macQosResolvType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, macQosResolvType);

        /*
            1.2. Call cpssExMxPmQosMacQosConflictResolutionGet with non-NULL macQosResolvTypePtr, other param same as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmQosMacQosConflictResolutionGet(dev, &macQosResolvTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosMacQosConflictResolutionGet: %d", dev);

        /* Verify values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosResolvType, macQosResolvTypeGet,
                       "get another macQosResolvType than was set: %d", dev);

        /* Call with macQosResolvType [CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_MIN_TC_DA_E] */
        macQosResolvType = CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_MIN_TC_DA_E;

        st = cpssExMxPmQosMacQosConflictResolutionSet(dev, macQosResolvType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, macQosResolvType);

        /*
            1.2. Call cpssExMxPmQosMacQosConflictResolutionGet with non-NULL macQosResolvTypePtr, other param same as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmQosMacQosConflictResolutionGet(dev, &macQosResolvTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosMacQosConflictResolutionGet: %d", dev);

        /* Verify values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macQosResolvType, macQosResolvTypeGet,
                       "get another macQosResolvType than was set: %d", dev);

        /*
            1.3. Check invalid enum. Call with macQosResolvType [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        macQosResolvType = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosMacQosConflictResolutionSet(dev, macQosResolvType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, macQosResolvType);
    }

    macQosResolvType = CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_DA_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosMacQosConflictResolutionSet(dev, macQosResolvType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosMacQosConflictResolutionSet(dev, macQosResolvType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosMacQosConflictResolutionGet
(
    IN GT_U8                             devNum,
    OUT CPSS_EXMXPM_QOS_MAC_RESOLVE_ENT  *macQosResolvTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosMacQosConflictResolutionGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL macQosResolvTypePtr.
    Expected: GT_OK.
    1.2. Call with macQosResolvTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_QOS_MAC_RESOLVE_ENT macQosResolvType = CPSS_EXMXPM_QOS_MAC_RESOLVE_USE_DA_E;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL macQosResolvTypePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmQosMacQosConflictResolutionGet(dev, &macQosResolvType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with macQosResolvTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosMacQosConflictResolutionGet(dev, &macQosResolvType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, macQosResolvTypePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosMacQosConflictResolutionGet(dev, &macQosResolvType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosMacQosConflictResolutionGet(dev, &macQosResolvType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosUnkUcEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosUnkUcEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosUnkUcEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same value as written.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmQosUnkUcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmQosUnkUcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosUnkUcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUnkUcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmQosUnkUcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmQosUnkUcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosUnkUcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUnkUcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosUnkUcEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosUnkUcEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosUnkUcEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosUnkUcEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmQosUnkUcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosUnkUcEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosUnkUcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosUnkUcEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosUnregMcBcEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosUnregMcBcEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosUnregMcBcEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same value as written.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmQosUnregMcBcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmQosUnregMcBcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosUnregMcBcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUnregMcBcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmQosUnregMcBcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmQosUnregMcBcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same value as written.
        */
        st = cpssExMxPmQosUnregMcBcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosUnregMcBcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosUnregMcBcEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosUnregMcBcEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosUnregMcBcEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosUnregMcBcEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmQosUnregMcBcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosUnregMcBcEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosUnregMcBcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosUnregMcBcEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDpToCfiMapEnableSet
(
    IN GT_U8        devNum,
    IN GT_U8        port,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDpToCfiMapEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortDpToCfiMapEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same value as written.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmQosPortDpToCfiMapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortDpToCfiMapEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDpToCfiMapEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmQosPortDpToCfiMapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmQosPortDpToCfiMapEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmQosPortDpToCfiMapEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDpToCfiMapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDpToCfiMapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDpToCfiMapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDpToCfiMapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDpToCfiMapEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortDpToCfiMapEnableGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        port,
    OUT GT_BOOL                      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortDpToCfiMapEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = QOS_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
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
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = QOS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmQosPortDpToCfiMapEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosDpToCfiMapSet
(
    IN GT_U8                devNum,
    IN CPSS_DP_LEVEL_ENT    dp,
    IN GT_U32               cfi
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosDpToCfiMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dp [CPSS_DP_GREEN_E /
                       CPSS_DP_YELLOW_E],
                   cfi [0 / 1].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosDpToCfiMapGet with not NULL cfiPtr.
    Expected: GT_OK and the same cfi as was set.
    1.3. Call dp [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.4. Call cfi [2] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DP_LEVEL_ENT    dp = CPSS_DP_GREEN_E;
    GT_U32               cfi = 0;
    GT_U32               cfiGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with dp [CPSS_DP_GREEN_E /
                               CPSS_DP_YELLOW_E],
                           cfi [0 / 1].
            Expected: GT_OK.
        */
        dp = CPSS_DP_GREEN_E;
        cfi = 0;

        st = cpssExMxPmQosDpToCfiMapSet(dev, dp, cfi);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, dp, cfi);

        /*
            1.2. Call cpssExMxPmQosDpToCfiMapGet with not NULL cfiPtr.
            Expected: GT_OK and the same cfi as was set.
        */
        st = cpssExMxPmQosDpToCfiMapGet(dev, dp, &cfiGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosDpToCfiMapGet: %d, %d", dev, dp);
        UTF_VERIFY_EQUAL1_STRING_MAC(cfi, cfiGet, "got another cfi than wa sset: %d", dev);

        dp = CPSS_DP_YELLOW_E;
        cfi = 1;

        st = cpssExMxPmQosDpToCfiMapSet(dev, dp, cfi);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, dp, cfi);

        /*
            1.2. Call cpssExMxPmQosDpToCfiMapGet with not NULL cfiPtr.
            Expected: GT_OK and the same cfi as was set.
        */
        st = cpssExMxPmQosDpToCfiMapGet(dev, dp, &cfiGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosDpToCfiMapGet: %d, %d", dev, dp);
        UTF_VERIFY_EQUAL1_STRING_MAC(cfi, cfiGet, "got another cfi than wa sset: %d", dev);

        /*
            1.3. Call dp [0x5AAAAAA5] (out of range).
            Expected: GT_BAD_PARAM.
        */
        dp = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosDpToCfiMapSet(dev, dp, cfi);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, dp);

        dp = CPSS_DP_GREEN_E;

        /*
            1.4. Call cfi [2] (out of range).
            Expected: NOT GT_OK.
        */
        cfi = 2;

        st = cpssExMxPmQosDpToCfiMapSet(dev, dp, cfi);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, dp, cfi);
    }

    dp = CPSS_DP_GREEN_E;
    cfi = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosDpToCfiMapSet(dev, dp, cfi);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosDpToCfiMapSet(dev, dp, cfi);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosDpToCfiMapGet
(
    IN  GT_U8                devNum,
    IN CPSS_DP_LEVEL_ENT     dp,
    OUT  GT_U32              *cfiPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosDpToCfiMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dp [CPSS_DP_GREEN_E /
                       CPSS_DP_YELLOW_E],
                   not NULL cfiPtr.
    Expected: GT_OK.
    1.2. Call dp [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.3. Call cfi [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DP_LEVEL_ENT  dp;
    GT_U32             cfi;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with dp [CPSS_DP_GREEN_E /
                               CPSS_DP_YELLOW_E],
                           not NULL cfiPtr.
            Expected: GT_OK.
        */
        dp = CPSS_DP_GREEN_E;

        st = cpssExMxPmQosDpToCfiMapGet(dev, dp, &cfi);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dp);

        dp = CPSS_DP_YELLOW_E;

        st = cpssExMxPmQosDpToCfiMapGet(dev, dp, &cfi);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dp);

        /*
            1.2. Call dp [0x5AAAAAA5] (out of range).
            Expected: GT_BAD_PARAM.
        */
        dp = QOS_INVALID_ENUM_CNS;

        st = cpssExMxPmQosDpToCfiMapGet(dev, dp, &cfi);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, dp);

        dp = CPSS_DP_YELLOW_E;

        /*
            1.3. Call cfi [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosDpToCfiMapGet(dev, dp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cfi = NULL", dev);
    }

    dp = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosDpToCfiMapGet(dev, dp, &cfi);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosDpToCfiMapGet(dev, dp, &cfi);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortTrustConfigSet
(
    IN GT_U8                                devNum,
    IN GT_U8                                port,
    IN CPSS_EXMXPM_QOS_PORT_TRUST_PARAM_STC *trustParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortTrustConfigSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with trustParamPtr {  dsaTagQos [GT_TRUE / GT_FALSE] /
                                    up [GT_TRUE / GT_FALSE] /
                                    dscp [GT_TRUE / GT_FALSE] /
                                    exp [GT_TRUE / GT_FALSE] }
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmQosPortTrustConfigGet with not NULL trustParamPtr
    Expected: GT_OK and the same trustParam as was set.
    1.1.3. Call with trustParamPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       port = 0;

    CPSS_EXMXPM_QOS_PORT_TRUST_PARAM_STC trustParam;
    CPSS_EXMXPM_QOS_PORT_TRUST_PARAM_STC trustParamGet;


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
                1.1.1. Call with trustParamPtr {  dsaTagQos [GT_TRUE / GT_FALSE] /
                                                up [GT_TRUE / GT_FALSE] /
                                                dscp [GT_TRUE / GT_FALSE] /
                                                exp [GT_TRUE / GT_FALSE] }
                Expected: GT_OK.
            */
            trustParam.dsaTagQos = GT_TRUE;
            trustParam.up = GT_TRUE;
            trustParam.dscp = GT_TRUE;
            trustParam.exp = GT_TRUE;

            st = cpssExMxPmQosPortTrustConfigSet(dev, port, &trustParam);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssExMxPmQosPortTrustConfigGet with not NULL trustParamPtr
                Expected: GT_OK and the same trustParam as was set.
            */
            st = cpssExMxPmQosPortTrustConfigGet(dev, port, &trustParamGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmQosPortTrustConfigGet: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(trustParam.dsaTagQos, trustParamGet.dsaTagQos, "got another trustParam.dsaTagQos than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(trustParam.up, trustParamGet.up, "got another trustParam.up than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(trustParam.dscp, trustParamGet.dscp, "got another trustParam.dscp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(trustParam.exp, trustParamGet.exp, "got another trustParam.exp than was set: %d", dev);

            trustParam.dsaTagQos = GT_FALSE;
            trustParam.up = GT_FALSE;
            trustParam.dscp = GT_FALSE;
            trustParam.exp = GT_FALSE;

            st = cpssExMxPmQosPortTrustConfigSet(dev, port, &trustParam);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssExMxPmQosPortTrustConfigGet with not NULL trustParamPtr
                Expected: GT_OK and the same trustParam as was set.
            */
            st = cpssExMxPmQosPortTrustConfigGet(dev, port, &trustParamGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmQosPortTrustConfigGet: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(trustParam.dsaTagQos, trustParamGet.dsaTagQos, "got another trustParam.dsaTagQos than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(trustParam.up, trustParamGet.up, "got another trustParam.up than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(trustParam.dscp, trustParamGet.dscp, "got another trustParam.dscp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(trustParam.exp, trustParamGet.exp, "got another trustParam.exp than was set: %d", dev);

            /*
                1.1.3. Call with trustParamPtr [NULL]
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortTrustConfigSet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, trustParam = NULL", dev);
        }

        trustParam.dsaTagQos = GT_TRUE;
        trustParam.up = GT_TRUE;
        trustParam.dscp = GT_TRUE;
        trustParam.exp = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortTrustConfigSet(dev, port, &trustParam);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortTrustConfigSet(dev, port, &trustParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortTrustConfigSet(dev, port, &trustParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    trustParam.dsaTagQos = GT_TRUE;
    trustParam.up = GT_TRUE;
    trustParam.dscp = GT_TRUE;
    trustParam.exp = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortTrustConfigSet(dev, port, &trustParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosPortTrustConfigSet(dev, port, &trustParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosPortTrustConfigGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        port,
    OUT CPSS_EXMXPM_QOS_PORT_TRUST_PARAM_STC *trustParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosPortTrustConfigGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with not NULL trustParamPtr.
    Expected: GT_OK.
    1.1.2. Call with trustParamPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_EXMXPM_QOS_PORT_TRUST_PARAM_STC trustParam;


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
                1.1.1. Call with not NULL trustParamPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmQosPortTrustConfigGet(dev, port, &trustParam);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call with trustParamPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmQosPortTrustConfigGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, trustParam = NULL", dev);
        }


        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmQosPortTrustConfigGet(dev, port, &trustParam);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmQosPortTrustConfigGet(dev, port, &trustParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number. */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmQosPortTrustConfigGet(dev, port, &trustParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosPortTrustConfigGet(dev, port, &trustParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosPortTrustConfigGet(dev, port, &trustParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosAutoLearnQosProfileSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosAutoLearnQosProfileSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 7].
    Expected: GT_OK.
    1.2. Call cpssExMxPmQosAutoLearnQosProfileGet with not NULL profileIndexPtr.
    Expected: GT_OK and the same profileIndex as was set.
    1.3. Call with profileIndex[8] (out of range)
    Expectred: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32   profileIndex = 0;
    GT_U32   profileIndexGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 7].
            Expected: GT_OK.
        */
        /* profileIndex = 0 */
        profileIndex = 0;

        st = cpssExMxPmQosAutoLearnQosProfileSet(dev, profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        /*
            1.2. Call cpssExMxPmQosAutoLearnQosProfileGet with not NULL profileIndexPtr.
            Expected: GT_OK and the same profileIndex as was set.
        */
        st = cpssExMxPmQosAutoLearnQosProfileGet(dev, &profileIndexGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmQosAutoLearnQosProfileGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profileIndex, profileIndexGet, "got another profileIndex than was set: %d", dev);

        /* profileIndex = 7 */
        profileIndex = 7;

        st = cpssExMxPmQosAutoLearnQosProfileSet(dev, profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        /*
            1.2. Call cpssExMxPmQosAutoLearnQosProfileGet with not NULL profileIndexPtr.
            Expected: GT_OK and the same profileIndex as was set.
        */
        st = cpssExMxPmQosAutoLearnQosProfileGet(dev, &profileIndexGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmQosAutoLearnQosProfileGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profileIndex, profileIndexGet, "got another profileIndex than was set: %d", dev);

        /*
            1.3. Call with profileIndex[8] (out of range)
            Expectred: NOT GT_OK.
        */
        profileIndex = 8;

        st = cpssExMxPmQosAutoLearnQosProfileSet(dev, profileIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);
    }

    profileIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosAutoLearnQosProfileSet(dev, profileIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosAutoLearnQosProfileSet(dev, profileIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmQosAutoLearnQosProfileGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *profileIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosAutoLearnQosProfileGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL profileIndexPtr
    Expected: GT_OK.
    1.2. Call with profileIndexPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      profileIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL profileIndexPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmQosAutoLearnQosProfileGet(dev, &profileIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with profileIndexPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmQosAutoLearnQosProfileGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, profileIndexPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmQosAutoLearnQosProfileGet(dev, &profileIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmQosAutoLearnQosProfileGet(dev, &profileIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
    Test function to Fill MAC QoS table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmQosFillMacQoSTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in MAC QoS table.
         Call cpssExMxPmQosMacQosEntrySet with entryIdx [1..numEntries - 1],
                           macQosCfgPtr {qosParamsModify {modifyTc [GT_FALSE],
                                                          modifyUp [GT_FALSE],
                                                          modifyDp [GT_FALSE],
                                                          modifyDscp [GT_FALSE],
                                                          modifyExp [GT_FALSE]},
                                         qosParams {tc [0 ],
                                                    dp [CPSS_DP_GREEN_E],
                                                    up [0 ],
                                                    dscp [0 ],
                                                    exp [0 ] },
                                         qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] } .
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmQosMacQosEntrySet with entryIdx [numEntries],
    Expected: NOT GT_OK.
    1.4. Read all entries in MAC QoS table and compare with original.
         Call cpssExMxPmQosMacQosEntryGet with entryIdx [1  ... numEntries-1],
                                                       non-NULL macQosCfgPtr.
    Expected: GT_OK and same values as written.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmQosMacQosEntryGet with entryIdx [numEntries],non-NULL macQosCfgPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_QOS_ENTRY_STC   macQosCfg;
    CPSS_EXMXPM_QOS_ENTRY_STC   macQosCfgGet;


    cpssOsBzero((GT_VOID*) &macQosCfg, sizeof(macQosCfg));
    cpssOsBzero((GT_VOID*) &macQosCfgGet, sizeof(macQosCfgGet));

    /* Fill the entry for MAC QoS table */
    macQosCfg.qosParamsModify.modifyTc   = GT_TRUE;
    macQosCfg.qosParamsModify.modifyUp   = GT_TRUE;
    macQosCfg.qosParamsModify.modifyDp   = GT_TRUE;
    macQosCfg.qosParamsModify.modifyDscp = GT_TRUE;
    macQosCfg.qosParamsModify.modifyExp  = GT_TRUE;
    macQosCfg.qosParams.tc   = 0;
    macQosCfg.qosParams.dp   = CPSS_DP_GREEN_E;
    macQosCfg.qosParams.up   = 0;
    macQosCfg.qosParams.dscp = 0;
    macQosCfg.qosParams.exp = 0;
    macQosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 9;

        /* 1.2. Fill all entries in MAC QoS table */
        for(iTemp = 1; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            macQosCfg.qosParams.tc = iTemp % 7;
            macQosCfg.qosParams.up = iTemp % 7;
            macQosCfg.qosParams.dscp = iTemp % 63;

            st = cpssExMxPmQosMacQosEntrySet(dev, iTemp, &macQosCfg);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosMacQosEntrySet: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmQosMacQosEntrySet(dev, 0, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosMacQosEntrySet: %d, %d", dev, iTemp);

        st = cpssExMxPmQosMacQosEntrySet(dev, numEntries, &macQosCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosMacQosEntrySet: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in MAC QoS table and compare with original */
        for(iTemp = 1; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            macQosCfg.qosParams.tc = iTemp % 7;
            macQosCfg.qosParams.up = iTemp % 7;
            macQosCfg.qosParams.dscp = iTemp % 63;

            st = cpssExMxPmQosMacQosEntryGet(dev, iTemp, &macQosCfgGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmQosMacQosEntryGet: %d, %d", dev, iTemp);
            
            /* Verifying macQosCfgPtr->qosParamsModify values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macQosCfg.qosParamsModify,
                                   (GT_VOID*) &macQosCfg.qosParamsModify,
                                    sizeof(macQosCfg.qosParamsModify))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another macQosCfgPtr->qosParamsModify than was set: %d", dev);
    
            /* Verifying macQosCfgPtr->qosParams values */
            UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.tc, macQosCfgGet.qosParams.tc,
                           "get another macQosCfgPtr->qosParams.tc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.dp, macQosCfgGet.qosParams.dp,
                           "get another macQosCfgPtr->qosParams.dp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.up, macQosCfgGet.qosParams.up,
                           "get another macQosCfgPtr->qosParams.up than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.dscp, macQosCfgGet.qosParams.dscp,
                           "get another macQosCfgPtr->qosParams.dscp than was set: %d", dev);        
            UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosParams.exp, macQosCfgGet.qosParams.exp,
                           "get another macQosCfgPtr->qosParams.exp than was set: %d", dev);        
    
            /* Verifying macQosCfgPtr->qosPrecedence values */
            UTF_VERIFY_EQUAL1_STRING_MAC(macQosCfg.qosPrecedence, macQosCfgGet.qosPrecedence,
                           "get another macQosCfgPtr->qosPrecedence than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmQosMacQosEntryGet(dev, 0, &macQosCfgGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosMacQosEntryGet: %d, %d", dev, iTemp);

        st = cpssExMxPmQosMacQosEntryGet(dev, numEntries, &macQosCfgGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmQosMacQosEntryGet: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmQos suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmQos)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosDsaTagQosProfileToQosParamMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosDsaTagQosProfileToQosParamMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosMplsExpToDpMappingTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosMplsExpToDpMappingTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosDscpToQosParamMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosDscpToQosParamMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosExpToQosParamMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosExpToQosParamMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosUpToQosParamMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosUpToQosParamMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosDscpToDscpMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosDscpToDscpMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosUpToUpMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosUpToUpMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosCfiToDpMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosCfiToDpMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortCfiToDpMapEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortCfiToDpMapEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortTrustEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultTcGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultUpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultUpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultDscpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultDscpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultDpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultDpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultExpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDefaultExpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortQosPrecedenceSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortQosPrecedenceGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortModifyQoSParamEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortModifyQoSParamEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortReMapDscpEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortReMapDscpEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortReMapUpEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortReMapUpEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosMacQosEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosMacQosEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosMacQosConflictResolutionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosMacQosConflictResolutionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosUnkUcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosUnkUcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosUnregMcBcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosUnregMcBcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDpToCfiMapEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortDpToCfiMapEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosDpToCfiMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosDpToCfiMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortTrustConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosPortTrustConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosAutoLearnQosProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosAutoLearnQosProfileGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmQosFillMacQoSTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmQos)

