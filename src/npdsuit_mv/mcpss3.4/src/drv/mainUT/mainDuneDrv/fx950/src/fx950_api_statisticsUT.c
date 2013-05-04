/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_statisticsUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_statistics, that provides
*       statistics API of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_statistics.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_API_STATISTICS_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_get_global_counter(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     FX950_STATISTICS_GLOBAL_COUNTER_TYPE    global_counter_type,
    SAND_OUT    SAND_64CNT*                             result_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_get_global_counter)
{
/*
    ITERATE_DEVICES
    1.1. Call with global_counter_type [FX950_STATISTICS_GLOBAL_QUEUE_NOT_VALID_COUNTER / 
                                        FX950_STATISTICS_GLOBAL_IN_CHOP_COUNTER,
                                        FX950_STATISTICS_GLOBAL_OUT_CHOP_COUNTER,
                                        FX950_STATISTICS_GLOBAL_EMPTY_MC_MAPPING_COUNTER,
                                        FX950_STATISTICS_GLOBAL_NOF_COUNTER_TYPES],
                   not NULL result_ptr.
    Expected: SAND_OK.
    1.2. Call with global_counter_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with result_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_STATISTICS_GLOBAL_COUNTER_TYPE global_counter_type = FX950_STATISTICS_GLOBAL_QUEUE_NOT_VALID_COUNTER;
    SAND_64CNT res;

    
    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with global_counter_type [FX950_STATISTICS_GLOBAL_QUEUE_NOT_VALID_COUNTER / 
                                                FX950_STATISTICS_GLOBAL_IN_CHOP_COUNTER,
                                                FX950_STATISTICS_GLOBAL_OUT_CHOP_COUNTER,
                                                FX950_STATISTICS_GLOBAL_EMPTY_MC_MAPPING_COUNTER,
                                                FX950_STATISTICS_GLOBAL_NOF_COUNTER_TYPES],
                           not NULL result_ptr.
            Expected: SAND_OK.
        */
        /* iterate with FX950_STATISTICS_GLOBAL_QUEUE_NOT_VALID_COUNTER */
        global_counter_type = FX950_STATISTICS_GLOBAL_QUEUE_NOT_VALID_COUNTER;

        result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, global_counter_type);

        /* iterate with FX950_STATISTICS_GLOBAL_IN_CHOP_COUNTER */
        global_counter_type = FX950_STATISTICS_GLOBAL_IN_CHOP_COUNTER;

        result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, global_counter_type);

        /* iterate with FX950_STATISTICS_GLOBAL_OUT_CHOP_COUNTER */
        global_counter_type = FX950_STATISTICS_GLOBAL_OUT_CHOP_COUNTER;

        result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, global_counter_type);

        /* iterate with FX950_STATISTICS_GLOBAL_EMPTY_MC_MAPPING_COUNTER */
        /* This is an internal counter 
        global_counter_type = FX950_STATISTICS_GLOBAL_EMPTY_MC_MAPPING_COUNTER;

        result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, global_counter_type);
        */

        /* iterate with FX950_STATISTICS_GLOBAL_NOF_COUNTER_TYPES */
        global_counter_type = FX950_STATISTICS_GLOBAL_NOF_COUNTER_TYPES;

        result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, global_counter_type);

        /*
            1.2. Call with global_counter_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        global_counter_type = SAND_UTF_API_STATISTICS_INVALID_ENUM;

        result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, global_counter_type);

        global_counter_type = FX950_STATISTICS_GLOBAL_QUEUE_NOT_VALID_COUNTER;

        /*
            1.3. Call with result_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, result = NULL", device_id);
    }

    global_counter_type = FX950_STATISTICS_GLOBAL_QUEUE_NOT_VALID_COUNTER;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &res);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &res);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_get_hgl_counter(
    SAND_IN     unsigned int                        device_id,
    SAND_IN     FX950_STATISTICS_HGL_COUNTER_TYPE    hgl_counter_type,
    SAND_IN     unsigned long                       link_number,
    SAND_OUT    SAND_64CNT*                         result_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_get_hgl_counter)
{
/*
    ITERATE_DEVICES
    1.1. Call with hgl_counter_type [FX950_STATISTICS_HGL_CRC_ERROR_COUNTER / 
                                     FX950_STATISTICS_HGL_PACKET_ERROR_COUNTER], 
                   link_number [0 / 1], 
                   non-null result_ptr.
    Expected: SAND_OK.
    1.2. Call with hgl_counter_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with link_number [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with result_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_STATISTICS_HGL_COUNTER_TYPE hgl_counter_type = FX950_STATISTICS_HGL_BAD_HEADER_RX_COUNTER;
    SAND_U32   link_number = 0;
    SAND_64CNT res;

    
    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with hgl_counter_type [FX950_STATISTICS_HGL_CRC_ERROR_COUNTER / 
                                             FX950_STATISTICS_HGL_PACKET_ERROR_COUNTER], 
                           link_number [0 / 1], 
                           non-null result_ptr.
            Expected: SAND_OK.
        */
        /* iterate with FX950_STATISTICS_HGL_CRC_ERROR_COUNTER */
        hgl_counter_type = FX950_STATISTICS_HGL_CRC_ERROR_COUNTER;
        link_number = 0;

        result = fx950_api_statistics_get_hgl_counter(device_id, hgl_counter_type, link_number, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, hgl_counter_type, link_number);

        /* iterate with FX950_STATISTICS_HGL_PACKET_ERROR_COUNTER */
        hgl_counter_type = FX950_STATISTICS_HGL_PACKET_ERROR_COUNTER;
        link_number = 1;

        result = fx950_api_statistics_get_hgl_counter(device_id, hgl_counter_type, link_number, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, hgl_counter_type, link_number);

        /*
            1.2. Call with hgl_counter_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        hgl_counter_type = SAND_UTF_API_STATISTICS_INVALID_ENUM;

        result = fx950_api_statistics_get_hgl_counter(device_id, hgl_counter_type, link_number, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, hgl_counter_type);

        hgl_counter_type = FX950_STATISTICS_HGL_CRC_ERROR_COUNTER;

        /*
            1.3. Call with link_number [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_number = 2;

        result = fx950_api_statistics_get_hgl_counter(device_id, hgl_counter_type, link_number, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link_number = %d", device_id, link_number);

        link_number = 0;

        /*
            1.4. Call with result_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_get_hgl_counter(device_id, hgl_counter_type, link_number, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, result = NULL", device_id);
    }

    hgl_counter_type = FX950_STATISTICS_HGL_CRC_ERROR_COUNTER;
    link_number = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_get_hgl_counter(device_id, hgl_counter_type, link_number, &res);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_get_hgl_counter(device_id, hgl_counter_type, link_number, &res);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_get_all_global_counters(
    SAND_IN  unsigned int                             device_id,
    SAND_OUT FX950_STATISTICS_ALL_GLOBAL_COUNTERS*    all_global_statistic_counters_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_get_all_global_counters)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null all_global_statistic_counters_ptr.
    Expected: SAND_OK.
    1.2. Call with all_global_statistic_counters_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_STATISTICS_ALL_GLOBAL_COUNTERS all_global_statistic_counters;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null all_global_statistic_counters_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_statistics_get_all_global_counters(device_id, &all_global_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with all_global_statistic_counters_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_get_all_global_counters(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_global_statistic_counters_ptr = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_get_all_global_counters(device_id, &all_global_statistic_counters);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_get_all_global_counters(device_id, &all_global_statistic_counters);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_get_all_hgl_counters(
    SAND_IN  unsigned int                       device_id,
    SAND_IN  unsigned long                      link_number,
    SAND_OUT FX950_STATISTICS_ALL_HGL_COUNTERS* all_hgl_statistic_counters_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_get_all_hgl_counters)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_number [0 / 1], non-null all_hgl_statistic_counters_ptr.
    Expected: SAND_OK.
    1.2. Call with link_number [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with all_hgl_statistic_counters_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link_number = 0;
    FX950_STATISTICS_ALL_HGL_COUNTERS all_hgl_statistic_counters;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_number [0 / 1], non-null all_hgl_statistic_counters_ptr.
            Expected: SAND_OK.
        */
        /* iterate with link_number = 0 */
        link_number = 0;

        result = fx950_api_statistics_get_all_hgl_counters(device_id, link_number, &all_hgl_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        /* iterate with link_number = 1 */
        link_number = 1;

        result = fx950_api_statistics_get_all_hgl_counters(device_id, link_number, &all_hgl_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        /*
            1.2. Call with link_number [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_number = 2;

        result = fx950_api_statistics_get_all_hgl_counters(device_id, link_number, &all_hgl_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link_number = %d", device_id, link_number);

        link_number = 0;

        /*
            1.3. Call with all_hgl_statistic_counters_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_get_all_hgl_counters(device_id, link_number, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_hgl_statistic_counters_ptr = NULL", device_id);
    }

    link_number = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_get_all_hgl_counters(device_id, link_number, &all_hgl_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_get_all_hgl_counters(device_id, link_number, &all_hgl_statistic_counters);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_get_hgs_port_mac_mib_counter(
    SAND_IN     unsigned int                        device_id,
    SAND_IN     FX950_STATISTICS_HGS_COUNTER_TYPE   hgs_counter_type,
    SAND_IN     unsigned long                       port,
    SAND_IN     unsigned int                        read_from_capture_area,
    SAND_OUT    SAND_64CNT*                         result_ptr
  )
*/
#if 0 /* existing appDemo does not supports HGS ports and all related API */
UTF_TEST_CASE_MAC(fx950_api_statistics_get_hgs_port_mac_mib_counter)
{
/*
    ITERATE_DEVICES
    1.1. Call with hgs_counter_type [FX950_STATISTICS_HGS_GOOD_OCTETS_RECEIVED / 
                                     FX950_STATISTICS_HGS_MULTICAST_FRAMES_RECEIVED],
                   port [0 / 1],
                   read_from_capture_area [TRUE / FALSE],
                   non-null result_ptr.
    Expected: SAND_OK.
    1.2. Call with hgs_counter_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with result_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_STATISTICS_HGS_COUNTER_TYPE hgs_counter_type = FX950_STATISTICS_HGS_BAD_CRC;
    SAND_U32   port = 0;
    SAND_UINT  read_from_capture_area = 0;
    SAND_64CNT res;

    
    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with hgs_counter_type [FX950_STATISTICS_HGS_GOOD_OCTETS_RECEIVED / 
                                             FX950_STATISTICS_HGS_MULTICAST_FRAMES_RECEIVED],
                           port [0 / 1],
                           read_from_capture_area [TRUE / FALSE],
                           non-null result_ptr.
            Expected: SAND_OK.
        */
        /* iterate with FX950_STATISTICS_HGS_GOOD_OCTETS_RECEIVED */
        hgs_counter_type = FX950_STATISTICS_HGS_GOOD_OCTETS_RECEIVED;
        port = 0;
        read_from_capture_area = TRUE;

        result = fx950_api_statistics_get_hgs_port_mac_mib_counter(device_id, hgs_counter_type, port, read_from_capture_area, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, hgs_counter_type, port, read_from_capture_area);

        /* iterate with FX950_STATISTICS_HGS_MULTICAST_FRAMES_RECEIVED */
        hgs_counter_type = FX950_STATISTICS_HGS_MULTICAST_FRAMES_RECEIVED;
        port = 1;
        read_from_capture_area = FALSE;

        result = fx950_api_statistics_get_hgs_port_mac_mib_counter(device_id, hgs_counter_type, port, read_from_capture_area, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, hgs_counter_type, port, read_from_capture_area);

        /*
            1.2. Call with hgs_counter_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        hgs_counter_type = SAND_UTF_API_STATISTICS_INVALID_ENUM;

        result = fx950_api_statistics_get_hgs_port_mac_mib_counter(device_id, hgs_counter_type, port, read_from_capture_area, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, hgs_counter_type);

        hgs_counter_type = FX950_STATISTICS_HGS_GOOD_OCTETS_RECEIVED;

        /*
            1.3. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_statistics_get_hgs_port_mac_mib_counter(device_id, hgs_counter_type, port, read_from_capture_area, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port = %d", device_id, port);

        port = 0;

        /*
            1.4. Call with result_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_get_hgs_port_mac_mib_counter(device_id, hgs_counter_type, port, read_from_capture_area, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, result_ptr = NULL", device_id);
    }

    hgs_counter_type = FX950_STATISTICS_HGS_GOOD_OCTETS_RECEIVED;
    port = 0;
    read_from_capture_area = TRUE;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_get_hgs_port_mac_mib_counter(device_id, hgs_counter_type, port, read_from_capture_area, &res);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_get_hgs_port_mac_mib_counter(device_id, hgs_counter_type, port, read_from_capture_area, &res);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_get_all_hgs_counters(
    SAND_IN  unsigned int                       device_id,
    SAND_IN  unsigned long                      port,
    SAND_IN  unsigned int                       read_from_capture_area,
    SAND_OUT FX950_STATISTICS_ALL_HGS_COUNTERS* all_hgs_statistic_counters_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_get_all_hgs_counters)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1],
                   read_from_capture_area [TRUE / FALSE],
                   non-null all_hgs_statistic_counters_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with all_hgs_statistic_counters_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_UINT  read_from_capture_area = 0;
    FX950_STATISTICS_ALL_HGS_COUNTERS all_hgs_statistic_counters;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1],
                           read_from_capture_area [TRUE / FALSE],
                           non-null all_hgs_statistic_counters_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        read_from_capture_area = TRUE;

        result = fx950_api_statistics_get_all_hgs_counters(device_id, port, read_from_capture_area, &all_hgs_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, read_from_capture_area);

        /* iterate with port = 1 */
        port = 1;
        read_from_capture_area = FALSE;

        result = fx950_api_statistics_get_all_hgs_counters(device_id, port, read_from_capture_area, &all_hgs_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, read_from_capture_area);

        /*
            1.2. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_statistics_get_all_hgs_counters(device_id, port, read_from_capture_area, &all_hgs_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with all_hgs_statistic_counters_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_get_all_hgs_counters(device_id, port, read_from_capture_area, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_hgs_statistic_counters = NULL", device_id);
    }

    port = 0;
    read_from_capture_area = TRUE;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_get_all_hgs_counters(device_id, port, read_from_capture_area, &all_hgs_statistic_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_get_all_hgs_counters(device_id, port, read_from_capture_area, &all_hgs_statistic_counters);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_set_port_mac_mib_counter_cfg(
    SAND_IN     unsigned int                                              device_id,
    SAND_IN     unsigned long                                             port,
    SAND_IN     FX950_STATISTICS_HGS_INTERFACE_PORT_MAC_MIB_COUNTER_CFG   *mac_mib_counter_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_set_port_mac_mib_counter_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], 
                   mac_mib_counter_cfg_ptr { mib_cnt_disable [TRUE / FALSE],
                                             rx_histogram_enable [TRUE / FALSE],
                                             tx_histogram_enable [TRUE / FALSE],
                                             mib_clear_on_read_enable [TRUE / FALSE]. }
    Expected: SAND_OK.
    1.2. Call fx950_api_statistics_get_port_mac_mib_counter_cfg with not NULL mac_mib_counter_cfg_ptr
              and port from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with mac_mib_counter_cfg_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    FX950_STATISTICS_HGS_INTERFACE_PORT_MAC_MIB_COUNTER_CFG mac_mib_counter_cfg;
    FX950_STATISTICS_HGS_INTERFACE_PORT_MAC_MIB_COUNTER_CFG mac_mib_counter_cfgGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], 
                           mac_mib_counter_cfg_ptr { mib_cnt_disable [TRUE / FALSE],
                                                     rx_histogram_enable [TRUE / FALSE],
                                                     tx_histogram_enable [TRUE / FALSE],
                                                     mib_clear_on_read_enable [TRUE / FALSE]. }
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        mac_mib_counter_cfg.mib_cnt_disable = TRUE;
        mac_mib_counter_cfg.rx_histogram_enable = TRUE;
        mac_mib_counter_cfg.tx_histogram_enable = TRUE;
        mac_mib_counter_cfg.mib_clear_on_read_enable = TRUE;

        result = fx950_api_statistics_set_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_statistics_get_port_mac_mib_counter_cfg with not NULL mac_mib_counter_cfg_ptr
                      and port from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfgGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_statistics_get_port_mac_mib_counter_cfg: %d, %d", device_id, port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mac_mib_counter_cfg.mib_cnt_disable, mac_mib_counter_cfgGet.mib_cnt_disable, 
                                     "got another mac_mib_counter_cfg.mib_cnt_disable than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mac_mib_counter_cfg.rx_histogram_enable, mac_mib_counter_cfgGet.rx_histogram_enable, 
                                     "got another mac_mib_counter_cfg.rx_histogram_enable than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mac_mib_counter_cfg.tx_histogram_enable, mac_mib_counter_cfgGet.tx_histogram_enable, 
                                     "got another mac_mib_counter_cfg.tx_histogram_enable than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mac_mib_counter_cfg.mib_clear_on_read_enable, mac_mib_counter_cfgGet.mib_clear_on_read_enable, 
                                     "got another mac_mib_counter_cfg.mib_clear_on_read_enable than was set : %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        mac_mib_counter_cfg.mib_cnt_disable = FALSE;
        mac_mib_counter_cfg.rx_histogram_enable = FALSE;
        mac_mib_counter_cfg.tx_histogram_enable = FALSE;
        mac_mib_counter_cfg.mib_clear_on_read_enable = FALSE;

        result = fx950_api_statistics_set_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_statistics_get_port_mac_mib_counter_cfg with not NULL mac_mib_counter_cfg_ptr
                      and port from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfgGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_statistics_get_port_mac_mib_counter_cfg: %d, %d", device_id, port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mac_mib_counter_cfg.mib_cnt_disable, mac_mib_counter_cfgGet.mib_cnt_disable, 
                                     "got another mac_mib_counter_cfg.mib_cnt_disable than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mac_mib_counter_cfg.rx_histogram_enable, mac_mib_counter_cfgGet.rx_histogram_enable, 
                                     "got another mac_mib_counter_cfg.rx_histogram_enable than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mac_mib_counter_cfg.tx_histogram_enable, mac_mib_counter_cfgGet.tx_histogram_enable, 
                                     "got another mac_mib_counter_cfg.tx_histogram_enable than was set : %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mac_mib_counter_cfg.mib_clear_on_read_enable, mac_mib_counter_cfgGet.mib_clear_on_read_enable, 
                                     "got another mac_mib_counter_cfg.mib_clear_on_read_enable than was set : %d", device_id);

        /*
            1.3. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_statistics_set_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with mac_mib_counter_cfg_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_set_port_mac_mib_counter_cfg(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mac_mib_counter_cfg = NULL", device_id);
    }

    port = 0;
    mac_mib_counter_cfg.mib_cnt_disable = TRUE;
    mac_mib_counter_cfg.rx_histogram_enable = TRUE;
    mac_mib_counter_cfg.tx_histogram_enable = TRUE;
    mac_mib_counter_cfg.mib_clear_on_read_enable = TRUE;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_set_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_set_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_get_port_mac_mib_counter_cfg(
    SAND_IN     unsigned int                                             device_id,
    SAND_IN     unsigned long                                            port,
    SAND_OUT    FX950_STATISTICS_HGS_INTERFACE_PORT_MAC_MIB_COUNTER_CFG  *mac_mib_counter_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_get_port_mac_mib_counter_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], non-null mac_mib_counter_cfg_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with mac_mib_counter_cfg_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    FX950_STATISTICS_HGS_INTERFACE_PORT_MAC_MIB_COUNTER_CFG mac_mib_counter_cfg;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], non-null mac_mib_counter_cfg_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with mac_mib_counter_cfg_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mac_mib_counter_cfg = NULL", device_id);
    }

    port = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_set_port_mac_mib_copy_trigger(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_set_port_mac_mib_copy_trigger)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1].
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_statistics_set_port_mac_mib_copy_trigger(device_id, port);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 0 */
        port = 1;

        result = fx950_api_statistics_set_port_mac_mib_copy_trigger(device_id, port);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range).
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_statistics_set_port_mac_mib_copy_trigger(device_id, port);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);
    }

    port = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_set_port_mac_mib_copy_trigger(device_id, port);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_set_port_mac_mib_copy_trigger(device_id, port);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_get_port_mac_mib_copy_status(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_OUT    unsigned int    *copy_done_status_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_get_port_mac_mib_copy_status)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1],  non-null copy_done_status_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range).
    Expected: NOT SAND_OK.
    1.3. Call with copy_done_status_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_UINT  copy_done_status = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1],  non-null copy_done_status_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_statistics_get_port_mac_mib_copy_status(device_id, port, &copy_done_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 0 */
        port = 1;

        result = fx950_api_statistics_get_port_mac_mib_copy_status(device_id, port, &copy_done_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range).
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_statistics_get_port_mac_mib_copy_status(device_id, port, &copy_done_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with copy_done_status_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_get_port_mac_mib_copy_status(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, copy_done_status = NULL", device_id);
    }

    port = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_get_port_mac_mib_copy_status(device_id, port, &copy_done_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_get_port_mac_mib_copy_status(device_id, port, &copy_done_status);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_statistics suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_statistics)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_get_global_counter)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_get_hgl_counter)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_get_all_global_counters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_get_all_hgl_counters)
#if 0 /* existing appDemo does not supports HGS ports and all related API */
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_get_hgs_port_mac_mib_counter)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_get_all_hgs_counters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_set_port_mac_mib_counter_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_get_port_mac_mib_counter_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_set_port_mac_mib_copy_trigger)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_get_port_mac_mib_copy_status)
#endif
UTF_SUIT_END_TESTS_MAC(fx950_api_statistics)
