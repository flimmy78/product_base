/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
*  cpssExMxPolicyUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPolicy, that provides 
*   CPSS EXMX Policy Engine common settings API.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/policy/cpssExMxPolicy.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */
#define POLICY_INVALID_ENUM_CNS     0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
cpssExMxPolicyLookupModeSet
(
    IN  GT_U8                             dev,
    IN  CPSS_EXMX_POLICY_LOOKUP_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicyLookupModeSet)
{
/*
ITERATE_DEVICES (ExMx)
1.1. Call with mode [CPSS_EXMX_POLICY_LOOKUP_MODE_TCAM_DEFAULT_E
 / CPSS_EXMX_POLICY_LOOKUP_MODE_TCAM_IP_DEFAULT_E / CPSS_EXMX_POLICY_LOOKUP_MODE_DEFAULT_E].
Expected: GT_OK.
1.2. Call with out of range mode [0x5AAAAAA5].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS                           st = GT_OK;
    GT_U8                               dev;
    CPSS_EXMX_POLICY_LOOKUP_MODE_ENT    mode;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with mode [CPSS_EXMX_POLICY_LOOKUP_MODE_TCAM_DEFAULT_E
        / CPSS_EXMX_POLICY_LOOKUP_MODE_TCAM_IP_DEFAULT_E / CPSS_EXMX_POLICY_LOOKUP_MODE_DEFAULT_E].
        Expected: GT_OK. */
        mode = CPSS_EXMX_POLICY_LOOKUP_MODE_TCAM_DEFAULT_E;

        st = cpssExMxPolicyLookupModeSet(dev,mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        mode = CPSS_EXMX_POLICY_LOOKUP_MODE_TCAM_IP_DEFAULT_E;

        st = cpssExMxPolicyLookupModeSet(dev,mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        mode = CPSS_EXMX_POLICY_LOOKUP_MODE_DEFAULT_E;

        st = cpssExMxPolicyLookupModeSet(dev,mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* 1.2. Call with out of range mode [0x5AAAAAA5].
        Expected: GT_BAD_PARAM. */
        mode = POLICY_INVALID_ENUM_CNS;

        st = cpssExMxPolicyLookupModeSet(dev,mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }
    mode = CPSS_EXMX_POLICY_LOOKUP_MODE_TCAM_DEFAULT_E;
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPolicyLookupModeSet(dev,mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicyLookupModeSet(dev,mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssExMxPolicyCosParamModifyEnable
(
    IN GT_U8              dev,
    IN CPSS_COS_PARAM_ENT cosParam,
    IN GT_BOOL            enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicyCosParamModifyEnable)
{
/*
ITERATE_DEVICES (ExMx)
1.1. Call with cosParam [CPSS_COS_PARAM_DSCP_E / CPSS_COS_PARAM_EXP_E 
/ CPSS_COS_PARAM_TC_E / CPSS_COS_PARAM_UP_E / CPSS_COS_PARAM_DP_E] 
and enable [GT_FALSE and GT_TRUE].
Expected: GT_OK for not 98EX1x6 device and NOT GT_OK for 98EX1x6 device.
1.2. For non-Tiger devices call with out of range cosParam [0x5AAAAAA5] and other parameters from 1.1.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_COS_PARAM_ENT      cosParam;
    GT_BOOL                 enable;
    CPSS_PP_FAMILY_TYPE_ENT             family;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &family);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Call with cosParam [CPSS_COS_PARAM_DSCP_E / CPSS_COS_PARAM_EXP_E 
        / CPSS_COS_PARAM_TC_E / CPSS_COS_PARAM_UP_E / CPSS_COS_PARAM_DP_E] 
        and enable [GT_FALSE and GT_TRUE].
        Expected: GT_OK for not 98EX1x6 device and NOT GT_OK for 98EX1x6 device. */
        cosParam = CPSS_COS_PARAM_DSCP_E;
        enable = GT_TRUE;

        st = cpssExMxPolicyCosParamModifyEnable(dev, cosParam, enable);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d", dev, cosParam, enable);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cosParam, enable);
        }

        cosParam = CPSS_COS_PARAM_EXP_E;
        enable = GT_FALSE;

        st = cpssExMxPolicyCosParamModifyEnable(dev, cosParam, enable);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d", dev, cosParam, enable);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cosParam, enable);
        }

        cosParam = CPSS_COS_PARAM_TC_E;
        enable = GT_TRUE;

        st = cpssExMxPolicyCosParamModifyEnable(dev, cosParam, enable);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d", dev, cosParam, enable);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cosParam, enable);
        }

        cosParam = CPSS_COS_PARAM_UP_E;
        enable = GT_FALSE;

        st = cpssExMxPolicyCosParamModifyEnable(dev, cosParam, enable);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d", dev, cosParam, enable);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cosParam, enable);
        }

        cosParam = CPSS_COS_PARAM_DP_E;
        enable = GT_TRUE;

        st = cpssExMxPolicyCosParamModifyEnable(dev, cosParam, enable);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d", dev, cosParam, enable);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cosParam, enable);
        }

        /* 1.2. For non Tiger devices call with out of range cosParam [0x5AAAAAA5]
           and other parameters from 1.1. Expected: GT_BAD_PARAM.   */

        if (CPSS_PP_FAMILY_TIGER_E != family) 
        {
            cosParam = POLICY_INVALID_ENUM_CNS;
            enable = GT_TRUE;

            st = cpssExMxPolicyCosParamModifyEnable(dev, cosParam, enable);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cosParam, enable);
        }
    }
    cosParam = CPSS_COS_PARAM_DSCP_E;
    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPolicyCosParamModifyEnable(dev, cosParam, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicyCosParamModifyEnable(dev, cosParam, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssExMxPolicyRstFinModeSet
(
    IN GT_U8                            dev,
    IN CPSS_EXMX_POLICY_RST_FIN_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicyRstFinModeSet)
{
/*
ITERATE_DEVICES (ExMx)
1.1. Call with mode [CPSS_EXMX_POLICY_RST_FIN_MODE_NO_ACTION / 
CPSS_EXMX_POLICY_RST_FIN_MODE_MIRROR_TO_CPU_E].
Expected: GT_OK.
1.2. Call with mode [CPSS_EXMX_POLICY_RST_FIN_MODE_PASS_TO_EPI_E] 
(not supported for 98MX6x5/8 and 98EX1x6).
Expected: NON GT_OK for Samba and Tiger and GT_OK for others.
1.3. Call with out of range mode [0x5AAAAAA5].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS                           st = GT_OK;
    GT_U8                               dev;
    CPSS_EXMX_POLICY_RST_FIN_MODE_ENT   mode;
    CPSS_PP_FAMILY_TYPE_ENT             family;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &family);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Call with mode [CPSS_EXMX_POLICY_RST_FIN_MODE_NO_ACTION / 
        CPSS_EXMX_POLICY_RST_FIN_MODE_MIRROR_TO_CPU_E].
        Expected: GT_OK. */
        mode = CPSS_EXMX_POLICY_RST_FIN_MODE_NO_ACTION;

        st = cpssExMxPolicyRstFinModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        mode = CPSS_EXMX_POLICY_RST_FIN_MODE_MIRROR_TO_CPU_E;

        st = cpssExMxPolicyRstFinModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* 1.2. Call with mode [CPSS_EXMX_POLICY_RST_FIN_MODE_PASS_TO_EPI_E] 
        (not supported for 98MX6x5/8 and 98EX1x6).
        Expected: NON GT_OK for Samba and Tiger and GT_OK for others. */
        mode = CPSS_EXMX_POLICY_RST_FIN_MODE_PASS_TO_EPI_E;

        st = cpssExMxPolicyRstFinModeSet(dev, mode);

        if (CPSS_PP_FAMILY_SAMBA_E == family || CPSS_PP_FAMILY_TIGER_E == family)
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "Samba or Tiger device: %d, %d", dev, mode);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);
        }

        /* 1.3. Call with out of range mode [0x5AAAAAA5].
        Expected: GT_BAD_PARAM. */
        mode = POLICY_INVALID_ENUM_CNS;

        st = cpssExMxPolicyRstFinModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }
    mode = CPSS_EXMX_POLICY_RST_FIN_MODE_NO_ACTION;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPolicyRstFinModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicyRstFinModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssExMxPolicyRedirectMtuExceedCmdSet
