/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxTgIpCtrlUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxTgIpCtrl, that provides
*       the CPSS EXMX Tiger Ip HW control registers APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxTg/ip/cpssExMxTgIpCtrl.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define TG_IP_CTRL_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpMcMirrorCpuCodeEnable
(
    IN GT_U8                devNum,
    IN GT_BOOL              enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpMcMirrorCpuCodeEnable)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS       st     = GT_OK;

    GT_U8           dev;
    GT_BOOL         enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxTgIpMcMirrorCpuCodeEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxTgIpMcMirrorCpuCodeEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpMcMirrorCpuCodeEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpMcMirrorCpuCodeEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpCtrlEcmpHashModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT           protocolStack,
    IN CPSS_EXMX_TG_IP_ECMP_HASH_MODE_ENT   ecmpHashMode 
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpCtrlEcmpHashModeSet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E]
                   and ecmpHashMode[CPSS_EXMX_TG_IP_ECMP_REGULAR_HASH_MODE_E /
                                    CPSS_EXMX_TG_IP_ECMP_USE_PROT_HASH_MODE_E].
    Expected: GT_OK.
    1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range ecmpHashMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                          st            = GT_OK;

    GT_U8                              dev;
    CPSS_IP_PROTOCOL_STACK_ENT         protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_EXMX_TG_IP_ECMP_HASH_MODE_ENT ecmpHashMode  = CPSS_EXMX_TG_IP_ECMP_REGULAR_HASH_MODE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E]
                           and ecmpHashMode[CPSS_EXMX_TG_IP_ECMP_REGULAR_HASH_MODE_E /
                                            CPSS_EXMX_TG_IP_ECMP_USE_PROT_HASH_MODE_E].
            Expected: GT_OK.
        */

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] and ecmpHashMode[CPSS_EXMX_TG_IP_ECMP_REGULAR_HASH_MODE_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        ecmpHashMode  = CPSS_EXMX_TG_IP_ECMP_REGULAR_HASH_MODE_E;

        st = cpssExMxTgIpCtrlEcmpHashModeSet(dev, protocolStack, ecmpHashMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, ecmpHashMode);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] and ecmpHashMode[CPSS_EXMX_TG_IP_ECMP_USE_PROT_HASH_MODE_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        ecmpHashMode  = CPSS_EXMX_TG_IP_ECMP_USE_PROT_HASH_MODE_E;

        st = cpssExMxTgIpCtrlEcmpHashModeSet(dev, protocolStack, ecmpHashMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, ecmpHashMode);

        /*
            1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
        ecmpHashMode  = CPSS_EXMX_TG_IP_ECMP_REGULAR_HASH_MODE_E;

        st = cpssExMxTgIpCtrlEcmpHashModeSet(dev, protocolStack, ecmpHashMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.3. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = TG_IP_CTRL_INVALID_ENUM_CNS;
        
        st = cpssExMxTgIpCtrlEcmpHashModeSet(dev, protocolStack, ecmpHashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.4. Call with out of range ecmpHashMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ecmpHashMode = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpCtrlEcmpHashModeSet(dev, protocolStack, ecmpHashMode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ecmpHashMode = %d", dev, ecmpHashMode);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    ecmpHashMode  = CPSS_EXMX_TG_IP_ECMP_REGULAR_HASH_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpCtrlEcmpHashModeSet(dev, protocolStack, ecmpHashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpCtrlEcmpHashModeSet(dev, protocolStack, ecmpHashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpCtrlCosRoutingModeSet
(
    IN GT_U8                                     devNum,
    IN CPSS_EXMX_TG_IP_COS_ROUTING_HASH_MODE_ENT cosRouteMode 
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpCtrlCosRoutingModeSet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with cosRouteMode [CPSS_EXMX_TG_IP_COS_ROUTING_LSB_HASH_MODE_E /
                                 CPSS_EXMX_TG_IP_COS_ROUTING_MOD_HASH_MODE_E].
    Expected: GT_OK.
    1.2. Call with out of range cosRouteMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                                 st           = GT_OK;

    GT_U8                                     dev;
    CPSS_EXMX_TG_IP_COS_ROUTING_HASH_MODE_ENT cosRouteMode = CPSS_EXMX_TG_IP_COS_ROUTING_LSB_HASH_MODE_E;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cosRouteMode [CPSS_EXMX_TG_IP_COS_ROUTING_LSB_HASH_MODE_E /
                                         CPSS_EXMX_TG_IP_COS_ROUTING_MOD_HASH_MODE_E].
            Expected: GT_OK.
        */

        /* Call with cosRouteMode [CPSS_EXMX_TG_IP_COS_ROUTING_LSB_HASH_MODE_E] */
        cosRouteMode = CPSS_EXMX_TG_IP_COS_ROUTING_LSB_HASH_MODE_E;

        st = cpssExMxTgIpCtrlCosRoutingModeSet(dev, cosRouteMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cosRouteMode);

        /* Call with cosRouteMode [CPSS_EXMX_TG_IP_COS_ROUTING_MOD_HASH_MODE_E] */
        cosRouteMode = CPSS_EXMX_TG_IP_COS_ROUTING_MOD_HASH_MODE_E;

        st = cpssExMxTgIpCtrlCosRoutingModeSet(dev, cosRouteMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cosRouteMode);

        /*
            1.2. Call with out of range cosRouteMode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        cosRouteMode = TG_IP_CTRL_INVALID_ENUM_CNS;
        
        st = cpssExMxTgIpCtrlCosRoutingModeSet(dev, cosRouteMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cosRouteMode);
    }

    cosRouteMode = CPSS_EXMX_TG_IP_COS_ROUTING_LSB_HASH_MODE_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpCtrlCosRoutingModeSet(dev, cosRouteMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpCtrlCosRoutingModeSet(dev, cosRouteMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpCtrlMcMacMatchEnable
(
    IN GT_U8                            devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN GT_BOOL                          enable 
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpCtrlMcMacMatchEnable)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                  st            = GT_OK;

    GT_U8                      dev;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable        = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, enable);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, enable);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, enable);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, enable);

        /*
            1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.3. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);
    }

    enable        = GT_FALSE;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpCtrlL2L3ByteCountMatchEnable
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      enable 
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpCtrlL2L3ByteCountMatchEnable)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                  st            = GT_OK;

    GT_U8                      dev;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable        = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, enable);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, enable);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, enable);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, enable);

        /*
            1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.3. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);
    }

    enable        = GT_FALSE;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpv6AddrPrefixScopeSet
(
    IN  GT_U8                           dev,
    IN  GT_IPV6ADDR                     prefix,
    IN  GT_U32                          prefixLen,
    IN  CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addressScope, 
    IN  GT_U32                          prefixScopeIndex 
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpv6AddrPrefixScopeSet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with prefix [10.15.0.1],
                   prefixLen [0 / 1 / 8 / 16],
                   addressScope [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                 CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                 CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                 CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]
                   and prefixScopeIndex[0 / 1 / 2 / 3] .
    Expected: GT_OK.
    1.2. Call with out of range prefixLen [17]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range addressScope [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range prefixScopeIndex [5]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS                       st           = GT_OK;

    GT_U8                           dev;
    GT_IPV6ADDR                     prefix;
    GT_U32                          prefixLen    = 0;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addressScope = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_U32                          prefixIndex  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with prefix [10.15.0.1],
                           prefixLen [0 / 1 / 8 / 16],
                           addressScope [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                         CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                         CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                         CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]
                           and prefixScopeIndex[0 / 1 / 2 / 3] .
            Expected: GT_OK.
        */
        prefix.u32Ip[0] = 10;
        prefix.u32Ip[1] = 15;
        prefix.u32Ip[2] = 0;
        prefix.u32Ip[3] = 1;

        /* Call with prefixLen [0]                                           */
        /*           addressScope [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E] */ 
        /*           prefixScopeIndex[0]                                     */
        prefixLen    = 0;
        addressScope = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        prefixIndex  = 0;

        st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, prefixLen = %d, addressScope = %d, prefixScopeIndex = %d",
                                     dev, prefixLen, addressScope, prefixIndex);

        /* Call with prefixLen [1]                                           */
        /*           addressScope [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E] */ 
        /*           prefixScopeIndex[1]                                     */
        prefixLen    = 1;
        addressScope = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        prefixIndex  = 1;

        st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, prefixLen = %d, addressScope = %d, prefixScopeIndex = %d",
                                     dev, prefixLen, addressScope, prefixIndex);

        /* Call with prefixLen [8]                                             */
        /*           addressScope [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E] */ 
        /*           prefixScopeIndex[2]                                       */
        prefixLen    = 8;
        addressScope = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        prefixIndex  = 2;

        st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, prefixLen = %d, addressScope = %d, prefixScopeIndex = %d",
                                     dev, prefixLen, addressScope, prefixIndex);

        /* Call with prefixLen [16]                                      */
        /*           addressScope [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E] */ 
        /*           prefixScopeIndex[3]                                 */
        prefixLen    = 16;
        addressScope = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        prefixIndex  = 3;

        st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, prefixLen = %d, addressScope = %d, prefixScopeIndex = %d",
                                     dev, prefixLen, addressScope, prefixIndex);

        /*
            1.2. Call with out of range prefixLen [17]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        prefixLen = 17;

        st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", dev, prefixLen);

        prefixLen = 0;

        /*
            1.3. Call with out of range addressScope [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScope = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScope = %d", dev, addressScope);

        addressScope = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.4. Call with out of range prefixScopeIndex [5]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        prefixIndex = 5;

        st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, prefixIndex = %d", dev, prefixIndex);
    }

    prefix.u32Ip[0] = 10;
    prefix.u32Ip[1] = 15;
    prefix.u32Ip[2] = 0;
    prefix.u32Ip[3] = 1;

    prefixLen    = 0;
    addressScope = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    prefixIndex  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen, addressScope, prefixIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpv6UcBorderCrossCmdSet
(
    IN GT_U8                           dev,
    IN CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addressScopeSrc, 
    IN CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addressScopeDest, 
    IN GT_BOOL                         borderCrossed,
    IN CPSS_EXMX_IP_EXCP_CMD_ENT       cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpv6UcBorderCrossCmdSet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                    CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                    CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                    CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                     CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                     CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                     CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   borderCrossed [GT_FALSE and GT_TRUE]
                   and cmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                            CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                            CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
                            CPSS_EXMX_IP_EXCP_CMD_ROUTE_E].
    Expected: GT_OK.
    1.2. Call with cmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range addressScopeSrc [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range addressScopeDest [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range cmd [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                       st            = GT_OK;

    GT_U8                           dev;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_BOOL                         borderCrossed = GT_FALSE;
    CPSS_EXMX_IP_EXCP_CMD_ENT       cmd           = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                            CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                            CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                            CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                             CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                             CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                             CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           borderCrossed [GT_FALSE and GT_TRUE]
                           and cmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                                    CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                                    CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
                                    CPSS_EXMX_IP_EXCP_CMD_ROUTE_E].
            Expected: GT_OK.
        */

        /* Call with borderCrossed [GT_FALSE] */
        borderCrossed = GT_FALSE;

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E] */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]    */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E]                    */
        addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        cmd           = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E]    */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E] */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E]                */
        addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        cmd           = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E] */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E]  */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]                      */
        addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        cmd           = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]      */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E] */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]                         */
        addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        cmd           = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);

        /* Call with borderCrossed [GT_TRUE] */
        borderCrossed = GT_TRUE;

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E] */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]    */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E]                    */
        addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        cmd           = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E]    */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E] */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E]                */
        addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        cmd           = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E] */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E]  */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]                      */
        addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        cmd           = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]      */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E] */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]                         */
        addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        cmd           = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);

        /*
            1.2. Call with cmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        cmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

        /*
            1.3. Call with out of range addressScopeSrc [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addrScopeSrc = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeSrc = %d", dev, addrScopeSrc);

        addrScopeSrc = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.4. Call with out of range addressScopeDest [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addrScopeDest = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeDest = %d", dev, addrScopeDest);

        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.5. Call with out of range cmd [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        cmd = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cmd = %d", dev, cmd);
    }

    borderCrossed = GT_FALSE;
    addrScopeSrc  = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
    cmd           = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpv6McBorderCrossCmdSet
(
    IN GT_U8                                    dev,
    IN CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT          addressScopeSrc,
    IN CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT          addressScopeDest,
    IN GT_BOOL                                  borderCrossed,
    IN CPSS_EXMX_IP_EXCP_CMD_ENT                cmd,
    IN CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_ENT mllSelectionRule
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpv6McBorderCrossCmdSet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                    CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                    CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                    CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                     CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                     CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                     CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   borderCrossed [GT_FALSE and GT_TRUE],
                   cmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                        CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                        CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
                        CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]
                   and mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E /
                                         CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E].
    Expected: GT_OK.
    1.2. Call with cmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range addressScopeSrc [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range addressScopeDest [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range cmd [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range mllSelectionRule [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                                st               = GT_OK;

    GT_U8                                    dev;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT          addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT          addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_BOOL                                  borderCrossed    = GT_FALSE;
    CPSS_EXMX_IP_EXCP_CMD_ENT                cmd              = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;
    CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_ENT mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                            CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                            CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                            CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                             CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                             CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                             CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           borderCrossed [GT_FALSE and GT_TRUE],
                           cmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                                CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                                CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
                                CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]
                           and mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E /
                                                 CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E].
            Expected: GT_OK.
        */

        /* Call with borderCrossed [GT_FALSE] */
        borderCrossed = GT_FALSE;

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]      */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]         */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E]                         */
        /*           mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E] */
        addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        cmd              = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;
        mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E]      */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E]   */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E]                  */
        /*           mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E] */
        addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        cmd              = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;
        mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E]     */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E]      */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]                          */
        /*           mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E] */
        addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        cmd              = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;
        mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]           */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]      */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]                              */
        /*           mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E] */
        addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        cmd              = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;
        mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);

        /* Call with borderCrossed [GT_TRUE] */
        borderCrossed = GT_TRUE;

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]      */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]         */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E]                         */
        /*           mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E] */
        addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        cmd              = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;
        mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E]      */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E]   */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E]                  */
        /*           mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E] */
        addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        cmd              = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;
        mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E]     */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E]      */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]                          */
        /*           mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E] */
        addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        cmd              = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;
        mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);

        /* Call with addressScopeSrc [CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E]           */
        /*           addressScopeDest [CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]      */
        /*           cmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]                              */
        /*           mllSelectionRule [CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E] */
        addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        cmd              = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;
        mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_GLOBAL_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);

        /*
            1.2. Call with cmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        cmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

        /*
            1.3. Call with out of range addressScopeSrc [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addrScopeSrc = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeSrc = %d", dev, addrScopeSrc);

        addrScopeSrc = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.4. Call with out of range addressScopeDest [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addrScopeDest = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeDest = %d", dev, addrScopeDest);

        addrScopeDest = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.5. Call with out of range cmd [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        cmd = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cmd = %d", dev, cmd);

        cmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

        /*
            1.6. Call with out of range mllSelectionRule [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mllSelectionRule = TG_IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, mllSelectionRule = %d", dev, mllSelectionRule);
    }

    borderCrossed    = GT_FALSE;
    addrScopeSrc     = CPSS_EXMX_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    addrScopeDest    = CPSS_EXMX_IPV6_PREFIX_SCOPE_GLOBAL_E;
    cmd              = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;
    mllSelectionRule = CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_LOCAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addrScopeSrc, addrScopeDest, borderCrossed, cmd, mllSelectionRule);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxTgIpCtrl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxTgIpCtrl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpMcMirrorCpuCodeEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpCtrlEcmpHashModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpCtrlCosRoutingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpCtrlMcMacMatchEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpCtrlL2L3ByteCountMatchEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpv6AddrPrefixScopeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpv6UcBorderCrossCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpv6McBorderCrossCmdSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxTgIpCtrl)

