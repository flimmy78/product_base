/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmFabricConfigUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmFabricConfig, that provides
*       CPSS ExMxPm Fabric Connectivity Confuguration API:
*        - Fabric Load Balancing Index (LBI) configuration
*        - Fabric VIDX configuration
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricConfig.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define FABRIC_CONFIG_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define FABRIC_CONFIG_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define FABRIC_CONFIG_TESTED_VLAN_ID_CNS  100


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigVidxSet
(
    IN  GT_U8                                  devNum,
    IN  CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC     *vidxConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigVidxSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vidxConfigPtr {fabricVidxBit0IsLbiBit0 [GT_FALSE / GT_TRUE],
                                  fabricVIDX0 [100 / 4095],
                                  fabricVIDX1[100 / 4095],
                                  fabricVIDXMode [CPSS_EXMXPM_FABRIC_CONFIG_VIDX_FLOOD_ALL_E /
                                                  CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L2_VIDX_L3_E /
                                                  CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L2_VID_L3_E /
                                                  CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L3_E]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricConfigVidxGet with non-NULL vidxConfigPtr.
    Expected: GT_OK and the same vidxConfigPtr.
    1.3. Call with out of range vidxConfigPtr->fabricVIDX0 [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range vidxConfigPtr->fabricVIDX1 [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range vidxConfigPtr->fabricVIDXMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with vidxConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC  vidxConfig;
    CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC  vidxConfigGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vidxConfigPtr {fabricVidxBit0IsLbiBit0 [GT_FALSE / GT_TRUE],
                                          fabricVIDX0 [100 / 4095],
                                          fabricVIDX1 [100 / 4095],
                                          fabricVIDXMode [CPSS_EXMXPM_FABRIC_CONFIG_VIDX_FLOOD_ALL_E /
                                                          CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L2_VIDX_L3_E /
                                                          CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L2_VID_L3_E /
                                                          CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L3_E]}.
            Expected: GT_OK.
        */

        /* Call with vidxConfigPtr->fabricVidxBit0IsLbiBit0 [GT_FALSE] */
        vidxConfig.fabricVidxBit0IsLbiBit0 = GT_FALSE;

        vidxConfig.fabricVIDX0 = FABRIC_CONFIG_TESTED_VLAN_ID_CNS;
        vidxConfig.fabricVIDX1 = FABRIC_CONFIG_TESTED_VLAN_ID_CNS;

        /* Call with vidxConfigPtr->fabricVIDXMode [CPSS_EXMXPM_FABRIC_CONFIG_VIDX_FLOOD_ALL_E] */
        vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_FLOOD_ALL_E;

        st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricConfigVidxGet with non-NULL vidxConfigPtr.
            Expected: GT_OK and the same vidxConfigPtr.
        */
        st = cpssExMxPmFabricConfigVidxGet(dev, &vidxConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricConfigVidxGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVidxBit0IsLbiBit0, vidxConfigGet.fabricVidxBit0IsLbiBit0,
                       "get another vidxConfigPtr->fabricVidxBit0IsLbiBit0 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDX0, vidxConfigGet.fabricVIDX0,
                       "get another vidxConfigPtr->fabricVIDX0 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDX1, vidxConfigGet.fabricVIDX1,
                       "get another vidxConfigPtr->fabricVIDX1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDXMode, vidxConfigGet.fabricVIDXMode,
                       "get another vidxConfigPtr->fabricVIDXMode than was set: %d", dev);

        /* Call with vidxConfigPtr->fabricVIDXMode [CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L2_VIDX_L3_E] */
        vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L2_VIDX_L3_E;

        st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricConfigVidxGet with non-NULL vidxConfigPtr.
            Expected: GT_OK and the same vidxConfigPtr.
        */
        st = cpssExMxPmFabricConfigVidxGet(dev, &vidxConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricConfigVidxGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVidxBit0IsLbiBit0, vidxConfigGet.fabricVidxBit0IsLbiBit0,
                       "get another vidxConfigPtr->fabricVidxBit0IsLbiBit0 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDX0, vidxConfigGet.fabricVIDX0,
                       "get another vidxConfigPtr->fabricVIDX0 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDX1, vidxConfigGet.fabricVIDX1,
                       "get another vidxConfigPtr->fabricVIDX1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDXMode, vidxConfigGet.fabricVIDXMode,
                       "get another vidxConfigPtr->fabricVIDXMode than was set: %d", dev);

        /* Call with vidxConfigPtr->fabricVidxBit0IsLbiBit0 [GT_TRUE] */
        vidxConfig.fabricVidxBit0IsLbiBit0 = GT_TRUE;

        vidxConfig.fabricVIDX0 = 4095;
        vidxConfig.fabricVIDX1 = 4095;

        /* Call with vidxConfigPtr->fabricVIDXMode [CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L2_VID_L3_E] */
        vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L2_VID_L3_E;

        st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricConfigVidxGet with non-NULL vidxConfigPtr.
            Expected: GT_OK and the same vidxConfigPtr.
        */
        st = cpssExMxPmFabricConfigVidxGet(dev, &vidxConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricConfigVidxGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVidxBit0IsLbiBit0, vidxConfigGet.fabricVidxBit0IsLbiBit0,
                       "get another vidxConfigPtr->fabricVidxBit0IsLbiBit0 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDX0, vidxConfigGet.fabricVIDX0,
                       "get another vidxConfigPtr->fabricVIDX0 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDX1, vidxConfigGet.fabricVIDX1,
                       "get another vidxConfigPtr->fabricVIDX1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDXMode, vidxConfigGet.fabricVIDXMode,
                       "get another vidxConfigPtr->fabricVIDXMode than was set: %d", dev);

        /* Call with vidxConfigPtr->fabricVIDXMode [CPSS_EXMXPM_FABRIC_CONFIG_VIDX_GLOBAL_MODE_E] */
        vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L3_E;

        st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricConfigVidxGet with non-NULL vidxConfigPtr.
            Expected: GT_OK and the same vidxConfigPtr.
        */
        st = cpssExMxPmFabricConfigVidxGet(dev, &vidxConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricConfigVidxGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVidxBit0IsLbiBit0, vidxConfigGet.fabricVidxBit0IsLbiBit0,
                       "get another vidxConfigPtr->fabricVidxBit0IsLbiBit0 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDX0, vidxConfigGet.fabricVIDX0,
                       "get another vidxConfigPtr->fabricVIDX0 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDX1, vidxConfigGet.fabricVIDX1,
                       "get another vidxConfigPtr->fabricVIDX1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidxConfig.fabricVIDXMode, vidxConfigGet.fabricVIDXMode,
                       "get another vidxConfigPtr->fabricVIDXMode than was set: %d", dev);

        /*
            1.3. Call with out of range vidxConfigPtr->fabricVIDX0 [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        vidxConfig.fabricVIDX0 = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vidxConfigPtr->fabricVIDX0 = %d",
                                         dev, vidxConfig.fabricVIDX0);

        vidxConfig.fabricVIDX0 = FABRIC_CONFIG_TESTED_VLAN_ID_CNS;

        /*
            1.4. Call with out of range vidxConfigPtr->fabricVIDX1 [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        vidxConfig.fabricVIDX1 = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vidxConfigPtr->fabricVIDX1 = %d",
                                         dev, vidxConfig.fabricVIDX1);

        vidxConfig.fabricVIDX1 = FABRIC_CONFIG_TESTED_VLAN_ID_CNS;

        /*
            1.5. Call with out of range vidxConfigPtr->fabricVIDXMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vidxConfig.fabricVIDXMode = FABRIC_CONFIG_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vidxConfigPtr->fabricVIDXMode = %d",
                                     dev, vidxConfig.fabricVIDXMode);

        vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_FLOOD_ALL_E;

        /*
            1.6. Call with vidxConfigPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricConfigVidxSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vidxConfigPtr = NULL", dev);
    }

    vidxConfig.fabricVidxBit0IsLbiBit0 = GT_FALSE;

    vidxConfig.fabricVIDX0    = FABRIC_CONFIG_TESTED_VLAN_ID_CNS;
    vidxConfig.fabricVIDX1    = FABRIC_CONFIG_TESTED_VLAN_ID_CNS;
    vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_TRUST_L3_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigVidxSet(dev, &vidxConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigVidxGet
(
    IN  GT_U8                                  devNum,
    OUT CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC     *vidxConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigVidxGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL vidxConfigPtr.
    Expected: GT_OK.
    1.2. Call with vidxConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC  vidxConfig;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL vidxConfigPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricConfigVidxGet(dev, &vidxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.7. Call with vidxConfigPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricConfigVidxGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vidxConfigPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigVidxGet(dev, &vidxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigVidxGet(dev, &vidxConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigLbiModeSet
(
    IN  GT_U8                                  devNum,
    IN  CPSS_EXMXPM_FABRIC_CONFIG_LBI_MODE_ENT lbiMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigLbiModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with lbiMode [CPSS_EXMXPM_FABRIC_CONFIG_LBI_SRC_PORT_BASED_E /
                            CPSS_EXMXPM_FABRIC_CONFIG_LBI_HASH_BASED_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricConfigLbiModeGet with non-NULL lbiModePtr.
    Expected: GT_OK and the same lbiMode.
    1.3. Call with out of range lbiMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_CONFIG_LBI_MODE_ENT  lbiMode    = CPSS_EXMXPM_FABRIC_CONFIG_LBI_SRC_PORT_BASED_E;
    CPSS_EXMXPM_FABRIC_CONFIG_LBI_MODE_ENT  lbiModeGet = CPSS_EXMXPM_FABRIC_CONFIG_LBI_SRC_PORT_BASED_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with lbiMode [CPSS_EXMXPM_FABRIC_CONFIG_LBI_SRC_PORT_BASED_E /
                                    CPSS_EXMXPM_FABRIC_CONFIG_LBI_HASH_BASED_E].
            Expected: GT_OK.
        */

        /* Call with lbiMode [CPSS_EXMXPM_FABRIC_CONFIG_LBI_SRC_PORT_BASED_E] */
        lbiMode = CPSS_EXMXPM_FABRIC_CONFIG_LBI_SRC_PORT_BASED_E;

        st = cpssExMxPmFabricConfigLbiModeSet(dev, lbiMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lbiMode);

        /*
            1.2. Call cpssExMxPmFabricConfigLbiModeGet with non-NULL lbiModePtr.
            Expected: GT_OK and the same lbiMode.
        */
        st = cpssExMxPmFabricConfigLbiModeGet(dev, &lbiModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricConfigLbiModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(lbiMode, lbiModeGet,
                   "get another lbiMode than was set: %d", dev);

        /* Call with lbiMode [CPSS_EXMXPM_FABRIC_CONFIG_LBI_HASH_BASED_E] */
        lbiMode = CPSS_EXMXPM_FABRIC_CONFIG_LBI_HASH_BASED_E;

        st = cpssExMxPmFabricConfigLbiModeSet(dev, lbiMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lbiMode);

        /*
            1.2. Call cpssExMxPmFabricConfigLbiModeGet with non-NULL lbiModePtr.
            Expected: GT_OK and the same lbiMode.
        */
        st = cpssExMxPmFabricConfigLbiModeGet(dev, &lbiModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricConfigLbiModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(lbiMode, lbiModeGet,
                   "get another lbiMode than was set: %d", dev);

        /*
            1.3. Call function with out of range dropMode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        lbiMode = FABRIC_CONFIG_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricConfigLbiModeSet(dev, lbiMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, lbiMode);
    }

    lbiMode = CPSS_EXMXPM_FABRIC_CONFIG_LBI_SRC_PORT_BASED_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigLbiModeSet(dev, lbiMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigLbiModeSet(dev, lbiMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigLbiModeGet
(
    IN  GT_U8                                  devNum,
    OUT CPSS_EXMXPM_FABRIC_CONFIG_LBI_MODE_ENT *lbiModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigLbiModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null lbiModePtr.
    Expected: GT_OK.
    1.2. Call with lbiModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_CONFIG_LBI_MODE_ENT  lbiMode = CPSS_EXMXPM_FABRIC_CONFIG_LBI_SRC_PORT_BASED_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null lbiModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricConfigLbiModeGet(dev, &lbiMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null lbiModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricConfigLbiModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lbiModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigLbiModeGet(dev, &lbiMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigLbiModeGet(dev, &lbiMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigSrcPortToLbiMapSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  port,
    IN  GT_U32 lbi
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigSrcPortToLbiMapSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with lbi [0 / 1 / 2 / 3].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmFabricConfigSrcPortToLbiMapGet with non-NULL lbiPtr.
    Expected: GT_OK and the same lbi.
    1.1.3. Call with out of range lbi [4].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = FABRIC_CONFIG_VALID_VIRT_PORT_CNS;

    GT_U32      lbi    = 0;
    GT_U32      lbiGet = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with lbi [0 / 1 / 2 / 3].
                Expected: GT_OK.
            */

            /* Call with lbi [0] */
            lbi = 0;

            st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lbi);

            /*
                1.1.2. Call cpssExMxPmFabricConfigSrcPortToLbiMapGet with non-NULL lbiPtr.
                Expected: GT_OK and the same lbi.
            */
            st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbiGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmFabricConfigSrcPortToLbiMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(lbi, lbiGet,
                       "get another lbi than was set: %d, %d", dev, port);

            /* Call with lbi [1] */
            lbi = 1;

            st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lbi);

            /*
                1.1.2. Call cpssExMxPmFabricConfigSrcPortToLbiMapGet with non-NULL lbiPtr.
                Expected: GT_OK and the same lbi.
            */
            st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbiGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmFabricConfigSrcPortToLbiMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(lbi, lbiGet,
                       "get another lbi than was set: %d, %d", dev, port);

            /* Call with lbi [2] */
            lbi = 2;

            st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lbi);

            /*
                1.1.3. Call cpssExMxPmFabricConfigSrcPortToLbiMapGet with non-NULL lbiPtr.
                Expected: GT_OK and the same lbi.
            */
            st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbiGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmFabricConfigSrcPortToLbiMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(lbi, lbiGet,
                       "get another lbi than was set: %d, %d", dev, port);

            /* Call with lbi [3] */
            lbi = 3;

            st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lbi);

            /*
                1.1.4. Call cpssExMxPmFabricConfigSrcPortToLbiMapGet with non-NULL lbiPtr.
                Expected: GT_OK and the same lbi.
            */
            st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbiGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmFabricConfigSrcPortToLbiMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(lbi, lbiGet,
                       "get another lbi than was set: %d, %d", dev, port);

            /*
                1.1.5. Call with out of range lbi [4].
                Expected: NOT GT_OK.
            */
            lbi = 4;

            st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lbi);
        }

        lbi = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    lbi = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = FABRIC_CONFIG_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigSrcPortToLbiMapSet(dev, port, lbi);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigSrcPortToLbiMapGet
(
    IN  GT_U8  devNum,
    IN  GT_U8  port,
    OUT GT_U32 *lbiPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigSrcPortToLbiMapGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null lbiPtr.
    Expected: GT_OK.
    1.1.2. Call with lbiPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = FABRIC_CONFIG_VALID_VIRT_PORT_CNS;

    GT_U32      lbi = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL lbiPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbi);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with lbiPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, lbiPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbi);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbi);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbi);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = FABRIC_CONFIG_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbi);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigSrcPortToLbiMapGet(dev, port, &lbi);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigEccTypeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_EXMXPM_FABRIC_CONFIG_ECC_TYPE_ENT       eccType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigEccTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with eccType [CPSS_EXMXPM_FABRIC_CONFIG_ECC_ECC7_E /
                            CPSS_EXMXPM_FABRIC_CONFIG_ECC_CRC8_E]
    Expected: GT_OK
    1.2. Call cpssExMxPmFabricConfigEccTypeGet with non-NULL  eccTypePtr.
    Expected: GT_OK and the same eccType
    1.3. Call with out of range eccType [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_CONFIG_ECC_TYPE_ENT  eccType    = CPSS_EXMXPM_FABRIC_CONFIG_ECC_ECC7_E;
    CPSS_EXMXPM_FABRIC_CONFIG_ECC_TYPE_ENT  eccTypeGet = CPSS_EXMXPM_FABRIC_CONFIG_ECC_ECC7_E;;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with eccType [CPSS_EXMXPM_FABRIC_CONFIG_ECC_ECC7_E /
                                    CPSS_EXMXPM_FABRIC_CONFIG_ECC_CRC8_E]
            Expected: GT_OK
        */
        eccType = CPSS_EXMXPM_FABRIC_CONFIG_ECC_ECC7_E;

        st = cpssExMxPmFabricConfigEccTypeSet(dev, eccType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, eccType);

        /*
            1.2. Call cpssExMxPmFabricConfigEccTypeGet with non-NULL  eccTypePtr.
            Expected: GT_OK and the same eccType
        */
        st = cpssExMxPmFabricConfigEccTypeGet(dev, &eccTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricConfigEccTypeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(eccType, eccTypeGet,
                   "get another eccType than was set: %d", dev);

        /*
            1.1. Call with eccType [CPSS_EXMXPM_FABRIC_CONFIG_ECC_ECC7_E /
                                    CPSS_EXMXPM_FABRIC_CONFIG_ECC_CRC8_E]
            Expected: GT_OK
        */
        eccType = CPSS_EXMXPM_FABRIC_CONFIG_ECC_CRC8_E;

        st = cpssExMxPmFabricConfigEccTypeSet(dev, eccType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, eccType);

        /*
            1.2. Call cpssExMxPmFabricConfigEccTypeGet with non-NULL  eccTypePtr.
            Expected: GT_OK and the same eccType
        */
        st = cpssExMxPmFabricConfigEccTypeGet(dev, &eccTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricConfigEccTypeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(eccType, eccTypeGet,
                   "get another eccType than was set: %d", dev);

        /*
            1.3. Call with out of range eccType [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        eccType = FABRIC_CONFIG_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricConfigEccTypeSet(dev, eccType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, eccType);
    }

    eccType = CPSS_EXMXPM_FABRIC_CONFIG_ECC_ECC7_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigEccTypeSet(dev, eccType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigEccTypeSet(dev, eccType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigEccTypeGet
(
    IN  GT_U8                                       devNum,
    OUT CPSS_EXMXPM_FABRIC_CONFIG_ECC_TYPE_ENT      *eccTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigEccTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL eccTypePtr.
    Expected: GT_OK
    1.2. Call with eccTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_CONFIG_ECC_TYPE_ENT  eccType = CPSS_EXMXPM_FABRIC_CONFIG_ECC_ECC7_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL eccTypePtr.
            Expected: GT_OK
        */
        st = cpssExMxPmFabricConfigEccTypeGet(dev, &eccType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with eccTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricConfigEccTypeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, eccTypePrt = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigEccTypeGet(dev, &eccType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigEccTypeGet(dev, &eccType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  port,
    IN  GT_U32 lbiMsb
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with lbiMsb [0 / 1].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet with non-NULL lbiMsb.
    Expected: GT_OK and the same lbiMsb.
    1.1.3. Call with out of range lbiMsb [2].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = FABRIC_CONFIG_VALID_VIRT_PORT_CNS;

    GT_U32      lbiMsb    = 0;
    GT_U32      lbiMsbGet = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with lbiMsb [0 / 1].
                Expected: GT_OK.
            */

            /* Call with lbiMsb [0] */
            lbiMsb = 0;

            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet(dev, port, lbiMsb);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lbiMsb);

            /*
                1.1.2. Call cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet with non-NULL lbiMsb.
                Expected: GT_OK and the same lbiMsb.
            */
            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, &lbiMsbGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(lbiMsb, lbiMsbGet, "get another lbiMsb than was set: %d, %d", dev, port);

            /* Call with lbiMsb [1] */
            lbiMsb = 1;

            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet(dev, port, lbiMsb);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lbiMsb);

            /*
                1.1.2. Call cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet with non-NULL lbiMsb.
                Expected: GT_OK and the same lbiMsb.
            */
            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, &lbiMsbGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(lbiMsb, lbiMsbGet, "get another lbiMsb than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range lbiMsb [2].
                Expected: NOT GT_OK.
            */
            lbiMsb = 2;

            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet(dev, port, lbiMsb);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lbiMsb);
        }

        lbiMsb = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet(dev, port, lbiMsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet(dev, port, lbiMsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet(dev, port, lbiMsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    lbiMsb = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = FABRIC_CONFIG_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet(dev, port, lbiMsb);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet(dev, port, lbiMsb);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet
(
    IN  GT_U8  devNum,
    IN  GT_U8  port,
    OUT GT_U32 *lbiMsbPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null lbiMsbPtr.
    Expected: GT_OK.
    1.1.2. Call with lbiMsbPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = FABRIC_CONFIG_VALID_VIRT_PORT_CNS;

    GT_U32      lbiMsb = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL lbiMsbPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, &lbiMsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with lbiMsbPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, lbiMsbPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, &lbiMsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, &lbiMsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, &lbiMsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = FABRIC_CONFIG_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, &lbiMsb);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet(dev, port, &lbiMsb);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmFabricConfig suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmFabricConfig)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigVidxSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigVidxGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigLbiModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigLbiModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigSrcPortToLbiMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigSrcPortToLbiMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigEccTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigEccTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigTrgPortToLbiMsbMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricConfigTrgPortToLbiMsbMapGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmFabricConfig)

