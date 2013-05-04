/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgGenUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgGen, that provides
*       CPSS ExMxPm Bridge Generic APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgGen.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_GEN_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define BRG_GEN_INVALID_ENUM_CNS    0x5AAAAAA5

/* Internal functions forward declaration */
static GT_STATUS prvUtfCoreClockGet(IN GT_U8 dev, OUT GT_U32* coreClockPtr);


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropInvalidSaEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropInvalidSaEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenDropInvalidSaEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
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

        st = cpssExMxPmBrgGenDropInvalidSaEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenDropInvalidSaEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenDropInvalidSaEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropInvalidSaEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgGenDropInvalidSaEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenDropInvalidSaEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenDropInvalidSaEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropInvalidSaEnableGet: %d", dev);

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
        st = cpssExMxPmBrgGenDropInvalidSaEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropInvalidSaEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropInvalidSaEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropInvalidSaEnableGet)
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
        st = cpssExMxPmBrgGenDropInvalidSaEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenDropInvalidSaEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgGenDropInvalidSaEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropInvalidSaEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_GEN_VALID_VIRT_PORT_CNS;

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

            st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet: %d, %d", dev, port);

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
            st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet)
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
    GT_U8       port = BRG_GEN_VALID_VIRT_PORT_CNS;

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
            st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_GEN_VALID_VIRT_PORT_CNS;

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

            st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet: %d, %d", dev, port);

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
            st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet)
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
    GT_U8       port = BRG_GEN_VALID_VIRT_PORT_CNS;

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
            st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropIpMcEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropIpMcEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenDropIpMcEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
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

        st = cpssExMxPmBrgGenDropIpMcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenDropIpMcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenDropIpMcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropIpMcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgGenDropIpMcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenDropIpMcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenDropIpMcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropIpMcEnableGet: %d", dev);

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
        st = cpssExMxPmBrgGenDropIpMcEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropIpMcEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropIpMcEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropIpMcEnableGet)
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
        st = cpssExMxPmBrgGenDropIpMcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenDropIpMcEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgGenDropIpMcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropIpMcEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropIpMcDropModeSet
