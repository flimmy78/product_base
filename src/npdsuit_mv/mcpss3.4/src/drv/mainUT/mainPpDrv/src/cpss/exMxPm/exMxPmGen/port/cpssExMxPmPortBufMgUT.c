/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPortBufMgUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmPortBufMg, that provides
*       ExMxPm CPSS implementation for Port configuration, control facility and
*       ingress bandwidth management.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortBufMg.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define PORT_BUF_MG_VALID_PHY_PORT_CNS  0

/* Invalid enum */
#define PORT_BUF_MG_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXonLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   xonLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXonLimitSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmPortFlowControlXoffLimitSet with xoffLimit [0x00FF].
    Expected: GT_OK.
    1.2. Call with xonLimit [0x00EF].
    Expected: GT_OK.
    1.3. Call cpssExMxPmPortFlowControlXonLimitGet with non-NULL pointer.
    Expected: GT_OK and the same xonLimit.
    1.4. Call with xonLimit [0x800] > xoffLimit.
    Expected: GT_BAD_PARAM (The Global Xon limit must be not be greater than Global Xoff Limit).
    1.5. Call with out of range xonLimit [2048].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32 limit     = 0;
    GT_U32 limitGet  = 0;
    GT_U32 xoffLimit = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmPortFlowControlXoffLimitSet with xoffLimit [0x00FF].
            Expected: GT_OK.
        */
        xoffLimit = 0xFF;

        st = cpssExMxPmPortFlowControlXoffLimitSet(dev, xoffLimit);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortFlowControlXoffLimitSet: %d, %d", dev, xoffLimit);

        /*
           1.2. Call with xonLimit [0x00FE].
           Expected: GT_OK.
        */
        limit = 0x00FE;

        st = cpssExMxPmPortFlowControlXonLimitSet(dev, limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, limit);

        /*
            1.3. Call cpssExMxPmPortFlowControlXonLimitGet.
            Expected: GT_OK and the same xonLimit.
        */
        st = cpssExMxPmPortFlowControlXonLimitGet(dev, &limitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortFlowControlXonLimitGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(limit, limitGet,
                   "get another xonLimit than was set: %d", dev);

        /*
            1.4. Call with xonLimit [0x800] > xoffLimit.
            Expected: GT_BAD_PARAM (The Global Xon limit must be not be
            greater than Global Xoff Limit).
        */
        limit = 0x800;

        st = cpssExMxPmPortFlowControlXonLimitSet(dev, limit);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
                       "%d, xonLimit = %d > xoffLimit = %d", dev, limit, xoffLimit);

        /*
            1.5. Call with out of range xonLimit [2048].
            Expected: GT_BAD_PARAM.
        */
        limit = 2048;

        st = cpssExMxPmPortFlowControlXonLimitSet(dev, limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, limit);
    }

    limit = 0x00FE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXonLimitSet(dev, limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXonLimitSet(dev, limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXonLimitGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *xonLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXonLimitGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null xonLimitPtr.
    Expected: GT_OK.
    1.2. Call with xonLimitPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32 limit = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL xonLimitPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortFlowControlXonLimitGet(dev, &limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with xonLimitPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortFlowControlXonLimitGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d , xonLimitPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXonLimitGet(dev, &limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXonLimitGet(dev, &limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXoffLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   xoffLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXoffLimitSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with xoffLimit [0x00EF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortFlowControlXoffLimitGet with non-NULL pointer.
    Expected: GT_OK and the same xoffLimit.
    1.3. Call with out of range xoffLimit [2048].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32 limit    = 0;
    GT_U32 limitGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with xoffLimit [0x00FE].
           Expected: GT_OK.
        */
        limit = 0x00FE;

        st = cpssExMxPmPortFlowControlXoffLimitSet(dev, limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, limit);

        /*
            1.2. Call cpssExMxPmPortFlowControlXoffLimitGet.
            Expected: GT_OK and the same xoffLimit.
        */
        st = cpssExMxPmPortFlowControlXoffLimitGet(dev, &limitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortFlowControlXoffLimitGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(limit, limitGet,
                   "get another xoffLimit than was set: %d", dev);

        /*
            1.3. Call with out of range xoffLimit [2048].
            Expected: GT_BAD_PARAM.
        */
        limit = 2048;

        st = cpssExMxPmPortFlowControlXoffLimitSet(dev, limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, limit);
    }

    limit = 0x00FE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXoffLimitSet(dev, limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXoffLimitSet(dev, limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXoffLimitGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *xoffLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXoffLimitGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null xoffLimitPtr.
    Expected: GT_OK.
    1.2. Call with xoffLimitPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32 limit = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL xoffLimitPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortFlowControlXoffLimitGet(dev, &limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with xoffLimitPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortFlowControlXoffLimitGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d , xoffLimitPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXoffLimitGet(dev, &limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXoffLimitGet(dev, &limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXonLimitPortSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           xonLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXonLimitPortSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
                               CPSS_PORT_RX_FC_PROFILE_4_E]
                   and xonLimit [1 / 30].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortFlowControlXonLimitPortGet with profileSet from 1.1.
    Expected: GT_OK and the same xonLimit.
    1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E]
                   and out of range xonLimit [2048].
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range profileSet [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile  = CPSS_PORT_RX_FC_PROFILE_1_E;
    GT_U32                          limit    = 0;
    GT_U32                          limitGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
                                      CPSS_PORT_RX_FC_PROFILE_4_E]
                          and xonLimit[1 / 30].
           Expected: GT_OK.
        */

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E], xonLimit[1] */
        profile = CPSS_PORT_RX_FC_PROFILE_1_E;
        limit = 1;

        st = cpssExMxPmPortFlowControlXonLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profile, limit);

        /*
            1.2. Call cpssExMxPmPortFlowControlXonLimitPortGet with profileSet from 1.1.
            Expected: GT_OK and the same xonLimit.
        */
        st = cpssExMxPmPortFlowControlXonLimitPortGet(dev, profile, &limitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortFlowControlXonLimitPortGet: %d, %d", dev, profile);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(limit, limitGet,
                   "get another xonLimit than was set: %d, %d", dev, profile);

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_4_E], xonLimit[30] */
        profile = CPSS_PORT_RX_FC_PROFILE_4_E;
        limit = 30;

        st = cpssExMxPmPortFlowControlXonLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profile, limit);

        /*
            1.2. Call cpssExMxPmPortFlowControlXonLimitPortGet with profileSet from 1.1.
            Expected: GT_OK and the same xonLimit.
        */
        st = cpssExMxPmPortFlowControlXonLimitPortGet(dev, profile, &limitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortFlowControlXonLimitPortGet: %d, %d", dev, profile);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(limit, limitGet,
                   "get another xonLimit than was set: %d, %d", dev, profile);

        /*
            1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E]
                           and out of range xonLimit [2048].
            Expected: GT_BAD_PARAM.
        */
        profile = CPSS_PORT_RX_FC_PROFILE_3_E;
        limit = 2048;

        st = cpssExMxPmPortFlowControlXonLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, xonLimit = %d", dev, limit);

        limit = 1;

        /*
            1.4. Call with out of range profileSet [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        profile = PORT_BUF_MG_INVALID_ENUM_CNS;

        st = cpssExMxPmPortFlowControlXonLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profile);
    }

    profile = CPSS_PORT_RX_FC_PROFILE_1_E;
    limit   = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXonLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXonLimitPortSet(dev, profile, limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXonLimitPortGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *xonLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXonLimitPortGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_2_E /
                               CPSS_PORT_RX_FC_PROFILE_3_E]
                   and non-null xonLimitPtr.
    Expected: GT_OK.
    1.2. Call with xonLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with out of range profileSet [0x5AAAAAA5]
                   and non-null xonLimitPtr.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile = CPSS_PORT_RX_FC_PROFILE_1_E;
    GT_U32                          limit   = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_2_E /
                                       CPSS_PORT_RX_FC_PROFILE_3_E]
                           and non-null xonLimitPtr.
            Expected: GT_OK.
        */

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_2_E] */
        profile = CPSS_PORT_RX_FC_PROFILE_2_E;

        st = cpssExMxPmPortFlowControlXonLimitPortGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E] */
        profile = CPSS_PORT_RX_FC_PROFILE_3_E;

        st = cpssExMxPmPortFlowControlXonLimitPortGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /*
            1.2. Call with xonLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */

        st = cpssExMxPmPortFlowControlXonLimitPortGet(dev, profile, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xonLimitPtr = NULL", dev);

        /*
            1.3. Call with out of range profileSet [0x5AAAAAA5]
                           and non-null xonLimitPtr.
            Expected: GT_BAD_PARAM.
        */
        profile = PORT_BUF_MG_INVALID_ENUM_CNS;

        st = cpssExMxPmPortFlowControlXonLimitPortGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profile);
    }

    profile = CPSS_PORT_RX_FC_PROFILE_2_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXonLimitPortGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXonLimitPortGet(dev, profile, &limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXoffLimitPortSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           xoffLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXoffLimitPortSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
                               CPSS_PORT_RX_FC_PROFILE_4_E]
                   and xoffLimit [1 / 30].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortFlowControlXoffLimitPortGet with profileSet from 1.1.
    Expected: GT_OK and the same xoffLimit.
    1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E]
                   and out of range xoffLimit [2048].
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range profileSet [0x5AAAAAA5]
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile  = CPSS_PORT_RX_FC_PROFILE_1_E;
    GT_U32                          limit    = 0;
    GT_U32                          limitGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
                                      CPSS_PORT_RX_FC_PROFILE_4_E]
                          and xoffLimit[1 / 30].
           Expected: GT_OK.
        */

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E], xoffLimit[1] */
        profile = CPSS_PORT_RX_FC_PROFILE_1_E;
        limit = 1;

        st = cpssExMxPmPortFlowControlXoffLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profile, limit);

        /*
            1.2. Call cpssExMxPmPortFlowControlXoffLimitPortGet with profileSet from 1.1.
            Expected: GT_OK and the same xoffLimit.
        */
        st = cpssExMxPmPortFlowControlXoffLimitPortGet(dev, profile, &limitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortFlowControlXoffLimitPortGet: %d, %d", dev, profile);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(limit, limitGet,
                   "get another xoffLimit than was set: %d, %d", dev, profile);

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_4_E], xoffLimit[30] */
        profile = CPSS_PORT_RX_FC_PROFILE_4_E;
        limit = 30;

        st = cpssExMxPmPortFlowControlXoffLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profile, limit);

        /*
            1.2. Call cpssExMxPmPortFlowControlXoffLimitPortGet with profileSet from 1.1.
            Expected: GT_OK and the same xoffLimit.
        */
        st = cpssExMxPmPortFlowControlXoffLimitPortGet(dev, profile, &limitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortFlowControlXoffLimitPortGet: %d, %d", dev, profile);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(limit, limitGet,
                       "get another xoffLimit than was set: %d, %d", dev, profile);

        /*
            1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E]
                           and out of range xoffLimit [2048].
            Expected: GT_BAD_PARAM.
        */
        profile = CPSS_PORT_RX_FC_PROFILE_3_E;
        limit = 2048;

        st = cpssExMxPmPortFlowControlXoffLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, xoffLimit = %d", dev, limit);

        limit = 1;

        /*
            1.4. Call with out of range profileSet [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        profile = PORT_BUF_MG_INVALID_ENUM_CNS;

        st = cpssExMxPmPortFlowControlXoffLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profile);
    }

    profile = CPSS_PORT_RX_FC_PROFILE_1_E;
    limit   = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXoffLimitPortSet(dev, profile, limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXoffLimitPortSet(dev, profile, limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXoffLimitPortGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *xoffLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXoffLimitPortGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_2_E /
                               CPSS_PORT_RX_FC_PROFILE_3_E]
                   and non-null xoffLimitPtr.
    Expected: GT_OK.
    1.2. Call with xoffLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with out of range profileSet [0x5AAAAAA5]
                   and non-null xoffLimitPtr.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile = CPSS_PORT_RX_FC_PROFILE_1_E;
    GT_U32                          limit   = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_2_E /
                                       CPSS_PORT_RX_FC_PROFILE_3_E]
                           and non-null xoffLimitPtr.
            Expected: GT_OK.
        */

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_2_E] */
        profile = CPSS_PORT_RX_FC_PROFILE_2_E;

        st = cpssExMxPmPortFlowControlXoffLimitPortGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E] */
        profile = CPSS_PORT_RX_FC_PROFILE_3_E;

        st = cpssExMxPmPortFlowControlXoffLimitPortGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /*
            1.2. Call with xoffLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */

        st = cpssExMxPmPortFlowControlXoffLimitPortGet(dev, profile, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xoffLimitPtr = NULL", dev);

        /*
            1.3. Call with out of range profileSet [0x5AAAAAA5]
                           and non-null xoffLimitPtr.
            Expected: GT_BAD_PARAM.
        */
        profile = PORT_BUF_MG_INVALID_ENUM_CNS;

        st = cpssExMxPmPortFlowControlXoffLimitPortGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profile);
    }

    profile = CPSS_PORT_RX_FC_PROFILE_2_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXoffLimitPortGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXoffLimitPortGet(dev, profile, &limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlProfileSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlProfileSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
                                 CPSS_PORT_RX_FC_PROFILE_4_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortRxFlowControlProfileGet
    Expected: GT_OK and the same profileSet.
    1.1.3. Call with out of range profileSet [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_BUF_MG_VALID_PHY_PORT_CNS;

    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile = CPSS_PORT_RX_FC_PROFILE_1_E;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT pfGet   = CPSS_PORT_RX_FC_PROFILE_1_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
                                            CPSS_PORT_RX_FC_PROFILE_4_E].
               Expected: GT_OK.
            */

            /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E] */
            profile = CPSS_PORT_RX_FC_PROFILE_1_E;

            st = cpssExMxPmPortFlowControlProfileSet(dev, port, profile);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profile);

            /*
                1.1.2. Call cpssExMxPmPortRxFlowControlProfileGet
                Expected: GT_OK and the same profileSet.
            */
            st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, &pfGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortRxFlowControlProfileGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(profile, pfGet,
                       "get another profileSet than was set: %d, %d", dev, port);

            /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_4_E] */
            profile = CPSS_PORT_RX_FC_PROFILE_4_E;

            st = cpssExMxPmPortFlowControlProfileSet(dev, port, profile);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profile);

            /*
                1.1.2. Call cpssExMxPmPortRxFlowControlProfileGet
                Expected: GT_OK and the same profileSet.
            */
            st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, &pfGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortRxFlowControlProfileGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(profile, pfGet,
                       "get another profileSet than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range profileSet [0x5AAAAAA5]
                Expected: GT_BAD_PARAM.
            */
            profile = PORT_BUF_MG_INVALID_ENUM_CNS;

            st = cpssExMxPmPortFlowControlProfileSet(dev, port, profile);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, profile);
        }

        profile = CPSS_PORT_RX_FC_PROFILE_1_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlProfileSet(dev, port, profile);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlProfileSet(dev, port, profile);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlProfileSet(dev, port, profile);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    profile = CPSS_PORT_RX_FC_PROFILE_1_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_BUF_MG_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlProfileSet(dev, port, profile);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlProfileSet(dev, port, profile);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortRxFlowControlProfileGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            portNum,
    OUT CPSS_PORT_RX_FC_PROFILE_SET_ENT  *profileSetPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortRxFlowControlProfileGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null profileSetPtr.
    Expected: GT_OK.
    1.1.2. Call with profileSetPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_BUF_MG_VALID_PHY_PORT_CNS;

    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile = CPSS_PORT_RX_FC_PROFILE_1_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null profileSetPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, &profile);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with profileSetPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, profileSetPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, &profile);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, &profile);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, &profile);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_BUF_MG_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, &profile);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortRxFlowControlProfileGet(dev, port, &profile);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortRxBufLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           rxBufLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortRxBufLimitSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
                               CPSS_PORT_RX_FC_PROFILE_4_E]
                   and rxBufLimit [10 / 100].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortRxBufLimitGet with profileSet from 1.1.
    Expected: GT_OK and the same rxBufLimit.
    1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E]
                   and rxBufLimit [1024].
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range profileSet [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile  = CPSS_PORT_RX_FC_PROFILE_1_E;
    GT_U32                          limit    = 0;
    GT_U32                          limitGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
                                      CPSS_PORT_RX_FC_PROFILE_4_E]
                          and rxBufLimit [10 / 100].
            Expected: GT_OK.
        */

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E], rxBufLimit[10] */
        profile = CPSS_PORT_RX_FC_PROFILE_1_E;
        limit   = 10;

        st = cpssExMxPmPortRxBufLimitSet(dev, profile, limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profile, limit);

        /*
            1.2. Call cpssExMxPmPortRxBufLimitGet with profileSet from 1.1.
            Expected: GT_OK and the same rxBufLimit.
        */
        st = cpssExMxPmPortRxBufLimitGet(dev, profile, &limitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortRxBufLimitGet: %d, %d", dev, profile);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(limit, limitGet,
                   "get another rxBufLimit than was set: %d, %d", dev, profile);

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E], rxBufLimit[100] */
        profile = CPSS_PORT_RX_FC_PROFILE_4_E;
        limit   = 100;

        st = cpssExMxPmPortRxBufLimitSet(dev, profile, limit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profile, limit);

        /*
            1.2. Call cpssExMxPmPortRxBufLimitGet with profileSet from 1.1.
            Expected: GT_OK and the same rxBufLimit.
        */
        st = cpssExMxPmPortRxBufLimitGet(dev, profile, &limitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortRxBufLimitGet: %d, %d", dev, profile);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(limit, limitGet,
                   "get another rxBufLimit than was set: %d, %d", dev, profile);

        /*
            1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E]
                           and rxBufLimit [1024].
            Expected: GT_BAD_PARAM.
        */
        profile = CPSS_PORT_RX_FC_PROFILE_3_E;
        limit   = 1024;

        st = cpssExMxPmPortRxBufLimitSet(dev, profile, limit);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, rxBufLimit = %d", dev, limit);

        limit = 10;

        /*
            1.4. Call with out of range profileSet [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        profile = PORT_BUF_MG_INVALID_ENUM_CNS;

        st = cpssExMxPmPortRxBufLimitSet(dev, profile, limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profile);
    }

    profile = CPSS_PORT_RX_FC_PROFILE_1_E;
    limit   = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortRxBufLimitSet(dev, profile, limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortRxBufLimitSet(dev, profile, limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortRxBufLimitGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *rxBufLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortRxBufLimitGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileSet [CPCPSS_PORT_RX_FC_PROFILE_2_E /
                               CPSS_PORT_RX_FC_PROFILE_3_E]
                   and non-null rxBufLimitPtr.
    Expected: GT_OK.
    1.2. Call with rxBufLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with out of range profileSet [0x5AAAAAA5]
                   and non-null rxBufLimitPtr.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile  = CPSS_PORT_RX_FC_PROFILE_1_E;
    GT_U32                          limit    = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileSet [CPCPSS_PORT_RX_FC_PROFILE_2_E /
                                      CPSS_PORT_RX_FC_PROFILE_3_E]
                           and non-null rxBufLimitPtr.
            Expected: GT_OK.
        */

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_2_E] */
        profile = CPSS_PORT_RX_FC_PROFILE_2_E;

        st = cpssExMxPmPortRxBufLimitGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /* Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E] */
        profile = CPSS_PORT_RX_FC_PROFILE_3_E;

        st = cpssExMxPmPortRxBufLimitGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profile);

        /*
            1.2. Call with rxBufLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortRxBufLimitGet(dev, profile, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, rxBufLimitPtr = NULL", dev, profile);

        /*
            1.3. Call with out of range profileSet [0x5AAAAAA5]
                           and non-null rxBufLimitPtr.
            Expected: GT_BAD_PARAM.
        */
        profile = PORT_BUF_MG_INVALID_ENUM_CNS;

        st = cpssExMxPmPortRxBufLimitGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profile);
    }

    profile = CPSS_PORT_RX_FC_PROFILE_1_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortRxBufLimitGet(dev, profile, &limit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortRxBufLimitGet(dev, profile, &limit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlHolSysModeSet
(
    IN  GT_U8                     devNum,
    IN  CPSS_EXMXPM_PORT_HOL_FC_ENT modeFcHol
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlHolSysModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with modeFcHol [CPSS_EXMXPM_PORT_HOL_E /
                              CPSS_EXMXPM_PORT_FC_E].
    Expected: GT_OK.
    1.2. Call with out of range modeFcHol [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PORT_HOL_FC_ENT modeFcHol = CPSS_EXMXPM_PORT_FC_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with modeFcHol [CPSS_EXMXPM_PORT_HOL_E /
                                     CPSS_EXMXPM_PORT_FC_E].
            Expected: GT_OK.
        */

        /* Call with modeFcHol [CPSS_EXMXPM_PORT_HOL_E] */
        modeFcHol = CPSS_EXMXPM_PORT_HOL_E;

        st = cpssExMxPmPortFlowControlHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, modeFcHol);

        /* Call with modeFcHol [CPSS_EXMXPM_PORT_FC_E] */
        modeFcHol = CPSS_EXMXPM_PORT_FC_E;

        st = cpssExMxPmPortFlowControlHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, modeFcHol);

        /*
            1.2. Call with out of range modeFcHol [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        modeFcHol = PORT_BUF_MG_INVALID_ENUM_CNS;

        st = cpssExMxPmPortFlowControlHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, modeFcHol);
    }

    modeFcHol = CPSS_EXMXPM_PORT_HOL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlHolSysModeSet(dev, modeFcHol);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmPortBufMg suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmPortBufMg)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXonLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXonLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXoffLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXoffLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXonLimitPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXonLimitPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXoffLimitPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXoffLimitPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortRxFlowControlProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortRxBufLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortRxBufLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlHolSysModeSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmPortBufMg)

