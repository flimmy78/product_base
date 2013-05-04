/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalCfgInitUT.c
*
* DESCRIPTION:
*       Unit Tests for CPSS DxSal initialization of PPs
*       and shadow data structures, and declarations of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxSal/config/cpssDxSalCfgInit.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Invalid enumeration value used for testing */
#define CFG_INIT_INVALID_ENUM_CNS       0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalCfgDevEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalCfgDevEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_FALSE and GT_TRUE].           */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.   */
        enable = GT_FALSE;

        st = cpssDxSalCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);

        enable = GT_TRUE;

        st = cpssDxSalCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxSalCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalCfgDevEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalCfgDevMacAddrSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalCfgDevMacAddrSet)
{
/*
ITERATE_DEVICES
1.1. Call with non-NULL macPtr [AB:00:00:12:FF:FF]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
1.2. For Salsa devices call with macPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_ETHERADDR                mac = {{0xAB, 0x00, 0x00, 0x12, 0xFF, 0xFF}};

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-NULL macPtr [AB:00:00:12:FF:FF].              */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.   */

        st = cpssDxSalCfgDevMacAddrSet(dev, &mac);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "Salsa device: %d", dev);

        /* 1.2. For Salsa devices call with macPtr [NULL].  */
        /* Expected: GT_BAD_PTR.                            */

        st = cpssDxSalCfgDevMacAddrSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxSalCfgDevMacAddrSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalCfgDevMacAddrSet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxSalCfgInit suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalCfgInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalCfgDevEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalCfgDevMacAddrSet)
UTF_SUIT_END_TESTS_MAC(cpssDxSalCfgInit)
