/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_diagnosticsUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_diagnostics, that provides
*       Diagnostics APIs of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_diagnostics.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_DIAGNOSTICS_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_diagnostics_set_prbs_mode(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT     *prbs_setup_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_diagnostics_set_prbs_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with prbs_setup_ptr { prbs_generator_checker_unit[0..FX950_DIAGNOSTICS_PRBS_NOF_UNITS] {
                                                 prbs_test_mode_enable [TRUE / FALSE],
                                                 prbs_test_mode [FX950_DIAGNOSTICS_PRBS_TEST_CYCLIC_DATA / FX950_DIAGNOSTICS_PRBS_TEST_PRBS_7],
                                                 activate_prbs_generator [0 / 1],
                                                 activate_prbs_checker [0 / 1],
                                                 valid [0 / 1] } }.
    Expected: SAND_OK.
    1.2. Call with fx950_api_diagnostics_get_prbs_mode with not NULL pointer.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with prbs_setup_ptr { prbs_generator_checker_unit[0] {prbs_test_mode [0x5AAAAAA5] } (out of range)
                   and other params from 1.1.
    Expectede: SAND_OK.
    1.4. Call with prbs_setup_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_U8   index     = 0;
    SAND_UINT device_id = 0;

    FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT prbs_setup;
    FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT prbs_setupGet;

  
    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with prbs_setup_ptr { prbs_generator_checker_unit[0..FX950_DIAGNOSTICS_PRBS_NOF_UNITS] {
                                                         prbs_test_mode_enable [TRUE / FALSE],
                                                         prbs_test_mode [FX950_DIAGNOSTICS_PRBS_TEST_CYCLIC_DATA / FX950_DIAGNOSTICS_PRBS_TEST_PRBS_7],
                                                         activate_prbs_generator [0 / 1],
                                                         activate_prbs_checker [0 / 1],
                                                         valid [0 / 1] } }.
            Expected: SAND_OK.
        */
        /* iterate with prbs_test_mode_enable = TRUE */
        for(index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++)
        {
            prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode_enable = TRUE;
            prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode = FX950_DIAGNOSTICS_PRBS_TEST_CYCLIC_DATA;
            prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator = 0;
            prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker = 0;
            prbs_setup.prbs_generator_checker_unit[index].valid = 1;
        }

        result = fx950_api_diagnostics_set_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with fx950_api_diagnostics_get_prbs_mode with not NULL pointer.
            Expected: SAND_OK and the same params as was set.
        */
        for(index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++)
        {
            prbs_setupGet.prbs_generator_checker_unit[index].valid = 1;
        }
        result = fx950_api_diagnostics_get_prbs_mode(device_id, &prbs_setupGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_diagnostics_get_prbs_mode: %d", device_id);

        /* compare  prbs_setup and prbs_setupGet */
        for(index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode_enable, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].prbs_test_mode_enable, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].prbs_test_mode_enable than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].prbs_test_mode, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].prbs_test_mode than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_generator, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].activate_prbs_generator than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_checker, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].activate_prbs_checker than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].valid, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].valid, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].valid than was set: %d", index, device_id);
        }

        /* iterate with prbs_test_mode_enable = FALSE */
        for(index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++)
        {
            prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode_enable = FALSE;
            prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode = FX950_DIAGNOSTICS_PRBS_TEST_PRBS_7;
            prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator = 1;
            prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker = 1;
            prbs_setup.prbs_generator_checker_unit[index].valid = 1;
        }

        result = fx950_api_diagnostics_set_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with fx950_api_diagnostics_get_prbs_mode with not NULL pointer.
            Expected: SAND_OK and the same params as was set.
        */
        for(index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++)
        {
            prbs_setupGet.prbs_generator_checker_unit[index].valid = 1;
        }
        result = fx950_api_diagnostics_get_prbs_mode(device_id, &prbs_setupGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_diagnostics_get_prbs_mode: %d", device_id);

        /* compare  prbs_setup and prbs_setupGet */
        for(index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode_enable, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].prbs_test_mode_enable, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].prbs_test_mode_enable than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].prbs_test_mode, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].prbs_test_mode than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_generator, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].activate_prbs_generator than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_checker, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].activate_prbs_checker than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].valid, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].valid, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].valid than was set: %d", index, device_id);
        }

        /*
            1.3. Call with prbs_setup_ptr { prbs_generator_checker_unit[0] {prbs_test_mode [0x5AAAAAA5] } (out of range)
                           and other params from 1.1.
            Expectede: NOT SAND_OK.
        */

        prbs_setup.prbs_generator_checker_unit[0].prbs_test_mode_enable = TRUE;
        prbs_setup.prbs_generator_checker_unit[0].prbs_test_mode = SAND_UTF_DIAGNOSTICS_INVALID_ENUM;

        result = fx950_api_diagnostics_set_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, prbs_setup.prbs_generator_checker_unit[0].prbs_test_mode = %d",
                                         device_id, prbs_setup.prbs_generator_checker_unit[0].prbs_test_mode);

        prbs_setup.prbs_generator_checker_unit[0].prbs_test_mode = FX950_DIAGNOSTICS_PRBS_TEST_CYCLIC_DATA;

        /*
            1.4. Call with prbs_setup_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_diagnostics_set_prbs_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, prbs_setup = NULL", device_id);
    }

    for(index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++)
    {
        prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode_enable = TRUE;
        prbs_setup.prbs_generator_checker_unit[index].prbs_test_mode = FX950_DIAGNOSTICS_PRBS_TEST_CYCLIC_DATA;
        prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator = 0;
        prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker = 0;
        prbs_setup.prbs_generator_checker_unit[index].valid = 0;
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_diagnostics_set_prbs_mode(device_id, &prbs_setup);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_diagnostics_set_prbs_mode(device_id, &prbs_setup);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_diagnostics_get_prbs_mode(
    SAND_IN     unsigned int                            device_id,
    SAND_INOUT  FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT     *prbs_setup_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_diagnostics_get_prbs_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL prbs_setup_ptr.
    Expected: SAND_OK.
    1.2. Call with prbs_setup_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT prbs_setup;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL prbs_setup_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_diagnostics_get_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with prbs_setup_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_diagnostics_get_prbs_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, prbs_setup = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_diagnostics_get_prbs_mode(device_id, &prbs_setup);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_diagnostics_get_prbs_mode(device_id, &prbs_setup);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_diagnostics_get_prbs_counters(
    SAND_IN     unsigned int                                    device_id,
    SAND_OUT    FX950_DIAGNOSTICS_PRBS_COUNTERS_REPORT_STRUCT   *prbs_counters_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_diagnostics_get_prbs_counters)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL prbs_counters_ptr.
    Expected: SAND_OK.
    1.2. Call with prbs_counters_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FX950_DIAGNOSTICS_PRBS_COUNTERS_REPORT_STRUCT prbs_counters;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL prbs_counters_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_diagnostics_get_prbs_counters(device_id, &prbs_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with prbs_counters_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_diagnostics_get_prbs_counters(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, prbs_counters_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_diagnostics_get_prbs_counters(device_id, &prbs_counters);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_diagnostics_get_prbs_counters(device_id, &prbs_counters);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_diagnostics suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_diagnostics)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_diagnostics_set_prbs_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_diagnostics_get_prbs_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_diagnostics_get_prbs_counters)
UTF_SUIT_END_TESTS_MAC(fx950_api_diagnostics)

