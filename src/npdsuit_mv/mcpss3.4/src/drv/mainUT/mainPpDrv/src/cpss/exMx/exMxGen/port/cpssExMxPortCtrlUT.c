/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPortCtrlUT.c
*
* DESCRIPTION:
*       Unit Tests for Port configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortCtrl.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Default valid value for port id */
#define PORT_CTRL_VALID_PHY_PORT_CNS    0

/* Invalid enumeration value used for testing */
#define PORT_CTRL_INVALID_ENUM_CNS      0x5AAAAAA5

/* Macro to define is device of 98Ex136 type       */
/* devType  - type of device (CPSS_PP_DEVICE_TYPE) */
#define IS_98EX136_DEV_MAC(devType)                 \
                ((CPSS_98EX136_CNS == devType) ||   \
                 (CPSS_98EX136DI_CNS == devType))

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortMacSaLsbSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_U8    macSaLsb
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortMacSaLsbSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with macSaLsb [32]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   macSaLsb;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with macSaLsb [32]. Expected: GT_OK. */
            macSaLsb = 32;

            st = cpssExMxPortMacSaLsbSet(dev, port, macSaLsb);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, macSaLsb);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        macSaLsb = 32;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortMacSaLsbSet(dev, port, macSaLsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortMacSaLsbSet(dev, port, macSaLsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    macSaLsb = 32;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortMacSaLsbSet(dev, port, macSaLsb);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortMacSaLsbSet(dev, port, macSaLsb);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with state [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.1.2. Call cpssExMxPortEnableGet with non-null statePtr. Expected: GT_OK and the same state as written.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            GT_BOOL stateWritten;

            /* 1.1.1. Call with state [GT_TRUE and GT_FALSE].               */
            /* Expected: GT_OK.                                             */
            state = GT_TRUE;

            st = cpssExMxPortEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            state = GT_FALSE;

            st = cpssExMxPortEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* 1.1.2. Call cpssExMxPortEnableGet with non-null statePtr.    */
            /* Expected: GT_OK and the same state as written.               */
            st = cpssExMxPortEnableGet(dev, port, &stateWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPortEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(state, stateWritten, dev, port, state);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        state = GT_FALSE;

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortEnableGet
(
    IN   GT_U8     dev,
    IN   GT_U8     port,
    OUT  GT_BOOL  *statePtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortEnableGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null statePtr. Expected: GT_OK.
1.1.2. Call with statePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null statePtr. Expected: GT_OK. */
            st = cpssExMxPortEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* 1.1.2. Call with statePtr [NULL]. Expected: GT_BAD_PTR. */
            st = cpssExMxPortEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortDuplexModeSet
(
    IN  GT_U8                 dev,
    IN  GT_U8                 port,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortDuplexModeSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with dMode [CPSS_PORT_FULL_DUPLEX_E]. Expected: GT_OK.
1.1.2. Call cpssExMxPortDuplexModeGet with non-null dModePtr. Expected: GT_OK and full duplex mode.
1.1.3. Call with dMode [CPSS_PORT_HALF_DUPLEX_E]. Expected: GT_OK if port type isn't 10Gbps, GT_NOT_SUPPORTED if port type is 10Gbps.
1.1.4. Call with dMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port;
    CPSS_PORT_DUPLEX_ENT    dMode;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            CPSS_PORT_DUPLEX_ENT    dModeWritten;
            PRV_CPSS_PORT_TYPE_ENT  portType;

            /* 1.1.1. Call with dMode [CPSS_PORT_FULL_DUPLEX_E].    */
            /* Expected: GT_OK.                                     */
            dMode = CPSS_PORT_FULL_DUPLEX_E;

            st = cpssExMxPortDuplexModeSet(dev, port, dMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dMode);

            /* 1.1.2. Call cpssExMxPortDuplexModeGet with non-null dModePtr.    */
            /* Expected: GT_OK and full duplex mode.                            */
            st = cpssExMxPortDuplexModeGet(dev, port, &dModeWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPortDuplexModeGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(dMode, dModeWritten, dev, port, dMode);

            /* 1.1.3. Call with dMode [CPSS_PORT_HALF_DUPLEX_E].                                    */
            /* Expected: GT_OK if port type isn't 10Gbps, GT_NOT_SUPPORTED if port type is 10Gbps.  */
            dMode = CPSS_PORT_HALF_DUPLEX_E;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            st = cpssExMxPortDuplexModeSet(dev, port, dMode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, dMode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dMode);
            }

            /* 1.1.4. Call with dMode [0x5AAAAAA5].     */
            /* Expected: GT_BAD_PARAM.                  */
            dMode = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPortDuplexModeSet(dev, port, dMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, dMode);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        dMode = CPSS_PORT_FULL_DUPLEX_E;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortDuplexModeSet(dev, port, dMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortDuplexModeSet(dev, port, dMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    dMode = CPSS_PORT_FULL_DUPLEX_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortDuplexModeSet(dev, port, dMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortDuplexModeSet(dev, port, dMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortDuplexModeGet
(
    IN  GT_U8                 dev,
    IN  GT_U8                 port,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortDuplexModeGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null dModePtr. Expected: GT_OK and always full duplex value for 10Gbps port.
1.1.2. Call with dModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port;
    CPSS_PORT_DUPLEX_ENT    dMode;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;

            /* 1.1.1. Call with non-null dModePtr.                              */
            /* Expected: GT_OK and always full duplex value for 10Gbps port.    */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            dMode = CPSS_PORT_FULL_DUPLEX_E;

            st = cpssExMxPortDuplexModeGet(dev, port, &dMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dMode);

            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(CPSS_PORT_FULL_DUPLEX_E, dMode, dev, port);
            }

            /* 1.1.2. Call with dModePtr [NULL].    */
            /* Expected: GT_BAD_PTR.                */
            st = cpssExMxPortDuplexModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortDuplexModeGet(dev, port, &dMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortDuplexModeGet(dev, port, &dMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortDuplexModeGet(dev, port, &dMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortDuplexModeGet(dev, port, &dMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortSpeedSet
(
    IN  GT_U8                dev,
    IN  GT_U8                port,
    IN  CPSS_PORT_SPEED_ENT  speed
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortSpeedSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with speed [CPSS_PORT_SPEED_10_E/100_E]. Expected: GT_OK for non 10Gbps port and GT_NOT_SUPPORTED for 10Gbps port.
1.1.2. Call cpssExMxPortSpeedGet with non-null speedPtr. Expected: GT_OK and the same value as written for non 10Gbps ports and always CPSS_PORT_SPEED_10000_E] for 10Gbps port.
1.1.3. Call with speed [CPSS_PORT_SPEED_1000_E]. Expected: GT_OK for 1Gbps port and GT_NOT_SUPPORTED for non 1Gbps port.
1.1.4. Call with speed [CPSS_PORT_SPEED_10000_E]. Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.
1.1.5. Call with speed [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port;
    CPSS_PORT_SPEED_ENT     speed;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;
#ifndef ASIC_SIMULATION
/* currently simulation not support change of speed */
            CPSS_PORT_SPEED_ENT     speedWritten;
#endif /*ASIC_SIMULATION*/

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with speed [CPSS_PORT_SPEED_10_E/100_E].                         */
            /* Expected: GT_OK for non 10Gbps port and GT_NOT_SUPPORTED for 10Gbps port.    */
            speed = CPSS_PORT_SPEED_10_E;

            st = cpssExMxPortSpeedSet(dev, port, speed);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            speed = CPSS_PORT_SPEED_100_E;

            st = cpssExMxPortSpeedSet(dev, port, speed);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

#ifndef ASIC_SIMULATION
/* currently simulation not support change of speed */
            /* 1.1.2. Call cpssExMxPortSpeedGet with non-null speedPtr.                     */
            /* Expected: GT_OK and the same value as written for non 10Gbps ports and       */
            /* always CPSS_PORT_SPEED_10000_E] for 10Gbps port.                             */
            st = cpssExMxPortSpeedGet(dev, port, &speedWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPortSpeedGet: %d, %d", dev, port);

            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(CPSS_PORT_SPEED_10000_E, speedWritten, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(speed, speedWritten, dev, port, speed);
            }
#endif /*ASIC_SIMULATION*/

            /* 1.1.3. Call with speed [CPSS_PORT_SPEED_1000_E].                             */
            /* Expected: GT_OK for 1Gbps port and GT_NOT_SUPPORTED for non 1Gbps port.      */
            speed = CPSS_PORT_SPEED_1000_E;

            st = cpssExMxPortSpeedSet(dev, port, speed);
            if (PRV_CPSS_PORT_GE_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
            }

            /* 1.1.4. Call with speed [CPSS_PORT_SPEED_10000_E].                            */
            /* Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.    */
            speed = CPSS_PORT_SPEED_10000_E;

            st = cpssExMxPortSpeedSet(dev, port, speed);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
            }

            /* 1.1.5. Call with speed [0x5AAAAAA5].                                         */
            /* Expected: GT_BAD_PARAM.                                                      */
            speed = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPortSpeedSet(dev, port, speed);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, speed);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        speed = CPSS_PORT_SPEED_100_E;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortSpeedSet(dev, port, speed);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortSpeedSet(dev, port, speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    speed = CPSS_PORT_SPEED_100_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortSpeedSet(dev, port, speed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortSpeedSet(dev, port, speed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortSpeedGet
(
    IN  GT_U8                 dev,
    IN  GT_U8                 port,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortSpeedGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null speedPtr. Expected: GT_OK and always [CPSS_PORT_SPEED_10000_E] for 10Gbps port.
1.1.2. Call with speedPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port;
    CPSS_PORT_SPEED_ENT     speed;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with non-null speedPtr.                                      */
            /* Expected: GT_OK and always [CPSS_PORT_SPEED_10000_E] for 10Gbps port.    */
            st = cpssExMxPortSpeedGet(dev, port, &speed);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);

            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(CPSS_PORT_SPEED_10000_E, speed, dev, port);
            }

            /* 1.1.2. Call with speedPtr [NULL].                                        */
            /* Expected: GT_BAD_PTR.                                                    */
            st = cpssExMxPortSpeedGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortSpeedGet(dev, port, &speed);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortSpeedGet(dev, port, &speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortSpeedGet(dev, port, &speed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortSpeedGet(dev, port, &speed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortAttributesOnPortGet
(
    IN    GT_U8  dev,
    IN    GT_U8  port,
    OUT   CPSS_PORT_ATTRIBUTES_STC *portAttributesPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortAttributesOnPortGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null portAttributesPtr. Expected: GT_OK.
1.1.2. Call with portAttributesPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    GT_U8                       port;
    CPSS_PORT_ATTRIBUTES_STC    portAttributes;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null portAttributesPtr.     */
            /* Expected: GT_OK.                                 */
            st = cpssExMxPortAttributesOnPortGet(dev, port, &portAttributes);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with portAttributesPtr [NULL].       */
            /* Expected: GT_BAD_PTR.                            */
            st = cpssExMxPortAttributesOnPortGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortAttributesOnPortGet(dev, port, &portAttributes);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortAttributesOnPortGet(dev, port, &portAttributes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortAttributesOnPortGet(dev, port, &portAttributes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortAttributesOnPortGet(dev, port, &portAttributes);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortDuplexAutoNegEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortDuplexAutoNegEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with state [GT_TRUE]. Expected: GT_OK for non 10Gbps port and GT_NOT_SUPPORTED for 10Gbps port.
1.1.2. Call with state [GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with state [GT_TRUE and GT_FALSE].                               */
            /* Expected: GT_OK for non 10Gbps port and GT_NOT_SUPPORTED for 10Gbps port.    */
            state = GT_TRUE;

            st = cpssExMxPortDuplexAutoNegEnable(dev, port, state);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }

            /* 1.1.2. Call with state [GT_FALSE]. Expected: GT_OK. */
            state = GT_FALSE;

            st = cpssExMxPortDuplexAutoNegEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        state = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortDuplexAutoNegEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortDuplexAutoNegEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortDuplexAutoNegEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortDuplexAutoNegEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortFlowCntrlAutoNegEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state,
    IN  GT_BOOL   pauseAdvertise
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortFlowCntrlAutoNegEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with state [GT_TRUE and GT_FALSE], pauseAdvertise [GT_TRUE and GT_FALSE]. Expected: GT_OK for non 10Gbps and GT_NOT_SUPPORTED for 10Gbps.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     state;
    GT_BOOL     pauseAdvertise;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with state [GT_TRUE and GT_FALSE], pauseAdvertise [GT_TRUE and GT_FALSE].    */
            /* Expected: GT_OK for non 10Gbps and GT_NOT_SUPPORTED for 10Gbps.                          */
            state = GT_TRUE;
            pauseAdvertise = GT_TRUE;

            st = cpssExMxPortFlowCntrlAutoNegEnable(dev, port, state, pauseAdvertise);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state, pauseAdvertise);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, pauseAdvertise);
            }

            state = GT_FALSE;

            st = cpssExMxPortFlowCntrlAutoNegEnable(dev, port, state, pauseAdvertise);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state, pauseAdvertise);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, pauseAdvertise);
            }
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        state = GT_TRUE;
        pauseAdvertise = GT_TRUE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortFlowCntrlAutoNegEnable(dev, port, state, pauseAdvertise);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortFlowCntrlAutoNegEnable(dev, port, state, pauseAdvertise);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    state = GT_TRUE;
    pauseAdvertise = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortFlowCntrlAutoNegEnable(dev, port, state, pauseAdvertise);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortFlowCntrlAutoNegEnable(dev, port, state, pauseAdvertise);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortSpeedAutoNegEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortSpeedAutoNegEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with state [GT_TRUE]. Expected: GT_OK for non 10Gbps port and GT_NOT_SUPPORTED for 10Gbps port.
1.1.2. Call with state [GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with state [GT_TRUE].                               */
            /* Expected: GT_OK for non 10Gbps port and GT_NOT_SUPPORTED for 10Gbps port.    */
            state = GT_TRUE;

            st = cpssExMxPortSpeedAutoNegEnable(dev, port, state);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }

            /* 1.1.2. Call with state [GT_FALSE]. Expected: GT_OK. */
            state = GT_FALSE;

            st = cpssExMxPortSpeedAutoNegEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        state = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortSpeedAutoNegEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortSpeedAutoNegEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortSpeedAutoNegEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortSpeedAutoNegEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortFlowControlEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortFlowControlEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with state [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.1.2. Call cpssExMxPortFlowControlEnableGet with non-null statePtr. Expected: GT_OK and the same state as written.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            GT_BOOL stateWritten;

            /* 1.1.1. Call with state [GT_TRUE and GT_FALSE].                           */
            /* Expected: GT_OK.                                                         */
            state = GT_TRUE;

            st = cpssExMxPortFlowControlEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            state = GT_FALSE;

            st = cpssExMxPortFlowControlEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* 1.1.2. Call cpssExMxPortFlowControlEnableGet with non-null statePtr.     */
            /* Expected: GT_OK and the same state as written.                           */
            st = cpssExMxPortFlowControlEnableGet(dev, port, &stateWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPortFlowControlEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(state, stateWritten, dev, port, state);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        state = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortFlowControlEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortFlowControlEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortFlowControlEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortFlowControlEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortFlowControlEnableGet
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    OUT GT_BOOL   *statePtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortFlowControlEnableGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null statePtr. Expected: GT_OK.
1.1.2. Call with statePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null statePtr.      */
            /* Expected: GT_OK.                         */
            st = cpssExMxPortFlowControlEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* 1.1.2. Call with statePtr [NULL].        */
            /* Expected: GT_BAD_PTR.                    */
            st = cpssExMxPortFlowControlEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortFlowControlEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortFlowControlEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortFlowControlEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortFlowControlEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortPeriodicFcEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortPeriodicFcEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL enable;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with enable [GT_TRUE and GT_FALSE].      */
            /* Expected: GT_OK.                                     */
            enable = GT_TRUE;

            st = cpssExMxPortPeriodicFcEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_FALSE;

            st = cpssExMxPortPeriodicFcEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        enable = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortPeriodicFcEnable(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortPeriodicFcEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortPeriodicFcEnable(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortPeriodicFcEnable(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortBackPressureEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortBackPressureEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with state [GT_TRUE and GT_FALSE]. Expected: GT_OK for non 10Gbps port and GT_NOT_SUPPORTED for 10Gbps port.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with state [GT_TRUE and GT_FALSE].                               */
            /* Expected: GT_OK for non 10Gbps port and GT_NOT_SUPPORTED for 10Gbps port.    */
            state = GT_TRUE;

            st = cpssExMxPortBackPressureEnable(dev, port, state);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }

            state = GT_FALSE;

            st = cpssExMxPortBackPressureEnable(dev, port, state);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        state = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortBackPressureEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortBackPressureEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortBackPressureEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortBackPressureEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortLinkStatusGet
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    OUT GT_BOOL   *isLinkUpPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortLinkStatusGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null isLinkUpPtr. Expected: GT_OK.
1.1.2. Call with isLinkUpPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL isLinkUp;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null isLinkUpPtr.   */
            /* Expected: GT_OK.                         */
            st = cpssExMxPortLinkStatusGet(dev, port, &isLinkUp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, isLinkUp);

            /* 1.1.2. Call with isLinkUpPtr [NULL].     */
            /* Expected: GT_BAD_PTR.                    */
            st = cpssExMxPortLinkStatusGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortLinkStatusGet(dev, port, &isLinkUp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortLinkStatusGet(dev, port, &isLinkUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortLinkStatusGet(dev, port, &isLinkUp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortLinkStatusGet(dev, port, &isLinkUp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortInterfaceModeGet
(
    IN  GT_U8                          dev,
    IN  GT_U8                          port,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortInterfaceModeGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null ifModePtr. Expected: GT_OK.
1.1.2. Call with ifModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                           dev;
    GT_U8                           port;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null ifModePtr.     */
            /* Expected: GT_OK.                         */
            st = cpssExMxPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);

            /* 1.1.2. Call with ifModePtr [NULL].       */
            /* Expected: GT_BAD_PTR.                    */
            st = cpssExMxPortInterfaceModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortInterfaceModeGet(dev, port, &ifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortInterfaceModeGet(dev, port, &ifMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortInterfaceModeGet(dev, port, &ifMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortForceLinkDownEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortForceLinkDownEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with state [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                 */
            state = GT_TRUE;

            st = cpssExMxPortForceLinkDownEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            state = GT_FALSE;

            st = cpssExMxPortForceLinkDownEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        state = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortForceLinkDownEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortForceLinkDownEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortForceLinkDownEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortForceLinkDownEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortForceLinkPassEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortForceLinkPassEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with state [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                 */
            state = GT_TRUE;

            st  = cpssExMxPortForceLinkPassEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            state = GT_FALSE;

            st  = cpssExMxPortForceLinkPassEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        state = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortForceLinkPassEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortForceLinkPassEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortForceLinkPassEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortForceLinkPassEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortMruSet
(
    IN  GT_U8                  dev,
    IN  GT_U8                  port,
    IN  CPSS_PORT_MRU_SIZE_ENT mruSize
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortMruSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with mruSize [CPSS_PORT_MRU_1518/1522/1552_E]. Expected: GT_OK.
1.1.2. Call with mruSize [CPSS_PORT_MRU_2048_E]. Expected: GT_OK for 98Ex1x6 devices and GT_BAD_PARAM for others.
1.1.3. Call with mruSize [CPSS_PORT_MRU_9022/9192/10240_E]. Expected: GT_OK for GE, XG (non-FE) ports and GT_BAD_PARAM for others.
1.1.4. Call with mruSize [CPSS_PORT_MRU_1526/1536/1632/1664/9216/9220_E]. Expected: GT_BAD_PARAM.
1.1.5. Call with mruSize [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;
    GT_STATUS portTypeStExpected;/*expected st according to portType*/

    GT_U8                   dev;
    GT_U8                   port;
    CPSS_PORT_MRU_SIZE_ENT  mruSize;

    PRV_CPSS_PORT_TYPE_ENT  portType;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            if (PRV_CPSS_PORT_FE_E == portType)
            {
                /* Jumbo frames not support on FE port */
                portTypeStExpected = GT_BAD_PARAM;
            }
            else
            {
                portTypeStExpected = GT_OK;
            }

            /* 1.1.1. Call with mruSize [CPSS_PORT_MRU_1518/1522/1552_E].
            Expected: GT_OK. */
            mruSize = CPSS_PORT_MRU_1518_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);

            mruSize = CPSS_PORT_MRU_1522_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);

            mruSize = CPSS_PORT_MRU_1552_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);

            /* 1.1.2. Call with mruSize [CPSS_PORT_MRU_2048_E].
            Expected: GT_OK for 98Ex1x6 devices and GT_BAD_PARAM for others. */
            mruSize = CPSS_PORT_MRU_2048_E;

            st = cpssExMxPortMruSet(dev, port, mruSize);
            if (CPSS_PP_FAMILY_TIGER_E == devFamily)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mruSize);
            }
            else /* other then Tiger should return GT_BAD_PARAM */
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mruSize);
            }

            /* 1.1.3. Call with mruSize [CPSS_PORT_MRU_9022/9192/10240_E].
            Expected: GT_OK for GE, XG (non-FE) ports and GT_BAD_PARAM for others. */
            /* Jumbo frames not support on FE port */
            mruSize = CPSS_PORT_MRU_9022_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(portTypeStExpected, st, dev, port, mruSize);

            mruSize = CPSS_PORT_MRU_9192_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(portTypeStExpected, st, dev, port, mruSize);

            mruSize = CPSS_PORT_MRU_10240_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(portTypeStExpected, st, dev, port, mruSize);

            /* 1.1.4. Call with mruSize [CPSS_PORT_MRU_1526/1536/1632/1664/9216/9220_E].
            Expected: GT_BAD_PARAM. */
            /* MRU not supported by ExMx */
            mruSize = CPSS_PORT_MRU_1526_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mruSize);

            /* MRU not supported by ExMx */
            mruSize = CPSS_PORT_MRU_1536_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mruSize);

            /* MRU not supported by ExMx */
            mruSize = CPSS_PORT_MRU_1632_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mruSize);

            /* MRU not supported by ExMx */
            mruSize = CPSS_PORT_MRU_1664_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mruSize);

            /* MRU not supported by ExMx */
            mruSize = CPSS_PORT_MRU_9216_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mruSize);

            /* MRU not supported by ExMx */
            mruSize = CPSS_PORT_MRU_9220_E;
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mruSize);

            /* 1.1.5. Call with mruSize [0x5AAAAAA5]. Expected: GT_BAD_PARAM. */
            mruSize = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mruSize);
        }

        mruSize = CPSS_PORT_MRU_1518_E;

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortMruSet(dev, port, mruSize);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortMruSet(dev, port, mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    mruSize = CPSS_PORT_MRU_1518_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortMruSet(dev, port, mruSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortMruSet(dev, port, mruSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortCrcCheckEnableSet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    IN  GT_BOOL     enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortCrcCheckEnableSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL enable;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with enable [GT_TRUE and GT_FALSE].  */
            /* Expected: GT_OK.                                 */
            enable = GT_TRUE;

            st = cpssExMxPortCrcCheckEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_FALSE;

            st = cpssExMxPortCrcCheckEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        enable = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortCrcCheckEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortCrcCheckEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortCrcCheckEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortCrcCheckEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortXGmiiModeSet
(
    IN  GT_U8                     dev,
    IN  GT_U8                     port,
    IN  CPSS_PORT_XGMII_MODE_ENT  mode
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortXGmiiModeSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with mode [CPSS_PORT_XGMII_LAN_E]. Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.
1.1.2. Call with mode [CPSS_PORT_XGMII_WAN_E]. Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.
1.1.3. Call with mode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    GT_U8                       port;
    CPSS_PORT_XGMII_MODE_ENT    mode;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;
            CPSS_PP_DEVICE_TYPE     devType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            st = prvUtfDeviceTypeGet(dev, &devType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with mode [CPSS_PORT_XGMII_LAN_E].                               */
            /* Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.    */
            mode = CPSS_PORT_XGMII_LAN_E;

            st = cpssExMxPortXGmiiModeSet(dev, port, mode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
                continue; /* No need to check more */
            }

            /* 1.1.2. Call with mode [CPSS_PORT_XGMII_WAN_E].                               */
            /* Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.    */
            mode = CPSS_PORT_XGMII_WAN_E;

            st = cpssExMxPortXGmiiModeSet(dev, port, mode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }

            /* 1.1.3. Call with mode [0x5AAAAAA5].  */
            /* Expected: GT_BAD_PARAM.              */
            mode = PORT_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxPortXGmiiModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mode);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        mode = CPSS_PORT_XGMII_LAN_E;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortXGmiiModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortXGmiiModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    mode = CPSS_PORT_XGMII_LAN_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortXGmiiModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortXGmiiModeSet(dev, port, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortExtraIpgSet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    IN  GT_U8   number
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortExtraIpgSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with number [24]. Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   number;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with number [24].                                                */
            /* Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.    */
            number = 24;

            st = cpssExMxPortExtraIpgSet(dev, port, number);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, number);
            }
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        number = 24;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortExtraIpgSet(dev, port, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortExtraIpgSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    number = 24;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortExtraIpgSet(dev, port, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortExtraIpgSet(dev, port, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortXgmiiLocalFaultGet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    OUT GT_BOOL     *isLocalFaultPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortXgmiiLocalFaultGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null isLocalFaultPtr. Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.
1.1.2. Call with isLocalFaultPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     isLocalFault;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with non-null isLocalFaultPtr.                                   */
            /* Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.    */
            st = cpssExMxPortXgmiiLocalFaultGet(dev, port, &isLocalFault);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, isLocalFault);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, isLocalFault);
            }

            /* 1.1.2. Call with isLocalFaultPtr [NULL].     */
            /* Expected: GT_BAD_PTR.                        */
            st = cpssExMxPortXgmiiLocalFaultGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortXgmiiLocalFaultGet(dev, port, &isLocalFault);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortXgmiiLocalFaultGet(dev, port, &isLocalFault);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortXgmiiLocalFaultGet(dev, port, &isLocalFault);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortXgmiiLocalFaultGet(dev, port, &isLocalFault);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortXgmiiRemoteFaultGet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    OUT GT_BOOL     *isRemoteFaultPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortXgmiiRemoteFaultGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null isRemoteFaultPtr. Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.
1.1.2. Call with isRemoteFaultPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     isRemoteFault;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with non-null isRemoteFaultPtr.                                  */
            /* Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.    */
            st = cpssExMxPortXgmiiRemoteFaultGet(dev, port, &isRemoteFault);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, isRemoteFault);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, isRemoteFault);
            }

            /* 1.1.2. Call with isRemoteFaultPtr [NULL].    */
            /* Expected: GT_BAD_PTR.                        */
            st = cpssExMxPortXgmiiRemoteFaultGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortXgmiiRemoteFaultGet(dev, port, &isRemoteFault);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortXgmiiRemoteFaultGet(dev, port, &isRemoteFault);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortXgmiiRemoteFaultGet(dev, port, &isRemoteFault);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortXgmiiRemoteFaultGet(dev, port, &isRemoteFault);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortMacStatusGet
(
    IN  GT_U8                      dev,
    IN  GT_U8                      port,
    OUT CPSS_PORT_MAC_STATUS_STC   *portMacStatusPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortMacStatusGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null portMacStatusPtr. Expected: GT_OK.
1.1.2. Call with portMacStatusPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    GT_U8                       port;
    CPSS_PORT_MAC_STATUS_STC    portMacStatus;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null portMacStatusPtr.  */
            /* Expected: GT_OK.                             */
            st = cpssExMxPortMacStatusGet(dev, port, &portMacStatus);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with portMacStatusPtr [NULL].    */
            /* Expected: GT_BAD_PTR.                        */
            st = cpssExMxPortMacStatusGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortMacStatusGet(dev, port, &portMacStatus);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortMacStatusGet(dev, port, &portMacStatus);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortMacStatusGet(dev, port, &portMacStatus);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortMacStatusGet(dev, port, &portMacStatus);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortInternalLoopbackEnableSet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    IN  GT_BOOL     enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortInternalLoopbackEnableSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK for 10Gbps port and GT_BAD_PARAM for non 10Gbps port.
1.1.2. Call cpssExMxPortInternalLoopbackEnableGet with non-null enablePtr. Expected: GT_OK and the same enable value for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL enable;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;
            GT_BOOL                 enableWritten;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with enable [GT_TRUE and GT_FALSE].                              */
            /* Expected: GT_OK for 10Gbps port and GT_BAD_PARAM for non 10Gbps port.        */
            enable = GT_TRUE;

            st = cpssExMxPortInternalLoopbackEnableSet(dev, port, enable);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
            }

            enable = GT_FALSE;

            st = cpssExMxPortInternalLoopbackEnableSet(dev, port, enable);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
            }

            /* 1.1.2. Call cpssExMxPortInternalLoopbackEnableGet with non-null enablePtr.   */
            /* Expected: GT_OK and the same enable value for 10Gbps port and                */
            /* GT_BAD_PARAM for non 10Gbps port.                                            */
            st = cpssExMxPortInternalLoopbackEnableGet(dev, port, &enableWritten);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPortInternalLoopbackEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL3_PARAM_MAC(enable, enableWritten, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "cpssExMxPortInternalLoopbackEnableGet: %d, %d", dev, port);
            }
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        enable = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortInternalLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortInternalLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortInternalLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortInternalLoopbackEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortInternalLoopbackEnableGet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    OUT GT_BOOL    *enablePtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortInternalLoopbackEnableGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null enablePtr. Expected: GT_OK for 10Gbps port and GT_BAD_PARAM for non 10Gbps port.
1.1.2. Call with enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL enable;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with non-null enablePtr.                                         */
            /* Expected: GT_OK for 10Gbps port and GT_BAD_PARAM for non 10Gbps port.        */
            st = cpssExMxPortInternalLoopbackEnableGet(dev, port, &enable);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
            }

            /* 1.1.2. Call with enablePtr [NULL].   */
            /* Expected: GT_BAD_PTR.                */
            st = cpssExMxPortInternalLoopbackEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortInternalLoopbackEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortInternalLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortInternalLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortInternalLoopbackEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    port,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortPreambleLengthSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call function with direction [CPSS_PORT_DIRECTION_RX_E/ CPSS_PORT_DIRECTION_BOTH_E] and length [4/ 8]. Expected: GT_OK for 98EX136 devices and NON GT_OK for others.
1.1.2. Check for non valid enum value for direction. For 98EX136 device call function with direction [0x5AAAAAA5] and length [4]. Expected: GT_BAD_PARAM.
1.1.3. Check for non valid length value. For 98EX136 device call function with direction [CPSS_PORT_DIRECTION_RX_E] and length [0/ 3/ 10]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port;
    CPSS_PORT_DIRECTION_ENT direction;
    GT_U32                  length;

    CPSS_PP_DEVICE_TYPE     devType;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceTypeGet(dev, &devType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceTypeGet: %d", dev);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with direction [CPSS_PORT_DIRECTION_RX_E/
            CPSS_PORT_DIRECTION_BOTH_E] and length [4/ 8].
            Expected: GT_OK for 98EX136 devices and NON GT_OK for others. */
            direction = CPSS_PORT_DIRECTION_RX_E;
            length = 4;

            st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
            if (IS_98EX136_DEV_MAC(devType))
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "98Ex136 device: %d, %d, %d, %d", dev, port, direction, length);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st,
                    "non-98Ex136 device: %d", dev);
            }

            direction = CPSS_PORT_DIRECTION_BOTH_E;
            length = 8;

            st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
            if (IS_98EX136_DEV_MAC(devType))
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "98Ex136 device: %d, %d, %d, %d", dev, port, direction, length);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st,
                    "non-98Ex136 device: %d", dev);
            }

            if (IS_98EX136_DEV_MAC(devType))
            {
                /* 1.1.2. Check for non valid enum value for direction.
                For 98EX136 device call function with direction [0x5AAAAAA5]
                and length [4]. Expected: GT_BAD_PARAM. */
                direction = PORT_CTRL_INVALID_ENUM_CNS;
                length = 4;

                st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction);

                /* 1.1.3. Check for non valid length value.
                For 98EX136 device call function with direction
                [CPSS_PORT_DIRECTION_RX_E] and length [0/ 3/ 10].
                Expected: NON GT_OK. */
                direction = CPSS_PORT_DIRECTION_RX_E;

                length = 0;
                st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

                length = 3;
                st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

                length = 10;
                st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
            }
        }

        if (IS_98EX136_DEV_MAC(devType))
        {
            direction = CPSS_PORT_DIRECTION_RX_E;
            length = 4;

            /* prepare physical port iterator */
            st = prvUtfNextPhyPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
            /* 1.2. Go over non configured ports */
            while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
            {
                /* 1.2.1. Call function for each non configured port.   */
                /* Expected: GT_BAD_PARAM                               */
                st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }

            /* 1.3. For active device check that function returns GT_BAD_PARAM  */
            /* for out of bound value for port number.                          */
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    direction = CPSS_PORT_DIRECTION_RX_E;
    length = 4;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortPreambleLengthSet(dev, port, direction, length);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPortCtrl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPortCtrl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortMacSaLsbSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortDuplexModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortDuplexModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortSpeedSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortSpeedGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortAttributesOnPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortDuplexAutoNegEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortFlowCntrlAutoNegEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortSpeedAutoNegEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortFlowControlEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortFlowControlEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortPeriodicFcEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortBackPressureEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortLinkStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortInterfaceModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortForceLinkDownEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortForceLinkPassEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortMruSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortCrcCheckEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortXGmiiModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortExtraIpgSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortXgmiiLocalFaultGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortXgmiiRemoteFaultGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortMacStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortInternalLoopbackEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortInternalLoopbackEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortPreambleLengthSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPortCtrl)
