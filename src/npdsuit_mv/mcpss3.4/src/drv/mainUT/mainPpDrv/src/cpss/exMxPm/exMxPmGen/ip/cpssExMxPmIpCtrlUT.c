/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmIpCtrlUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmIpCtrl, that provides
*       The CPSS EXMXPM Ip Control APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpCtrl.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define IP_CTRL_VALID_VIRT_PORT_CNS  0

#define IP_CTRL_VALID_PHY_PORT_CNS  0

/* Invalid enum */
#define IP_CTRL_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define IP_CTRL_TESTED_VLAN_ID_CNS  100

/* Default next pointer array size */
#define IP_CTRL_ARRAY_SIZE_CNS      16


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouterEngineEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enableRouting
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterEngineEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enableRouting [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpRouterEngineEnableGet with non-NULL enabledRoutingPtr.
    Expected: GT_OK and the same enableRouting.
*/
    GT_STATUS   st = GT_OK;
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
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmIpRouterEngineEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmIpRouterEngineEnableGet with non-NULL enabledRoutingPtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpRouterEngineEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpRouterEngineEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enabledRoutingPtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmIpRouterEngineEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmIpRouterEngineEnableGet with non-NULL enabledRoutingPtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpRouterEngineEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpRouterEngineEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enabledRoutingPtr than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterEngineEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterEngineEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouterEngineEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enabledRoutingPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterEngineEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enabledRoutingPtr.
    Expected: GT_OK.
    1.2. Call with enabledRoutingPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enabledRoutingPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmIpRouterEngineEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enabledRoutingPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpRouterEngineEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enabledRoutingPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterEngineEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterEngineEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpSpecialRouterTriggerEnableSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_TYPE_ENT  routerTriggerType,
    IN  GT_BOOL                                         enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpSpecialRouterTriggerEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with routerTriggerType [  CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_TRAP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_TRAP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_TRAP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_TRAP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E]
                   and enable [GT_FALSE].
    Expected: GT_OK.
    1.2. Call with routerTriggerType [  CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_TRAP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_TRAP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_TRAP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_TRAP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E,
                                        CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E]
                   and enable [GT_TRUE].
    Expected: GT_OK.
    1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
    Expected: GT_OK and the same enable. (for 1.1. and 1.2.)
    1.4. Call with out of range routerTriggerType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_TYPE_ENT  trigType  = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E;
    GT_BOOL                                         enable    = GT_FALSE;
    GT_BOOL                                         enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerTriggerType [  CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_TRAP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_TRAP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_TRAP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_TRAP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E]
                           and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable   = GT_FALSE;
        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);


        /*
            1.2. Call with routerTriggerType [  CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_TRAP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_TRAP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_TRAP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_TRAP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E,
                                                CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E]
                           and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable   = GT_TRUE;
        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_UC_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV6_MC_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trigType, enable);

        /*
            1.3. Call cpssExMxPmIpSpecialRouterTriggerEnableGet with the same routerTriggerType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpSpecialRouterTriggerEnableGet: %d, %d", dev, trigType);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /*
            1.4. Call with out of range routerTriggerType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        trigType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trigType);
    }

    trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E;
    enable   = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpSpecialRouterTriggerEnableSet(dev, trigType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpSpecialRouterTriggerEnableGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_TYPE_ENT  routerTriggerType,
    OUT GT_BOOL                                         *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpSpecialRouterTriggerEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E /
                                      CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E /
                                      CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E /
                                      CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E]
                   and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range routerTriggerType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_TYPE_ENT  trigType  = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E;
    GT_BOOL                                         enable    = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E /
                                              CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E /
                                              CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E /
                                              CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E]
                           and non-NULL enablePtr.
            Expected: GT_OK.
        */

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trigType);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_MC_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trigType);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_SOFT_DROP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trigType);

        /* Call with routerTriggerType [CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E] */
        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_ARP_TRAP_E;

        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trigType);

        /*
            1.2. Call with out of range routerTriggerType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        trigType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trigType);

        trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E;

        /*
            1.3. Call with enablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    trigType = CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_IPV4_UC_SOFT_DROP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpSpecialRouterTriggerEnableGet(dev, trigType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT   exceptionType,
    IN  CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT       packetType,
    IN  CPSS_PACKET_CMD_ENT                 exceptionCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpExceptionCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E /
                                  CPSS_EXMXPM_IP_EXCEPTION_TYPE_TTL1_EXCEED_E /
                                  CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E],
                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV6_E],
                   packetType [CPSS_IP_UNICAST_E /
                               CPSS_IP_MULTICAST_E /
                               CPSS_IP_UNICAST_E]
                   and exceptionCmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                     CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                     CPSS_PACKET_CMD_DROP_SOFT_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpExceptionCmdGet with the same exceptionType, protocolStack and non-NULL exceptionCmdPtr.
    Expected: GT_OK and the same exceptionCmd.
    1.3. Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E],
                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E] (this exceptionType are for IPv6 only)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range exceptionType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range packetType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range exceptionCmd [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT       exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_IP_UNICAST_MULTICAST_ENT           packetType    = CPSS_IP_UNICAST_E;
    CPSS_EXMXPM_IP_EXCEPTION_COMMAND_ENT    exceptionCmd  = CPSS_EXMXPM_IP_EXCEPTION_COMMAND_NONE;
    CPSS_EXMXPM_IP_EXCEPTION_COMMAND_ENT    exceptCmdGet  = CPSS_EXMXPM_IP_EXCEPTION_COMMAND_NONE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E /
                                          CPSS_EXMXPM_IP_EXCEPTION_TYPE_TTL1_EXCEED_E /
                                          CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E],
                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E /
                                          CPSS_IP_PROTOCOL_IPV6_E],
                           packetType [CPSS_IP_UNICAST_E /
                                       CPSS_IP_MULTICAST_E /
                                       CPSS_IP_UNICAST_E]
                           and exceptionCmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                             CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                             CPSS_PACKET_CMD_DROP_SOFT_E].
            Expected: GT_OK.
        */

        /* Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E] */
        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        packetType    = CPSS_IP_UNICAST_E;
        exceptionCmd  = CPSS_EXMXPM_IP_EXCEPTION_COMMAND_TRAP_TO_CPU_E;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, packetType, exceptionCmd);

        /*
            1.2. Call cpssExMxPmIpExceptionCmdGet with the same exceptionType, protocolStack and non-NULL exceptionCmdPtr.
            Expected: GT_OK and the same exceptionCmd.
        */
        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptCmdGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpExceptionCmdGet: %d, %d, %d, %d", dev, exceptionType, protocolStack, packetType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exceptionCmd, exceptCmdGet,
                   "get another exceptionCmd than was set: %d", dev);

        /* Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_TTL1_EXCEED_E] */
        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_TTL1_EXCEED_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        packetType    = CPSS_IP_MULTICAST_E;
        exceptionCmd  = CPSS_EXMXPM_IP_EXCEPTION_COMMAND_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, packetType, exceptionCmd);

        /*
            1.2. Call cpssExMxPmIpExceptionCmdGet with the same exceptionType, protocolStack and non-NULL exceptionCmdPtr.
            Expected: GT_OK and the same exceptionCmd.
        */
        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptCmdGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpExceptionCmdGet: %d, %d, %d, %d", dev, exceptionType, protocolStack, packetType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exceptionCmd, exceptCmdGet,
                   "get another exceptionCmd than was set: %d", dev);

        /* Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E] */
        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        packetType    = CPSS_IP_UNICAST_E;
        exceptionCmd  = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, packetType, exceptionCmd);

        /*
            1.2. Call cpssExMxPmIpExceptionCmdGet with the same exceptionType, protocolStack and non-NULL exceptionCmdPtr.
            Expected: GT_OK and the same exceptionCmd.
        */
        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptCmdGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpExceptionCmdGet: %d, %d, %d, %d", dev, exceptionType, protocolStack, packetType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exceptionCmd, exceptCmdGet,
                   "get another exceptionCmd than was set: %d", dev);

        /*
            1.3. Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E],
                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E] (this exceptionType are for IPv6 only)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack);

        /*
            1.4. Call with out of range exceptionType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        exceptionType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, exceptionType);

        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E;

        /*
            1.5. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocolStack = %d", dev, protocolStack);

        /*
            1.6. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, protocolStack = %d", dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.7. Call with out of range packetType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        packetType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, packetType = %d", dev, packetType);

        packetType = CPSS_IP_UNICAST_E;

        /*
            1.8. Call with out of range exceptionCmd [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        exceptionCmd = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, exceptionCmd = %d", dev, exceptionCmd);
    }

    exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    packetType    = CPSS_IP_UNICAST_E;
    exceptionCmd  = CPSS_EXMXPM_IP_EXCEPTION_COMMAND_NONE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpExceptionCmdSet(dev, exceptionType, protocolStack, packetType, exceptionCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT   exceptionType,
    IN  CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT       packetType,
    OUT CPSS_PACKET_CMD_ENT                 *exceptionCmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpExceptionCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E /
                                  CPSS_EXMXPM_IP_EXCEPTION_TYPE_TTL1_EXCEED_E /
                                  CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E],
                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                   packetType [CPSS_IP_UNICAST_E /
                               CPSS_IP_MULTICAST_E /
                               CPSS_IP_UNICAST_E],
                   and non-NULL exceptionCmdPtr.
    Expected: GT_OK.
    1.2. Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E],
                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E] (this exceptionType are for IPv6 only)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range exceptionType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range packetType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with exceptionCmdPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT       exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_IP_UNICAST_MULTICAST_ENT           packetType    = CPSS_IP_UNICAST_E;
    CPSS_EXMXPM_IP_EXCEPTION_COMMAND_ENT    exceptionCmd  = CPSS_EXMXPM_IP_EXCEPTION_COMMAND_NONE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E /
                                          CPSS_EXMXPM_IP_EXCEPTION_TYPE_TTL1_EXCEED_E /
                                          CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E],
                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E /
                                          CPSS_IP_PROTOCOL_IPV6_E],
                           packetType [CPSS_IP_UNICAST_E /
                                       CPSS_IP_MULTICAST_E /
                                       CPSS_IP_UNICAST_E]
                           and non-NULL exceptionCmdPtr.
            Expected: GT_OK.
        */

        /* Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E] */
        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        packetType    = CPSS_IP_UNICAST_E;

        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, packetType);

        /* Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_TTL1_EXCEED_E] */
        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_TTL1_EXCEED_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        packetType    = CPSS_IP_MULTICAST_E;

        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, packetType);

        /* Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E] */
        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        packetType    = CPSS_IP_UNICAST_E;

        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, packetType);

        /*
            1.2. Call with exceptionType [CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E],
                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E] (this exceptionType are for IPv6 only)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_NON_HOP_BY_HOP_EXT_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack);

        /*
            1.3. Call with out of range exceptionType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        exceptionType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, exceptionType);

        exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E;

        /*
            1.4. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocolStack = %d", dev, protocolStack);

        /*
            1.5. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, protocolStack = %d", dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.6. Call with out of range packetType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        packetType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, packetType = %d", dev, packetType);

        packetType = CPSS_IP_UNICAST_E;

        /*
            1.6. Call with exceptionCmdPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, exceptionCmdPtr = NULL", dev);
    }

    exceptionType = CPSS_EXMXPM_IP_EXCEPTION_TYPE_ILLEGAL_ADDRESS_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    packetType    = CPSS_IP_UNICAST_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpExceptionCmdGet(dev, exceptionType, protocolStack, packetType, &exceptionCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouteEntryAgeRefreshEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouteEntryAgeRefreshEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpRouteEntryAgeRefreshEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
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
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmIpRouteEntryAgeRefreshEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmIpRouteEntryAgeRefreshEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpRouteEntryAgeRefreshEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpRouteEntryAgeRefreshEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmIpRouteEntryAgeRefreshEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmIpRouteEntryAgeRefreshEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmIpRouteEntryAgeRefreshEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpRouteEntryAgeRefreshEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enablePtr than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouteEntryAgeRefreshEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouteEntryAgeRefreshEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouteEntryAgeRefreshEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouteEntryAgeRefreshEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


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
        st = cpssExMxPmIpRouteEntryAgeRefreshEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpRouteEntryAgeRefreshEnableGet(dev, NULL);
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
        st = cpssExMxPmIpRouteEntryAgeRefreshEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouteEntryAgeRefreshEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpMtuProfileSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mtuProfileIndex,
    IN  GT_U32  mtu
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpMtuProfileSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mtuProfileIndex [0 / 15]
                   and mtu [0 / 0x3FFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpMtuProfileGet with the same mtuProfileIndex  and non-NULL mtuPrt.
    Expected: GT_OK and the same mtu.
    1.3. Call with out of range mtuProfileIndex [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range mtu [16384]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      mtuProfileIndex = 0;
    GT_U32      mtu             = 0;
    GT_U32      mtuGet          = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mtuProfileIndex [0 / 15]
                           and mtu [0 / 0x3FFF].
            Expected: GT_OK.
        */

        /* Call with mtuProfileIndex [0] */
        mtuProfileIndex = 0;
        mtu             = 0;

        st = cpssExMxPmIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex, mtu);

        /*
            1.2. Call cpssExMxPmIpMtuProfileGet with the same mtuProfileIndex  and non-NULL mtuPrt.
            Expected: GT_OK and the same mtu.
        */
        st = cpssExMxPmIpMtuProfileGet(dev, mtuProfileIndex, &mtuGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMtuProfileGet: %d, %d", dev, mtuProfileIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mtu, mtuGet,
                   "get another mtu than was set: %d", dev);

        /* Call with mtuProfileIndex [15] */
        mtuProfileIndex = 15;
        mtu             = 0x3FFF;

        st = cpssExMxPmIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex, mtu);

        /*
            1.2. Call cpssExMxPmIpMtuProfileGet with the same mtuProfileIndex  and non-NULL mtuPrt.
            Expected: GT_OK and the same mtu.
        */
        st = cpssExMxPmIpMtuProfileGet(dev, mtuProfileIndex, &mtuGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMtuProfileGet: %d, %d", dev, mtuProfileIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mtu, mtuGet,
                   "get another mtu than was set: %d", dev);

        /*
            1.3. Call with out of range mtuProfileIndex [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mtuProfileIndex = 16;

        st = cpssExMxPmIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex);

        mtuProfileIndex = 0;

        /*
            1.4. Call with out of range mtu [16384]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mtu = 16384;

        st = cpssExMxPmIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mtu = %d", dev, mtu);
    }

    mtuProfileIndex = 0;
    mtu             = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpMtuProfileSet(dev, mtuProfileIndex, mtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpMtuProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mtuProfileIndex,
    OUT GT_U32  *mtuPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpMtuProfileGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mtuProfileIndex [0 / 15]
                   and non-NULL mtuPtr.
    Expected: GT_OK.
    1.2. Call with out of range mtuProfileIndex [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with mtuPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      mtuProfileIndex = 0;
    GT_U32      mtu             = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mtuProfileIndex [0 / 15]
                           and non-NULL mtuPtr.
            Expected: GT_OK.
        */

        /* Call with mtuProfileIndex [0] */
        mtuProfileIndex = 0;

        st = cpssExMxPmIpMtuProfileGet(dev, mtuProfileIndex, &mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex);

        /* Call with mtuProfileIndex [15] */
        mtuProfileIndex = 15;

        st = cpssExMxPmIpMtuProfileGet(dev, mtuProfileIndex, &mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex);

        /*
            1.2. Call with out of range mtuProfileIndex [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mtuProfileIndex = 16;

        st = cpssExMxPmIpMtuProfileGet(dev, mtuProfileIndex, &mtu);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex);

        mtuProfileIndex = 0;

        /*
            1.3. Call with mtuPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpMtuProfileGet(dev, mtuProfileIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mtu = NULL", dev);
    }

    mtuProfileIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpMtuProfileGet(dev, mtuProfileIndex, &mtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpMtuProfileGet(dev, mtuProfileIndex, &mtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6AddrPrefixScopeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          prefixScopeIndex,
    IN  GT_IPV6ADDR                     *prefixPtr,
    IN  GT_U32                          prefixLen,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT      addressScope
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6AddrPrefixScopeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with prefixScopeIndex [0 / 3],
                   prefixPtr->u32Ip[0x0, 0x0, 0x0, 0x0 /
                                    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
                   prefixLen [0 / 15]
                   and addressScope [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpv6AddrPrefixScopeGet with the same prefixScopeIndex,
                                                    non-NULL prefixPtr, non-NULL prefixLenPtr and non-NULL addressScopePtr.
    Expected: GT_OK and the same prefix,prefixLenPtr and addressScope.
    1.3. Call with out of range prefixScopeIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with prefixPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with out of range prefixLen [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range addressScope [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      prefixScopeIndex = 0;
    GT_IPV6ADDR                 prefix;
    GT_U32                      prefixLen        = 0;
    CPSS_IPV6_PREFIX_SCOPE_ENT  addressScope     = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_IPV6ADDR                 prefixGet;
    GT_U32                      prefixLenGet     = 0;
    CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeGet  = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_BOOL                     isEqual          = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with prefixScopeIndex [0 / 3],
                           prefixPtr->u32Ip[0x0, 0x0, 0x0, 0x0 /
                                            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
                           prefixLen [0 / 15]
                           and addressScope [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E].
            Expected: GT_OK.
        */

        /* Call with prefixScopeIndex [0] */
        prefixScopeIndex = 0;

        prefix.u32Ip[0] = 0;
        prefix.u32Ip[1] = 0;
        prefix.u32Ip[2] = 0;
        prefix.u32Ip[3] = 0;

        prefixLen    = 0;
        addressScope = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, prefixScopeIndex, prefixLen, addressScope);

        /*
            1.2. Call cpssExMxPmIpv6AddrPrefixScopeGet with the same prefixScopeIndex,
                                                            non-NULL prefixPtr, non-NULL prefixLenPtr and non-NULL addressScopePtr.
            Expected: GT_OK and the same prefix,prefixLenPtr and addressScope.
        */
        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefixGet, &prefixLenGet, &addressScopeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpv6AddrPrefixScopeGet: %d, %d", dev, prefixScopeIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(prefixLen, prefixLenGet,
                   "get another prefixLen than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(addressScope, addressScopeGet,
                   "get another addressScope than was set: %d", dev);

        /* Call with prefixScopeIndex [0] */
        addressScope = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;

        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, prefixScopeIndex, prefixLen, addressScope);

        /*
            1.2. Call cpssExMxPmIpv6AddrPrefixScopeGet with the same prefixScopeIndex,
                                                            non-NULL prefixPtr, non-NULL prefixLenPtr and non-NULL addressScopePtr.
            Expected: GT_OK and the same prefix,prefixLenPtr and addressScope.
        */
        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefixGet, &prefixLenGet, &addressScopeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpv6AddrPrefixScopeGet: %d, %d", dev, prefixScopeIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(prefixLen, prefixLenGet,
                   "get another prefixLen than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(addressScope, addressScopeGet,
                   "get another addressScope than was set: %d", dev);

        /* Call with prefixScopeIndex [3] */
        prefixScopeIndex = 3;

        prefix.u32Ip[0] = 0xFFFF;
        prefix.u32Ip[1] = 0xFFFF;
        prefix.u32Ip[2] = 0xFFFF;
        prefix.u32Ip[3] = 0xFFFF;

        prefixLen    = 15;
        addressScope = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;

        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, prefixScopeIndex, prefixLen, addressScope);

        /*
            1.2. Call cpssExMxPmIpv6AddrPrefixScopeGet with the same prefixScopeIndex,
                                                            non-NULL prefixPtr, non-NULL prefixLenPtr and non-NULL addressScopePtr.
            Expected: GT_OK and the same prefix,prefixLenPtr and addressScope.
        */
        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefixGet, &prefixLenGet, &addressScopeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpv6AddrPrefixScopeGet: %d, %d", dev, prefixScopeIndex);

        /* Verifying values (only 16 bits of prefix should be compared) */
        isEqual = ((prefix.arIP[0] == prefixGet.arIP[0]) && (prefix.arIP[1] == prefixGet.arIP[1])) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another prefix than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(prefixLen, prefixLenGet,
                   "get another prefixLen than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(addressScope, addressScopeGet,
                   "get another addressScope than was set: %d", dev);

        /* Call with prefixScopeIndex [3] */
        addressScope = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;

        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, prefixScopeIndex, prefixLen, addressScope);

        /*
            1.2. Call cpssExMxPmIpv6AddrPrefixScopeGet with the same prefixScopeIndex,
                                                            non-NULL prefixPtr, non-NULL prefixLenPtr and non-NULL addressScopePtr.
            Expected: GT_OK and the same prefix,prefixLenPtr and addressScope.
        */
        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefixGet, &prefixLenGet, &addressScopeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpv6AddrPrefixScopeGet: %d, %d", dev, prefixScopeIndex);

        /* Verifying values (only 16 bits of prefix should be compared) */
        isEqual = ((prefix.arIP[0] == prefixGet.arIP[0]) && (prefix.arIP[1] == prefixGet.arIP[1])) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another prefix than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(prefixLen, prefixLenGet,
                   "get another prefixLen than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(addressScope, addressScopeGet,
                   "get another addressScope than was set: %d", dev);

        /*
            1.3. Call with out of range prefixScopeIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        prefixScopeIndex = 4;

        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, prefixScopeIndex);

        prefixScopeIndex = 0;

        /*
            1.4. Call with prefixPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, NULL, prefixLen, addressScope);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, prefixPtr = NULL", dev);

        /*
            1.5. Call with out of range prefixLen [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        prefixLen = 16;

        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", dev, prefixLen);

        prefixLen = 0;

        /*
            1.6. Call with out of range addressScope [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScope = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScope = %d", dev, addressScope);
    }

    prefixScopeIndex = 0;

    prefix.u32Ip[0] = 0;
    prefix.u32Ip[1] = 0;
    prefix.u32Ip[2] = 0;
    prefix.u32Ip[3] = 0;

    prefixLen    = 0;
    addressScope = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6AddrPrefixScopeSet(dev, prefixScopeIndex, &prefix, prefixLen, addressScope);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6AddrPrefixScopeGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          prefixScopeIndex,
    OUT GT_IPV6ADDR                     *prefixPtr,
    OUT GT_U32                          *prefixLenPtr,
    OUT CPSS_IPV6_PREFIX_SCOPE_ENT      *addressScopePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6AddrPrefixScopeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with prefixScopeIndex [0 / 3],
                   non-NULL prefixPtr,
                   non-NULL prefixLenPtr
                   and non-NULL addressScopePtr.
    Expected: GT_OK.
    1.2. Call with out of range prefixScopeIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with prefixPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with prefixLenPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with addressScopePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      prefixScopeIndex = 0;
    GT_IPV6ADDR                 prefix;
    GT_U32                      prefixLen        = 0;
    CPSS_IPV6_PREFIX_SCOPE_ENT  addressScope     = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with prefixScopeIndex [0 / 3],
                           non-NULL prefixPtr,
                           non-NULL prefixLenPtr
                           and non-NULL addressScopePtr.
            Expected: GT_OK.
        */

        /* Call with prefixScopeIndex [0] */
        prefixScopeIndex = 0;

        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefix, &prefixLen, &addressScope);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, prefixScopeIndex);

        /* Call with prefixScopeIndex [3] */
        prefixScopeIndex = 3;

        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefix, &prefixLen, &addressScope);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, prefixScopeIndex);

        /*
            1.2. Call with out of range prefixScopeIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        prefixScopeIndex = 4;

        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefix, &prefixLen, &addressScope);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, prefixScopeIndex);

        prefixScopeIndex = 0;

        /*
            1.3. Call with prefixPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, NULL, &prefixLen, &addressScope);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, prefixPtr = NULL", dev);

        /*
            1.4. Call with prefixLenPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefix, NULL, &addressScope);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, prefixLenPtr = NULL", dev);

        /*
            1.5. Call with addressScopePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefix, &prefixLen, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, addressScopePtr = NULL", dev);
    }

    prefixScopeIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefix, &prefixLen, &addressScope);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6AddrPrefixScopeGet(dev, prefixScopeIndex, &prefix, &prefixLen, &addressScope);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6SourcePortSiteIdSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  CPSS_IP_SITE_ID_ENT     ipv6SourceSiteId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6SourcePortSiteIdSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with ipv6SourceSiteId [CPSS_IP_SITE_ID_INTERNAL_E / CPSS_IP_SITE_ID_EXTERNAL_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmIpv6SourcePortSiteIdGet with not NULL ipv6SourceSiteIdPtr.
    Expected: GT_OK and the same ipv6SourceSiteId as was set.
    1.1.3. Call with ipv6SourceSiteId [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_IP_SITE_ID_ENT ipv6SourceSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
    CPSS_IP_SITE_ID_ENT ipv6SourceSiteIdGet = CPSS_IP_SITE_ID_INTERNAL_E;

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
                1.1.1. Call with ipv6SourceSiteId [CPSS_IP_SITE_ID_INTERNAL_E / CPSS_IP_SITE_ID_EXTERNAL_E].
                Expected: GT_OK.
            */
            /* iterate with ipv6SourceSiteId = CPSS_IP_SITE_ID_INTERNAL_E */
            ipv6SourceSiteId = CPSS_IP_SITE_ID_INTERNAL_E;

            st = cpssExMxPmIpv6SourcePortSiteIdSet(dev, port, ipv6SourceSiteId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipv6SourceSiteId);

            /*
                1.1.2. Call cpssExMxPmIpv6SourcePortSiteIdGet with not NULL ipv6SourceSiteIdPtr.
                Expected: GT_OK and the same ipv6SourceSiteId as was set.
            */
            st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, &ipv6SourceSiteIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6SourcePortSiteIdGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(ipv6SourceSiteId, ipv6SourceSiteIdGet, "got another ipv6SourceSiteId than wa sset: %d, %d", dev, port);

            /* iterate with ipv6SourceSiteId = CPSS_IP_SITE_ID_EXTERNAL_E */
            ipv6SourceSiteId = CPSS_IP_SITE_ID_EXTERNAL_E;

            st = cpssExMxPmIpv6SourcePortSiteIdSet(dev, port, ipv6SourceSiteId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipv6SourceSiteId);

            /*
                1.1.2. Call cpssExMxPmIpv6SourcePortSiteIdGet with not NULL ipv6SourceSiteIdPtr.
                Expected: GT_OK and the same ipv6SourceSiteId as was set.
            */
            st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, &ipv6SourceSiteIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6SourcePortSiteIdGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(ipv6SourceSiteId, ipv6SourceSiteIdGet, "got another ipv6SourceSiteId than wa sset: %d, %d", dev, port);

            /*
                1.1.3. Call with ipv6SourceSiteId [0x5AAAAAA5] (out of range).
                Expected: GT_BAD_PARAM.
            */
            ipv6SourceSiteId = IP_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmIpv6SourcePortSiteIdSet(dev, port, ipv6SourceSiteId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, ipv6SourceSiteId);
        }

        ipv6SourceSiteId = CPSS_IP_SITE_ID_INTERNAL_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmIpv6SourcePortSiteIdSet(dev, port, ipv6SourceSiteId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmIpv6SourcePortSiteIdSet(dev, port, ipv6SourceSiteId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmIpv6SourcePortSiteIdSet(dev, port, ipv6SourceSiteId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    ipv6SourceSiteId = CPSS_IP_SITE_ID_INTERNAL_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6SourcePortSiteIdSet(dev, port, ipv6SourceSiteId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6SourcePortSiteIdSet(dev, port, ipv6SourceSiteId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6SourcePortSiteIdGet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    OUT CPSS_IP_SITE_ID_ENT     *ipv6SourceSiteIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6SourcePortSiteIdGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with not NULL ipv6SourceSiteIdPtr.
    Expected: GT_OK,
    1.1.2. Call with ipv6SourceSiteIdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_IP_SITE_ID_ENT ipv6SourceSiteId;


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
                1.1.1. Call with not NULL ipv6SourceSiteIdPtr.
                Expected: GT_OK,
            */
            st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, &ipv6SourceSiteId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with ipv6SourceSiteIdPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipv6SourceSiteId = NULL", dev, port);
        }


        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, &ipv6SourceSiteId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, &ipv6SourceSiteId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, &ipv6SourceSiteId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, &ipv6SourceSiteId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6SourcePortSiteIdGet(dev, port, &ipv6SourceSiteId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6UcScopeCommandSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeSrc,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeDest,
    IN  GT_BOOL                     isBorderCrossed,
    IN  CPSS_PACKET_CMD_ENT         scopeCommand
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6UcScopeCommandSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   isBorderCrossed [GT_TRUE and GT_FALSE]
                   and scopeCommand [CPSS_PACKET_CMD_ROUTE_E /
                                     CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                                     CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                     CPSS_PACKET_CMD_DROP_SOFT_E /
                                     CPSS_PACKET_CMD_DROP_HARD_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpv6UcScopeCommandGet with the same addressScopeSrc,
                                                   addressScopeDest,
                                                   isBorderCrossed
                                                   and non-NULL scopeCommandPtr.
    Expected: GT_OK and the same scopeCommandPtr.
    1.3. Call with scopeCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                 CPSS_PACKET_CMD_BRIDGE_E /
                                 CPSS_PACKET_CMD_NONE_E /
                                 CPSS_PACKET_CMD_FORWARD_E /
                                 CPSS_PACKET_CMD_MIRROR_TO_CPU_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range addressScopeSrc [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range addressScopeDest [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range scopeCommand [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_BOOL                     isBorderCrossed  = GT_FALSE;
    CPSS_PACKET_CMD_ENT         scopeCommand     = CPSS_PACKET_CMD_ROUTE_E;
    CPSS_PACKET_CMD_ENT         scopeCommandGet  = CPSS_PACKET_CMD_ROUTE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           isBorderCrossed [GT_TRUE and GT_FALSE]
                           and scopeCommand [CPSS_PACKET_CMD_ROUTE_E /
                                             CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                                             CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                             CPSS_PACKET_CMD_DROP_SOFT_E /
                                             CPSS_PACKET_CMD_DROP_HARD_E].
            Expected: GT_OK.
        */

        /* Call with scopeCommand [CPSS_PACKET_CMD_ROUTE_E] */
        addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        isBorderCrossed  = GT_FALSE;
        scopeCommand     = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);

        /*
            1.2. Call cpssExMxPmIpv6UcScopeCommandGet with the same addressScopeSrc,
                                                           addressScopeDest,
                                                           isBorderCrossed
                                                           and non-NULL scopeCommandPtr.
            Expected: GT_OK and the same scopeCommandPtr.
        */
        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouterSrcIdGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet,
                   "get another scopeCommand than was set: %d", dev);

        /* Call with scopeCommand [CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E] */
        addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        scopeCommand     = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);

        /*
            1.2. Call cpssExMxPmIpv6UcScopeCommandGet with the same addressScopeSrc,
                                                           addressScopeDest,
                                                           isBorderCrossed
                                                           and non-NULL scopeCommandPtr.
            Expected: GT_OK and the same scopeCommandPtr.
        */
        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouterSrcIdGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet,
                   "get another scopeCommand than was set: %d", dev);

        /* Call with scopeCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        isBorderCrossed  = GT_TRUE;
        scopeCommand     = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);

        /*
            1.2. Call cpssExMxPmIpv6UcScopeCommandGet with the same addressScopeSrc,
                                                           addressScopeDest,
                                                           isBorderCrossed
                                                           and non-NULL scopeCommandPtr.
            Expected: GT_OK and the same scopeCommandPtr.
        */
        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouterSrcIdGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet,
                   "get another scopeCommand than was set: %d", dev);

        /* Call with scopeCommand [CPSS_PACKET_CMD_DROP_SOFT_E] */
        addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        scopeCommand     = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);

        /*
            1.2. Call cpssExMxPmIpv6UcScopeCommandGet with the same addressScopeSrc,
                                                           addressScopeDest,
                                                           isBorderCrossed
                                                           and non-NULL scopeCommandPtr.
            Expected: GT_OK and the same scopeCommandPtr.
        */
        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouterSrcIdGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet,
                   "get another scopeCommand than was set: %d", dev);

        /* Call with scopeCommand [CPSS_PACKET_CMD_DROP_HARD_E] */
        scopeCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);

        /*
            1.2. Call cpssExMxPmIpv6UcScopeCommandGet with the same addressScopeSrc,
                                                           addressScopeDest,
                                                           isBorderCrossed
                                                           and non-NULL scopeCommandPtr.
            Expected: GT_OK and the same scopeCommandPtr.
        */
        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouterSrcIdGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet,
                   "get another scopeCommand than was set: %d", dev);

        /*
            1.3. Call with scopeCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                         CPSS_PACKET_CMD_BRIDGE_E /
                                         CPSS_PACKET_CMD_NONE_E /
                                         CPSS_PACKET_CMD_FORWARD_E /
                                         CPSS_PACKET_CMD_MIRROR_TO_CPU_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with scopeCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E] */
        scopeCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d", dev, scopeCommand);

        /* Call with scopeCommand [CPSS_PACKET_CMD_BRIDGE_E] */
        scopeCommand = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d", dev, scopeCommand);

        /* Call with scopeCommand [CPSS_PACKET_CMD_NONE_E] */
        scopeCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d", dev, scopeCommand);

        /* Call with scopeCommand [CPSS_PACKET_CMD_FORWARD_E] */
        scopeCommand = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d", dev, scopeCommand);

        /* Call with scopeCommand [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        scopeCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d", dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_ROUTE_E;

        /*
            1.4. Call with out of range addressScopeSrc [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScopeSrc = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeSrc = %d", dev, addressScopeSrc);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.5. Call with out of range addressScopeDest [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScopeDest = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeDest = %d", dev, addressScopeDest);

        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.6. Call with out of range scopeCommand [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        scopeCommand = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, scopeCommand = %d", dev, scopeCommand);
    }

    addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    isBorderCrossed  = GT_TRUE;
    scopeCommand     = CPSS_PACKET_CMD_ROUTE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6UcScopeCommandGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeSrc,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeDest,
    IN  GT_BOOL                     isBorderCrossed,
    OUT CPSS_PACKET_CMD_ENT         *scopeCommandPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6UcScopeCommandGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   isBorderCrossed [GT_TRUE and GT_FALSE]
                   and non-NULL scopeCommandPtr.
    Expected: GT_OK.
    1.2. Call with out of range addressScopeSrc [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range addressScopeDest [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with scopeCommandPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    CPSS_IPV6_PREFIX_SCOPE_ENT  addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_BOOL                     isBorderCrossed  = GT_FALSE;
    CPSS_PACKET_CMD_ENT         scopeCommand     = CPSS_PACKET_CMD_ROUTE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           isBorderCrossed [GT_TRUE and GT_FALSE]
                           and non-NULL scopeCommandPtr.
            Expected: GT_OK.
        */

        /* addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E] */
        addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        isBorderCrossed  = GT_FALSE;

        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E] */
        addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;

        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E] */
        addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        isBorderCrossed  = GT_TRUE;

        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E] */
        addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;

        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /*
            1.2. Call with out of range addressScopeSrc [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScopeSrc = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeSrc = %d", dev, addressScopeSrc);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.3. Call with out of range addressScopeDest [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScopeDest = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeDest = %d", dev, addressScopeDest);

        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        /*
            1.4. Call with scopeCommandPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, scopeCommandPtr = NULL", dev);
    }

    addressScopeSrc  = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    isBorderCrossed  = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6UcScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McScopeCommandSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeSrc,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeDest,
    IN  GT_BOOL                             isBorderCrossed,
    IN  CPSS_PACKET_CMD_ENT                 scopeCommand,
    IN  CPSS_EXMXPM_IP_DIT_POINTER_TYPE_ENT ditSelector
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McScopeCommandSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   isBorderCrossed  [GT_TRUE / GT_FALSE],
                   scopeCommand [ CPSS_PACKET_CMD_ROUTE_E,
                                  CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
                                  CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                  CPSS_PACKET_CMD_BRIDGE_E],
                   ditSelector [CPSS_EXMXPM_IP_DIT_POINTER_INTERNAL_E / CPSS_EXMXPM_IP_DIT_POINTER_EXTERNAL_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpv6McScopeCommandGet with not NULL scopeCommandPtr and ditSelectorPtr and other params from 1.1.
    Expected: GT_OK and the same params as was set.
    1.3. Call with addressScopeSrc [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with addressScopeDest [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with scopeCommand [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with scopeCommand [CPSS_PACKET_CMD_MIRROR_TO_CPU_E / CPSS_PACKET_CMD_FORWARD_E] (not supported) and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with ditSelector [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;

    CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_BOOL                             isBorderCrossed = GT_TRUE;
    CPSS_PACKET_CMD_ENT                 scopeCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    CPSS_EXMXPM_IP_DIT_POINTER_TYPE_ENT ditSelector = CPSS_EXMXPM_IP_DIT_POINTER_INTERNAL_E;
    CPSS_PACKET_CMD_ENT                 scopeCommandGet = CPSS_PACKET_CMD_DROP_HARD_E;
    CPSS_EXMXPM_IP_DIT_POINTER_TYPE_ENT ditSelectorGet = CPSS_EXMXPM_IP_DIT_POINTER_INTERNAL_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           isBorderCrossed  [GT_TRUE / GT_FALSE],
                           scopeCommand [ CPSS_PACKET_CMD_ROUTE_E,
                                          CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
                                          CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                          CPSS_PACKET_CMD_BRIDGE_E],
                           ditSelector [CPSS_EXMXPM_IP_DIT_POINTER_INTERNAL_E / CPSS_EXMXPM_IP_DIT_POINTER_EXTERNAL_E].
            Expected: GT_OK.
        */
        /* iterate with addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E */
        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        isBorderCrossed = GT_TRUE;
        scopeCommand = CPSS_PACKET_CMD_ROUTE_E;
        ditSelector = CPSS_EXMXPM_IP_DIT_POINTER_INTERNAL_E;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);

        /*
            1.2. Call cpssExMxPmIpv6McScopeCommandGet with not NULL scopeCommandPtr and ditSelectorPtr and other params from 1.1.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet, &ditSelectorGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McScopeCommandGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet, "got another scopeCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditSelector, ditSelectorGet, "got another ditSelector than was set: %d", dev);

        /* iterate with addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E */
        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        isBorderCrossed = GT_FALSE;
        scopeCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
        ditSelector = CPSS_EXMXPM_IP_DIT_POINTER_EXTERNAL_E;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);

        /*
            1.2. Call cpssExMxPmIpv6McScopeCommandGet with not NULL scopeCommandPtr and ditSelectorPtr and other params from 1.1.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet, &ditSelectorGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McScopeCommandGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet, "got another scopeCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditSelector, ditSelectorGet, "got another ditSelector than was set: %d", dev);

        /* iterate with addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E */
        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        isBorderCrossed = GT_TRUE;
        scopeCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        ditSelector = CPSS_EXMXPM_IP_DIT_POINTER_INTERNAL_E;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);

        /*
            1.2. Call cpssExMxPmIpv6McScopeCommandGet with not NULL scopeCommandPtr and ditSelectorPtr and other params from 1.1.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet, &ditSelectorGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McScopeCommandGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet, "got another scopeCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditSelector, ditSelectorGet, "got another ditSelector than was set: %d", dev);

        /* iterate with addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E */
        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        isBorderCrossed = GT_FALSE;
        scopeCommand = CPSS_PACKET_CMD_BRIDGE_E;
        ditSelector = CPSS_EXMXPM_IP_DIT_POINTER_EXTERNAL_E;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);

        /*
            1.2. Call cpssExMxPmIpv6McScopeCommandGet with not NULL scopeCommandPtr and ditSelectorPtr and other params from 1.1.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommandGet, &ditSelectorGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McScopeCommandGet: %d, %d, %d, %d", dev, addressScopeSrc, addressScopeDest, isBorderCrossed);
        UTF_VERIFY_EQUAL1_STRING_MAC(scopeCommand, scopeCommandGet, "got another scopeCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditSelector, ditSelectorGet, "got another ditSelector than was set: %d", dev);

        /*
            1.3. Call with addressScopeSrc [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScopeSrc = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, addressScopeSrc);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;

        /*
            1.4. Call with addressScopeDest [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScopeDest = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeDest = %d", dev, addressScopeDest);

        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;

        /*
            1.5. Call with scopeCommand [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        scopeCommand = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, scopeCommand = %d", dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_DROP_SOFT_E;

        /*
            1.6. Call with scopeCommand [CPSS_PACKET_CMD_MIRROR_TO_CPU_E / CPSS_PACKET_CMD_FORWARD_E] (not supported) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        scopeCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d", dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d", dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_DROP_SOFT_E;

        /*
            1.7. Call with ditSelector [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditSelector = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditSelector = %d", dev, ditSelector);
    }

    addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    isBorderCrossed = GT_TRUE;
    scopeCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    ditSelector = CPSS_EXMXPM_IP_DIT_POINTER_INTERNAL_E;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McScopeCommandGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeSrc,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeDest,
    IN  GT_BOOL                             isBorderCrossed,
    OUT CPSS_PACKET_CMD_ENT                 *scopeCommandPtr,
    OUT CPSS_EXMXPM_IP_DIT_POINTER_TYPE_ENT *ditSelectorPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McScopeCommandGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                    CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                     CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                   isBorderCrossed  [GT_TRUE / GT_FALSE],
                   not NULL scopeCommandPtr, ditSelectorPtr.
    Expected: GT_OK.
    1.2. Call with addressScopeSrc [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with addressScopeDest [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with scopeCommandPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with ditSelectorPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;

    CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    GT_BOOL                             isBorderCrossed = GT_TRUE;
    CPSS_PACKET_CMD_ENT                 scopeCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    CPSS_EXMXPM_IP_DIT_POINTER_TYPE_ENT ditSelector = CPSS_EXMXPM_IP_DIT_POINTER_INTERNAL_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                            CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
                                             CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
                           isBorderCrossed  [GT_TRUE / GT_FALSE],
                           not NULL scopeCommandPtr, ditSelectorPtr.
            Expected: GT_OK.
        */
        /* iterate with addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E */
        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        isBorderCrossed = GT_TRUE;

        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, &ditSelector);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* iterate with addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E */
        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        isBorderCrossed = GT_TRUE;

        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, &ditSelector);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* iterate with addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E */
        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        isBorderCrossed = GT_FALSE;

        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, &ditSelector);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /* iterate with addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E */
        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        isBorderCrossed = GT_FALSE;

        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, &ditSelector);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest, isBorderCrossed);

        /*
            1.2. Call with addressScopeSrc [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScopeSrc = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, &ditSelector);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, addressScopeSrc);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;

        /*
            1.3. Call with addressScopeDest [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addressScopeDest = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, &ditSelector);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addressScopeDest = %d", dev, addressScopeDest);

        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;

        /*
            1.4. Call with scopeCommandPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, NULL, &ditSelector);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, scopeCommand = NULL", dev);

        /*
            1.5. Call with ditSelectorPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ditSelector = NULL", dev);
    }

    addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    isBorderCrossed = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, &ditSelector);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McScopeCommandGet(dev, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand, &ditSelector);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpQosModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_QOS_ROUTING_MODE_ENT    mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpQosModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_EXMXPM_QOS_ROUTING_BITWISE_MODE_E /
                         CPSS_EXMXPM_QOS_ROUTING_MODULUS_MODE_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpQosModeGet with non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_QOS_ROUTING_MODE_ENT mode    = CPSS_EXMXPM_QOS_ROUTING_BITWISE_MODE_E;
    CPSS_EXMXPM_QOS_ROUTING_MODE_ENT modeGet = CPSS_EXMXPM_QOS_ROUTING_BITWISE_MODE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_EXMXPM_QOS_ROUTING_BITWISE_MODE_E /
                                 CPSS_EXMXPM_QOS_ROUTING_MODULUS_MODE_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_QOS_ROUTING_BITWISE_MODE_E] */
        mode = CPSS_EXMXPM_QOS_ROUTING_BITWISE_MODE_E;

        st = cpssExMxPmIpQosModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmIpQosModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmIpQosModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpQosModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_QOS_ROUTING_MODULUS_MODE_E] */
        mode = CPSS_EXMXPM_QOS_ROUTING_MODULUS_MODE_E;

        st = cpssExMxPmIpQosModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmIpQosModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmIpQosModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpQosModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpQosModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_QOS_ROUTING_BITWISE_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpQosModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpQosModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpQosModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_EXMXPM_QOS_ROUTING_MODE_ENT    *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpQosModeGet)
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

    CPSS_EXMXPM_QOS_ROUTING_MODE_ENT mode = CPSS_EXMXPM_QOS_ROUTING_BITWISE_MODE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmIpQosModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpQosModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpQosModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpQosModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouterMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterMacSaBaseSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with macPtr->arEther [0x0, 0x0, 0x0, 0x0, 0x0, 0x0 /
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpRouterMacSaBaseGet with non-NULL macPtr.
    Expected: GT_OK and the same macPtr.
    1.3. Call with macPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR    mac;
    GT_ETHERADDR    macGet;
    GT_BOOL         isEqual = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with macPtr->arEther [0x0, 0x0, 0x0, 0x0, 0x0, 0x0 /
                                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF].
            Expected: GT_OK.
        */

        /* Call with macPtr->arEther [0x0, 0x0, 0x0, 0x0, 0x0, 0x0] */
        mac.arEther[0] = 0;
        mac.arEther[1] = 0;
        mac.arEther[2] = 0;
        mac.arEther[3] = 0;
        mac.arEther[4] = 0;

        st = cpssExMxPmIpRouterMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmIpRouterMacSaBaseGet with non-NULL macPtr.
            Expected: GT_OK and the same macPtr.
        */
        st = cpssExMxPmIpRouterMacSaBaseGet(dev, &macGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpRouterMacSaBaseGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mac,
                                     (GT_VOID*) &macGet,
                                     sizeof(mac.arEther[0] * 5))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another mac than was set: %d", dev);

        /* Call with macPtr->arEther [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF] */
        mac.arEther[0] = 0xFF;
        mac.arEther[1] = 0xFF;
        mac.arEther[2] = 0xFF;
        mac.arEther[3] = 0xFF;
        mac.arEther[4] = 0xFF;

        st = cpssExMxPmIpRouterMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmIpRouterMacSaBaseGet with non-NULL macPtr.
            Expected: GT_OK and the same macPtr.
        */
        st = cpssExMxPmIpRouterMacSaBaseGet(dev, &macGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpRouterMacSaBaseGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mac,
                                     (GT_VOID*) &macGet,
                                     sizeof(mac.arEther[0] * 5))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another mac than was set: %d", dev);

        /*
            1.3. Call with macPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpRouterMacSaBaseSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macPtr = NULL", dev);
    }

    mac.arEther[0] = 0;
    mac.arEther[1] = 0;
    mac.arEther[2] = 0;
    mac.arEther[3] = 0;
    mac.arEther[4] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterMacSaBaseSet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouterMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterMacSaBaseGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null macPtr.
    Expected: GT_OK.
    1.2. Call with macPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR    mac;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null macPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmIpRouterMacSaBaseGet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with macPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpRouterMacSaBaseGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterMacSaBaseGet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterMacSaBaseGet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpPortRouterMacSaLsbModeSet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    IN  CPSS_MAC_SA_LSB_MODE_ENT     saLsbMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpPortRouterMacSaLsbModeSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1. Call with saLsbMode [CPSS_SA_LSB_PER_PORT_E /
                              CPSS_SA_LSB_PER_PKT_VID_E /
                              CPSS_SA_LSB_PER_VLAN_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpPortRouterMacSaLsbModeGet with non-NULL saLsbModePtr.
    Expected: GT_OK and the same saLsbMode.
    1.3. Call with out of range saLsbMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_MAC_SA_LSB_MODE_ENT     saLsbMode    = CPSS_SA_LSB_PER_PORT_E;
    CPSS_MAC_SA_LSB_MODE_ENT     saLsbModeGet = CPSS_SA_LSB_PER_PORT_E;


    /* prepare device iterator */
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
                1.1. Call with saLsbMode [CPSS_SA_LSB_PER_PORT_E /
                                          CPSS_SA_LSB_PER_PKT_VID_E /
                                          CPSS_SA_LSB_PER_VLAN_E].
                Expected: GT_OK.
            */

            /* 1.1. Call with saLsbMode [CPSS_SA_LSB_PER_PORT_E] */
            saLsbMode = CPSS_SA_LSB_PER_PORT_E;

            st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saLsbMode);

            /*
                1.2. Call cpssExMxPmIpPortRouterMacSaLsbModeGet with non-NULL saLsbModePtr.
                Expected: GT_OK and the same saLsbMode.
            */
            st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmIpRouterMacSaLsbModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saLsbMode, saLsbModeGet,
                           "get another saLsbMode than was set: %d, %d", dev, port);

            /* 1.1. Call with saLsbMode [CPSS_SA_LSB_PER_PKT_VID_E] */
            saLsbMode = CPSS_SA_LSB_PER_PKT_VID_E;

            st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saLsbMode);

            /*
                1.2. Call cpssExMxPmIpPortRouterMacSaLsbModeGet with non-NULL saLsbModePtr.
                Expected: GT_OK and the same saLsbMode.
            */
            st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmIpPortRouterMacSaLsbModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saLsbMode, saLsbModeGet,
                           "get another saLsbMode than was set: %d, %d", dev, port);

            /* 1.1. Call with saLsbMode [CPSS_SA_LSB_PER_VLAN_E] */
            saLsbMode = CPSS_SA_LSB_PER_VLAN_E;

            st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saLsbMode);

            /*
                1.2. Call cpssExMxPmIpPortRouterMacSaLsbModeGet with non-NULL saLsbModePtr.
                Expected: GT_OK and the same saLsbMode.
            */
            st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmIpPortRouterMacSaLsbModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saLsbMode, saLsbModeGet,
                           "get another saLsbMode than was set: %d, %d", dev, port);

            /*
                1.3. Call function with out of range saLsbMode [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */
            saLsbMode = IP_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, saLsbMode);
        }

        saLsbMode = CPSS_SA_LSB_PER_PORT_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_VIRT_PORT_CNS;
    saLsbMode = CPSS_SA_LSB_PER_PORT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpPortRouterMacSaLsbModeGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    OUT CPSS_MAC_SA_LSB_MODE_ENT     *saLsbModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpPortRouterMacSaLsbModeGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1. Call cpssExMxPmIpPortRouterMacSaLsbModeSet with saLsbMode [CPSS_SA_LSB_PER_PORT_E].
    Expected: GT_OK.
    1.2. Call with non-null saLsbModePtr.
    Expected: GT_OK.
    1.3. Call with saLsbModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_MAC_SA_LSB_MODE_ENT     saLsbMode = CPSS_SA_LSB_PER_PORT_E;

    /* prepare device iterator */
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
                1.1. Call cpssExMxPmIpPortRouterMacSaLsbModeSet with saLsbMode [CPSS_SA_LSB_PER_PORT_E]
                Expected: GT_OK.
            */
            saLsbMode = CPSS_SA_LSB_PER_PORT_E;

            st = cpssExMxPmIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpPortRouterMacSaLsbModeSet: %d, %d, %d", dev, port, saLsbMode);

            /*
                1.2. Call with non-null saLsbModePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.3. Call with null saLsbModePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, saLsbModePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouterVlanMacSaLsbSet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanPortId,
    IN  GT_U32      saMac
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterVlanMacSaLsbSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vlanPortId [100 / 4095]
                   and saMac [0 / 255].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpRouterVlanMacSaLsbGet with the same vlanPortId and non-NULL saMacPtr.
    Expected: GT_OK and the same saMac.
    1.3. Call with out of range vlanPortId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range saMac [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16      vlanPortId = 0;
    GT_U32      saMac      = 0;
    GT_U32      saMacGet   = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanPortId [100 / 4095]
                           and saMac [0 / 255].
            Expected: GT_OK.
        */

        /* Call with vlanPortId [100] */
        vlanPortId = IP_CTRL_TESTED_VLAN_ID_CNS;
        saMac      = 0;

        st = cpssExMxPmIpRouterVlanMacSaLsbSet(dev, vlanPortId, saMac);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanPortId, saMac);

        /*
            1.2. Call cpssExMxPmIpRouterVlanMacSaLsbGet with the same vlanPortId and non-NULL saMacPtr.
            Expected: GT_OK and the same saMac.
        */
        st = cpssExMxPmIpRouterVlanMacSaLsbGet(dev, vlanPortId, &saMacGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpRouterVlanMacSaLsbGet: %d, %d", dev, vlanPortId);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(saMac, saMacGet,
                   "get another saMac than was set: %d", dev);

        /* Call with vlanPortId [4095] */
        vlanPortId = 4095;
        saMac      = 255;

        st = cpssExMxPmIpRouterVlanMacSaLsbSet(dev, vlanPortId, saMac);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanPortId, saMac);

        /*
            1.2. Call cpssExMxPmIpRouterVlanMacSaLsbGet with the same vlanPortId and non-NULL saMacPtr.
            Expected: GT_OK and the same saMac.
        */
        st = cpssExMxPmIpRouterVlanMacSaLsbGet(dev, vlanPortId, &saMacGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpRouterVlanMacSaLsbGet: %d, %d", dev, vlanPortId);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(saMac, saMacGet,
                   "get another saMac than was set: %d", dev);

        /*
            1.3. Call with out of range vlanPortId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanPortId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmIpRouterVlanMacSaLsbSet(dev, vlanPortId, saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanPortId);

        vlanPortId = IP_CTRL_TESTED_VLAN_ID_CNS;

        /*
            1.4. Call with out of range saMac [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        saMac = 256;

        st = cpssExMxPmIpRouterVlanMacSaLsbSet(dev, vlanPortId, saMac);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, saMac = %d", dev, saMac);
    }

    vlanPortId = IP_CTRL_TESTED_VLAN_ID_CNS;
    saMac      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterVlanMacSaLsbSet(dev, vlanPortId, saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterVlanMacSaLsbSet(dev, vlanPortId, saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouterVlanMacSaLsbGet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanPortId,
    OUT GT_U32      *saMacPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterVlanMacSaLsbGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vlanPortId [100 / 4095]
                   and non-NULL saMacPtr.
    Expected: GT_OK.
    1.2. Call with out of range vlanPortId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with saMacPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16      vlanPortId = 0;
    GT_U32      saMac      = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanPortId [100 / 4095]
                           and non-NULL saMacPtr.
            Expected: GT_OK.
        */

        /* Call with vlanPortId [100] */
        vlanPortId = IP_CTRL_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmIpRouterVlanMacSaLsbGet(dev, vlanPortId, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanPortId);

        /* Call with vlanPortId [4095] */
        vlanPortId = 4095;

        st = cpssExMxPmIpRouterVlanMacSaLsbGet(dev, vlanPortId, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanPortId);

        /*
            1.2. Call with out of range vlanPortId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanPortId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmIpRouterVlanMacSaLsbGet(dev, vlanPortId, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanPortId);

        vlanPortId = IP_CTRL_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call with saMacPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpRouterVlanMacSaLsbGet(dev, vlanPortId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, saMacPtr = NULL", dev);
    }

    vlanPortId = IP_CTRL_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterVlanMacSaLsbGet(dev, vlanPortId, &saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterVlanMacSaLsbGet(dev, vlanPortId, &saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet
(
    IN  GT_U8                                      devNum,
    IN  GT_U32                                     portNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                 protocolStack,
    IN  CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                    CPSS_IP_PROTOCOL_IPV6_E]
                     and mode [CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E /
                               CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_VLAN_E /
                               CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_VLAN_E /
                               CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_ROUTE_ENTRY_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet with the same protocolStack and non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.1.3. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E]
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call with out of range protocolStack [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with out of range mode [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_VIRT_PORT_CNS;

    CPSS_IP_PROTOCOL_STACK_ENT                 protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_MODE_ENT  mode          = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E;
    CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_MODE_ENT  modeGet       = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E;


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
                1.1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                CPSS_IP_PROTOCOL_IPV6_E]
                                 and mode [CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E /
                                           CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_VLAN_E /
                                           CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_VLAN_E /
                                           CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_ROUTE_ENTRY_E].
                Expected: GT_OK.
            */

            /* Call with mode [CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E] */
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            mode          = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, protocolStack, mode);

            /*
                1.1.2. Call cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet with the same protocolStack and non-NULL modePtr.
                Expected: GT_OK and the same mode.
            */
            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d, %d", dev, port);

            /* Call with mode [CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_VLAN_E] */
            mode = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_VLAN_E;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, protocolStack, mode);

            /*
                1.1.2. Call cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet with the same protocolStack and non-NULL modePtr.
                Expected: GT_OK and the same mode.
            */
            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d, %d", dev, port);

            /* Call with mode [CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_VLAN_E] */
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            mode          = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_VLAN_E;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, protocolStack, mode);

            /*
                1.1.2. Call cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet with the same protocolStack and non-NULL modePtr.
                Expected: GT_OK and the same mode.
            */
            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d, %d", dev, port);

            /* Call with mode [CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_ROUTE_ENTRY_E] */
            mode = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_ROUTE_ENTRY_E;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, protocolStack, mode);

            /*
                1.1.2. Call cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet with the same protocolStack and non-NULL modePtr.
                Expected: GT_OK and the same mode.
            */
            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E]
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, protocolStack);

            /*
                1.1.4. Call with out of range protocolStack [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            protocolStack = IP_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, protocolStack);

            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

            /*
                1.1.5. Call with out of range mode [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            mode = IP_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, mode = %d", dev, port, mode);
        }

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        mode          = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    mode          = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(dev, port, protocolStack, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet
(
    IN  GT_U8                                      devNum,
    IN  GT_U32                                     portNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                 protocolStack,
    OUT CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_MODE_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                    CPSS_IP_PROTOCOL_IPV6_E]
                     and non-NULL modePtr.
    Expected: GT_OK.
    1.1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E]
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with out of range protocolStack [0x5AAAAAA5]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with modePtr [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_VIRT_PORT_CNS;

    CPSS_IP_PROTOCOL_STACK_ENT                 protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_MODE_ENT  mode          = CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_PER_PORT_E;


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
                1.1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                CPSS_IP_PROTOCOL_IPV6_E]
                                 and non-NULL modePtr.
                Expected: GT_OK.
            */

            /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, protocolStack);

            /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, protocolStack);

            /*
                1.1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E]
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, protocolStack);

            /*
                1.1.3. Call with out of range protocolStack [0x5AAAAAA5]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            protocolStack = IP_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, protocolStack);

            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

            /*
                1.1.4. Call with modePtr [NULL]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            mode = IP_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, modePtr = NULL", dev, port);
        }

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(dev, port, protocolStack, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpArpBcCommandSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PACKET_CMD_ENT     arpBcCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpArpBcCommandSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with arpBcCmd [CPSS_PACKET_CMD_NONE_E /
                             CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                             CPSS_PACKET_CMD_MIRROR_TO_CPU_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpArpBcCommandGet with non-NULL arpBcCmdPtr.
    Expected: GT_OK and the same arpBcCmd.
    1.3. Call with arpBcCmd [CPSS_PACKET_CMD_FORWARD_E /
                             CPSS_PACKET_CMD_DROP_HARD_E] (not supported).
    Expected: NOT GT_OK.
    1.4. Call with out of range arpBcCmd [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT     arpBcCmd    = CPSS_PACKET_CMD_NONE_E;
    CPSS_PACKET_CMD_ENT     arpBcCmdGet = CPSS_PACKET_CMD_NONE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with arpBcCmd [CPSS_PACKET_CMD_NONE_E /
                                     CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                     CPSS_PACKET_CMD_MIRROR_TO_CPU_E].
            Expected: GT_OK.
        */

        /* Call with arpBcCmd [CPSS_PACKET_CMD_NONE_E] */
        arpBcCmd = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmIpArpBcCommandSet(dev, arpBcCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcCmd);

        /*
            1.2. Call cpssExMxPmIpArpBcCommandGet with non-NULL arpBcCmdPtr.
            Expected: GT_OK and the same arpBcCmd.
        */
        st = cpssExMxPmIpArpBcCommandGet(dev, &arpBcCmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpArpBcCommandGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arpBcCmd, arpBcCmdGet,
                       "get another arpBcCmd than was set: %d", dev);

        /* Call with arpBcCmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        arpBcCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmIpArpBcCommandSet(dev, arpBcCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcCmd);

        /*
            1.2. Call cpssExMxPmIpArpBcCommandGet with non-NULL arpBcCmdPtr.
            Expected: GT_OK and the same arpBcCmd.
        */
        st = cpssExMxPmIpArpBcCommandGet(dev, &arpBcCmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpArpBcCommandGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arpBcCmd, arpBcCmdGet,
                       "get another arpBcCmd than was set: %d", dev);

        /* Call with arpBcCmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        arpBcCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmIpArpBcCommandSet(dev, arpBcCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcCmd);

        /*
            1.2. Call cpssExMxPmIpArpBcCommandGet with non-NULL arpBcCmdPtr.
            Expected: GT_OK and the same arpBcCmd.
        */
        st = cpssExMxPmIpArpBcCommandGet(dev, &arpBcCmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpArpBcCommandGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arpBcCmd, arpBcCmdGet,
                       "get another arpBcCmd than was set: %d", dev);

        /*
            1.3. Call with arpBcCmd [CPSS_PACKET_CMD_FORWARD_E /
                                     CPSS_PACKET_CMD_DROP_HARD_E] (not supported).
            Expected: NOT GT_OK.
        */

        /* Call with arpBcCmd [CPSS_PACKET_CMD_FORWARD_E] */
        arpBcCmd = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmIpArpBcCommandSet(dev, arpBcCmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcCmd);

        /* Call with arpBcCmd [CPSS_PACKET_CMD_DROP_HARD_E] */
        arpBcCmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmIpArpBcCommandSet(dev, arpBcCmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcCmd);

        /*
            1.4. Call with out of range arpBcCmd [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        arpBcCmd = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpArpBcCommandSet(dev, arpBcCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, arpBcCmd);
    }

    arpBcCmd = CPSS_PACKET_CMD_NONE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpArpBcCommandSet(dev, arpBcCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpArpBcCommandSet(dev, arpBcCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpArpBcCommandGet
(
    IN  GT_U8               devNum,
    OUT CPSS_PACKET_CMD_ENT *arpBcCmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpArpBcCommandGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null arpBcCmdPtr.
    Expected: GT_OK.
    1.2. Call with arpBcCmdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT     arpBcCmd = CPSS_PACKET_CMD_NONE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null arpBcCmdPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmIpArpBcCommandGet(dev, &arpBcCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null arpBcCmdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpArpBcCommandGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arpBcCmdPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpArpBcCommandGet(dev, &arpBcCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpArpBcCommandGet(dev, &arpBcCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmMemoryOffsetSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_EXMXPM_IP_LPM_TYPE_ENT     lpmType,
    IN  GT_U32                          lpmOffset
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmMemoryOffsetSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with lpmType [CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E /
                            CPSS_EXMXPM_IP_IPV4_UC_SIP_LPM_E /
                            CPSS_EXMXPM_IP_IPV4_MC_DIP_LPM_E /
                            CPSS_EXMXPM_IP_IPV6_UC_DIP_LPM_E]
                   and lpmOffset [0 / 100 / 4095 / 0xFFFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmMemoryOffsetGet with the same lpmType and non-NULL lpmOffsetPtr.
    Expected: GT_OK and the same lpmOffset.
    1.3. Call with out of range lpmType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range lpmOffset [0x1000000]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_IP_LPM_TYPE_ENT     lpmType      = CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E;
    GT_U32                          lpmOffset    = 0;
    GT_U32                          lpmOffsetGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with lpmType [CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E /
                                    CPSS_EXMXPM_IP_IPV4_UC_SIP_LPM_E /
                                    CPSS_EXMXPM_IP_IPV4_MC_DIP_LPM_E /
                                    CPSS_EXMXPM_IP_IPV6_UC_DIP_LPM_E]
                           and lpmOffset [0 / 100 / 4095 / 0xFFFFFF].
            Expected: GT_OK.
        */

        /* Call with lpmType [CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E] */
        lpmType   = CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E;
        lpmOffset = 0;

        st = cpssExMxPmIpLpmMemoryOffsetSet(dev, lpmType, lpmOffset);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lpmType, lpmOffset);

        /*
            1.2. Call cpssExMxPmIpLpmMemoryOffsetGet with the same lpmType and non-NULL lpmOffsetPtr.
            Expected: GT_OK and the same lpmOffset.
        */
        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffsetGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpLpmMemoryOffsetGet: %d, %d", dev, lpmType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(lpmOffset, lpmOffsetGet,
                       "get another lpmOffset than was set: %d", dev);

        /* Call with lpmType [CPSS_EXMXPM_IP_IPV4_UC_SIP_LPM_E] */
        lpmType   = CPSS_EXMXPM_IP_IPV4_UC_SIP_LPM_E;
        lpmOffset = 100;

        st = cpssExMxPmIpLpmMemoryOffsetSet(dev, lpmType, lpmOffset);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lpmType, lpmOffset);

        /*
            1.2. Call cpssExMxPmIpLpmMemoryOffsetGet with the same lpmType and non-NULL lpmOffsetPtr.
            Expected: GT_OK and the same lpmOffset.
        */
        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffsetGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpLpmMemoryOffsetGet: %d, %d", dev, lpmType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(lpmOffset, lpmOffsetGet,
                       "get another lpmOffset than was set: %d", dev);

        /* Call with lpmType [CPSS_EXMXPM_IP_IPV4_MC_DIP_LPM_E] */
        lpmType   = CPSS_EXMXPM_IP_IPV4_MC_DIP_LPM_E;
        lpmOffset = 4095;

        st = cpssExMxPmIpLpmMemoryOffsetSet(dev, lpmType, lpmOffset);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lpmType, lpmOffset);

        /*
            1.2. Call cpssExMxPmIpLpmMemoryOffsetGet with the same lpmType and non-NULL lpmOffsetPtr.
            Expected: GT_OK and the same lpmOffset.
        */
        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffsetGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpLpmMemoryOffsetGet: %d, %d", dev, lpmType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(lpmOffset, lpmOffsetGet,
                       "get another lpmOffset than was set: %d", dev);

        /* Call with lpmType [CPSS_EXMXPM_IP_IPV6_UC_DIP_LPM_E] */
        lpmType   = CPSS_EXMXPM_IP_IPV6_UC_DIP_LPM_E;
        lpmOffset = 0xFFFFFF;

        st = cpssExMxPmIpLpmMemoryOffsetSet(dev, lpmType, lpmOffset);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lpmType, lpmOffset);

        /*
            1.2. Call cpssExMxPmIpLpmMemoryOffsetGet with the same lpmType and non-NULL lpmOffsetPtr.
            Expected: GT_OK and the same lpmOffset.
        */
        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffsetGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpLpmMemoryOffsetGet: %d, %d", dev, lpmType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(lpmOffset, lpmOffsetGet,
                       "get another lpmOffset than was set: %d", dev);

        /*
            1.3. Call with out of range lpmType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lpmType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpLpmMemoryOffsetSet(dev, lpmType, lpmOffset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, lpmType);

        lpmType = CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E;

        /*
            1.4. Call with out of range lpmOffset [0x1000000]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        lpmOffset = 0x1000000;

        st = cpssExMxPmIpLpmMemoryOffsetSet(dev, lpmType, lpmOffset);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, lpmOffset = %d", dev, lpmOffset);
    }

    lpmType   = CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E;
    lpmOffset = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpLpmMemoryOffsetSet(dev, lpmType, lpmOffset);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpLpmMemoryOffsetSet(dev, lpmType, lpmOffset);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmMemoryOffsetGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_EXMXPM_IP_LPM_TYPE_ENT     lpmType,
    OUT GT_U32                          *lpmOffsetPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmMemoryOffsetGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with lpmType [CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E /
                            CPSS_EXMXPM_IP_IPV4_UC_SIP_LPM_E /
                            CPSS_EXMXPM_IP_IPV4_MC_DIP_LPM_E /
                            CPSS_EXMXPM_IP_IPV6_UC_DIP_LPM_E]
                   and non-NULL lpmOffsetPtr.
    Expected: GT_OK.
    1.2. Call with out of range lpmType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with lpmOffsetPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_IP_LPM_TYPE_ENT     lpmType   = CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E;
    GT_U32                          lpmOffset = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with lpmType [CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E /
                                    CPSS_EXMXPM_IP_IPV4_UC_SIP_LPM_E /
                                    CPSS_EXMXPM_IP_IPV4_MC_DIP_LPM_E /
                                    CPSS_EXMXPM_IP_IPV6_UC_DIP_LPM_E]
                           and non-NULL lpmOffsetPtr.
            Expected: GT_OK.
        */

        /* Call with lpmType [CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E] */
        lpmType = CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E;

        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lpmType);

        /* Call with lpmType [CPSS_EXMXPM_IP_IPV4_UC_SIP_LPM_E] */
        lpmType = CPSS_EXMXPM_IP_IPV4_UC_SIP_LPM_E;

        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lpmType);

        /* Call with lpmType [CPSS_EXMXPM_IP_IPV4_MC_DIP_LPM_E] */
        lpmType = CPSS_EXMXPM_IP_IPV4_MC_DIP_LPM_E;

        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lpmType);

        /* Call with lpmType [CPSS_EXMXPM_IP_IPV6_UC_DIP_LPM_E] */
        lpmType = CPSS_EXMXPM_IP_IPV6_UC_DIP_LPM_E;

        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lpmType);

        /*
            1.3. Call with out of range lpmType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lpmType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, lpmType);

        lpmType = CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E;

        /*
            1.3. Call with lpmOffsetPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lpmOffsetPtr = NULL", dev);
    }

    lpmType = CPSS_EXMXPM_IP_IPV4_UC_DIP_LPM_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffset);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpLpmMemoryOffsetGet(dev, lpmType, &lpmOffset);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpHwLpmNodeNextPointersWrite
(
    IN GT_U8                                      dev,
    IN GT_U32                                     memAddr,
    IN GT_U32                                     sramIndex,
    IN GT_U32                                     pointingToSramIndex,
    IN GT_U32                                     numOfNextPointers,
    IN CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC   *nextPointerArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpHwLpmNodeNextPointersWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with memAddr [0],
                   sramIndex [0],
                   pointingToSramIndex [0],
                   numOfNextPointers [1],
                   nextPointerArrayPtr[0] {pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E],
                                           pointerData.nextNodePointer {nextPointer [0], range5Index [1]}}.
    Expected: GT_OK.
    1.2. Call with memAddr [0xFF],
                   sramIndex [15],
                   pointingToSramIndex [1],
                   numOfNextPointers [1],
                   nextPointerArrayPtr[0] {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                           routeEntryPointer {routeEntryBaseMemAddr [0],
                                                              blockSize [1],
                                                              routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]}}.
    Expected: GT_OK.
    1.3. Call with memAddr [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.4. Call with sramIndex [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.5. Call with pointingToSramIndex [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.6. Call with out of range nextPointerArrayPtr->pointerType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                                        out of range pointerData.nextNodePointer.range5Index [0 / 256]}
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                                        out of range pointerData.nextNodePointer. nextPointer [0x1000001]}
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                         pointerData.routeEntryPointer {out of range blockSize [2],
                                                                        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                         out of range pointerData.routeEntryPointer.routeEntryMethod [0x5AAAAAA5]}
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with nextPointerArrayPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                     memAddr             = 0;
    GT_U32                                     sramIndex           = 0;
    GT_U32                                     pointingToSramIndex = 0;
    GT_U32                                     numOfNextPointers   = 0;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC   nextPointerArray[IP_CTRL_ARRAY_SIZE_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memAddr [0],
                           sramIndex [0],
                           pointingToSramIndex [0],
                           numOfNextPointers [1],
                           nextPointerArrayPtr[0] {pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E],
                                                   pointerData.nextNodePointer {nextPointer [0], range5Index [1]}}.
            Expected: GT_OK.
        */
        memAddr             = 0;
        sramIndex           = 0;
        pointingToSramIndex = 0;
        numOfNextPointers   = 1;

        nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 0;
        /* not needed - needed only when pointer type is compressed 2 */
        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 1;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, memAddr, sramIndex,
                                    pointingToSramIndex, numOfNextPointers);

        /*
            1.2. Call with memAddr [0xFF],
                           sramIndex [15],
                           pointingToSramIndex [1],
                           numOfNextPointers [1],
                           nextPointerArrayPtr[0] {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                                   routeEntryPointer {routeEntryBaseMemAddr [0],
                                                                      blockSize [1],
                                                                      routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]}}.
            Expected: GT_OK.
        */
        memAddr             = 0xFF;
        sramIndex           = 15;
        pointingToSramIndex = 1;
        numOfNextPointers   = 1;

        nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryBaseMemAddr = 0;
        nextPointerArray[0].pointerData.routeEntryPointer.blockSize             = 1;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod      = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, memAddr, sramIndex,
                                    pointingToSramIndex, numOfNextPointers);

        /*
            1.3. Call with memAddr [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        memAddr = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, memAddr);

        memAddr = 0;

        /*
            1.4. Call with sramIndex [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        sramIndex = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, sramIndex = %d", dev, sramIndex);

        sramIndex = 0;

        /*
            1.5. Call with pointingToSramIndex [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        pointingToSramIndex = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pointingToSramIndex = %d", dev, pointingToSramIndex);

        pointingToSramIndex = 0;

        /*
            1.6. Call with out of range nextPointerArrayPtr->pointerType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nextPointerArray[0].pointerType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nextPointerArray[0].pointerType = %d",
                                     dev, nextPointerArray[0].pointerType);

        /*
            1.7. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                                                out of range pointerData.nextNodePointer.range5Index [0 / 256]}
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E;

        /* Call with nextPointerArrayPtr.pointerData.nextNodePointer.range5Index [0] */
        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 0;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.nextNodePointer.range5Index = %d",
                                         dev, nextPointerArray[0].pointerType,
                                         nextPointerArray[0].pointerData.nextNodePointer.range5Index);

        /* Call with nextPointerArrayPtr.pointerData.nextNodePointer.range5Index [256] */
        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 256;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.nextNodePointer.range5Index = %d",
                                         dev, nextPointerArray[0].pointerType,
                                         nextPointerArray[0].pointerData.nextNodePointer.range5Index);

        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 1;

        /*
            1.8. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                                                out of range pointerData.nextNodePointer. nextPointer [0x1000001]}
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E;

        nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 0x1000001;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.nextNodePointer.nextPointer = %d",
                                         dev, nextPointerArray[0].pointerType,
                                         nextPointerArray[0].pointerData.nextNodePointer.nextPointer);

        nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 0;

        /*
            1.9. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                                 pointerData.routeEntryPointer {out of range blockSize [2],
                                                                                routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;

        nextPointerArray[0].pointerData.routeEntryPointer.blockSize        = 2;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.routeEntryPointer.blockSize = %d",
                                         dev, nextPointerArray[0].pointerType,
                                         nextPointerArray[0].pointerData.routeEntryPointer.blockSize);

        nextPointerArray[0].pointerData.routeEntryPointer.blockSize = 0;

        /*
            1.10. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                                 out of range pointerData.routeEntryPointer.routeEntryMethod [0x5AAAAAA5]}
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.routeEntryPointer.routeEntryMethod = %d",
                                     dev, nextPointerArray[0].pointerType,
                                     nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod);

        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        /*
            1.11. Call with nextPointerArrayPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nextPointerArrayPtr = NULL", dev);
    }

    memAddr             = 0;
    sramIndex           = 0;
    pointingToSramIndex = 0;
    numOfNextPointers   = 1;

    nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

    nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 0;
    nextPointerArray[0].pointerData.nextNodePointer.range5Index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                                                    numOfNextPointers, nextPointerArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpHwLpmNodeWrite
(
    IN GT_U8                                            dev,
    IN GT_U32                                           memAddr,
    IN GT_U32                                           sramIndex,
    IN GT_U32                                           pointingToSramIndex,
    IN CPSS_EXMXPM_IP_NEXT_POINTER_TYPE_ENT             nodeType,
    IN CPSS_EXMXPM_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  *rangeSelectSecPtr,
    IN GT_U32                                           numOfNextPointers,
    IN CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC         *nextPointerArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpHwLpmNodeWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with memAddr [0],
                   sramIndex [0],
                   pointingToSramIndex [0],
                   numOfNextPointers [1],
                   nodeType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E],
                   rangeSelectSecPtr->regularNodeType.bitVectorEntry {bitMap [0],
                                                                      rangeCounter [0]},
                   numOfNextPointers [1],
                   nextPointerArrayPtr[0] {pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E],
                                           pointerData.nextNodePointer {nextPointer [0], range5Index [0]}}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpHwLpmNodeRead with the same memAddr,
                                             sramIndex,
                                             nodeType,
                                             non-NULL rangeSelectSecPtr,
                                             non-NULL numOfNextPointersPtr,
                                             and non-NULL nextPointerArrayPtr.
    Expected: GT_OK and the same rangeSelectSecPtr, numOfNextPointers and nextPointerArrayPtr.
    1.3. Call with memAddr [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.4. Call with sramIndex [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.5. Call with pointingToSramIndex [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.6. Call with out of range nodeType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with nodeType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with rangeSelectSecPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.9. Call with numOfNextPointers [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.10. Call with out of range nextPointerArrayPtr->pointerType [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                                         out of range pointerData.nextNodePointer.range5Index [0 / 256]}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.12. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                                         out of range pointerData.nextNodePointer.nextPointer [0x1000001]}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                         pointerData.routeEntryPointer {out of range blockSize [2],
                                         routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.14. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                         out of range pointerData.routeEntryPointer.routeEntryMethod [0x5AAAAAA5]}
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with nextPointerArrayPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                           memAddr              = 0;
    GT_U32                                           sramIndex            = 0;
    GT_U32                                           pointingToSramIndex  = 0;
    CPSS_EXMXPM_IP_NEXT_POINTER_TYPE_ENT             nodeType             = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
    CPSS_EXMXPM_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  rangeSelectSec;
    GT_U32                                           numOfNextPointers    = 0;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC         nextPointerArray[IP_CTRL_ARRAY_SIZE_CNS];
#if 0
    CPSS_EXMXPM_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  rangeSelectSecGet;
    GT_U32                                           numOfNextPointersGet = 0;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC         nextPointerArrayGet[IP_CTRL_ARRAY_SIZE_CNS];
#endif

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memAddr [0],
                           sramIndex [0],
                           pointingToSramIndex [0],
                           numOfNextPointers [1],
                           nodeType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E],
                           rangeSelectSecPtr->regularNodeType.bitVectorEntry {bitMap [0],
                                                                              rangeCounter [0]},
                           numOfNextPointers [1],
                           nextPointerArrayPtr[0] {pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E],
                                                   pointerData.nextNodePointer {nextPointer [0], range5Index [1]}}.
            Expected: GT_OK.
        */
        memAddr             = 0;
        sramIndex           = 0;
        pointingToSramIndex = 0;
        nodeType            = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        rangeSelectSec.regularNodeType.bitVectorEntry[0].bitMap       = 0;
        rangeSelectSec.regularNodeType.bitVectorEntry[0].rangeCounter = 0;

        numOfNextPointers = 1;

        nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 0;
        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 0;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, memAddr, sramIndex,
                                    pointingToSramIndex, nodeType, numOfNextPointers);

        /*
            1.2. Call cpssExMxPmIpHwLpmNodeRead with the same memAddr,
                                                     sramIndex,
                                                     nodeType,
                                                     non-NULL rangeSelectSecPtr,
                                                     non-NULL numOfNextPointersPtr,
                                                     and non-NULL nextPointerArrayPtr.
            Expected: GT_OK and the same rangeSelectSecPtr, numOfNextPointers and nextPointerArrayPtr.
        */
