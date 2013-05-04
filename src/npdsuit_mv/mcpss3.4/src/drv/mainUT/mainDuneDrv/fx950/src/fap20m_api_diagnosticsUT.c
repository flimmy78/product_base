/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_diagnosticsUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_diagnostics, that provides
*       diagnostics API of FAP20M device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_diagnostics.h>
#include <SAND/Utils/include/sand_os_interface.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_API_DIAGNOSTICS_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_last_fabric_header_get(
    SAND_IN   unsigned int              device_id,
    SAND_OUT  FAP20M_LAST_PACKET_HEADER *hrd
  )
*/
UTF_TEST_CASE_MAC(fap20m_last_fabric_header_get)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null hrd. 
    Expected: SAND_OK.
    1.2. Call with hrd [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_LAST_PACKET_HEADER hrd;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with non-null hrd.
            Expected: SAND_OK.
        */
        result = fap20m_last_fabric_header_get(device_id, &hrd);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with hrd [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_last_fabric_header_get(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, hrd = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_last_fabric_header_get(device_id, &hrd);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_last_fabric_header_get(device_id, &hrd);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_prbs_mode(
    unsigned int       device_id,
    FAP20M_PRBS_SETUP* prbs_setup
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_prbs_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with prbs_setup { prbs_generator_checker_unit[] { activate_prbs_generator [0 / 1],
                                                                activate_prbs_checker [0 / 1],
                                                                local_loopback_on [0 / 1],
                                                                valid [0 / 1] } }
    Expected: SAND_OK.
    1.2. Call fap20m_get_prbs_mode with not NULL prbs_setup.
    Expected: SAND_OK and the same prbs_setup as was set.
    1.3. Call with prbs_setup [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  index = 0;
    SAND_UINT  device_id = 0;

    FAP20M_PRBS_SETUP prbs_setup;
    FAP20M_PRBS_SETUP prbs_setupGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with prbs_setup { prbs_generator_checker_unit[] { activate_prbs_generator [0 / 1],
                                                                        activate_prbs_checker [0 / 1],
                                                                        local_loopback_on [0 / 1],
                                                                        valid [0 / 1] } }
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        for(index = 0; index < FAP20M_PRBS_NOF_UNITS; index++)
        {
            prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator = 0;
            prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker = 0;
            prbs_setup.prbs_generator_checker_unit[index].local_loopback_on = 0;
            prbs_setup.prbs_generator_checker_unit[index].valid = 0;
        }

        result = fap20m_set_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_prbs_mode with not NULL prbs_setup.
            Expected: SAND_OK and the same prbs_setup as was set.
        */
        for(index = 0; index < FAP20M_PRBS_NOF_UNITS; index++)
        {
            prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_generator = 0;
            prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_checker = 0;
            prbs_setupGet.prbs_generator_checker_unit[index].local_loopback_on = 0;
            prbs_setupGet.prbs_generator_checker_unit[index].valid = 0;
        }
        result = fap20m_get_prbs_mode(device_id, &prbs_setupGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_prbs_mode = %d", device_id);

        /* verifying values */
        for(index = 0; index < FAP20M_PRBS_NOF_UNITS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_generator, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].activate_prbs_generator than was set: %d", 
                                         index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_checker, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].activate_prbs_checker than was set: %d", 
                                         index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].local_loopback_on, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].local_loopback_on, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].local_loopback_on than was set: %d", 
                                         index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].valid, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].valid, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].valid than was set: %d", 
                                         index, device_id);
        }

        /* iterate with 1 */
        for(index = 0; index < FAP20M_PRBS_NOF_UNITS; index++)
        {
            prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator = 1;
            prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker = 1;
            prbs_setup.prbs_generator_checker_unit[index].local_loopback_on = 1;
            prbs_setup.prbs_generator_checker_unit[index].valid = 1;
        }

        result = fap20m_set_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_prbs_mode with not NULL prbs_setup.
            Expected: SAND_OK and the same prbs_setup as was set.
        */

        for(index = 0; index < FAP20M_PRBS_NOF_UNITS; index++)
        {
            prbs_setupGet.prbs_generator_checker_unit[index].valid = 1;
        }
        result = fap20m_get_prbs_mode(device_id, &prbs_setupGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_prbs_mode = %d", device_id);

        /* verifying values */
        for(index = 0; index < FAP20M_PRBS_NOF_UNITS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_generator, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].activate_prbs_generator than was set: %d", 
                                         index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].activate_prbs_checker, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].activate_prbs_checker than was set: %d", 
                                         index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].local_loopback_on, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].local_loopback_on, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].local_loopback_on than was set: %d", 
                                         index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(prbs_setup.prbs_generator_checker_unit[index].valid, 
                                         prbs_setupGet.prbs_generator_checker_unit[index].valid, 
                                         "got another prbs_setup.prbs_generator_checker_unit[%d].valid than was set: %d", 
                                         index, device_id);
        }

        /*
            1.3. Call with prbs_setup [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_prbs_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, prbs_setup = NULL", device_id);
    }

    for(index = 0; index < FAP20M_PRBS_NOF_UNITS; index++)
    {
        prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator = 0;
        prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker = 0;
        prbs_setup.prbs_generator_checker_unit[index].local_loopback_on = 0;
        prbs_setup.prbs_generator_checker_unit[index].valid = 0;
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_prbs_mode(device_id, &prbs_setup);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_prbs_mode(
    unsigned int       device_id,
    FAP20M_PRBS_SETUP* prbs_setup
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_prbs_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null prbs_setup. 
    Expected: SAND_OK.
    1.2. Call with prbs_setup [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_PRBS_SETUP prbs_setup;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with non-null prbs_setup.
            Expected: SAND_OK.
        */
        result = fap20m_get_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with prbs_setup [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_prbs_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, prbs_setup = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_prbs_mode(device_id, &prbs_setup);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_prbs_mode(device_id, &prbs_setup);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_start_prbs_test(
    unsigned int         device_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_start_prbs_test)
{
/*
    ITERATE_DEVICES
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        result = fap20m_start_prbs_test(device_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_start_prbs_test(device_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_start_prbs_test(device_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_stop_prbs_test(
    unsigned int         device_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_stop_prbs_test)
{
/*
    ITERATE_DEVICES
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        result = fap20m_stop_prbs_test(device_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_stop_prbs_test(device_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_stop_prbs_test(device_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_ber_counters(
    unsigned int         device_id,
    FAP20M_BER_COUNTERS_REPORT* ber_counters
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_ber_counters)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null ber_counters. 
    Expected: SAND_OK.
    1.2. Call with ber_counters [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_BER_COUNTERS_REPORT ber_counters;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with non-null ber_counters.
            Expected: SAND_OK.
        */
        result = fap20m_get_ber_counters(device_id, &ber_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with ber_counters [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_ber_counters(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ber_counters = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_ber_counters(device_id, &ber_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_ber_counters(device_id, &ber_counters);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_iddr_read(
    unsigned int   device_id,
    unsigned long  dram_offset,
    unsigned long  data[FAP20M_IDRAM_WORD_NOF_LONGS]
  )
*/
UTF_TEST_CASE_MAC(fap20m_iddr_read)
{
/*
    ITERATE_DEVICES
    1.1. Call with dram_offset [0 / 255], non-null data. 
    Expected: SAND_OK.
    1.2. Call with data [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   dram_offset = 0;
    SAND_U32   data[FAP20M_IDRAM_WORD_NOF_LONGS];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with dram_offset [0 / 255], non-null data. 
            Expected: SAND_OK.
        */
        /* iterate with dram_offset = 0 */
        dram_offset = 0;

        result = fap20m_iddr_read(device_id, dram_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, dram_offset);

        /* iterate with dram_offset = 255 */
        dram_offset = 255;

        result = fap20m_iddr_read(device_id, dram_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, dram_offset);

        /*
            1.2. Call with data [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_iddr_read(device_id, dram_offset, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, data = NULL", device_id);
    }

    dram_offset = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_iddr_read(device_id, dram_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_iddr_read(device_id, dram_offset, data);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_iddr_write(
    unsigned int   device_id,
    unsigned long  dram_offset,
    unsigned long  data[FAP20M_IDRAM_WORD_NOF_LONGS]
  )
*/
UTF_TEST_CASE_MAC(fap20m_iddr_write)
{
/*
    ITERATE_DEVICES
    1.1. Call with dram_offset[0 / 255]
                   data {0,..,0 / 1,..,1}.
    Expected: SAND_OK.
    1.2. Call fap20m_iddr_read with not NULL data and dram_offset from 1.1.
    Expected: SAND_OK and the same data as was set.
    1.3. Call with data [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id   = 0;
    SAND_UINT  index       = 0;
    SAND_U32   dram_offset = 0;

    SAND_U32   data[FAP20M_IDRAM_WORD_NOF_LONGS];
    SAND_U32   dataGet[FAP20M_IDRAM_WORD_NOF_LONGS];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with dram_offset[0 / 255]
                           data {0,..,0 / 1,..,1}.
            Expected: SAND_OK.
        */
        /* iterate with dram_offset = 0 */
        dram_offset = 0;

        for(index=0; index<FAP20M_IDRAM_WORD_NOF_LONGS; index++)
        {
            data[index] = 0;
        }
        
        result = fap20m_iddr_write(device_id, dram_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, dram_offset);

        /*
            1.2. Call fap20m_iddr_read with not NULL data and dram_offset from 1.1.
            Expected: SAND_OK and the same data as was set.
        */
        result = fap20m_iddr_read(device_id, dram_offset, dataGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_iddr_read: %d, %d", device_id, dram_offset);

        /* verifying values */
        for(index=0; index<FAP20M_PRBS_NOF_UNITS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(data[0], dataGet[0], "got data[%d] than was set: %d", 
                                         index, device_id);
        }

        /* iterate with dram_offset = 255 */
        dram_offset = 255;

        for(index=0; index<FAP20M_IDRAM_WORD_NOF_LONGS; index++)
        {
            data[index] = 1;
        }
        
        result = fap20m_iddr_write(device_id, dram_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, dram_offset);

        /*
            1.2. Call fap20m_iddr_read with not NULL data and dram_offset from 1.1.
            Expected: SAND_OK and the same data as was set.
        */
        result = fap20m_iddr_read(device_id, dram_offset, dataGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_iddr_read: %d, %d", device_id, dram_offset);

        /* verifying values */
        for(index=0; index<FAP20M_PRBS_NOF_UNITS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(data[0], dataGet[0], "got data[%d] than was set: %d", 
                                         index, device_id);
        }

        /*
            1.3. Call with data [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_iddr_write(device_id, dram_offset, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, data = NULL", device_id);
    }

    dram_offset = 0;

    for(index=0; index<FAP20M_IDRAM_WORD_NOF_LONGS; index++)
    {
        data[index] = 0;
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_iddr_write(device_id, dram_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_iddr_write(device_id, dram_offset, data);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_qdr_read(
    SAND_IN   unsigned int    device_id,
    SAND_IN   unsigned long   qdr_offset,
    SAND_OUT  unsigned long   data[FAP20M_QDR_WORD_NOF_LONGS]
  )
*/
UTF_TEST_CASE_MAC(fap20m_qdr_read)
{
/*
    ITERATE_DEVICES
    1.1. Call with qdr_offset [0 / 68], non-null data. 
    Expected: SAND_OK.
    1.2. Call with data [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   qdr_offset = 0;
    SAND_U32   data[FAP20M_QDR_WORD_NOF_LONGS];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with qdr_offset [0 / 68], non-null data. 
            Expected: SAND_OK.
        */
        /* iterate with qdr_offset = 0 */
        qdr_offset = 0;

        result = fap20m_qdr_read(device_id, qdr_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qdr_offset);

        /* iterate with qdr_offset = 68 */
        qdr_offset = 68;

        result = fap20m_qdr_read(device_id, qdr_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qdr_offset);

        /*
            1.2. Call with data [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_qdr_read(device_id, qdr_offset, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, data = NULL", device_id);
    }

    qdr_offset = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_qdr_read(device_id, qdr_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_qdr_read(device_id, qdr_offset, data);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_qdr_write(
    SAND_IN   unsigned int    device_id,
    SAND_IN   unsigned long   qdr_offset,
    SAND_IN   unsigned long   data[FAP20M_QDR_WORD_NOF_LONGS]
  )
*/
UTF_TEST_CASE_MAC(fap20m_qdr_write)
{
/*
    ITERATE_DEVICES
    1.1. Call with qdr_offset[0 / 68]
                   data {0,..,0 / 1,..,1}.
    Expected: SAND_OK.
    1.2. Call fap20m_qdr_read with not NULL data and qdr_offset from 1.1.
    Expected: SAND_OK and the same data as was set.
    1.3. Call with data [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id  = 0;
    SAND_UINT  index      = 0;
    SAND_U32   qdr_offset = 0;

    SAND_U32   data[FAP20M_QDR_WORD_NOF_LONGS];
    SAND_U32   dataGet[FAP20M_QDR_WORD_NOF_LONGS];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with qdr_offset[0 / 68]
                           data {0,..,0 / 1,..,1}.
            Expected: SAND_OK.
        */
        /* iterate with qdr_offset = 0 */
        qdr_offset = 0;

        for(index=0; index<FAP20M_QDR_WORD_NOF_LONGS; index++)
        {
            data[index] = 0;
        }
        
        result = fap20m_qdr_write(device_id, qdr_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qdr_offset);

        /*
            1.2. Call fap20m_qdr_read with not NULL data and dram_offset from 1.1.
            Expected: SAND_OK and the same data as was set.
        */
        result = fap20m_qdr_read(device_id, qdr_offset, dataGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_qdr_read: %d, %d", device_id, qdr_offset);

        /* verifying values */
        for(index=0; index<FAP20M_QDR_WORD_NOF_LONGS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(data[0], dataGet[0], "got data[%d] than was set: %d", 
                                         index, device_id);
        }

        /* iterate with dram_offset = 68 */
        qdr_offset = 68;

        for(index=0; index<FAP20M_QDR_WORD_NOF_LONGS; index++)
        {
            data[index] = 1;
        }
        
        result = fap20m_qdr_write(device_id, qdr_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qdr_offset);

        /*
            1.2. Call fap20m_iddr_read with not NULL data and qdr_offset from 1.1.
            Expected: SAND_OK and the same data as was set.
        */
        result = fap20m_qdr_read(device_id, qdr_offset, dataGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_qdr_read: %d, %d", device_id, qdr_offset);

        /* verifying values */
        for(index=0; index<FAP20M_QDR_WORD_NOF_LONGS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(data[0], dataGet[0], "got data[%d] than was set: %d", 
                                         index, device_id);
        }

        /*
            1.3. Call with data [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_qdr_write(device_id, qdr_offset, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, data = NULL", device_id);
    }

    qdr_offset = 0;

    for(index=0; index<FAP20M_QDR_WORD_NOF_LONGS; index++)
    {
        data[index] = 0;
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_qdr_write(device_id, qdr_offset, data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_qdr_write(device_id, qdr_offset, data);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_qdr_get_num_sft_err(
    unsigned int  device_id,
    FAP20M_QDR_GET_NUM_SFT_ERR_REPORT
                  *fap20m_qdr_get_num_sft_err_report
  )
*/
UTF_TEST_CASE_MAC(fap20m_qdr_get_num_sft_err)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null fap20m_qdr_get_num_sft_err_report. 
    Expected: NOT SAND_OK because the function may be called only 
     as callback of relevant interrupt
    1.2. Call with fap20m_qdr_get_num_sft_err_report [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_QDR_GET_NUM_SFT_ERR_REPORT fap20m_qdr_get_num_sft_err_report;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with non-null countValuePtr.
            Expected: NOT SAND_OK.
        */
        result = fap20m_qdr_get_num_sft_err(device_id, &fap20m_qdr_get_num_sft_err_report);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with fap20m_qdr_get_num_sft_err_report [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_qdr_get_num_sft_err(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc,
                                     "%d, fap20m_qdr_get_num_sft_err_report = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_qdr_get_num_sft_err(device_id, &fap20m_qdr_get_num_sft_err_report);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_qdr_get_num_sft_err(device_id, &fap20m_qdr_get_num_sft_err_report);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_QDP_init_sequence(
    unsigned int  device_id,
    unsigned long wait_time_in_nano_sec,
    unsigned int* device_is_working
  )
*/
UTF_TEST_CASE_MAC(fap20m_QDP_init_sequence)
{
/*
    ITERATE_DEVICES
    1.1. Call with wait_time_in_nano_sec [0 / 1000000], 
                   not NULL device_is_working.
    Expected: SAND_OK.
    1.2. Call with device_is_working [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   wait_time_in_nano_sec = 0;
    SAND_UINT  device_is_working = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with wait_time_in_nano_sec [0 / 1000000], 
                           not NULL device_is_working.
            Expected: SAND_OK.
        */
        /* iterate with wait_time_in_nano_sec = 0 */
        wait_time_in_nano_sec = 0;
        
        result = fap20m_QDP_init_sequence(device_id, wait_time_in_nano_sec, &device_is_working);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, wait_time_in_nano_sec, device_is_working);

        /* iterate with wait_time_in_nano_sec = 1000000 */
        wait_time_in_nano_sec = 1000000;

        result = fap20m_QDP_init_sequence(device_id, wait_time_in_nano_sec, &device_is_working);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, wait_time_in_nano_sec, device_is_working);

        /*
            1.2. Call with device_is_working [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_QDP_init_sequence(device_id, wait_time_in_nano_sec, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, device_is_working = NULL", device_id);
    }

    wait_time_in_nano_sec = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_QDP_init_sequence(device_id, wait_time_in_nano_sec, &device_is_working);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_QDP_init_sequence(device_id, wait_time_in_nano_sec, &device_is_working);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_diagnostics suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_diagnostics)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_last_fabric_header_get)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_prbs_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_prbs_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_start_prbs_test)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_stop_prbs_test)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_ber_counters)
#ifndef ASIC_SIMULATION
/* the simulation does not supports external memories */
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_iddr_read)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_iddr_write)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_qdr_read)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_qdr_write)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_qdr_get_num_sft_err)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_QDP_init_sequence)
UTF_SUIT_END_TESTS_MAC(fap20m_api_diagnostics)