(
   IN GT_U8                               dev,
   IN CPSS_EXMX_POLICY_MTU_EXCEED_CMD_ENT exceedCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicyRedirectMtuExceedCmdSet)
{
/*
ITERATE_DEVICES (ExMx)
1.1. Call with exceedCmd [CPSS_EXMX_POLICY_MTU_EXCEED_DROP_E / 
CPSS_EXMX_POLICY_MTU_EXCEED_TRAP_E / CPSS_EXMX_POLICY_MTU_EXCEED_MIRROR_E 
/ CPSS_EXMX_POLICY_MTU_EXCEED_IGNORE_E].
Expected: GT_OK for 98EX1x6 devices and NOT GT_OK for others.
1.2. For 98EX1x6 call with out of range exceedCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;
    CPSS_EXMX_POLICY_MTU_EXCEED_CMD_ENT     exceedCmd;
    CPSS_PP_FAMILY_TYPE_ENT                 family;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &family);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Call with exceedCmd [CPSS_EXMX_POLICY_MTU_EXCEED_DROP_E / 
        CPSS_EXMX_POLICY_MTU_EXCEED_TRAP_E / CPSS_EXMX_POLICY_MTU_EXCEED_MIRROR_E 
        / CPSS_EXMX_POLICY_MTU_EXCEED_IGNORE_E].
        Expected: GT_OK for 98EX1x6 devices and NOT GT_OK for others. */
        exceedCmd = CPSS_EXMX_POLICY_MTU_EXCEED_DROP_E;

        st = cpssExMxPolicyRedirectMtuExceedCmdSet(dev, exceedCmd);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, exceedCmd);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceedCmd);
        }

        exceedCmd = CPSS_EXMX_POLICY_MTU_EXCEED_TRAP_E;

        st = cpssExMxPolicyRedirectMtuExceedCmdSet(dev, exceedCmd);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, exceedCmd);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceedCmd);
        }

        exceedCmd = CPSS_EXMX_POLICY_MTU_EXCEED_MIRROR_E;

        st = cpssExMxPolicyRedirectMtuExceedCmdSet(dev, exceedCmd);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, exceedCmd);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceedCmd);
        }

        exceedCmd = CPSS_EXMX_POLICY_MTU_EXCEED_IGNORE_E;

        st = cpssExMxPolicyRedirectMtuExceedCmdSet(dev, exceedCmd);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, exceedCmd);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceedCmd);
        }

        /* 1.2. For 98EX1x6 call with out of range exceedCmd [0x5AAAAAA5].
           Expected: GT_BAD_PARAM.  */

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            exceedCmd = POLICY_INVALID_ENUM_CNS;

            st = cpssExMxPolicyRedirectMtuExceedCmdSet(dev, exceedCmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "Tiger device: %d, %d", dev, exceedCmd);
        }
    }
    exceedCmd = CPSS_EXMX_POLICY_MTU_EXCEED_DROP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPolicyRedirectMtuExceedCmdSet(dev, exceedCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicyRedirectMtuExceedCmdSet(dev, exceedCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssExMxPolicyRedirectMtuSet
(
    IN GT_U8    dev,
    IN GT_U32   mtu
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicyRedirectMtuSet)
{
/*
ITERATE_DEVICES (ExMx)
1.1. Call with mtu [0 / 1000 / 0xFFFF].
Expected: GT_OK.
1.2. Call with out of range mtu [65536].
Expected: NOT GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U32          mtu;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with mtu [0 / 1000 / 0xFFFF].
        Expected: GT_OK. */
        mtu = 0;

        st = cpssExMxPolicyRedirectMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtu);

        mtu = 1000;

        st = cpssExMxPolicyRedirectMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtu);

        mtu = 0xFFFF;

        st = cpssExMxPolicyRedirectMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtu);

        /* 1.2. Call with out of range mtu [65536].
        Expected: NOT GT_OK. */
        mtu = 65536;

        st = cpssExMxPolicyRedirectMtuSet(dev, mtu);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtu);
    }
    mtu = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPolicyRedirectMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicyRedirectMtuSet(dev, mtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssExMxPolicyTrapTcSet
(
    IN GT_U8   dev,
    IN GT_U8   trafClass
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicyTrapTcSet)
{
/*
ITERATE_DEVICES (ExMx)
1.1. Call with trafClass [0 / 5 / 7].
Expected: GT_OK.
1.2. Call with out of range trafClass [8].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           trafClass;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trafClass [0 / 5 / 7].
        Expected: GT_OK. */
        trafClass = 0;

        st = cpssExMxPolicyTrapTcSet(dev, trafClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trafClass);

        trafClass = 5;

        st = cpssExMxPolicyTrapTcSet(dev, trafClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trafClass);

        trafClass = 7;

        st = cpssExMxPolicyTrapTcSet(dev, trafClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trafClass);

        /* 1.2. Call with out of range trafClass [8].
        Expected: GT_BAD_PARAM. */
        trafClass = 8;

        st = cpssExMxPolicyTrapTcSet(dev, trafClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trafClass);
    }
    trafClass = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPolicyTrapTcSet(dev, trafClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicyTrapTcSet(dev, trafClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPolicy suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPolicy)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicyLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicyCosParamModifyEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicyRstFinModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicyRedirectMtuExceedCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicyRedirectMtuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicyTrapTcSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPolicy)


