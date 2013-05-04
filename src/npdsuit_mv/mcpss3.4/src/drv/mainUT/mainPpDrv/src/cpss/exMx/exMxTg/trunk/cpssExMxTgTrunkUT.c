/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxTgTrunkUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxTgTrunk.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxTg/trunk/cpssExMxTgTrunk.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define TG_TRUNK_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgTrunkHashIpv6ModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_EXMX_TRUNK_IPV6_HASH_MODE_ENT   hashMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgTrunkHashIpv6ModeSet)
{
/*
ITERATE_DEVICES
1.1. Call function with hashMode [CPSS_EXMX_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. For Tiger device call function with out of range hashMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                   dev;
    CPSS_EXMX_TRUNK_IPV6_HASH_MODE_ENT      hashMode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with hashMode                                 */
        /* [CPSS_EXMX_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW].                */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        hashMode = CPSS_EXMX_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW_E;

        st = cpssExMxTgTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, hashMode);

        /* 1.2. For Tiger device call function with out of range    */
        /* hashMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.           */
        hashMode = TG_TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxTgTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, hashMode);
    }

    hashMode = CPSS_EXMX_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgTrunkHashIpv6ModeSet(dev, hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgTrunkHashMaskParametersSet
(
    IN GT_U8                                    devNum,
    IN CPSS_TRUNK_EXMX_MASK_HASH_PARAM_INFO_STC *trunkMaskInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgTrunkHashMaskParametersSet)
{
/*
ITERATE_DEVICES
1.1. Call function with trunkMaskInfoPtr [0xFF…F]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2 For Tiger device call function with trunkMaskInfoPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                       dev;
    CPSS_TRUNK_EXMX_MASK_HASH_PARAM_INFO_STC    trunkMaskInfo;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with trunkMaskInfoPtr [0xFF…F].               */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        cpssOsMemSet((GT_VOID*)&trunkMaskInfo, 0xFF, sizeof(trunkMaskInfo));

        st = cpssExMxTgTrunkHashMaskParametersSet(dev, &trunkMaskInfo);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "Tiger device: %d", dev);

        /* 1.2 For Tiger device call function with trunkMaskInfoPtr [NULL]. */
        /* Expected: GT_BAD_PTR.                                            */
        st = cpssExMxTgTrunkHashMaskParametersSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    cpssOsMemSet((GT_VOID*)&trunkMaskInfo, 0xFF, sizeof(trunkMaskInfo));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgTrunkHashMaskParametersSet(dev, &trunkMaskInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgTrunkHashMaskParametersSet(dev, &trunkMaskInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxTgTrunk suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxTgTrunk)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgTrunkHashIpv6ModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgTrunkHashMaskParametersSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxTgTrunk)
