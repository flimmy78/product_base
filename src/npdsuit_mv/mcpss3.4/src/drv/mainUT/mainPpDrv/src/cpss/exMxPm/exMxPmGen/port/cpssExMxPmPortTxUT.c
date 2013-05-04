/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmPortTxUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmPortTx, that provides
*       Egress Bandwidth Management
*       APIs for configuring the Physical Port Tx Traffic Class Queues.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortTx.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define PORT_TX_VALID_PHY_PORT_CNS  0

/* Invalid enum */
#define PORT_TX_INVALID_ENUM_CNS    0x5AAAAAA5

/* the number of queus groups, each related to its HW physical port - PRV_CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS*/ 
#define PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS    28


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueBlockEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueBlockEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxQueueBlockEnableGet with non-NULL enablePtr.
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

        st = cpssExMxPmPortTxQueueBlockEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPortTxQueueBlockEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPortTxQueueBlockEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueBlockEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmPortTxQueueBlockEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPortTxQueueBlockEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPortTxQueueBlockEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueBlockEnableGet: %d", dev);

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
        st = cpssExMxPmPortTxQueueBlockEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueBlockEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueBlockEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueBlockEnableGet)
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
        st = cpssExMxPmPortTxQueueBlockEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxQueueBlockEnableGet(dev, NULL);
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
        st = cpssExMxPmPortTxQueueBlockEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueBlockEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U32    tc,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueingEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE]
                     and tc [0 / 4].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortTxQueueingEnableGet with same tc and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.1.3. Call with enable [GT_FALSE and GT_TRUE]
                     and out of range tc [CPSS_TC_RANGE_CNS] = 8.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      tc        = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_FALSE and GT_TRUE]
                                 and tc [0 / 4].
                Expected: GT_OK.
            */

            /* Call with tc [0], enable [GT_FALSE] */
            tc     = 0;
            enable = GT_FALSE;

            st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueingEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueingEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [4], enable [GT_FALSE] */
            tc = 4;

            st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueingEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueingEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [0], enable [GT_TRUE] */
            tc     = 0;
            enable = GT_TRUE;

            st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueingEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueingEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [4], enable [GT_TRUE] */
            tc = 4;

            st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueingEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueingEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with enable [GT_FALSE and GT_TRUE]
                                 and out of range tc [CPSS_TC_RANGE_CNS] = 8.
                Expected: GT_BAD_PARAM.
            */
            tc = CPSS_TC_RANGE_CNS;

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);
        }

        tc     = 0;
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc     = 0;
    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueingEnableSet(dev, port, tc, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueingEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    IN   GT_U32    tc,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueingEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 4]
                     and non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8
                     and non-NULL enablePtr.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with tc[7]
                     and enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      tc     = 0;
    GT_BOOL     enable = GT_FALSE;

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
                1.1.1. Call with tc [0 / 4]
                                 and non-NULL enablePtr.
                Expected: GT_OK.
            */

            /* Call with tc [0] */
            tc = 0;

            st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /* Call with tc [4] */
            tc = 4;

            st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /*
                1.1.2. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8
                                 and non-NULL enablePtr.
                Expected: GT_BAD_PARAM.
            */
            tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            tc = 7;

            /*
                1.1.3. Call with tc[7]
                                 and enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        tc = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueingEnableGet(dev, port, tc, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueTxEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U32    tc,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueTxEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE]
                     and tc [0 / 4].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortTxQueueTxEnableGet with same tc and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.1.3. Call with enable [GT_FALSE and GT_TRUE]
                     and out of range tc [CPSS_TC_RANGE_CNS] = 8.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      tc        = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_FALSE and GT_TRUE]
                                 and tc [0 / 4].
                Expected: GT_OK.
            */

            /* Call with tc [0], enable [GT_FALSE] */
            tc     = 0;
            enable = GT_FALSE;

            st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueTxEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueTxEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [4], enable [GT_FALSE] */
            tc = 4;

            st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueTxEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueTxEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [0], enable [GT_TRUE] */
            tc     = 0;
            enable = GT_TRUE;

            st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueTxEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueTxEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [4], enable [GT_TRUE] */
            tc = 4;

            st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueTxEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueTxEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with enable [GT_FALSE and GT_TRUE]
                                 and out of range tc [CPSS_TC_RANGE_CNS] = 8.
                Expected: GT_BAD_PARAM.
            */
            tc = CPSS_TC_RANGE_CNS;

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);
        }

        tc     = 0;
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc     = 0;
    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueTxEnableSet(dev, port, tc, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueTxEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U32    tc,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueTxEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 4]
                     and non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range tc [CPSS_TC_RANGE_CNS]
                     and non-NULL enablePtr.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with tc[7]
                     and enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      tc     = 0;
    GT_BOOL     enable = GT_FALSE;

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
                1.1.1. Call with tc [0 / 4]
                                 and non-NULL enablePtr.
                Expected: GT_OK.
            */

            /* Call with tc [0] */
            tc = 0;

            st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /* Call with tc [4] */
            tc = 4;

            st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /*
                1.1.2. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8
                                 and non-NULL enablePtr.
                Expected: GT_BAD_PARAM.
            */
            tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            tc = 7;

            /*
                1.1.3. Call with tc[7]
                                 and enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        tc = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueTxEnableGet(dev, port, tc, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueuesFlush
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueuesFlush)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortTxQueuesFlush(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueuesFlush(dev, port);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueuesFlush(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueuesFlush(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueuesFlush(dev, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueuesFlush(dev, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxShaperEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxShaperEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortTxShaperEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortTxShaperEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxShaperEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxShaperEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxShaperEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortTxShaperEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxShaperEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxShaperEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxShaperEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxShaperEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxShaperEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxShaperEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxShaperEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxShaperEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxShaperEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxShaperEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;

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
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortTxShaperEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxShaperEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxShaperEnableGet(dev, port, &enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxShaperEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxShaperEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxShaperEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxShaperEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxShaperProfileSet
(
    IN    GT_U8     devNum,
    IN    GT_U8     portNum,
    IN    GT_U32    bucketSize,
    INOUT GT_U32    *maxRatePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxShaperProfileSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with bucketSize [0 / 1000]
                     and non-null maxRatePtr [1 / 100].
    Expected: GT_OK and non-null maxRatePtr.
    1.1.2. Call cpssExMxPmPortTxShaperProfileGet with non-NULL bucketSizePtr and maxRatePtr.
    Expected: GT_OK and the same bucketSize and maxRatePtr.
    1.1.3. Call with out of range bucketSize [0x1000]
                     and non-null maxRatePtr.
    Expected: NOT GT_OK.
    1.1.4. Call with bucketSize [0]
                     and maxRatePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      bucketSize    = 0;
    GT_U32      bucketSizeGet = 0;
    GT_U32      maxRate       = 0;
    GT_U32      maxRateGet    = 0;

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
                1.1.1. Call with bucketSize [0 / 1000]
                                 and non-null maxRatePtr [1 / 100].
                Expected: GT_OK and non-null maxRatePtr.
            */

            /* Call with bucketSize [0], maxRatePtr [1] */
            bucketSize = 0;
            maxRate    = 1;

            st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, &maxRate);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, bucketSize);

            /*
                1.1.2. Call cpssExMxPmPortTxShaperProfileGet with non-NULL bucketSizePtr and maxRatePtr.
                Expected: GT_OK and the same bucketSize and maxRatePtr.
            */
            st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSizeGet, &maxRateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxShaperProfileGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(bucketSize, bucketSizeGet,
                       "get another bucketSize than was set: %d, %d", dev, port);

            /* Call with bucketSize [1000], maxRatePtr [100] */
            bucketSize = 1000;
            maxRate    = 100;

            st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, &maxRate);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, bucketSize);

            /*
                1.1.2. Call cpssExMxPmPortTxShaperProfileGet with non-NULL bucketSizePtr and maxRatePtr.
                Expected: GT_OK and the same bucketSize and maxRatePtr.
            */
            st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSizeGet, &maxRateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxShaperProfileGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(bucketSize, bucketSizeGet,
                       "get another bucketSize than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range bucketSize [0x1000]
                                 and non-null maxRatePtr.
                Expected: NOT GT_OK.
            */
            bucketSize = 0x1000;

            st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, &maxRate);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, bucketSize);

            bucketSize = 0;

            /*
                1.1.4. Call with bucketSize [0]
                                 and maxRatePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, maxRatePtr = NULL", dev, port);
        }

        bucketSize = 0;
        maxRate    = 1;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, &maxRate);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    bucketSize = 0;
    maxRate    = 1;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, &maxRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxShaperProfileSet(dev, port, bucketSize, &maxRate);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxShaperProfileGet
(
    IN    GT_U8      devNum,
    IN    GT_U8      portNum,
    OUT   GT_U32     *bucketSizePtr,
    OUT   GT_U32     *maxRatePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxShaperProfileGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null bucketSizePtr and non-null maxRatePtr.
    Expected: GT_OK.
    1.1.2. Call with bucketSizePtr [NULL] and non-null maxRatePtr.
    Expected: GT_BAD_PTR.
    1.1.3. Call with non-null bucketSizePtr and maxRatePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      bucketSize = 0;
    GT_U32      maxRate    = 0;

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
                1.1.1. Call with non-null bucketSizePtr and non-null maxRatePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSize, &maxRate);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with bucketSizePtr [NULL] and non-null maxRatePtr.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxShaperProfileGet(dev, port, NULL, &maxRate);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, bucketSizePtr = NULL", dev, port);

            /*
                1.1.3. Call with non-null bucketSizePtr and maxRatePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSize, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, maxRatePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSize, &maxRate);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSize, &maxRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxShaperProfileGet(dev, port, &bucketSize, &maxRate);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueShaperEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U32    tc,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueShaperEnableSet)
{
/*
    ITERATE_DEVICES_PHY_ PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE]
                     and tc [0 / 3].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortTxQueueShaperEnableGet with the same tc and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.1.3. Call with enable [GT_FALSE and GT_TRUE]
                     and out of range tc [CPSS_TC_RANGE_CNS] = 8
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      tc        = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_FALSE and GT_TRUE]
                                 and tc [0 / 4].
                Expected: GT_OK.
            */

            /* Call with tc [0], enable [GT_FALSE] */
            tc     = 0;
            enable = GT_FALSE;

            st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueShaperEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueShaperEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [4], enable [GT_FALSE] */
            tc = 4;

            st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueShaperEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueShaperEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [0], enable [GT_TRUE] */
            tc     = 0;
            enable = GT_TRUE;

            st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueShaperEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueShaperEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [4], enable [GT_TRUE] */
            tc = 4;

            st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueShaperEnableGet with same tc and non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueShaperEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with enable [GT_FALSE and GT_TRUE]
                                 and out of range tc [CPSS_TC_RANGE_CNS] = 8.
                Expected: GT_BAD_PARAM.
            */
            tc = CPSS_TC_RANGE_CNS;

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);
        }

        tc     = 0;
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc     = 0;
    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueShaperEnableSet(dev, port, tc, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueShaperEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U32    tc,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueShaperEnableGet)
{
/*
    ITERATE_DEVICES_PHY_ PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 3]
                     and non_NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8
                     and non_NULL enablePtr.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with tc [5]
                     and enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      tc     = 0;
    GT_BOOL     enable = GT_FALSE;

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
                1.1.1. Call with tc [0 / 4]
                                 and non-NULL enablePtr.
                Expected: GT_OK.
            */

            /* Call with tc [0] */
            tc = 0;

            st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /* Call with tc [4] */
            tc = 4;

            st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /*
                1.1.2. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8
                                 and non-NULL enablePtr.
                Expected: GT_BAD_PARAM.
            */
            tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            tc = 7;

            /*
                1.1.3. Call with tc[7]
                                 and enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        tc = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueShaperEnableGet(dev, port, tc, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueShaperProfileSet
(
    IN    GT_U8     devNum,
    IN    GT_U8     portNum,
    IN    GT_U32    tc,
    IN    GT_U32    bucketSize,
    INOUT GT_U32    *maxRatePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueShaperProfileSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 7],
                     bucketSize [0 / 1000]
                     and non-null maxRatePtr [1 / 100].
    Expected: GT_OK and non-null maxRatePtr.
    1.1.2. Call cpssExMxPmPortTxQueueShaperProfileGet with same tc and non-NULL bucketSizePtr and maxRatePtr.
    Expected: GT_OK and the same bucketSize and maxRatePtr.
    1.1.3. Call with out of range tc [CPSS_TC_RANGE_CNS]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with out of range bucketSize [0x1000]
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.5. Call with bucketSize [0]
                     and maxRatePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      tc            = 0;
    GT_U32      bucketSize    = 0;
    GT_U32      bucketSizeGet = 0;
    GT_U32      maxRate       = 0;
    GT_U32      maxRateGet    = 0;

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
                1.1.1. Call with tc [0 / 7],
                                 bucketSize [0 / 1000]
                                 and non-null maxRatePtr [1 / 100].
                Expected: GT_OK and non-null maxRatePtr.
            */

            /* Call with tc [0] */
            tc         = 0;
            bucketSize = 0;
            maxRate    = 1;

            st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, bucketSize);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueShaperProfileGet with same tc and non-NULL bucketSizePtr and maxRatePtr.
                Expected: GT_OK and the same bucketSize and maxRatePtr.
            */
            st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSizeGet, &maxRateGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueShaperProfileGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(bucketSize, bucketSizeGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with tc [7] */
            tc         = 7;
            bucketSize = 1000;
            maxRate    = 100;

            st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, bucketSize);

            /*
                1.1.2. Call cpssExMxPmPortTxQueueShaperProfileGet with same tc and non-NULL bucketSizePtr and maxRatePtr.
                Expected: GT_OK and the same bucketSize and maxRatePtr.
            */
            st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSizeGet, &maxRateGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxQueueShaperProfileGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(bucketSize, bucketSizeGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range tc [CPSS_TC_RANGE_CNS]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            tc = 0;

            /*
                1.1.4. Call with out of range bucketSize [0x1000]
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            bucketSize = 0x1000;

            st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, bucketSize = %d", dev, port, bucketSize);

            bucketSize = 0;

            /*
                1.1.5. Call with bucketSize [0]
                                 and maxRatePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, maxRatePtr = NULL", dev, port);
        }

        tc         = 0;
        bucketSize = 0;
        maxRate    = 1;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc         = 0;
    bucketSize = 0;
    maxRate    = 1;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueShaperProfileSet(dev, port, tc, bucketSize, &maxRate);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxQueueShaperProfileGet
(
    IN    GT_U8      devNum,
    IN    GT_U8      portNum,
    IN    GT_U32     tc,
    OUT   GT_U32     *bucketSizePtr,
    OUT   GT_U32     *maxRatePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxQueueShaperProfileGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 5 / 7]
                     non-null bucketSizePtr
                     and non-null maxRatePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range tc [CPSS_TC_RANGE_CNS]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with bucketSizePtr [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.1.4. Call with non-null bucketSizePtr
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      tc         = 0;
    GT_U32      bucketSize = 0;
    GT_U32      maxRate    = 0;

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
                1.1.1. Call with tc [0 / 5 / 7]
                                 non-null bucketSizePtr
                                 and non-null maxRatePtr.
                Expected: GT_OK.
            */

            /* Call with tc [0] */
            tc = 0;

            st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /* Call with tc [5] */
            tc = 5;

            st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /* Call with tc [7] */
            tc = 7;

            st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /*
                1.1.2. Call with out of range tc [CPSS_TC_RANGE_CNS]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            tc = 0;

            /*
                1.1.3. Call with bucketSizePtr [NULL]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, NULL, &maxRate);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, bucketSizePtr = NULL", dev, port);

            /*
                1.1.4. Call with non-null bucketSizePtr
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, bucketSizePtr = NULL", dev, port);
        }

        tc = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxQueueShaperProfileGet(dev, port, tc, &bucketSize, &maxRate);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxShaperParamsSet
(
    IN    GT_U8          devNum,
    IN    GT_U32         xgPortsTokensRate,
    IN    GT_U32         gigPortsTokensRate,
    IN    GT_U32         gigPortsSlowRateRatio
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxShaperParamsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with xgPortsTokensRate [4],
                   gigPortsTokensRate [8],
                   gigPortsSlowRateRatio[16].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxShaperParamsGet with non-NULL pointers.
    Expected: GT_OK san the same xgPortsTokensRate, gigPortsTokensRate and gigPortsSlowRateRatio.
    1.3. Call with out of range xgPortsTokensRate [16],
                   gigPortsTokensRate [4],
                   gigPortsSlowRateRatio[8].
    Expected: NOT GT_OK.
    1.4. Call with xgPortsTokensRate [1],
                   out of range gigPortsTokensRate [16],
                   gigPortsSlowRateRatio[4].
    Expected: NOT GT_OK.
    1.5. Call with xgPortsTokensRate [8],
                   gigPortsTokensRate [1],
                   out of range gigPortsSlowRateRatio[17].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      xgPortsTokensRate       = 0;
    GT_U32      xgPortsTokensRateGet    = 0;
    GT_U32      gigPortsTokensRate      = 0;
    GT_U32      gigPortsTokensRateGet   = 0;
    GT_U32      gigPortsSlowRateRatio    = 0;
    GT_U32      gigPortsSlowRateRatioGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with xgPortsTokensRate [4],
                           gigPortsTokensRate [8],
                           gigPortsSlowRateRatio[16].
            Expected: GT_OK.
        */
        xgPortsTokensRate    = 4;
        gigPortsTokensRate   = 8;
        gigPortsSlowRateRatio = 16;

        st = cpssExMxPmPortTxShaperParamsSet(dev, xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);

        /*
            1.2. Call cpssExMxPmPortTxShaperParamsGet with non-NULL pointers.
            Expected: GT_OK san the same xgPortsTokensRate, gigPortsTokensRate and gigPortsSlowRateRatio.
        */
        st = cpssExMxPmPortTxShaperParamsGet(dev, &xgPortsTokensRateGet, &gigPortsTokensRateGet, &gigPortsSlowRateRatioGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxShaperParamsGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(xgPortsTokensRate, xgPortsTokensRateGet,
                       "get another xgPortsTokensRate than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(gigPortsTokensRate, gigPortsTokensRateGet,
                       "get another gigPortsTokensRate than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(gigPortsSlowRateRatio, gigPortsSlowRateRatioGet,
                       "get another gigPortsSlowRateRatio than was set: %d", dev);

        /*
            1.3. Call with out of range xgPortsTokensRate [16],
                           gigPortsTokensRate [4],
                           gigPortsSlowRateRatio[8].
            Expected: NOT GT_OK.
        */
        xgPortsTokensRate = 16;

        st = cpssExMxPmPortTxShaperParamsSet(dev, xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, xgPortsTokensRate);

        xgPortsTokensRate = 1;

        /*
            1.4. Call with xgPortsTokensRate [1],
                           out of range gigPortsTokensRate [16],
                           gigPortsSlowRateRatio[4].
            Expected: NOT GT_OK.
        */
        gigPortsTokensRate = 16;

        st = cpssExMxPmPortTxShaperParamsSet(dev, xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, gigPortsTokensRate = %d", dev, gigPortsTokensRate);

        gigPortsTokensRate = 1;

        /*
            1.5. Call with xgPortsTokensRate [8],
                           gigPortsTokensRate [1],
                           out of range gigPortsSlowRateRatio[17].
            Expected: NOT GT_OK.
        */
        gigPortsSlowRateRatio = 17;

        st = cpssExMxPmPortTxShaperParamsSet(dev, xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, gigPortsSlowRateRatio = %d", dev, gigPortsSlowRateRatio);
    }

    xgPortsTokensRate    = 4;
    gigPortsTokensRate   = 8;
    gigPortsSlowRateRatio = 16;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxShaperParamsSet(dev, xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxShaperParamsSet(dev, xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxShaperParamsGet
(
    IN    GT_U8          devNum,
    OUT   GT_U32         *xgPortsTokensRatePtr,
    OUT   GT_U32         *gigPortsTokensRatePtr,
    OUT   GT_U32         *gigPortsSlowRateRatioPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxShaperParamsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL xgPortsTokensRatePtr,
                   non-NULL gigPortsTokensRatePtr,
                   non-NULL gigPortsSlowRateRatioPtr.
    Expected: GT_OK.
    1.2. Call with xgPortsTokensRatePtr [NULL],
                   non-NULL gigPortsTokensRatePtr,
                   non-NULL gigPortsSlowRateRatioPtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-NULL xgPortsTokensRatePtr,
                   gigPortsTokensRatePtr [NULL],
                   non-NULL gigPortsSlowRateRatioPtr.
    Expected: GT_BAD_PTR.
    1.4. Call with non-NULL xgPortsTokensRatePtr,
                   non-NULL gigPortsTokensRatePtr,
                   gigPortsSlowRateRatioPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      xgPortsTokensRate    = 0;
    GT_U32      gigPortsTokensRate   = 0;
    GT_U32      gigPortsSlowRateRatio = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL xgPortsTokensRatePtr,
                           non-NULL gigPortsTokensRatePtr,
                           non-NULL gigPortsSlowRateRatioPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxShaperParamsGet(dev, &xgPortsTokensRate, &gigPortsTokensRate, &gigPortsSlowRateRatio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with xgPortsTokensRatePtr [NULL],
                           non-NULL gigPortsTokensRatePtr,
                           non-NULL gigPortsSlowRateRatioPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxShaperParamsGet(dev, NULL, &gigPortsTokensRate, &gigPortsSlowRateRatio);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xgPortsTokensRatePtr = NULL", dev);

        /*
            1.3. Call with non-NULL xgPortsTokensRatePtr,
                           gigPortsTokensRatePtr [NULL],
                           non-NULL gigPortsSlowRateRatioPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxShaperParamsGet(dev, &xgPortsTokensRate, NULL, &gigPortsSlowRateRatio);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, gigPortsTokensRatePtr = NULL", dev);

        /*
            1.4. Call with non-NULL xgPortsTokensRatePtr,
                           non-NULL gigPortsTokensRatePtr,
                           gigPortsSlowRateRatioPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxShaperParamsGet(dev, &xgPortsTokensRate, &gigPortsTokensRate, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, gigPortsSlowRateRatioPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxShaperParamsGet(dev, &xgPortsTokensRate, &gigPortsTokensRate, &gigPortsSlowRateRatio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxShaperParamsGet(dev, &xgPortsTokensRate, &gigPortsTokensRate, &gigPortsSlowRateRatio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxToCpuShaperModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxToCpuShaperModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E /
                         CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxToCpuShaperModeGet with non_NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call with out of range mode [0x5AAAAAA5].
    Expected:  GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT mode    = CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E;
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT modeGet = CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E /
                                 CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E] */
        mode = CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E;

        st = cpssExMxPmPortTxToCpuShaperModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPortTxToCpuShaperModeGet with non_NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmPortTxToCpuShaperModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxToCpuShaperModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* Call with mode [CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E] */
        mode = CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E;

        st = cpssExMxPmPortTxToCpuShaperModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPortTxToCpuShaperModeGet with non_NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmPortTxToCpuShaperModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxToCpuShaperModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = PORT_TX_INVALID_ENUM_CNS;

        st = cpssExMxPmPortTxToCpuShaperModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxToCpuShaperModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxToCpuShaperModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxToCpuShaperModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxToCpuShaperModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT  mode = CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E;

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
        st = cpssExMxPmPortTxToCpuShaperModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxToCpuShaperModeGet(dev, NULL);
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
        st = cpssExMxPmPortTxToCpuShaperModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxToCpuShaperModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxTailDropUcEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxTailDropUcEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxTailDropUcEnableGet with non-NULL enablePtr.
    Expected:  GT_OK and the same enable.
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

        st = cpssExMxPmPortTxTailDropUcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPortTxTailDropUcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPortTxTailDropUcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxTailDropUcEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmPortTxTailDropUcEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPortTxTailDropUcEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPortTxTailDropUcEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxTailDropUcEnableGet: %d", dev);

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
        st = cpssExMxPmPortTxTailDropUcEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxTailDropUcEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxTailDropUcEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxTailDropUcEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected:  GT_BAD_PTR.
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
        st = cpssExMxPmPortTxTailDropUcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxTailDropUcEnableGet(dev, NULL);
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
        st = cpssExMxPmPortTxTailDropUcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxTailDropUcEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxBufferTailDropEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxBufferTailDropEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxBufferTailDropEnableGet with non-NULL enablePtr.
    Expected:  GT_OK and the same enable.
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

        st = cpssExMxPmPortTxBufferTailDropEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPortTxBufferTailDropEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPortTxBufferTailDropEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxBufferTailDropEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmPortTxBufferTailDropEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPortTxBufferTailDropEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPortTxBufferTailDropEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxBufferTailDropEnableGet: %d", dev);

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
        st = cpssExMxPmPortTxBufferTailDropEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxBufferTailDropEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxBufferTailDropEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxBufferTailDropEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected:  GT_BAD_PTR.
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
        st = cpssExMxPmPortTxBufferTailDropEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxBufferTailDropEnableGet(dev, NULL);
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
        st = cpssExMxPmPortTxBufferTailDropEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxBufferTailDropEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxTailDropProfileSet
(
    IN    GT_U8                                   devNum,
    IN    GT_U32                                  index,
    IN    GT_U32                                  tc,
    IN    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxTailDropProfileSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5],
                   tc [0 / 3],
                   and non-null tailDropProfileParamsPtr{dp0MaxBuffNum [2],
                                                         dp1MaxBuffNum [4],
                                                         dp2MaxBuffNum [8],
                                                         dp0MaxDescrNum [16],
                                                         dp1MaxDescrNum [32],
                                                         dp2MaxDescrNum [128]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxTailDropProfileGet with the same index and tc and non-NULL tailDropProfileParamsPtr.
    Expected: GT_OK and the same tailDropProfileParamsPtr.
    1.3. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8,
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range tailDropProfileParamsPtr->dp0MaxBuffNum [PRV_CPSS_EXMXPM_TC_DP_BUFFER_LIMIT_MAX_VAL + 1] = 0x2000,
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range tailDropProfileParamsPtr->dp0MaxDescrNum [PRV_CPSS_EXMXPM_TC_DP_DESC_LIMIT_MAX_VAL + 1] = 0x2000,
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range tailDropProfileParamsPtr->dp2MaxBuffNum [PRV_CPSS_EXMXPM_TC_DP_BUFFER_LIMIT_MAX_VAL + 1] = 0x2000,
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with out of range tailDropProfileParamsPtr->dp2MaxDescrNum [PRV_CPSS_EXMXPM_TC_DP_DESC_LIMIT_MAX_VAL + 1] = 0x2000,
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with out of range tailDropProfileParamsPtr-> dp1MaxBuffNum [PRV_CPSS_EXMXPM_TC_DP_BUFFER_LIMIT_MAX_VAL + 1] = 0x2000 (not relevant),
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.10. Call with out of range tailDropProfileParamsPtr-> dp1MaxDescrNum [PRV_CPSS_EXMXPM_TC_DP_DESC_LIMIT_MAX_VAL + 1] = 0x2000 (not relevant),
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.11. Call with tailDropProfileParamsPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  index = 0;
    GT_U32                                  tc    = 0;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS params;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS paramsGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5],
                           tc [0 / 3],
                           and non-null tailDropProfileParamsPtr{dp0MaxBuffNum [2],
                                                                 dp1MaxBuffNum [4],
                                                                 dp2MaxBuffNum [8],
                                                                 dp0MaxDescrNum [16],
                                                                 dp1MaxDescrNum [32],
                                                                 dp2MaxDescrNum [128]}.
            Expected: GT_OK.
        */
        params.dp0MaxBuffNum = 2;
        params.dp1MaxBuffNum = 8;
        params.dp2MaxBuffNum = 16;
        params.dp0MaxDescrNum = 32;
        params.dp1MaxDescrNum = 64;
        params.dp2MaxDescrNum = 128;

        /* Call with index [0], tc [0] */
        index = 0;
        tc    = 0;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /*
            1.2. Call cpssExMxPmPortTxTailDropProfileGet with the same index and tc and non-NULL tailDropProfileParamsPtr.
            Expected: GT_OK and the same tailDropProfileParamsPtr.
        */
        st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, &paramsGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxTailDropProfileGet: %d, %d, %d", dev, index, tc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp0MaxBuffNum, paramsGet.dp0MaxBuffNum,
                   "get another tailDropProfileParamsPtr->dp0MaxBuffNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp1MaxBuffNum, paramsGet.dp1MaxBuffNum,
                   "get another tailDropProfileParamsPtr->dp1MaxBuffNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp2MaxBuffNum, paramsGet.dp2MaxBuffNum,
                   "get another tailDropProfileParamsPtr->dp2MaxBuffNum than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp0MaxDescrNum, paramsGet.dp0MaxDescrNum,
                   "get another tailDropProfileParamsPtr->dp0MaxDescrNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp1MaxDescrNum, paramsGet.dp1MaxDescrNum,
                   "get another tailDropProfileParamsPtr->dp1MaxDescrNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp2MaxDescrNum, paramsGet.dp2MaxDescrNum,
                   "get another tailDropProfileParamsPtr->dp2MaxDescrNum than was set: %d", dev);

        /* Call with index [5], tc [3] */
        index = 5;
        tc    = 3;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /*
            1.2. Call cpssExMxPmPortTxTailDropProfileGet with the same index and tc and non-NULL tailDropProfileParamsPtr.
            Expected: GT_OK and the same tailDropProfileParamsPtr.
        */
        st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, &paramsGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxTailDropProfileGet: %d, %d, %d", dev, index, tc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp0MaxBuffNum, paramsGet.dp0MaxBuffNum,
                   "get another tailDropProfileParamsPtr->dp0MaxBuffNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp1MaxBuffNum, paramsGet.dp1MaxBuffNum,
                   "get another tailDropProfileParamsPtr->dp1MaxBuffNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp2MaxBuffNum, paramsGet.dp2MaxBuffNum,
                   "get another tailDropProfileParamsPtr->dp2MaxBuffNum than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp0MaxDescrNum, paramsGet.dp0MaxDescrNum,
                   "get another tailDropProfileParamsPtr->dp0MaxDescrNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp1MaxDescrNum, paramsGet.dp1MaxDescrNum,
                   "get another tailDropProfileParamsPtr->dp1MaxDescrNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(params.dp2MaxDescrNum, paramsGet.dp2MaxDescrNum,
                   "get another tailDropProfileParamsPtr->dp2MaxDescrNum than was set: %d", dev);

        /*
            1.3. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8,
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tc = %d", dev, tc);

        tc = 0;

        /*
            1.5. Call with out of range tailDropProfileParamsPtr->dp0MaxBuffNum [PRV_CPSS_EXMXPM_TC_DP_BUFFER_LIMIT_MAX_VAL + 1] = 0x2000,
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        params.dp0MaxBuffNum = 0x2000;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tailDropProfileParamsPtr->dp0MaxBuffNum = %d",
                                         dev, params.dp0MaxBuffNum);

        params.dp0MaxBuffNum = 2;

        /*
            1.6. Call with out of range tailDropProfileParamsPtr->dp0MaxDescrNum [PRV_CPSS_EXMXPM_TC_DP_DESC_LIMIT_MAX_VAL + 1] = 0x2000,
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        params.dp0MaxDescrNum = 0x2000;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tailDropProfileParamsPtr->dp0MaxDescrNum = %d",
                                         dev, params.dp0MaxDescrNum);

        params.dp0MaxDescrNum = 16;

        /*
            1.7. Call with out of range tailDropProfileParamsPtr->dp2MaxBuffNum [PRV_CPSS_EXMXPM_TC_DP_BUFFER_LIMIT_MAX_VAL + 1] = 0x2000,
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        params.dp2MaxBuffNum = 0x2000;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tailDropProfileParamsPtr->dp2MaxBuffNum = %d",
                                         dev, params.dp2MaxBuffNum);

        params.dp2MaxBuffNum = 8;

        /*
            1.8. Call with out of range tailDropProfileParamsPtr->dp2MaxDescrNum [PRV_CPSS_EXMXPM_TC_DP_DESC_LIMIT_MAX_VAL + 1] = 0x2000,
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        params.dp2MaxDescrNum = 0x2000;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tailDropProfileParamsPtr->dp2MaxDescrNum = %d",
                                         dev, params.dp2MaxDescrNum);

        params.dp2MaxDescrNum = 128;

        /*
            1.9. Call with out of range tailDropProfileParamsPtr-> dp1MaxBuffNum [PRV_CPSS_EXMXPM_TC_DP_BUFFER_LIMIT_MAX_VAL + 1] = 0x2000 (out of range),
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        params.dp1MaxBuffNum = 0x2000;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tailDropProfileParamsPtr->dp12MaxBuffNum = %d",
                                     dev, params.dp1MaxBuffNum);

        params.dp1MaxBuffNum = 4;

        /*
            1.10. Call with out of range tailDropProfileParamsPtr-> dp1MaxDescrNum [PRV_CPSS_EXMXPM_TC_DP_DESC_LIMIT_MAX_VAL + 1] = 0x2000 (out of range),
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        params.dp1MaxDescrNum = 0x2000;

        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tailDropProfileParamsPtr->dp1MaxDescrNum = %d",
                                     dev, params.dp1MaxDescrNum);

        params.dp1MaxDescrNum = 32;

        /*
            1.11. Call with tailDropProfileParamsPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tailDropProfileParamsPtr = NULL", dev);
    }

    index = 0;
    tc    = 0;

    params.dp0MaxBuffNum = 2;
    params.dp1MaxBuffNum = 8;
    params.dp2MaxBuffNum = 16;
    params.dp0MaxDescrNum = 32;
    params.dp1MaxDescrNum = 64;
    params.dp2MaxDescrNum = 128;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxTailDropProfileSet(dev, index, tc, &params);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxTailDropProfileGet
(
    IN    GT_U8                                   devNum,
    IN    GT_U32                                  index,
    IN    GT_U32                                  tc,
    OUT   CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxTailDropProfileGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5],
                   tc [0 / 3],
                   and non-null tailDropProfileParamsPtr.
    Expected: GT_OK.
    1.2. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8,
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with tailDropProfileParamsPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  index = 0;
    GT_U32                                  tc    = 0;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS params;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5],
                           tc [0 / 3],
                           and non-null tailDropProfileParamsPtr.
            Expected: GT_OK.
        */

        /* Call with index [0], tc [0] */
        index = 0;
        tc    = 0;

        st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, &params);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /* Call with index [5], tc [3] */
        index = 5;
        tc    = 3;

        st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, &params);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /*
            1.2. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, &params);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8,
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, &params);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tc = %d", dev, tc);

        tc = 0;

        /*
            1.4. Call with tailDropProfileParamsPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tailDropProfileParamsPtr = NULL", dev);
    }

    index = 0;
    tc    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, &params);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxTailDropProfileGet(dev, index, tc, &params);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxProfileTailDropLimitsSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   index,
    IN  GT_U32   portMaxBuffLimit,
    IN  GT_U32   portMaxDescrLimit,
    IN  CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ENT sharedPoolUsers
}
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxProfileTailDropLimitsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5 / 7],
                   portMaxBuffLimit [0 / 100 / 8191],
                   portMaxDescrLimit [0 / 500 / 8191],
                   sharedPoolUsers [CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_GREEN_E /
                                    CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ALL_E]
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxProfileTailDropLimitsGet with the same index and non-NULL portMaxBuffLimitPtr, portMaxDescrLimitPtr, sharedPoolUsersPtr.
    Expected: GT_OK and the same portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers.
    1.3. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range portMaxBuffLimit [32768]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range portMaxDescrLimit [8192]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range sharedPoolUsers [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32   index                = 0;
    GT_U32   portMaxBuffLimit     = 0;
    GT_U32   portMaxDescrLimit    = 0;
    GT_U32   portMaxBuffLimitGet  = 0;
    GT_U32   portMaxDescrLimitGet = 0;

    CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ENT sharedPoolUsers;
    CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ENT sharedPoolUsersGet;

    sharedPoolUsers    = CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ALL_E;
    sharedPoolUsersGet = CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ALL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5 / 7],
                           portMaxBuffLimit [0 / 100 / 8191]
                           and portMaxDescrLimit [0 / 500 / 8191]
                           sharedPoolUsers [CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_GREEN_E /
                                            CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ALL_E]
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        portMaxBuffLimit  = 0;
        portMaxDescrLimit = 0;
        sharedPoolUsers   = CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_GREEN_E;

        st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, portMaxBuffLimit, portMaxDescrLimit);

        /*
            1.2. Call cpssExMxPmPortTxProfileTailDropLimitsGet with the same index and non-NULL portMaxBuffLimitPtr and portMaxDescrLimitPtr.
            Expected: GT_OK and the same portMaxBuffLimit and portMaxDescrLimit.
        */
        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimitGet, &portMaxDescrLimitGet, &sharedPoolUsersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxProfileTailDropLimitsGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(portMaxBuffLimit, portMaxBuffLimitGet,
                   "get another portMaxBuffLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portMaxDescrLimit, portMaxDescrLimitGet,
                   "get another portMaxDescrLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedPoolUsers, sharedPoolUsersGet,
                   "get another sharedPoolUsers than was set: %d", dev);

        /* Call with index [5] */
        index = 5;

        portMaxBuffLimit  = 100;
        portMaxDescrLimit = 500;
        sharedPoolUsers   = CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ALL_E;

        st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, portMaxBuffLimit, portMaxDescrLimit);

        /*
            1.2. Call cpssExMxPmPortTxProfileTailDropLimitsGet with the same index and non-NULL portMaxBuffLimitPtr and portMaxDescrLimitPtr.
            Expected: GT_OK and the same portMaxBuffLimit and portMaxDescrLimit.
        */
        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimitGet, &portMaxDescrLimitGet, &sharedPoolUsersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxProfileTailDropLimitsGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(portMaxBuffLimit, portMaxBuffLimitGet,
                   "get another portMaxBuffLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portMaxDescrLimit, portMaxDescrLimitGet,
                   "get another portMaxDescrLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedPoolUsers, sharedPoolUsersGet,
                   "get another sharedPoolUsers than was set: %d", dev);

        /* Call with index [7] */
        index = 7;

        portMaxBuffLimit  = 8191;
        portMaxDescrLimit = 8191;
        sharedPoolUsers   = CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ALL_E;

        st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, portMaxBuffLimit, portMaxDescrLimit);

        /*
            1.2. Call cpssExMxPmPortTxProfileTailDropLimitsGet with the same index and non-NULL portMaxBuffLimitPtr and portMaxDescrLimitPtr.
            Expected: GT_OK and the same portMaxBuffLimit and portMaxDescrLimit.
        */
        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimitGet, &portMaxDescrLimitGet, &sharedPoolUsersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxProfileTailDropLimitsGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(portMaxBuffLimit, portMaxBuffLimitGet,
                   "get another portMaxBuffLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portMaxDescrLimit, portMaxDescrLimitGet,
                   "get another portMaxDescrLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedPoolUsers, sharedPoolUsersGet,
                   "get another sharedPoolUsers than was set: %d", dev);

        /*
            1.3. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range portMaxBuffLimit [32768]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        portMaxBuffLimit = 32768;

        st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, portMaxBuffLimit = %d", dev, portMaxBuffLimit);

        portMaxBuffLimit = 0;

        /*
            1.5. Call with out of range portMaxDescrLimit [8192]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        portMaxDescrLimit = 8192;

        st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, portMaxDescrLimit = %d", dev, portMaxDescrLimit);

        portMaxDescrLimit = 8191;

        /*
            1.6. Call with out of range sharedPoolUsers [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        sharedPoolUsers = PORT_TX_INVALID_ENUM_CNS;

        st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, sharedPoolUsers = %d", dev, sharedPoolUsers);
    }

    index = 0;

    portMaxBuffLimit  = 0;
    portMaxDescrLimit = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxProfileTailDropLimitsSet(dev, index, portMaxBuffLimit, portMaxDescrLimit, sharedPoolUsers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxProfileTailDropLimitsGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   index,
    OUT GT_U32   *portMaxBuffLimitPtr,
    OUT GT_U32   *portMaxDescrLimitPtr,
    OUT CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ENT *sharedPoolUsersPtr

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxProfileTailDropLimitsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5],
                   and non-NULL portMaxBuffLimitPtr and portMaxDescrLimitPtr.
    Expected: GT_OK.
    1.2. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with portMaxBuffLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with portMaxDescrLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with sharedPoolUsersPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      index             = 0;
    GT_U32      portMaxBuffLimit  = 0;
    GT_U32      portMaxDescrLimit = 0;

    CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ENT  sharedPoolUsers;


    sharedPoolUsers = CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ALL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5],
                           and non-NULL portMaxBuffLimitPtr and portMaxDescrLimitPtr.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimit, &portMaxDescrLimit, &sharedPoolUsers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [5] */
        index = 5;

        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimit, &portMaxDescrLimit, &sharedPoolUsers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimit, &portMaxDescrLimit, &sharedPoolUsers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with portMaxBuffLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, NULL, &portMaxDescrLimit, &sharedPoolUsers);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portMaxBuffLimitPtr = NULL", dev);

        /*
            1.4. Call with portMaxDescrLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimit, NULL, &sharedPoolUsers);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portMaxDescrLimitPtr = NULL", dev);

        /*
            1.5. Call with sharedPoolUsersPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimit, &portMaxDescrLimit, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, sharedPoolUsersPtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimit, &portMaxDescrLimit, &sharedPoolUsers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxProfileTailDropLimitsGet(dev, index, &portMaxBuffLimit, &portMaxDescrLimit, &sharedPoolUsers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxTailDropE2eLimitsSet
(
    IN   GT_U8                                     devNum,
    IN   GT_U32                                    profileIndex,
    IN   CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC *xoffThresholdCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxTailDropE2eLimitsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 5 / 7],
                   xoffThresholdCfgPtr {dp0XoffThreshold [0 / 100 / 8191],
                                        dp1XoffThreshold [0 / 500 / 8191],
                                        dp2XoffThreshold [0 / 500 / 8191]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxTailDropE2eLimitsGet with the same profileIndex and non-NULL xoffThresholdCfgPtr.
    Expected: GT_OK and the same xoffThresholdCfgPtr.
    1.3. Call with out of range profileIndex [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range xoffThresholdCfgPtr->dp0XoffThreshold [8192]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range xoffThresholdCfgPtr->dp1XoffThreshold [8192]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range xoffThresholdCfgPtr->dp2XoffThreshold [8192]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with  xoffThresholdCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                   index = 0;
    CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC xoffCfg;
    CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC xoffCfgGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 5 / 7],
                           xoffThresholdCfgPtr {dp0XoffThreshold [0 / 100 / 8191],
                                                dp1XoffThreshold [0 / 500 / 8191]
                                                dp2XoffThreshold [0 / 500 / 8191]}.
            Expected: GT_OK.
        */

        /* Call with profileIndex [0] */
        index = 0;

        xoffCfg.dp0XoffThreshold = 0;
        xoffCfg.dp1XoffThreshold = 0;
        xoffCfg.dp2XoffThreshold = 0;

        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call cpssExMxPmPortTxTailDropE2eLimitsGet with the same profileIndex and non-NULL xoffThresholdCfgPtr.
            Expected: GT_OK and the same xoffThresholdCfgPtr.
        */
        st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, &xoffCfgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxTailDropE2eLimitsGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp0XoffThreshold, xoffCfgGet.dp0XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp0XoffThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp1XoffThreshold, xoffCfgGet.dp1XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp1XoffThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp2XoffThreshold, xoffCfgGet.dp2XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp2XoffThreshold than was set: %d", dev);

        /* Call with profileIndex [5] */
        index = 5;

        xoffCfg.dp0XoffThreshold = 100;
        xoffCfg.dp1XoffThreshold = 500;
        xoffCfg.dp2XoffThreshold = 500;

        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call cpssExMxPmPortTxTailDropE2eLimitsGet with the same profileIndex and non-NULL xoffThresholdCfgPtr.
            Expected: GT_OK and the same xoffThresholdCfgPtr.
        */
        st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, &xoffCfgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxTailDropE2eLimitsGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp0XoffThreshold, xoffCfgGet.dp0XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp0XoffThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp1XoffThreshold, xoffCfgGet.dp1XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp1XoffThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp2XoffThreshold, xoffCfgGet.dp2XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp2XoffThreshold than was set: %d", dev);

        /* Call with profileIndex [7] */
        index = 7;

        xoffCfg.dp0XoffThreshold = 8191;
        xoffCfg.dp1XoffThreshold = 8191;
        xoffCfg.dp2XoffThreshold = 8191;

        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call cpssExMxPmPortTxTailDropE2eLimitsGet with the same profileIndex and non-NULL xoffThresholdCfgPtr.
            Expected: GT_OK and the same xoffThresholdCfgPtr.
        */
        st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, &xoffCfgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxTailDropE2eLimitsGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp0XoffThreshold, xoffCfgGet.dp0XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp0XoffThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp1XoffThreshold, xoffCfgGet.dp1XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp1XoffThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffCfg.dp2XoffThreshold, xoffCfgGet.dp2XoffThreshold,
                   "get another xoffThresholdCfgPtr->dp2XoffThreshold than was set: %d", dev);

        /*
            1.3. Call with out of range profileIndex [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range xoffThresholdCfgPtr->dp0XoffThreshold [8192]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        xoffCfg.dp0XoffThreshold = 8192;

        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, xoffThresholdCfgPtr->dp0XoffThreshold = %d", dev, xoffCfg.dp0XoffThreshold);

        xoffCfg.dp0XoffThreshold = 0;

        /*
            1.5. Call with out of range xoffThresholdCfgPtr->dp1XoffThreshold [8192]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        xoffCfg.dp1XoffThreshold = 8192;

        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, xoffThresholdCfgPtr->dp1XoffThreshold = %d", dev, xoffCfg.dp1XoffThreshold);

        xoffCfg.dp1XoffThreshold = 0;

        /*
            1.6. Call with out of range xoffThresholdCfgPtr->dp2XoffThreshold [8192]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        xoffCfg.dp2XoffThreshold = 8192;

        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, xoffThresholdCfgPtr->dp2XoffThreshold = %d", dev, xoffCfg.dp2XoffThreshold);

        xoffCfg.dp2XoffThreshold = 0;

        /*
            1.7. Call with  xoffThresholdCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xoffThresholdCfgPtr = NULL", dev);
    }

    index = 0;

    xoffCfg.dp0XoffThreshold = 0;
    xoffCfg.dp1XoffThreshold = 0;
    xoffCfg.dp2XoffThreshold = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxTailDropE2eLimitsSet(dev, index, &xoffCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxTailDropE2eLimitsGet
(
    IN   GT_U8                                     devNum,
    IN   GT_U32                                    profileIndex,
    OUT  CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC *xoffThresholdCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxTailDropE2eLimitsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 5],
                   and non-NULL xoffThresholdCfgPtr.
    Expected: GT_OK.
    1.2. Call with out of range profileIndex [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with xoffThresholdCfgPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                   index = 0;
    CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC xoffCfg;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 5],
                           and non-NULL xoffThresholdCfgPtr.
            Expected: GT_OK.
        */

        /* Call with profileIndex [0] */
        index = 0;

        st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, &xoffCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with profileIndex [5] */
        index = 5;

        st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, &xoffCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with out of range profileIndex [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, &xoffCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with xoffThresholdCfgPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xoffThresholdCfgPtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, &xoffCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxTailDropE2eLimitsGet(dev, index, &xoffCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxTailDropProfileIndexSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U32   index
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxTailDropProfileIndexSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with index [0 / 5 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortTxTailDropProfileIndexGet with non_NULL indexPtr.
    Expected: GT_OK and the same index.
    1.1.3. Call with out of range index [8].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      index    = 0;
    GT_U32      indexGet = 0;

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
                1.1.1. Call with index [0 / 5 / 7].
                Expected: GT_OK.
            */

            /* Call with index [0] */
            index = 0;

            st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call cpssExMxPmPortTxTailDropProfileIndexGet with non_NULL indexPtr.
                Expected: GT_OK and the same index.
            */
            st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &indexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxTailDropProfileIndexGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(index, indexGet,
                       "get another index than was set: %d, %d", dev, port);

            /* Call with index [5] */
            index = 5;

            st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call cpssExMxPmPortTxTailDropProfileIndexGet with non_NULL indexPtr.
                Expected: GT_OK and the same index.
            */
            st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &indexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxTailDropProfileIndexGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(index, indexGet,
                       "get another index than was set: %d, %d", dev, port);

            /* Call with index [7] */
            index = 7;

            st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call cpssExMxPmPortTxTailDropProfileIndexGet with non_NULL indexPtr.
                Expected: GT_OK and the same index.
            */
            st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &indexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxTailDropProfileIndexGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(index, indexGet,
                       "get another index than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range index [8].
                Expected: NOT GT_OK.
            */
            index = 8;

            st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);
        }

        index = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    index = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxTailDropProfileIndexSet(dev, port, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxTailDropProfileIndexGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U32   *indexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxTailDropProfileIndexGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-NULL indexPtr.
    Expected: GT_OK.
    1.1.2. Call with indexPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      index = 0;

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
                1.1.1. Call with non-NULL indexPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &index);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with indexPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, indexPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &index);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet
(
    IN    GT_U8      devNum,
    IN    GT_U32     ingressMirroredLimitNum,
    IN    GT_U32     egressMirroredLimitNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ingressMirroredLimitNum [0 / 4095]
                   and egressMirroredLimitNum [0 / 4095]
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet with non-NULL ingressMirroredLimitPtr and egressMirroredLimitPtr.
    Expected: GT_OK and the same ingressMirroredLimitNum and egressMirroredLimitNum.
    1.3. Call with out of range ingressMirroredLimitNum [8192]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range egressMirroredLimitNum [8192]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ingressLimit    = 0;
    GT_U32      egressLimit     = 0;
    GT_U32      ingressLimitGet = 0;
    GT_U32      egressLimitGet  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ingressMirroredLimitNum [0 / 4095]
                           and egressMirroredLimitNum [0 / 4095]
            Expected: GT_OK.
        */

        /* Call with ingressMirroredLimitNum [0], egressMirroredLimitNum [0] */
        ingressLimit = 0;
        egressLimit  = 0;

        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet(dev, ingressLimit, egressLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ingressLimit, egressLimit);

        /*
            1.2. Call cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet with non-NULL ingressMirroredLimitPtr and egressMirroredLimitPtr.
            Expected: GT_OK and the same ingressMirroredLimitNum and egressMirroredLimitNum.
        */
        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet(dev, &ingressLimitGet, &egressLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ingressLimit, ingressLimitGet,
                   "get another ingressMirroredLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(egressLimit, egressLimitGet,
                   "get another egressMirroredLimit than was set: %d", dev);

        /* Call with ingressMirroredLimitNum [4095], egressMirroredLimitNum [4095] */
        ingressLimit = 4095;
        egressLimit  = 4095;

        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet(dev, ingressLimit, egressLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ingressLimit, egressLimit);

        /*
            1.2. Call cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet with non-NULL ingressMirroredLimitPtr and egressMirroredLimitPtr.
            Expected: GT_OK and the same ingressMirroredLimitNum and egressMirroredLimitNum.
        */
        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet(dev, &ingressLimitGet, &egressLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ingressLimit, ingressLimitGet,
                   "get another ingressMirroredLimitNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(egressLimit, egressLimitGet,
                   "get another egressMirroredLimitNum than was set: %d", dev);

        /*
            1.3. Call with out of range ingressMirroredLimitNum [8192]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ingressLimit = 8192;

        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet(dev, ingressLimit, egressLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ingressMirroredLimitNum = %d", dev, ingressLimit);

        ingressLimit = 0;

        /*
            1.4. Call with out of range egressMirroredLimitNum [8192]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        egressLimit = 8192;

        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet(dev, ingressLimit, egressLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, egressMirroredLimitNum = %d", dev, egressLimit);
    }

    ingressLimit = 0;
    egressLimit  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet(dev, ingressLimit, egressLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet(dev, ingressLimit, egressLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet
(
    IN    GT_U8   devNum,
    OUT   GT_U32  *ingressMirroredLimitPtr,
    OUT   GT_U32  *egressMirroredLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL ingressMirroredLimitPtr
                   and egressMirroredLimitPtr.
    Expected: GT_OK.
    1.2. Call with ingressMirroredLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with egressMirroredLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ingressLimit = 0;
    GT_U32      egressLimit  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL ingressMirroredLimitPtr
                           and egressMirroredLimitPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet(dev, &ingressLimit, &egressLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with ingressMirroredLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet(dev, NULL, &egressLimit);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ingressMirroredLimitPtr = NULL", dev);

        /*
            1.3. Call with egressMirroredLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet(dev, &ingressLimit, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, egressMirroredLimitPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet(dev, &ingressLimit, &egressLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet(dev, &ingressLimit, &egressLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxMcPacketDescriptorsLimitSet
(
    IN    GT_U8      devNum,
    IN    GT_U32     mcLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxMcPacketDescriptorsLimitSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mcLimit [0 / 10 / 0x7FFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxMcPacketDescriptorsLimitGet with non_NULL mcLimitPtr.
    Expected: GT_OK and the same mcLimit.
    1.3. Call with out of range mcLimit [0x8000].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      mcLimit    = 0;
    GT_U32      mcLimitGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mcLimit [0 / 10 / 0x7FFF].
            Expected: GT_OK.
        */

        /* Call with mcLimit [0] */
        mcLimit = 0;

        st = cpssExMxPmPortTxMcPacketDescriptorsLimitSet(dev, mcLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcLimit);

        /*
            1.2. Call cpssExMxPmPortTxMcPacketDescriptorsLimitGet with non_NULL mcLimitPtr.
            Expected: GT_OK and the same mcLimit.
        */
        st = cpssExMxPmPortTxMcPacketDescriptorsLimitGet(dev, &mcLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxMcPacketDescriptorsLimitGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcLimit, mcLimitGet,
                       "get another mcLimit than was set: %d", dev);

        /* Call with mcLimit [10] */
        mcLimit = 10;

        st = cpssExMxPmPortTxMcPacketDescriptorsLimitSet(dev, mcLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcLimit);

        /*
            1.2. Call cpssExMxPmPortTxMcPacketDescriptorsLimitGet with non_NULL mcLimitPtr.
            Expected: GT_OK and the same mcLimit.
        */
        st = cpssExMxPmPortTxMcPacketDescriptorsLimitGet(dev, &mcLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxMcPacketDescriptorsLimitGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcLimit, mcLimitGet,
                       "get another mcLimit than was set: %d", dev);

        /* Call with mcLimit [0x7FFF] */
        mcLimit = 0x7FFF;

        st = cpssExMxPmPortTxMcPacketDescriptorsLimitSet(dev, mcLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcLimit);

        /*
            1.2. Call cpssExMxPmPortTxMcPacketDescriptorsLimitGet with non_NULL mcLimitPtr.
            Expected: GT_OK and the same mcLimit.
        */
        st = cpssExMxPmPortTxMcPacketDescriptorsLimitGet(dev, &mcLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxMcPacketDescriptorsLimitGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcLimit, mcLimitGet,
                       "get another mcLimit than was set: %d", dev);

        /*
            1.3. Call with out of range mcLimit [0x8000].
            Expected: NOT GT_OK.
        */
        mcLimit = 0x8000;

        st = cpssExMxPmPortTxMcPacketDescriptorsLimitSet(dev, mcLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcLimit);
    }

    mcLimit = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxMcPacketDescriptorsLimitSet(dev, mcLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxMcPacketDescriptorsLimitSet(dev, mcLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxMcPacketDescriptorsLimitGet
(
    IN    GT_U8  devNum,
    OUT   GT_U32 *mcLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxMcPacketDescriptorsLimitGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1.1. Call with non-NULL mcLimitPtr.
    Expected: GT_OK.
    1.1.2. Call with mcLimitPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      mcLimit = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null mcLimitPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxMcPacketDescriptorsLimitGet(dev, &mcLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null mcLimitPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxMcPacketDescriptorsLimitGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcLimitPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxMcPacketDescriptorsLimitGet(dev, &mcLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxMcPacketDescriptorsLimitGet(dev, &mcLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSchedulerConfigSet
(
    IN    GT_U8                     devNum,
    IN    CPSS_PORT_TX_WRR_MODE_ENT wrrMode,
    IN    CPSS_PORT_TX_WRR_MTU_ENT  wrrMtu
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSchedulerConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with wrrMode [CPSS_PORT_TX_WRR_BYTE_MODE_E /
                            CPSS_PORT_TX_WRR_PACKET_MODE_E]
                   and wrrMtu [CPSS_PORT_TX_WRR_MTU_2K_E /
                               CPSS_PORT_TX_WRR_MTU_8K_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxSchedulerConfigGet with non-NULL wrrModePtr and wrrMtuPtr.
    Expected: GT_OK and the same wrrMode and wrrMtu.
    1.3. Call with out of range wrrMode [0x5AAAAAA5]
                   and wrrMtu [CPSS_PORT_TX_WRR_MTU_8K_E].
    Expected: GT_BAD_PARAM.
    1.4. Call with wrrMode [CPSS_PORT_TX_WRR_BYTE_MODE_E]
                   and wrrMtu [CPSS_PORT_TX_WRR_MTU_256_E] (not supported).
    Expected: NOT GT_OK.
    1.5. Call with wrrMode [CPSS_PORT_TX_WRR_BYTE_MODE_E]
                   and out of range wrrMtu [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_TX_WRR_MODE_ENT wrrMode    = CPSS_PORT_TX_WRR_BYTE_MODE_E;
    CPSS_PORT_TX_WRR_MTU_ENT  wrrMtu     = CPSS_PORT_TX_WRR_MTU_2K_E;
    CPSS_PORT_TX_WRR_MODE_ENT wrrModeGet = CPSS_PORT_TX_WRR_BYTE_MODE_E;
    CPSS_PORT_TX_WRR_MTU_ENT  wrrMtuGet  = CPSS_PORT_TX_WRR_MTU_2K_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with wrrMode [CPSS_PORT_TX_WRR_BYTE_MODE_E /
                                    CPSS_PORT_TX_WRR_PACKET_MODE_E]
                           and wrrMtu [CPSS_PORT_TX_WRR_MTU_2K_E /
                                       CPSS_PORT_TX_WRR_MTU_8K_E].
            Expected: GT_OK.
        */

        /* Call with wrrMode [CPSS_PORT_TX_WRR_BYTE_MODE_E], wrrMtu [CPSS_PORT_TX_WRR_MTU_2K_E] */
        wrrMode = CPSS_PORT_TX_WRR_BYTE_MODE_E;
        wrrMtu  = CPSS_PORT_TX_WRR_MTU_2K_E;

        st = cpssExMxPmPortTxSchedulerConfigSet(dev, wrrMode, wrrMtu);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, wrrMode, wrrMtu);

        /*
            1.2. Call cpssExMxPmPortTxSchedulerConfigGet with non-NULL wrrModePtr and wrrMtuPtr.
            Expected: GT_OK and the same wrrMode and wrrMtu.
        */
        st = cpssExMxPmPortTxSchedulerConfigGet(dev, &wrrModeGet, &wrrMtuGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxSchedulerConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrMode, wrrModeGet,
                   "get another wrrMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrMtu, wrrMtuGet,
                   "get another wrrMtu than was set: %d", dev);

        /* Call with wrrMode [CPSS_PORT_TX_WRR_PACKET_MODE_E], wrrMtu [CPSS_PORT_TX_WRR_MTU_8K_E] */
        wrrMode = CPSS_PORT_TX_WRR_PACKET_MODE_E;
        wrrMtu  = CPSS_PORT_TX_WRR_MTU_8K_E;

        st = cpssExMxPmPortTxSchedulerConfigSet(dev, wrrMode, wrrMtu);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, wrrMode, wrrMtu);

        /*
            1.2. Call cpssExMxPmPortTxSchedulerConfigGet with non-NULL wrrModePtr and wrrMtuPtr.
            Expected: GT_OK and the same wrrMode and wrrMtu.
        */
        st = cpssExMxPmPortTxSchedulerConfigGet(dev, &wrrModeGet, &wrrMtuGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxSchedulerConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrMode, wrrModeGet,
                   "get another wrrMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrMtu, wrrMtuGet,
                   "get another wrrMtu than was set: %d", dev);

        /*
            1.3. Call with out of range wrrMode [0x5AAAAAA5]
                           and wrrMtu [CPSS_PORT_TX_WRR_MTU_8K_E].
            Expected: GT_BAD_PARAM.
        */
        wrrMode = PORT_TX_INVALID_ENUM_CNS;

        st = cpssExMxPmPortTxSchedulerConfigSet(dev, wrrMode, wrrMtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, wrrMode);

        wrrMode = CPSS_PORT_TX_WRR_BYTE_MODE_E;

        /*
            1.4. Call with wrrMode [CPSS_PORT_TX_WRR_BYTE_MODE_E]
                           and wrrMtu [CPSS_PORT_TX_WRR_MTU_256_E] (not supported).
            Expected: NOT GT_OK.
        */
        wrrMtu = CPSS_PORT_TX_WRR_MTU_256_E;

        st = cpssExMxPmPortTxSchedulerConfigSet(dev, wrrMode, wrrMtu);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, wrrMtu = %d", dev, wrrMtu);

        /*
            1.5. Call with wrrMode [CPSS_PORT_TX_WRR_BYTE_MODE_E]
                           and out of range wrrMtu [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        wrrMtu = PORT_TX_INVALID_ENUM_CNS;

        st = cpssExMxPmPortTxSchedulerConfigSet(dev, wrrMode, wrrMtu);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, wrrMtu = %d", dev, wrrMtu);
    }

    wrrMode = CPSS_PORT_TX_WRR_BYTE_MODE_E;
    wrrMtu  = CPSS_PORT_TX_WRR_MTU_2K_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSchedulerConfigSet(dev, wrrMode, wrrMtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSchedulerConfigSet(dev, wrrMode, wrrMtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSchedulerConfigGet
(
    IN    GT_U8                     devNum,
    OUT   CPSS_PORT_TX_WRR_MODE_ENT *wrrModePtr,
    OUT   CPSS_PORT_TX_WRR_MTU_ENT  *wrrMtuPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSchedulerConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL wrrModePtr
                   and wrrMtuPtr.
    Expected: GT_OK.
    1.2. Call with wrrModePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with wrrMtuPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PORT_TX_WRR_MODE_ENT wrrMode = CPSS_PORT_TX_WRR_BYTE_MODE_E;
    CPSS_PORT_TX_WRR_MTU_ENT  wrrMtu  = CPSS_PORT_TX_WRR_MTU_2K_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL wrrModePtr
                           and wrrMtuPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxSchedulerConfigGet(dev, &wrrMode, &wrrMtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrrModePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSchedulerConfigGet(dev, NULL, &wrrMtu);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, wrrModePtr = NULL", dev);

        /*
            1.3. Call with wrrMtuPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSchedulerConfigGet(dev, &wrrMode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, wrrMtuPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSchedulerConfigGet(dev, &wrrMode, &wrrMtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSchedulerConfigGet(dev, &wrrMode, &wrrMtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSchedulerProfileIndexSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  index
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSchedulerProfileIndexSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with index [0 / 5 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortTxSchedulerProfileIndexGet with non_NULL indexPtr.
    Expected: GT_OK and the same index.
    1.1.3. Call with out of range index [8].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      index    = 0;
    GT_U32      indexGet = 0;

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
                1.1.1. Call with index [0 / 5 / 7].
                Expected: GT_OK.
            */

            /* Call with index [0] */
            index = 0;

            st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call cpssExMxPmPortTxSchedulerProfileIndexGet with non_NULL indexPtr.
                Expected: GT_OK and the same index.
            */
            st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, &indexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSchedulerProfileIndexGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(index, indexGet,
                       "get another index than was set: %d, %d", dev, port);

            /* Call with index [5] */
            index = 5;

            st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call cpssExMxPmPortTxSchedulerProfileIndexGet with non_NULL indexPtr.
                Expected: GT_OK and the same index.
            */
            st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, &indexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSchedulerProfileIndexGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(index, indexGet,
                       "get another index than was set: %d, %d", dev, port);

            /* Call with index [7] */
            index = 7;

            st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call cpssExMxPmPortTxTailDropProfileIndexGet with non_NULL indexPtr.
                Expected: GT_OK and the same index.
            */
            st = cpssExMxPmPortTxTailDropProfileIndexGet(dev, port, &indexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxTailDropProfileIndexGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(index, indexGet,
                       "get another index than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range index [8].
                Expected: NOT GT_OK.
            */
            index = 8;

            st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);
        }

        index = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    index = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSchedulerProfileIndexSet(dev, port, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSchedulerProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *indexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSchedulerProfileIndexGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-NULL indexPtr.
    Expected: GT_OK.
    1.1.2. Call with indexPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_TX_VALID_PHY_PORT_CNS;

    GT_U32      index = 0;

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
                1.1.1. Call with non-NULL indexPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, &index);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with indexPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, indexPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if (port >= PORT_TX__CPSS_EXMXPM_QUEUE_GROUPS_NUM_CNS)
            {
                /* 1.2.1. Call function for each non-active port */
                st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, &index);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSchedulerProfileIndexGet(dev, port, &index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSchedulerWrrProfileSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  index,
    IN  GT_U32  tc,
    IN  GT_U32  wrrWeight
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSchedulerWrrProfileSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5 / 7],
                   tc [0 / 3 / 7]
                   and wrrWeight [0 / 100 / 0xFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxSchedulerWrrProfileGet with the same index, tc and non_NULL wrrWeightPtr.
    Expected: GT_OK and the same wrrWeight.
    1.3. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range tc [CPSS_TC_RANGE_CNS]=8
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range wrrWeight [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      index        = 0;
    GT_U32      tc           = 0;
    GT_U32      wrrWeight    = 0;
    GT_U32      wrrWeightGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5 / 7],
                           tc [0 / 3 / 7]
                           and wrrWeight [0 / 100 / 0xFF].
            Expected: GT_OK.
        */

        /* Call with index [0], tc [0] */
        index     = 0;
        tc        = 0;
        wrrWeight = 0;

        st = cpssExMxPmPortTxSchedulerWrrProfileSet(dev, index, tc, wrrWeight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, tc, wrrWeight);

        /*
            1.2. Call cpssExMxPmPortTxSchedulerWrrProfileGet with the same index, tc and non_NULL wrrWeightPtr.
            Expected: GT_OK and the same wrrWeight.
        */
        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeightGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSchedulerWrrProfileGet: %d, %d, %d", dev, index, tc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrWeight, wrrWeightGet,
                   "get another wrrWeight than was set: %d", dev);

        /* Call with index [5], tc [3] */
        index     = 5;
        tc        = 3;
        wrrWeight = 100;

        st = cpssExMxPmPortTxSchedulerWrrProfileSet(dev, index, tc, wrrWeight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, tc, wrrWeight);

        /*
            1.2. Call cpssExMxPmPortTxSchedulerWrrProfileGet with the same index, tc and non_NULL wrrWeightPtr.
            Expected: GT_OK and the same wrrWeight.
        */
        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeightGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSchedulerWrrProfileGet: %d, %d, %d", dev, index, tc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrWeight, wrrWeightGet,
                   "get another wrrWeight than was set: %d", dev);

        /* Call with index [7], tc [7] */
        index     = 7;
        tc        = 7;
        wrrWeight = 0xFF;

        st = cpssExMxPmPortTxSchedulerWrrProfileSet(dev, index, tc, wrrWeight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, tc, wrrWeight);

        /*
            1.2. Call cpssExMxPmPortTxSchedulerWrrProfileGet with the same index, tc and non_NULL wrrWeightPtr.
            Expected: GT_OK and the same wrrWeight.
        */
        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeightGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSchedulerWrrProfileGet: %d, %d, %d", dev, index, tc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrWeight, wrrWeightGet,
                   "get another wrrWeight than was set: %d", dev);

        /*
            1.3. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxSchedulerWrrProfileSet(dev, index, tc, wrrWeight);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8,
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPortTxSchedulerWrrProfileSet(dev, index, tc, wrrWeight);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tc = %d", dev, tc);

        tc = 0;

        /*
            1.5. Call with out of range wrrWeight [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        wrrWeight = 256;

        st = cpssExMxPmPortTxSchedulerWrrProfileSet(dev, index, tc, wrrWeight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, wrrWeight = %d", dev, wrrWeight);
    }

    index     = 0;
    tc        = 0;
    wrrWeight = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSchedulerWrrProfileSet(dev, index, tc, wrrWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSchedulerWrrProfileSet(dev, index, tc, wrrWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSchedulerWrrProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  index,
    IN  GT_U32  tc,
    OUT GT_U32  *wrrWeightPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSchedulerWrrProfileGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5 / 7],
                   tc [0 / 3 / 7]
                   and non-NULL wrrWeightPtr.
    Expected: GT_OK.
    1.2. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrrWeightPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      index     = 0;
    GT_U32      tc        = 0;
    GT_U32      wrrWeight = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5 / 7],
                           tc [0 / 3 / 7]
                           and non-NULL wrrWeightPtr.
            Expected: GT_OK.
        */

        /* Call with index [0], tc [0] */
        index = 0;
        tc    = 0;

        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeight);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /* Call with index [5], tc [3] */
        index = 5;
        tc    = 3;

        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeight);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /* Call with index [7], tc [7] */
        index = 7;
        tc    = 7;

        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeight);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /*
            1.2. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeight);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8,
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeight);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tc = %d", dev, tc);

        tc = 0;

        /*
            1.4. Call with wrrWeightPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, wrrWeightPtr = NULL", dev);
    }

    index = 0;
    tc    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSchedulerWrrProfileGet(dev, index, tc, &wrrWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSchedulerArbitrationGroupSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    IN  GT_U32                          tc,
    IN  CPSS_PORT_TX_Q_ARB_GROUP_ENT    arbGroup
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSchedulerArbitrationGroupSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5 / 7],
                   tc [0 / 3 / 7]
                   and arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E /
                                 CPSS_PORT_TX_WRR_ARB_GROUP_1_E /
                                 CPSS_PORT_TX_SP_ARB_GROUP_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxSchedulerArbitrationGroupGet with the same index, tc and non_NULL arbGroupPtr.
    Expected: GT_OK and the same arbGroup.
    1.3. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range tc [CPSS_TC_RANGE_CNS]=8
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range arbGroup [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                       index       = 0;
    GT_U32                       tc          = 0;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT arbGroup    = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT arbGroupGet = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5 / 7],
                           tc [0 / 3 / 7]
                           and arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E /
                                         CPSS_PORT_TX_WRR_ARB_GROUP_1_E /
                                         CPSS_PORT_TX_SP_ARB_GROUP_E].
            Expected: GT_OK.
        */

        /* Call with index [0], tc [0] */
        index    = 0;
        tc       = 0;
        arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupSet(dev, index, tc, arbGroup);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, tc, arbGroup);

        /*
            1.2. Call cpssExMxPmPortTxSchedulerArbitrationGroupGet with the same index, tc and non_NULL arbGroupPtr.
            Expected: GT_OK and the same arbGroup.
        */
        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroupGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSchedulerArbitrationGroupGet: %d, %d, %d", dev, index, tc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arbGroup, arbGroupGet,
                   "get another arbGroup than was set: %d", dev);

        /* Call with index [5], tc [3] */
        index     = 5;
        tc        = 3;
        arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupSet(dev, index, tc, arbGroup);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, tc, arbGroup);

        /*
            1.2. Call cpssExMxPmPortTxSchedulerArbitrationGroupGet with the same index, tc and non_NULL arbGroupPtr.
            Expected: GT_OK and the same arbGroup.
        */
        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroupGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSchedulerArbitrationGroupGet: %d, %d, %d", dev, index, tc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arbGroup, arbGroupGet,
                   "get another arbGroup than was set: %d", dev);

        /* Call with index [7], tc [7] */
        index     = 7;
        tc        = 7;
        arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupSet(dev, index, tc, arbGroup);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, tc, arbGroup);

        /*
            1.2. Call cpssExMxPmPortTxSchedulerArbitrationGroupGet with the same index, tc and non_NULL arbGroupPtr.
            Expected: GT_OK and the same arbGroup.
        */
        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroupGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSchedulerArbitrationGroupGet: %d, %d, %d", dev, index, tc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arbGroup, arbGroupGet,
                   "get another arbGroup than was set: %d", dev);

        /*
            1.3. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupSet(dev, index, tc, arbGroup);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8,
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupSet(dev, index, tc, arbGroup);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tc = %d", dev, tc);

        tc = 0;

        /*
            1.5. Call with out of range arbGroup [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        arbGroup = PORT_TX_INVALID_ENUM_CNS;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupSet(dev, index, tc, arbGroup);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, arbGroup = %d", dev, arbGroup);
    }

    index    = 0;
    tc       = 0;
    arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSchedulerArbitrationGroupSet(dev, index, tc, arbGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSchedulerArbitrationGroupSet(dev, index, tc, arbGroup);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSchedulerArbitrationGroupGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            index,
    IN  GT_U32                            tc,
    OUT CPSS_PORT_TX_Q_ARB_GROUP_ENT      *arbGroupPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSchedulerArbitrationGroupGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5 / 7],
                   tc [0 / 3 / 7]
                   and non-NULL arbGroupPtr.
    Expected: GT_OK.
    1.2. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with arbGroupPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                       index    = 0;
    GT_U32                       tc       = 0;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5 / 7],
                           tc [0 / 3 / 7]
                           and non-NULL arbGroupPtr.
            Expected: GT_OK.
        */

        /* Call with index [0], tc [0] */
        index = 0;
        tc    = 0;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroup);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /* Call with index [5], tc [3] */
        index = 5;
        tc    = 3;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroup);;
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /* Call with index [7], tc [7] */
        index = 7;
        tc    = 7;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroup);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, tc);

        /*
            1.2. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroup);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with out of range tc [CPSS_TC_RANGE_CNS] = 8,
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroup);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tc = %d", dev, tc);

        tc = 0;

        /*
            1.4. Call with arbGroupPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arbGroupPtr = NULL", dev);
    }

    index = 0;
    tc    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSchedulerArbitrationGroupGet(dev, index, tc, &arbGroup);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSharingResourcesEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSharingResourcesEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxSharingResourcesEnableGet with non-NULL enablePtr.
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

        st = cpssExMxPmPortTxSharingResourcesEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPortTxSharingResourcesEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPortTxSharingResourcesEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSharingResourcesEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmPortTxSharingResourcesEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPortTxSharingResourcesEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPortTxSharingResourcesEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxSharingResourcesEnableGet: %d", dev);

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
        st = cpssExMxPmPortTxSharingResourcesEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSharingResourcesEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSharingResourcesEnableGet
(
    IN   GT_U8       devNum,
    OUT  GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSharingResourcesEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected:  GT_BAD_PTR.
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
        st = cpssExMxPmPortTxSharingResourcesEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSharingResourcesEnableGet(dev, NULL);
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
        st = cpssExMxPmPortTxSharingResourcesEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSharingResourcesEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSharedResourcesLimitsSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  sharedBufLimit,
    IN  GT_U32  sharedDescLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSharedResourcesLimitsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with sharedBufLimit [0 / 100 / 8191]
                   and sharedDescLimit [0 / 500 / 8191]
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortTxSharedResourcesLimitsGet with the same index and non-NULL sharedBufLimitPtr and sharedDescLimitPtr.
    Expected: GT_OK and the same sharedBufLimit and sharedDescLimit.
    1.3. Call with out of range sharedBufLimit [8192]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range sharedDescLimit [8192]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      sharedBufLimit     = 0;
    GT_U32      sharedDescLimit    = 0;
    GT_U32      sharedBufLimitGet  = 0;
    GT_U32      sharedDescLimitGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with sharedBufLimit [0 / 100 / 8191]
                           and sharedDescLimit [0 / 500 / 8191]
            Expected: GT_OK.
        */

        /* Call with sharedBufLimit [0], sharedDescLimit [0] */
        sharedBufLimit  = 0;
        sharedDescLimit = 0;

        st = cpssExMxPmPortTxSharedResourcesLimitsSet(dev, sharedBufLimit, sharedDescLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sharedBufLimit, sharedDescLimit);

        /*
            1.2. Call cpssExMxPmPortTxSharedResourcesLimitsGet with the same index and non-NULL sharedBufLimitPtr and sharedDescLimitPtr.
            Expected: GT_OK and the same sharedBufLimit and sharedDescLimit.
        */
        st = cpssExMxPmPortTxSharedResourcesLimitsGet(dev, &sharedBufLimitGet, &sharedDescLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxSharedResourcesLimitsGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedBufLimit, sharedBufLimitGet,
                   "get another sharedBufLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedDescLimit, sharedDescLimitGet,
                   "get another sharedDescLimit than was set: %d", dev);

        /* Call with sharedBufLimit [100], sharedDescLimit [500] */
        sharedBufLimit  = 100;
        sharedDescLimit = 500;

        st = cpssExMxPmPortTxSharedResourcesLimitsSet(dev, sharedBufLimit, sharedDescLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sharedBufLimit, sharedDescLimit);

        /*
            1.2. Call cpssExMxPmPortTxSharedResourcesLimitsGet with the same index and non-NULL sharedBufLimitPtr and sharedDescLimitPtr.
            Expected: GT_OK and the same sharedBufLimit and sharedDescLimit.
        */
        st = cpssExMxPmPortTxSharedResourcesLimitsGet(dev, &sharedBufLimitGet, &sharedDescLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxSharedResourcesLimitsGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedBufLimit, sharedBufLimitGet,
                   "get another sharedBufLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedDescLimit, sharedDescLimitGet,
                   "get another sharedDescLimit than was set: %d", dev);

        /* Call with sharedBufLimit [8191], sharedDescLimit [8191] */
        sharedBufLimit  = 8191;
        sharedDescLimit = 8191;

        st = cpssExMxPmPortTxSharedResourcesLimitsSet(dev, sharedBufLimit, sharedDescLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sharedBufLimit, sharedDescLimit);

        /*
            1.2. Call cpssExMxPmPortTxSharedResourcesLimitsGet with the same index and non-NULL sharedBufLimitPtr and sharedDescLimitPtr.
            Expected: GT_OK and the same sharedBufLimit and sharedDescLimit.
        */
        st = cpssExMxPmPortTxSharedResourcesLimitsGet(dev, &sharedBufLimitGet, &sharedDescLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxSharedResourcesLimitsGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedBufLimit, sharedBufLimitGet,
                   "get another sharedBufLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(sharedDescLimit, sharedDescLimitGet,
                   "get another sharedDescLimit than was set: %d", dev);

        /*
            1.3. Call with out of range sharedBufLimit [8192]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        sharedBufLimit = 8192;

        st = cpssExMxPmPortTxSharedResourcesLimitsSet(dev, sharedBufLimit, sharedDescLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, sharedBufLimit = %d", dev, sharedBufLimit);

        sharedBufLimit = 0;

        /*
            1.4. Call with out of range sharedDescLimit [8192]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        sharedDescLimit = 8192;

        st = cpssExMxPmPortTxSharedResourcesLimitsSet(dev, sharedBufLimit, sharedDescLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, sharedDescLimit = %d", dev, sharedDescLimit);
    }

    sharedBufLimit  = 0;
    sharedDescLimit = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSharedResourcesLimitsSet(dev, sharedBufLimit, sharedDescLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSharedResourcesLimitsSet(dev, sharedBufLimit, sharedDescLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxSharedResourcesLimitsGet
(
    IN   GT_U8   devNum,
    OUT  GT_U32  *sharedBufLimitPtr,
    OUT  GT_U32  *sharedDescLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxSharedResourcesLimitsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5],
                   and non-NULL sharedBufLimitPtr and sharedDescLimitPtr.
    Expected: GT_OK.
    1.2. Call with sharedBufLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with sharedDescLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      sharedBufLimit  = 0;
    GT_U32      sharedDescLimit = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5],
                           and non-NULL sharedBufLimitPtr and sharedDescLimitPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxSharedResourcesLimitsGet(dev, &sharedBufLimit, &sharedDescLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with sharedBufLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSharedResourcesLimitsGet(dev, NULL, &sharedDescLimit);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, sharedBufLimitPtr = NULL", dev);

        /*
            1.3. Call with sharedDescLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxSharedResourcesLimitsGet(dev, &sharedBufLimit, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, sharedDescLimitPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxSharedResourcesLimitsGet(dev, &sharedBufLimit, &sharedDescLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxSharedResourcesLimitsGet(dev, &sharedBufLimit, &sharedDescLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxRandomTailDropMasksSet
(
    IN  GT_U8   devNum,
    IN  CPSS_EXMXPM_PORT_TX_RANDOM_TAIL_DROP_MASKS_STC  *randomTailDropMasksPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxRandomTailDropMasksSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with masks triples {0,0,0), (100,150,200}, (255,255,255}
    Expected: GT_OK.
    1.2. Call with masks triples {256,0,0), (0,256,0}, (0,0,256}
    Expected: NOT GT_OK.
    1.3. Call with NULL pointer
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PORT_TX_RANDOM_TAIL_DROP_MASKS_STC  randomTailDropMasks;
    CPSS_EXMXPM_PORT_TX_RANDOM_TAIL_DROP_MASKS_STC  randomTailDropMasksGet;

    randomTailDropMasks.globalRandomTailDropMask    = 0;
    randomTailDropMasks.portRandomTailDropMask      = 0;
    randomTailDropMasks.priorityRandomTailDropMask  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with masks triples {0,0,0), (100,150,200}, (255,255,255}
            Expected: GT_OK.
        */

        /* Call with triple {0,0,0} */
        randomTailDropMasks.globalRandomTailDropMask    = 0;
        randomTailDropMasks.portRandomTailDropMask      = 0;
        randomTailDropMasks.priorityRandomTailDropMask  = 0;

        st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, &randomTailDropMasks);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev,
                                    randomTailDropMasks.globalRandomTailDropMask,
                                    randomTailDropMasks.portRandomTailDropMask,
                                    randomTailDropMasks.priorityRandomTailDropMask);

        st = cpssExMxPmPortTxRandomTailDropMasksGet(dev, &randomTailDropMasksGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxRandomTailDropMasksGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.globalRandomTailDropMask,
            randomTailDropMasksGet.globalRandomTailDropMask,
                   "get another globalRandomTailDropMask than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.portRandomTailDropMask,
            randomTailDropMasksGet.portRandomTailDropMask,
                   "get another portRandomTailDropMask than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.priorityRandomTailDropMask,
            randomTailDropMasksGet.priorityRandomTailDropMask,
                   "get another priorityRandomTailDropMask than was set: %d", dev);

        /* Call with triple {100,150,200} */
        randomTailDropMasks.globalRandomTailDropMask    = 100;
        randomTailDropMasks.portRandomTailDropMask      = 150;
        randomTailDropMasks.priorityRandomTailDropMask  = 200;

        st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, &randomTailDropMasks);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev,
                                    randomTailDropMasks.globalRandomTailDropMask,
                                    randomTailDropMasks.portRandomTailDropMask,
                                    randomTailDropMasks.priorityRandomTailDropMask);

        st = cpssExMxPmPortTxRandomTailDropMasksGet(dev, &randomTailDropMasksGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxRandomTailDropMasksGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.globalRandomTailDropMask,
            randomTailDropMasksGet.globalRandomTailDropMask,
                   "get another globalRandomTailDropMask than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.portRandomTailDropMask,
            randomTailDropMasksGet.portRandomTailDropMask,
                   "get another portRandomTailDropMask than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.priorityRandomTailDropMask,
            randomTailDropMasksGet.priorityRandomTailDropMask,
                   "get another priorityRandomTailDropMask than was set: %d", dev);

        /* Call with triple {255,255,255} */
        randomTailDropMasks.globalRandomTailDropMask    = 255;
        randomTailDropMasks.portRandomTailDropMask      = 255;
        randomTailDropMasks.priorityRandomTailDropMask  = 255;

        st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, &randomTailDropMasks);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev,
                                    randomTailDropMasks.globalRandomTailDropMask,
                                    randomTailDropMasks.portRandomTailDropMask,
                                    randomTailDropMasks.priorityRandomTailDropMask);

        st = cpssExMxPmPortTxRandomTailDropMasksGet(dev, &randomTailDropMasksGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmPortTxRandomTailDropMasksGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.globalRandomTailDropMask,
            randomTailDropMasksGet.globalRandomTailDropMask,
                   "get another globalRandomTailDropMask than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.portRandomTailDropMask,
            randomTailDropMasksGet.portRandomTailDropMask,
                   "get another portRandomTailDropMask than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            randomTailDropMasks.priorityRandomTailDropMask,
            randomTailDropMasksGet.priorityRandomTailDropMask,
                   "get another priorityRandomTailDropMask than was set: %d", dev);

        /*
            1.2. Call with masks triples {256,0,0), (0,256,0}, (0,0,256}
            Expected: NOT GT_OK.
        */

        /* Call with triple {256,0,0} */
        randomTailDropMasks.globalRandomTailDropMask    = 256;
        randomTailDropMasks.portRandomTailDropMask      = 0;
        randomTailDropMasks.priorityRandomTailDropMask  = 0;

        st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, &randomTailDropMasks);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, globalRandomTailDropMask = %d",
                                         dev, randomTailDropMasks.globalRandomTailDropMask);

        /* Call with triple {0,256,0} */
        randomTailDropMasks.globalRandomTailDropMask    = 0;
        randomTailDropMasks.portRandomTailDropMask      = 256;
        randomTailDropMasks.priorityRandomTailDropMask  = 0;

        st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, &randomTailDropMasks);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, portRandomTailDropMask = %d",
                                         dev, randomTailDropMasks.portRandomTailDropMask);

        /* Call with triple {0,0,256} */
        randomTailDropMasks.globalRandomTailDropMask    = 0;
        randomTailDropMasks.portRandomTailDropMask      = 0;
        randomTailDropMasks.priorityRandomTailDropMask  = 256;

        st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, &randomTailDropMasks);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, priorityRandomTailDropMask = %d",
                                         dev, randomTailDropMasks.priorityRandomTailDropMask);

        /*
            1.3. Call with NULL pointer
            Expected: NOT GT_OK.
        */

        randomTailDropMasks.globalRandomTailDropMask    = 0;
        randomTailDropMasks.portRandomTailDropMask      = 0;
        randomTailDropMasks.priorityRandomTailDropMask  = 0;

        st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, randomTailDropMasksPtr = NULL", dev);
    }

    randomTailDropMasks.globalRandomTailDropMask    = 0;
    randomTailDropMasks.portRandomTailDropMask      = 0;
    randomTailDropMasks.priorityRandomTailDropMask  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_NOT_APPLICABLE_DEVICE.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, &randomTailDropMasks);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxRandomTailDropMasksSet(dev, &randomTailDropMasks);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxRandomTailDropMasksGet
(
    IN   GT_U8   devNum,
    OUT  CPSS_EXMXPM_PORT_TX_RANDOM_TAIL_DROP_MASKS_STC  *randomTailDropMasksPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxRandomTailDropMasksGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL randomTailDropMasksPtr.
    Expected: GT_OK.
    1.2. Call with NULL randomTailDropMasksPtr.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PORT_TX_RANDOM_TAIL_DROP_MASKS_STC  randomTailDropMasks;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL randomTailDropMasksPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxRandomTailDropMasksGet(dev, &randomTailDropMasks);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with NULL randomTailDropMasksPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxRandomTailDropMasksGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, randomTailDropMasksPtr = NULL", dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_NOT_APPLICABLE_DEVICE.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxRandomTailDropMasksGet(dev, &randomTailDropMasks);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxRandomTailDropMasksGet(dev, &randomTailDropMasks);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxShaperTokenBucketMtuSet
(
    IN GT_U8                                    devNum,
    IN CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxShaperTokenBucketMtuSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mtu [CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E,
                        CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_2K_E,
                        CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_10K_E].
    Expected: GT_OK.
    1.2. Call with cpssExMxPmPortTxShaperTokenBucketMtuGet mtuPtr.
    Expected: GT_OK.
    1.3. Call with mtu [0x5AAAAAA5] (out of range)
    Expected: GT_BAD_PARAM.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E;
    CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_ENT mtuGet = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mtu [CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E,
                                CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_2K_E,
                                CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_10K_E].
            Expected: GT_OK.
        */
        /* iterate with mtu = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E */
        mtu = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E;

        st = cpssExMxPmPortTxShaperTokenBucketMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtu);

        /*
            1.2. Call with cpssExMxPmPortTxShaperTokenBucketMtuGet mtuPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxShaperTokenBucketMtuGet(dev, &mtuGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPortTxShaperTokenBucketMtuGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mtu, mtuGet, "got another mtu than was set: %d", dev);

        /* iterate with mtu = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_2K_E */
        mtu = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_2K_E;

        st = cpssExMxPmPortTxShaperTokenBucketMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtu);

        /*
            1.2. Call with cpssExMxPmPortTxShaperTokenBucketMtuGet mtuPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxShaperTokenBucketMtuGet(dev, &mtuGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPortTxShaperTokenBucketMtuGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mtu, mtuGet, "got another mtu than was set: %d", dev);

        /* iterate with mtu = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_10K_E */
        mtu = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_10K_E;

        st = cpssExMxPmPortTxShaperTokenBucketMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtu);

        /*
            1.2. Call with cpssExMxPmPortTxShaperTokenBucketMtuGet mtuPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortTxShaperTokenBucketMtuGet(dev, &mtuGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPortTxShaperTokenBucketMtuGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mtu, mtuGet, "got another mtu than was set: %d", dev);

        /*
            1.3. Call with mtu [0x5AAAAAA5] (out of range)
            Expected: GT_BAD_PARAM.
        */
        mtu = PORT_TX_INVALID_ENUM_CNS;

        st = cpssExMxPmPortTxShaperTokenBucketMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mtu);
    }

    mtu = CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_10K_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxShaperTokenBucketMtuSet(dev, mtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxShaperTokenBucketMtuSet(dev, mtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxShaperTokenBucketMtuGet
(
    IN GT_U8                                    devNum,
    IN CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxShaperTokenBucketMtuGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with mtu [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu;

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
        st = cpssExMxPmPortTxShaperTokenBucketMtuGet(dev, &mtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null mtu [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortTxShaperTokenBucketMtuGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mtu = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxShaperTokenBucketMtuGet(dev, &mtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxShaperTokenBucketMtuGet(dev, &mtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmPortTx suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmPortTx)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueBlockEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueBlockEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueTxEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueTxEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueuesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxShaperEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxShaperEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxShaperProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxShaperProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueShaperEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueShaperEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueShaperProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxQueueShaperProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxShaperParamsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxShaperParamsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxToCpuShaperModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxToCpuShaperModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxTailDropUcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxTailDropUcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxBufferTailDropEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxBufferTailDropEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxTailDropProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxTailDropProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxProfileTailDropLimitsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxProfileTailDropLimitsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxTailDropE2eLimitsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxTailDropE2eLimitsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxTailDropProfileIndexSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxTailDropProfileIndexGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxMcPacketDescriptorsLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxMcPacketDescriptorsLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSchedulerConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSchedulerConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSchedulerProfileIndexSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSchedulerProfileIndexGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSchedulerWrrProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSchedulerWrrProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSchedulerArbitrationGroupSet) 
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSchedulerArbitrationGroupGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSharingResourcesEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSharingResourcesEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSharedResourcesLimitsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxSharedResourcesLimitsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxRandomTailDropMasksSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxRandomTailDropMasksGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxShaperTokenBucketMtuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxShaperTokenBucketMtuGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmPortTx)