(
    IN GT_U8                    devNum,
    IN CPSS_DROP_MODE_TYPE_ENT  dropMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropIpMcDropModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dropMode [CPSS_DROP_MODE_SOFT_E /
                             CPSS_DROP_MODE_HARD_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenDropIpMcDropModeGet with non-NULL dropModePtr.
    Expected: GT_OK and the same dropMode.
    1.3. Call with out of range dropMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DROP_MODE_TYPE_ENT  dropMode    = CPSS_DROP_MODE_SOFT_E;
    CPSS_DROP_MODE_TYPE_ENT  dropModeGet = CPSS_DROP_MODE_SOFT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with dropMode [CPSS_DROP_MODE_SOFT_E /
                                              CPSS_DROP_MODE_HARD_E].
            Expected: GT_OK.
        */

        /* Call with dropMode [CPSS_DROP_MODE_SOFT_E] */
        dropMode = CPSS_DROP_MODE_SOFT_E;

        st = cpssExMxPmBrgGenDropIpMcDropModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropMode);

        /*
            1.2. Call cpssExMxPmBrgGenDropIpMcDropModeGet with non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgGenDropIpMcDropModeGet(dev, &dropModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropIpMcDropModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /* Call with dropMode [CPSS_DROP_MODE_HARD_E] */
        dropMode = CPSS_DROP_MODE_HARD_E;

        st = cpssExMxPmBrgGenDropIpMcDropModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropMode);

        /*
            1.2. Call cpssExMxPmBrgGenDropIpMcDropModeGet with non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgGenDropIpMcDropModeGet(dev, &dropModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropIpMcDropModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /*
            1.3. Call function with out of range dropMode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        dropMode = BRG_GEN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgGenDropIpMcDropModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, dropMode);
    }

    dropMode = CPSS_DROP_MODE_SOFT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenDropIpMcDropModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropIpMcDropModeSet(dev, dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropIpMcDropModeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_DROP_MODE_TYPE_ENT     *dropModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropIpMcDropModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null dropModePtr.
    Expected: GT_OK.
    1.2. Call with dropModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DROP_MODE_TYPE_ENT  dropMode = CPSS_DROP_MODE_SOFT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL dropModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgGenDropIpMcDropModeGet(dev, &dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null dropModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenDropIpMcDropModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenDropIpMcDropModeGet(dev, &dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropIpMcDropModeGet(dev, &dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropNonIpMcEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropNonIpMcEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenDropNonIpMcEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
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

        st = cpssExMxPmBrgGenDropNonIpMcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenDropNonIpMcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenDropNonIpMcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropNonIpMcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgGenDropNonIpMcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenDropNonIpMcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenDropNonIpMcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropNonIpMcEnableGet: %d", dev);

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
        st = cpssExMxPmBrgGenDropNonIpMcEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropNonIpMcEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropNonIpMcEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropNonIpMcEnableGet)
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
        st = cpssExMxPmBrgGenDropNonIpMcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenDropNonIpMcEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgGenDropNonIpMcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropNonIpMcEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropNonIpMcDropModeSet
(
    IN GT_U8                    devNum,
    IN CPSS_DROP_MODE_TYPE_ENT  dropMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropNonIpMcDropModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dropMode [CPSS_DROP_MODE_SOFT_E /
                             CPSS_DROP_MODE_HARD_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenDropNonIpMcDropModeGet with non-NULL dropModePtr.
    Expected: GT_OK and the same dropMode.
    1.3. Call with out of range dropMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DROP_MODE_TYPE_ENT  dropMode    = CPSS_DROP_MODE_SOFT_E;
    CPSS_DROP_MODE_TYPE_ENT  dropModeGet = CPSS_DROP_MODE_SOFT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with dropMode [CPSS_DROP_MODE_SOFT_E /
                                              CPSS_DROP_MODE_HARD_E].
            Expected: GT_OK.
        */

        /* Call with dropMode [CPSS_DROP_MODE_SOFT_E] */
        dropMode = CPSS_DROP_MODE_SOFT_E;

        st = cpssExMxPmBrgGenDropNonIpMcDropModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropMode);

        /*
            1.2. Call cpssExMxPmBrgGenDropNonIpMcDropModeGet with non-NULL dropTypePtr.
            Expected: GT_OK and the same dropType.
        */
        st = cpssExMxPmBrgGenDropNonIpMcDropModeGet(dev, &dropModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropNonIpMcDropModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /* Call with dropMode [CPSS_DROP_MODE_HARD_E] */
        dropMode = CPSS_DROP_MODE_HARD_E;

        st = cpssExMxPmBrgGenDropNonIpMcDropModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropMode);

        /*
            1.2. Call cpssExMxPmBrgGenDropNonIpMcDropModeGet with non-NULL dropTypePtr.
            Expected: GT_OK and the same dropType.
        */
        st = cpssExMxPmBrgGenDropNonIpMcDropModeGet(dev, &dropModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDropNonIpMcDropModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /*
            1.3. Call function with out of range dropMode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        dropMode = BRG_GEN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgGenDropNonIpMcDropModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, dropMode);
    }

    dropMode = CPSS_DROP_MODE_SOFT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenDropNonIpMcDropModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropNonIpMcDropModeSet(dev, dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDropNonIpMcDropModeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_DROP_MODE_TYPE_ENT     *dropModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDropNonIpMcDropModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null dropModePtr.
    Expected: GT_OK.
    1.2. Call with dropModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DROP_MODE_TYPE_ENT  dropMode = CPSS_DROP_MODE_SOFT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL dropModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgGenDropNonIpMcDropModeGet(dev, &dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null dropModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenDropNonIpMcDropModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenDropNonIpMcDropModeGet(dev, &dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDropNonIpMcDropModeGet(dev, &dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT      protocolStack,
    IN  GT_BOOL                         enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet with the same protocolStack and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT protocol  = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable    = GT_FALSE;
    GT_BOOL                    enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E /
                                          CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV4_E;
        enable   = GT_FALSE;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocol, enable);

        /*
            1.2. Call cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet with the same protocolStack and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet: %d, %d", dev, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call function with protocolStack [CPSS_IP_PROTOCOL_IPV6_E], enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocol, enable);

        /*
            1.2. Call cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet with the same protocolStack and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet: %d, %d", dev, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E], enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocol, enable);

        /* Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV4_E;
        enable   = GT_TRUE;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocol, enable);

        /*
            1.2. Call cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet with the same protocolStack and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet: %d, %d", dev, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call function with protocolStack [CPSS_IP_PROTOCOL_IPV6_E], enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocol, enable);

        /*
            1.2. Call cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet with the same protocolStack and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet: %d, %d", dev, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E], enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocol, enable);

        /*
            1.3. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM
        */
        protocol = BRG_GEN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocol);
    }

    protocol = CPSS_IP_PROTOCOL_IPV4_E;
    enable   = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(dev, protocol, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT      protocolStack,
    OUT GT_BOOL                         *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E]
                   and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT protocol = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable   = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E /
                                          CPSS_IP_PROTOCOL_IPV4V6_E]
                           and non-NULL enablePtr.
            Expected: GT_OK.
        */

        /* Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocol = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocol);

        /* Call function with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocol = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocol);

        /*
            1.3. Call with out of range protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocol);

        /*
            1.4. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM
        */
        protocol = BRG_GEN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocol);

        protocol = CPSS_IP_PROTOCOL_IPV6_E;

        /*
            1.5. Call with enablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    protocol = CPSS_IP_PROTOCOL_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(dev, protocol, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenRateLimitGlobalCfgSet
(
    IN GT_U8                                devNum,
    IN CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC   *brgRateLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenRateLimitGlobalCfgSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with brgRateLimitPtr{rateLimitModeL1[CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_EXCL_E /
                                                   CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_INCL_E],
                                   dropMode [CPSS_DROP_MODE_SOFT_E /
                                             CPSS_DROP_MODE_HARD_E],
                                   rMode [CPSS_RATE_LIMIT_PCKT_BASED_E /
                                          CPSS_RATE_LIMIT_BYTE_BASED_E],
                                   win10Mbps [0 / 10000],
                                   win100Mbps [0 / 1000],
                                   win1000Mbps [0 / 100],
                                   win10Gbps [0 / 100]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenRateLimitGlobalCfgGet with non-NULL brgRateLimitPtr.
    Expected: GT_OK and the same brgRateLimitPtr.
    1.3. Call with out of range brgRateLimitPtr->rateLimitModeL1 [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range brgRateLimitPtr->dropMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range brgRateLimitPtr->rMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range brgRateLimitPtr->win10Mbps [(4095 + 1) * 256 * 200 / coreClock]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range brgRateLimitPtr->win100Mbps [(511 + 1) * 256 * 200 / coreClock]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with out of range brgRateLimitPtr->win1000Mbps [(63 + 1) * 256 * 200 / coreClock]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with out of range brgRateLimitPtr->win10Gbps [(4095 + 1) * 25.6 * 200 / coreClock]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with brgRateLimitPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC   brgRateLimit;
    CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC   brgRateLimitGet;
    GT_U32                               granularity = 0;
    GT_U32                               coreClock = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting core clock */
        st = prvUtfCoreClockGet(dev, &coreClock);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCoreClockGet: %d", dev);
        granularity = (256 * PRV_CPSS_EXMXPM_REFERENCE_CORE_CLOCK_CNS) / PRV_CPSS_PP_MAC(dev)->coreClock;

        /*
            1.1. Call with brgRateLimitPtr{rateLimitModeL1[CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_EXCL_E /
                                                           CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_INCL_E],
                                           dropMode [CPSS_DROP_MODE_SOFT_E /
                                                     CPSS_DROP_MODE_HARD_E],
                                           rMode [CPSS_RATE_LIMIT_PCKT_BASED_E /
                                                  CPSS_RATE_LIMIT_BYTE_BASED_E],
                                           win10Mbps [0 / 10000],
                                           win100Mbps [0 / 1000],
                                           win1000Mbps [0 / 100],
                                           win10Gbps [0 / 100]}.
            Expected: GT_OK.
        */

        /* Call with rateLimitModeL1 [CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_EXCL_E] */
        brgRateLimit.rateLimitModeL1 = CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_EXCL_E;
        brgRateLimit.dropMode        = CPSS_DROP_MODE_SOFT_E;
        brgRateLimit.rMode           = CPSS_RATE_LIMIT_PCKT_BASED_E;

        brgRateLimit.win10Mbps   = 0;
        brgRateLimit.win100Mbps  = 0;
        brgRateLimit.win1000Mbps = 0;
        brgRateLimit.win10Gbps   = 0;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgGenRateLimitGlobalCfgGet with non-NULL brgRateLimitPtr.
            Expected: GT_OK and the same brgRateLimitPtr.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalCfgGet(dev, &brgRateLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenRateLimitGlobalCfgGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.rateLimitModeL1, brgRateLimitGet.rateLimitModeL1,
                       "get another brgRateLimitPtr->rateLimitModeL1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.dropMode, brgRateLimitGet.dropMode,
                       "get another brgRateLimitPtr->dropMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.rMode, brgRateLimitGet.rMode,
                       "get another brgRateLimitPtr->rMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.win10Mbps, brgRateLimitGet.win10Mbps,
                       "get another brgRateLimitPtr->win10Mbps than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.win100Mbps, brgRateLimitGet.win100Mbps,
                       "get another brgRateLimitPtr->win100Mbps than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.win1000Mbps, brgRateLimitGet.win1000Mbps,
                       "get another brgRateLimitPtr->win1000Mbps than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.win10Gbps, brgRateLimitGet.win10Gbps,
                       "get another brgRateLimitPtr->win10Gbps than was set: %d", dev);

        /* Call with rateLimitModeL1 [CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_INCL_E] */
        brgRateLimit.rateLimitModeL1 = CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_INCL_E;
        brgRateLimit.dropMode        = CPSS_DROP_MODE_HARD_E;
        brgRateLimit.rMode           = CPSS_RATE_LIMIT_BYTE_BASED_E;

        brgRateLimit.win10Mbps   = 256 * granularity;
        brgRateLimit.win100Mbps  = 256 * granularity;
        brgRateLimit.win1000Mbps = 63  * granularity;
        brgRateLimit.win10Gbps   = 256 * granularity;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgGenRateLimitGlobalCfgGet with non-NULL brgRateLimitPtr.
            Expected: GT_OK and the same brgRateLimitPtr.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalCfgGet(dev, &brgRateLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenRateLimitGlobalCfgGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.rateLimitModeL1, brgRateLimitGet.rateLimitModeL1,
                       "get another brgRateLimitPtr->rateLimitModeL1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.dropMode, brgRateLimitGet.dropMode,
                       "get another brgRateLimitPtr->dropMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.rMode, brgRateLimitGet.rMode,
                       "get another brgRateLimitPtr->rMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.win10Mbps, brgRateLimitGet.win10Mbps,
                       "get another brgRateLimitPtr->win10Mbps than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.win100Mbps, brgRateLimitGet.win100Mbps,
                       "get another brgRateLimitPtr->win100Mbps than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.win1000Mbps, brgRateLimitGet.win1000Mbps,
                       "get another brgRateLimitPtr->win1000Mbps than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(brgRateLimit.win10Gbps, brgRateLimitGet.win10Gbps,
                       "get another brgRateLimitPtr->win10Gbps than was set: %d", dev);

        /*
            1.3. Call with out of range brgRateLimitPtr->rateLimitModeL1 [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        brgRateLimit.rateLimitModeL1 = BRG_GEN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, brgRateLimitPtr->rateLimitModeL1 = %d",
                                     dev, brgRateLimit.rateLimitModeL1);

        brgRateLimit.rateLimitModeL1 = CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_EXCL_E;

        /*
            1.4. Call with out of range brgRateLimitPtr->dropMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        brgRateLimit.dropMode = BRG_GEN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, brgRateLimitPtr->dropMode = %d",
                                     dev, brgRateLimit.dropMode);

        brgRateLimit.dropMode = CPSS_DROP_MODE_SOFT_E;

        /*
            1.5. Call with out of range brgRateLimitPtr->rMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        brgRateLimit.rMode = BRG_GEN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, brgRateLimitPtr->rMode = %d",
                                     dev, brgRateLimit.rMode);

        brgRateLimit.rMode = CPSS_RATE_LIMIT_PCKT_BASED_E;

        /*
            1.6. Call with out of range brgRateLimitPtr->win10Mbps [(4095 + 1) * 256 * 200 / coreClock]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        brgRateLimit.win10Mbps = (4095 + 1) * 256 * PRV_CPSS_EXMXPM_REFERENCE_CORE_CLOCK_CNS / coreClock;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, brgRateLimitPtr->win10Mbps = %d",
                                         dev, brgRateLimit.win10Mbps);

        brgRateLimit.win10Mbps = 10000;

        /*
            1.7. Call with out of range brgRateLimitPtr->win100Mbps [(511 + 1) * 256 * 200 / coreClock]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        brgRateLimit.win100Mbps = (511 + 1) * 256 * PRV_CPSS_EXMXPM_REFERENCE_CORE_CLOCK_CNS / coreClock;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, brgRateLimitPtr->win100Mbps = %d",
                                         dev, brgRateLimit.win100Mbps);

        brgRateLimit.win100Mbps = 1000;

        /*
            1.8. Call with out of range brgRateLimitPtr->win1000Mbps [(63 + 1) * 256 * 200 / coreClock]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        brgRateLimit.win1000Mbps = (63 + 1) * 256 * PRV_CPSS_EXMXPM_REFERENCE_CORE_CLOCK_CNS / coreClock;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, brgRateLimitPtr->win1000Mbps = %d",
                                         dev, brgRateLimit.win1000Mbps);

        brgRateLimit.win1000Mbps = 100;

        /*
            1.9. Call with out of range brgRateLimitPtr->win10Gbps [(4095 + 1) * 25.6 * 200 / coreClock]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        brgRateLimit.win10Gbps = (4095 + 2) * 256 * PRV_CPSS_EXMXPM_REFERENCE_CORE_CLOCK_CNS / coreClock;

        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, brgRateLimitPtr->win10Gbps = %d",
                                         dev, brgRateLimit.win10Gbps);

        brgRateLimit.win10Gbps = 100;

        /*
            1.10. Call with brgRateLimitPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, brgRateLimitPtr = NULL", dev);
    }

    brgRateLimit.rateLimitModeL1 = CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_EXCL_E;
    brgRateLimit.dropMode        = CPSS_DROP_MODE_SOFT_E;
    brgRateLimit.rMode           = CPSS_RATE_LIMIT_PCKT_BASED_E;

    brgRateLimit.win10Mbps   = 0;
    brgRateLimit.win100Mbps  = 0;
    brgRateLimit.win1000Mbps = 0;
    brgRateLimit.win10Gbps   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenRateLimitGlobalCfgGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC  *brgRateLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenRateLimitGlobalCfgGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null brgRateLimitPtr.
    Expected: GT_OK.
    1.2. Call with brgRateLimitPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC  brgRateLimit;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null brgRateLimitPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalCfgGet(dev, &brgRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null brgRateLimitPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalCfgGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, brgRateLimitPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenRateLimitGlobalCfgGet(dev, &brgRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenRateLimitGlobalCfgGet(dev, &brgRateLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenRateLimitGlobalDropCntrSet
(
    IN GT_U8    devNum,
    IN GT_U64   *dropCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenRateLimitGlobalDropCntrSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dropCntrPtr->l[0] [0 / 0xFFFFFFFF],
                   dropCntrPtr->l[1] [0 / 0xFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenRateLimitGlobalDropCntrGet with non-NULL dropCntrPtr.
    Expected: GT_OK and the same dropCntrPtr.
    1.3. Call with dropCntrPtr->l[0] [0xFFFFFFFF],
                   dropCntrPtr->l[1] [0xFFFFFFFF].
    Expected: GT_OK.
    1.4. Call with dropCntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U64      dropCntr;
    GT_U64      dropCntrGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with dropCntrPtr->l[0] [0 / 0xFFFFFFFF],
                           dropCntrPtr->l[1] [0 / 0xFF].
            Expected: GT_OK.
        */

        /* Call with dropCntrPtr->l[0] [0] */
        dropCntr.l[0] = 0;
        dropCntr.l[1] = 0;

        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrSet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgGenRateLimitGlobalDropCntrGet with non-NULL dropCntrPtr.
            Expected: GT_OK and the same dropCntrPtr.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrGet(dev, &dropCntrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenRateLimitGlobalDropCntrGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr.l[0], dropCntrGet.l[0],
                       "get another dropCntrPtr->l[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr.l[1], dropCntrGet.l[1],
                       "get another dropCntrPtr->l[1] than was set: %d", dev);

        /* Call with dropCntrPtr->l[0] [0xFFFFFFFF] */
        dropCntr.l[0] = 0xFFFFFFFF;
        dropCntr.l[1] = 0xFF;

        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrSet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgGenRateLimitGlobalDropCntrGet with non-NULL dropCntrPtr.
            Expected: GT_OK and the same dropCntrPtr.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrGet(dev, &dropCntrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenRateLimitGlobalDropCntrGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr.l[0], dropCntrGet.l[0],
                       "get another dropCntrPtr->l[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr.l[1], dropCntrGet.l[1],
                       "get another dropCntrPtr->l[1] than was set: %d", dev);

        /*
            1.3. Call with dropCntrPtr->l[0] [0xFFFFFFFF],
                           dropCntrPtr->l[1] [0xFFFFFFFF].
            Expected: GT_OK.
        */
        dropCntr.l[0] = 0xFFFFFFFF;
        dropCntr.l[1] = 0xFFFFFFFF;

        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrSet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.4. Call with dropCntrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropCntrPtr = NULL", dev);
    }

    dropCntr.l[0] = 0;
    dropCntr.l[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrSet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenRateLimitGlobalDropCntrSet(dev, &dropCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenRateLimitGlobalDropCntrGet
(
    IN  GT_U8   devNum,
    OUT GT_U64  *dropCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenRateLimitGlobalDropCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null dropCntrPtr.
    Expected: GT_OK.
    1.2. Call with dropCntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U64      dropCntr;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null dropCntrPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrGet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null dropCntrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropCntrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenRateLimitGlobalDropCntrGet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenRateLimitGlobalDropCntrGet(dev, &dropCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenPortRateLimitSet
(
    IN GT_U8                                    devNum,
    IN GT_U8                                    port,
    IN CPSS_EXMXPM_BRG_GEN_PORT_RATE_LIMIT_STC  *portCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenPortRateLimitSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with portCfgPtr {enableBc [GT_FALSE and GT_TRUE],
                                 enableMc [GT_FALSE and GT_TRUE],
                                 enableMcUnreg [GT_FALSE and GT_TRUE],
                                 enableUcUnk [GT_FALSE and GT_TRUE],
                                 enableUcKnown [GT_FALSE and GT_TRUE],
                                 enableTcpSyn [GT_FALSE and GT_TRUE],
                     rateLimit [100 / 1000]}.
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgGenPortRateLimitGet with non-NULL portCfgPtr.
    Expected: GT_OK and the same portCfgPtr.
    1.1.3. Call with out of range portCfgPtr->rateLimit [0x10000]
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call with portCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_GEN_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_BRG_GEN_PORT_RATE_LIMIT_STC  portCfg;
    CPSS_EXMXPM_BRG_GEN_PORT_RATE_LIMIT_STC  portCfgGet;


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
                1.1.1. Call with portCfgPtr {enableBc [GT_FALSE and GT_TRUE],
                                             enableMc [GT_FALSE and GT_TRUE],
                                             enableMcUnreg [GT_FALSE and GT_TRUE],
                                             enableUcUnk [GT_FALSE and GT_TRUE],
                                             enableUcKnown [GT_FALSE and GT_TRUE],
                                             enableTcpSyn [GT_FALSE and GT_TRUE],
                                 rateLimit [100 / 1000]}.
                Expected: GT_OK.
            */

            /* Call with rateLimit [100] */
            portCfg.rateLimit = 100;

            portCfg.enableBc      = GT_FALSE;
            portCfg.enableMc      = GT_FALSE;
            portCfg.enableMcUnreg = GT_FALSE;
            portCfg.enableUcUnk   = GT_FALSE;
            portCfg.enableUcKnown = GT_FALSE;
            portCfg.enableTcpSyn  = GT_FALSE;

            st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssExMxPmBrgGenPortRateLimitGet with non-NULL portCfgPtr.
                Expected: GT_OK and the same portCfgPtr.
            */
            st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portCfgGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenPortRateLimitGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableBc, portCfgGet.enableBc,
                       "get another portCfgPtr->enableBc than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableMc, portCfgGet.enableMc,
                       "get another portCfgPtr->enableMc than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableMcUnreg, portCfgGet.enableMcUnreg,
                       "get another portCfgPtr->enableMcUnreg than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableUcUnk, portCfgGet.enableUcUnk,
                       "get another portCfgPtr->enableUcUnk than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableUcKnown, portCfgGet.enableUcKnown,
                       "get another portCfgPtr->enableUcKnown than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableTcpSyn, portCfgGet.enableTcpSyn,
                       "get another portCfgPtr->enableTcpSyn than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.rateLimit, portCfgGet.rateLimit,
                       "get another portCfgPtr->rateLimit than was set: %d, %d", dev, port);

            /* Call with rateLimit [1000] */
            portCfg.rateLimit = 1000;

            portCfg.enableBc      = GT_TRUE;
            portCfg.enableMc      = GT_TRUE;
            portCfg.enableMcUnreg = GT_TRUE;
            portCfg.enableUcUnk   = GT_TRUE;
            portCfg.enableUcKnown = GT_TRUE;
            portCfg.enableTcpSyn  = GT_TRUE;

            st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssExMxPmBrgGenPortRateLimitGet with non-NULL portCfgPtr.
                Expected: GT_OK and the same portCfgPtr.
            */
            st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portCfgGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenPortRateLimitGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableBc, portCfgGet.enableBc,
                       "get another portCfgPtr->enableBc than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableMc, portCfgGet.enableMc,
                       "get another portCfgPtr->enableMc than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableMcUnreg, portCfgGet.enableMcUnreg,
                       "get another portCfgPtr->enableMcUnreg than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableUcUnk, portCfgGet.enableUcUnk,
                       "get another portCfgPtr->enableUcUnk than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableUcKnown, portCfgGet.enableUcKnown,
                       "get another portCfgPtr->enableUcKnown than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.enableTcpSyn, portCfgGet.enableTcpSyn,
                       "get another portCfgPtr->enableTcpSyn than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portCfg.rateLimit, portCfgGet.rateLimit,
                       "get another portCfgPtr->rateLimit than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range portCfgPtr->rateLimit [0x10000]
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            portCfg.rateLimit = 0x10000;

            st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfg);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, portCfgPtr->rateLimit = %d",
                                             dev, port, portCfg.rateLimit);

            portCfg.rateLimit = 100;

            /*
                1.1.4. Call with portCfgPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portCfgPtr = NULL", dev, port);
        }

        portCfg.rateLimit = 1000;

        portCfg.enableBc      = GT_TRUE;
        portCfg.enableMc      = GT_TRUE;
        portCfg.enableMcUnreg = GT_TRUE;
        portCfg.enableUcUnk   = GT_TRUE;
        portCfg.enableUcKnown = GT_TRUE;
        portCfg.enableTcpSyn  = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    portCfg.rateLimit = 1000;

    portCfg.enableBc      = GT_TRUE;
    portCfg.enableMc      = GT_TRUE;
    portCfg.enableUcUnk   = GT_TRUE;
    portCfg.enableUcUnk   = GT_TRUE;
    portCfg.enableUcKnown = GT_TRUE;
    portCfg.enableTcpSyn  = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenPortRateLimitGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U8                                       port,
    OUT CPSS_EXMXPM_BRG_GEN_PORT_RATE_LIMIT_STC     *portCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenPortRateLimitGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with non-null portCfgPtr.
    Expected: GT_OK.
    1.1.2. Call with portCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_GEN_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_BRG_GEN_PORT_RATE_LIMIT_STC  portCfg;


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
                1.1.1. Call with non-null portCfgPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with portCfgPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portCfgPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_GEN_VALID_VIRT_PORT_CNS;

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

            st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet: %d, %d", dev, port);

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
            st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_GEN_VALID_VIRT_PORT_CNS;

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
            st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
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

        st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet: %d", dev);

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
        st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet)
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
        st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenBpduTrapEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenBpduTrapEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgGenBpduTrapEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
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

        st = cpssExMxPmBrgGenBpduTrapEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenBpduTrapEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenBpduTrapEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenBpduTrapEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgGenBpduTrapEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgGenBpduTrapEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgGenBpduTrapEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgGenBpduTrapEnableGet: %d", dev);

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
        st = cpssExMxPmBrgGenBpduTrapEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenBpduTrapEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgGenBpduTrapEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgGenBpduTrapEnableGet)
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
        st = cpssExMxPmBrgGenBpduTrapEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgGenBpduTrapEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgGenBpduTrapEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgGenBpduTrapEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgGen suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgGen)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropInvalidSaEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropInvalidSaEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropIpMcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropIpMcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropIpMcDropModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropIpMcDropModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropNonIpMcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropNonIpMcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropNonIpMcDropModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDropNonIpMcDropModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenRateLimitGlobalCfgSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenRateLimitGlobalCfgGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenRateLimitGlobalDropCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenRateLimitGlobalDropCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenPortRateLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenPortRateLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenBpduTrapEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgGenBpduTrapEnableGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgGen)


/*******************************************************************************
* prvUtfCoreClockGet
*
* DESCRIPTION:
*       This routine returns core clock per device.
*
* INPUTS:
*       dev - device id
*
* OUTPUTS:
*       coreClockPtr - (pointer to) core clock
*
*       GT_OK        - Get core clock was OK
*       GT_BAD_PARAM - Invalid device id
*       GT_BAD_PTR   - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfCoreClockGet
(
    IN  GT_U8       dev,
    OUT GT_U32      *coreClockPtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(coreClockPtr);

    /* check if dev active and from ExMxPm family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    *coreClockPtr = PRV_CPSS_PP_MAC(dev)->coreClock;

    return(GT_OK);
}