#if 0
        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSecGet,
                                       nextPointerArrayGet, &numOfNextPointersGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpHwLpmNodeRead: %d, %d, %d, %d", dev, memAddr, sramIndex, nodeType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfNextPointers, numOfNextPointersGet,
                   "get another numOfNextPointers than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(rangeSelectSec.regularNodeType.bitVectorEntry[0].bitMap,
                                     rangeSelectSecGet.regularNodeType.bitVectorEntry[0].bitMap,
                   "get another rangeSelectSecPtr->regularNodeType.bitVectorEntry[0].bitMap than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(rangeSelectSec.regularNodeType.bitVectorEntry[0].rangeCounter,
                                     rangeSelectSecGet.regularNodeType.bitVectorEntry[0].rangeCounter,
                   "get another rangeSelectSecPtr->regularNodeType.bitVectorEntry[0].rangeCounter than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nextPointerArray[0].pointerType, nextPointerArrayGet[0].pointerType,
                   "get another nextPointerArrayPtr->pointerType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nextPointerArray[0].pointerData.nextNodePointer.nextPointer,
                                     nextPointerArrayGet[0].pointerData.nextNodePointer.nextPointer,
                   "get another nextPointerArrayPtr->pointerData.nextNodePointer.nextPointer than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(nextPointerArray[0].pointerData.nextNodePointer.range5Index,
                                     nextPointerArrayGet[0].pointerData.nextNodePointer.range5Index,
                   "get another nextPointerArrayPtr->pointerData.nextNodePointer.range5Index than was set: %d", dev);
