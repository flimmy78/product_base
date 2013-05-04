/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPortBackEndUT.c
*
* DESCRIPTION:
*       Unit Tests for Port Back-End ExMx CPSS implementation
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortBackEnd.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Default valid value for port id */
#define PORT_BACKEND_VALID_PHY_PORT_CNS     0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortBackEndModeEnable
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_BOOL  backEndEnable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortBackEndModeEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with backEndEnable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL backEndEnable;

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
            /* 1.1.1. Call with backEndEnable [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                         */
            backEndEnable = GT_TRUE;

            st = cpssExMxPortBackEndModeEnable(dev, port, backEndEnable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st ,dev, port, backEndEnable);

            backEndEnable = GT_FALSE;

            st = cpssExMxPortBackEndModeEnable(dev, port, backEndEnable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st ,dev, port, backEndEnable);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        backEndEnable = GT_FALSE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortBackEndModeEnable(dev, port, backEndEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortBackEndModeEnable(dev, port, backEndEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_BACKEND_VALID_PHY_PORT_CNS;
    backEndEnable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortBackEndModeEnable(dev, port, backEndEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortBackEndModeEnable(dev, port, backEndEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPortBackEnd suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPortBackEnd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortBackEndModeEnable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPortBackEnd)
