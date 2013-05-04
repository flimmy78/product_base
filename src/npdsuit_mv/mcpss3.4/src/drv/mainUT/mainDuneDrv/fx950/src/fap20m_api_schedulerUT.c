/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_schedulerUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_scheduler, that provides
*       Definitions and functions related to the Scheduler 
*       module of the device.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_scheduler.h>
#include <FX950/include/fap20m_api_configuration.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_API_SCHEDULER_INVALID_ENUM   0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_primary_scheduler_role(
    SAND_IN   unsigned int      device_id,
    SAND_IN   FAP20M_SCH_ROLE   scheduler_role
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_primary_scheduler_role)
{
/*
    ITERATE_DEVICES
    1.1. Call with scheduler_role [FAP20M_SCH_ROLE_FABRIC / FAP20M_SCH_ROLE_EG_TM]. 
    Expected: SAND_OK.
    1.2. Call fap20m_get_primary_scheduler_role with not NULL scheduler_role.
    ExpecteD: SAND_OK and the same scheduler_role as was set.
    1.3. Call with scheduler_role [0x5AAAAAA5] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_ROLE scheduler_role = FAP20M_SCH_ROLE_FABRIC;
    FAP20M_SCH_ROLE scheduler_roleGet = FAP20M_SCH_ROLE_FABRIC;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with scheduler_role [FAP20M_SCH_ROLE_FABRIC / FAP20M_SCH_ROLE_EG_TM]. 
            Expected: SAND_OK.
        */
        /* iterate with scheduler_role = FAP20M_SCH_ROLE_FABRIC */
        scheduler_role = FAP20M_SCH_ROLE_FABRIC;

        result = fap20m_set_primary_scheduler_role(device_id, scheduler_role);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, scheduler_role);

        /*
            1.2. Call fap20m_get_primary_scheduler_role with not NULL scheduler_role.
            ExpecteD: SAND_OK and the same scheduler_role as was set.
        */
        result = fap20m_get_primary_scheduler_role(device_id, &scheduler_roleGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_primary_scheduler_role: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler_role, scheduler_roleGet, 
                                     "got another scheduler_role than was set: %d", device_id);

        /* iterate with scheduler_role = FAP20M_SCH_ROLE_EG_TM */
        scheduler_role = FAP20M_SCH_ROLE_EG_TM;

        result = fap20m_set_primary_scheduler_role(device_id, scheduler_role);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, scheduler_role);

        /*
            1.2. Call fap20m_get_primary_scheduler_role with not NULL scheduler_role.
            ExpecteD: SAND_OK and the same scheduler_role as was set.
        */
        result = fap20m_get_primary_scheduler_role(device_id, &scheduler_roleGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_primary_scheduler_role: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler_role, scheduler_roleGet, 
                                     "got another scheduler_role than was set: %d", device_id);

        /*
            1.3. Call with scheduler_role [0x5AAAAAA5] (out of range).
            Expected: NOT SAND_OK.
        */
        scheduler_role = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_primary_scheduler_role(device_id, scheduler_role);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, scheduler_role);
    }

    scheduler_role = FAP20M_SCH_ROLE_EG_TM;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_primary_scheduler_role(device_id, scheduler_role);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_primary_scheduler_role(device_id, scheduler_role);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_primary_scheduler_role(
    SAND_IN   unsigned int      device_id,
    SAND_OUT  FAP20M_SCH_ROLE   *scheduler_role
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_primary_scheduler_role)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null scheduler_role. 
    Expected: SAND_OK.
    1.2. Call with scheduler_role [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_ROLE scheduler_role = FAP20M_SCH_ROLE_FABRIC;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with non-null scheduler_role.
            Expected: SAND_OK.
        */
        result = fap20m_get_primary_scheduler_role(device_id, &scheduler_role);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with scheduler_role [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_primary_scheduler_role(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, scheduler_role = NULL", device_id);
    }

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_primary_scheduler_role(device_id, &scheduler_role);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_primary_scheduler_role(device_id, &scheduler_role);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_device_rate_entry(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_IN     FAP20M_DEVICE_RATE_ENTRY     *dev_rate,
    SAND_OUT    FAP20M_DEVICE_RATE_ENTRY     *exact_dev_rate
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_device_rate_entry)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                   dev_rate { rci_level [0 / 7], 
                              num_active_links [0 / 24],
                              rate [0 / 100] }, 
                   non-null exact_dev_rate. 
    Expected: SAND_OK.
    1.2. Call fap20m_get_device_rate_entry with not NULL dev_rate and other params from 1.1.
    Expected: SAND_OK and the same dev_rate as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1..
    Expected: NOT SAND_OK.
    1.4. Call with dev_rate->rci_level [8] (out of range) and other params from 1.1..
    Expected: NOT SAND_OK.
    1.5. Call with dev_rate->num_active_links [25] (out of range) and other params from 1.1..
    Expected: NOT SAND_OK.
    1.6. Call with dev_rate [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.7. Call with exact_dev_rate [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;

    FAP20M_DEVICE_RATE_ENTRY dev_rate;
    FAP20M_DEVICE_RATE_ENTRY dev_rateGet;
    FAP20M_DEVICE_RATE_ENTRY exact_dev_rate;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* Set default credit size */
        result = fap20m_set_credit_size(device_id, SAND_CR_512);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, SAND_CR_512);

        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                           dev_rate { rci_level [0 / 7], 
                                      num_active_links [0 / 24],
                                      rate [0 / 100] }, 
                           non-null exact_dev_rate. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        dev_rate.rci_level = 0;
        dev_rate.num_active_links = 0;
        dev_rate.rate = 0;

        /* initialize indexes of dev_rateGet */
        dev_rateGet.num_active_links = dev_rate.num_active_links;
        dev_rateGet.rci_level = dev_rate.rci_level;

        result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, &exact_dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_device_rate_entry with not NULL dev_rate and other params from 1.1.
            Expected: SAND_OK and the same dev_rate as was set.
        */
        result = fap20m_get_device_rate_entry(device_id, sch_rank, &dev_rateGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_device_rate_entry: %d, %d", device_id, sch_rank);

        /* verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(exact_dev_rate.rate, dev_rateGet.rate, 
                                     "got another dev_rate.rate than was set: %d, %d", device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        dev_rate.rci_level = 7;
        dev_rate.num_active_links = 24;
        dev_rate.rate = 100;

        /* initialize indexes of dev_rateGet */
        dev_rateGet.num_active_links = dev_rate.num_active_links;
        dev_rateGet.rci_level = dev_rate.rci_level;

        result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, &exact_dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_device_rate_entry with not NULL dev_rate and other params from 1.1.
            Expected: SAND_OK and the same dev_rate as was set.
        */
        result = fap20m_get_device_rate_entry(device_id, sch_rank, &dev_rateGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_device_rate_entry: %d, %d", device_id, sch_rank);

        /* verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(exact_dev_rate.rate, dev_rateGet.rate, 
                                     "got another dev_rate.rate than was set: %d, %d", device_id, sch_rank);

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1..
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, &exact_dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with dev_rate->rci_level [8] (out of range) and other params from 1.1..
            Expected: NOT SAND_OK.
        */
        dev_rate.rci_level = 8;

        result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, &exact_dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, dev_rate.rci_level = %d", 
                                         device_id, dev_rate.rci_level);

        dev_rate.rci_level = 0;

        /*
            1.5. Call with dev_rate->num_active_links [25] (out of range) and other params from 1.1..
            Expected: NOT SAND_OK.
        */
        dev_rate.num_active_links = 25;

        result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, &exact_dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, dev_rate.num_active_links = %d", 
                                         device_id, dev_rate.num_active_links);

        dev_rate.num_active_links = 0;

        /*
            1.6. Call with dev_rate [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_device_rate_entry(device_id, sch_rank, NULL, &exact_dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, dev_rate = NULL", device_id);

        /*
            1.7. Call with exact_dev_rate [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_dev_rate = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    dev_rate.rci_level = 0;
    dev_rate.num_active_links = 0;
    dev_rate.rate = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, &exact_dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, &exact_dev_rate);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_device_rate_entry(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_INOUT  FAP20M_DEVICE_RATE_ENTRY     *dev_rate
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_device_rate_entry)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], non-null dev_rate. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with dev_rate [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_DEVICE_RATE_ENTRY dev_rate;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], non-null dev_rate. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        dev_rate.num_active_links = 0;
        dev_rate.rci_level = 0;

        result = fap20m_get_device_rate_entry(device_id, sch_rank, &dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_device_rate_entry(device_id, sch_rank, &dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1..
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_device_rate_entry(device_id, sch_rank, &dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        /*
            1.3. Call with dev_rate [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_device_rate_entry(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, dev_rate = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_device_rate_entry(device_id, sch_rank, &dev_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_device_rate_entry(device_id, sch_rank, &dev_rate);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_slow_max_rate(
    SAND_IN  unsigned int     device_id,
    SAND_IN  FAP20M_SCH_RANK  sch_rank,
    SAND_IN  unsigned int     slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES]
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_slow_max_rate)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                   slow_max_rates [0,0 / 100,100]. 
    Expected: SAND_OK.
    1.2. Call fap20m_get_slow_max_rate with not NULL slow_max_rates and sch_rank from 1.1.
    Expected: SAND_OK and slow_max_rates as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1..
    Expected: NOT SAND_OK.
    1.4. Call with slow_max_rates [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK  sch_rank = FAP20M_SCH_RANK_PRIM;

    SAND_UINT  slow_max_rates [FAP20M_SCH_NOF_SLOW_RATES];
    SAND_UINT  slow_max_ratesGet [FAP20M_SCH_NOF_SLOW_RATES];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND] 
                           slow_max_rates [32,32 / 129,129]. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        slow_max_rates[0] = 32;
        slow_max_rates[1] = 32;

        result = fap20m_set_slow_max_rate(device_id, sch_rank, slow_max_rates);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_slow_max_rate with not NULL slow_max_rates and sch_rank from 1.1.
            Expected: SAND_OK and slow_max_rates as was set.
        */
        result = fap20m_get_slow_max_rate(device_id, sch_rank, slow_max_ratesGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_slow_max_rate: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(slow_max_rates[0], slow_max_ratesGet[0], 
                                     "got another slow_max_rates[0] than was set: %d, %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(slow_max_rates[1], slow_max_ratesGet[1], 
                                     "got another slow_max_rates[1] than was set: %d, %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        slow_max_rates[0] = 129;
        slow_max_rates[1] = 129;

        result = fap20m_set_slow_max_rate(device_id, sch_rank, slow_max_rates);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_slow_max_rate with not NULL slow_max_rates and sch_rank from 1.1.
            Expected: SAND_OK and slow_max_rates as was set.
        */
        result = fap20m_get_slow_max_rate(device_id, sch_rank, slow_max_ratesGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_slow_max_rate: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(slow_max_rates[0], slow_max_ratesGet[0], 
                                     "got another slow_max_rates[0] than was set: %d, %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(slow_max_rates[1], slow_max_ratesGet[1], 
                                     "got another slow_max_rates[1] than was set: %d, %d", device_id);
        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1..
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_slow_max_rate(device_id, sch_rank, slow_max_rates);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with slow_max_rates [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_slow_max_rate(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, slow_max_rates = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    slow_max_rates[0] = 32;
    slow_max_rates[1] = 32;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_slow_max_rate(device_id, sch_rank, slow_max_rates);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_slow_max_rate(device_id, sch_rank, slow_max_rates);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_slow_max_rate(
    SAND_IN  unsigned int     device_id,
    SAND_IN  FAP20M_SCH_RANK  sch_rank,
    SAND_OUT unsigned int     slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES]
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_slow_max_rate)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null slow_max_rates. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1..
    Expected: NOT SAND_OK.
    1.3. Call with slow_max_rates [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK  sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT  slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null slow_max_rates. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        result = fap20m_get_slow_max_rate(device_id, sch_rank, slow_max_rates);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_slow_max_rate(device_id, sch_rank, slow_max_rates);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1..
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_slow_max_rate(device_id, sch_rank, slow_max_rates);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call with slow_max_rates [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_slow_max_rate(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, slow_max_rates = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_slow_max_rate(device_id, sch_rank, slow_max_rates);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_slow_max_rate(device_id, sch_rank, slow_max_rates);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

#if 0
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_device_interfaces_scheduler(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_IN     FAP20M_DEVICE_IF_SCHEDULER   *dev_if_sch,
    SAND_OUT    FAP20M_DEVICE_IF_SCHEDULER   *exact_dev_if_sch
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_device_interfaces_scheduler)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                   dev_if_sch { valid_mask [0 / 0xFFFFFF],
                                link_a_max_rate [0 / 100],
                                link_a_weight [0 / 100],
                                link_b_max_rate [0 / 100],
                                link_b_weight [0 / 100],
                   non-null exact_dev_if_sch.
    Expected: SAND_OK.
    1.2. Call fap20m_get_device_interfaces_scheduler with not NULL dev_if_sch and sch_rank from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with dev_if_sch [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with exact_dev_if_sch [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;

    FAP20M_DEVICE_IF_SCHEDULER dev_if_sch;
    FAP20M_DEVICE_IF_SCHEDULER dev_if_schGet;
    FAP20M_DEVICE_IF_SCHEDULER exact_dev_if_sch;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                           dev_if_sch { valid_mask [0 / 0xFFFFFF],
                                        link_a_max_rate [0 / 100],
                                        link_a_weight [0 / 100],
                                        link_b_max_rate [0 / 100],
                                        link_b_weight [0 / 100],
                           non-null exact_dev_if_sch.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        dev_if_sch.valid_mask = 0;
        dev_if_sch.link_a_max_rate = 0;
        dev_if_sch.link_a_weight = 0;
        dev_if_sch.link_b_max_rate = 0;
        dev_if_sch.link_b_weight = 0;

        result = fap20m_set_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch, &exact_dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_device_interfaces_scheduler with not NULL dev_if_sch and sch_rank from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, &dev_if_schGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_device_interfaces_scheduler: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.valid_mask, dev_if_schGet.valid_mask, 
                                     "got another dev_if_sch.valid_mask than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.link_a_max_rate, dev_if_schGet.link_a_max_rate, 
                                     "got another dev_if_sch.link_a_max_rate than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.link_a_weight, dev_if_schGet.link_a_weight, 
                                     "got another dev_if_sch.link_a_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.link_b_max_rate, dev_if_schGet.link_b_max_rate, 
                                     "got another dev_if_sch.link_b_max_rate than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.link_b_weight, dev_if_schGet.link_b_weight, 
                                     "got another dev_if_sch.link_b_weight than was set: %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        dev_if_sch.valid_mask = 0xFFFFFF;
        dev_if_sch.link_a_max_rate = 0xFFFFFF;
        dev_if_sch.link_a_weight = 100;
        dev_if_sch.link_b_max_rate = 100;
        dev_if_sch.link_b_weight = 100;

        result = fap20m_set_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch, &exact_dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_device_interfaces_scheduler with not NULL dev_if_sch and sch_rank from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, &dev_if_schGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_device_interfaces_scheduler: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.valid_mask, dev_if_schGet.valid_mask, 
                                     "got another dev_if_sch.valid_mask than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.link_a_max_rate, dev_if_schGet.link_a_max_rate, 
                                     "got another dev_if_sch.link_a_max_rate than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.link_a_weight, dev_if_schGet.link_a_weight, 
                                     "got another dev_if_sch.link_a_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.link_b_max_rate, dev_if_schGet.link_b_max_rate, 
                                     "got another dev_if_sch.link_b_max_rate than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_if_sch.link_b_weight, dev_if_schGet.link_b_weight, 
                                     "got another dev_if_sch.link_b_weight than was set: %d", device_id);
        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch, &exact_dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with dev_if_sch [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_device_interfaces_scheduler(device_id, sch_rank, NULL, &exact_dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, dev_if_sch = NULL", device_id);

        /*
            1.5. Call with exact_dev_if_sch [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_dev_if_sch = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    dev_if_sch.valid_mask = TRUE;
    dev_if_sch.link_a_max_rate = 0;
    dev_if_sch.link_a_weight = 0;
    dev_if_sch.link_b_max_rate = 0;
    dev_if_sch.link_b_weight = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch, &exact_dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch, &exact_dev_if_sch);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_device_interfaces_scheduler(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_INOUT  FAP20M_DEVICE_IF_SCHEDULER   *dev_if_sch
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_device_interfaces_scheduler)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null dev_if_sch. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with dev_if_sch [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_DEVICE_IF_SCHEDULER dev_if_sch;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null dev_if_sch. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_PRIM;

        /*
            1.3. Call with dev_if_sch [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, dev_if_sch = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

#if 0
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_device_scheduler(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_IN     FAP20M_DEVICE_SCHEDULER      *dev_scheduler,
    SAND_OUT    FAP20M_DEVICE_SCHEDULER      *exact_dev_scheduler
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_device_scheduler)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   dev_scheduler { valid_mask [0 / 0xFFFFFF]
                                   fcg { drt { rates[FAP20M_DRT_SIZE] { rci_level [0 / 7],
                                                                        num_active_links [0 / 24],
                                                                        rate [0 / 100] } } }
                                   dev_if_scheduler{ valid_mask [0 / 0xFFFFFF],
                                                     link_a_max_rate [0 / 100],
                                                     link_a_weight [0 / 100],
                                                     link_b_max_rate [0 / 100],
                                                     link_b_weight [0 / 100],
                   not NULL exact_dev_scheduler.
    Expected: SAND_OK.
    1.2. Call fap20m_get_device_scheduler with not NULL dev_scheduler and sch_rank from 1.1.
    Expected: SAND_OK and tha same dev_scheduler as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with dev_scheduler->fcg.drt.rates[0].rci_level [8], (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with dev_scheduler->fcg.drt.rates[0].num_active_links [24], (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with dev_scheduler [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.7. Call with exact_dev_scheduler [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT  index = 0;

    FAP20M_DEVICE_SCHEDULER dev_scheduler;
    FAP20M_DEVICE_SCHEDULER dev_schedulerGet;
    FAP20M_DEVICE_SCHEDULER exact_dev_scheduler;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           dev_scheduler { valid_mask [0 / 0xFFFFFF]
                                           fcg { drt { rates[FAP20M_DRT_SIZE] { rci_level [0 / 7],
                                                                                num_active_links [0 / 24],
                                                                                rate [0 / 100] } } }
                                           dev_if_scheduler{ valid_mask [0 / 0xFFFFFF],
                                                             link_a_max_rate [0 / 100],
                                                             link_a_weight [0 / 100],
                                                             link_b_max_rate [0 / 100],
                                                             link_b_weight [0 / 100],
                           not NULL exact_dev_scheduler.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        dev_scheduler.valid_mask = 0;

        for(index = 0; index < FAP20M_DRT_SIZE; index++)
        {
            dev_scheduler.fcg.drt.rates[index].rci_level = 0;
            dev_scheduler.fcg.drt.rates[index].num_active_links = 0;
            dev_scheduler.fcg.drt.rates[index].rate = 0;
        }

        dev_scheduler.dev_if_scheduler.valid_mask = 0;
        dev_scheduler.dev_if_scheduler.link_a_max_rate = 0;
        dev_scheduler.dev_if_scheduler.link_a_weight = 0;
        dev_scheduler.dev_if_scheduler.link_b_max_rate = 0;
        dev_scheduler.dev_if_scheduler.link_b_weight = 0;

        result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler, &exact_dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_device_scheduler with not NULL dev_scheduler and sch_rank from 1.1.
            Expected: SAND_OK and tha same dev_scheduler as was set.
        */
        result = fap20m_get_device_scheduler(device_id, sch_rank, &dev_schedulerGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_device_scheduler: %d, %d", device_id, sch_rank);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.valid_mask, dev_schedulerGet.valid_mask,
                                     "got another dev_scheduler.valid_mask than was set: %d", device_id);

        for(index = 0; index < FAP20M_DRT_SIZE; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_dev_scheduler.fcg.drt.rates[index].rci_level, 
                                     dev_schedulerGet.fcg.drt.rates[index].rci_level, 
                                     "got another dev_scheduler.fcg.drt.rates[%d].rci_level than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_dev_scheduler.fcg.drt.rates[index].num_active_links, 
                                     dev_schedulerGet.fcg.drt.rates[index].num_active_links, 
                                     "got another dev_scheduler.fcg.drt.rates[%d].num_active_links than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_dev_scheduler.fcg.drt.rates[index].rate, 
                                     dev_schedulerGet.fcg.drt.rates[index].rate, 
                                     "got another dev_scheduler.fcg.drt.rates[%d].rate than was set: %d", index, device_id);
        }

        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.valid_mask, 
                                     dev_schedulerGet.dev_if_scheduler.valid_mask, 
                                     "got another dev_scheduler.dev_if_scheduler.valid_mask than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.link_a_max_rate, 
                                     dev_schedulerGet.dev_if_scheduler.link_a_max_rate, 
                                     "got another dev_scheduler.dev_if_scheduler.link_a_max_rate than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.link_a_weight, 
                                     dev_schedulerGet.dev_if_scheduler.link_a_weight, 
                                     "got another dev_scheduler.dev_if_scheduler.link_a_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.link_b_max_rate, 
                                     dev_schedulerGet.dev_if_scheduler.link_b_max_rate, 
                                     "got another dev_scheduler.dev_if_scheduler.link_b_max_rate than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.link_b_weight, 
                                     dev_schedulerGet.dev_if_scheduler.link_b_weight, 
                                     "got another dev_scheduler.dev_if_scheduler.link_b_weight than was set: %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        dev_scheduler.valid_mask = 0xFFFFFF;

        for(index = 0; index < FAP20M_DRT_SIZE; index++)
        {
            dev_scheduler.fcg.drt.rates[index].rci_level = 7;
            dev_scheduler.fcg.drt.rates[index].num_active_links = 24;
            dev_scheduler.fcg.drt.rates[index].rate = 100;
        }

        dev_scheduler.dev_if_scheduler.valid_mask = 0xFFFFFF;
        dev_scheduler.dev_if_scheduler.link_a_max_rate = 100;
        dev_scheduler.dev_if_scheduler.link_a_weight = 100;
        dev_scheduler.dev_if_scheduler.link_b_max_rate = 100;
        dev_scheduler.dev_if_scheduler.link_b_weight = 100;

        result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler, &exact_dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_device_scheduler with not NULL dev_scheduler and sch_rank from 1.1.
            Expected: SAND_OK and tha same dev_scheduler as was set.
        */
        result = fap20m_get_device_scheduler(device_id, sch_rank, &dev_schedulerGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_device_scheduler: %d, %d", device_id, sch_rank);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.valid_mask, dev_schedulerGet.valid_mask,
                                     "got another dev_scheduler.valid_mask than was set: %d", device_id);

        for(index = 0; index < FAP20M_DRT_SIZE; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_dev_scheduler.fcg.drt.rates[index].rci_level, 
                                     dev_schedulerGet.fcg.drt.rates[index].rci_level, 
                                     "got another dev_scheduler.fcg.drt.rates[%d].rci_level than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_dev_scheduler.fcg.drt.rates[index].num_active_links, 
                                     dev_schedulerGet.fcg.drt.rates[index].num_active_links, 
                                     "got another dev_scheduler.fcg.drt.rates[%d].num_active_links than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_dev_scheduler.fcg.drt.rates[index].rate, 
                                     dev_schedulerGet.fcg.drt.rates[index].rate, 
                                     "got another dev_scheduler.fcg.drt.rates[%d].rate than was set: %d", index, device_id);
        }

        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.valid_mask, 
                                     dev_schedulerGet.dev_if_scheduler.valid_mask, 
                                     "got another dev_scheduler.dev_if_scheduler.valid_mask than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.link_a_max_rate, 
                                     dev_schedulerGet.dev_if_scheduler.link_a_max_rate, 
                                     "got another dev_scheduler.dev_if_scheduler.link_a_max_rate than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.link_a_weight, 
                                     dev_schedulerGet.dev_if_scheduler.link_a_weight, 
                                     "got another dev_scheduler.dev_if_scheduler.link_a_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.link_b_max_rate, 
                                     dev_schedulerGet.dev_if_scheduler.link_b_max_rate, 
                                     "got another dev_scheduler.dev_if_scheduler.link_b_max_rate than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_dev_scheduler.dev_if_scheduler.link_b_weight, 
                                     dev_schedulerGet.dev_if_scheduler.link_b_weight, 
                                     "got another dev_scheduler.dev_if_scheduler.link_b_weight than was set: %d", device_id);

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler, &exact_dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with dev_scheduler->fcg.drt.rates[0].rci_level [8], (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        dev_scheduler.fcg.drt.rates[0].rci_level = 8;

        result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler, &exact_dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, dev_scheduler.fcg.drt.rates[0].rci_level = %d", 
                                         device_id, dev_scheduler.fcg.drt.rates[0].rci_level);

        dev_scheduler.fcg.drt.rates[0].rci_level = 0;

        /*
            1.5. Call with dev_scheduler->fcg.drt.rates[0].num_active_links [24], (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        dev_scheduler.fcg.drt.rates[0].num_active_links = 24;

        result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler, &exact_dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, dev_scheduler.fcg.drt.rates[0].num_active_links = %d", 
                                         device_id, dev_scheduler.fcg.drt.rates[0].num_active_links);

        dev_scheduler.fcg.drt.rates[0].num_active_links = 0;

        /*
            1.6. Call with dev_scheduler [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_device_scheduler(device_id, sch_rank, NULL, &exact_dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, dev_scheduler = NULL", device_id);

        /*
            1.7. Call with exact_dev_scheduler [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_dev_scheduler = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    dev_scheduler.valid_mask = 0;

    for(index = 0; index < FAP20M_DRT_SIZE; index++)
    {
        dev_scheduler.fcg.drt.rates[index].rci_level = 0;
        dev_scheduler.fcg.drt.rates[index].num_active_links = 0;
        dev_scheduler.fcg.drt.rates[index].rate = 0;
    }

    dev_scheduler.dev_if_scheduler.valid_mask = 0;
    dev_scheduler.dev_if_scheduler.link_a_max_rate = 0;
    dev_scheduler.dev_if_scheduler.link_a_weight = 0;
    dev_scheduler.dev_if_scheduler.link_b_max_rate = 0;
    dev_scheduler.dev_if_scheduler.link_b_weight = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler, &exact_dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler, &exact_dev_scheduler);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_device_scheduler(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_INOUT  FAP20M_DEVICE_SCHEDULER      *dev_scheduler
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_device_scheduler)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null dev_scheduler. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with dev_scheduler [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;

    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_DEVICE_SCHEDULER dev_scheduler;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null dev_scheduler. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        result = fap20m_get_device_scheduler(device_id, sch_rank, &dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_device_scheduler(device_id, sch_rank, &dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_device_scheduler(device_id, sch_rank, &dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_PRIM;

        /*
            1.3. Call with dev_scheduler [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_device_scheduler(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, dev_scheduler = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_device_scheduler(device_id, sch_rank, &dev_scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_device_scheduler(device_id, sch_rank, &dev_scheduler);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_HR_scheduler_modes(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_IN     unsigned int                   use_enhanced_priority
    )
*/
UTF_TEST_CASE_MAC(fap20m_set_HR_scheduler_modes)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   use_enhanced_priority [0 / 1]
    Expected: SAND_OK.
    1.2. Call fap20m_get_HR_scheduler_modes with not NULL use_enhanced_priority and sch_rank from 1.1.
    Expected: SAND_OK and the same use_enhanced_priority as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT  use_enhanced_priority = 0;
    SAND_UINT  use_enhanced_priorityGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           use_enhanced_priority [0 / 1]
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        use_enhanced_priority = 0;

        result = fap20m_set_HR_scheduler_modes(device_id, sch_rank, use_enhanced_priority);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, use_enhanced_priority);

        /*
            1.2. Call fap20m_get_HR_scheduler_modes with not NULL use_enhanced_priority and sch_rank from 1.1.
            Expected: SAND_OK and the same use_enhanced_priority as was set.
        */
        result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, &use_enhanced_priorityGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_set_HR_scheduler_modes: %d, %d", device_id, sch_rank);

        UTF_VERIFY_EQUAL2_STRING_MAC(use_enhanced_priority, use_enhanced_priorityGet, 
                                     "got another use_enhanced_priority than was set: %d, %d", device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        use_enhanced_priority = 1;

        result = fap20m_set_HR_scheduler_modes(device_id, sch_rank, use_enhanced_priority);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, use_enhanced_priority);

        /*
            1.2. Call fap20m_get_HR_scheduler_modes with not NULL use_enhanced_priority and sch_rank from 1.1.
            Expected: SAND_OK and the same use_enhanced_priority as was set.
        */
        result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, &use_enhanced_priorityGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_set_HR_scheduler_modes: %d, %d", device_id, sch_rank);

        UTF_VERIFY_EQUAL2_STRING_MAC(use_enhanced_priority, use_enhanced_priorityGet, 
                                     "got another use_enhanced_priority than was set: %d, %d", device_id, sch_rank);

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_HR_scheduler_modes(device_id, sch_rank, use_enhanced_priority);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;
    use_enhanced_priority = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_HR_scheduler_modes(device_id, sch_rank, use_enhanced_priority);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_HR_scheduler_modes(device_id, sch_rank, use_enhanced_priority);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_HR_scheduler_modes(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_OUT    unsigned int*                  use_enhanced_priority
    )
*/
UTF_TEST_CASE_MAC(fap20m_get_HR_scheduler_modes)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null use_enhanced_priority. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with use_enhanced_priority [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT  use_enhanced_priority = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null use_enhanced_priority. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, &use_enhanced_priority);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, &use_enhanced_priority);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, &use_enhanced_priority);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_PRIM;

        /*
            1.3. Call with use_enhanced_priority [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, use_enhanced_priority = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, &use_enhanced_priority);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, &use_enhanced_priority);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_scheduler_class_type_params(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_IN     FAP20M_CL_SCHED_CLASS_PARAMS *class_type_params,
    SAND_OUT    FAP20M_CL_SCHED_CLASS_PARAMS *exact_class_type_params
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_scheduler_class_type_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   class_type_params { id [0 / 31],
                                       mode [FAP20M_CL_SCHED_CLASS_MODE_2 / FAP20M_CL_SCHED_CLASS_MODE_3],
                                       weight[FAP20M_MAX_NOF_DISCRETE_WEIGHT_VALS],
                                       weight_mode [FAP20M_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW / FAP20M_CLASS_WEIGHTS_MODE_DISCRETE_PER_FLOW] },
                   not NULL exact_class_type_params.
    Expected: SAND_OK.
    1.2. Call fap20m_get_scheduler_class_type_params with not NULL class_type_params and sch_rank from 1.1.
    Expected: SAND_OK and tha same class_type_params as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with class_type_params->id [32], (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with class_type_params->mode [0x5AAAAAA5], (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with class_type_params->weight_mode [0x5AAAAAA5], (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with class_type_params [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.8. Call with exact_class_type_params [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;

    FAP20M_CL_SCHED_CLASS_PARAMS class_type_params;
    FAP20M_CL_SCHED_CLASS_PARAMS class_type_paramsGet;
    FAP20M_CL_SCHED_CLASS_PARAMS exact_class_type_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           class_type_params { id [0 / 31],
                                               mode [FAP20M_CL_SCHED_CLASS_MODE_2 / FAP20M_CL_SCHED_CLASS_MODE_3],
                                               weight[FAP20M_MAX_NOF_DISCRETE_WEIGHT_VALS] [1 / 1023],
                                               weight_mode [FAP20M_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW / FAP20M_CLASS_WEIGHTS_MODE_DISCRETE_PER_FLOW] },
                           not NULL exact_class_type_params.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM  */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        class_type_params.id = 0;
        class_type_params.mode = FAP20M_CL_SCHED_CLASS_MODE_2;

        class_type_params.weight[0] = 1;
        class_type_params.weight[1] = 1;
        class_type_params.weight[2] = 1;
        class_type_params.weight[3] = 1;

        class_type_params.weight_mode = FAP20M_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW;

        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* initialize class_type_paramsGet id */
        class_type_paramsGet.id = class_type_params.id;

        /*
            1.2. Call fap20m_get_scheduler_class_type_params with not NULL class_type_params and sch_rank from 1.1.
            Expected: SAND_OK and tha same class_type_params as was set.
        */
        result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, &class_type_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_class_type_params: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.mode, class_type_paramsGet.mode, 
                                     "got another class_type_params.mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight[0], class_type_paramsGet.weight[0], 
                                     "got another class_type_params.weight[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight[1], class_type_paramsGet.weight[1], 
                                     "got another class_type_params.weight[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight[2], class_type_paramsGet.weight[2], 
                                     "got another class_type_params.weight[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight[3], class_type_paramsGet.weight[3], 
                                     "got another class_type_params.weight[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight_mode, class_type_paramsGet.weight_mode, 
                                     "got another class_type_params.weight_mode than was set: %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND  */
        sch_rank = FAP20M_SCH_RANK_SCND;

        class_type_params.id = 31;
        class_type_params.mode = FAP20M_CL_SCHED_CLASS_MODE_3;

        class_type_params.weight[0] = 1023;
        class_type_params.weight[1] = 1023;
        class_type_params.weight[2] = 1023;
        class_type_params.weight[3] = 1023;

        class_type_params.weight_mode = FAP20M_CLASS_WEIGHTS_MODE_DISCRETE_PER_FLOW;

        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* initialize class_type_paramsGet id */
        class_type_paramsGet.id = class_type_params.id;

        /*
            1.2. Call fap20m_get_scheduler_class_type_params with not NULL class_type_params and sch_rank from 1.1.
            Expected: SAND_OK and tha same class_type_params as was set.
        */
        result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, &class_type_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_class_type_params: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.mode, class_type_paramsGet.mode, 
                                     "got another class_type_params.mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight[0], class_type_paramsGet.weight[0], 
                                     "got another class_type_params.weight[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight[1], class_type_paramsGet.weight[1], 
                                     "got another class_type_params.weight[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight[2], class_type_paramsGet.weight[2], 
                                     "got another class_type_params.weight[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight[3], class_type_paramsGet.weight[3], 
                                     "got another class_type_params.weight[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_class_type_params.weight_mode, class_type_paramsGet.weight_mode, 
                                     "got another class_type_params.weight_mode than was set: %d", device_id);

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with class_type_params->id [32], (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        class_type_params.id = 32;

        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, class_type_params.id = %d", device_id, class_type_params.id);

        class_type_params.id = 0;

        /*
            1.5. Call with class_type_params->mode [0x5AAAAAA5], (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        class_type_params.mode = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, class_type_params.mode = %d", device_id, class_type_params.mode);

        class_type_params.mode = FAP20M_CL_SCHED_CLASS_MODE_2;

        /*
            1.6. Call with class_type_params->weight_mode [0x5AAAAAA5], (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        class_type_params.weight_mode = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, class_type_params.weight_mode = %d", device_id, class_type_params.weight_mode);

        class_type_params.weight_mode = FAP20M_CLASS_WEIGHTS_MODE_DISCRETE_PER_FLOW;

        /*
            1.7. Call with class_type_params [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, NULL, &exact_class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, class_type_params = NULL", device_id);

        /*
            1.8. Call with exact_class_type_params [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_class_type_params = NULL", device_id);
    }

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_scheduler_class_type_params(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_INOUT  FAP20M_CL_SCHED_CLASS_PARAMS *class_type_params
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_scheduler_class_type_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null class_type_params. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with class_type_params [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_CL_SCHED_CLASS_PARAMS class_type_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null class_type_params. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        /* initialize index */
        class_type_params.id = 0;

        result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, &class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, &class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, &class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_PRIM;

        /*
            1.3. Call with class_type_params [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, class_type_params = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, &class_type_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, &class_type_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_scheduler_class_type_params_table(
    SAND_IN     unsigned int                     device_id,
    SAND_IN     FAP20M_SCH_RANK                  sch_rank,
    SAND_IN     FAP20M_SCT                     *sct,
    SAND_OUT    FAP20M_SCT                     *exact_sct
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_scheduler_class_type_params_table)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   sct { class_types[FAP20M_SCH_NOF_CLASS_TYPES]{  id [0 / 31],
                                                                   mode [FAP20M_CL_SCHED_CLASS_MODE_2 / 
                                                                         FAP20M_CL_SCHED_CLASS_MODE_3],
                                                                   weight[FAP20M_MAX_NOF_DISCRETE_WEIGHT_VALS],
                                                                   weight_mode [FAP20M_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW /
                                                                                FAP20M_CLASS_WEIGHTS_MODE_DISCRETE_PER_FLOW] }
                         nof_class_types [1 / FAP20M_SCH_NOF_CLASS_TYPES] },
                   not NULL exact_sct.
    Expected: SAND_OK.
    1.2. Call fap20m_get_scheduler_class_type_params_table with not NULL sct and sch_rank from 1.1.
    Expected: SAND_OK and tha same sct as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with sct.class_types[0].id [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with sct.class_types[0].mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with sct.class_types[0].weight[0] [0 / 1024] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with sct.class_types[0].weight_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with sct->nof_class_types [FAP20M_SCH_NOF_CLASS_TYPES+1] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.9. Call with sct [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.10. Call with exact_sct [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT  index = 0;

    FAP20M_SCT sct;
    FAP20M_SCT sctGet;
    FAP20M_SCT exact_sct;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           sct { class_types[FAP20M_SCH_NOF_CLASS_TYPES]{  id [0 / 31],
                                                                           mode [FAP20M_CL_SCHED_CLASS_MODE_2 / 
                                                                                 FAP20M_CL_SCHED_CLASS_MODE_3],
                                                                           weight[FAP20M_MAX_NOF_DISCRETE_WEIGHT_VALS],
                                                                           weight_mode [FAP20M_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW /
                                                                                        FAP20M_CLASS_WEIGHTS_MODE_DISCRETE_PER_FLOW] }
                                 nof_class_types [1 / FAP20M_SCH_NOF_CLASS_TYPES] },
                           not NULL exact_sct.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM  */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        for(index = 0; index < FAP20M_SCH_NOF_CLASS_TYPES; index++)
        {
            sct.class_types[index].id = 0;
            sct.class_types[index].mode = FAP20M_CL_SCHED_CLASS_MODE_2;

            sct.class_types[index].weight[0] = 1;
            sct.class_types[index].weight[1] = 1;
            sct.class_types[index].weight[2] = 1;
            sct.class_types[index].weight[3] = 1;

            sct.class_types[index].weight_mode = FAP20M_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW;

            /* initialize id of the get parameter */
            sctGet.class_types[index].id = 0;
        }

        sct.nof_class_types = 1;

        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* initialize nof_class_types of the get parameter */
        sctGet.nof_class_types = sct.nof_class_types;

        /*
            1.2. Call fap20m_get_scheduler_class_type_params_table with not NULL sct and sch_rank from 1.1.
            Expected: SAND_OK and the same sct as was set.
        */
        result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, &sctGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_class_type_params_table: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sct.class_types[0].mode, sctGet.class_types[0].mode,
                                     "got another sct.class_types[0].mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sct.class_types[0].weight[0], sctGet.class_types[0].weight[0], 
                                     "got another sct.class_types[0].weight[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sct.class_types[0].weight[1], sctGet.class_types[0].weight[1], 
                                     "got another sct.class_types[0].weight[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sct.class_types[0].weight[2], sctGet.class_types[0].weight[2], 
                                     "got another sct.class_types[0].weight[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sct.class_types[0].weight[3], sctGet.class_types[0].weight[3], 
                                     "got another sct.class_types[0].weight[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sct.class_types[0].weight_mode, sctGet.class_types[0].weight_mode, 
                                     "got another sct.class_types[0].weight_mode than was set: %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM  */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        for(index = 0; index < FAP20M_SCH_NOF_CLASS_TYPES; index++)
        {
            sct.class_types[index].id = 0;
            sct.class_types[index].mode = FAP20M_CL_SCHED_CLASS_MODE_2;

            sct.class_types[index].weight[0] = 1;
            sct.class_types[index].weight[1] = 1;
            sct.class_types[index].weight[2] = 1;
            sct.class_types[index].weight[3] = 1;

            sct.class_types[index].weight_mode = FAP20M_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW;
        }

        sct.nof_class_types = 31;

        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* initialize nof_class_types of the get parameter */
        sctGet.nof_class_types = sct.nof_class_types;

        /*
            1.2. Call fap20m_get_scheduler_class_type_params_table with not NULL sct and sch_rank from 1.1.
            Expected: SAND_OK and tha same sct as was set.
        */
        result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, &sctGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_class_type_params_table: %d, %d", device_id, sch_rank);

        /* Verifying values */
        for(index = 0; index < FAP20M_SCH_NOF_CLASS_TYPES; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_sct.class_types[index].mode, sctGet.class_types[index].mode,
                                         "got another sct.class_types[%d].mode than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_sct.class_types[index].weight[0], sctGet.class_types[index].weight[0], 
                                         "got another sct.class_types[%d].weight[0] than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_sct.class_types[index].weight[1], sctGet.class_types[index].weight[1], 
                                         "got another sct.class_types[%d].weight[1] than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_sct.class_types[index].weight[2], sctGet.class_types[index].weight[2], 
                                         "got another sct.class_types[%d].weight[2] than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_sct.class_types[index].weight[3], sctGet.class_types[index].weight[3], 
                                         "got another sct.class_types[%d].weight[3] than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_sct.class_types[index].weight_mode, sctGet.class_types[index].weight_mode, 
                                         "got another sct.class_types[%d].weight_mode than was set: %d", index, device_id);
        }

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with sct.class_types[0].id [32] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sct.class_types[0].id = 32;

        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sct.class_types[0].id = %d", device_id, sct.class_types[0].id);

        sct.class_types[0].id = 0;

        /*
            1.5. Call with sct.class_types[0].mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sct.class_types[0].mode = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sct.class_types[0].mode = %d", device_id, sct.class_types[0].mode);

        sct.class_types[0].mode = FAP20M_CL_SCHED_CLASS_MODE_2;

        /*
            1.6. Call with sct.class_types[0].weight[0] [0 / 1024] (out of range) and other params from 1.1.
            Expected: SAND_OK because the function does not check weights.
                      the waits are used only on some configurations and weight 
                      check is complex routine.
        */
        /* uterate with sct.class_types[0].weight[0] = 0 */
        sct.class_types[0].weight[0] = 0;

        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sct.class_types[0].weight[0] = %d", device_id, sct.class_types[0].weight[0]);

        /* uterate with sct.class_types[0].weight[0] = 1024 */
        sct.class_types[0].weight[0] = 1024;

        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sct.class_types[0].weight[0] = %d", device_id, sct.class_types[0].weight[0]);

        sct.class_types[0].weight[0] = 1;

        /*
            1.7. Call with sct.class_types[0].weight_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sct.class_types[0].weight_mode = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sct.class_types[0].weight_mode = %d", device_id, sct.class_types[0].weight_mode);

        sct.class_types[0].weight_mode = FAP20M_CLASS_WEIGHTS_MODE_DISCRETE_PER_FLOW;

        /*
            1.8. Call with class_type_params [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, NULL, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sct = NULL", device_id);

        /*
            1.9. Call with exact_class_type_params [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_sct = NULL", device_id);
    }

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct, &exact_sct);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_scheduler_class_type_params_table(
    SAND_IN     unsigned int                     device_id,
    SAND_IN     FAP20M_SCH_RANK                  sch_rank,
    SAND_OUT    FAP20M_SCT                     *sct
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_scheduler_class_type_params_table)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null sct. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with sct [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_SCT sct;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null sct.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        /* initialize indexes */
        sct.nof_class_types = 1;
        sct.class_types[0].id = 0;

        result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, &sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, &sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, &sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_PRIM;

        /*
            1.3. Call with class_type_params [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sct = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, &sct);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, &sct);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_scheduler_port(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_IN     FAP20M_ELEMENTARY_SCHEDULER        *scheduler
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_scheduler_port)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / 
                             FAP20M_SCH_RANK_SCND],
                   scheduler { id [0 / 31],
                               type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR / 
                                     FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL]
                               state [FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE / 
                                      FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE]
                               sched_properties {hr.mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ / 
                                                          FAP20M_HR_SCHED_MODE_DUAL_WFQ],
                                                 cl.class_type [0 / 31] } }.
    Expected: SAND_OK.
    1.2. Call fap20m_get_scheduler_port with not NULL scheduler and sch_rank from 1.1.
    Expected: SAND_OK and the same scheduler as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with scheduler->id [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with scheduler->type[0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with scheduler->state[0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with scheduler->type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR], 
                   scheduler->sched_properties.hr.mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with scheduler->type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL], 
                   scheduler->sched_properties.cl.class_type [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.9. Call with scheduler [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;

    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    FAP20M_ELEMENTARY_SCHEDULER schedulerGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / 
                                     FAP20M_SCH_RANK_SCND],
                           scheduler { id [0 / 31],
                                       type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR / 
                                             FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL]
                                       state [FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE / 
                                              FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE]
                                       sched_properties {hr.mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ / 
                                                                  FAP20M_HR_SCHED_MODE_DUAL_WFQ],
                                                         cl.class_type [0 / 31] } }.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        scheduler.id = 0;
        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
        scheduler.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;
        scheduler.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* initialize schedulerGet id */
        schedulerGet.id = scheduler.id;

        /*
            1.2. Call fap20m_get_scheduler_port with not NULL scheduler and sch_rank from 1.1.
            Expected: SAND_OK and the same scheduler as was set.
        */
        result = fap20m_get_scheduler_port(device_id, sch_rank, &schedulerGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_port: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.type, schedulerGet.type, 
                                     "got another scheduler.type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.state, schedulerGet.state, 
                                     "got another scheduler.state than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.sched_properties.hr.mode, schedulerGet.sched_properties.hr.mode, 
                                     "got another scheduler.sched_properties.hr.mode than was set: %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        scheduler.id = 31;
        /* only HR schedulers are used for port */
        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
        scheduler.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE;
        /* CL specific property, shouldn't be checked */
        scheduler.sched_properties.cl.class_type = 0;

        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* initialize schedulerGet id */
        schedulerGet.id = scheduler.id;

        /*
            1.2. Call fap20m_get_scheduler_port with not NULL scheduler and sch_rank from 1.1.
            Expected: SAND_OK and the same scheduler as was set.
        */
        result = fap20m_get_scheduler_port(device_id, sch_rank, &schedulerGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_port: %d, %d", device_id, sch_rank);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.type, schedulerGet.type, 
                                     "got another scheduler.type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.state, schedulerGet.state, 
                                     "got another scheduler.state than was set: %d", device_id);

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with scheduler->id [65] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = FAP20M_SCH_RANK_SCND;
        scheduler.id = 65;

        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler.id = %d", device_id, scheduler.id);

        scheduler.id = 0;

        /*
            1.5. Call with scheduler->type[0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler.type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler.type = %d", device_id, scheduler.type);

        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;

        /*
            1.6. Call with scheduler->state[0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler.state = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler.state = %d", device_id, scheduler.state);

        scheduler.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;

        /*
            1.7. Call with scheduler->type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR], 
                           scheduler->sched_properties.hr.mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
        scheduler.sched_properties.hr.mode = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, scheduler.type = %d, scheduler.sched_properties.hr.mode = %d",
                                         device_id, scheduler.type, scheduler.sched_properties.hr.mode);

        scheduler.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

        /*
            1.8. Call with scheduler->type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL], 
                           scheduler->sched_properties.cl.class_type [32] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;
        scheduler.sched_properties.cl.class_type = 32;

        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, scheduler.type = %d, scheduler.sched_properties.cl.class_type = %d",
                                         device_id, scheduler.type, scheduler.sched_properties.cl.class_type);

        scheduler.sched_properties.cl.class_type = 0;

        /*
            1.9. Call with scheduler [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_port(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, scheduler = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    scheduler.id = 0;
    scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
    scheduler.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;
    scheduler.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_scheduler_port(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_INOUT  FAP20M_ELEMENTARY_SCHEDULER      *scheduler
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_scheduler_port)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null scheduler. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with scheduler [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_ELEMENTARY_SCHEDULER scheduler;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null scheduler.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        scheduler.id = 0;

        result = fap20m_get_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_PRIM;

        /*
            1.3. Call with scheduler [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scheduler_port(device_id, sch_rank, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, scheduler = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_scheduler_port(device_id, sch_rank, &scheduler);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_is_scheduler_port_dynamically_used(
    SAND_IN     unsigned int      device_id,
    SAND_IN     FAP20M_SCH_RANK   sch_rank,
    SAND_IN     unsigned int      port_id,
    SAND_OUT    unsigned int      *is_dynamically_used
  )
*/
UTF_TEST_CASE_MAC(fap20m_is_scheduler_port_dynamically_used)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   port_id [0 / 64],
                   non-null is_dynamically_used. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with port_id [65] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with is_dynamically_used [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT port_id = 0;
    SAND_UINT is_dynamically_used = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           port_id [0 / 63],
                           non-null is_dynamically_used. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        port_id = 0;

        result = fap20m_is_scheduler_port_dynamically_used(device_id, sch_rank, port_id, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        port_id = 63;

        result = fap20m_is_scheduler_port_dynamically_used(device_id, sch_rank, port_id, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port_id);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_is_scheduler_port_dynamically_used(device_id, sch_rank, port_id, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call with port_id [65] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_id = 65;

        result = fap20m_is_scheduler_port_dynamically_used(device_id, sch_rank, port_id, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port_id);

        port_id = 0;

        /*
            1.4. Call with is_dynamically_used [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_is_scheduler_port_dynamically_used(device_id, sch_rank, port_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, is_dynamically_used = NULL", device_id);
    }

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_is_scheduler_port_dynamically_used(device_id, sch_rank, port_id, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_is_scheduler_port_dynamically_used(device_id, sch_rank, port_id, &is_dynamically_used);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_scheduler_aggregate(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_IN     FAP20M_ELEMENTARY_SCHEDULER      *scheduler,
    SAND_IN     FAP20M_FLOW                      *flow,
    SAND_OUT    FAP20M_FLOW                      *exact_flow_given
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_scheduler_aggregate)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   scheduler { id [0 / 31],
                               type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR / 
                                     FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL]
                               state [FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE / 
                                      FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE]
                               sched_properties {hr.mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ / 
                                                          FAP20M_HR_SCHED_MODE_DUAL_WFQ],
                                                 cl.class_type [0 / 31] } }.
                   flow { id [64 / 16K] ,
                          sub_flow[FAP20M_NOF_SUB_FLOWS] { valid_mask [0 / 0xFFFFFF],
                                                           id [64 / 16K],
                                                           credit_source [0 / 100],
                                                           shaper { max_rate[0 / 100], max_burst [0 / 100]},
                                                           type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                           subflow_properties  { hr { sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF1 / FAP20M_SUB_FLOW_HR_CLASS_EF2];
                                                                                      weight [0 / 100]};
                                                                                 cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP1 / FAP20M_SUB_FLOW_CL_CLASS_SP2]
                                                                                      weight [1 / 100] } }
                                                          },
                          flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW] ,
                          is_slow_enabled [0 / 1] }
                   non-null exact_flow_given. 
    Expected: SAND_OK.
    1.2. Call fap20m_get_scheduler_aggregate with not NULL scheduler, flow and other params from 1.1.
    Expected: SAND_OK and the same scheduler, flow as was set.

    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with scheduler->id [1281] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with scheduler->type[0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with scheduler->state[0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with scheduler->type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR], 
                   scheduler->sched_properties.hr.mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with scheduler->type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL], 
                   scheduler->sched_properties.cl.class_type [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.9. Call with flow->sub_flow[0].type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.10. Call with flow->sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_HR], 
                   flow->sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.11. Call with scheduler->type [FAP20M_SUB_FLOW_TYPE_CL], 
                   flow->sub_flow[0].subflow_properties.cl.class_val [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.12. Call with scheduler [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.13. Call with flow [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.14. Call with exact_flow_given [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;

    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    FAP20M_FLOW flow;
    FAP20M_FLOW exact_flow_given;

    FAP20M_ELEMENTARY_SCHEDULER schedulerGet;
    FAP20M_FLOW flowGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           scheduler { id [0 / 31],
                                       type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR / 
                                             FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL]
                                       state [FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE / 
                                              FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE]
                                       sched_properties {hr.mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ / 
                                                                  FAP20M_HR_SCHED_MODE_DUAL_WFQ],
                                                         cl.class_type [0 / 31] } }.
                           flow { id [64 / 16K] ,
                                  sub_flow[FAP20M_NOF_SUB_FLOWS] { valid_mask [0 / 0xFFFFFF],
                                                                   id [64 / 16K],
                                                                   credit_source [0 / 100],
                                                                   shaper { max_rate[0 / 100], max_burst [0 / 100]},
                                                                   type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                                   subflow_properties  { hr { sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF1 / FAP20M_SUB_FLOW_HR_CLASS_EF2];
                                                                                              weight [0 / 100]};
                                                                                         cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP1 / FAP20M_SUB_FLOW_CL_CLASS_SP2]
                                                                                              weight [1 / 100] } }
                                                                  },
                                  flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW] ,
                                  is_slow_enabled [0 / 1] }
                           non-null exact_flow_given. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        /* scheduler.id = 0; */

        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
        scheduler.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;
        scheduler.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

        scheduler.id = flow.id = 66;

        flow.sub_flow[0].valid_mask = 0;
        flow.sub_flow[0].id = 66;
        flow.sub_flow[0].credit_source = 0;
        flow.sub_flow[0].shaper.max_rate = 0;
        flow.sub_flow[0].shaper.max_burst = 0;
        flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
        flow.sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_SINGLE_CLASS_AF1_WFQ;
        flow.sub_flow[0].subflow_properties.hr.weight = 2;

        flow.sub_flow[1].valid_mask = 0;
        flow.sub_flow[1].id = 67;
        flow.sub_flow[1].credit_source = 0;
        flow.sub_flow[1].shaper.max_rate = 0;
        flow.sub_flow[1].shaper.max_burst = 0;
        flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_HR;
        flow.sub_flow[1].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_SINGLE_CLASS_AF1_WFQ;
        flow.sub_flow[1].subflow_properties.hr.weight = 100;

        flow.flow_type = FAP20M_AGGREGATE_SCH_FLOW;
        flow.is_slow_enabled = 0;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* initialize schedulerGet id */
        schedulerGet.id = scheduler.id;

        /*
            1.2. Call fap20m_get_scheduler_aggregate with not NULL scheduler, flow and other params from 1.1.
            Expected: SAND_OK and the same scheduler, flow as was set.
        */
        result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &schedulerGet, &flowGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_aggregate: %d, %d", device_id, sch_rank);

        /* verifying values - scheduler */
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.id, schedulerGet.id, 
                                     "got another scheduler.id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.type, schedulerGet.type, 
                                     "got another scheduler.type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.state, schedulerGet.state, 
                                     "got another scheduler.state than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.sched_properties.hr.mode, schedulerGet.sched_properties.hr.mode, 
                                     "got another scheduler.sched_properties.hr.mode than was set : %d", device_id);

        /* verifying values - flow */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.id, flowGet.id, 
                                     "got another flow.id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.flow_type, flowGet.flow_type, 
                                     "got another flow.flow_type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.is_slow_enabled, flowGet.is_slow_enabled, 
                                     "got another flow.flow_type than was set : %d", device_id);

        /* verifying values - flow.sub_flow[0] */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].id, flowGet.sub_flow[0].id, 
                                     "got another flow.sub_flow[0].id than was set : %d", device_id);
        /* verifying values - flow.sub_flow[1] */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].id, flowGet.sub_flow[1].id, 
                                     "got another flow.sub_flow[1].id than was set : %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        /* scheduler.id = 31; */

        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;
        scheduler.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;
        scheduler.sched_properties.cl.class_type  = 31;

        scheduler.id = flow.id = 0x100;
        flow.sub_flow[0].valid_mask = 0xFFFFFF;
        flow.sub_flow[0].id = 0x100;
        flow.sub_flow[0].credit_source = 100;
        flow.sub_flow[0].shaper.max_rate = 100;
        flow.sub_flow[0].shaper.max_burst = 100;
        flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
        flow.sub_flow[0].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP3_WFQ1;
        flow.sub_flow[0].subflow_properties.cl.weight = 1;

        flow.sub_flow[1].valid_mask = 0xFFFFFF;
        flow.sub_flow[1].id = 0x101;
        flow.sub_flow[1].credit_source = 100;
        flow.sub_flow[1].shaper.max_rate = 100;
        flow.sub_flow[1].shaper.max_burst = 100;
        flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_CL;
        flow.sub_flow[1].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP3_WFQ1;
        flow.sub_flow[1].subflow_properties.cl.weight = 1;

		/* the flow type will be cleared in the aggregation process */
        flow.flow_type = FAP20M_AGGREGATE_SCH_FLOW;
        flow.is_slow_enabled = 1;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* update schedulerGet id */
        schedulerGet.id = scheduler.id;

        /*
            1.2. Call fap20m_get_scheduler_aggregate with not NULL scheduler, flow and other params from 1.1.
            Expected: SAND_OK and the same scheduler, flow as was set.
        */
        result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &schedulerGet, &flowGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_aggregate: %d, %d", device_id, sch_rank);

        /* verifying values - scheduler */
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.id, schedulerGet.id, 
                                     "got another scheduler.id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.type, schedulerGet.type, 
                                     "got another scheduler.type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.state, schedulerGet.state, 
                                     "got another scheduler.state than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(scheduler.sched_properties.hr.mode, schedulerGet.sched_properties.hr.mode, 
                                     "got another scheduler.sched_properties.hr.mode than was set : %d", device_id);

        /* verifying values - flow */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.id, flowGet.id, 
                                     "got another flow.id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.flow_type, flowGet.flow_type, 
                                     "got another flow.flow_type than was set : %d", device_id);
 
        /* verifying values - flow.sub_flow[0] */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].id, flowGet.sub_flow[0].id, 
                                     "got another flow.sub_flow[0].id than was set : %d", device_id);
        /* verifying values - flow.sub_flow[1] */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].id, flowGet.sub_flow[1].id, 
                                     "got another flow.sub_flow[1].id than was set : %d", device_id);

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with scheduler->id [1281] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler.id = 1281;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler.id = %d", device_id, scheduler.id);

        scheduler.id = 0;

        /*
            1.5. Call with scheduler->type[0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler.type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler.type = %d", device_id, scheduler.type);

        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;

        /*
            1.6. Call with scheduler->state[0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler.state = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler.state = %d", device_id, scheduler.state);

        scheduler.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;

        /*
            1.7. Call with scheduler->type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR], 
                           scheduler->sched_properties.hr.mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
        scheduler.sched_properties.hr.mode = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, scheduler.type = %d, scheduler.sched_properties.hr.mode = %d", 
                                         device_id, scheduler.type, scheduler.sched_properties.hr.mode);

        scheduler.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

        /*
            1.8. Call with scheduler->type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL], 
                           scheduler->sched_properties.cl.class_type [32] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
		sch_rank = FAP20M_SCH_RANK_SCND;
        scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
		scheduler.id = 65;
        scheduler.sched_properties.cl.class_type = 32;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, scheduler.type = %d, scheduler.sched_properties.cl.class_type = %d", 
                                         device_id, scheduler.type, scheduler.sched_properties.cl.class_type);

        scheduler.sched_properties.cl.class_type = 0;

        /*
            1.9. Call with flow->sub_flow[0].type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow.sub_flow[0].type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, flow.sub_flow[0].type = %d", device_id, flow.sub_flow[0].type);

        flow.sub_flow[0].type = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;

        /*
            1.11. Call with flow->sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_HR], 
                           flow->sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
        flow.sub_flow[0].subflow_properties.hr.sp_class = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, flow.sub_flow[0].type = %d, flow.sub_flow[0].subflow_properties.hr.sp_class = %d", 
                                         device_id, flow.sub_flow[0].type, flow.sub_flow[0].subflow_properties.hr.sp_class);

        flow.sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;

        /*
            1.11. Call with scheduler->type [FAP20M_SUB_FLOW_TYPE_CL], 
                           flow->sub_flow[0].subflow_properties.cl.class_val [32] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
        flow.sub_flow[0].subflow_properties.cl.class_val = 32;

        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, flow.sub_flow[0].type = %d, flow.sub_flow[0].subflow_properties.cl.class_val = %d", 
                                         device_id, flow.sub_flow[0].type, flow.sub_flow[0].subflow_properties.cl.class_val);

        flow.sub_flow[1].subflow_properties.cl.class_val = 0;

        /*
            1.12. Call with scheduler [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, NULL, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, scheduler = NULL", device_id);

        /*
            1.13. Call with flow [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, NULL, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, flow = NULL", device_id);

        /*
            1.14. Call with exact_flow_given [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_flow_given = NULL", device_id);        
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;
    scheduler.id = 0;

    scheduler.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
    scheduler.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;
    scheduler.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

    flow.id = 66;
    flow.sub_flow[0].valid_mask = 0;
    flow.sub_flow[0].id = 67;
    flow.sub_flow[0].credit_source = 0;
    flow.sub_flow[0].shaper.max_rate = 0;
    flow.sub_flow[0].shaper.max_burst = 0;
    flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
    flow.sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF1;
    flow.sub_flow[0].subflow_properties.hr.weight = 0;

    flow.sub_flow[1].valid_mask = 0;
    flow.sub_flow[1].id = 66;
    flow.sub_flow[1].credit_source = 0;
    flow.sub_flow[1].shaper.max_rate = 0;
    flow.sub_flow[1].shaper.max_burst = 0;
    flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_HR;
    flow.sub_flow[1].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;
    flow.sub_flow[1].subflow_properties.hr.weight = 100;

    flow.flow_type = FAP20M_AGGREGATE_SCH_FLOW;
    flow.is_slow_enabled = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow, &exact_flow_given);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_scheduler_aggregate(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_INOUT  FAP20M_ELEMENTARY_SCHEDULER      *scheduler,
    SAND_OUT    FAP20M_FLOW                      *flow
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_scheduler_aggregate)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null scheduler, flow.
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with scheduler [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with flow [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 1;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 1;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    FAP20M_FLOW flow;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null scheduler, flow.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        scheduler.id = 0;

        result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call with scheduler [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scheduler_aggregate(device_id, sch_rank, NULL, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, scheduler = NULL", device_id);

        /*
            1.4. Call with flow [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &scheduler, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, flow = NULL", device_id);
    }

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_scheduler_flow(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_IN     unsigned int                       source_fap_id,
    SAND_IN     unsigned int                       source_queue_id,
    SAND_IN     FAP20M_FLOW                        *flow,
    SAND_OUT    FAP20M_FLOW                        *exact_flow_given
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_scheduler_flow)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   source_fap_id [0 / 2047],
                   source_queue_id [0 / 8191],
                   flow { id [64 / 16K] ,
                          sub_flow[FAP20M_NOF_SUB_FLOWS] { valid_mask [0 / 0xFFFFFF],
                                                           id [64 / 16K],
                                                           credit_source [0 / 100],
                                                           shaper { max_rate[0 / 100], max_burst [0 / 100]},
                                                           type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                           subflow_properties  { hr { sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF1 / FAP20M_SUB_FLOW_HR_CLASS_EF2];
                                                                                      weight [0 / 100]};
                                                                                 cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP1 / FAP20M_SUB_FLOW_CL_CLASS_SP2]
                                                                                      weight [1 / 100] } }
                                                          },
                          flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW] ,
                          is_slow_enabled [0 / 1] },
                   non-null exact_flow_given. 
    Expected: SAND_OK.
    1.2. Call FAP20M_get_scheduler_flow with not NULL source_fap_id, source_queue_id, flow.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with source_fap_id [2048] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with source_queue_id [8192] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with flow->sub_flow[0].type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with flow->sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_HR], 
                   flow->sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with scheduler->type [FAP20M_SUB_FLOW_TYPE_CL], 
                   flow->sub_flow[0].subflow_properties.cl.class_val [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.9. Call with flow [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.10. Call with exact_flow_given [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT source_fap_id = 0;
    SAND_UINT source_queue_id = 0;

    FAP20M_FLOW flow;
    FAP20M_FLOW exact_flow_given;
    FAP20M_FLOW flowGet;

    SAND_UINT source_fap_idGet = 0;
    SAND_UINT source_queue_idGet = 0;
    

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           source_fap_id [0 / 2047],
                           source_queue_id [0 / 8191],
                           flow { id [64 / 16K] ,
                                  sub_flow[FAP20M_NOF_SUB_FLOWS] { valid_mask [0 / 0xFFFFFF],
                                                                   id [64 / 16K],
                                                                   credit_source [0 / 100],
                                                                   shaper { max_rate[0 / 100], max_burst [0 / 100]},
                                                                   type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                                   subflow_properties  { hr { sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF1 / FAP20M_SUB_FLOW_HR_CLASS_EF2];
                                                                                              weight [0 / 100]};
                                                                                         cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP1 / FAP20M_SUB_FLOW_CL_CLASS_SP2]
                                                                                              weight [1 / 100] } }
                                                                  },
                                  flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW] ,
                                  is_slow_enabled [0 / 1] },
                           non-null exact_flow_given. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        source_fap_id = 0;
        source_queue_id = 0;

        flow.id = 66;
        flow.sub_flow[0].valid_mask = 0;
        flow.sub_flow[0].id = 66;
        flow.sub_flow[0].credit_source = 0;
        flow.sub_flow[0].shaper.max_rate = 0;
        flow.sub_flow[0].shaper.max_burst = 0;
        flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
        flow.sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_SINGLE_CLASS_AF1_WFQ;
        flow.sub_flow[0].subflow_properties.hr.weight = 2;

        flow.sub_flow[1].valid_mask = 0;
        flow.sub_flow[1].id = 67;
        flow.sub_flow[1].credit_source = 0;
        flow.sub_flow[1].shaper.max_rate = 0;
        flow.sub_flow[1].shaper.max_burst = 0;
        flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_HR;
        flow.sub_flow[1].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_SINGLE_CLASS_AF1_WFQ;
        flow.sub_flow[1].subflow_properties.hr.weight = 100;

        flow.flow_type = FAP20M_AGGREGATE_SCH_FLOW;
        flow.is_slow_enabled = 0;

        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, source_fap_id, source_queue_id);

		flowGet.id = flow.id;
        /*
            1.2. Call fap20m_get_scheduler_flow with not NULL source_fap_id, source_queue_id, flow.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_idGet, &source_queue_idGet, &flowGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_flow:", device_id, sch_rank);

        /* verifying values */
         UTF_VERIFY_EQUAL1_STRING_MAC(source_fap_id, source_fap_idGet, 
                                     "got another source_fap_id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(source_queue_id, source_queue_idGet, 
                                     "got another source_queue_id was set : %d", device_id); 

        /* verifying values - flow */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.id, flowGet.id, 
                                     "got another flow.id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.flow_type, flowGet.flow_type, 
                                     "got another flow.flow_type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.is_slow_enabled, flowGet.is_slow_enabled, 
                                     "got another flow.is_slow_enabled than was set : %d", device_id);

        /* verifying values - flow.sub_flow[0] */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].valid_mask, flowGet.sub_flow[0].valid_mask, 
                                     "got another flow.sub_flow[0].valid_mask than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].id, flowGet.sub_flow[0].id, 
                                     "got another flow.sub_flow[0].id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].credit_source, flowGet.sub_flow[0].credit_source, 
                                     "got another flow.sub_flow[0].credit_source than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].shaper.max_rate, flowGet.sub_flow[0].shaper.max_rate, 
                                     "got another flow.sub_flow[0].shaper.max_rate than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].shaper.max_burst, flowGet.sub_flow[0].shaper.max_burst, 
                                     "got another flow.sub_flow[0].shaper.max_burst than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].type, flowGet.sub_flow[0].type, 
                                     "got another flow.sub_flow[0].type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].subflow_properties.cl.class_val, flowGet.sub_flow[0].subflow_properties.cl.class_val, 
                                     "got another flow.sub_flow[0].subflow_properties.cl.class_val than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].subflow_properties.cl.weight, flowGet.sub_flow[0].subflow_properties.cl.weight, 
                                     "got another flow.sub_flow[0].subflow_properties.cl.weight than was set : %d", device_id);

        /* verifying values - flow.sub_flow[1] */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].valid_mask, flowGet.sub_flow[1].valid_mask, 
                                     "got another flow.sub_flow[1].valid_mask than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].id, flowGet.sub_flow[1].id, 
                                     "got another flow.sub_flow[1].id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].credit_source, flowGet.sub_flow[1].credit_source, 
                                     "got another flow.sub_flow[1].credit_source than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].shaper.max_rate, flowGet.sub_flow[1].shaper.max_rate, 
                                     "got another flow.sub_flow[1].shaper.max_rate than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].shaper.max_burst, flowGet.sub_flow[1].shaper.max_burst, 
                                     "got another flow.sub_flow[1].shaper.max_burst than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].type, flowGet.sub_flow[1].type, 
                                     "got another flow.sub_flow[1].type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].subflow_properties.cl.class_val, flowGet.sub_flow[1].subflow_properties.cl.class_val, 
                                     "got another flow.sub_flow[1].subflow_properties.cl.class_val than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].subflow_properties.cl.weight, flowGet.sub_flow[1].subflow_properties.cl.weight, 
                                     "got another flow.sub_flow[1].subflow_properties.cl.weight than was set : %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;

        /* if it is an aggregate flow and scheduling is enabled then those values have no meaning */
        source_fap_id = 2047;
        source_queue_id = 8191;

        flow.id = 0x100;
        flow.sub_flow[0].valid_mask = 0xFFFFFF;
        flow.sub_flow[0].id = 0x100;
        flow.sub_flow[0].credit_source = 100;
        flow.sub_flow[0].shaper.max_rate = 100;
        flow.sub_flow[0].shaper.max_burst = 100;
        flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
        flow.sub_flow[0].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP3_WFQ2;
        flow.sub_flow[0].subflow_properties.cl.weight = 1;

        flow.sub_flow[1].valid_mask = 0xFFFFFF;
        flow.sub_flow[1].id = 0x101;
        flow.sub_flow[1].credit_source = 100;
        flow.sub_flow[1].shaper.max_rate = 100;
        flow.sub_flow[1].shaper.max_burst = 100;
        flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_CL;
        flow.sub_flow[1].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP3_WFQ2;
        flow.sub_flow[1].subflow_properties.cl.weight = 1;

		/* the flow type will be cleared in the aggregation process */
        flow.flow_type = FAP20M_AGGREGATE_SCH_FLOW;
        flow.is_slow_enabled = 1;


        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, source_fap_id, source_queue_id);

 		flowGet.id = flow.id;
        /*
            1.2. Call fap20m_get_scheduler_flow with not NULL source_fap_id, source_queue_id, flow.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_idGet, &source_queue_idGet, &flowGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_scheduler_flow:", device_id, sch_rank);

        /* verifying values */
        /*UTF_VERIFY_EQUAL1_STRING_MAC(source_fap_id, source_fap_idGet, 
                                     "got another source_fap_id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(source_queue_id, source_queue_idGet, 
                                     "got another source_queue_id was set : %d", device_id);*/

        /* verifying values - flow */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.id, flowGet.id, 
                                     "got another flow.id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.flow_type, flowGet.flow_type, 
                                     "got another flow.flow_type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.is_slow_enabled, flowGet.is_slow_enabled, 
                                     "got another flow.is_slow_enabled than was set : %d", device_id);

        /* verifying values - flow.sub_flow[0] */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].valid_mask, flowGet.sub_flow[0].valid_mask, 
                                     "got another flow.sub_flow[0].valid_mask than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].id, flowGet.sub_flow[0].id, 
                                     "got another flow.sub_flow[0].id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].credit_source, flowGet.sub_flow[0].credit_source, 
                                     "got another flow.sub_flow[0].credit_source than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].shaper.max_rate, flowGet.sub_flow[0].shaper.max_rate, 
                                     "got another flow.sub_flow[0].shaper.max_rate than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].shaper.max_burst, flowGet.sub_flow[0].shaper.max_burst, 
                                     "got another flow.sub_flow[0].shaper.max_burst than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].type, flowGet.sub_flow[0].type, 
                                     "got another flow.sub_flow[0].type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].subflow_properties.cl.class_val, flowGet.sub_flow[0].subflow_properties.cl.class_val, 
                                     "got another flow.sub_flow[0].subflow_properties.cl.class_val than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[0].subflow_properties.cl.weight, flowGet.sub_flow[0].subflow_properties.cl.weight, 
                                     "got another flow.sub_flow[0].subflow_properties.cl.weight than was set : %d", device_id);

        /* verifying values - flow.sub_flow[1] */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].valid_mask, flowGet.sub_flow[1].valid_mask, 
                                     "got another flow.sub_flow[1].valid_mask than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].id, flowGet.sub_flow[1].id, 
                                     "got another flow.sub_flow[1].id than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].credit_source, flowGet.sub_flow[1].credit_source, 
                                     "got another flow.sub_flow[1].credit_source than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].shaper.max_rate, flowGet.sub_flow[1].shaper.max_rate, 
                                     "got another flow.sub_flow[1].shaper.max_rate than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].shaper.max_burst, flowGet.sub_flow[1].shaper.max_burst, 
                                     "got another flow.sub_flow[1].shaper.max_burst than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].type, flowGet.sub_flow[1].type, 
                                     "got another flow.sub_flow[1].type than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].subflow_properties.cl.class_val, flowGet.sub_flow[1].subflow_properties.cl.class_val, 
                                     "got another flow.sub_flow[1].subflow_properties.cl.class_val than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_flow_given.sub_flow[1].subflow_properties.cl.weight, flowGet.sub_flow[1].subflow_properties.cl.weight, 
                                     "got another flow.sub_flow[1].subflow_properties.cl.weight than was set : %d", device_id);

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with source_fap_id [2048] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        source_fap_id = 2048;

        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, source_fap_id = %d", device_id, source_fap_id);

        source_fap_id = 0;

        /*
            1.5. Call with source_queue_id [12K] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        source_queue_id = 12*1024;

        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, source_queue_id = %d", device_id, source_queue_id);

        source_queue_id = 0;

        /*
            1.6. Call with flow->sub_flow[0].type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow.sub_flow[0].type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, flow.sub_flow[0].type = %d", device_id, flow.sub_flow[0].type);

        flow.sub_flow[0].type = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;

        /*
            1.7. Call with flow->sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_HR], 
                           flow->sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
        flow.sub_flow[0].subflow_properties.hr.sp_class = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, flow.sub_flow[0].type = %d, flow.sub_flow[0].subflow_properties.hr.sp_class = %d", 
                                         device_id, flow.sub_flow[0].type, flow.sub_flow[0].subflow_properties.hr.sp_class);

        flow.sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;

        /*
            1.8. Call with scheduler->type [FAP20M_SUB_FLOW_TYPE_CL], 
                           flow->sub_flow[0].subflow_properties.cl.class_val [32] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
        flow.sub_flow[0].subflow_properties.cl.class_val = 32;

        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, flow.sub_flow[0].type = %d, flow.sub_flow[0].subflow_properties.cl.class_val = %d", 
                                         device_id, flow.sub_flow[0].type, flow.sub_flow[0].subflow_properties.cl.class_val);

        flow.sub_flow[1].subflow_properties.cl.class_val = 0;

        /*
            1.9. Call with flow [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, NULL, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, flow = NULL", device_id);

        /*
            1.10. Call with exact_flow_given [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_flow_given = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    source_fap_id = 0;
    source_queue_id = 0;

    flow.id = 66;
    flow.sub_flow[0].valid_mask = 0;
    flow.sub_flow[0].id = 100;
    flow.sub_flow[0].credit_source = 0;
    flow.sub_flow[0].shaper.max_rate = 0;
    flow.sub_flow[0].shaper.max_burst = 0;
    flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
    flow.sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF1;
    flow.sub_flow[0].subflow_properties.hr.weight = 0;

    flow.sub_flow[1].valid_mask = 0;
    flow.sub_flow[1].id = 66;
    flow.sub_flow[1].credit_source = 0;
    flow.sub_flow[1].shaper.max_rate = 0;
    flow.sub_flow[1].shaper.max_burst = 0;
    flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_HR;
    flow.sub_flow[1].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;
    flow.sub_flow[1].subflow_properties.hr.weight = 100;

    flow.flow_type = FAP20M_REGULAR_SCH_FLOW;
    flow.is_slow_enabled = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_update_scheduler_flow(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_IN     unsigned int                       flow_id,
    SAND_IN     unsigned int                       sub_flow_id,
    SAND_IN     FAP20M_FLOW_UPDATE_TYPE          update_type,
    SAND_IN     unsigned long                      update_value,
    SAND_OUT    unsigned long                      *exact_value
  )
*/
UTF_TEST_CASE_MAC(fap20m_update_scheduler_flow)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   flow_id [0 / 16383],
                   sub_flow_id [0 / 16383],
                   update_type [FAP20M_UPDATE_SHAPER_BURST / FAP20M_UPDATE_SHAPER_RATE],
                   update_value [100],
                   non-null exact_value. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with flow_id [16384] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with sub_flow_id [16384] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with update_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with exact_value [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT flow_id = 0;
    SAND_UINT sub_flow_id = 0;
    FAP20M_FLOW_UPDATE_TYPE update_type = FAP20M_UPDATE_SHAPER_BURST;
    SAND_U32 update_value = 0;
    SAND_U32 exact_value = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           flow_id [0 / 16383],
                           sub_flow_id [0 / 16383],
                           update_type [FAP20M_UPDATE_SHAPER_BURST / FAP20M_UPDATE_SHAPER_RATE],
                           update_value [100],
                           non-null exact_value. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        flow_id = 66;
        sub_flow_id = 66;
        update_type = FAP20M_UPDATE_SHAPER_BURST;
        update_value = 100;

        result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL6_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        flow_id = 2047;
        sub_flow_id = 2047;
        update_type = FAP20M_UPDATE_SHAPER_RATE;
        update_value = 100;

        result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL6_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call with flow_id [16384] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow_id = 16384;

        result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, flow_id = %d", device_id, flow_id);

        flow_id = 0;

        /*
            1.4. Call with sub_flow_id [16384] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sub_flow_id = 16384;

        result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sub_flow_id = %d", device_id, sub_flow_id);

        sub_flow_id = 0;

        /*
            1.5. Call with update_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        update_type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, update_type = %d", device_id, update_type);

        update_type = FAP20M_UPDATE_SHAPER_BURST;

        /*
            1.6. Call with exact_value [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_value = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;
    flow_id = 0;
    sub_flow_id = 0;
    update_type = FAP20M_UPDATE_SHAPER_BURST;
    update_value = 100;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_scheduler_flow(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_OUT    unsigned int                       *source_fap_id,  
    SAND_OUT    unsigned int                       *source_queue_id,
    SAND_INOUT  FAP20M_FLOW                      *flow
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_scheduler_flow)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null source_fap_id, source_queue_id, flow.
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with source_fap_id [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with source_queue_id [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with flow [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT source_fap_id = 0;
    SAND_UINT source_queue_id = 0;
    FAP20M_FLOW flow;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null source_fap_id, source_queue_id, flow.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
		flow.id = 66;

        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, &source_queue_id, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
		flow.id = 2047;

        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, &source_queue_id, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, &source_queue_id, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call with source_fap_id [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scheduler_flow(device_id, sch_rank, NULL, &source_queue_id, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, source_fap_id = NULL", device_id);

        /*
            1.4. Call with source_queue_id [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, NULL, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, source_queue_id = NULL", device_id);

        /*
            1.5. Call with flow [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, &source_queue_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, flow = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, &source_queue_id, &flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, &source_queue_id, &flow);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_is_scheduler_flow_dynamically_used(
    SAND_IN     unsigned int      device_id,
    SAND_IN     FAP20M_SCH_RANK   sch_rank,
    SAND_IN     unsigned int      flow_id,
    SAND_IN     unsigned int      nof_subflows,
    SAND_OUT    unsigned int      *is_dynamically_used
  )
*/
UTF_TEST_CASE_MAC(fap20m_is_scheduler_flow_dynamically_used)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   flow_id [65 / 16383],
                   nof_subflows [0 / 1],
                   non-null is_dynamically_used. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with flow_id [64 / 16384] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with nof_subflows [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with is_dynamically_used [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT flow_id = 0;
    SAND_UINT nof_subflows = 0;
    SAND_UINT is_dynamically_used = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           flow_id [65 / 16383],
                           nof_subflows [0 / 1],
                           non-null is_dynamically_used. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        flow_id = 16383;
        nof_subflows = 0;

        result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, flow_id, nof_subflows);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        flow_id = 65;
        nof_subflows = 1;

        result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, flow_id, nof_subflows);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call with flow_id [8192 / 16384] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow_id = 8192;

        result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, flow_id = %d", device_id, flow_id);

        sch_rank = FAP20M_SCH_RANK_PRIM;
        flow_id = 16384;

        result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, flow_id = %d", device_id, flow_id);

        flow_id = 0;

        /*
            1.4. Call with nof_subflows [2] (don't care) and other params from 1.1.
            Expected: SAND_OK.
        */
        nof_subflows = 2;

        result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, nof_subflows = %d", device_id, nof_subflows);

        nof_subflows = 0;

        /*
            1.5. Call with is_dynamically_used [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, is_dynamically_used = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;
    flow_id = 65;
    nof_subflows = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_del_scheduler_flow(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_IN     unsigned int                       scheduler_flow_id,
    SAND_IN     unsigned int                       do_delay
  )
*/
UTF_TEST_CASE_MAC(fap20m_del_scheduler_flow)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   scheduler_flow_id [0 to 16383],
                   do_delay [TRUE / FALSE]
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with scheduler_flow_id [16384] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT scheduler_flow_id = 0;
    SAND_UINT do_delay = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           scheduler_flow_id [0 / 16383],
                           do_delay [TRUE / FALSE]
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        scheduler_flow_id = 0x500;
        do_delay = TRUE;

        result = fap20m_del_scheduler_flow(device_id, sch_rank, scheduler_flow_id, do_delay);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, scheduler_flow_id, do_delay);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        scheduler_flow_id = 2047;
        do_delay = FALSE;

        result = fap20m_del_scheduler_flow(device_id, sch_rank, scheduler_flow_id, do_delay);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, scheduler_flow_id, do_delay);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_del_scheduler_flow(device_id, sch_rank, scheduler_flow_id, do_delay);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call with scheduler_flow_id [16384] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler_flow_id = 16384;

        result = fap20m_del_scheduler_flow(device_id, sch_rank, scheduler_flow_id, do_delay);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler_flow_id = %d", device_id, scheduler_flow_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;
    scheduler_flow_id = 0;
    do_delay = TRUE;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_del_scheduler_flow(device_id, sch_rank, scheduler_flow_id, do_delay);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_del_scheduler_flow(device_id, sch_rank, scheduler_flow_id, do_delay);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_sch_port_flow_control_set(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_IN     unsigned int                       port,
    SAND_IN     unsigned int                       enable
  )
