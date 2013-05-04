/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmPortCtrlUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmPortCtrl, that provides
*       ExMxPm CPSS implementation for Port configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define PORT_CTRL_VALID_PHY_PORT_CNS  0

/* Invalid enum */
#define PORT_CTRL_INVALID_ENUM_CNS    0x5AAAAAA5

#define IS_PORT_XG_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType==PRV_CPSS_PORT_XG_E)

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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

            st = cpssExMxPmPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortEnableGet: %d, %d", dev, port);

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
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortDuplexModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortDuplexModeSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with dMode [CPSS_PORT_FULL_DUPLEX_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortDuplexModeGet
    Expected: GT_OK and the same dMode.
    1.1.3. Call with dMode [CPSS_PORT_HALF_DUPLEX_E].
    Expected: GT_OK for non 10Gbps ports and NOT GT_OK for 10Gbps ports.
    1.1.4. For non 10Gbps ports call cpssExMxPmPortDuplexModeGet
    Expected: GT_OK and the same dMode.
    1.1.5. Call with out of range dMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_DUPLEX_ENT   mode     = CPSS_PORT_FULL_DUPLEX_E;
    CPSS_PORT_DUPLEX_ENT   modeGet  = CPSS_PORT_FULL_DUPLEX_E;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);


        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);


            /* make duplex auto-neg disable */
            st = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, GT_FALSE);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortDuplexAutoNegEnableSet: %d, %d", dev, port);
            /*
                1.1.1. Call with dMode [CPSS_PORT_FULL_DUPLEX_E].
                Expected: GT_OK.
            */
            mode = CPSS_PORT_FULL_DUPLEX_E;

            st = cpssExMxPmPortDuplexModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

            /*
                1.1.2. Call cpssExMxPmPortDuplexModeGet
                Expected: GT_OK and the same dMode.
            */
            st = cpssExMxPmPortDuplexModeGet(dev, port, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortDuplexModeGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with dMode [CPSS_PORT_HALF_DUPLEX_E].
                Expected: GT_OK for non 10Gpbs ports and NOT GT_OK otherwise.
            */
            mode = CPSS_PORT_HALF_DUPLEX_E;

            st = cpssExMxPmPortDuplexModeSet(dev, port, mode);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }

            /*
                1.1.4. For non 10Gbps ports call cpssExMxPmPortDuplexModeGet
                Expected: GT_OK and the same dMode.
            */
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                /*
                    1.1.2. Call cpssExMxPmPortDuplexModeGet
                    Expected: GT_OK and the same dMode.
                */
                st = cpssExMxPmPortDuplexModeGet(dev, port, &modeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortDuplexModeGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                           "get another mode than was set: %d, %d", dev, port);
            }

            /*
                1.1.2. Call with out of range dMode [0x5AAAAAA5]
                Expected: GT_BAD_PARAM
            */
            mode = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmPortDuplexModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mode);
        }

        mode = CPSS_PORT_FULL_DUPLEX_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortDuplexModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortDuplexModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortDuplexModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    mode = CPSS_PORT_FULL_DUPLEX_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortDuplexModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortDuplexModeSet(dev, port, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortDuplexModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortDuplexModeGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null dModePtr.
    Expected: GT_OK.
    1.1.2. Call with dModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_DUPLEX_ENT   mode = CPSS_PORT_FULL_DUPLEX_E;

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
                1.1.1. Call with non-NULL dModePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortDuplexModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with dModePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortDuplexModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, dModePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortDuplexModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortDuplexModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortDuplexModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortDuplexModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortDuplexModeGet(dev, port, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortSpeedSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with speed [CPSS_PORT_SPEED_10_E /
                            CPSS_PORT_SPEED_100_E /
                            CPSS_PORT_SPEED_1000_E].
    Expected: GT_OK for non-XG ports and NOT GT_OK for others.
    1.1.2. Call with speed [CPSS_PORT_SPEED_10000_E /
                            CPSS_PORT_SPEED_12000_E /
                            CPSS_PORT_SPEED_2500_E /
                            CPSS_PORT_SPEED_5000_E].
    Expected: GT_OK for XG ports and NOT GT_OK for others.
    1.1.3. Call cpssExMxPmPortSpeedGet
    Expected: GT_OK and the same speed.
    1.1.4. Call with speed [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_SPEED_ENT    speed;
    CPSS_PORT_SPEED_ENT    speedGet;

    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /* make speed auto-negotiation disable */
            st = cpssExMxPmPortSpeedAutoNegEnableSet (dev, port, GT_FALSE);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortSpeedAutoNegEnableSet: %d, %d", dev, port);

            /*
                1.1.1. Call with speed [CPSS_PORT_SPEED_10_E /
                                        CPSS_PORT_SPEED_100_E /
                                        CPSS_PORT_SPEED_1000_E].
                Expected: GT_OK for non-XG ports and NOT GT_OK for others.
            */

            /* Call with speed [CPSS_PORT_SPEED_10_E] */
            speed = CPSS_PORT_SPEED_10_E;

            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);

                /*
                    1.1.3. Call cpssExMxPmPortSpeedGet
                    Expected: GT_OK and the same speed.
                */
                st = cpssExMxPmPortSpeedGet(dev, port, &speedGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortSpeedGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                           "get another speed than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /* Call with speed [CPSS_PORT_SPEED_100_E] */
            speed = CPSS_PORT_SPEED_100_E;

            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);

                /*
                    1.1.3. Call cpssExMxPmPortSpeedGet
                    Expected: GT_OK and the same speed.
                */
                st = cpssExMxPmPortSpeedGet(dev, port, &speedGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortSpeedGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                           "get another speed than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /* Call with speed [CPSS_PORT_SPEED_1000_E] */
            speed = CPSS_PORT_SPEED_1000_E;

            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);

                /*
                    1.1.3. Call cpssExMxPmPortSpeedGet
                    Expected: GT_OK and the same speed.
                */
                st = cpssExMxPmPortSpeedGet(dev, port, &speedGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortSpeedGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                           "get another speed than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /*
                1.1.2. Call with speed [CPSS_PORT_SPEED_10000_E /
                                        CPSS_PORT_SPEED_12000_E /
                                        CPSS_PORT_SPEED_2500_E /
                                        CPSS_PORT_SPEED_5000_E].
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */

            /* Call with speed [CPSS_PORT_SPEED_10000_E] */
            speed = CPSS_PORT_SPEED_10000_E;

            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);

                /*
                    1.1.3. Call cpssExMxPmPortSpeedGet
                    Expected: GT_OK and the same speed.
                */
                st = cpssExMxPmPortSpeedGet(dev, port, &speedGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortSpeedGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                           "get another speed than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /* Call with speed [CPSS_PORT_SPEED_12000_E] */
            speed = CPSS_PORT_SPEED_12000_E;

            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);

                /*
                    1.1.3. Call cpssExMxPmPortSpeedGet
                    Expected: GT_OK and the same speed.
                */
                st = cpssExMxPmPortSpeedGet(dev, port, &speedGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortSpeedGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                           "get another speed than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /* Call with speed [CPSS_PORT_SPEED_2500_E] */
            speed = CPSS_PORT_SPEED_2500_E;

            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);

                /*
                    1.1.3. Call cpssExMxPmPortSpeedGet
                    Expected: GT_OK and the same speed.
                */
                st = cpssExMxPmPortSpeedGet(dev, port, &speedGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortSpeedGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                           "get another speed than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /* Call with speed [CPSS_PORT_SPEED_5000_E] */
            speed = CPSS_PORT_SPEED_5000_E;

            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);

                /*
                    1.1.3. Call cpssExMxPmPortSpeedGet
                    Expected: GT_OK and the same speed.
                */
                st = cpssExMxPmPortSpeedGet(dev, port, &speedGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortSpeedGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                           "get another speed than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /*
                1.1.4. Call with speed [0x5AAAAAA5]
                Expected: GT_BAD_PARAM.
            */
            speed = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, speed);
        }

        speed = CPSS_PORT_SPEED_10_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortSpeedSet(dev, port, speed);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortSpeedSet(dev, port, speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortSpeedSet(dev, port, speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    speed = CPSS_PORT_SPEED_10_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortSpeedSet(dev, port, speed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortSpeedSet(dev, port, speed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortSpeedGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null speedPtr.
    Expected: GT_OK.
    1.1.2. Call with speedPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_SPEED_ENT   speed = CPSS_PORT_SPEED_10_E;

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
                1.1.1. Call with non-NULL speedPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortSpeedGet(dev, port, &speed);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with speedPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortSpeedGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, speedPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortSpeedGet(dev, port, &speed);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortSpeedGet(dev, port, &speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortSpeedGet(dev, port, &speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortSpeedGet(dev, port, &speed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortSpeedGet(dev, port, &speed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortDuplexAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortDuplexAutoNegEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortDuplexAutoNegEnableGet.
    Expected: GT_OK and the same enable.
    1.1.3. Call with enable [GT_TRUE].
    Expected: GT_OK for non-XG ports and NOT GT_OK for others.
    1.1.4. For non-XG ports call cpssExMxPmPortDuplexAutoNegEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                enable    = GT_FALSE;
    GT_BOOL                enableGet = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with enable [GT_FALSE].
                Expected: GT_OK.
            */
            enable = GT_FALSE;

            st = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortDuplexAutoNegEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortDuplexAutoNegEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with enable [GT_TRUE].
                Expected: GT_OK for non-XG ports and NOT GT_OK for others.
            */
            enable = GT_TRUE;

            st = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, enable);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }

            /*
                1.1.4. For non-XG ports call cpssExMxPmPortDuplexAutoNegEnableGet.
                Expected: GT_OK and the same enable.
            */
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortDuplexAutoNegEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortDuplexAutoNegEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortDuplexAutoNegEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortDuplexAutoNegEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable,
    IN  GT_BOOL   pauseAdvertise
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlAutoNegEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE]
                     and pauseAdvertise [GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortFlowControlAutoNegEnableGet
    Expected: GT_OK and the same enable and pauseAdvertise.
    1.1.3. Call with enable [GT_TRUE / GT_FALSE / GT_TRUE]
                     and pauseAdvertise [GT_TRUE / TRUE / FALSE].
    Expected: GT_OK for non-XG ports and NOT GT_OK for others.
    1.1.4. For non-XG ports call cpssExMxPmPortFlowControlAutoNegEnableGet
    Expected: GT_OK and the same enable and pauseAdvertise.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                enable            = GT_FALSE;
    GT_BOOL                enableGet         = GT_FALSE;
    GT_BOOL                pauseAdvertise    = GT_FALSE;
    GT_BOOL                pauseAdvertiseGet = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType          = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with enable [GT_FALSE]
                                 and pauseAdvertise [GT_FALSE].
                Expected: GT_OK.
            */
            enable         = GT_FALSE;
            pauseAdvertise = GT_FALSE;

            st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, pauseAdvertise);

            /*
                1.1.2. Call cpssExMxPmPortFlowControlAutoNegEnableGet
                Expected: GT_OK and the same enable and pauseAdvertise.
            */
            st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enableGet, &pauseAdvertiseGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortFlowControlAutoNegEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(pauseAdvertise, pauseAdvertiseGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with enable [GT_TRUE / GT_FALSE / GT_TRUE]
                                 and pauseAdvertise [GT_TRUE / TRUE / FALSE].
                Expected: GT_OK for non-XG ports and NOT GT_OK for others.
            */

            /* Call with enable [GT_TRUE], pauseAdvertise [GT_TRUE] */
            enable         = GT_TRUE;
            pauseAdvertise = GT_TRUE;

            st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, pauseAdvertise);

                /*
                    1.1.4. For non-XG ports call cpssExMxPmPortFlowControlAutoNegEnableGet
                    Expected: GT_OK and the same enable and pauseAdvertise.
                */
                st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enableGet, &pauseAdvertiseGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortFlowControlAutoNegEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(pauseAdvertise, pauseAdvertiseGet,
                       "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, pauseAdvertise);
            }

            /* Call with enable [GT_FALSE], pauseAdvertise [GT_TRUE] */
            enable         = GT_FALSE;
            pauseAdvertise = GT_TRUE;

            st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, pauseAdvertise);

                /*
                    1.1.4. For non-XG ports call cpssExMxPmPortFlowControlAutoNegEnableGet
                    Expected: GT_OK and the same enable and pauseAdvertise.
                */
                st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enableGet, &pauseAdvertiseGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortFlowControlAutoNegEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(pauseAdvertise, pauseAdvertiseGet,
                       "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, pauseAdvertise);
            }

            /* Call with enable [GT_TRUE], pauseAdvertise [GT_FALSE] */
            enable         = GT_TRUE;
            pauseAdvertise = GT_FALSE;

            st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, pauseAdvertise);

                /*
                    1.1.4. For non-XG ports call cpssExMxPmPortFlowControlAutoNegEnableGet
                    Expected: GT_OK and the same enable and pauseAdvertise.
                */
                st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enableGet, &pauseAdvertiseGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortFlowControlAutoNegEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(pauseAdvertise, pauseAdvertiseGet,
                       "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, pauseAdvertise);
            }
        }

        enable         = GT_TRUE;
        pauseAdvertise = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable         = GT_TRUE;
    pauseAdvertise = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, enable, pauseAdvertise);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlAutoNegEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_BOOL   *pauseAdvertisePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlAutoNegEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr
                     and pauseAdvertisePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL]
                     and non-NULL pauseAdvertisePtr.
    Expected: GT_BAD_PTR.
    1.1.3. Call with non-NULL enablePtr
                     and pauseAdvertisePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL   enable         = GT_FALSE;
    GT_BOOL   pauseAdvertise = GT_FALSE;

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
                1.1.1. Call with non-null enablePtr
                                 and pauseAdvertisePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enable, &pauseAdvertise);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL]
                                 and non-NULL pauseAdvertisePtr.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, NULL, &pauseAdvertise);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);

            /*
                1.1.3. Call with non-NULL enablePtr
                                 and pauseAdvertisePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enable, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, pauseAdvertisePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enable, &pauseAdvertise);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enable, &pauseAdvertise);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enable, &pauseAdvertise);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enable, &pauseAdvertise);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlAutoNegEnableGet(dev, port, &enable, &pauseAdvertise);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortSpeedAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortSpeedAutoNegEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortSpeedAutoNegEnableGet.
    Expected: GT_OK and the same enable.
    1.1.3. Call with enable [GT_TRUE].
    Expected: GT_OK for non-XG ports and NOT GT_OK for others.
    1.1.4. For non-XG ports call cpssExMxPmPortSpeedAutoNegEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                enable    = GT_FALSE;
    GT_BOOL                enableGet = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with enable [GT_FALSE].
                Expected: GT_OK.
            */
            enable = GT_FALSE;

            st = cpssExMxPmPortSpeedAutoNegEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortSpeedAutoNegEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortSpeedAutoNegEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with enable [GT_TRUE].
                Expected: GT_OK for non-XG ports and NOT GT_OK for others.
            */
            enable = GT_TRUE;

            st = cpssExMxPmPortSpeedAutoNegEnableSet(dev, port, enable);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }

            /*
                1.1.4. For non-XG ports call cpssExMxPmPortSpeedAutoNegEnableGet.
                Expected: GT_OK and the same enable.
            */
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortSpeedAutoNegEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortSpeedAutoNegEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortSpeedAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortSpeedAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortSpeedAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortSpeedAutoNegEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortSpeedAutoNegEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortSpeedAutoNegEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortSpeedAutoNegEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortFlowControlEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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

            /* disable flow control auto-negotiation  */
            st = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, GT_FALSE, GT_FALSE);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortFlowControlAutoNegEnableSet: %d, %d", dev, port);
            /* provide full duplex */
            st = cpssExMxPmPortDuplexModeSet(dev, port, CPSS_PORT_FULL_DUPLEX_E);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortDuplexModeSet: %d, %d", dev, port);
            /*
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortFlowControlEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortFlowControlEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortFlowControlEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortFlowControlEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortFlowControlEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortFlowControlEnableGet: %d, %d", dev, port);

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
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortFlowControlEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortFlowControlEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXonPeriodicEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXonPeriodicEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortFlowControlXonPeriodicEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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

            st = cpssExMxPmPortFlowControlXonPeriodicEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortFlowControlXonPeriodicEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortFlowControlXonPeriodicEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortFlowControlXonPeriodicEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortFlowControlXonPeriodicEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortFlowControlXonPeriodicEnableGet: %d, %d", dev, port);

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
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlXonPeriodicEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlXonPeriodicEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlXonPeriodicEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXonPeriodicEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXonPeriodicEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXonPeriodicEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXonPeriodicEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXonPeriodicEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXoffPeriodicEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXoffPeriodicEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortFlowControlXoffPeriodicEnableGet.
    Expected: GT_OK and the same enable.
    1.1.3. Call with enable [GT_FALSE].
    Expected: GT_OK for XG ports and NOT GT_OK for others.
    1.1.4. For XG ports call cpssExMxPmPortFlowControlXoffPeriodicEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                enable    = GT_FALSE;
    GT_BOOL                enableGet = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with enable [GT_TRUE].
                Expected: GT_OK.
            */
            enable = GT_TRUE;

            st = cpssExMxPmPortFlowControlXoffPeriodicEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortFlowControlXoffPeriodicEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortFlowControlXoffPeriodicEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with enable [GT_FALSE].
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */
            enable = GT_FALSE;

            st = cpssExMxPmPortFlowControlXoffPeriodicEnableSet(dev, port, enable);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }

            /*
                1.1.4. For XG ports call cpssExMxPmPortFlowControlXoffPeriodicEnableGet.
                Expected: GT_OK and the same enable.
            */
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortFlowControlXoffPeriodicEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlXoffPeriodicEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlXoffPeriodicEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlXoffPeriodicEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXoffPeriodicEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXoffPeriodicEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlXoffPeriodicEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlXoffPeriodicEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortBackPressureEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortBackPressureEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK for non XG ports and NOT GT_OK for XG ports.
    1.1.2. For non-XG ports call cpssExMxPmPortBackPressureEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                enable    = GT_FALSE;
    GT_BOOL                enableGet = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK for non XG ports and NOT GT_OK for XG ports.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortBackPressureEnableSet(dev, port, enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. For non-XG ports call cpssExMxPmPortBackPressureEnableGet.
                    Expected: GT_OK and the same enable.
                */
                st = cpssExMxPmPortBackPressureEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortBackPressureEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortBackPressureEnableSet(dev, port, enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. For non-XG ports call cpssExMxPmPortBackPressureEnableGet.
                    Expected: GT_OK and the same enable.
                */
                st = cpssExMxPmPortBackPressureEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortBackPressureEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortBackPressureEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortBackPressureEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortBackPressureEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortBackPressureEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortBackPressureEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortBackPressureEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortBackPressureEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK for non XG ports and NOT GT_OK for XG ports.
    1.1.2. For non-XG ports call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                enable   = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK for non XG ports and NOT GT_OK for XG ports.
            */
            st = cpssExMxPmPortBackPressureEnableGet(dev, port, &enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. For non-XG ports call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                st = cpssExMxPmPortBackPressureEnableGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortBackPressureEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortBackPressureEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortBackPressureEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortBackPressureEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortBackPressureEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortLinkStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *isLinkUpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortLinkStatusGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null isLinkUpPtr.
    Expected: GT_OK.
    1.1.2. Call with isLinkUpPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL   isLinkUp = GT_FALSE;

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
                1.1.1. Call with non-NULL isLinkUpPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortLinkStatusGet(dev, port, &isLinkUp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with isLinkUpPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortLinkStatusGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, isLinkUpPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortLinkStatusGet(dev, port, &isLinkUp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortLinkStatusGet(dev, port, &isLinkUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortLinkStatusGet(dev, port, &isLinkUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortLinkStatusGet(dev, port, &isLinkUp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortLinkStatusGet(dev, port, &isLinkUp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInterfaceModeSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInterfaceModeSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with ifMode[CPSS_PORT_INTERFACE_MODE_SGMII_E /
                            CPSS_PORT_INTERFACE_MODE_1000BASE_X_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortInterfaceModeSet with not-NULL ifModePtr.
    Expected: GT_OK and the same ifMode as was set.
    1.1.3. Call with out of range ifMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_INTERFACE_MODE_ENT ifMode    = CPSS_PORT_INTERFACE_MODE_NA_E;
    CPSS_PORT_INTERFACE_MODE_ENT ifModeGet = CPSS_PORT_INTERFACE_MODE_NA_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* the Interface for XG port cannot be changed */
            if (!IS_PORT_XG_E(dev,port))
            {
                /*
                    1.1.1. Call with ifMode[CPSS_PORT_INTERFACE_MODE_SGMII_E /
                                            CPSS_PORT_INTERFACE_MODE_1000BASE_X_E].
                    Expected: GT_OK.
                */
                /* iterate with ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E */
                ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;

                st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);

                /*
                    1.1.2. Call cpssExMxPmPortInterfaceModeGet with not-NULL ifModePtr.
                    Expected: GT_OK and the same ifMode as was set.
                */
                st = cpssExMxPmPortInterfaceModeGet(dev, port, &ifModeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortInterfaceModeGet: %d, %d", dev, port);

                /* verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(ifMode, ifModeGet, "got another ifMode then was set: %d, %d", dev, port);

                /* iterate with ifMode = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E */
                ifMode = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;

                st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);

                /*
                    1.1.2. Call cpssExMxPmPortInterfaceModeGet with not-NULL ifModePtr.
                    Expected: GT_OK and the same ifMode as was set.
                */
                st = cpssExMxPmPortInterfaceModeGet(dev, port, &ifModeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortInterfaceModeGet: %d, %d", dev, port);

                /* verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(ifMode, ifModeGet, "got another ifMode then was set: %d, %d", dev, port);

                /*
                    1.1.3. Call with out of range ifMode [0x5AAAAAA5].
                    Expected: GT_BAD_PARAM.
                */
                ifMode = PORT_CTRL_INVALID_ENUM_CNS;

                st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, ifMode);
            }
            else
            {
                /* the Interface for XG port cannot be changed */
                /* ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E   */
                /* Expected: GT_BAD_PARAM                      */
                ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;

                st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, ifMode);

                /* ifMode = CPSS_PORT_INTERFACE_MODE_XGMII_E   */
                /* Expected: GT_OK                             */
                ifMode = CPSS_PORT_INTERFACE_MODE_XGMII_E;

                st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);
            }
        }

        ifMode = CPSS_PORT_INTERFACE_MODE_GMII_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInterfaceModeSet(dev, port, ifMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInterfaceModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInterfaceModeGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null ifModePtr.
    Expected: GT_OK.
    1.1.2. Call with ifModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_INTERFACE_MODE_ENT   ifMode = CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E;

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
                1.1.1. Call with non-NULL ifModePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with ifModePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortInterfaceModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ifModePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortInterfaceModeGet(dev, port, &ifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInterfaceModeGet(dev, port, &ifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInterfaceModeGet(dev, port, &ifMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInterfaceModeGet(dev, port, &ifMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortForceLinkUpSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   force
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortForceLinkUpSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with force [GT_FALSE and GT_TRUE]. Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortForceLinkUpGet.
    Expected: GT_OK and the same force.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL   force    = GT_FALSE;
    GT_BOOL   forceGet = GT_FALSE;

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
                1.1.1. Call with force [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with force [GT_FALSE] */
            force = GT_FALSE;

            st = cpssExMxPmPortForceLinkUpSet(dev, port, force);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, force);

            /*
                1.1.2. Call cpssExMxPmPortForceLinkUpGet.
                Expected: GT_OK and the same force.
            */
            st = cpssExMxPmPortForceLinkUpGet(dev, port, &forceGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortForceLinkUpGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(force, forceGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with force [GT_TRUE] */
            force = GT_TRUE;

            st = cpssExMxPmPortForceLinkUpSet(dev, port, force);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, force);

            /*
                1.1.2. Call cpssExMxPmPortForceLinkUpGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortForceLinkUpGet(dev, port, &forceGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortForceLinkUpGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(force, forceGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        force = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortForceLinkUpSet(dev, port, force);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortForceLinkUpSet(dev, port, force);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortForceLinkUpSet(dev, port, force);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    force = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortForceLinkUpSet(dev, port, force);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortForceLinkUpSet(dev, port, force);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortForceLinkUpGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *forcePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortForceLinkUpGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null forcePtr.
    Expected: GT_OK.
    1.1.2. Call with forcePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL   force = GT_FALSE;

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
                1.1.1. Call with non-NULL forcePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortForceLinkUpGet(dev, port, &force);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with forcePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortForceLinkUpGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, forcePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortForceLinkUpGet(dev, port, &force);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortForceLinkUpGet(dev, port, &force);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortForceLinkUpGet(dev, port, &force);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortForceLinkUpGet(dev, port, &force);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortForceLinkUpGet(dev, port, &force);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortForceLinkDownSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   force
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortForceLinkDownSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with force [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortForceLinkDownGet.
    Expected: GT_OK and the same force.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL   force    = GT_FALSE;
    GT_BOOL   forceGet = GT_FALSE;

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
                1.1.1. Call with force [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with force [GT_FALSE] */
            force = GT_FALSE;

            st = cpssExMxPmPortForceLinkDownSet(dev, port, force);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, force);

            /*
                1.1.2. Call cpssExMxPmPortForceLinkDownGet.
                Expected: GT_OK and the same force.
            */
            st = cpssExMxPmPortForceLinkDownGet(dev, port, &forceGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortForceLinkDownGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(force, forceGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with force [GT_TRUE] */
            force = GT_TRUE;

            st = cpssExMxPmPortForceLinkDownSet(dev, port, force);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, force);

            /*
                1.1.2. Call cpssExMxPmPortForceLinkDownGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortForceLinkDownGet(dev, port, &forceGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortForceLinkDownGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(force, forceGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        force = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortForceLinkDownSet(dev, port, force);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortForceLinkDownSet(dev, port, force);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortForceLinkDownSet(dev, port, force);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    force = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortForceLinkDownSet(dev, port, force);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortForceLinkDownSet(dev, port, force);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortForceLinkDownGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *forcePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortForceLinkDownGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null forcePtr.
    Expected: GT_OK.
    1.1.2. Call with forcePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL   force = GT_FALSE;

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
                1.1.1. Call with non-NULL forcePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortForceLinkDownGet(dev, port, &force);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with forcePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortForceLinkDownGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, forcePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortForceLinkDownGet(dev, port, &force);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortForceLinkDownGet(dev, port, &force);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortForceLinkDownGet(dev, port, &force);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortForceLinkDownGet(dev, port, &force);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortForceLinkDownGet(dev, port, &force);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMruSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum,
    IN  GT_U32 mruSize
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMruSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with mruSize [0 / 100 / 10304].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortMruGet.
    Expected: GT_OK and the same mruSize.
    1.1.3. Call with out of range mruSize [1 / 4095] (odd values are unsupported).
    Expected: NOT GT_OK.
    1.1.4. Call with out of range mruSize [16384].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U32      mruSize    = 0;
    GT_U32      mruSizeGet = 0;

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
                1.1.1. Call with mruSize [0 / 100 / 10304].
                Expected: GT_OK.
            */

            /* Call with mrySize [0] */
            mruSize = 0;

            st = cpssExMxPmPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);

            /*
                1.1.2. Call cpssExMxPmPortMruGet.
                Expected: GT_OK and the same mruSize.
            */
            st = cpssExMxPmPortMruGet(dev, port, &mruSizeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortMruGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mruSize, mruSizeGet,
                       "get another mruSize than was set: %d, %d", dev, port);

            /* Call with mrySize [100] */
            mruSize = 100;

            st = cpssExMxPmPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);

            /*
                1.1.2. Call cpssExMxPmPortMruGet.
                Expected: GT_OK and the same mruSize.
            */
            st = cpssExMxPmPortMruGet(dev, port, &mruSizeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortMruGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mruSize, mruSizeGet,
                       "get another mruSize than was set: %d, %d", dev, port);

            /* Call with mrySize [10304] */
            mruSize = 10304;

            st = cpssExMxPmPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);

            /*
                1.1.2. Call cpssExMxPmPortMruGet.
                Expected: GT_OK and the same mruSize.
            */
            st = cpssExMxPmPortMruGet(dev, port, &mruSizeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortMruGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mruSize, mruSizeGet,
                       "get another mruSize than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range mruSize [1 / 4095] (odd values are unsupported).
                Expected: NOT GT_OK.
            */

            /* Call with mrySize [1] */
            mruSize = 1;

            st = cpssExMxPmPortMruSet(dev, port, mruSize);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);

            /* Call with mrySize [4095] */
            mruSize = 4095;

            st = cpssExMxPmPortMruSet(dev, port, mruSize);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);

            /*
                1.1.4. Call with out of range mruSize [16384].
                Expected: NOT GT_OK.
            */
            mruSize = 16384;

            st = cpssExMxPmPortMruSet(dev, port, mruSize);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);
        }

        mruSize = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMruSet(dev, port, mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMruSet(dev, port, mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    mruSize = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMruSet(dev, port, mruSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortMruSet(dev, port, mruSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMruGet
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum,
    OUT GT_U32 *mruSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMruGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null mruSizePtr.
    Expected: GT_OK.
    1.1.2. Call with mruSizePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U32 mruSize = 0;

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
                1.1.1. Call with non-NULL mruSizePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortMruGet(dev, port, &mruSize);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with mruSizePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortMruGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mruSizePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMruGet(dev, port, &mruSize);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMruGet(dev, port, &mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMruGet(dev, port, &mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMruGet(dev, port, &mruSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortMruGet(dev, port, &mruSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortCrcCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortCrcCheckEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortCrcCheckEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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

            st = cpssExMxPmPortCrcCheckEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortCrcCheckEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortCrcCheckEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortCrcCheckEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortCrcCheckEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortCrcCheckEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortCrcCheckEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortCrcCheckEnableGet: %d, %d", dev, port);

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
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortCrcCheckEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortCrcCheckEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortCrcCheckEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortCrcCheckEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortCrcCheckEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortCrcCheckEnableGet
(
    IN   GT_U8       devNum,
    IN   GT_U8       portNum,
    OUT  GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortCrcCheckEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortCrcCheckEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortCrcCheckEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortCrcCheckEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortCrcCheckEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortCrcCheckEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortCrcCheckEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortCrcCheckEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxIpgModeSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  CPSS_PORT_XGMII_MODE_ENT       ipgMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxIpgModeSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with ipgMode [CPSS_PORT_XGMII_LAN_E /
                              CPSS_PORT_XGMII_WAN_E /
                              CPSS_PORT_XGMII_FIXED_E].
    Expected: GT_OK for XG ports and NON GT_OK for others.
    1.1.2. For XG ports call cpssExMxPmPortTxIpgModeGet.
    Expected: GT_OK and the same ipgMode.
    1.1.3. For XG-ports call with ipgMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_XGMII_MODE_ENT    ipgMode    = CPSS_PORT_XGMII_LAN_E;
    CPSS_PORT_XGMII_MODE_ENT    ipgModeGet = CPSS_PORT_XGMII_LAN_E;
    PRV_CPSS_PORT_TYPE_ENT      portType   = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with ipgMode [CPSS_PORT_XGMII_LAN_E /
                                          CPSS_PORT_XGMII_WAN_E /
                                          CPSS_PORT_XGMII_FIXED_E].
                Expected: GT_OK for XG ports and NON GT_OK for others.
            */

            /* Call with ipgMode [CPSS_PORT_XGMII_LAN_E] */
            ipgMode = CPSS_PORT_XGMII_LAN_E;

            st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgMode);

                /*
                    1.1.2. For XG ports call cpssExMxPmPortTxIpgModeGet.
                    Expected: GT_OK and the same ipgMode.
                */
                st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgModeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortTxIpgModeGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(ipgMode, ipgModeGet,
                           "get another ipgMode than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgMode);
            }

            /* Call with ipgMode [CPSS_PORT_XGMII_LAN_E] */
            ipgMode = CPSS_PORT_XGMII_LAN_E;

            st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgMode);

                /*
                    1.1.2. For XG ports call cpssExMxPmPortTxIpgModeGet.
                    Expected: GT_OK and the same mode.
                */
                st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgModeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortTxIpgModeGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(ipgMode, ipgModeGet,
                           "get another ipgMode than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgMode);
            }

            /* Call with ipgMode [PSS_PORT_XGMII_FIXED_E] */
            ipgMode = CPSS_PORT_XGMII_FIXED_E;

            st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgMode);

                /*
                    1.1.2. For XG ports call cpssExMxPmPortTxIpgModeGet.
                    Expected: GT_OK and the same mode.
                */
                st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgModeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortTxIpgModeGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(ipgMode, ipgModeGet,
                           "get another ipgMode than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgMode);
            }

            /*
                1.1.3. For XG-ports call with ipgMode [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                ipgMode = PORT_CTRL_INVALID_ENUM_CNS;

                st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, ipgMode);
            }
        }

        ipgMode = CPSS_PORT_XGMII_LAN_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    ipgMode = CPSS_PORT_XGMII_LAN_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxIpgModeSet(dev, port, ipgMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxIpgModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    OUT CPSS_PORT_XGMII_MODE_ENT       *ipgmodePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxIpgModeGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null ipgmodePtr.
    Expected: GT_OK for XG ports and NOT GT_OK for others.
    1.1.2. For XG ports call with ipgmodePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_XGMII_MODE_ENT    ipgMode  = CPSS_PORT_XGMII_LAN_E;
    PRV_CPSS_PORT_TYPE_ENT      portType = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with non-null ipgmodePtr.
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */
            st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgMode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. For XG ports call with ipgmodePtr [NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxPmPortTxIpgModeGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipgmodePtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxIpgModeGet(dev, port, &ipgMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFixedIpgBaseSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U32      ipgLength
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFixedIpgBaseSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with ipgLength [8 / 12].
    Expected: GT_OK for XG ports and NON GT_OK for others.
    1.1.2. For XG ports call cpssExMxPmPortFixedIpgBaseGet.
    Expected: GT_OK and the same ipgLength.
    1.1.3. For XG ports call with out of range ipgLength [0 / 9 / 15].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U32                  ipgLength = 0;
    GT_U32                  ipgLenGet = 0;
    PRV_CPSS_PORT_TYPE_ENT  portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with ipgLength [8 / 12].
                Expected: GT_OK for XG ports and NON GT_OK for others.
            */

            /* Call with ipgLength [8] */
            ipgLength = 8;

            st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgLength);

                /*
                    1.1.2. For XG ports call cpssExMxPmPortFixedIpgBaseGet.
                    Expected: GT_OK and the same ipgLength.
                */
                st = cpssExMxPmPortFixedIpgBaseGet(dev, port, &ipgLenGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortFixedIpgBaseGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(ipgLength, ipgLenGet,
                           "get another ipgLength than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgLength);
            }

            /* Call with ipgLength [12] */
            ipgLength = 12;

            st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgLength);

                /*
                    1.1.2. For XG ports call cpssExMxPmPortFixedIpgBaseGet.
                    Expected: GT_OK and the same ipgLength.
                */
                st = cpssExMxPmPortFixedIpgBaseGet(dev, port, &ipgLenGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortFixedIpgBaseGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(ipgLength, ipgLenGet,
                           "get another ipgLength than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgLength);
            }

            /*
                1.1.3. For XG ports call with out of range ipgLength [0 / 9 / 15].
                Expected: NOT GT_OK.
            */
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                /* Call with ipgLength [0] */
                ipgLength = 0;

                st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgLength);

                /* Call with ipgLength [9] */
                ipgLength = 9;

                st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgLength);

                /* Call with ipgLength [15] */
                ipgLength = 15;

                st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ipgLength);
            }
        }

        ipgLength = 8;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    ipgLength = 8;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFixedIpgBaseSet(dev, port, ipgLength);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFixedIpgBaseGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_U32      *ipgLengthPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFixedIpgBaseGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null ipgLengthPtr.
    Expected: GT_OK for XG ports and NON GT_OK for others.
    1.1.2. For XG ports call with ipgLengthPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U32                  ipgLength = 0;
    PRV_CPSS_PORT_TYPE_ENT  portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with non-null ipgLengthPtr.
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */
            st = cpssExMxPmPortFixedIpgBaseGet(dev, port, &ipgLength);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. For XG ports call with ipgLengthPtr [NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxPmPortFixedIpgBaseGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipgLengthPtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFixedIpgBaseGet(dev, port, &ipgLength);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortFixedIpgBaseGet(dev, port, &ipgLength);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFixedIpgBaseGet(dev, port, &ipgLength);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFixedIpgBaseGet(dev, port, &ipgLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFixedIpgBaseGet(dev, port, &ipgLength);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortExtraIpgSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U32      number
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortExtraIpgSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with number [0 / 1 / 3].
    Expected: GT_OK for XG ports and NON GT_OK for others.
    1.1.2. For XG ports call cpssExMxPmPortExtraIpgGet.
    Expected: GT_OK and the same number.
    1.1.3. For XG ports call with out of range number [4].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U32                  number    = 0;
    GT_U32                  numberGet = 0;
    PRV_CPSS_PORT_TYPE_ENT  portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with number [0 / 1 / 3].
                Expected: GT_OK for XG ports and NON GT_OK for others.
            */

            /* Call with number [0] */
            number = 0;

            st = cpssExMxPmPortExtraIpgSet(dev, port, number);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

                /*
                    1.1.2. For XG ports call cpssExMxPmPortExtraIpgGet.
                    Expected: GT_OK and the same number.
                */
                st = cpssExMxPmPortExtraIpgGet(dev, port, &numberGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortExtraIpgGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(number, numberGet,
                           "get another number than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);
            }

            /* Call with number [1] */
            number = 1;

            st = cpssExMxPmPortExtraIpgSet(dev, port, number);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

                /*
                    1.1.2. For XG ports call cpssExMxPmPortExtraIpgGet.
                    Expected: GT_OK and the same number.
                */
                st = cpssExMxPmPortExtraIpgGet(dev, port, &numberGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortExtraIpgGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(number, numberGet,
                           "get another number than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);
            }

            /* Call with number [3] */
            number = 3;

            st = cpssExMxPmPortExtraIpgSet(dev, port, number);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

                /*
                    1.1.2. For XG ports call cpssExMxPmPortExtraIpgGet.
                    Expected: GT_OK and the same number.
                */
                st = cpssExMxPmPortExtraIpgGet(dev, port, &numberGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortExtraIpgGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(number, numberGet,
                           "get another number than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);
            }

            /*
                1.1.3. For XG ports call with out of range number [4].
                Expected: NOT GT_OK.
            */
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                number = 4;

                st = cpssExMxPmPortExtraIpgSet(dev, port, number);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);
            }
        }

        number = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortExtraIpgSet(dev, port, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortExtraIpgSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortExtraIpgSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    number = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortExtraIpgSet(dev, port, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortExtraIpgSet(dev, port, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortExtraIpgGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_U32      *numberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortExtraIpgGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null numberPtr.
    Expected: GT_OK for XG ports and NON GT_OK for others.
    1.1.2. For XG ports call with numberPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U32                  number   = 0;
    PRV_CPSS_PORT_TYPE_ENT  portType = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with non-null numberPtr.
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */
            st = cpssExMxPmPortExtraIpgGet(dev, port, &number);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. For XG ports call with numberPtr [NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxPmPortExtraIpgGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, numberPtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortExtraIpgGet(dev, port, &number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortExtraIpgGet(dev, port, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortExtraIpgGet(dev, port, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortExtraIpgGet(dev, port, &number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortExtraIpgGet(dev, port, &number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortLocalFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isLocalFaultPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortLocalFaultGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null isLocalFaultPtr.
    Expected: GT_OK for XG ports and NON GT_OK for others.
    1.1.2. For XG ports call with isLocalFaultPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                 isLocalFault = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT  portType     = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with non-null isLocalFaultPtr.
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */
            st = cpssExMxPmPortLocalFaultGet(dev, port, &isLocalFault);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. For XG ports call with isLocalFaultPtr [NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxPmPortLocalFaultGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, isLocalFaultPtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortLocalFaultGet(dev, port, &isLocalFault);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortLocalFaultGet(dev, port, &isLocalFault);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortLocalFaultGet(dev, port, &isLocalFault);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortLocalFaultGet(dev, port, &isLocalFault);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortLocalFaultGet(dev, port, &isLocalFault);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortRemoteFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isRemoteFaultPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortRemoteFaultGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null isRemoteFaultPtr.
    Expected: GT_OK for XG ports and NON GT_OK for others.
    1.1.2. For XG ports call with isRemoteFaultPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                 isRemoteFault = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT  portType      = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with non-null isRemoteFaultPtr.
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */
            st = cpssExMxPmPortRemoteFaultGet(dev, port, &isRemoteFault);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. For XG ports call with isRemoteFaultPtr [NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxPmPortRemoteFaultGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, isRemoteFaultPtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortRemoteFaultGet(dev, port, &isRemoteFault);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortRemoteFaultGet(dev, port, &isRemoteFault);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortRemoteFaultGet(dev, port, &isRemoteFault);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortRemoteFaultGet(dev, port, &isRemoteFault);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortRemoteFaultGet(dev, port, &isRemoteFault);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacStatusGet
(
    IN  GT_U8                      devNum,
    IN  GT_U8                      portNum,
    OUT CPSS_PORT_MAC_STATUS_STC   *portMacStatusPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacStatusGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null portMacStatusPtr.
    Expected: GT_OK.
    1.1.2. Call with portMacStatusPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_MAC_STATUS_STC   portMacStatus;

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
                1.1.1. Call with non-NULL portMacStatusPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortMacStatusGet(dev, port, &portMacStatus);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with portMacStatusPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortMacStatusGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portMacStatusPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacStatusGet(dev, port, &portMacStatus);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacStatusGet(dev, port, &portMacStatus);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacStatusGet(dev, port, &portMacStatus);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacStatusGet(dev, port, &portMacStatus);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortMacStatusGet(dev, port, &portMacStatus);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInternalLoopbackEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInternalLoopbackEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortInternalLoopbackEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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

            st = cpssExMxPmPortInternalLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortInternalLoopbackEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortInternalLoopbackEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortInternalLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortInternalLoopbackEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortInternalLoopbackEnableGet: %d, %d", dev, port);

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
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortInternalLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortInternalLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInternalLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInternalLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInternalLoopbackEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInternalLoopbackEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInternalLoopbackEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInternalLoopbackEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInbandAutoNegEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInbandAutoNegEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK for non XG ports and NOT GT_OK for others.
    1.1.2. For non-XG ports call cpssExMxPmPortInbandAutoNegEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                 enable    = GT_FALSE;
    GT_BOOL                 enableGet = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT  portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK for non XG ports and NOT GT_OK for others.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortInbandAutoNegEnableSet(dev, port, enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssExMxPmPortInbandAutoNegEnableGet.
                    Expected: GT_OK and the same enable.
                */
                st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortInbandAutoNegEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }

            /* Call with enable [GT_TRUE] */
            st = cpssExMxPmPortInbandAutoNegEnableSet(dev, port, enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssExMxPmPortInbandAutoNegEnableGet.
                    Expected: GT_OK and the same enable.
                */
                st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortInbandAutoNegEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortInbandAutoNegEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortInbandAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInbandAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInbandAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInbandAutoNegEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInbandAutoNegEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInbandAutoNegEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK for non XG ports and NOT GT_OK for others.
    1.1.2. For non-XG ports call with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                 enable   = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT  portType = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with non-null enablePtr.
                Expected: GT_OK for non XG ports and NOT GT_OK for others.
            */
            st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, &enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. For non-XG ports call with enablePtr [NULL].
                    Expected: GT_BAD_PTR
                */
                st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInbandAutoNegEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortAttributesGet
(
    IN    GT_U8                     devNum,
    IN    GT_U8                     portNum,
    OUT   CPSS_PORT_ATTRIBUTES_STC  *portAttributesPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortAttributesGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null portAttributSetArrayPtr.
    Expected: GT_OK.
    1.1.2. Call with portAttributSetArrayPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_ATTRIBUTES_STC   portAttributes;

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
                1.1.1. Call with non-NULL portAttributesPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortAttributesGet(dev, port, &portAttributes);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with portAttributesPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortAttributesGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portAttributesPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortAttributesGet(dev, port, &portAttributes);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortAttributesGet(dev, port, &portAttributes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortAttributesGet(dev, port, &portAttributes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortAttributesGet(dev, port, &portAttributes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortAttributesGet(dev, port, &portAttributes);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortPreambleLengthSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with direction [CPSS_PORT_DIRECTION_TX_E]
                     and length [4 / 8]
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortPreambleLengthGet with the same direction.
    Expected: GT_OK and the same length.
    1.1.3. Call with direction [CPSS_PORT_DIRECTION_RX_E /
                                CPSS_PORT_DIRECTION_BOTH_E]
                     and length [4]
    Expected: GT_OK for XG ports and NOT GT_OK for others.
    1.1.4. Call with direction [CPSS_PORT_DIRECTION_TX_E]
                     and out of range length [1 / 5 / 10].
    Expected: NOT GT_OK.
    1.1.5. Call with out of range direction [0x5AAAAAA5]
                     and length [8]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U32                   length    = 0;
    GT_U32                   lengthGet = 0;
    PRV_CPSS_PORT_TYPE_ENT   portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with direction [CPSS_PORT_DIRECTION_TX_E]
                                 and length [4 / 8]
                Expected: GT_OK.
            */
            direction = CPSS_PORT_DIRECTION_TX_E;

            /* Call with length [4] */
            length = 4;

            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

            /*
                1.1.2. Call cpssExMxPmPortPreambleLengthGet with the same direction.
                Expected: GT_OK and the same length.
            */
            st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &lengthGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortPreambleLengthGet: %d, %d, %d", dev, port, direction);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(length, lengthGet,
                       "get another length than was set: %d, %d", dev, port);

            /* Call with length [8] */
            length = 8;

            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

            /*
                1.1.2. Call cpssExMxPmPortPreambleLengthGet with the same direction.
                Expected: GT_OK and the same length.
            */
            st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &lengthGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortPreambleLengthGet: %d, %d, %d", dev, port, direction);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(length, lengthGet,
                       "get another length than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with direction [CPSS_PORT_DIRECTION_RX_E /
                                            CPSS_PORT_DIRECTION_BOTH_E]
                                 and length [4]
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */
            length = 4;

            /* Call with direction [CPSS_PORT_DIRECTION_RX_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
            }

            /* Call with direction [CPSS_PORT_DIRECTION_BOTH_E] */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
            }

            /*
                1.1.4. Call with direction [CPSS_PORT_DIRECTION_TX_E]
                                 and out of range length [1 / 5 / 10].
                Expected: NOT GT_OK.
            */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            /* Call with length [1] */
            length = 1;

            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, length = %d", dev, port, length);

            /* Call with length [5] */
            length = 1;

            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, length = %d", dev, port, length);

            /* Call with length [10] */
            length = 1;

            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, length = %d", dev, port, length);

            length = 8;

            /*
                1.1.5. Call with out of range direction [0x5AAAAAA5]
                                 and length [8]
                Expected: GT_BAD_PARAM.
            */
            direction = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction);
        }

        direction = CPSS_PORT_DIRECTION_TX_E;
        length    = 4;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    direction = CPSS_PORT_DIRECTION_TX_E;
    length    = 4;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortPreambleLengthSet(dev, port, direction, length);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortPreambleLengthGet
(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_U32                  *lengthPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortPreambleLengthGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with direction [CPSS_PORT_DIRECTION_TX_E]
                     and non-NULL lengthPtr.
    Expected: GT_OK.
    1.1.2. Call with direction [CPSS_PORT_DIRECTION_RX_E /
                                CPSS_PORT_DIRECTION_BOTH_E]
                     and non-NULL lengthPtr.
    Expected: GT_OK for XG ports and NOT GT_OK for others.
    1.1.3. Call with out of range direction [0x5AAAAAA5]
                     and non-NULL lengthPtr.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with with direction [CPSS_PORT_DIRECTION_TX_E]
                     and lengthPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U32                   length    = 0;
    PRV_CPSS_PORT_TYPE_ENT   portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with direction [CPSS_PORT_DIRECTION_TX_E]
                                 and non-NULL lengthPtr.
                Expected: GT_OK.
            */
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);

            /*
                1.1.2. Call with direction [CPSS_PORT_DIRECTION_RX_E /
                                            CPSS_PORT_DIRECTION_BOTH_E]
                                 and non-NULL lengthPtr.
                Expected: GT_OK for XG ports and NOT GT_OK for others.
            */

            /* Call with direction [CPSS_PORT_DIRECTION_RX_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);
            }

            /* Call with direction [CPSS_PORT_DIRECTION_BOTH_E] */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);
            }

            /*
                1.1.3. Call with out of range direction [0x5AAAAAA5]
                                 and non-NULL lengthPtr.
                Expected: GT_BAD_PARAM.
            */
            direction = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction);

            direction = CPSS_PORT_DIRECTION_TX_E;

            /*
                1.1.4. Call with with direction [CPSS_PORT_DIRECTION_TX_E]
                                 and lengthPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, lengthPtr = NULL", dev, port);
        }

        direction = CPSS_PORT_DIRECTION_TX_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    direction = CPSS_PORT_DIRECTION_TX_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortPreambleLengthGet(dev, port, direction, &length);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacSaBaseSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with macPtr->arEther[0xFF, 0xFF, 0xFF, 0xAA, 0x0, 0x0].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPortMacSaBaseGet.
    Expected: GT_OK and the same macPtr.
    1.3. Call with macPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR mac;
    GT_ETHERADDR macGet;
    GT_BOOL      isEqual = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with macPtr->arEther[0xFF, 0xFF, 0xFF, 0xAA, 0x0, 0x0].
            Expected: GT_OK.
        */
        mac.arEther[0] = 0xFF;
        mac.arEther[1] = 0xFF;
        mac.arEther[2] = 0xFF;
        mac.arEther[3] = 0xAA;
        mac.arEther[4] = 0x0;

        st = cpssExMxPmPortMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmPortMacSaBaseGet.
            Expected: GT_OK and the same macPtr.
        */
        st = cpssExMxPmPortMacSaBaseGet(dev, &macGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortMacSaBaseGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mac,
                                     (GT_VOID*) &macGet,
                                     sizeof(mac.arEther[0] * 5))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macPtr than was set: %d", dev);

        /*
            1.3. Call with macPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortMacSaBaseSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macPtr = NULL", dev);
    }

    mac.arEther[0] = 0xFF;
    mac.arEther[1] = 0xFF;
    mac.arEther[2] = 0xFF;
    mac.arEther[3] = 0xAA;
    mac.arEther[4] = 0x0;
    mac.arEther[5] = 0x0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortMacSaBaseSet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacSaBaseGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL macPtr.
    Expected: GT_OK.
    1.2. Call with macPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR mac;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL macPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPortMacSaBaseGet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with macPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPortMacSaBaseGet(dev, NULL);
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
        st = cpssExMxPmPortMacSaBaseGet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortMacSaBaseGet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacSaLsbSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U8    macSaLsb
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacSaLsbSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with macSaLsb [0 / 0xFF].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortMacSaLsbGet.
    Expected: GT_OK and the same macSaLsb.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U8       macSaLsb    = 0;
    GT_U8       macSaLsbGet = 0;

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
                1.1.1. Call with macSaLsb [0 / 0xFF].
                Expected: GT_OK.
            */

            /* Call with macSaLsb [0] */
            macSaLsb = 0;

            st = cpssExMxPmPortMacSaLsbSet(dev, port, macSaLsb);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, macSaLsb);

            /*
                1.1.2. Call cpssExMxPmPortMacSaLsbGet.
                Expected: GT_OK and the same macSaLsb.
            */
            st = cpssExMxPmPortMacSaLsbGet(dev, port, &macSaLsbGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortMacSaLsbGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(macSaLsb, macSaLsbGet,
                       "get another macSaLsb than was set: %d, %d", dev, port);

            /* Call with macSaLsb [0xFF] */
            macSaLsb = 0xFF;

            st = cpssExMxPmPortMacSaLsbSet(dev, port, macSaLsb);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, macSaLsb);

            /*
                1.1.2. Call cpssExMxPmPortMacSaLsbGet.
                Expected: GT_OK and the same macSaLsb.
            */
            st = cpssExMxPmPortMacSaLsbGet(dev, port, &macSaLsbGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortMacSaLsbGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(macSaLsb, macSaLsbGet,
                       "get another macSaLsb than was set: %d, %d", dev, port);
        }

        macSaLsb = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacSaLsbSet(dev, port, macSaLsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacSaLsbSet(dev, port, macSaLsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacSaLsbSet(dev, port, macSaLsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    macSaLsb = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacSaLsbSet(dev, port, macSaLsb);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortMacSaLsbSet(dev, port, macSaLsb);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacSaLsbGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U8    *macSaLsbPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacSaLsbGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null macSaLsbPtr.
    Expected: GT_OK.
    1.1.2. Call with macSaLsbPtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U8       macSaLsb = 0;

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
                1.1.1. Call with non-NULL macSaLsbPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortMacSaLsbGet(dev, port, &macSaLsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with macSaLsbPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortMacSaLsbGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, macSaLsbPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacSaLsbGet(dev, port, &macSaLsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacSaLsbGet(dev, port, &macSaLsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacSaLsbGet(dev, port, &macSaLsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacSaLsbGet(dev, port, &macSaLsb);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortMacSaLsbGet(dev, port, &macSaLsb);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxPaddingEnableSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN GT_BOOL                  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxPaddingEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortTxPaddingEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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

            st = cpssExMxPmPortTxPaddingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxPaddingEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxPaddingEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxPaddingEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortTxPaddingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortTxPaddingEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortTxPaddingEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortTxPaddingEnableGet: %d, %d", dev, port);

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
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortTxPaddingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxPaddingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxPaddingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxPaddingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxPaddingEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortTxPaddingEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    OUT GT_BOOL                 *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortTxPaddingEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortTxPaddingEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortTxPaddingEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortTxPaddingEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortTxPaddingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortTxPaddingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortTxPaddingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortTxPaddingEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortExcessiveCollisionDropEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortExcessiveCollisionDropEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK for non XG-ports and NOT GT_OK for others.
    1.1.2. For non XG-port call cpssExMxPmPortExcessiveCollisionDropEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                enable    = GT_FALSE;
    GT_BOOL                enableGet = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK for non XG-ports and NOT GT_OK for others.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortExcessiveCollisionDropEnableSet(dev, port, enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. For non XG-port call cpssExMxPmPortExcessiveCollisionDropEnableGet.
                    Expected: GT_OK and the same enable.
                */
                st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortExcessiveCollisionDropEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortExcessiveCollisionDropEnableSet(dev, port, enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. For non XG-port call cpssExMxPmPortExcessiveCollisionDropEnableGet.
                    Expected: GT_OK and the same enable.
                */
                st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmPortExcessiveCollisionDropEnableGet: %d, %d", dev, port);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortExcessiveCollisionDropEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortExcessiveCollisionDropEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortExcessiveCollisionDropEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortExcessiveCollisionDropEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortExcessiveCollisionDropEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortExcessiveCollisionDropEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortExcessiveCollisionDropEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK for non XG-ports and NOT GT_OK for others.
    1.1.2. For non XG-ports call with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL                enable   = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

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
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK for non XG ports and NOT GT_OK for XG ports.
            */
            st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, &enable);
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. For non-XG ports call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortExcessiveCollisionDropEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortPriorityFlowControlEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortPriorityFlowControlEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK
    1.1.2. Call cpssExMxPmPortPriorityFlowControlEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK
            */
            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssExMxPmPortPriorityFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortPriorityFlowControlEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortPriorityFlowControlEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);

            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssExMxPmPortPriorityFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortPriorityFlowControlEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortPriorityFlowControlEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortPriorityFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortPriorityFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortPriorityFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_FALSE;
    port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortPriorityFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortPriorityFlowControlEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortPriorityFlowControlEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortPriorityFlowControlEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null enablePtr.
                Expected: GT_OK
            */
            st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR
            */
            st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortPriorityFlowControlEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlPeriodicTypeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  CPSS_EXMXPM_PORT_FC_TYPE_ENT    fcPeriodicType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlPeriodicTypeSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with fcPeriodicType [CPSS_EXMXPM_PORT_FC_802_3X_E / CPSS_EXMXPM_PORT_FC_PER_PRIORITY_E].
    Expected: GT_OK, for Errata: GT_NOT_SUPPORTED.
    1.1.2. Call cpssExMxPmPortFlowControlPeriodicTypeGet.
    Expected: GT_OK and the same fcPeriodicType.
    1.1.3. Call with fcPeriodicType [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    CPSS_EXMXPM_PORT_FC_TYPE_ENT    fcPeriodicType    = CPSS_EXMXPM_PORT_FC_802_3X_E;
    CPSS_EXMXPM_PORT_FC_TYPE_ENT    fcPeriodicTypeGet = CPSS_EXMXPM_PORT_FC_802_3X_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with fcPeriodicType [CPSS_EXMXPM_PORT_FC_802_3X_E /
                                                 CPSS_EXMXPM_PORT_FC_PER_PRIORITY_E].
                Expected: GT_OK
            */
            /* iterate with fcPeriodicType = CPSS_EXMXPM_PORT_FC_802_3X_E */
            fcPeriodicType = CPSS_EXMXPM_PORT_FC_802_3X_E;

            st = cpssExMxPmPortFlowControlPeriodicTypeSet(dev, port, fcPeriodicType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcPeriodicType);

            /*
                1.1.2. Call cpssExMxPmPortFlowControlPeriodicTypeGet.
                Expected: GT_OK and the same fcPeriodicType.
            */
            st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, &fcPeriodicTypeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortFlowControlPeriodicTypeGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(fcPeriodicType, fcPeriodicTypeGet, "got another fcPeriodicType than was set: %d, %d", dev, port);

            /* iterate with fcPeriodicType = CPSS_EXMXPM_PORT_FC_PER_PRIORITY_E */
            /* Ref#: FE-8130
                Per Priority Flow Control packets are counted as 802.3x Flow Control packets */
            fcPeriodicType = CPSS_EXMXPM_PORT_FC_PER_PRIORITY_E;

            st = cpssExMxPmPortFlowControlPeriodicTypeSet(dev, port, fcPeriodicType);
             /* Ref#: FE-8130
                Per Priority Flow Control packets are counted as 802.3x Flow Control packets */
            if (GT_TRUE == PRV_CPSS_EXMXPM_ERRATA_GET_MAC(dev, PRV_CPSS_EXMXPM_FER_PERIODIC_FC_PER_PRIO_E))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, fcPeriodicType);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcPeriodicType);

                /*
                    1.1.2. Call cpssExMxPmPortFlowControlPeriodicTypeGet.
                    Expected: GT_OK and the same fcPeriodicType.
                */
                st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, &fcPeriodicTypeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortFlowControlPeriodicTypeGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(fcPeriodicType, fcPeriodicTypeGet, "got another fcPeriodicType than was set: %d, %d", dev, port);
            }

            /*
                1.1.3. Call with fcPeriodicType [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */
            fcPeriodicType = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPmPortFlowControlPeriodicTypeSet(dev, port, fcPeriodicType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, fcPeriodicType);
        }

        fcPeriodicType = CPSS_EXMXPM_PORT_FC_802_3X_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlPeriodicTypeSet(dev, port, fcPeriodicType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortFlowControlPeriodicTypeSet(dev, port, fcPeriodicType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlPeriodicTypeSet(dev, port, fcPeriodicType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }
    fcPeriodicType = CPSS_EXMXPM_PORT_FC_802_3X_E;
    port           = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlPeriodicTypeSet(dev, port, fcPeriodicType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlPeriodicTypeSet(dev, port, fcPeriodicType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortFlowControlPeriodicTypeGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_EXMXPM_PORT_FC_TYPE_ENT    *fcPeriodicTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortFlowControlPeriodicTypeGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with not NULL fcPeriodicTypePtr.
    Expected: GT_OK
    1.1.2. Call with fcPeriodicTypePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    CPSS_EXMXPM_PORT_FC_TYPE_ENT    fcPeriodicType    = CPSS_EXMXPM_PORT_FC_802_3X_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not NULL fcPeriodicTypePtr.
                Expected: GT_OK
            */
            st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, &fcPeriodicType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with fcPeriodicTypePtr [NULL].
                Expected: GT_BAD_PTR
            */
            st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, fcPeriodicTypePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, &fcPeriodicType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, &fcPeriodicType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, &fcPeriodicType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, &fcPeriodicType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortFlowControlPeriodicTypeGet(dev, port, &fcPeriodicType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortForwardFlowControlEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortForwardFlowControlEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK
    1.1.2. Call cpssExMxPmPortForwardFlowControlEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK
            */
            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssExMxPmPortForwardFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortForwardFlowControlEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortForwardFlowControlEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);

            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssExMxPmPortForwardFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortForwardFlowControlEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortForwardFlowControlEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortForwardFlowControlEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortForwardFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortForwardFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_FALSE;
    port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortForwardFlowControlEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortForwardFlowControlEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortForwardFlowControlEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortForwardFlowControlEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null enablePtr.
                Expected: GT_OK
            */
            st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR
            */
            st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortForwardFlowControlEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortSerdesLoopbackEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortSerdesLoopbackEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK
    1.1.2. Call cpssExMxPmPortSerdesLoopbackEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK
            */
            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssExMxPmPortSerdesLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortSerdesLoopbackEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortSerdesLoopbackEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);

            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssExMxPmPortSerdesLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortSerdesLoopbackEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortSerdesLoopbackEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortSerdesLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortSerdesLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortSerdesLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_FALSE;
    port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortSerdesLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortSerdesLoopbackEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortSerdesLoopbackEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortSerdesLoopbackEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null enablePtr.
                Expected: GT_OK
            */
            st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR
            */
            st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortSerdesLoopbackEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortXgPcsLanesSwapSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U32 txSerdesLaneArr[CPSS_EXMXPM_PORT_XG_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortXgPcsLanesSwapSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with txSerdesLaneArr [0,0 / 3,3].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortXgPcsLanesSwapGet with not NULL txSerdesLaneArr.
    Expected: GT_OK and the same txSerdesLaneArr as was set.
    1.1.3. Call with txSerdesLaneArr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     isEqual = GT_FALSE;

    GT_U32 txSerdesLaneArr[CPSS_EXMXPM_PORT_XG_LANES_NUM_CNS];
    GT_U32 txSerdesLaneArrGet[CPSS_EXMXPM_PORT_XG_LANES_NUM_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Supported for XAUI or HyperG.Stack ports only */
            if(PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType == PRV_CPSS_PORT_XG_E)
            {
                /*
                    1.1.1. Call with txSerdesLaneArr [0,0 / 3,3].
                    Expected: GT_OK.
                */
                /* iterate with 0 */
                txSerdesLaneArr[0] = 0;
                txSerdesLaneArr[1] = 0;
                txSerdesLaneArr[2] = 0;
                txSerdesLaneArr[3] = 0;
    
                st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, txSerdesLaneArr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    
                /*
                    1.1.2. Call cpssExMxPmPortXgPcsLanesSwapGet with not NULL txSerdesLaneArr.
                    Expected: GT_OK and the same txSerdesLaneArr as was set.
                */
                st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArrGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortXgPcsLanesSwapGet: %d, %d", dev, port);
    
                /* Verifying values */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &txSerdesLaneArr, (GT_VOID*) &txSerdesLaneArrGet,
                                             sizeof(txSerdesLaneArr))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another txSerdesLaneArr than was set: %d", dev);
    
                /* iterate with 3 */
                txSerdesLaneArr[0] = 3;
                txSerdesLaneArr[1] = 3;
                txSerdesLaneArr[2] = 3;
                txSerdesLaneArr[3] = 3;
    
                st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, txSerdesLaneArr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    
                /*
                    1.1.2. Call cpssExMxPmPortXgPcsLanesSwapGet with not NULL txSerdesLaneArr.
                    Expected: GT_OK and the same txSerdesLaneArr as was set.
                */
                st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArrGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPortXgPcsLanesSwapGet: %d, %d", dev, port);
    
                /* Verifying values */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &txSerdesLaneArr, (GT_VOID*) &txSerdesLaneArrGet,
                                             sizeof(txSerdesLaneArr))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another txSerdesLaneArr than was set: %d", dev);
    
                /*
                    1.1.3. Call with txSerdesLaneArr [NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, txSerdesLaneArr = NULL", dev, port);
            }
            else 
            {
                /* iterate with 0 */
                txSerdesLaneArr[0] = 0;
                txSerdesLaneArr[1] = 0;
                txSerdesLaneArr[2] = 0;
                txSerdesLaneArr[3] = 0;
    
                st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, txSerdesLaneArr);
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        txSerdesLaneArr[0] = 0;
        txSerdesLaneArr[1] = 0;
        txSerdesLaneArr[2] = 0;
        txSerdesLaneArr[3] = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, txSerdesLaneArr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, txSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, txSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    txSerdesLaneArr[0] = 0;
    txSerdesLaneArr[1] = 0;
    txSerdesLaneArr[2] = 0;
    txSerdesLaneArr[3] = 0;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, txSerdesLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortXgPcsLanesSwapSet(dev, port, txSerdesLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortXgPcsLanesSwapGet
(
    IN  GT_U8 devNum,
    IN  GT_U8 portNum,
    OUT GT_U32 txSerdesLaneArr[CPSS_EXMXPM_PORT_XG_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortXgPcsLanesSwapGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with not NULL txSerdesLaneArr [0,0 / 3,3].
    Expected: GT_OK.
    1.1.2. Call with txSerdesLaneArr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_U32 txSerdesLaneArr[CPSS_EXMXPM_PORT_XG_LANES_NUM_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Supported for XAUI or HyperG.Stack ports only */
            if(PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType == PRV_CPSS_PORT_XG_E)
            {
                /*
                    1.1.1. Call with not NULL txSerdesLaneArr.
                    Expected: GT_OK.
                */
                st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    
                /*
                    1.1.2. Call with txSerdesLaneArr [NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, txSerdesLaneArr = NULL", dev, port);
            }
            else
            {
                st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArr);
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortXgPcsLanesSwapGet(dev, port, txSerdesLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInbandAutoNegRestartSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInbandAutoNegRestartSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortInbandAutoNegRestartSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortInbandAutoNegRestartSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortInbandAutoNegRestartSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInbandAutoNegRestartSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInbandAutoNegRestartSet(dev, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInbandAutoNegRestartSet(dev, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortSerdesPowerStatusSet
(
    IN  GT_U8                   dev,
    IN  GT_U8                   port,
    IN  GT_BOOL                 powerUp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortSerdesPowerStatusSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with powerUp [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL   powerUp    = GT_FALSE;

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
                1.1.1. Call with powerUp [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */
            /* Call with powerUp [GT_FALSE] */
            powerUp = GT_FALSE;

            st = cpssExMxPmPortSerdesPowerStatusSet(dev, port, powerUp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, powerUp);

            /* Call with powerUp [GT_TRUE] */
            powerUp = GT_TRUE;

            st = cpssExMxPmPortSerdesPowerStatusSet(dev, port, powerUp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, powerUp);
        }

        powerUp = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortSerdesPowerStatusSet(dev, port, powerUp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortSerdesPowerStatusSet(dev, port, powerUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortSerdesPowerStatusSet(dev, port, powerUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    powerUp = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortSerdesPowerStatusSet(dev, port, powerUp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortSerdesPowerStatusSet(dev, port, powerUp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInbandAutoNegBypassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInbandAutoNegBypassEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortInbandAutoNegBypassEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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

            st = cpssExMxPmPortInbandAutoNegBypassEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortInbandAutoNegBypassEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortInbandAutoNegBypassEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortInbandAutoNegBypassEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortInbandAutoNegBypassEnableGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPortInbandAutoNegBypassEnableGet: %d, %d", dev, port);

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
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortInbandAutoNegBypassEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortInbandAutoNegBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInbandAutoNegBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInbandAutoNegBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInbandAutoNegBypassEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortInbandAutoNegBypassEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortInbandAutoNegBypassEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPortInbandAutoNegBypassEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmPortCtrl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmPortCtrl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortDuplexModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortDuplexModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortSpeedSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortSpeedGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortDuplexAutoNegEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortDuplexAutoNegEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlAutoNegEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlAutoNegEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortSpeedAutoNegEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortSpeedAutoNegEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXonPeriodicEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXonPeriodicEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXoffPeriodicEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlXoffPeriodicEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortBackPressureEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortBackPressureEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortLinkStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInterfaceModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInterfaceModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortForceLinkUpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortForceLinkUpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortForceLinkDownSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortForceLinkDownGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMruSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMruGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortCrcCheckEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortCrcCheckEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxIpgModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxIpgModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFixedIpgBaseSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFixedIpgBaseGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortExtraIpgSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortExtraIpgGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortLocalFaultGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortRemoteFaultGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInternalLoopbackEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInternalLoopbackEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInbandAutoNegEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInbandAutoNegEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortAttributesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortPreambleLengthSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortPreambleLengthGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacSaBaseSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacSaBaseGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacSaLsbSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacSaLsbGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxPaddingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortTxPaddingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortExcessiveCollisionDropEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortExcessiveCollisionDropEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortPriorityFlowControlEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortPriorityFlowControlEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlPeriodicTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortFlowControlPeriodicTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortForwardFlowControlEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortForwardFlowControlEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortSerdesLoopbackEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortSerdesLoopbackEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortXgPcsLanesSwapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortXgPcsLanesSwapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInbandAutoNegRestartSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortSerdesPowerStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInbandAutoNegBypassEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortInbandAutoNegBypassEnableGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmPortCtrl)

