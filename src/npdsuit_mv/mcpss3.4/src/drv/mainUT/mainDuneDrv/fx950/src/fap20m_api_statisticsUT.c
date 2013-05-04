/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_statisticsUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_statistics, that provides
*       statistics API of FAP20M device driver
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_auto_management.h>
#include <FX950/include/fap20m_api_statistics.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_STATISTICS_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_counter(
    unsigned int         device_id,
    FAP20M_COUNTER_TYPE  counter_type,
    SAND_64CNT*          result_ptr
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_counter)
{
/*
    ITERATE_DEVICES
    1.1. Call with counter_type [FAP20M_EN_Q_PKT_CNT / FAP20M_HD_DEL_PKT_CNT / FAP20M_PRG_FLOW_STS_MSG_CNT]
                   and not NULL result_ptr.
    Expected: SAND_OK.
    1.2. Call with counter_type [0x5AAAAAA5]
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with result_ptr [NULL]
                   and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT           device_id = 0;
    FAP20M_COUNTER_TYPE counter_type = FAP20M_EN_Q_PKT_CNT;
    SAND_64CNT          result_ptr;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with counter_type [FAP20M_EN_Q_PKT_CNT / FAP20M_HD_DEL_PKT_CNT / FAP20M_PRG_FLOW_STS_MSG_CNT]
                           and not NULL result_ptr.
            Expected: SAND_OK.
        */
        /* iterate with counter_type = FAP20M_EN_Q_PKT_CNT */
        counter_type = FAP20M_EN_Q_PKT_CNT;

        result = fap20m_get_counter(device_id, counter_type, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, counter_type);

        /* iterate with counter_type = FAP20M_HD_DEL_PKT_CNT */
        counter_type = FAP20M_HD_DEL_PKT_CNT;

        result = fap20m_get_counter(device_id, counter_type, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, counter_type);

        /* iterate with counter_type = FAP20M_PRG_FLOW_STS_MSG_CNT */
        counter_type = FAP20M_PRG_FLOW_STS_MSG_CNT;

        result = fap20m_get_counter(device_id, counter_type, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, counter_type);

        /*
            1.2. Call with counter_type [0x5AAAAAA5]
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        counter_type = 0x5AAAAAA5;

        result = fap20m_get_counter(device_id, counter_type, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, counter_type);

        counter_type = FAP20M_EN_Q_PKT_CNT;

        /*
            1.3. Call with result_ptr [NULL]
                           and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_counter(device_id, counter_type, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, result_ptr = NULL", device_id);
    }

    counter_type = FAP20M_EN_Q_PKT_CNT;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_counter(device_id, counter_type, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_counter(device_id, counter_type, &result_ptr);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_all_counters(
    unsigned int                   device_id,
    FAP20M_ALL_STATISTIC_COUNTERS* all_statistic_counters
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_all_counters)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL all_statistic_counters.
    Expected: SAND_OK.
    1.2. Call with all_statistic_counters [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT           device_id = 0;
    FAP20M_ALL_STATISTIC_COUNTERS all_statistic_counters;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL all_statistic_counters.
            Expected: SAND_OK.
        */
        result = fap20m_get_all_counters(device_id, &all_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with all_statistic_counters [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_all_counters(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_statistic_counters = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_all_counters(device_id, &all_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_all_counters(device_id, &all_statistic_counters);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_collect_pkt_counters(
    unsigned long device_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_collect_pkt_counters)
{
/*
    ITERATE_DEVICES
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        result = fap20m_collect_pkt_counters(device_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_collect_pkt_counters(device_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_collect_pkt_counters(device_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long 
  fap20m_set_programmable_queue(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int programmable_queue,
    SAND_IN unsigned int programmable_queue_mask
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_programmable_queue)
{
/*
    ITERATE_DEVICES
    1.1. Call with programmable_queue [0 / 12K - 1]
                   and programmable_queue_mask [0 / 12K - 1].
    Expected: SAND_OK.
    1.2. Call with programmable_queue [12K] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with programmable_queue_mask [12K] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_UINT programmable_queue = 0;
    SAND_UINT programmable_queue_mask = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with programmable_queue [0 / 12K - 1]
                           and programmable_queue_mask [0 / 12K - 1].
            Expected: SAND_OK.
        */
        /* iterate with programmable_queue = 0 */
        programmable_queue = 0;
        programmable_queue_mask = 0;

        result = fap20m_set_programmable_queue(device_id, programmable_queue, programmable_queue_mask);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, programmable_queue, programmable_queue_mask);

        /* iterate with programmable_queue = 12K - 1 */
        programmable_queue = 0x2FFF;
        programmable_queue_mask = 0x2FFF;

        result = fap20m_set_programmable_queue(device_id, programmable_queue, programmable_queue_mask);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, programmable_queue, programmable_queue_mask);

        /*
            1.2. Call with programmable_queue [12K] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        programmable_queue = 0x3000;
        
        result = fap20m_set_programmable_queue(device_id, programmable_queue, programmable_queue_mask);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, programmable_queue);

        programmable_queue = 0;

        /*
            1.3. Call with programmable_queue_mask [12K] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        programmable_queue_mask = 0x3000;
        
        result = fap20m_set_programmable_queue(device_id, programmable_queue, programmable_queue_mask);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, programmable_queue_mask = %d",
                                     device_id, programmable_queue_mask);
    }

    programmable_queue = 0;
    programmable_queue_mask = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_programmable_queue(device_id, programmable_queue, programmable_queue_mask);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_programmable_queue(device_id, programmable_queue, programmable_queue_mask);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_statistics suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_statistics)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_counter)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_all_counters)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_collect_pkt_counters)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_programmable_queue)
UTF_SUIT_END_TESTS_MAC(fap20m_api_statistics)

