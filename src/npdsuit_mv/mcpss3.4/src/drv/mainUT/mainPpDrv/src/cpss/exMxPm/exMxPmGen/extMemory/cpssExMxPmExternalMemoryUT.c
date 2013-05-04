/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmExternalMemoryUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmExternalMemory, that provides
*       CPSS ExMxPm External Memory Controllers API definitions.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/extMemory/cpssExMxPmExternalMemory.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define EXT_MEMORY_INVALID_ENUM_CNS            0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalMemoryParityErrorCntrGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_ENT    memType,
    OUT GT_U32                                  *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalMemoryParityErrorCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with memType [CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_CONTROL_CSU_E / 
                            CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_LPM_0_E] 
                   and not NULL cntrPtr.
    Expected: GT_OK.
    1.2. Call with memType [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.3. Call with cntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_ENT    memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_INGRESS_DATA_DSU_0_E;
    GT_U32                                  cntr = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memType [CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_CONTROL_CSU_E / 
                                    CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_LPM_0_E] 
                           and not NULL cntrPtr.
            Expected: GT_OK.
        */
        memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_CONTROL_CSU_E;

        st = cpssExMxPmExternalMemoryParityErrorCntrGet(dev, memType, &cntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_LPM_0_E;

        st = cpssExMxPmExternalMemoryParityErrorCntrGet(dev, memType, &cntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        /*
            1.2. Call with memType [0x5AAAAAA5] (out of range).
            Expected: GT_BAD_PARAM.
        */
        memType = EXT_MEMORY_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalMemoryParityErrorCntrGet(dev, memType, &cntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, memType);

        memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_LPM_0_E;

        /*
            1.3. Call with cntrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalMemoryParityErrorCntrGet(dev, memType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntr = NULL", dev);
    }

    memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_CONTROL_CSU_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalMemoryParityErrorCntrGet(dev, memType, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalMemoryParityErrorCntrGet(dev, memType, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalMemoryCsuEccErrorCntrGet
(
    IN  GT_U8                                   devNum,
    OUT GT_U32                                  *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalMemoryCsuEccErrorCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL cntrPtr.
    Expected: GT_OK.
    1.2. Call with cntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      cntr = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL cntrPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmExternalMemoryCsuEccErrorCntrGet(dev, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call with cntrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalMemoryCsuEccErrorCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalMemoryCsuEccErrorCntrGet(dev, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalMemoryCsuEccErrorCntrGet(dev, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalMemoryLastErrorAddressGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_ENT    memType,
    OUT GT_U32                                  *addrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalMemoryLastErrorAddressGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with memType [CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_CONTROL_CSU_E / 
                            CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_LPM_0_E] 
                   and not NULL addrPtr.
    Expected: GT_OK.
    1.2. Call with memType [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.3. Call with addrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_ENT    memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_INGRESS_DATA_DSU_0_E;
    GT_U32                                  addr = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memType [CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_CONTROL_CSU_E / 
                                    CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_LPM_0_E] 
                           and not NULL addrPtr.
            Expected: GT_OK.
        */
        memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_CONTROL_CSU_E;

        st = cpssExMxPmExternalMemoryLastErrorAddressGet(dev, memType, &addr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_LPM_0_E;

        st = cpssExMxPmExternalMemoryLastErrorAddressGet(dev, memType, &addr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        /*
            1.2. Call with memType [0x5AAAAAA5] (out of range).
            Expected: GT_BAD_PARAM.
        */
        memType = EXT_MEMORY_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalMemoryLastErrorAddressGet(dev, memType, &addr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, memType);

        memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_LPM_0_E;

        /*
            1.3. Call with addrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalMemoryLastErrorAddressGet(dev, memType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, addr = NULL", dev);
    }

    memType = CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_CONTROL_CSU_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalMemoryLastErrorAddressGet(dev, memType, &addr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalMemoryLastErrorAddressGet(dev, memType, &addr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmExternalMemory suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmExternalMemory)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalMemoryParityErrorCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalMemoryCsuEccErrorCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalMemoryLastErrorAddressGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmExternalMemory)
