/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_configurationUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_configuration, that provides
*       configuration API of FAP20M device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_configuration.h>
#include <FX950/include/fap20m_configuration.h>
#include <SAND/Utils/include/sand_os_interface.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_CONFIG_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_dram_parameters(
    SAND_IN unsigned int               device_id,
    SAND_IN FAP20M_DRAM_CONFIGURATION  dram_config,
    SAND_IN unsigned int               init_drc_conf,
    SAND_IN unsigned int               init_mmu_conf
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_dram_parameters)
{
/*
    ITERATE_DEVICES
    1.1. Call with dram_config {nof_columns [0 / 4095],
                                auto_precharge_pos [0 / 15],
                                enable_8bank_mode [0 / 1],
                                burst_size_mode [0 / 1]},
                   init_drc_conf [0 / 1],
                   init_mmu_conf [0 / 1]
    Expected: SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id     = 0;
    SAND_UINT init_drc_conf = 0;
    SAND_UINT init_mmu_conf = 0;
    FAP20M_DRAM_CONFIGURATION  dram_config;
    

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with dram_config {nof_columns [0 / 4095],
                                        auto_precharge_pos [0 / 15],
                                        enable_8bank_mode [0 / 1],
                                        burst_size_mode [0 / 1]},
                           init_drc_conf [0 / 1],
                           init_mmu_conf [0 / 1]
            Expected: SAND_OK.
        */
        /* iterate with init_drc_conf = 0 */
        dram_config.nof_columns = 0;
        dram_config.auto_precharge_pos = 0;
        dram_config.enable_8bank_mode = 0;
        dram_config.burst_size_mode = 0;

        init_drc_conf = 0;
        init_mmu_conf = 0;

        result = fap20m_set_dram_parameters(device_id, dram_config, init_drc_conf, init_mmu_conf);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, init_drc_conf, init_mmu_conf);

        /* iterate with init_drc_conf = 1 */
        dram_config.nof_columns = 4095;
        dram_config.auto_precharge_pos = 15;
        dram_config.enable_8bank_mode = 1;
        dram_config.burst_size_mode = 1;

        init_drc_conf = 1;
        init_mmu_conf = 1;

        result = fap20m_set_dram_parameters(device_id, dram_config, init_drc_conf, init_mmu_conf);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, init_drc_conf, init_mmu_conf);
    }

    dram_config.nof_columns = 0;
    dram_config.auto_precharge_pos = 0;
    dram_config.enable_8bank_mode = 0;
    dram_config.burst_size_mode = 0;

    init_drc_conf = 0;
    init_mmu_conf = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_dram_parameters(device_id, dram_config, init_drc_conf, init_mmu_conf);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_dram_parameters(device_id, dram_config, init_drc_conf, init_mmu_conf);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_fap_id(
    unsigned int  device_id,
    unsigned int* fap_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_fap_id)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL fap_id.
    Expected: SAND_OK.
    1.2. Call with fap_id [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_UINT fap_id    = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL fap_id.
            Expected: SAND_OK.
        */
        result = fap20m_get_fap_id(device_id, &fap_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with fap_id [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_fap_id(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, fap_id = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_fap_id(device_id, &fap_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_fap_id(device_id, &fap_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_fap_id(
    unsigned int  device_id,
    unsigned int fap_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_fap_id)
{
/*
    ITERATE_DEVICES
    1.1. Call with fap_id [0 / 2047].
    Expected: SAND_OK.
    1.2. Call fap20m_get_fap_id with not NULL pointers.
    expected: SAND_OK and the same params as was set.
    1.3. Call with fap_id [2048] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_UINT fap_id    = 0;
    SAND_UINT fap_idGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with fap_id [0 / 2047].
            Expected: SAND_OK.
        */
        /* iterate with fap_id = 0 */
        fap_id = 0;

        result = fap20m_set_fap_id(device_id, fap_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, fap_id);

        /*
            1.2. Call fap20m_get_fap_id with not NULL pointers.
            expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_fap_id(device_id, &fap_idGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_fap_id: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(fap_id, fap_idGet, "got another fap_id than was set: %d", device_id);
        
        /* iterate with fap_id = 2047 */
        fap_id = 2047;

        result = fap20m_set_fap_id(device_id, fap_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, fap_id);

        /*
            1.2. Call fap20m_get_fap_id with not NULL pointers.
            expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_fap_id(device_id, &fap_idGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_fap_id: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(fap_id, fap_idGet, "got another fap_id than was set: %d", device_id);

        /*
            1.3. Call with fap_id [2048] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        fap_id = 2048;

        result = fap20m_set_fap_id(device_id, fap_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, fap_id);
    }

    fap_id = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_fap_id(device_id, fap_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_fap_id(device_id, fap_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

#if 0
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_profile_id(
    unsigned int     device_id,
    unsigned long    *profile_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_profile_id)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL profile_id.
    Expected: SAND_OK.
    1.2. Call with profile_id [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id  = 0;
    SAND_U32  profile_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL profile_id.
            Expected: SAND_OK.
        */
        result = fap20m_get_profile_id(device_id, &profile_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with profile_id [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_profile_id(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, profile_id = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_profile_id(device_id, &profile_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_profile_id(device_id, &profile_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_profile_id(
    unsigned int  device_id,
    unsigned long profile_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_profile_id)
{
/*
    ITERATE_DEVICES
    1.1. Call with profile_id [0 / 2047].
    Expected: SAND_OK.
    1.2. Call fap20m_get_profile_id with not NULL pointers.
    expected: SAND_OK and the same params as was set.
    1.3. Call with profile_id [0xFFFFFFFF] (no any constraints) and other param from 1.1.
    Expected: SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id     = 0;
    SAND_U32  profile_id    = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with profile_id [0 / 2047].
            Expected: SAND_OK.
        */
        /* iterate with profile_id = 0 */
        profile_id = 0;

        result = fap20m_set_profile_id(device_id, profile_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, profile_id);

        /* iterate with profile_id = 2047 */
        profile_id = 2047;

        result = fap20m_set_profile_id(device_id, profile_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, profile_id);

        /*
            1.3. Call with profile_id [0xFFFFFFFF] (no any constraints) and other param from 1.1.
            Expected: SAND_OK.
        */
        profile_id = 0xFFFFFFFF;

        result = fap20m_set_profile_id(device_id, profile_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, profile_id);
    }

    profile_id = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_profile_id(device_id, profile_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_profile_id(device_id, profile_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

#if 0
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_profile_values(
    unsigned int     device_id,
    FAP20M_PROFILE*  profile 
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_profile_values)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL profile.
    Expected: SAND_OK.
    1.2. Call with profile [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_PROFILE profile;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL profile.
            Expected: SAND_OK.
        */
        result = fap20m_get_profile_values(device_id, &profile);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with profile [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_profile_values(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, profile = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_profile_values(device_id, &profile);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_profile_values(device_id, &profile);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_queue_type_params(
    SAND_IN  unsigned int                device_id,
    SAND_IN  FAP20M_QUEUE_TYPE           queue_type,
    SAND_OUT FAP20M_QUEUE_TYPE_PARAM*  queue_param
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_queue_type_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_type [FAP20M_QUEUE_0 /
                               FAP20M_QUEUE_5 /
                               FAP20M_QUEUE_15]
                   and not NULL queue_param.
    Expected: SAND_OK.
    1.2. Call with queue_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with queue_param [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_QUEUE_TYPE queue_type = FAP20M_QUEUE_0;
    FAP20M_QUEUE_TYPE_PARAM queue_param;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_type [FAP20M_QUEUE_0 /
                                       FAP20M_QUEUE_5 /
                                       FAP20M_QUEUE_15]
                           and not NULL queue_param.
            Expected: SAND_OK.
        */
        /* iterate with queue_type = FAP20M_QUEUE_0 */
        queue_type = FAP20M_QUEUE_0;

        result = fap20m_get_queue_type_params(device_id, queue_type, &queue_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        /* iterate with queue_type = FAP20M_QUEUE_5 */
        queue_type = FAP20M_QUEUE_5;

        result = fap20m_get_queue_type_params(device_id, queue_type, &queue_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        /* iterate with queue_type = FAP20M_QUEUE_15 */
        queue_type = FAP20M_QUEUE_15;

        result = fap20m_get_queue_type_params(device_id, queue_type, &queue_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        /*
            1.2. Call with queue_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_type = SAND_UTF_CONFIG_INVALID_ENUM;

        result = fap20m_get_queue_type_params(device_id, queue_type, &queue_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        queue_type = FAP20M_QUEUE_0;

        /*
            1.3. Call with queue_param [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_queue_type_params(device_id, queue_type, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, queue_param = NULL", device_id);
    }

    queue_type = FAP20M_QUEUE_0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_queue_type_params(device_id, queue_type, &queue_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_queue_type_params(device_id, queue_type, &queue_param);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_queue_type_params(
    SAND_IN  unsigned int                device_id,
    SAND_IN  FAP20M_QUEUE_TYPE           queue_type,
    SAND_IN  FAP20M_QUEUE_TYPE_PARAM*  queue_params,
    SAND_OUT FAP20M_QUEUE_TYPE_PARAM*  exact_queue_params_given
    )
*/
UTF_TEST_CASE_MAC(fap20m_set_queue_type_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_type [FAP20M_QUEUE_0 /
                               FAP20M_QUEUE_15]
                   queue_params {RED_enable [FALSE / TRUE],
                                 RED_exp_weight[0 / 31],
                                 flow_slow_th_up [0 / FAP20M_QDP_QTP_STS_SLW_UP_MAX],
                                 flow_stat_msg [0 / FAP20M_QDP_QTP_STS_MSG_MAX],
                                 max_q_size [0 / FAP20M_QDP_QTP_MAX_MNT_MAX],
                                 flow_slow_hys_en [FALSE / TRUE],
                                 hysteresis_size [0 / FAP20M_QDP_QTP_HDR_COMPENSATION_MAX],
                                 header_compensation [-1 / FAP20M_QDP_QTP_HDR_COMPENSATION_MAX]}
                   end non NULL exact_queue_params_given.
    Expected: SAND_OK.
    1.2. Call fap20m_get_queue_type_params with not NULL pointers and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with queue_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with queue_params->RED_exp_weight [32] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with queue_params->flow_slow_th_up [0xFFFFFFFF] (no any constraints) and other param from 1.1.
    Expected: SAND_OK.
    1.6. Call with queue_params->flow_stat_msg [0xFFFFFFFF] (no any constraints) and other param from 1.1.
    Expected: SAND_OK.
    1.7. Call with queue_params->max_q_size [0xFFFFFFFF] (no any constraints) and other param from 1.1.
    Expected: SAND_OK.
    1.8. Call with queue_params->hysteresis_size [0xFFFFFFFF] (no any constraints) and other param from 1.1.
    Expected: SAND_OK.
    1.9. Call with queue_params->header_compensation [0xFFFF] (no any constraints) and other param from 1.1.
    Expected: SAND_OK.
    1.10. Call with queue_params [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.11. Call with exact_queue_params_given [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT         device_id  = 0;
    FAP20M_QUEUE_TYPE queue_type = FAP20M_QUEUE_0;

    FAP20M_QUEUE_TYPE_PARAM queue_param;
    FAP20M_QUEUE_TYPE_PARAM queue_paramGet;
    FAP20M_QUEUE_TYPE_PARAM exact_queue_params_given;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_type [FAP20M_QUEUE_0 /
                                       FAP20M_QUEUE_15]
                           queue_params {RED_enable [FALSE / TRUE],
                                         RED_exp_weight[0 / 31],
                                         flow_slow_th_up [0 / FAP20M_QDP_QTP_STS_SLW_UP_MAX],
                                         flow_stat_msg [0 / FAP20M_QDP_QTP_STS_MSG_MAX],
                                         max_q_size [0 / FAP20M_QDP_QTP_MAX_MNT_MAX],
                                         flow_slow_hys_en [FALSE / TRUE],
                                         hysteresis_size [0 / FAP20M_QDP_QTP_HDR_COMPENSATION_MAX],
                                         header_compensation [-1 / FAP20M_QDP_QTP_HDR_COMPENSATION_MAX]}
                           end non NULL exact_queue_params_given.
            Expected: SAND_OK.
        */
        /* iterate with queue_type = FAP20M_QUEUE_0 */
        queue_type = FAP20M_QUEUE_0;

        queue_param.RED_enable = FALSE;
        queue_param.RED_exp_weight = 0;
        queue_param.flow_slow_th_up = 0;
        queue_param.flow_stat_msg = 0;
        queue_param.max_q_size = 0;
        queue_param.flow_slow_hys_en = FALSE;
        queue_param.hysteresis_size = 0;
        queue_param.header_compensation = -1;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        /*
            1.2. Call fap20m_get_queue_type_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_queue_type_params(device_id, queue_type, &queue_paramGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_queue_type_params: %d, %d", device_id, queue_type);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.RED_enable, queue_paramGet.RED_enable,
                   "got another queue_param->RED_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.RED_exp_weight, queue_paramGet.RED_exp_weight,
                   "got another queue_param->RED_exp_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.flow_slow_th_up, queue_paramGet.flow_slow_th_up,
                   "got another queue_param->flow_slow_th_up than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.flow_stat_msg, queue_paramGet.flow_stat_msg,
                   "got another queue_param->flow_stat_msg than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.max_q_size, queue_paramGet.max_q_size,
                   "got another queue_param->max_q_size than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.flow_slow_hys_en, queue_paramGet.flow_slow_hys_en,
                   "got another queue_param->flow_slow_hys_en than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.hysteresis_size, queue_paramGet.hysteresis_size,
                   "got another queue_param->hysteresis_size than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.header_compensation, queue_paramGet.header_compensation,
                   "got another queue_param->header_compensation than was set: %d", device_id);

        /* iterate with queue_type = FAP20M_QUEUE_15 */
        queue_type = FAP20M_QUEUE_15;

        queue_param.RED_enable = TRUE;
        queue_param.RED_exp_weight = 31;
        queue_param.flow_slow_th_up = FAP20M_QDP_QTP_STS_SLW_UP_MAX;
        queue_param.flow_stat_msg = FAP20M_QDP_QTP_STS_MSG_MAX;
        queue_param.max_q_size = FAP20M_QDP_QTP_MAX_MNT_MAX;
        queue_param.flow_slow_hys_en = TRUE;
        queue_param.hysteresis_size = FAP20M_QDP_QTP_HDR_COMPENSATION_MAX;
        queue_param.header_compensation = FAP20M_QDP_QTP_HDR_COMPENSATION_MAX;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        /*
            1.2. Call fap20m_get_queue_type_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_queue_type_params(device_id, queue_type, &queue_paramGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_queue_type_params: %d, %d", device_id, queue_type);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.RED_enable, queue_paramGet.RED_enable,
                   "got another queue_param->RED_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.RED_exp_weight, queue_paramGet.RED_exp_weight,
                   "got another queue_param->RED_exp_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.flow_slow_th_up, queue_paramGet.flow_slow_th_up,
                   "got another queue_param->flow_slow_th_up than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.flow_stat_msg, queue_paramGet.flow_stat_msg,
                   "got another queue_param->flow_stat_msg than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.max_q_size, queue_paramGet.max_q_size,
                   "got another queue_param->max_q_size than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.flow_slow_hys_en, queue_paramGet.flow_slow_hys_en,
                   "got another queue_param->flow_slow_hys_en than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.hysteresis_size, queue_paramGet.hysteresis_size,
                   "got another queue_param->hysteresis_size than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_params_given.header_compensation, queue_paramGet.header_compensation,
                   "got another queue_param->header_compensation than was set: %d", device_id);

        /*
            1.3. Call with queue_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_type = SAND_UTF_CONFIG_INVALID_ENUM;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        queue_type = FAP20M_QUEUE_0;

        /*
            1.4. Call with queue_params->RED_exp_weight [32] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_param.RED_exp_weight = 32;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, queue_param->RED_exp_weight = %d",
                                         device_id, queue_param.RED_exp_weight);

        queue_param.RED_exp_weight = 0;

        /*
            1.5. Call with queue_params->flow_slow_th_up [0xFFFFFFFF] (no any constraints) and other param from 1.1.
            Expected: SAND_OK.
        */
        queue_param.flow_slow_th_up = 0xFFFFFFFF;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, queue_param->flow_slow_th_up = %d",
                                     device_id, queue_param.flow_slow_th_up);

        queue_param.flow_slow_th_up = 0;

        /*
            1.6. Call with queue_params->flow_stat_msg [0xFFFFFFFF] (no any constraints) and other param from 1.1.
            Expected: SAND_OK.
        */
        queue_param.flow_stat_msg = 0xFFFFFFFF;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, queue_param->flow_stat_msg = %d",
                                     device_id, queue_param.flow_stat_msg);

        queue_param.flow_stat_msg = 0;

        /*
            1.7. Call with queue_params->max_q_size [0xFFFFFFFF] (no any constraints) and other param from 1.1.
            Expected: SAND_OK.
        */
        queue_param.max_q_size = 0xFFFFFFFF;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, queue_param->max_q_size = %d",
                                     device_id, queue_param.max_q_size);

        queue_param.max_q_size = 0;

        /*
            1.8. Call with queue_params->hysteresis_size [0xFFFFFFFF] (no any constraints) and other param from 1.1.
            Expected: SAND_OK.
        */
        queue_param.hysteresis_size = 0xFFFFFFFF;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, queue_param->hysteresis_size = %d",
                                     device_id, queue_param.hysteresis_size);

        queue_param.hysteresis_size = 0;

        /*
            1.9. Call with queue_params->header_compensation [0xFFFF] (no any constraints) and other param from 1.1.
            Expected: SAND_OK.
        */
        queue_param.header_compensation = 0xFFFF;

        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, queue_param->hysteresis_size = %d",
                                     device_id, queue_param.hysteresis_size);

        queue_param.hysteresis_size = 0;

        /*
            1.10. Call with queue_params [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_queue_type_params(device_id, queue_type, NULL, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, queue_params = NULL", device_id);

        /*
            1.11. Call with exact_queue_params_given [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_queue_params_given = NULL", device_id);
    }

    queue_type = FAP20M_QUEUE_0;

    queue_param.RED_enable = FALSE;
    queue_param.RED_exp_weight = 0;
    queue_param.flow_slow_th_up = 0;
    queue_param.flow_stat_msg = 0;
    queue_param.max_q_size = 0;
    queue_param.flow_slow_hys_en = FALSE;
    queue_param.hysteresis_size = 0;
    queue_param.header_compensation = -1;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_queue_type_params(device_id, queue_type, &queue_param, &exact_queue_params_given);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_queues_priority(
    SAND_IN  unsigned int     device_id,
    SAND_IN  unsigned long    queus_segments_bitstream[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE],
    SAND_IN  unsigned int     flip_score_board
    )
*/
UTF_TEST_CASE_MAC(fap20m_set_queues_priority)
{
/*
    ITERATE_DEVICES
    1.1. Call with queus_segments_bitstream[0 .. 0 / 0x7F .. 0x7F],
                   flip_score_board [FALSE / TRUE]
    Expected: SAND_OK.
    1.2. Call fap20m_get_queues_priority with not NULL pointers
    Expected: SAND_OK ad the same params as was set.
    1.3. Call with queus_segments_bitstream [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_UINT flip_score_board = FALSE;
    SAND_UINT flip_score_boardGet = FALSE;
    SAND_UINT iter = 0;
    SAND_BOOL isEqual = FALSE;

    SAND_U32 queus_segments_bitstream[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE];
    SAND_U32 queus_segments_bitstreamGet[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queus_segments_bitstream[0 .. 0 / 0x7F .. 0x7F],
                           flip_score_board [FALSE / TRUE]
            Expected: SAND_OK.
        */
        /* iterate with flip_score_board = FALSE */
        for (iter = 0; iter < FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE; iter++)
        {
            queus_segments_bitstream[iter] = 0;
        }

        flip_score_board = FALSE;

        result = fap20m_set_queues_priority(device_id, queus_segments_bitstream, flip_score_board);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, flip_score_board);

        /*
            1.2. Call fap20m_get_queues_priority with not NULL pointers
            Expected: SAND_OK ad the same params as was set.
        */
        result = fap20m_get_queues_priority(device_id, queus_segments_bitstreamGet, &flip_score_boardGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_queues_priority: %d", device_id);

        /* Verifying values */
        isEqual = (SAND_BOOL) ((0 == sand_os_memcmp((void*) queus_segments_bitstream,
                                                    (void*) queus_segments_bitstreamGet,
                                                    sizeof(queus_segments_bitstream[0]) * FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE)) ? TRUE : FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(TRUE, isEqual,
                   "get another queus_segments_bitstream than was set: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(flip_score_board, flip_score_boardGet,
                       "get another flip_score_board than was set: %d", device_id);

        /* iterate with flip_score_board = TRUE */
        for (iter = 0; iter < FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE; iter++)
        {
            queus_segments_bitstream[iter] = 0x7F;
        }

        flip_score_board = TRUE;

        result = fap20m_set_queues_priority(device_id, queus_segments_bitstream, flip_score_board);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, flip_score_board);

        /*
            1.2. Call fap20m_get_queues_priority with not NULL pointers
            Expected: SAND_OK ad the same params as was set.
        */
        result = fap20m_get_queues_priority(device_id, queus_segments_bitstreamGet, &flip_score_boardGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_queues_priority: %d", device_id);

        /* Verifying values */
        isEqual = (SAND_BOOL) ((0 == sand_os_memcmp((void*) queus_segments_bitstream,
                                                    (void*) queus_segments_bitstreamGet,
                                                    sizeof(queus_segments_bitstream[0]) * FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE)) ? TRUE : FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(TRUE, isEqual,
                   "get another queus_segments_bitstream than was set: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(flip_score_board, flip_score_boardGet,
                       "get another flip_score_board than was set: %d", device_id);

        /*
            1.3. Call with queus_segments_bitstream [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_queues_priority(device_id, NULL, flip_score_board);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, queus_segments_bitstream = NULL", device_id);
    }

    for (iter = 0; iter < FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE; iter++)
    {
        queus_segments_bitstream[iter] = 0;
    }

    flip_score_board = FALSE;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_queues_priority(device_id, queus_segments_bitstream, flip_score_board);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_queues_priority(device_id, queus_segments_bitstream, flip_score_board);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_queues_priority(
    SAND_IN  unsigned int     device_id,
    SAND_OUT unsigned long    queus_segments_bitstream[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE],
    SAND_OUT unsigned int     *flip_score_board
    )
*/
UTF_TEST_CASE_MAC(fap20m_get_queues_priority)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL queus_segments_bitstream
                   not NULL flip_score_board.
    Expected: SAND_OK.
    1.2. Call with queus_segments_bitstream [NULL]
                   and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with flip_score_board [NULL]
                   and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_UINT flip_score_board = FALSE;
    SAND_U32 queus_segments_bitstream[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE];
    

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL queus_segments_bitstream
                           not NULL flip_score_board.
            Expected: SAND_OK.
        */
        result = fap20m_get_queues_priority(device_id, queus_segments_bitstream, &flip_score_board);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with queus_segments_bitstream [NULL]
                           and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_queues_priority(device_id, NULL, &flip_score_board);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, queus_segments_bitstream = NULL", device_id);

        /*
            1.3. Call with flip_score_board [NULL]
                           and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_queues_priority(device_id, queus_segments_bitstream, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, flip_score_board = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_queues_priority(device_id, queus_segments_bitstream, &flip_score_board);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_queues_priority(device_id, queus_segments_bitstream, &flip_score_board);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_scrubber_params(
    SAND_IN  unsigned int                device_id,
    SAND_OUT FAP20M_SCRUBBER_PARAM*    scrubber_params
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_scrubber_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL scrubber_params.
    Expected: SAND_OK.
    1.2. Call with scrubber_params [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_SCRUBBER_PARAM scrubber_params;
    

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL scrubber_params.
            Expected: SAND_OK.
        */
        result = fap20m_get_scrubber_params(device_id, &scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with scrubber_params [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scrubber_params(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, scrubber_params = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_scrubber_params(device_id, &scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_scrubber_params(device_id, &scrubber_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_scrubber_params(
    SAND_IN  unsigned int                device_id,
    SAND_IN  FAP20M_SCRUBBER_PARAM*    scrubber_params,
    SAND_OUT FAP20M_SCRUBBER_PARAM*    exact_scrubber_params
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_scrubber_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with scrubber_params {enable [FALSE / TRUE],
                                    start_queue[0 / 12K],
                                    end_queue [0 / 12K],
                                    scan_rate [0 / FAP20M_QDP_QTP_STS_MSG_MAX]}
                   end non NULL exact_scrubber_params.
    Expected: SAND_OK.
    1.2. Call fap20m_get_scrubber_params with not NULL pointers and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with scrubber_params->start_queue [12K + 1] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with scrubber_params->end_queue [12K + 1] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with scrubber_params->scan_rate [FAP20M_SCRUBBER_QUE_SCAN_RATE_MAX] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with scrubber_params [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.7. Call with exact_scrubber_params [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id  = 0;
    FAP20M_SCRUBBER_PARAM scrubber_params;
    FAP20M_SCRUBBER_PARAM exact_scrubber_params;
    FAP20M_SCRUBBER_PARAM scrubber_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with scrubber_params {enable [FALSE / TRUE],
                                            start_queue[0 / 12K],
                                            end_queue [0 / 12K],
                                            scan_rate [0 / FAP20M_QDP_QTP_STS_MSG_MAX]}
                           end non NULL exact_scrubber_params.
            Expected: SAND_OK.
        */
        /* iterate with scrubber_params->enable = FALSE */
        scrubber_params.enable = FALSE;
        scrubber_params.start_queue = 0;
        scrubber_params.end_queue = 0;
        scrubber_params.scan_rate = 0;

        result = fap20m_set_scrubber_params(device_id, &scrubber_params, &exact_scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_scrubber_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_scrubber_params(device_id, &scrubber_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_scrubber_params: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_scrubber_params.enable, scrubber_paramsGet.enable,
                   "got another scrubber_params->enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_scrubber_params.start_queue, scrubber_paramsGet.start_queue,
                   "got another scrubber_params->start_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_scrubber_params.end_queue, scrubber_paramsGet.end_queue,
                   "got another scrubber_params->end_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_scrubber_params.scan_rate, scrubber_paramsGet.scan_rate,
                   "got another scrubber_params->scan_rate than was set: %d", device_id);

        /* iterate with scrubber_params->enable = TRUE */
        scrubber_params.enable = TRUE;
        scrubber_params.start_queue = 0x2FFF;
        scrubber_params.end_queue = 0x2FFF;
        scrubber_params.scan_rate = 0x2FFF;

        result = fap20m_set_scrubber_params(device_id, &scrubber_params, &exact_scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_scrubber_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_scrubber_params(device_id, &scrubber_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_scrubber_params: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_scrubber_params.enable, scrubber_paramsGet.enable,
                   "got another scrubber_params->enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_scrubber_params.start_queue, scrubber_paramsGet.start_queue,
                   "got another scrubber_params->start_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_scrubber_params.end_queue, scrubber_paramsGet.end_queue,
                   "got another scrubber_params->end_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_scrubber_params.scan_rate, scrubber_paramsGet.scan_rate,
                   "got another scrubber_params->scan_rate than was set: %d", device_id);        

        /*
            1.3. Call with scrubber_params->start_queue [12K + 1] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        scrubber_params.start_queue = FAP20M_NOF_QUEUES;

        result = fap20m_set_scrubber_params(device_id, &scrubber_params, &exact_scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scrubber_params->start_queue = %d",
                                         device_id, scrubber_params.start_queue);

        scrubber_params.start_queue = 0;

        /*
            1.4. Call with scrubber_params->end_queue [12K + 1] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        scrubber_params.end_queue = FAP20M_NOF_QUEUES;

        result = fap20m_set_scrubber_params(device_id, &scrubber_params, &exact_scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scrubber_params->end_queue = %d",
                                         device_id, scrubber_params.end_queue);

        scrubber_params.end_queue = 0;

        /*
            1.5. Call with scrubber_params->scan_rate [FAP20M_SCRUBBER_QUE_SCAN_RATE_MAX] (out of range) and other param from 1.1.
            Expected: SAND_OK. The API accepts all values of rate
        */
        scrubber_params.scan_rate = FAP20M_SCRUBBER_QUE_SCAN_RATE_MAX;

        result = fap20m_set_scrubber_params(device_id, &scrubber_params, &exact_scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scrubber_params->scan_rate = %d",
                                         device_id, scrubber_params.scan_rate);

        scrubber_params.scan_rate = 0;

        /*
            1.6. Call with scrubber_params [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scrubber_params(device_id, NULL, &exact_scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, scrubber_params = NULL", device_id);

        /*
            1.7. Call with exact_scrubber_params [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scrubber_params(device_id, &scrubber_params, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_scrubber_params = NULL", device_id);
    }

    scrubber_params.enable = FALSE;
    scrubber_params.start_queue = 0;
    scrubber_params.end_queue = 0;
    scrubber_params.scan_rate = 0;
    
    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_scrubber_params(device_id, &scrubber_params, &exact_scrubber_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_scrubber_params(device_id, &scrubber_params, &exact_scrubber_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_backoff_params(
    SAND_IN  unsigned int                device_id,
    SAND_OUT FAP20M_BACKOFF_PARAM*     backoff_params
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_backoff_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL backoff_params.
    Expected: SAND_OK.
    1.2. Call with backoff_params [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_BACKOFF_PARAM backoff_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL backoff_params.
            Expected: SAND_OK.
        */
        result = fap20m_get_backoff_params(device_id, &backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with backoff_params [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_backoff_params(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, backoff_params = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_backoff_params(device_id, &backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_backoff_params(device_id, &backoff_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_backoff_params(
    SAND_IN  unsigned int                device_id,
    SAND_IN  FAP20M_BACKOFF_PARAM*     backoff_params,
    SAND_OUT FAP20M_BACKOFF_PARAM*     exact_backoff_params
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_backoff_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with backoff_params {upper_threshold [0 / FAP20M_BACKOFF_SET_THRESHOLD_MAX - 1],
                                   lower_threshold[0 / FAP20M_BACKOFF_CLEAR_THRESHOLD_MAX - 1]}
                   end non NULL exact_scrubber_params.
    Expected: SAND_OK.
    1.2. Call fap20m_get_backoff_params with not NULL pointers and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with backoff_params->upper_threshold [0xF],
                   backoff_params->lower_threshold[0xF + 1].
    Expected: NOT SAND_OK.
    1.4. Call with backoff_params->upper_threshold [FAP20M_BACKOFF_SET_THRESHOLD_MAX] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with backoff_params->lower_threshold [FAP20M_BACKOFF_CLEAR_THRESHOLD_MAX] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with backoff_params [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.7. Call with exact_backoff_params [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id  = 0;
    FAP20M_BACKOFF_PARAM backoff_params;
    FAP20M_BACKOFF_PARAM exact_backoff_params;
    FAP20M_BACKOFF_PARAM backoff_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with backoff_params {upper_threshold [0 / FAP20M_BACKOFF_SET_THRESHOLD_MAX - 1],
                                           lower_threshold[0 / FAP20M_BACKOFF_CLEAR_THRESHOLD_MAX - 1]}
                           end non NULL exact_scrubber_params.
            Expected: SAND_OK.
        */
        /* iterate with backoff_params->upper_threshold = 0 */
        backoff_params.upper_threshold = 0;
        backoff_params.lower_threshold = 0;

        result = fap20m_set_backoff_params(device_id, &backoff_params, &exact_backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_backoff_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_backoff_params(device_id, &backoff_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_backoff_params: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(backoff_params.upper_threshold, backoff_paramsGet.upper_threshold,
                   "got another backoff_params->upper_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(backoff_params.lower_threshold, backoff_paramsGet.lower_threshold,
                   "got another backoff_params->lower_threshold than was set: %d", device_id);

        /* iterate with backoff_params->upper_threshold = FAP20M_BACKOFF_SET_THRESHOLD_MAX - 1 */
        backoff_params.upper_threshold = FAP20M_BACKOFF_SET_THRESHOLD_MAX;
        backoff_params.lower_threshold = FAP20M_BACKOFF_CLEAR_THRESHOLD_MAX;

        result = fap20m_set_backoff_params(device_id, &backoff_params, &exact_backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_backoff_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_backoff_params(device_id, &backoff_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_backoff_params: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_backoff_params.upper_threshold, backoff_paramsGet.upper_threshold,
                   "got another backoff_params->upper_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_backoff_params.lower_threshold, backoff_paramsGet.lower_threshold,
                   "got another backoff_params->lower_threshold than was set: %d", device_id);

        /*
            1.3. Call with backoff_params->upper_threshold [0xF],
                           backoff_params->lower_threshold[0xF + 1].
            Expected: NOT SAND_OK.
        */
        backoff_params.upper_threshold = 0x400;
        backoff_params.lower_threshold = 0x800;

        result = fap20m_set_backoff_params(device_id, &backoff_params, &exact_backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, ->upper_threshold = %d, ->lower_threshold = %d",
                                         device_id, backoff_params.upper_threshold, backoff_params.lower_threshold);

        /*
            1.4. Call with backoff_params->upper_threshold [FAP20M_BACKOFF_SET_THRESHOLD_MAX] (out of range)
                           and other param from 1.1.
            Expected: SAND_OK.
        */
        backoff_params.upper_threshold = FAP20M_BACKOFF_SET_THRESHOLD_MAX;

        result = fap20m_set_backoff_params(device_id, &backoff_params, &exact_backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, backoff_params->upper_threshold = %d",
                                         device_id, backoff_params.upper_threshold);

        backoff_params.upper_threshold = 0;

        /*
            1.5. Call with backoff_params->lower_threshold [FAP20M_BACKOFF_CLEAR_THRESHOLD_MAX] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK because of lower_threshold > upper_threshold.
        */
        backoff_params.lower_threshold = FAP20M_BACKOFF_CLEAR_THRESHOLD_MAX;

        result = fap20m_set_backoff_params(device_id, &backoff_params, &exact_backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, backoff_params->lower_threshold = %d",
                                         device_id, backoff_params.lower_threshold);

        backoff_params.lower_threshold = 0;

        /*
            1.6. Call with backoff_params [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_backoff_params(device_id, NULL, &exact_backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, backoff_params = NULL", device_id);

        /*
            1.7. Call with exact_backoff_params [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_backoff_params(device_id, &backoff_params, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, backoff_params = NULL", device_id);
    }

    backoff_params.upper_threshold = 0;
    backoff_params.lower_threshold = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_backoff_params(device_id, &backoff_params, &exact_backoff_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_backoff_params(device_id, &backoff_params, &exact_backoff_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_wred_params(
    unsigned int        device_id,
    FAP20M_QUEUE_TYPE   queue_type,
    unsigned int        drop_precedence,
    FAP20M_WRED_PARAM*  wred_param
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_wred_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_type [FAP20M_QUEUE_0 /
                               FAP20M_QUEUE_15],
                   drop_precedence [0 / 3],
                   and not NULL wred_param.
    Expected: SAND_OK.
    1.2. Call with queue_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with drop_precedence [4] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with wred_param [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_QUEUE_TYPE queue_type = FAP20M_QUEUE_0;
    SAND_UINT drop_precedence = 0;
    FAP20M_WRED_PARAM wred_param;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_type [FAP20M_QUEUE_0 /
                                       FAP20M_QUEUE_15],
                           drop_precedence [0 / 3],
                           and not NULL wred_param.
            Expected: SAND_OK.
        */
        /* iterate with queue_type = FAP20M_QUEUE_0 */
        queue_type = FAP20M_QUEUE_0;
        drop_precedence = 0;

        result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, queue_type, drop_precedence);

        /* iterate with queue_type = FAP20M_QUEUE_15 */
        queue_type = FAP20M_QUEUE_15;
        drop_precedence = 3;

        result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, queue_type, drop_precedence);

        /*
            1.2. Call with queue_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_type = SAND_UTF_CONFIG_INVALID_ENUM;

        result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        queue_type = FAP20M_QUEUE_0;

        /*
            1.3. Call with drop_precedence [4] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        drop_precedence = 4;

        result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, drop_precedence = %d",
                                         device_id, drop_precedence);

        drop_precedence = 0;

        /*
            1.4. Call with wred_param [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, wred_param = NULL", device_id);
    }

    queue_type = FAP20M_QUEUE_0;
    drop_precedence = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_param);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

#if 0
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_wred_params(
    unsigned int        device_id,
    FAP20M_QUEUE_TYPE   queue_type,
    unsigned int        drop_precedence,
    FAP20M_WRED_PARAM*  wred_param,
    FAP20M_WRED_PARAM*  exact_wred_param_given
    )
*/
UTF_TEST_CASE_MAC(fap20m_set_wred_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_type [FAP20M_QUEUE_0 /
                               FAP20M_QUEUE_15],
                   drop_precedence [0 / 3],
                   wred_param {min_avrg_th [0 / 0xFFFF],
                               max_avrg_th[1 / 0xFFFF],
                               max_packet_size [0 / FAP20M_WRED_MAX_PACKET_SIZE - 1],
                               max_probability [1 / 99],
                               valid [FALSE / TRUE]}
                   end non NULL exact_wred_param_given.
    Expected: SAND_OK.
    1.2. Call fap20m_get_wred_params with not NULL pointers and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with queue_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with drop_precedence [4] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with wred_param->min_avrg_th [0xFFFF + 1] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with wred_param->max_avrg_th [0xFFFF + 1] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with wred_param->max_packet_size [FAP20M_WRED_MAX_PACKET_SIZE] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with wred_param->max_probability [100] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.9. Call with wred_param [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.10. Call with exact_wred_param_given [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT         device_id       = 0;
    FAP20M_QUEUE_TYPE queue_type      = FAP20M_QUEUE_0;
    SAND_UINT         drop_precedence = 0;

    FAP20M_WRED_PARAM wred_param;
    FAP20M_WRED_PARAM wred_paramGet;
    FAP20M_WRED_PARAM exact_wred_param_given;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_type [FAP20M_QUEUE_0 /
                                       FAP20M_QUEUE_15],
                           drop_precedence [0 / 3],
                           wred_param {min_avrg_th [0 / 0xFFFF],
                                       max_avrg_th[1 / 0xFFFF],
                                       max_packet_size [0 / FAP20M_WRED_MAX_PACKET_SIZE - 1],
                                       max_probability [0 / 99],
                                       valid [FALSE / TRUE]}
                           end non NULL exact_wred_param_given.
            Expected: SAND_OK.
        */
        /* iterate with queue_type = FAP20M_QUEUE_0 */
        queue_type = FAP20M_QUEUE_0;
        drop_precedence = 0;

        wred_param.min_avrg_th = 0;
        wred_param.max_avrg_th = 1;
        wred_param.max_probability = 0;
        wred_param.valid = FALSE;
        wred_param.max_packet_size.arr[0] = 0;
        wred_param.max_packet_size.arr[1] = 0;

        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, queue_type, drop_precedence);

        /*
            1.2. Call fap20m_get_wred_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_paramGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_get_wred_params: %d, %d, %d",
                                     device_id, queue_type, drop_precedence);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.max_avrg_th, wred_paramGet.max_avrg_th,
                   "got another wred_paramGet->max_avrg_th than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.max_packet_size.arr[0], wred_paramGet.max_packet_size.arr[0],
                   "got another wred_paramGet->max_packet_size.arr[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.max_packet_size.arr[1], wred_paramGet.max_packet_size.arr[1],
                   "got another wred_paramGet->max_packet_size.arr[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.max_probability, wred_paramGet.max_probability,
                   "got another wred_paramGet->max_probability than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.min_avrg_th, wred_paramGet.min_avrg_th,
                   "got another wred_paramGet->min_avrg_th than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.valid, wred_paramGet.valid,
                   "got another wred_paramGet->valid than was set: %d", device_id);

        /* iterate with queue_type = FAP20M_QUEUE_15 */
        queue_type = FAP20M_QUEUE_15;
        drop_precedence = 3;

        wred_param.min_avrg_th = 0xFFFF;
        wred_param.max_avrg_th = 0xFFFF;
        wred_param.max_probability = 99;
        wred_param.valid = TRUE;
        sand_u64_multiply_longs(FAP20M_QDP_WORD_BYTE_SIZE, FAP20M_WRED_MAX_PACKET_SIZE, &(wred_param.max_packet_size));

        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, queue_type, drop_precedence);

        /*
            1.2. Call fap20m_get_wred_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_paramGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_get_wred_params: %d, %d, %d",
                                     device_id, queue_type, drop_precedence);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.max_avrg_th, wred_paramGet.max_avrg_th,
                   "got another wred_paramGet->max_avrg_th than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.max_packet_size.arr[0], wred_paramGet.max_packet_size.arr[0],
                   "got another wred_paramGet->max_packet_size.arr[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.max_packet_size.arr[1], wred_paramGet.max_packet_size.arr[1],
                   "got another wred_paramGet->max_packet_size.arr[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.max_probability, wred_paramGet.max_probability,
                   "got another wred_paramGet->max_probability than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.min_avrg_th, wred_paramGet.min_avrg_th,
                   "got another wred_paramGet->min_avrg_th than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_wred_param_given.valid, wred_paramGet.valid,
                   "got another wred_paramGet->valid than was set: %d", device_id);

        /*
            1.3. Call with queue_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_type = SAND_UTF_CONFIG_INVALID_ENUM;

        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_type);

        queue_type = FAP20M_QUEUE_0;

        /*
            1.4. Call with drop_precedence [4] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        drop_precedence = 4;

        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, drop_precedence = %d", device_id, drop_precedence);

        drop_precedence = 0;

        /*
            1.5. Call with wred_param->min_avrg_th [0xFFFF + 1] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        wred_param.min_avrg_th = 0xFFFF + 1;

        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, wred_param->min_avrg_th = %d",
                                         device_id, wred_param.min_avrg_th);

        wred_param.min_avrg_th = 0;

        /*
            1.6. Call with wred_param->max_avrg_th [0xFFFF + 1] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        wred_param.max_avrg_th = 0xFFFF + 1;

        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, wred_param->max_avrg_th = %d",
                                         device_id, wred_param.max_avrg_th);

        wred_param.max_avrg_th = 1;

        /*
            1.7. Call with wred_param->max_packet_size [FAP20M_WRED_MAX_PACKET_SIZE] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        wred_param.max_packet_size.arr[1]++;

        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, ->max_packet_size.arr[0] = %d, ->max_packet_size.arr[1] = %d",
                                         device_id, wred_param.max_packet_size.arr[0], wred_param.max_packet_size.arr[1]);

        wred_param.max_packet_size.arr[0] = 0;
        wred_param.max_packet_size.arr[1] = 0;

        /*
            1.8. Call with wred_param->max_probability [100] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        wred_param.max_probability = 100;

        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, wred_param->max_probability = %d",
                                         device_id, wred_param.max_probability);

        wred_param.max_probability = 0;

        /*
            1.9. Call with wred_param [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, NULL, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, wred_param = NULL", device_id);

        /*
            1.10. Call with exact_wred_param_given [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_wred_param_given = NULL", device_id);
    }

    queue_type = FAP20M_QUEUE_0;
    drop_precedence = 0;

    wred_param.min_avrg_th = 0;
    wred_param.max_avrg_th = 1;
    wred_param.max_probability = 0;
    wred_param.valid = FALSE;
    wred_param.max_packet_size.arr[0] = 0;
    wred_param.max_packet_size.arr[1] = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, &wred_param, &exact_wred_param_given);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_credit_size(
    SAND_IN  unsigned int      device_id,
    SAND_OUT SAND_CREDIT_SIZE  credit_byte_size
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_credit_size)
{
/*
    ITERATE_DEVICES
    1.1. Call with credit_byte_size [SAND_CR_256 / SAND_CR_512 /
                                     SAND_CR_1024 / SAND_CR_2048].
    Expected: SAND_OK.
    1.2. Call fap20m_get_credit_size with not NULL pointers.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with credit_byte_size [SAND_CR_4096 / SAND_CR_8192] (supported).
    Expected: NOT SAND_OK.
    1.3. Call with credit_byte_size [0x5AAAAAA5] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;

    SAND_CREDIT_SIZE credit_byte_size    = SAND_CR_256;
    SAND_CREDIT_SIZE credit_byte_sizeGet = SAND_CR_256;
        

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with credit_byte_size [SAND_CR_256 / SAND_CR_512 /
                                             SAND_CR_1024 / SAND_CR_2048].
            Expected: SAND_OK.
        */
        /* iterate with credit_byte_size = SAND_CR_256 */
        credit_byte_size = SAND_CR_256;

        result = fap20m_set_credit_size(device_id, credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, credit_byte_size);

        /*
            1.2. Call fap20m_get_credit_size with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_credit_size(device_id, &credit_byte_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_credit_size: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(credit_byte_size, credit_byte_sizeGet,
                   "got another credit_byte_size than was set: %d", device_id);

        /* iterate with credit_byte_size = SAND_CR_512 */
        credit_byte_size = SAND_CR_512;

        result = fap20m_set_credit_size(device_id, credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, credit_byte_size);

        /*
            1.2. Call fap20m_get_credit_size with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_credit_size(device_id, &credit_byte_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_credit_size: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(credit_byte_size, credit_byte_sizeGet,
                   "got another credit_byte_size than was set: %d", device_id);

        /* iterate with credit_byte_size = SAND_CR_1024 */
        credit_byte_size = SAND_CR_1024;

        result = fap20m_set_credit_size(device_id, credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, credit_byte_size);

        /*
            1.2. Call fap20m_get_credit_size with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_credit_size(device_id, &credit_byte_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_credit_size: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(credit_byte_size, credit_byte_sizeGet,
                   "got another credit_byte_size than was set: %d", device_id);

        /* iterate with credit_byte_size = SAND_CR_2048 */
        credit_byte_size = SAND_CR_2048;

        result = fap20m_set_credit_size(device_id, credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, credit_byte_size);

        /*
            1.2. Call fap20m_get_credit_size with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_credit_size(device_id, &credit_byte_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_credit_size: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(credit_byte_size, credit_byte_sizeGet,
                   "got another credit_byte_size than was set: %d", device_id);

        /*
            1.3. Call with credit_byte_size [SAND_CR_4096 / SAND_CR_8192] (supported).
            Expected: SAND_OK.
        */
        /* iterate with credit_byte_size = SAND_CR_4096 */
        credit_byte_size = SAND_CR_4096;

        result = fap20m_set_credit_size(device_id, credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, credit_byte_size);

        /* iterate with credit_byte_size = SAND_CR_8192 */
        credit_byte_size = SAND_CR_8192;

        result = fap20m_set_credit_size(device_id, credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, credit_byte_size);

        credit_byte_size = SAND_CR_256;

        /*
            1.3. Call with credit_byte_size [0x5AAAAAA5] (out of range).
            Expected: NOT SAND_OK.
        */
        credit_byte_size = (SAND_UTF_CONFIG_INVALID_ENUM + 1);

        result = fap20m_set_credit_size(device_id, credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, credit_byte_size);
    }

    credit_byte_size = SAND_CR_256;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_credit_size(device_id, credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_credit_size(device_id, credit_byte_size);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_credit_size(
    SAND_IN  unsigned int      device_id,
    SAND_OUT SAND_CREDIT_SIZE* credit_byte_size
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_credit_size)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL credit_byte_size.
    Expected: SAND_OK.
    1.2. Call with credit_byte_size [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_CREDIT_SIZE credit_byte_size;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL credit_byte_size.
            Expected: SAND_OK.
        */
        result = fap20m_get_credit_size(device_id, &credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with credit_byte_size [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_credit_size(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, credit_byte_size = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_credit_size(device_id, &credit_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_credit_size(device_id, &credit_byte_size);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_detect_if_single_fap_mode_required(
    SAND_IN   unsigned int  device_id,
    SAND_OUT  unsigned int* single_fap_mode_required
  )
*/
UTF_TEST_CASE_MAC(fap20m_detect_if_single_fap_mode_required)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL single_fap_mode_required.
    Expected: SAND_OK.
    1.2. Call with single_fap_mode_required [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_UINT single_fap_mode_required = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL single_fap_mode_required.
            Expected: SAND_OK.
        */
        result = fap20m_detect_if_single_fap_mode_required(device_id, &single_fap_mode_required);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with single_fap_mode_required [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_detect_if_single_fap_mode_required(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, single_fap_mode_required = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_detect_if_single_fap_mode_required(device_id, &single_fap_mode_required);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_detect_if_single_fap_mode_required(device_id, &single_fap_mode_required);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_single_fap_mode(
    unsigned int  device_id,
    unsigned int  single_fap_mode
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_single_fap_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with single_fap_mode [FALSE / TRUE].
    Expected: SAND_OK.
    1.2. Call fap20m_get_single_fap_mode with not NULL pointers.
    Expected: SAND_OK and the same params as was set.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;

    SAND_UINT single_fap_mode    = FALSE;
    SAND_UINT single_fap_modeGet = FALSE;
        

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with single_fap_mode [FALSE / TRUE].
            Expected: SAND_OK.
        */
        /* iterate with single_fap_mode = FALSE */
        single_fap_mode = FALSE;

        result = fap20m_set_single_fap_mode(device_id, single_fap_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, single_fap_mode);

        /*
            1.2. Call fap20m_get_single_fap_mode with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_single_fap_mode(device_id, &single_fap_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_single_fap_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(single_fap_mode, single_fap_modeGet,
                   "got another single_fap_mode than was set: %d", device_id);

        /* iterate with single_fap_mode = TRUE */
        single_fap_mode = TRUE;

        result = fap20m_set_single_fap_mode(device_id, single_fap_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, single_fap_mode);

        /*
            1.2. Call fap20m_get_single_fap_mode with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_single_fap_mode(device_id, &single_fap_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_single_fap_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(single_fap_mode, single_fap_modeGet,
                   "got another single_fap_mode than was set: %d", device_id);
    }

    single_fap_mode = FALSE;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_single_fap_mode(device_id, single_fap_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_single_fap_mode(device_id, single_fap_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_single_fap_mode(
    unsigned int  device_id,
    unsigned int* single_fap_mode
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_single_fap_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL single_fap_mode.
    Expected: SAND_OK.
    1.2. Call with single_fap_mode [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_UINT single_fap_mode = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL single_fap_mode.
            Expected: SAND_OK.
        */
        result = fap20m_get_single_fap_mode(device_id, &single_fap_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with single_fap_mode [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_single_fap_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, single_fap_mode = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_single_fap_mode(device_id, &single_fap_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_single_fap_mode(device_id, &single_fap_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_fabric_mode(
    unsigned int        device_id,
    FAP20M_FABRIC_MODE  fabric_mode
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_fabric_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with fabric_mode [FAP20M_FABRIC_MODE_FE /
                                FAP20M_FABRIC_MODE_BACK2BACK /
                                FAP20M_FABRIC_MODE_MESH /
                                FAP20M_FABRIC_MODE_MULT_STAGE_FE].
    Expected: SAND_OK.
    1.2. Call fap20m_get_fabric_mode with not NULL pointers.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with fabric_mode [0x5AAAAAA5] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;

    FAP20M_FABRIC_MODE fabric_mode    = FAP20M_FABRIC_MODE_FE;
    FAP20M_FABRIC_MODE fabric_modeGet = FAP20M_FABRIC_MODE_FE;
        

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with fabric_mode [FAP20M_FABRIC_MODE_FE /
                                        FAP20M_FABRIC_MODE_BACK2BACK /
                                        FAP20M_FABRIC_MODE_MESH /
                                        FAP20M_FABRIC_MODE_MULT_STAGE_FE].
            Expected: SAND_OK.
        */
        /* iterate with fabric_mode = FAP20M_FABRIC_MODE_FE */
        fabric_mode = FAP20M_FABRIC_MODE_FE;

        result = fap20m_set_fabric_mode(device_id, fabric_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, fabric_mode);

        /*
            1.2. Call fap20m_get_fabric_mode with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_fabric_mode(device_id, &fabric_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_fabric_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(fabric_mode, fabric_modeGet,
                   "got another fabric_mode than was set: %d", device_id);

        /* iterate with fabric_mode = FAP20M_FABRIC_MODE_BACK2BACK */
        fabric_mode = FAP20M_FABRIC_MODE_BACK2BACK;

        result = fap20m_set_fabric_mode(device_id, fabric_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, fabric_mode);

        /*
            1.2. Call fap20m_get_fabric_mode with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_fabric_mode(device_id, &fabric_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_fabric_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(fabric_mode, fabric_modeGet,
                   "got another fabric_mode than was set: %d", device_id);

        /* iterate with fabric_mode = FAP20M_FABRIC_MODE_MESH */
        fabric_mode = FAP20M_FABRIC_MODE_MESH;

        result = fap20m_set_fabric_mode(device_id, fabric_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, fabric_mode);

        /*
            1.2. Call fap20m_get_fabric_mode with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_fabric_mode(device_id, &fabric_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_fabric_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(fabric_mode, fabric_modeGet,
                   "got another fabric_mode than was set: %d", device_id);

        /* iterate with fabric_mode = FAP20M_FABRIC_MODE_MULT_STAGE_FE */
        fabric_mode = FAP20M_FABRIC_MODE_MULT_STAGE_FE;

        result = fap20m_set_fabric_mode(device_id, fabric_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, fabric_mode);

        /*
            1.2. Call fap20m_get_fabric_mode with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_fabric_mode(device_id, &fabric_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_fabric_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(fabric_mode, fabric_modeGet,
                   "got another fabric_mode than was set: %d", device_id);

        /*
            1.3. Call with fabric_mode [0x5AAAAAA5] (out of range).
            Expected: NOT SAND_OK.
        */
        fabric_mode = SAND_UTF_CONFIG_INVALID_ENUM;

        result = fap20m_set_fabric_mode(device_id, fabric_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, fabric_mode);
    }

    fabric_mode = FAP20M_FABRIC_MODE_FE;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_fabric_mode(device_id, fabric_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_fabric_mode(device_id, fabric_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_fabric_mode(
    unsigned int        device_id,
    FAP20M_FABRIC_MODE* fabric_mode
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_fabric_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL fabric_mode.
    Expected: SAND_OK.
    1.2. Call with fabric_mode [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_FABRIC_MODE fabric_mode = FAP20M_FABRIC_MODE_FE;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL fabric_mode.
            Expected: SAND_OK.
        */
        result = fap20m_get_fabric_mode(device_id, &fabric_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with fabric_mode [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_fabric_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, fabric_mode = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_fabric_mode(device_id, &fabric_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_fabric_mode(device_id, &fabric_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_mesh_links_topology(
    SAND_IN   unsigned int  device_id,
    SAND_IN   unsigned long mesh_links_topology[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH]
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_mesh_links_topology)
{
/*
    ITERATE_DEVICES
    1.1. Call with mesh_links_topology[0 .. 0 / 0xFFFFFFFF .. 0xFFFFFFFF].
    Expected: SAND_OK.
    1.2. Call fap20m_get_mesh_links_topology with not NULL pointers
    Expected: SAND_OK ad the same params as was set.
    1.3. Call with mesh_links_topology [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    
    SAND_UINT iter = 0;
    SAND_BOOL isEqual = FALSE;

    SAND_U32 mesh_links_topology[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH];
    SAND_U32 mesh_links_topologyGet[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with mesh_links_topology[0 .. 0 / 0x7F .. 0x7F].
            Expected: SAND_OK.
        */
        /* iterate with mesh_links_topology [0 .. 0] */
        for (iter = 0; iter < FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH; iter++)
        {
            mesh_links_topology[iter] = 0;
        }

        result = fap20m_set_mesh_links_topology(device_id, mesh_links_topology);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_mesh_links_topology with not NULL pointers
            Expected: SAND_OK ad the same params as was set.
        */
        result = fap20m_get_mesh_links_topology(device_id, mesh_links_topologyGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_mesh_links_topology: %d", device_id);

        /* Verifying values */
        isEqual = (SAND_BOOL) ((0 == sand_os_memcmp((void*) mesh_links_topology,
                                                    (void*) mesh_links_topologyGet,
                                                    sizeof(mesh_links_topology[0]) * FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH)) ? TRUE : FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(TRUE, isEqual,
                   "get another fap20m_get_mesh_links_topology than was set: %d", device_id);

        /* iterate with mesh_links_topology [0xFFFFFFFF .. 0xFFFFFFFF] */
        for (iter = 0; iter < FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH; iter++)
        {
            mesh_links_topology[iter] = 0xFFFFFFFF;
        }

        result = fap20m_set_mesh_links_topology(device_id, mesh_links_topology);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_mesh_links_topology with not NULL pointers
            Expected: SAND_OK ad the same params as was set.
        */
        result = fap20m_get_mesh_links_topology(device_id, mesh_links_topologyGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_mesh_links_topology: %d", device_id);

        /* Verifying values */
        isEqual = (SAND_BOOL) ((0 == sand_os_memcmp((void*) mesh_links_topology,
                                                    (void*) mesh_links_topologyGet,
                                                    sizeof(mesh_links_topology[0]) * FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH)) ? TRUE : FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(TRUE, isEqual,
                   "get another fap20m_get_mesh_links_topology than was set: %d", device_id);

        /*
            1.3. Call with mesh_links_topology [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_mesh_links_topology(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mesh_links_topology = NULL", device_id);
    }

    for (iter = 0; iter < FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH; iter++)
    {
        mesh_links_topology[iter] = 0;
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_mesh_links_topology(device_id, mesh_links_topology);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_mesh_links_topology(device_id, mesh_links_topology);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_mesh_links_topology(
    SAND_IN   unsigned int  device_id,
    SAND_OUT  unsigned long mesh_links_topology[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH]
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_mesh_links_topology)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL mesh_links_topology.
    Expected: SAND_OK.
    1.2. Call with mesh_links_topology [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  mesh_links_topology[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL mesh_links_topology.
            Expected: SAND_OK.
        */
        result = fap20m_get_mesh_links_topology(device_id, mesh_links_topology);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with mesh_links_topology [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_mesh_links_topology(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mesh_links_topology = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_mesh_links_topology(device_id, mesh_links_topology);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_mesh_links_topology(device_id, mesh_links_topology);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int        
  fap20m_set_dram_buff_size(
    unsigned int            device_id,
    FAP20M_DRAM_BUFF_SIZE   buff_size,
    unsigned long           scrm_bit
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_dram_buff_size)
{
/*
    ITERATE_DEVICES
    1.1. Call with buff_size [FAP20M_DRAM_BUFF_SIZE_64 /
                              FAP20M_DRAM_BUFF_SIZE_256 /
                              FAP20M_DRAM_BUFF_SIZE_2048]
                   scrm_bit [0 / 100 / 0xFF].
    Expected: SAND_OK.
    1.2. Call fap20m_get_dram_buff_size with not NULL pointers.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with buff_size [0x5AAAAAA5] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with scrm_bit [0xFFFFFFFF] (no any constraints)
                   and other param from 1.1.
    Expected: SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  scrm_bit  = 0;

    FAP20M_DRAM_BUFF_SIZE buff_size = FAP20M_DRAM_BUFF_SIZE_64;
    FAP20M_DRAM_BUFF_SIZE buff_sizeGet = FAP20M_DRAM_BUFF_SIZE_64;
        

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with buff_size [FAP20M_DRAM_BUFF_SIZE_64 /
                                      FAP20M_DRAM_BUFF_SIZE_256 /
                                      FAP20M_DRAM_BUFF_SIZE_2048]
                           scrm_bit [0 / 100 / 0xFF].
            Expected: SAND_OK.
        */
        /* iterate with buff_size = FAP20M_DRAM_BUFF_SIZE_64 */
        buff_size = FAP20M_DRAM_BUFF_SIZE_64;
        scrm_bit  = 0;

        result = fap20m_set_dram_buff_size(device_id, buff_size, scrm_bit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, buff_size, scrm_bit);

        /*
            1.2. Call fap20m_get_dram_buff_size with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_dram_buff_size(device_id, &buff_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_dram_buff_size: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(buff_size, buff_sizeGet,
                   "got another buff_size than was set: %d", device_id);

        /* iterate with buff_size = FAP20M_DRAM_BUFF_SIZE_128 */
        buff_size = FAP20M_DRAM_BUFF_SIZE_128;
        scrm_bit  = 100;

        result = fap20m_set_dram_buff_size(device_id, buff_size, scrm_bit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, buff_size, scrm_bit);

        /*
            1.2. Call fap20m_get_dram_buff_size with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_dram_buff_size(device_id, &buff_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_dram_buff_size: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(buff_size, buff_sizeGet,
                   "got another buff_size than was set: %d", device_id);

        /* iterate with buff_size = FAP20M_DRAM_BUFF_SIZE_2048 */
        buff_size = FAP20M_DRAM_BUFF_SIZE_2048;
        scrm_bit  = 0xFF;

        result = fap20m_set_dram_buff_size(device_id, buff_size, scrm_bit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, buff_size, scrm_bit);

        /*
            1.2. Call fap20m_get_dram_buff_size with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_dram_buff_size(device_id, &buff_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_dram_buff_size: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(buff_size, buff_sizeGet,
                   "got another buff_size than was set: %d", device_id);

        /*
            1.3. Call with buff_size [0x5AAAAAA5] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buff_size = SAND_UTF_CONFIG_INVALID_ENUM;

        result = fap20m_set_dram_buff_size(device_id, buff_size, scrm_bit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, buff_size);

        buff_size = FAP20M_DRAM_BUFF_SIZE_256;
        /*
            1.4. Call with scrm_bit [0xFFFFFFFF] (no any constraints)
                           and other param from 1.1.
            Expected: SAND_OK.
        */
        scrm_bit = 0xFFFFFFFF;

        result = fap20m_set_dram_buff_size(device_id, buff_size, scrm_bit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scrm_bit = %d", device_id, scrm_bit);
    }

    buff_size = FAP20M_DRAM_BUFF_SIZE_64;
    scrm_bit  = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_dram_buff_size(device_id, buff_size, scrm_bit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_dram_buff_size(device_id, buff_size, scrm_bit);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int        
  fap20m_get_dram_buff_size(
    unsigned int            device_id,
    FAP20M_DRAM_BUFF_SIZE   *buff_size  
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_dram_buff_size)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL buff_size.
    Expected: SAND_OK.
    1.2. Call with buff_size [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_DRAM_BUFF_SIZE buff_size = FAP20M_DRAM_BUFF_SIZE_64;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL buff_size.
            Expected: SAND_OK.
        */
        result = fap20m_get_dram_buff_size(device_id, &buff_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with buff_size [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_dram_buff_size(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, buff_size = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_dram_buff_size(device_id, &buff_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_dram_buff_size(device_id, &buff_size);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_configuration suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_configuration)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_dram_parameters)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_fap_id)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_fap_id)
#if 0
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_profile_id)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_profile_id)
#if 0
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_profile_values)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_queue_type_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_queue_type_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_queues_priority)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_queues_priority)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_scrubber_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_scrubber_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_backoff_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_backoff_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_wred_params)
#if 0
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_wred_params)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_credit_size)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_credit_size)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_detect_if_single_fap_mode_required)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_single_fap_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_single_fap_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_fabric_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_fabric_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_mesh_links_topology)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_mesh_links_topology)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_dram_buff_size)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_dram_buff_size)
UTF_SUIT_END_TESTS_MAC(fap20m_api_configuration)