#endif
        /*
            1.3. Call with memAddr [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        memAddr = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, memAddr);

        memAddr = 0;

        /*
            1.4. Call with sramIndex [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        sramIndex = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, sramIndex = %d", dev, sramIndex);

        sramIndex = 0;

        /*
            1.5. Call with pointingToSramIndex [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        pointingToSramIndex = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pointingToSramIndex = %d", dev, pointingToSramIndex);

        pointingToSramIndex = 0;

        /*
            1.6. Call with out of range nodeType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nodeType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nodeType = %d", dev, nodeType);

        /*
            1.7. Call with nodeType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        nodeType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nodeType = %d", dev, nodeType);

        nodeType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        /*
            1.8. Call with rangeSelectSecPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        NULL, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rangeSelectSecPtr = NULL", dev);

        /*
            1.9. Call with numOfNextPointers [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        numOfNextPointers = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfNextPointers = %d", dev, numOfNextPointers);

        numOfNextPointers = 1;

        /*
            1.10. Call with out of range nextPointerArrayPtr->pointerType [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nextPointerArray[0].pointerType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nextPointerArrayPtr->pointerType = %d",
                                     dev, nextPointerArray[0].pointerType);

        nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E;

        /*
            1.11. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                                                 out of range pointerData.nextNodePointer.range5Index [0 / 256]}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with nextPointerArrayPtr.pointerData.nextNodePointer.range5Index [0] */
        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 0;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.nextNodePointer.range5Index = %d",
                                         dev, nextPointerArray[0].pointerType,
                                         nextPointerArray[0].pointerData.nextNodePointer.range5Index);

        /* Call with nextPointerArrayPtr.pointerData.nextNodePointer.range5Index [256] */
        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 256;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.nextNodePointer.range5Index = %d",
                                         dev, nextPointerArray[0].pointerType,
                                         nextPointerArray[0].pointerData.nextNodePointer.range5Index);

        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 1;

        /*
            1.12. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                                                 out of range pointerData.nextNodePointer.nextPointer [0x1000001]}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 0x1000001;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.nextNodePointer.nextPointer = %d",
                                         dev, nextPointerArray[0].pointerType,
                                         nextPointerArray[0].pointerData.nextNodePointer.nextPointer);

        nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 0;

        /*
            1.13. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                                 pointerData.routeEntryPointer {out of range blockSize [2],
                                                 routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;

        nextPointerArray[0].pointerData.routeEntryPointer.blockSize        = 2;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.routeEntryPointer.blockSize = %d",
                                         dev, nextPointerArray[0].pointerType,
                                         nextPointerArray[0].pointerData.routeEntryPointer.blockSize);

        nextPointerArray[0].pointerData.routeEntryPointer.blockSize = 0;

        /*
            1.14. Call with nextPointerArrayPtr {pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E],
                                                 out of range pointerData.routeEntryPointer.routeEntryMethod [0x5AAAAAA5]}
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, nextPointerArray[0].pointerType = %d,"
                                                    "nextPointerArrayPtr.pointerData.routeEntryPointer.routeEntryMethod = %d",
                                     dev, nextPointerArray[0].pointerType,
                                     nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod);

        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        /*
            1.15. Call with nextPointerArrayPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nextPointerArrayPtr = NULL", dev);
    }

    memAddr             = 0;
    sramIndex           = 0;
    pointingToSramIndex = 0;
    nodeType            = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

    rangeSelectSec.regularNodeType.bitVectorEntry[0].bitMap       = 0;
    rangeSelectSec.regularNodeType.bitVectorEntry[0].rangeCounter = 0;

    numOfNextPointers = 1;

    nextPointerArray[0].pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

    nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 0;
    nextPointerArray[0].pointerData.nextNodePointer.range5Index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex, nodeType,
                                        &rangeSelectSec, numOfNextPointers, nextPointerArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpHwLpmNodeRead
(
    IN  GT_U8                                            dev,
    IN  GT_U32                                           memAddr,
    IN  GT_U32                                           sramIndex,
    IN  CPSS_EXMXPM_IP_NEXT_POINTER_TYPE_ENT             nodeType,
    OUT CPSS_EXMXPM_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  *rangeSelectSecPtr,
    OUT CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC         *nextPointerArrayPtr,
    OUT GT_U32                                           *numOfNextPointersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpHwLpmNodeRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with memAddr [0],
                   sramIndex [0],
                   nodeType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E /
                             CPSS_EXMXPM_IP_COMPRESSED_1_NODE_PTR_TYPE_E /
                             CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                   non-NULL rangeSelectSecPtr,
                   non-NULL nextPointerArrayPtr
                   and non-NULL numOfNextPointersPtr.
    Expected: GT_OK.
    1.2. Call with memAddr [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.3. Call with sramIndex [0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.4. Call with out of range nodeType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with nodeType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with rangeSelectSecPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with nextPointerArrayPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call with numOfNextPointersPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                           memAddr           = 0;
    GT_U32                                           sramIndex         = 0;
    CPSS_EXMXPM_IP_NEXT_POINTER_TYPE_ENT             nodeType          = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
    CPSS_EXMXPM_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  rangeSelectSec;
    GT_U32                                           numOfNextPointers = 0;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC         nextPointerArray[IP_CTRL_ARRAY_SIZE_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memAddr [0],
                           sramIndex [0],
                           nodeType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E /
                                     CPSS_EXMXPM_IP_COMPRESSED_1_NODE_PTR_TYPE_E /
                                     CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E],
                           non-NULL rangeSelectSecPtr,
                           non-NULL nextPointerArrayPtr
                           and non-NULL numOfNextPointersPtr.
            Expected: GT_OK.
        */
        /* nodeType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E */
        memAddr   = 0;
        sramIndex = 0;
        nodeType  = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, memAddr, sramIndex, nodeType);

        /* call with nodeType = CPSS_EXMXPM_IP_COMPRESSED_1_NODE_PTR_TYPE_E */
        nodeType  = CPSS_EXMXPM_IP_COMPRESSED_1_NODE_PTR_TYPE_E;

        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, memAddr, sramIndex, nodeType);

        /* call with nodeType = CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E */
        nodeType  = CPSS_EXMXPM_IP_COMPRESSED_2_NODE_PTR_TYPE_E;

        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, memAddr, sramIndex, nodeType);

        /*
            1.2. Call with memAddr [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        memAddr = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memAddr);

        memAddr = 0;

        /*
            1.3. Call with sramIndex [0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        sramIndex = 0xFFFFFFFF;

        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, sramIndex = %d", dev, sramIndex);

        sramIndex = 0;

        /*
            1.4. Call with out of range nodeType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        nodeType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nodeType = %d", dev, nodeType);

        /*
            1.5. Call with nodeType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        nodeType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;

        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, nodeType = %d", dev, nodeType);

        nodeType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        /*
            1.6. Call with rangeSelectSecPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, NULL,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rangeSelectSecPtr = NULL", dev);

        /*
            1.7. Call with nextPointerArrayPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       NULL, &numOfNextPointers);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nextPointerArrayPtr = NULL", dev);

        /*
            1.8. Call with numOfNextPointersPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfNextPointersPtr = NULL", dev);
    }

    memAddr   = 0;
    sramIndex = 0;
    nodeType  = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpHwLpmNodeRead(dev, memAddr, sramIndex, nodeType, &rangeSelectSec,
                                       nextPointerArray, &numOfNextPointers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McPclIdSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      pclId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McPclIdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with pclId [0 / 1023].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpv6McPclIdGet with not NULL pclIdPtr.
    Expected: GT_OK and the same pclId as was set.
    1.3. Call with pclId [1024] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;

    GT_U32      pclId = 0;
    GT_U32      pclIdGet = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with pclId [0 / 1023].
            Expected: GT_OK.
        */
        /* iterate with pclId = 0 */
        pclId = 0;

        st = cpssExMxPmIpv6McPclIdSet(dev, pclId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, pclId);

        /*
            1.2. Call cpssExMxPmIpv6McPclIdGet with not NULL pclIdPtr.
            Expected: GT_OK and the same pclId as was set.
        */
        st = cpssExMxPmIpv6McPclIdGet(dev, &pclIdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McPclIdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pclId, pclIdGet, "got another pclId than was set: %d", dev);

        /* iterate with pclId = 1023 */
        pclId = 1023;

        st = cpssExMxPmIpv6McPclIdSet(dev, pclId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, pclId);

        /*
            1.2. Call cpssExMxPmIpv6McPclIdGet with not NULL pclIdPtr.
            Expected: GT_OK and the same pclId as was set.
        */
        st = cpssExMxPmIpv6McPclIdGet(dev, &pclIdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McPclIdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pclId, pclIdGet, "got another pclId than was set: %d", dev);

        /*
            1.3. Call with pclId [1024] (out of range).
            Expected: NOT GT_OK.
        */
        pclId = 1024;

        st = cpssExMxPmIpv6McPclIdSet(dev, pclId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, pclId);
    }

    pclId = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McPclIdSet(dev, pclId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McPclIdSet(dev, pclId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McPclIdGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *pclIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McPclIdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL pclId.
    Expected: GT_OK.
    1.2. Call with pclId [NULL] (out of range).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;

    GT_U32      pclId = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL pclId.
            Expected: GT_OK.
        */
        st = cpssExMxPmIpv6McPclIdGet(dev, &pclId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with pclId [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McPclIdGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pclId = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McPclIdGet(dev, &pclId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McPclIdGet(dev, &pclId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McRuleSet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  GT_BOOL                                     isValid,
    IN  CPSS_EXMXPM_IPV6_MC_KEY_STC                 *maskPtr,
    IN  CPSS_EXMXPM_IPV6_MC_KEY_STC                 *patternPtr,
    IN  CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McRuleSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  ruleIndex [0 / 8192-1],
                    isValid [GT_TRUE / GT_FALSE],
                    mask { pclId [0x3FF],
                           dip[16] [0xFF,...,0xFF],
                           inlifId [0x3FFF],
                           vrId [0xFFF] }
                    pattern { pclId [0 / 1023],
                              dip[16] [0,...,0 / 255,...,255],
                              inlifId [0 / 0x3FFF],
                              vrId [0 / 4095] }
    action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
             pointerData { nextNodePointer { nextPointer [0 / 0x100000], range5Index [1 / 255] } } }
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpv6McRuleGet with not NULL pointers and other params from 1.1.
    Expected: GT_OK and the same params as was set. (for 1.1 and 1.13.)
  { ruleIndex }
    1.3. Call with ruleIndex [8192] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
  { mask }
    1.4. Call with mask->pclId [0x4FF] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with mask->inlifId [0x4FFF] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with mask->vrId [0x1FFF] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
  { pattern }
    1.7. Call with pattern->pclId [0x4FF] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with pattern->inlifId [0x4FFF] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with pattern->vrId [0x1FFF] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
  { action }
    1.10. Call with action->pointerType [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with action->pointerData.nextNodePointer.nextPointer [0x1000001] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    --- 1.12. Call with action->pointerData.nextNodePointer.range5Index [0 / 256] (out of range) and other params from 1.1.
    Expected: NOT GT_OK. ----

    1.13. Call with  ruleIndex [0 / 8192-1],
                    isValid [GT_TRUE / GT_FALSE],
                    mask { pclId [0x3FF],
                           dip[16] [0xFF,...,0xFF],
                           inlifId [0x3FFF],
                           vrId [0xFFF] }
                    pattern { pclId [0 / 1023],
                              dip[16] [0,...,0 / 255,...,255],
                              inlifId [0 / 0x3FFF],
                              vrId [0 / 4095] }
    action { pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E]
             pointerData { routeEntryPointer { routeEntryBaseMemAddr [0 / 100],
                                               blockSize [1 / 100],
                                               routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E /
                                                                 CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E] } } }
    Expected: GT_OK.
  { action }
    1.14. Call with action->pointerData.routeEntryPointer.routeEntryMethod [0x5AAAAAA5] (out of range) and other params from 1.13.
    Expected: GT_BAD_PARAM.

    1.15. Call with maskPtr [NULL] (out of range).
    Expected: GT_BAD_PTR.
    1.16. Call with patternPtr [NULL] (out of range).
    Expected: GT_BAD_PTR.
    1.17. Call with actionPtr [NULL] (out of range).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U32      i;
    GT_BOOL     isEqual = GT_FALSE;

    GT_U32                                      ruleIndex = 0;
    GT_BOOL                                     isValid = GT_FALSE;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    action;

    GT_BOOL                                     isValidGet = GT_FALSE;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 maskGet;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 patternGet;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    actionGet;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with  ruleIndex [0 / 8192-1],
                            isValid [GT_TRUE / GT_FALSE],
                            mask { pclId [0x3FF],
                                   dip[16] [0xFF,...,0xFF],
                                   inlifId [0x3FFF],
                                   vrId [0xFFF] }
                            pattern { pclId [0 / 1023],
                                      dip[16] [0,...,0 / 255,...,255],
                                      inlifId [0 / 0x3FFF],
                                      vrId [0 / 4095] }
            action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
                     pointerData { nextNodePointer { nextPointer [0 / 0x1000000], range5Index [1 / 255] } } }
            Expected: GT_OK.
        */
        /* iterate with ruleIndex = 0 */
        ruleIndex = 0;
        isValid = GT_TRUE;

        mask.pclId = 0x3FF;
        for(i=0; i<16; i++) mask.dip[i] = 0xFF;
        mask.inlifId = 0x3FFF;
        mask.vrId = 0xFFF;

        pattern.pclId = 0;
        for(i=0; i<16; i++) pattern.dip[i] = 0;
        pattern.inlifId = 0;
        pattern.vrId = 0;

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
        action.pointerData.nextNodePointer.nextPointer = 0;
        /* not needed - needed only when pointer type is compressed 2 */
        action.pointerData.nextNodePointer.range5Index = 1;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);

        /*
            1.2. Call cpssExMxPmIpv6McRuleGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK and the same params as was set. (for 1.1 and 1.13.)
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleGet: %d, %d", dev, ruleIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet, "got another isValid than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mask.pclId, maskGet.pclId, "got another mask.pclId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.dip, (GT_VOID*) &maskGet.dip, sizeof(mask.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another mask.dip[] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mask.inlifId, maskGet.inlifId, "got another mask.inlifId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mask.vrId, maskGet.vrId, "got another mask.vrId than was set: %d", dev);


        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.pclId, patternGet.pclId, "got another pattern.pclId than was set: %d", dev);

        for(i=0; i<16; i++) pattern.dip[i] = 0;
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.dip, (GT_VOID*) &patternGet.dip, sizeof(pattern.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another pattern.dip[] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.inlifId, patternGet.inlifId, "got another pattern.inlifId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.vrId, patternGet.vrId, "got another pattern.vrId than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerType, actionGet.pointerType, "got another action.pointerType than was set: %d", dev);
        /* since action.pointerType is regular node, only next pointer */
        /* field of the action.pointerData is valid                    */
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.nextNodePointer.nextPointer, actionGet.pointerData.nextNodePointer.nextPointer,
                                     "got another action.pointerData.nextNodePointer.nextPointer than was set: %d", dev);

        /* iterate with ruleIndex = 8192 - 1 */
        ruleIndex = 8192-1;
        isValid = GT_FALSE;

        mask.pclId = 0x3FF;
        for(i=0; i<16; i++) mask.dip[i] = 0xFF;
        mask.inlifId = 0x3FFF;
        mask.vrId = 0xFFF;

        pattern.pclId = 1023;
        for(i=0; i<16; i++) pattern.dip[i] = 255;
        pattern.inlifId = 0x3FFF;
        pattern.vrId = 4095;

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
        action.pointerData.nextNodePointer.nextPointer = 0x0100000;
        /* not needed - needed only when pointer type is compressed 2 */
        action.pointerData.nextNodePointer.range5Index = 255;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);

        /*
            1.2. Call cpssExMxPmIpv6McRuleGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK and the same params as was set. (for 1.1 and 1.13.)
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleGet: %d, %d", dev, ruleIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet, "got another isValid than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mask.pclId, maskGet.pclId, "got another mask.pclId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.dip, (GT_VOID*) &maskGet.dip, sizeof(mask.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another mask.dip[] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mask.inlifId, maskGet.inlifId, "got another mask.inlifId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mask.vrId, maskGet.vrId, "got another mask.vrId than was set: %d", dev);


        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.pclId, patternGet.pclId, "got another pattern.pclId than was set: %d", dev);

        /* in the set function, the pattern.dip was set to 0xffffff...    */
        /* the get read it ok, no need to zero the dip now before compare */
        /* for(i=0; i<16; i++) pattern.dip[i] = 0; */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.dip, (GT_VOID*) &patternGet.dip, sizeof(pattern.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another pattern.dip[] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.inlifId, patternGet.inlifId, "got another pattern.inlifId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.vrId, patternGet.vrId, "got another pattern.vrId than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerType, actionGet.pointerType, "got another action.pointerType than was set: %d", dev);
        /* since action.pointerType is regular node, only next pointer */
        /* field of the action.pointerData is valid                    */
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.nextNodePointer.nextPointer, actionGet.pointerData.nextNodePointer.nextPointer,
                                     "got another action.pointerData.nextNodePointer.nextPointer than was set: %d", dev);

        /*
          { ruleIndex }
            1.3. Call with ruleIndex [8192] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        ruleIndex = 8192;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
          { pattern }
            1.7. Call with pattern->pclId [0x4FF] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.pclId = 0x4FF;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern->pclId = %d", dev, pattern.pclId);

        pattern.pclId = 0;

        /*
            1.8. Call with pattern->inlifId [0x4FFF] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.inlifId = 0x4FFF;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern->inlifId = %d", dev, pattern.inlifId);

        pattern.inlifId = 0;

        /*
            1.9. Call with pattern->vrId [0x1FFF] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.vrId = 0x1FFF;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern->vrId = %d", dev, pattern.vrId);

        pattern.vrId = 0;

        /*
          { action }
            1.10. Call with action->pointerType [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.pointerType = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action->pointerType = %d", dev, action.pointerType);

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        /*
            1.11. Call with action->pointerData.nextNodePointer.nextPointer [0x1000001] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        action.pointerData.nextNodePointer.nextPointer = 0x1000001;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, action->pointerData.nextNodePointer.nextPointer = %d",
                                         dev, action.pointerData.nextNodePointer.nextPointer);

        action.pointerData.nextNodePointer.nextPointer = 0;

        /*
            1.12. Call with action->pointerData.nextNodePointer.range5Index [0 / 256] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.

        action.pointerData.nextNodePointer.range5Index = 0;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, action->pointerData.nextNodePointer.range5Index = %d",
                                         dev, action.pointerData.nextNodePointer.range5Index);

        action.pointerData.nextNodePointer.range5Index = 255;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, action->pointerData.nextNodePointer.range5Index = %d",
                                         dev, action.pointerData.nextNodePointer.range5Index);

        action.pointerData.nextNodePointer.range5Index = 1; */

        /*
            1.13. Call with  ruleIndex [0 / 8192-1],
                            isValid [GT_TRUE / GT_FALSE],
                            mask { pclId [0x3FF],
                                   dip[16] [0xFF,...,0xFF],
                                   inlifId [0x3FFF],
                                   vrId [0xFFF] }
                            pattern { pclId [0 / 1023],
                                      dip[16] [0,...,0 / 255,...,255],
                                      inlifId [0 / 0x3FFF],
                                      vrId [0 / 4095] }
            action { pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E]
                     pointerData { routeEntryPointer { routeEntryBaseMemAddr [0 / 100],
                                                       blockSize [1 / 100],
                                                       routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E /
                                                                         CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E] } } }
            Expected: GT_OK.
        */
        /* iterate with ruleIndex = 0 */
        ruleIndex = 0;
        isValid = GT_TRUE;

        mask.pclId = 0x3FF;
        for(i=0; i<16; i++) mask.dip[i] = 0xFF;
        mask.inlifId = 0x3FFF;
        mask.vrId = 0xFFF;

        pattern.pclId = 0;
        for(i=0; i<16; i++) pattern.dip[i] = 0;
        pattern.inlifId = 0;
        pattern.vrId = 0;

        action.pointerType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        action.pointerData.routeEntryPointer.routeEntryBaseMemAddr = 0;
        action.pointerData.routeEntryPointer.blockSize = 1;
        action.pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);

        /*
            1.2. Call cpssExMxPmIpv6McRuleGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK and the same params as was set. (for 1.1 and 1.13.)
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleGet: %d, %d", dev, ruleIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet, "got another isValid than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mask.pclId, maskGet.pclId, "got another mask.pclId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.dip, (GT_VOID*) &maskGet.dip, sizeof(mask.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another mask.dip[] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mask.inlifId, maskGet.inlifId, "got another mask.inlifId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mask.vrId, maskGet.vrId, "got another mask.vrId than was set: %d", dev);


        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.pclId, patternGet.pclId, "got another pattern.pclId than was set: %d", dev);

        for(i=0; i<16; i++) pattern.dip[i] = 0;
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.dip, (GT_VOID*) &patternGet.dip, sizeof(pattern.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another pattern.dip[] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.inlifId, patternGet.inlifId, "got another pattern.inlifId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.vrId, patternGet.vrId, "got another pattern.vrId than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerType, actionGet.pointerType, "got another action.pointerType than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.routeEntryBaseMemAddr, actionGet.pointerData.routeEntryPointer.routeEntryBaseMemAddr,
                                     "got another action.pointerData.routeEntryPointer.routeEntryBaseMemAddr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.blockSize, actionGet.pointerData.routeEntryPointer.blockSize,
                                     "got another action.pointerData.routeEntryPointer.blockSize than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.blockSize, actionGet.pointerData.routeEntryPointer.blockSize,
                                     "got another action.pointerData.routeEntryPointer.blockSize than was set: %d", dev);

        /* iterate with ruleIndex = 8192-1 */
        ruleIndex = 8192-1;
        isValid = GT_FALSE;

        mask.pclId = 0x3FF;
        for(i=0; i<16; i++) mask.dip[i] = 0xFF;
        mask.inlifId = 0x3FFF;
        mask.vrId = 0xFFF;

        pattern.pclId = 1023;
        for(i=0; i<16; i++) pattern.dip[i] = 255;
        pattern.inlifId = 0x3FFF;
        pattern.vrId = 4095;

        action.pointerType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        action.pointerData.routeEntryPointer.routeEntryBaseMemAddr = 0;
        action.pointerData.routeEntryPointer.blockSize = 1;
        action.pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);

        /*
            1.2. Call cpssExMxPmIpv6McRuleGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK and the same params as was set. (for 1.1 and 1.13.)
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleGet: %d, %d", dev, ruleIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet, "got another isValid than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mask.pclId, maskGet.pclId, "got another mask.pclId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.dip, (GT_VOID*) &maskGet.dip, sizeof(mask.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another mask.dip[] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mask.inlifId, maskGet.inlifId, "got another mask.inlifId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mask.vrId, maskGet.vrId, "got another mask.vrId than was set: %d", dev);


        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.pclId, patternGet.pclId, "got another pattern.pclId than was set: %d", dev);

        /* pattern.dip was set to 0xfffff..         */
        /* for(i=0; i<16; i++) pattern.dip[i] = 0;  */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.dip, (GT_VOID*) &patternGet.dip, sizeof(pattern.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another pattern.dip[] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.inlifId, patternGet.inlifId, "got another pattern.inlifId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pattern.vrId, patternGet.vrId, "got another pattern.vrId than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerType, actionGet.pointerType, "got another action.pointerType than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.routeEntryBaseMemAddr, actionGet.pointerData.routeEntryPointer.routeEntryBaseMemAddr,
                                     "got another action.pointerData.routeEntryPointer.routeEntryBaseMemAddr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.blockSize, actionGet.pointerData.routeEntryPointer.blockSize,
                                     "got another action.pointerData.routeEntryPointer.blockSize than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.blockSize, actionGet.pointerData.routeEntryPointer.blockSize,
                                     "got another action.pointerData.routeEntryPointer.blockSize than was set: %d", dev);

        /*
            1.14. Call with action->pointerData.routeEntryPointer.routeEntryMethod [0x5AAAAAA5] (out of range) and other params from 1.13.
            Expected: GT_BAD_PARAM.
        */
        action.pointerData.routeEntryPointer.routeEntryMethod = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, action.pointerData.routeEntryPointer.routeEntryMethod = %d", dev, action.pointerData.routeEntryPointer.routeEntryMethod);

        action.pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        /*
            1.15. Call with maskPtr [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, NULL, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mask = NULL", dev);

        /*
            1.16. Call with patternPtr [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, NULL, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pattern = NULL", dev);

        /*
            1.17. Call with actionPtr [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, action = NULL", dev);
    }

    ruleIndex = 0;
    isValid = GT_TRUE;

    mask.pclId = 0x3FF;
    for(i=0; i<16; i++) mask.dip[i] = 0xFF;
    mask.inlifId = 0x3FFF;
    mask.vrId = 0xFFF;

    pattern.pclId = 0;
    for(i=0; i<16; i++) pattern.dip[i] = 0;
    pattern.inlifId = 0;
    pattern.vrId = 0;

    action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
    action.pointerData.nextNodePointer.nextPointer = 0;
    action.pointerData.nextNodePointer.range5Index = 1;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McRuleGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    OUT GT_BOOL                                     *isValidPtr,
    OUT CPSS_EXMXPM_IPV6_MC_KEY_STC                 *maskPtr,
    OUT CPSS_EXMXPM_IPV6_MC_KEY_STC                 *patternPtr,
    OUT CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McRuleGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmIpv6McRuleSet with ruleIndex [0 / 8192-1],
                                            isValid [GT_TRUE / GT_FALSE],
                                            mask { pclId [0x3FF],
                                                   dip[16] [0xFF,...,0xFF],
                                                   inlifId [0x3FFF],
                                                   vrId [0xFFF] }
                                            pattern { pclId [0 / 1023],
                                                      dip[16] [0,...,0 / 255,...,255],
                                                      inlifId [0 / 0x3FFF],
                                                      vrId [0 / 4095] }
                                            action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
                                                     pointerData { nextNodePointer { nextPointer [0 / 0x1000000], range5Index [1 / 255] } } }
    Expected: GT_OK.
    1.2. Call with  ruleIndex [0 / 8192-1] and not NULL pointers.
    Expected: GT_OK.
    1.3. Call with  ruleIndex [8192] and not NULL pointers.
    Expected: NOT GT_OK.
    1.4. Call with isValidPtr [NULL] (out of range).
    Expected: GT_BAD_PTR.
    1.5. Call with maskPtr [NULL] (out of range).
    Expected: GT_BAD_PTR.
    1.6. Call with patternPtr [NULL] (out of range).
    Expected: GT_BAD_PTR.
    1.7. Call with actionPtr [NULL] (out of range).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       i;

    GT_U32                                      ruleIndex = 0;
    GT_BOOL                                     isValid = GT_FALSE;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    action;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with  ruleIndex [0 / 8192-1],
                            isValid [GT_TRUE / GT_FALSE],
                            mask { pclId [0x3FF],
                                   dip[16] [0xFF,...,0xFF],
                                   inlifId [0x3FFF],
                                   vrId [0xFFF] }
                            pattern { pclId [0 / 1023],
                                      dip[16] [0,...,0 / 255,...,255],
                                      inlifId [0 / 0x3FFF],
                                      vrId [0 / 4095] }
            action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
                     pointerData { nextNodePointer { nextPointer [0 / 0x1000000], range5Index [1 / 255] } } }
            Expected: GT_OK.
        */
        /* iterate with ruleIndex = 0 */
        ruleIndex = 0;
        isValid = GT_TRUE;

        mask.pclId = 0x3FF;
        for(i=0; i<16; i++) mask.dip[i] = 0xFF;
        mask.inlifId = 0x3FFF;
        mask.vrId = 0xFFF;

        pattern.pclId = 0;
        for(i=0; i<16; i++) pattern.dip[i] = 0;
        pattern.inlifId = 0;
        pattern.vrId = 0;

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
        action.pointerData.nextNodePointer.nextPointer = 0;
        action.pointerData.nextNodePointer.range5Index = 1;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        ruleIndex = 8192-1;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        /*
            1.2. Call with  ruleIndex [0 / 8192-1] and not NULL pointers.
            Expected: GT_OK.
        */
        ruleIndex = 0;

        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 8192-1;

        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.3. Call with  ruleIndex [8192] and not NULL pointers.
            Expected: NOT GT_OK.
        */
        ruleIndex = 8192;

        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.4. Call with isValidPtr [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, NULL, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isValid = NULL", dev);

        /*
            1.5. Call with maskPtr [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValid, NULL, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mask = NULL", dev);

        /*
            1.6. Call with patternPtr [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValid, &mask, NULL, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pattern = NULL", dev);

        /*
            1.7. Call with actionPtr [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValid, &mask, &pattern, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, action = NULL", dev);
    }

    ruleIndex = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValid, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McRuleActionUpdate
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McRuleActionUpdate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Create rule: call cpssExMxPmIpv6McRuleSet with
                    ruleIndex [0],
                    isValid [GT_TRUE],
                    mask { pclId [0x3FF],
                           dip[16] [0xFF,...,0xFF],
                           inlifId [0x3FFF],
                           vrId [0xFFF] }
                    pattern { pclId [0],
                              dip[16] [0,...,0],
                              inlifId [0],
                              vrId [0] }
    action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
             pointerData { nextNodePointer { nextPointer [0], range5Index [1] } } }
    Expected: GT_OK.
    1.2. Call with ruleIndex [0],
                    action { pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E]
                         pointerData { routeEntryPointer { routeEntryBaseMemAddr [0],
                                                           blockSize [1],
                                                           routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E] } } }
    Expected: GT_OK.
    1.3. Call cpssExMxPmIpv6McRuleGet with not NULL pointers and other params from 1.1.
    Expected: GT_OK and the same action as was set in 1.2.
    1.4. Call with  ruleIndex [8192] (out of range).
    Expected: NOT GT_OK.

    1.5. Call with action->pointerType [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.

  { action->pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E }
    1.6. Call with action->pointerData.nextNodePointer.nextPointer [0x1000001] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    --- 1.7. Call with action->pointerData.nextNodePointer.range5Index [0 / 256] (out of range) and other params from 1.1.
    Expected: NOT GT_OK. ---

  { action->pointerType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E }
    1.8. Call with action->pointerData.routeEntryPointer.routeEntryMethod [0x5AAAAAA5] (out of range) and other params from 1.13.
    Expected: GT_BAD_PARAM.

    1.9. Call with actionPtr [NULL] (out of range).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U32      i = 0;

    GT_U32 ruleIndex = 0;

    GT_BOOL                                     isValid = GT_FALSE;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    action;

    GT_BOOL                                     isValidGet = GT_FALSE;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 maskGet;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 patternGet;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    actionGet;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Create rule: call cpssExMxPmIpv6McRuleSet with
                            ruleIndex [0],
                            isValid [GT_TRUE],
                            mask { pclId [0x3FF],
                                   dip[16] [0xFF,...,0xFF],
                                   inlifId [0x3FFF],
                                   vrId [0xFFF] }
                            pattern { pclId [0],
                                      dip[16] [0,...,0],
                                      inlifId [0],
                                      vrId [0] }
            action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
                     pointerData { nextNodePointer { nextPointer [0], range5Index [1] } } }
            Expected: GT_OK.
        */
        ruleIndex = 0;
        isValid = GT_TRUE;

        mask.pclId = 0x3FF;
        for(i=0; i<16; i++) mask.dip[i] = 0xFF;
        mask.inlifId = 0x3FFF;
        mask.vrId = 0xFFF;

        pattern.pclId = 0;
        for(i=0; i<16; i++) pattern.dip[i] = 0;
        pattern.inlifId = 0;
        pattern.vrId = 0;

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
        action.pointerData.nextNodePointer.nextPointer = 0;
        action.pointerData.nextNodePointer.range5Index = 1;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        /*
            1.2. Call with ruleIndex [0],
                            action { pointerType [CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E]
                                 pointerData { routeEntryPointer { routeEntryBaseMemAddr [0],
                                                                   blockSize [1],
                                                                   routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E] } } }
            Expected: GT_OK.
        */
        action.pointerType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        action.pointerData.routeEntryPointer.routeEntryBaseMemAddr = 0;
        action.pointerData.routeEntryPointer.blockSize = 1;
        action.pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.3. Call cpssExMxPmIpv6McRuleGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK and the same action as was set in 1.2.
        */
        st = cpssExMxPmIpv6McRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleGet: %d, %d", dev, ruleIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerType, actionGet.pointerType, "got another action.pointerType than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.routeEntryBaseMemAddr, actionGet.pointerData.routeEntryPointer.routeEntryBaseMemAddr,
                                     "got another action.pointerData.routeEntryPointer.routeEntryBaseMemAddr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.blockSize, actionGet.pointerData.routeEntryPointer.blockSize,
                                     "got another action.pointerData.routeEntryPointer.blockSize than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.routeEntryPointer.blockSize, actionGet.pointerData.routeEntryPointer.blockSize,
                                     "got another action.pointerData.routeEntryPointer.blockSize than was set: %d", dev);

        /*
            1.4. Call with  ruleIndex [8192] (out of range).
            Expected: NOT GT_OK.
        */
        ruleIndex = 8192;

        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.5. Call with action->pointerType [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.pointerType = 8192;

        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.pointerType = %d", dev, action.pointerType);

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;

        /*
          { action->pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E }
            1.6. Call with action->pointerData.nextNodePointer.nextPointer [0x1000001] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
        action.pointerData.nextNodePointer.nextPointer = 0x1000001;
        action.pointerData.nextNodePointer.range5Index = 1;

        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, action.pointerData.nextNodePointer.nextPointer = %d",
                                         dev, action.pointerData.nextNodePointer.nextPointer);

        action.pointerData.nextNodePointer.nextPointer = 0;

        /*
            1.7. Call with action->pointerData.nextNodePointer.range5Index [0 / 256] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
        action.pointerData.nextNodePointer.nextPointer = 0;
        action.pointerData.nextNodePointer.range5Index = 0;

        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, action.pointerData.nextNodePointer.range5Index = %d",
                                         dev, action.pointerData.nextNodePointer.range5Index);

        action.pointerData.nextNodePointer.range5Index = 256;

        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, action.pointerData.nextNodePointer.range5Index = %d",
                                         dev, action.pointerData.nextNodePointer.range5Index);

        action.pointerData.nextNodePointer.range5Index = 1; */

        /*
          { action->pointerType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E }
            1.8. Call with action->pointerData.routeEntryPointer.routeEntryMethod [0x5AAAAAA5] (out of range) and other params from 1.13.
            Expected: GT_BAD_PARAM.
        */
        action.pointerType = CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        action.pointerData.routeEntryPointer.routeEntryBaseMemAddr = 0;
        action.pointerData.routeEntryPointer.blockSize = 1;
        action.pointerData.routeEntryPointer.routeEntryMethod = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, action.pointerData.routeEntryPointer.routeEntryMethod = %d",
                                 dev, action.pointerData.routeEntryPointer.routeEntryMethod);

        action.pointerData.routeEntryPointer.routeEntryMethod = IP_CTRL_INVALID_ENUM_CNS;

        /*
            1.9. Call with actionPtr [NULL] (out of range).
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, action = NULL", dev);
    }

    ruleIndex = 0;

    action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
    action.pointerData.nextNodePointer.nextPointer = 0;
    action.pointerData.nextNodePointer.range5Index = 1;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McRuleActionUpdate(dev, ruleIndex, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McRuleValidStatusSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      ruleIndex,
    IN  GT_BOOL     isValid
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McRuleValidStatusSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Create rule: call cpssExMxPmIpv6McRuleSet with
                    ruleIndex [0],
                    isValid [GT_TRUE],
                    mask { pclId [0x3FF],
                           dip[16] [0xFF,...,0xFF],
                           inlifId [0x3FFF],
                           vrId [0xFFF] }
                    pattern { pclId [0],
                              dip[16] [0,...,0],
                              inlifId [0],
                              vrId [0] }
    action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
             pointerData { nextNodePointer { nextPointer [0], range5Index [1] } } }
    Expected: GT_OK.
    1.2. Call with ruleIndex [0],
                   isValid [GT_FALSE].
    Expected: GT_OK.
    1.3. Call cpssExMxPmIpv6McRuleValidStatusGet with not NULL isValidPtr and ruleIndex from 1.1.
    Expected: GT_OK and the same isValid as was set.
    1.4. Call with ruleIndex [8192] (out of range) and isValid from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       i;

    CPSS_EXMXPM_IPV6_MC_KEY_STC                 mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    action;

    GT_U32      ruleIndex = 0;
    GT_BOOL     isValid = GT_FALSE;
    GT_BOOL     isValidGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Create rule: call cpssExMxPmIpv6McRuleSet with
                            ruleIndex [0],
                            isValid [GT_TRUE],
                            mask { pclId [0x3FF],
                                   dip[16] [0xFF,...,0xFF],
                                   inlifId [0x3FFF],
                                   vrId [0xFFF] }
                            pattern { pclId [0],
                                      dip[16] [0,...,0],
                                      inlifId [0],
                                      vrId [0] }
            action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
                     pointerData { nextNodePointer { nextPointer [0], range5Index [1] } } }
            Expected: GT_OK.
        */
        ruleIndex = 0;
        isValid = GT_TRUE;

        mask.pclId = 0x3FF;
        for(i=0; i<16; i++) mask.dip[i] = 0xFF;
        mask.inlifId = 0x3FFF;
        mask.vrId = 0xFFF;

        pattern.pclId = 0;
        for(i=0; i<16; i++) pattern.dip[i] = 0;
        pattern.inlifId = 0;
        pattern.vrId = 0;

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
        action.pointerData.nextNodePointer.nextPointer = 0;
        action.pointerData.nextNodePointer.range5Index = 1;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        /*
            1.2. Call with ruleIndex [0],
                           isValid [GT_FALSE].
            Expected: GT_OK.
        */
        /* iterate with ruleIndex = 0 */
        ruleIndex = 0;
        isValid = GT_FALSE;

        st = cpssExMxPmIpv6McRuleValidStatusSet(dev, ruleIndex, isValid);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);

        /*
            1.3. Call cpssExMxPmIpv6McRuleValidStatusGet with not NULL isValidPtr and ruleIndex from 1.1.
            Expected: GT_OK and the same isValid as was set.
        */
        st = cpssExMxPmIpv6McRuleValidStatusGet(dev, ruleIndex, &isValidGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleValidStatusGet: %d, %d", dev, ruleIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet, "got another isValid than was set: %d", dev);

        /*
            1.4. Call with ruleIndex [8192] (out of range) and isValid from 1.1.
            Expected: NOT GT_OK.
        */
        ruleIndex = 8192;

        st = cpssExMxPmIpv6McRuleValidStatusSet(dev, ruleIndex, isValid);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);
    }

    ruleIndex = 0;
    isValid = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McRuleValidStatusSet(dev, ruleIndex, isValid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McRuleValidStatusSet(dev, ruleIndex, isValid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpv6McRuleValidStatusGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      ruleIndex,
    OUT GT_BOOL     *isValidPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McRuleValidStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Create rule: call cpssExMxPmIpv6McRuleSet with
                    ruleIndex [0],
                    isValid [GT_TRUE],
                    mask { pclId [0x3FF],
                           dip[16] [0xFF,...,0xFF],
                           inlifId [0x3FFF],
                           vrId [0xFFF] }
                    pattern { pclId [0],
                              dip[16] [0,...,0],
                              inlifId [0],
                              vrId [0] }
    action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
             pointerData { nextNodePointer { nextPointer [0], range5Index [1] } } }
    Expected: GT_OK.
    1.2. Call with ruleIndex [0],
                   not NULL isValidPtr.
    Expected: GT_OK.
    1.3. Call with ruleIndex [8192] (out of range) and isValid from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with isValidPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       i;

    CPSS_EXMXPM_IPV6_MC_KEY_STC                 mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    action;

    GT_U32  ruleIndex = 0;
    GT_BOOL isValid = GT_FALSE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Create rule: call cpssExMxPmIpv6McRuleSet with
                            ruleIndex [0],
                            isValid [GT_TRUE],
                            mask { pclId [0x3FF],
                                   dip[16] [0xFF,...,0xFF],
                                   inlifId [0x3FFF],
                                   vrId [0xFFF] }
                            pattern { pclId [0],
                                      dip[16] [0,...,0],
                                      inlifId [0],
                                      vrId [0] }
            action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
                     pointerData { nextNodePointer { nextPointer [0], range5Index [1] } } }
            Expected: GT_OK.
        */
        ruleIndex = 0;
        isValid = GT_TRUE;

        mask.pclId = 0x3FF;
        for(i=0; i<16; i++) mask.dip[i] = 0xFF;
        mask.inlifId = 0x3FFF;
        mask.vrId = 0xFFF;

        pattern.pclId = 0;
        for(i=0; i<16; i++) pattern.dip[i] = 0;
        pattern.inlifId = 0;
        pattern.vrId = 0;

        action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
        action.pointerData.nextNodePointer.nextPointer = 0;
        action.pointerData.nextNodePointer.range5Index = 1;

        st = cpssExMxPmIpv6McRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        /*
            1.1. Call with ruleIndex [0],
                           not NULL isValidPtr.
            Expected: GT_OK.
        */
        /* iterate with ruleIndex = 0 */
        ruleIndex = 0;

        st = cpssExMxPmIpv6McRuleValidStatusGet(dev, ruleIndex, &isValid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.2. Call with ruleIndex [8192] (out of range) and isValid from 1.1.
            Expected: NOT GT_OK.
        */
        ruleIndex = 8192;

        st = cpssExMxPmIpv6McRuleValidStatusGet(dev, ruleIndex, &isValid);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.3. Call with isValidPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpv6McRuleValidStatusGet(dev, ruleIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isValid = NULL", dev);
    }

    ruleIndex = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpv6McRuleValidStatusGet(dev, ruleIndex, &isValid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpv6McRuleValidStatusGet(dev, ruleIndex, &isValid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpEcmpUcRpfCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpEcmpUcRpfCheckEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK
    1.2. Call cpssExMxPmIpEcmpUcRpfCheckEnableGet with not NULL enablePtr
    Expected: GT_OK and the same params as was set.
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
            1.1. Call with enable [GT_TRUE / GT_FALSE]
            Expected: GT_OK
        */
        enable = GT_TRUE;

        st = cpssExMxPmIpEcmpUcRpfCheckEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmIpEcmpUcRpfCheckEnableGet with not NULL enablePtr
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmIpEcmpUcRpfCheckEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpEcmpUcRpfCheckEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got anothe enable than was set: %d", dev);

        enable = GT_FALSE;

        st = cpssExMxPmIpEcmpUcRpfCheckEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmIpEcmpUcRpfCheckEnableGet with not NULL enablePtr
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmIpEcmpUcRpfCheckEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpEcmpUcRpfCheckEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got anothe enable than was set: %d", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpEcmpUcRpfCheckEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpEcmpUcRpfCheckEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpEcmpUcRpfCheckEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpEcmpUcRpfCheckEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL enablePtr
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
            1.1. Call with not NULL enablePtr
            Expected: GT_OK.
        */
        st = cpssExMxPmIpEcmpUcRpfCheckEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpEcmpUcRpfCheckEnableGet(dev, NULL);
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
        st = cpssExMxPmIpEcmpUcRpfCheckEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpEcmpUcRpfCheckEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*caesar*/



/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK
    1.2. Call cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet with not NULL enablePtr
    Expected: GT_OK and the same params as was set.
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
            1.1. Call with enable [GT_TRUE / GT_FALSE]
            Expected: GT_OK
        */
        enable = GT_TRUE;

        st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet with not NULL enablePtr
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got anothe enable than was set: %d", dev);

        enable = GT_FALSE;

        st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet with not NULL enablePtr
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got anothe enable than was set: %d", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL enablePtr
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
            1.1. Call with not NULL enablePtr
            Expected: GT_OK.
        */
        st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet(dev, NULL);
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
        st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#if 0
/*----------------------------------------------------------------------------*/
/*
    Test function to Fill IPv6 MC Rule with Mask, Pattern and Action table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpv6McRuleTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in table.
         Call cpssExMxPmIpv6McRuleSet with  ruleIndex [0 / numEntries-1],
                                            isValid [GT_TRUE / GT_FALSE],
                                            mask { pclId [0x3FF],
                                                   dip[16] [0xFF,...,0xFF],
                                                   inlifId [0x3FFF],
                                                   vrId [0xFFF] }
                                            pattern { pclId [0 / 1023],
                                                      dip[16] [0,...,0 / 255,...,255],
                                                      inlifId [0 / 0x3FFF],
                                                      vrId [0 / 4095] }
                                            action { pointerType [CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E]
                                                     pointerData { nextNodePointer { nextPointer [0 / 0x1000000], range5Index [1 / 255] } } }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIpv6McRuleSet with ruleIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in table and compare with original.
         Call cpssExMxPmIpv6McRuleGet with the same ruleIndex and non-NULL pointers.
    Expected: GT_OK and the same params as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIpv6McRuleGet with ruleIndex [numEntries] and non-NULL arpMacAddrPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;
    GT_U32      i          = 0;

    GT_BOOL                                     isValid = GT_FALSE;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    action;

    GT_BOOL                                     isValidGet = GT_FALSE;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 maskGet;
    CPSS_EXMXPM_IPV6_MC_KEY_STC                 patternGet;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC    actionGet;


    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* Fill the IPv6 MC Rule with Mask, Pattern and Action for table */
    isValid = GT_TRUE;

    mask.pclId = 0x3FF;
    for(iTemp=0; iTemp<16; iTemp++) mask.dip[iTemp] = 0xFF;
    mask.inlifId = 0x3FFF;
    mask.vrId = 0xFFF;

    pattern.pclId = 0;
    for(iTemp=0; iTemp<16; iTemp++) pattern.dip[iTemp] = 0;
    pattern.inlifId = 0;
    pattern.vrId = 0;

    action.pointerType = CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E;
    action.pointerData.nextNodePointer.nextPointer = 0;
    /* since pointer type is regular, range5Index is not relevant */
    action.pointerData.nextNodePointer.range5Index = 1;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 8192;

        /* 1.2. Fill all entries in table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            pattern.pclId = (GT_U32)(iTemp % 0x3FF);
            pattern.inlifId = (GT_U32)(iTemp % 0x3FFF);
            pattern.vrId = (GT_U32)(iTemp % 0xFFF);

            st = cpssExMxPmIpv6McRuleSet(dev, iTemp, isValid, &mask, &pattern, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleSet: %d, %d, %d", dev, iTemp, isValid);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmIpv6McRuleSet(dev, numEntries, isValid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleSet: %d, %d, %d", dev, numEntries, isValid);

        /* 1.4. Read all entries in table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            pattern.pclId = (GT_U32)(iTemp % 0x3FF);
            pattern.inlifId = (GT_U32)(iTemp % 0x3FFF);
            pattern.vrId = (GT_U32)(iTemp % 0xFFF);

            st = cpssExMxPmIpv6McRuleGet(dev, iTemp, &isValidGet, &maskGet, &patternGet, &actionGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleGet: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet, "got another isValid than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(mask.pclId, maskGet.pclId, "got another mask.pclId than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.dip, (GT_VOID*) &maskGet.dip, sizeof(mask.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another mask.dip[] than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(mask.inlifId, maskGet.inlifId, "got another mask.inlifId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(mask.vrId, maskGet.vrId, "got another mask.vrId than was set: %d", dev);


            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.pclId, patternGet.pclId, "got another pattern.pclId than was set: %d", dev);

            for(i=0; i<16; i++) pattern.dip[i] = 0;
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.dip, (GT_VOID*) &patternGet.dip, sizeof(pattern.dip[0] * 16))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another pattern.dip[] than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.inlifId, patternGet.inlifId, "got another pattern.inlifId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.vrId, patternGet.vrId, "got another pattern.vrId than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerType, actionGet.pointerType, "got another action.pointerType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(action.pointerData.nextNodePointer.nextPointer, actionGet.pointerData.nextNodePointer.nextPointer,
                                         "got another action.pointerData.nextNodePointer.nextPointer than was set: %d", dev);
            /* since pointer type is regular, range5Index is not relevant */
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmIpv6McRuleGet(dev, numEntries, &isValidGet, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpv6McRuleGet: %d, %d", dev, numEntries);
    }
}
#endif
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*
  GT_STATUS cpssExMxPmIpRouterPortMacSaLsbSet
  (
   IN GT_U8   devNum,
   IN GT_U8   portNum,
   IN GT_U32   saMac
  )
  */
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterPortMacSaLsbSet)
{
    /*
          ITERATE_DEVICES_VIRT_PORT (ExMxPm)
          1.1.1. Call with saMac [0 / 0xFF].
          Expected: GT_OK.
          1.1.2. Call cpssExMxPmIpRouterPortMacSaLsbGet with non-NULL saMacPtr.
          Expected: GT_OK and the same saMac.
          1.1.3. Call with saMac [256].
          Expected: GT_BAD_PARAM.
    */
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_VIRT_PORT_CNS;
    GT_U32      saMac    = 0;
    GT_U32      saMacGet = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
             *    1.1.1. Call with saMac [0 / 0xFF].
             *    Expected: GT_OK.
             *    */

            /* Call with saMac [0] */
            saMac = 0;

            st = cpssExMxPmIpRouterPortMacSaLsbSet(dev, port, saMac);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saMac);
            /*
             *    1.1.2. Call cpssExMxPmIpRouterPortMacSaLsbGet with non-NULL saMacPtr.
             *    Expected: GT_OK and the same saMac.
             *    */
            st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, &saMacGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssExMxPmIpRouterPortMacSaLsbGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saMac, saMacGet,
                    "get another saMac than was set: %d, %d", dev, port);

            /* Call with saMac [0xFF] */
            saMac = 0xFF;

            st = cpssExMxPmIpRouterPortMacSaLsbSet(dev, port, saMac);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saMac);

            /*
             *    1.1.3. Call cpssExMxPmIpRouterPortMacSaLsbGet with non-NULL saMacPtr.
             *    Expected: GT_OK and the same saMac.
             *    */
            st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, &saMacGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssExMxPmIpRouterPortMacSaLsbGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saMac, saMacGet,
                    "get another saMac than was set: %d, %d", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /*
                1.1.2. Call with saMac [256].
                Expected: GT_BAD_PARAM.
        */
        saMac = 256;

        st = cpssExMxPmIpRouterPortMacSaLsbSet(dev, port, saMac);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, saMac);

        saMac = 0;


        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmIpRouterPortMacSaLsbSet(dev, port, saMac);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpRouterPortMacSaLsbSet(dev, port, saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmIpRouterPortMacSaLsbSet(dev, port, saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    saMac = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterPortMacSaLsbSet(dev, port, saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterPortMacSaLsbSet(dev, port, saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
 * GT_STATUS cpssExMxPmIpRouterPortMacSaLsbGet
 * (
 *     IN  GT_U8   devNum,
 *     IN  GT_U8   portNum,
 *     OUT GT_U32   *saMacPtr
 * )
 */
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterPortMacSaLsbGet)
{
    /*
     *   ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
     *   1.1.1. Call with non-NULL saLsbModePtr.
     *   Expected: GT_OK.
     *   1.1.2. Call with saLsbModePtr [NULL].
     *   Expected: GT_BAD_PTR.
     *   */
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_VIRT_PORT_CNS;

    GT_U32   saMac = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
             * 1.1.1. Call with non-NULL saLsbModePtr.
             * Expected: GT_OK.
             * */
            st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, &saMac);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
             * 1.1.2. Call with saLsbModePtr [NULL].
             * Expected: GT_BAD_PTR.
             * */
            st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, saLsbModePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, &saMac);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, &saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterPortMacSaLsbGet(dev, port, &saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmIpCtrl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmIpCtrl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterEngineEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterEngineEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpSpecialRouterTriggerEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpSpecialRouterTriggerEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpExceptionCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpExceptionCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouteEntryAgeRefreshEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouteEntryAgeRefreshEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpMtuProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpMtuProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6AddrPrefixScopeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6AddrPrefixScopeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6SourcePortSiteIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6SourcePortSiteIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6UcScopeCommandSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6UcScopeCommandGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McScopeCommandSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McScopeCommandGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpQosModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpQosModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterMacSaBaseSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterMacSaBaseGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpPortRouterMacSaLsbModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpPortRouterMacSaLsbModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterVlanMacSaLsbSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterVlanMacSaLsbGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpArpBcCommandSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpArpBcCommandGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmMemoryOffsetSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmMemoryOffsetGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpHwLpmNodeNextPointersWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpHwLpmNodeWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpHwLpmNodeRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McPclIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McPclIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McRuleGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McRuleActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McRuleValidStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McRuleValidStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpEcmpUcRpfCheckEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpEcmpUcRpfCheckEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterPortMacSaLsbSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterPortMacSaLsbGet)

    /* Tests for Tables */
/*    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpv6McRuleTable) */
UTF_SUIT_END_TESTS_MAC(cpssExMxPmIpCtrl)

