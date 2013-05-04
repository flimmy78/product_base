/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPhySmiPreInitUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChPhySmiPreInit.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmiPreInit.h>
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/private/prvCpssDxChPhySmi.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define PHY_SMI_VALID_PHY_PORT_CNS  0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyAutoPollNumOfPortsSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi0,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi1
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyAutoPollNumOfPortsSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with all acceptable combinations.
        The acceptable combinations for Auto Poll number of ports are:
        |-----------------------|
        |   SMI 0   |   SMI 1   |
        |-----------|-----------|
        |     8     |    16     |
        |     8     |    12     |
        |    12     |    12     |
        |    16     |    8      |
        |-----------------------|
    Expected: GT_OK for each combination.
    1.2. Call cpssDxChPhyAutoPollNumOfPortsGet with the same params.
    Expected: GT_OK and the same value.
    1.3. Call with wrong combination [8 - 8].
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values autoPollNumOfPortsSmi0.
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong enum values autoPollNumOfPortsSmi1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT autoPollNumOfPortsSmi0 = 0;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT autoPollNumOfPortsSmi1 = 0;

    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT autoPollNumOfPortsSmi0Get = 1;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT autoPollNumOfPortsSmi1Get = 1;

    /* prepare iterator for go over all active devices, ALL families*/
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all acceptable combinations.
                The acceptable combinations for Auto Poll number of ports are:
                |-----------------------|
                |   SMI 0   |   SMI 1   |
                |-----------|-----------|
                |     8     |    16     |
                |     8     |    12     |
                |    12     |    12     |
                |    16     |    8      |
                |-----------------------|
            Expected: GT_OK for each combination.
        */

        /* call with  8 - 16 combination */
        autoPollNumOfPortsSmi0 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E;
        autoPollNumOfPortsSmi1 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E;

        st = cpssDxChPhyAutoPollNumOfPortsSet(dev, autoPollNumOfPortsSmi0,
                                                   autoPollNumOfPortsSmi1);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPhyAutoPollNumOfPortsGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChPhyAutoPollNumOfPortsGet(dev, &autoPollNumOfPortsSmi0Get,
                                                   &autoPollNumOfPortsSmi1Get);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(autoPollNumOfPortsSmi0,
                                     autoPollNumOfPortsSmi0Get,
            "get another autoPollNumOfPortsSmi0 than was set: dev = %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(autoPollNumOfPortsSmi1,
                                     autoPollNumOfPortsSmi1Get,
            "get another autoPollNumOfPortsSmi1 than was set: dev = %d", dev);


        /* call with  8 - 12 combination */
        autoPollNumOfPortsSmi0 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E;
        autoPollNumOfPortsSmi1 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E;

        st = cpssDxChPhyAutoPollNumOfPortsSet(dev, autoPollNumOfPortsSmi0,
                                                   autoPollNumOfPortsSmi1);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPhyAutoPollNumOfPortsGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChPhyAutoPollNumOfPortsGet(dev, &autoPollNumOfPortsSmi0Get,
                                                   &autoPollNumOfPortsSmi1Get);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(autoPollNumOfPortsSmi0,
                                     autoPollNumOfPortsSmi0Get,
            "get another autoPollNumOfPortsSmi0 than was set: dev = %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(autoPollNumOfPortsSmi1,
                                     autoPollNumOfPortsSmi1Get,
            "get another autoPollNumOfPortsSmi1 than was set: dev = %d", dev);


        /* call with  12 - 12 combination */
        autoPollNumOfPortsSmi0 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E;
        autoPollNumOfPortsSmi1 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E;

        st = cpssDxChPhyAutoPollNumOfPortsSet(dev, autoPollNumOfPortsSmi0,
                                                   autoPollNumOfPortsSmi1);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPhyAutoPollNumOfPortsGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChPhyAutoPollNumOfPortsGet(dev, &autoPollNumOfPortsSmi0Get,
                                                   &autoPollNumOfPortsSmi1Get);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(autoPollNumOfPortsSmi0,
                                     autoPollNumOfPortsSmi0Get,
            "get another autoPollNumOfPortsSmi0 than was set: dev = %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(autoPollNumOfPortsSmi1,
                                     autoPollNumOfPortsSmi1Get,
            "get another autoPollNumOfPortsSmi1 than was set: dev = %d", dev);


        /* call with  16 - 8 combination */
        autoPollNumOfPortsSmi0 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E;
        autoPollNumOfPortsSmi1 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E;

        st = cpssDxChPhyAutoPollNumOfPortsSet(dev, autoPollNumOfPortsSmi0,
                                                   autoPollNumOfPortsSmi1);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPhyAutoPollNumOfPortsGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChPhyAutoPollNumOfPortsGet(dev, &autoPollNumOfPortsSmi0Get,
                                                   &autoPollNumOfPortsSmi1Get);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(autoPollNumOfPortsSmi0,
                                     autoPollNumOfPortsSmi0Get,
            "get another autoPollNumOfPortsSmi0 than was set: dev = %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(autoPollNumOfPortsSmi1,
                                     autoPollNumOfPortsSmi1Get,
            "get another autoPollNumOfPortsSmi1 than was set: dev = %d", dev);

        /*
            1.3. Call with wrong combination [8 - 8].
            Expected: GT_BAD_PARAM.
        */

        autoPollNumOfPortsSmi0 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E;
        autoPollNumOfPortsSmi1 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E;

        st = cpssDxChPhyAutoPollNumOfPortsSet(dev, autoPollNumOfPortsSmi0,
                                                   autoPollNumOfPortsSmi1);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.4. Call with wrong enum values autoPollNumOfPortsSmi0.
            Expected: GT_BAD_PARAM.
        */
        autoPollNumOfPortsSmi1 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChPhyAutoPollNumOfPortsSet
                            (dev, autoPollNumOfPortsSmi0, autoPollNumOfPortsSmi1),
                            autoPollNumOfPortsSmi0);

        /*
            1.4. Call with wrong enum values autoPollNumOfPortsSmi1.
            Expected: GT_BAD_PARAM.
        */
        autoPollNumOfPortsSmi0 = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChPhyAutoPollNumOfPortsSet
                            (dev, autoPollNumOfPortsSmi0, autoPollNumOfPortsSmi1),
                            autoPollNumOfPortsSmi1);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator - iterate non-active devices*/
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyAutoPollNumOfPortsSet(dev, autoPollNumOfPortsSmi0,
                                                   autoPollNumOfPortsSmi1);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPhyAutoPollNumOfPortsSet(dev, autoPollNumOfPortsSmi0,
                                               autoPollNumOfPortsSmi1);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyAutoPollNumOfPortsGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi0Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi1Ptr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyAutoPollNumOfPortsGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with not null pointers.
    Expected: GT_OK.
    1.2. Call with wrong autoPollNumOfPortsSmi0Ptr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with wrong autoPollNumOfPortsSmi1Ptr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT autoPollNumOfPortsSmi0Get = 1;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT autoPollNumOfPortsSmi1Get = 1;

    /* prepare iterator for go over all active devices, ALL families*/
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null pointers.
            Expected: GT_OK.
        */

        st = cpssDxChPhyAutoPollNumOfPortsGet(dev, &autoPollNumOfPortsSmi0Get,
                                                   &autoPollNumOfPortsSmi1Get);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong autoPollNumOfPortsSmi0Ptr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChPhyAutoPollNumOfPortsGet(dev, NULL, &autoPollNumOfPortsSmi1Get);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with wrong autoPollNumOfPortsSmi1Ptr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChPhyAutoPollNumOfPortsGet(dev, &autoPollNumOfPortsSmi0Get, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator - iterate non-active devices*/
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyAutoPollNumOfPortsGet(dev, &autoPollNumOfPortsSmi0Get,
                                                   &autoPollNumOfPortsSmi1Get);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPhyAutoPollNumOfPortsGet(dev, &autoPollNumOfPortsSmi0Get,
                                               &autoPollNumOfPortsSmi1Get);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChPhySmiPreInit suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChPhySmiPreInit)

    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyAutoPollNumOfPortsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyAutoPollNumOfPortsGet)

UTF_SUIT_END_TESTS_MAC(cpssDxChPhySmiPreInit)


