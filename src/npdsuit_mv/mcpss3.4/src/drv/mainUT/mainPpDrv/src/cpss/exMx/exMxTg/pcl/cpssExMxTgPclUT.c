/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxTgPclUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxTgPcl.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxTg/pcl/cpssExMxTgPcl.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define TG_PCL_INVALID_ENUM_CNS    0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgPclProfileSizeSet
(
    IN GT_U8                              dev,
    IN CPSS_EXMX_TG_PCL_PROFILE_SIZE_ENT  profileSize
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgPclProfileSizeSet)
{
/*
ITERATE_DEVICES
1.1. Call with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Check for invalid enum value. For Tiger device call with profileSize [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                   dev;
    CPSS_EXMX_TG_PCL_PROFILE_SIZE_ENT       profileSize;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with profileSize                                       */
        /* [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E].                   */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E;

        st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, profileSize);

        /* 1.2. Check for invalid enum value. For Tiger device call with    */
        /* profileSize [0x5AAAAAA5]. Expected: GT_BAD_PARAM.                */
        profileSize = TG_PCL_INVALID_ENUM_CNS;

        st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profileSize);
    }

    profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgPclLookupKeySizeSelectModeSet
(
    IN GT_U8                                dev,
    IN CPSS_EXMX_PCL_PKT_TYPE_ENT           pktType,
    IN CPSS_EXMX_TG_PCL_KEY_SIZE_SELECT_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgPclLookupKeySizeSelectModeSet)
{
/*
ITERATE_DEVICES
1.1. Call with pktType [CPSS_EXMX_PCL_PKT_IPV4_TCP_E], mode [CPSS_EXMX_TG_PCL_KEY_SIZE_STD_ALWAYS_E]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Check for invalid enum value. For Tiger device call with pktType [0x5AAAAAA5], mode [CPSS_EXMX_TG_PCL_KEY_SIZE_STD_ALWAYS_E]. Expected: GT_BAD_PARAM.
1.3. Check for invalid enum value. For Tiger device call with pktType [CPSS_EXMX_PCL_PKT_IPV4_TCP_E], mode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                   dev;
    CPSS_EXMX_PCL_PKT_TYPE_ENT              pktType;
    CPSS_EXMX_TG_PCL_KEY_SIZE_SELECT_ENT    mode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with pktType [CPSS_EXMX_PCL_PKT_IPV4_TCP_E],           */
        /* mode [CPSS_EXMX_TG_PCL_KEY_SIZE_STD_ALWAYS_E].                   */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
        mode = CPSS_EXMX_TG_PCL_KEY_SIZE_STD_ALWAYS_E;

        st = cpssExMxTgPclLookupKeySizeSelectModeSet(dev, pktType, mode);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d", dev, pktType, mode);

        /* Tests for Tiger devices only */
        /* 1.2. Check for invalid enum value. For Tiger device call with    */
        /* pktType [0x5AAAAAA5],                                            */
        /* mode [CPSS_EXMX_TG_PCL_KEY_SIZE_STD_ALWAYS_E].                   */
        /* Expected: GT_BAD_PARAM.                                          */
        pktType = TG_PCL_INVALID_ENUM_CNS;
        mode = CPSS_EXMX_TG_PCL_KEY_SIZE_STD_ALWAYS_E;

        st = cpssExMxTgPclLookupKeySizeSelectModeSet(dev, pktType, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, pktType);

        /* 1.3. Check for invalid enum value. For Tiger device call with    */
        /* pktType [CPSS_EXMX_PCL_PKT_IPV4_TCP_E], mode [0x5AAAAAA5].       */
        /* Expected: GT_BAD_PARAM.                                          */
        mode = TG_PCL_INVALID_ENUM_CNS;
        pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;

        st = cpssExMxTgPclLookupKeySizeSelectModeSet(dev, pktType, mode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, pktType, mode);
    }

    pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
    mode = CPSS_EXMX_TG_PCL_KEY_SIZE_STD_ALWAYS_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgPclLookupKeySizeSelectModeSet(dev, pktType, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgPclLookupKeySizeSelectModeSet(dev, pktType, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgPclL3OffsetsModeSet
(
   IN GT_U8  dev,
   IN CPSS_EXMX_TG_PCL_L3_OFFSETS_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgPclL3OffsetsModeSet)
{
/*
ITERATE_DEVICES
1.1. Call with mode [CPSS_EXMX_TG_PCL_L3_OFFSETS_AFTER_L2_E]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Check for invalid enum value. For Tiger device call with mode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                   dev;
    CPSS_EXMX_TG_PCL_L3_OFFSETS_MODE_ENT    mode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with mode [CPSS_EXMX_TG_PCL_L3_OFFSETS_AFTER_L2_E].    */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        mode = CPSS_EXMX_TG_PCL_L3_OFFSETS_AFTER_L2_E;

        st = cpssExMxTgPclL3OffsetsModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, mode);

        /* 1.2. Check for invalid enum value. For Tiger device call with    */
        /* mode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.                       */
        mode = TG_PCL_INVALID_ENUM_CNS;

        st = cpssExMxTgPclL3OffsetsModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMX_TG_PCL_L3_OFFSETS_AFTER_L2_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgPclL3OffsetsModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgPclL3OffsetsModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgPclIpv6KeyDataSelectSet
(
   IN GT_U8                                          dev,
   IN CPSS_EXMX_TG_PCL_IPV6_KEY_DATA_SELECT_TYPE_ENT keyDataType
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgPclIpv6KeyDataSelectSet)
{
/*
ITERATE_DEVICES
1.1. Call with keyDataType [CPSS_EXMX_TG_PCL_IPV6_KEY_DATA_MAC_E]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Check for invalid enum value. For Tiger device call with keyDataType [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                           dev;
    CPSS_EXMX_TG_PCL_IPV6_KEY_DATA_SELECT_TYPE_ENT  keyDataType;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with keyDataType [CPSS_EXMX_TG_PCL_IPV6_KEY_DATA_MAC_E].*/
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.    */
        keyDataType = CPSS_EXMX_TG_PCL_IPV6_KEY_DATA_MAC_E;

        st = cpssExMxTgPclIpv6KeyDataSelectSet(dev, keyDataType);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, keyDataType);

        /* 1.2. Check for invalid enum value. For Tiger device call with    */
        /* keyDataType [0x5AAAAAA5]. Expected: GT_BAD_PARAM.                */
        keyDataType = TG_PCL_INVALID_ENUM_CNS;

        st = cpssExMxTgPclIpv6KeyDataSelectSet(dev, keyDataType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, keyDataType);
    }

    keyDataType = CPSS_EXMX_TG_PCL_IPV6_KEY_DATA_MAC_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgPclIpv6KeyDataSelectSet(dev, keyDataType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgPclIpv6KeyDataSelectSet(dev, keyDataType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgPclDefaultLookupActionIndexSet
(
    IN GT_U8                              dev,
    IN GT_U32                             actionIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgPclDefaultLookupActionIndexSet)
{
/*
ITERATE_DEVICES
1.1. Call with actionIndex [0]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Check for out of range actionIndex. For Tiger device call with actionIndex [4096]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                       dev;
    GT_U32                      actionIndex;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with actionIndex [0]. Expected: GT_OK for Tiger    */
        /* devices and GT_BAD_PARAM for others.                         */
        actionIndex = 0;

        st = cpssExMxTgPclDefaultLookupActionIndexSet(dev, actionIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, actionIndex);

        /* 1.2. Check for out of range actionIndex. For Tiger device    */
        /* call with actionIndex [4096]. Expected: NON GT_OK.           */
        actionIndex = 4096;

        st = cpssExMxTgPclDefaultLookupActionIndexSet(dev, actionIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, actionIndex);
    }

    actionIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgPclDefaultLookupActionIndexSet(dev, actionIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgPclDefaultLookupActionIndexSet(dev, actionIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxTgPcl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxTgPcl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgPclProfileSizeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgPclLookupKeySizeSelectModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgPclL3OffsetsModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgPclIpv6KeyDataSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgPclDefaultLookupActionIndexSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxTgPcl)

