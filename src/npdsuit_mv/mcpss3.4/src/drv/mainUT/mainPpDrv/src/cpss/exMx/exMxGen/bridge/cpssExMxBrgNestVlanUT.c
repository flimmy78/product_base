/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgNestVlanUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxBrgNestVlan, that provides
*       cpss Ex/Mx implementation for Prestera Private Edge VLANs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>

#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgNestVlan.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgNestVlanEnable
(
    IN  GT_U8   dev,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgNestVlanEnable)
{
/*
ITERATE_DEVICES
1.1. Call function with enable GT_TRUE and GT_FALSE. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */
        enable = GT_TRUE;

        st = cpssExMxBrgNestVlanEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;

        st = cpssExMxBrgNestVlanEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgNestVlanEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_TRUE */

    st = cpssExMxBrgNestVlanEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgNestVlanPortStateSet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    IN  CPSS_EXMX_NESTED_PORT_STATE_ENT     state
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgNestVlanPortStateSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call function with state [CPSS_EXMX_CORE_PORT_E]. 
    Expected: GT_OK.
1.1.2. Call function with out of range state [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                               dev;
    GT_U8                               port;
    CPSS_EXMX_NESTED_PORT_STATE_ENT     state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with state [CPSS_EXMX_CORE_PORT_E]. */
            /* Expected: GT_OK.                                         */
            state = CPSS_EXMX_CORE_PORT_E;

            st = cpssExMxBrgNestVlanPortStateSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* 1.1.2. Call function with out of range state [0x5AAAAAA5]. */
            /* Expected: GT_BAD_PARAM.                                    */
            state = 0x5AAAAAA5;

            st = cpssExMxBrgNestVlanPortStateSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, state);
        }

        state = CPSS_EXMX_CORE_PORT_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each invalid port */
            st = cpssExMxBrgNestVlanPortStateSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* state == CPSS_EXMX_CORE_PORT_E */

        st = cpssExMxBrgNestVlanPortStateSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    port = 0;
    state = CPSS_EXMX_CORE_PORT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgNestVlanPortStateSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0                        */
    /* state == CPSS_EXMX_CORE_PORT_E   */

    st = cpssExMxBrgNestVlanPortStateSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxBrgNestVlan suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxBrgNestVlan)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgNestVlanEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgNestVlanPortStateSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxBrgNestVlan)
