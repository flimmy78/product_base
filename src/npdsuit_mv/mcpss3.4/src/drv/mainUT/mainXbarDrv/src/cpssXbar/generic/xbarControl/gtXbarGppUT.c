/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtXbarGppUT.c
*
* DESCRIPTION:
*       Unit tests for gtXbarGpp, that provides
*       Prestera API definitions for Fabric stacking XBAR interface.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssXbar/generic/xbarControl/gtXbarGpp.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarGppIoConfig
(
	IN GT_U8 	xbarDevNum,
	IN GT_U8 	gppPinNum,
	IN GT_BOOL 	isInput
)
*/
UTF_TEST_CASE_MAC(gtXbarGppIoConfig)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with gppPinNum [0 / 0xFF], 
                   isInput [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U8       gppPinNum = 0;
    GT_BOOL 	isInput   = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with gppPinNum [0 / 0xFF], 
                           isInput [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* Call with gppPinNum = 0 */
        gppPinNum = 0;
        isInput   = GT_FALSE;

        st = gtXbarGppIoConfig(dev, gppPinNum, isInput);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, gppPinNum, isInput);

        /* Call with gppPinNum = 0xFF */
        gppPinNum = 0xFF;
        isInput   = GT_TRUE;

        st = gtXbarGppIoConfig(dev, gppPinNum, isInput);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, gppPinNum, isInput);
    }

    gppPinNum = 0;
    isInput   = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGppIoConfig(dev, gppPinNum, isInput);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtXbarGppIoConfig(dev, gppPinNum, isInput);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarGppSetOutputPin
(
	IN GT_U8 	xbarDevNum ,
	IN GT_U8 	gppPinNum,
	IN GT_BOOL 	pinOutputVal
)
*/
UTF_TEST_CASE_MAC(gtXbarGppSetOutputPin)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with gppPinNum [0 / 0xFF], 
                   pinOutputVal [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call gtXbarGppGetInputPin with the same gppPinNum 
                                         and non-NULL pinInputVal.
    Expected: GT_OK and the same pinInputVal.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U8       gppPinNum       = 0;
    GT_BOOL 	pinOutputVal    = GT_FALSE;
    GT_BOOL 	pinOutputValGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with gppPinNum [0 / 0xFF], 
                           pinOutputVal [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* Call with gppPinNum = 0 */
        gppPinNum    = 0;
        pinOutputVal = GT_FALSE;

        st = gtXbarGppSetOutputPin(dev, gppPinNum, pinOutputVal);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, gppPinNum, pinOutputVal);
        
        /*
            1.2. Call gtXbarGppGetInputPin with the same gppPinNum 
                                                 and non-NULL pinInputVal.
            Expected: GT_OK and the same pinInputVal.
        */
        st = gtXbarGppGetInputPin(dev, gppPinNum, &pinOutputValGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "gtXbarGppGetInputPin: %d, %d", dev, gppPinNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(pinOutputVal, pinOutputValGet,
                   "get another pinOutputVal then was set: %d", dev);
        
        /* Call with gppPinNum = 0xFF */
        gppPinNum    = 0xFF;
        pinOutputVal = GT_TRUE;
        
        st = gtXbarGppSetOutputPin(dev, gppPinNum, pinOutputVal);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, gppPinNum, pinOutputVal);
        
        /*
            1.2. Call gtXbarGppGetInputPin with the same gppPinNum 
                                                 and non-NULL pinInputVal.
            Expected: GT_OK and the same pinInputVal.
        */
        st = gtXbarGppGetInputPin(dev, gppPinNum, &pinOutputValGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "gtXbarGppGetInputPin: %d, %d", dev, gppPinNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(pinOutputVal, pinOutputValGet,
                   "get another pinOutputVal then was set: %d", dev);        
    }

    gppPinNum    = 0;
    pinOutputVal = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGppSetOutputPin(dev, gppPinNum, pinOutputVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtXbarGppSetOutputPin(dev, gppPinNum, pinOutputVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarGppGetInputPin
(
	IN 	GT_U8 	xbarDevNum,
	IN 	GT_U8 	gppPinNum,
	OUT GT_BOOL *pinInputVal
)
*/
UTF_TEST_CASE_MAC(gtXbarGppGetInputPin)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with gppPinNum [0 / 0xFF] 
                   and non-NULL pinOutputVal.
    Expected: GT_OK.
    1.2. Call with pinInputVal [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U8       gppPinNum   = 0;
    GT_BOOL 	pinInputVal = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with gppPinNum [0 / 0xFF] 
                           and non-NULL pinOutputVal.
            Expected: GT_OK.
        */
        /* Call with gppPinNum = 0 */
        gppPinNum = 0;

        st = gtXbarGppGetInputPin(dev, gppPinNum, &pinInputVal);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, gppPinNum);

        /* Call with gppPinNum = 0xFF */
        gppPinNum = 0xFF;

        st = gtXbarGppGetInputPin(dev, gppPinNum, &pinInputVal);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, gppPinNum);

        /*
            1.2. Call with pinInputVal [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtXbarGppGetInputPin(dev, gppPinNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pinInputVal = NULL", dev);
    }

    gppPinNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGppGetInputPin(dev, gppPinNum, &pinInputVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtXbarGppGetInputPin(dev, gppPinNum, &pinInputVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of gtXbarGpp suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(gtXbarGpp)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGppIoConfig)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGppSetOutputPin)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGppGetInputPin)
UTF_SUIT_END_TESTS_MAC(gtXbarGpp)

