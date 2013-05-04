/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgSecurityBreachUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgSecurityBreach, that provides
*       CPSS ExMxPm Bridge Security Breach Functionality API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgSecurityBreach.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_BREACH_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define BRG_BREACH_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define BRG_BREACH_TESTED_VLAN_ID_CNS  100

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet with non-NULL enablePtr. 
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = BRG_BREACH_VALID_VIRT_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

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

            st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet with non-NULL enablePtr. 
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet: %d, %d", dev, port);

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
            st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_BREACH_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr. 
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = BRG_BREACH_VALID_VIRT_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;
    
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
            st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_BREACH_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachEventDropModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    IN CPSS_DROP_MODE_TYPE_ENT              dropMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachEventDropModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E]
                   and dropMode [CPSS_DROP_MODE_SOFT_E /
                                 CPSS_DROP_MODE_HARD_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgSecurityBreachEventDropModeGet with same eventType and non-NULL dropModePtr.
    Expected: GT_OK and the same dropMode.
    1.3. Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E] (not acceptable)
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.4. Call with out of range eventType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range dropMode[0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_BRG_SECUR_BREACH_EVENTS_ENT    eventType   = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E;
    CPSS_DROP_MODE_TYPE_ENT             dropMode    = CPSS_DROP_MODE_SOFT_E;
    CPSS_DROP_MODE_TYPE_ENT             dropModeGet = CPSS_DROP_MODE_SOFT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E]
                           and dropMode [CPSS_DROP_MODE_SOFT_E /
                                         CPSS_DROP_MODE_HARD_E].
            Expected: GT_OK.
        */

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E;
        dropMode  = CPSS_DROP_MODE_SOFT_E;

        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, eventType, dropMode);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachEventDropModeGet with same eventType and non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgGenDropIpMcDropModeGet: %d, %d", dev, eventType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, eventType, dropMode);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachEventDropModeGet with same eventType and non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgGenDropIpMcDropModeGet: %d, %d", dev, eventType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E;
        dropMode  = CPSS_DROP_MODE_HARD_E;

        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, eventType, dropMode);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachEventDropModeGet with same eventType and non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgGenDropIpMcDropModeGet: %d, %d", dev, eventType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, eventType, dropMode);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachEventDropModeGet with same eventType and non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgGenDropIpMcDropModeGet: %d, %d", dev, eventType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, eventType, dropMode);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachEventDropModeGet with same eventType and non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgGenDropIpMcDropModeGet: %d, %d", dev, eventType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /*
            1.3. Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E] (not acceptable)
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /*
            1.4. Call with out of range eventType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        eventType = BRG_BREACH_INVALID_ENUM_CNS;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, eventType);

        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E;

        /*
            1.5. Call with out of range dropMode[0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dropMode = BRG_BREACH_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dropMode = %d", dev, dropMode);
    }

    eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E;
    dropMode  = CPSS_DROP_MODE_SOFT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachEventDropModeSet(dev, eventType, dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachEventDropModeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_BRG_SECUR_BREACH_EVENTS_ENT    eventType,
    OUT CPSS_DROP_MODE_TYPE_ENT             *dropModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachEventDropModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E]
                   and non-NULL dropModePtr.
    Expected: GT_OK.
    1.2. Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E /
                              CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E] (not acceptable)
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.3. Call with out of range eventType[0x5AAAAAA5]
                   and and other parameters from 1.1.
    Expected: GT_BAD_PARAM. 
    1.4. Call with dropModePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_BRG_SECUR_BREACH_EVENTS_ENT    eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E;
    CPSS_DROP_MODE_TYPE_ENT             dropMode  = CPSS_DROP_MODE_SOFT_E;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E]
                           and non-NULL dropModePtr.
            Expected: GT_OK.
        */

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E;

        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /*
            1.2. Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E /
                                      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E] (not acceptable)
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /* Call with eventType [CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E] */
        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, eventType);

        /*
            1.3. Call with out of range eventType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        eventType = BRG_BREACH_INVALID_ENUM_CNS;
        
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, eventType);

        eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E;

        /*
            1.4. Call with dropModePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropModePtr = NULL", dev);
    }

    eventType = CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachEventDropModeGet(dev, eventType, &dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachMsgGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_BRG_SECUR_BREACH_MSG_STC   *msgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachMsgGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null msgPtr. 
    Expected: GT_OK.
    1.2. Call with msgPtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_BRG_SECUR_BREACH_MSG_STC  msg;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL msgPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgSecurityBreachMsgGet(dev, &msg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null msgPtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSecurityBreachMsgGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, msgPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachMsgGet(dev, &msg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachMsgGet(dev, &msg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachGlobalDropCntrSet
(
    IN GT_U8    devNum,
    IN GT_U32   counterValue
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachGlobalDropCntrSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterValue [0 / 100].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgSecurityBreachGlobalDropCntrGet with non-NULL counterValuePtr. 
    Expected: GT_OK and the same counterValue.
    1.3. Call with counterValue [0xFFFFFFFF] (no any constraints).
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      counterValue    = 0;
    GT_U32      counterValueGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with counterValue [0 / 100].
            Expected: GT_OK.
        */

        /* Call with counterValue [0] */
        counterValue = 0;

        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrSet(dev, counterValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterValue);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachGlobalDropCntrGet with non-NULL counterValuePtr. 
            Expected: GT_OK and the same counterValue.
        */
        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrGet(dev, &counterValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgSecurityBreachGlobalDropCntrGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterValue, counterValueGet,
                       "get another counterValue than was set: %d", dev);

        /* Call with counterValue [100] */
        counterValue = 100;

        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrSet(dev, counterValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterValue);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachGlobalDropCntrGet with non-NULL counterValuePtr. 
            Expected: GT_OK and the same counterValue.
        */
        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrGet(dev, &counterValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgSecurityBreachGlobalDropCntrGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterValue, counterValueGet,
                       "get another counterValue than was set: %d", dev);

        /*
            1.3. Call with counterValue [0xFFFFFFFF] (no any constraints).
            Expected: GT_OK.
        */
        counterValue = 0xFFFFFFFF;

        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrSet(dev, counterValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterValue);
    }

    counterValue = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrSet(dev, counterValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachGlobalDropCntrSet(dev, counterValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachGlobalDropCntrGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *counterValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachGlobalDropCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null counterValuePtr. 
    Expected: GT_OK.
    1.2. Call with counterValuePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      counterValue = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL counterValuePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrGet(dev, &counterValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null counterValuePtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterValuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachGlobalDropCntrGet(dev, &counterValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachGlobalDropCntrGet(dev, &counterValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet
(
    IN GT_U8                                            devNum,
    IN CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC  *cntrCfgPtr
)      
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrCfgPtr {dropCntMode [CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E],
                               port [0]}.
    Expected: GT_OK.
    1.2. Call with cntrCfgPtr {dropCntMode [CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_FID_E],
                               fId [100]}.
    Expected: GT_OK.
    1.3. Call cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet with non-NULL cntrCfgPtr.
    Expected: GT_OK and the same cntrCfgPtr.
    1.4. Call with out of range cntrCfgPtr->dropCntMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with cntrCfgPtr {dropCntMode [CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E],
                               out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]}.
    Expected: NOT GT_OK.
    1.9. Call with cntrCfgPtr {dropCntMode [CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_FID_E],
                               out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (ignored) }.
    Expected: GT_OK.
    1.10. Call with cntrCfgPtr [NULL]. 
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC cntrCfg;
    CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC cntrCfgGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with cntrCfgPtr {dropCntMode [CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E],
                                       port [0]}.
            Expected: GT_OK.
        */
        cntrCfg.dropCntMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E;
        cntrCfg.port        = BRG_BREACH_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(dev, &cntrCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet with non-NULL cntrCfgPtr.
            Expected: GT_OK and the same cntrCfgPtr.
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet(dev, &cntrCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrCfg.dropCntMode, cntrCfgGet.dropCntMode,
                       "get another cntrCfgPtr->dropCntMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrCfg.port, cntrCfgGet.port,
                       "get another cntrCfgPtr->port than was set: %d", dev);

        /*
            1.2. Call with cntrCfgPtr {dropCntMode [CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_FID_E],
                                       port [100]}.
            Expected: GT_OK.
        */
        cntrCfg.dropCntMode = CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_FID_E;
        cntrCfg.fId         = 0;

        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(dev, &cntrCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet with non-NULL cntrCfgPtr.
            Expected: GT_OK and the same cntrCfgPtr.
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet(dev, &cntrCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrCfg.dropCntMode, cntrCfgGet.dropCntMode,
                       "get another cntrCfgPtr->dropCntMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrCfg.fId, cntrCfgGet.fId,
                       "get another cntrCfgPtr->fId than was set: %d", dev);

        /*
            1.4. Call with out of range cntrCfgPtr->dropCntMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        cntrCfg.dropCntMode = BRG_BREACH_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(dev, &cntrCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cntrCfgPtr->dropCntMode = %d", dev, cntrCfg.dropCntMode);

        cntrCfg.dropCntMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E;

        /*
            1.5. Call with cntrCfgPtr {dropCntMode [CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E],
                                       out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]}.
            Expected: NOT GT_OK.
        */
        cntrCfg.dropCntMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E;
        cntrCfg.port        = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(dev, &cntrCfg);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, cntrCfgPtr->dropCntMode = %d, cntrCfgPtr->port = %d",
                                         dev, cntrCfg.dropCntMode, cntrCfg.port);

        cntrCfg.port = BRG_BREACH_VALID_VIRT_PORT_CNS;

        /*
            1.9. Call with cntrCfgPtr {dropCntMode [CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_FID_E],
                                       out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (ignored) }.
            Expected: GT_OK.
        */
        cntrCfg.dropCntMode = CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_FID_E;
        cntrCfg.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(dev, &cntrCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, cntrCfgPtr->dropCntMode = %d, cntrCfgPtr->port = %d",
                                     dev, cntrCfg.dropCntMode, cntrCfg.port);

        cntrCfg.port = BRG_BREACH_VALID_VIRT_PORT_CNS;

        /*
            1.10. Call with cntrCfgPtr [NULL]. 
            Expected: GT_BAD_PTR
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrCfgPtr = NULL", dev);
    }

    cntrCfg.dropCntMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E;
    cntrCfg.port        = BRG_BREACH_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(dev, &cntrCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(dev, &cntrCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet
(
    IN  GT_U8                                              devNum,
    OUT CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC    *cntrCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null cntrCfgPtr. 
    Expected: GT_OK.
    1.2. Call with cntrCfgPtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC cntrCfg;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL cntrCfgPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet(dev, &cntrCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null cntrCfgPtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrCfgPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet(dev, &cntrCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet(dev, &cntrCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachPortVlanCntrSet
(
    IN GT_U8    devNum,
    IN GT_U32   counterValue
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachPortVlanCntrSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterValue [0 / 100].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgSecurityBreachPortVlanCntrGet with non-NULL counterValuePtr. 
    Expected: GT_OK and the same counterValue.
    1.3. Call with counterValue [0xFFFFFFFF] (no any constraints).
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      counterValue    = 0;
    GT_U32      counterValueGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with counterValue [0 / 100].
            Expected: GT_OK.
        */

        /* Call with counterValue [0] */
        counterValue = 0;

        st = cpssExMxPmBrgSecurityBreachPortVlanCntrSet(dev, counterValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterValue);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachPortVlanCntrGet with non-NULL counterValuePtr. 
            Expected: GT_OK and the same counterValue.
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanCntrGet(dev, &counterValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgSecurityBreachPortVlanCntrGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterValue, counterValueGet,
                       "get another counterValue than was set: %d", dev);

        /* Call with counterValue [100] */
        counterValue = 100;

        st = cpssExMxPmBrgSecurityBreachPortVlanCntrSet(dev, counterValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterValue);

        /*
            1.2. Call cpssExMxPmBrgSecurityBreachPortVlanCntrGet with non-NULL counterValuePtr. 
            Expected: GT_OK and the same counterValue.
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanCntrGet(dev, &counterValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgSecurityBreachPortVlanCntrGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterValue, counterValueGet,
                       "get another counterValue than was set: %d", dev);

        /*
            1.3. Call with counterValue [0xFFFFFFFF] (no any constraints).
            Expected: GT_OK.
        */
        counterValue = 0xFFFFFFFF;

        st = cpssExMxPmBrgSecurityBreachPortVlanCntrSet(dev, counterValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterValue);
    }

    counterValue = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachPortVlanCntrSet(dev, counterValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachPortVlanCntrSet(dev, counterValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSecurityBreachPortVlanCntrGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *counterValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSecurityBreachPortVlanCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null counterValuePtr. 
    Expected: GT_OK.
    1.2. Call with counterValuePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      counterValue = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL counterValuePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanCntrGet(dev, &counterValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null counterValuePtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSecurityBreachPortVlanCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterValuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSecurityBreachPortVlanCntrGet(dev, &counterValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSecurityBreachPortVlanCntrGet(dev, &counterValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgSecurityBreach suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgSecurityBreach)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachEventDropModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachEventDropModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachMsgGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachGlobalDropCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachGlobalDropCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachPortVlanCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSecurityBreachPortVlanCntrGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgSecurityBreach)

