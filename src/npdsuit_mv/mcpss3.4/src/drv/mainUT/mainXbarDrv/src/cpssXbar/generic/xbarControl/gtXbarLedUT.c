/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtXbarLedUT.c
*
* DESCRIPTION:
*       Unit tests for gtXbarLed, that provides
*       Prestera API definitions for Fabric stacking XBAR interface.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssXbar/generic/xbarControl/gtXbarLed.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
GT_STATUS xbarLedSetStreamParams
(
	IN GT_U8 xbarDevNum,
	IN GT_U16 startBit,
	IN GT_U16 endBit
)
*/
UTF_TEST_CASE_MAC(xbarLedSetStreamParams)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with startBit [0 / 0xFFFF] 
                   and endBit [0 / 0xFFFF].
    Expected: GT_OK. 
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

	GT_U16  startBit = 0;
	GT_U16  endBit   = 0;
    
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with startBit [0 / 0xFFFF] 
                           and endBit [0 / 0xFFFF].
            Expected: GT_OK. 
        */
        /* Call with startBit = 0 */
        startBit = 0;
        endBit   = 0;

        st = xbarLedSetStreamParams(dev, startBit, endBit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, startBit, endBit);

        /* Call with startBit = 0xFFFF */
        startBit = 0xFFFF;
        endBit   = 0xFFFF;

        st = xbarLedSetStreamParams(dev, startBit, endBit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, startBit, endBit);
    }

    startBit = 0xFFFF;
    endBit   = 0xFFFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = xbarLedSetStreamParams(dev, startBit, endBit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = xbarLedSetStreamParams(dev, startBit, endBit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS xbarLedSetOverrideData
(
    IN GT_U8  xbarDevNum,
    IN GT_U32 ledData
)
*/
UTF_TEST_CASE_MAC(xbarLedSetOverrideData)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with ledData [0 / 0xFFFFFFFF].
    Expected: GT_OK. 
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U32 ledData = 0;
    
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ledData [0 / 0xFFFFFFFF].
            Expected: GT_OK. 
        */
        /* Call with ledData = 0 */
        ledData = 0;

        st = xbarLedSetOverrideData(dev, ledData);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ledData);

        /* Call with ledData = 0xFFFFFFFF */
        ledData = 0xFFFFFFFF;

        st = xbarLedSetOverrideData(dev, ledData);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ledData);
    }

    ledData = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = xbarLedSetOverrideData(dev, ledData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = xbarLedSetOverrideData(dev, ledData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of gtXbarLed suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(gtXbarLed)
    UTF_SUIT_DECLARE_TEST_MAC(xbarLedSetStreamParams)
    UTF_SUIT_DECLARE_TEST_MAC(xbarLedSetOverrideData)
UTF_SUIT_END_TESTS_MAC(gtXbarLed)