*/
UTF_TEST_CASE_MAC(fap20m_sch_port_flow_control_set)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   port [0 / 255],
                   enable [0 / 1]
    Expected: SAND_OK.
    1.2. Call fap20m_sch_port_flow_control_get with not NULL enable and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with port [256] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT port = 0;
    SAND_UINT enable = 0;
    SAND_UINT enableGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           port [0 / 255],
                           enable [0 / 1]
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        port = 0;
        enable = 0;

        result = fap20m_sch_port_flow_control_set(device_id, sch_rank, port, enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port, enable);

        /*
            1.2. Call fap20m_sch_port_flow_control_get with not NULL enable and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enableGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_sch_port_flow_control_get: %d, %d, %d", device_id, sch_rank, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        port = 255;
        enable = 1;

        result = fap20m_sch_port_flow_control_set(device_id, sch_rank, port, enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port, enable);

        /*
            1.2. Call fap20m_sch_port_flow_control_get with not NULL enable and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enableGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_sch_port_flow_control_get: %d, %d, %d", device_id, sch_rank, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", device_id);

        /*
            1.3. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_sch_port_flow_control_set(device_id, sch_rank, port, enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with port [256] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 256;

        result = fap20m_sch_port_flow_control_set(device_id, sch_rank, port, enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port = %d", device_id, port);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;
    port = 0;
    enable = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_sch_port_flow_control_set(device_id, sch_rank, port, enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_sch_port_flow_control_set(device_id, sch_rank, port, enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_sch_port_flow_control_get(
    SAND_IN     unsigned int                       device_id,
    SAND_IN     FAP20M_SCH_RANK                    sch_rank,
    SAND_IN     unsigned int                       port,
    SAND_OUT    unsigned int                       *enable
  )
*/
UTF_TEST_CASE_MAC(fap20m_sch_port_flow_control_get)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   port [0 / 255],
                   non-null enable. 
    Expected: SAND_OK.
    1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with port [256] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with enable [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT port = 0;
    SAND_UINT enable = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           port [0 / 255],
                           non-null enable. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        port = 0;

        result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        port = 255;

        result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port);

        /*
            1.2. Call with sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call with port [256] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 256;

        result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port = %d", device_id, port);

        port = 0;

        /*
            1.4. Call with enable [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, enable = NULL", device_id);
    }

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_scheduler suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_scheduler)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_primary_scheduler_role)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_primary_scheduler_role)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_device_rate_entry)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_device_rate_entry)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_slow_max_rate)

#if 0
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_device_scheduler)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_device_interfaces_scheduler)
#endif

    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_slow_max_rate)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_device_interfaces_scheduler)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_device_scheduler)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_HR_scheduler_modes)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_HR_scheduler_modes)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_scheduler_class_type_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_scheduler_class_type_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_scheduler_class_type_params_table)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_scheduler_class_type_params_table)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_scheduler_port)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_scheduler_port)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_is_scheduler_port_dynamically_used)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_scheduler_aggregate)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_scheduler_aggregate)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_scheduler_flow)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_update_scheduler_flow)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_scheduler_flow)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_is_scheduler_flow_dynamically_used)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_del_scheduler_flow)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_sch_port_flow_control_set)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_sch_port_flow_control_get)
UTF_SUIT_END_TESTS_MAC(fap20m_api_scheduler)

