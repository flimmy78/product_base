/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_tail_dropUT.
*
* DESCRIPTION:
*       Unit tests for fx950_api_tail_drop, that provides
*       egress congestion avoidance API of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_tail_drop.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_tail_drop_set_packet_thresholds(
    SAND_IN     unsigned int       device_id,
    SAND_IN     unsigned long      packet_share_threshold,
    SAND_IN     unsigned long      packet_total_max_limit
  )
*/
UTF_TEST_CASE_MAC(fx950_api_tail_drop_set_packet_thresholds)
{
/*
    ITERATE_DEVICES
    1.1. Call with packet_share_threshold [0 / 1530], packet_total_max_limit[1 / 1530].
    Expected: SAND_OK.
    1.2. Call fx950_api_tail_drop_get_packet_thresholds with not NULL pointers and params from 1.1.
    Expected: SAND_OK and the same packet_share_threshold, packet_total_max_limit as was set.
    1.3. Call with packet_share_threshold [1531] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with packet_total_max_limit [0 / 1531] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  packet_share_threshold = 0;
    SAND_U32  packet_total_max_limit = 0;
    SAND_U32  packet_share_thresholdGet = 0;
    SAND_U32  packet_total_max_limitGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with packet_share_threshold [0 / 1530], packet_total_max_limit[1 / 1530].
            Expected: SAND_OK.
        */
        /* iterate with packet_share_threshold = 0 */
        packet_share_threshold = 0;
        packet_total_max_limit = 1;

        result = fx950_api_tail_drop_set_packet_thresholds(device_id, packet_share_threshold, packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, packet_share_threshold, packet_total_max_limit);

        /*
            1.2. Call fx950_api_tail_drop_get_packet_thresholds with not NULL pointers and params from 1.1.
            Expected: SAND_OK and the same packet_share_threshold, packet_total_max_limit as was set.
        */
        result = fx950_api_tail_drop_get_packet_thresholds(device_id, &packet_share_thresholdGet, &packet_total_max_limitGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_tail_drop_get_packet_thresholds: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(packet_share_threshold, packet_share_thresholdGet,
                   "got another packet_share_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(packet_total_max_limit, packet_total_max_limitGet,
                   "got another packet_total_max_limit than was set: %d", device_id);

        /* iterate with packet_share_threshold = 1530 */
        packet_share_threshold = 1530;
        packet_total_max_limit = 1530;

        result = fx950_api_tail_drop_set_packet_thresholds(device_id, packet_share_threshold, packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, packet_share_threshold, packet_total_max_limit);

        /*
            1.2. Call fx950_api_tail_drop_get_packet_thresholds with not NULL pointers and params from 1.1.
            Expected: SAND_OK and the same packet_share_threshold, packet_total_max_limit as was set.
        */
        result = fx950_api_tail_drop_get_packet_thresholds(device_id, &packet_share_thresholdGet, &packet_total_max_limitGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_tail_drop_get_packet_thresholds: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(packet_share_threshold, packet_share_thresholdGet,
                   "got another packet_share_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(packet_total_max_limit, packet_total_max_limitGet,
                   "got another packet_total_max_limit than was set: %d", device_id);

        /*
            1.3. Call with packet_share_threshold [1531] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        packet_share_threshold = 1531;

        result = fx950_api_tail_drop_set_packet_thresholds(device_id, packet_share_threshold, packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_share_threshold);

        packet_share_threshold = 0;

        /*
            1.4. Call with packet_total_max_limit [0 / 1531] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        /* iterate with packet_total_max_limit = 0 */
        packet_total_max_limit = 0;

        result = fx950_api_tail_drop_set_packet_thresholds(device_id, packet_share_threshold, packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, packet_total_max_limit = %d", device_id, packet_total_max_limit);

        /* iterate with packet_total_max_limit = 1531 */
        packet_total_max_limit = 1531;

        result = fx950_api_tail_drop_set_packet_thresholds(device_id, packet_share_threshold, packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, packet_total_max_limit = %d", device_id, packet_total_max_limit);
    }

    packet_share_threshold = 0;
    packet_total_max_limit = 1;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_tail_drop_set_packet_thresholds(device_id, packet_share_threshold, packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_tail_drop_set_packet_thresholds(device_id, packet_share_threshold, packet_total_max_limit);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_tail_drop_get_packet_thresholds(
    SAND_IN     unsigned int       device_id,
    SAND_OUT    unsigned long      *packet_share_threshold_ptr,
    SAND_OUT    unsigned long      *packet_total_max_limit_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_tail_drop_get_packet_thresholds)
{
/*
    ITERATE_DEVICES
    1.1. Call with non NULL packet_share_threshold_ptr
                   and non NULL packet_total_max_limit_ptr.
    Expected: SAND_OK.
    1.2. Call with packet_share_threshold_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with packet_total_max_limit_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  packet_share_threshold = 0;
    SAND_U32  packet_total_max_limit = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non NULL packet_share_threshold_ptr
                           and non NULL packet_total_max_limit_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_tail_drop_get_packet_thresholds(device_id, &packet_share_threshold, &packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with packet_share_threshold_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_packet_thresholds(device_id, NULL, &packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, packet_share_threshold_ptr = NULL", device_id);

        /*
            1.3. Call with packet_total_max_limit_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_packet_thresholds(device_id, &packet_share_threshold, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, packet_total_max_limit_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_tail_drop_get_packet_thresholds(device_id, &packet_share_threshold, &packet_total_max_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_tail_drop_get_packet_thresholds(device_id, &packet_share_threshold, &packet_total_max_limit);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_tail_drop_set_total_buffer_threshold(
    SAND_IN     unsigned int       device_id,
    SAND_IN     unsigned long      buffer_total_threshold
  )
*/
UTF_TEST_CASE_MAC(fx950_api_tail_drop_set_total_buffer_threshold)
{
/*
    ITERATE_DEVICES
    1.1. Call with buffer_total_threshold [1 / 1446].
    Expected: SAND_OK.
    1.2. Call fx950_api_tail_drop_get_total_buffer_threshold with not NULL pointers and params from 1.1.
    Expected: SAND_OK and the same buffer_total_threshold as was set.
    1.3. Call with buffer_total_threshold [0 / 1447] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  buffer_total_threshold = 0;
    SAND_U32  buffer_total_thresholdGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with buffer_total_threshold [1 / 1446].
            Expected: SAND_OK.
        */
        /* iterate with buffer_total_threshold = 1 */
        buffer_total_threshold = 1;

        result = fx950_api_tail_drop_set_total_buffer_threshold(device_id, buffer_total_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, buffer_total_threshold);

        /*
            1.2. Call fx950_api_tail_drop_get_total_buffer_threshold with not NULL pointers and params from 1.1.
            Expected: SAND_OK and the same buffer_total_threshold as was set.
        */
        result = fx950_api_tail_drop_get_total_buffer_threshold(device_id, &buffer_total_thresholdGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_tail_drop_get_total_buffer_threshold: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(buffer_total_threshold, buffer_total_thresholdGet,
                   "got another buffer_total_threshold than was set: %d", device_id);
        
        /* iterate with buffer_total_threshold = 1446 */
        buffer_total_threshold = 1446;

        result = fx950_api_tail_drop_set_total_buffer_threshold(device_id, buffer_total_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, buffer_total_threshold);

        /*
            1.2. Call fx950_api_tail_drop_get_total_buffer_threshold with not NULL pointers and params from 1.1.
            Expected: SAND_OK and the same buffer_total_threshold as was set.
        */
        result = fx950_api_tail_drop_get_total_buffer_threshold(device_id, &buffer_total_thresholdGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_tail_drop_get_total_buffer_threshold: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(buffer_total_threshold, buffer_total_thresholdGet,
                   "got another buffer_total_threshold than was set: %d", device_id);

        /*
            1.3. Call with buffer_total_threshold [0 / 1447] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        /* iterate with buffer_total_threshold = 0 */
        buffer_total_threshold = 0;

        result = fx950_api_tail_drop_set_total_buffer_threshold(device_id, buffer_total_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, buffer_total_threshold);

        /* iterate with buffer_total_threshold = 1447 */
        buffer_total_threshold = 1447;

        result = fx950_api_tail_drop_set_total_buffer_threshold(device_id, buffer_total_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, buffer_total_threshold);
    }

    buffer_total_threshold = 1;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_tail_drop_set_total_buffer_threshold(device_id, buffer_total_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_tail_drop_set_total_buffer_threshold(device_id, buffer_total_threshold);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_tail_drop_get_total_buffer_threshold(
    SAND_IN     unsigned int       device_id,
    SAND_OUT    unsigned long      *buffer_total_threshold_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_tail_drop_get_total_buffer_threshold)
{
/*
    ITERATE_DEVICES
    1.1. Call with non NULL packet_share_threshold_ptr
                   and non NULL packet_total_max_limit_ptr.
    Expected: SAND_OK.
    1.2. Call with packet_share_threshold_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with packet_total_max_limit_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  buffer_total_threshold = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non NULL packet_share_threshold_ptr
                           and non NULL packet_total_max_limit_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_tail_drop_get_total_buffer_threshold(device_id, &buffer_total_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with packet_share_threshold_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_total_buffer_threshold(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, buffer_total_threshold_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_tail_drop_get_total_buffer_threshold(device_id, &buffer_total_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_tail_drop_get_total_buffer_threshold(device_id, &buffer_total_threshold);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_tail_drop_set_queue_thresholds(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       link_num,
    SAND_IN     unsigned long       uc_queue_0_threshold,
    SAND_IN     unsigned long       uc_queue_1_threshold,
    SAND_IN     unsigned long       mc_queue_0_threshold,
    SAND_IN     unsigned long       mc_queue_1_threshold
  )
*/
UTF_TEST_CASE_MAC(fx950_api_tail_drop_set_queue_thresholds)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_num [0 / 1],
                   uc_queue_0_threshold[0 / 1530],
                   uc_queue_1_threshold[0 / 1530],
                   mc_queue_0_threshold[0 / 1530],
                   mc_queue_1_threshold[0 / 1530].
    Expected: SAND_OK.
    1.2. Call fx950_api_tail_drop_get_queue_thresholds with not NULL pointers and params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with link_num [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with uc_queue_0_threshold [1531] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with uc_queue_1_threshold [1531] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with mc_queue_0_threshold [1531] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with mc_queue_1_threshold [1531] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  link_num = 0;

    SAND_U32  uc_queue_0_threshold    = 0;
    SAND_U32  uc_queue_1_threshold    = 0;
    SAND_U32  mc_queue_0_threshold    = 0;
    SAND_U32  mc_queue_1_threshold    = 0;
    SAND_U32  uc_queue_0_thresholdGet = 0;
    SAND_U32  uc_queue_1_thresholdGet = 0;
    SAND_U32  mc_queue_0_thresholdGet = 0;
    SAND_U32  mc_queue_1_thresholdGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_num [0 / 1],
                           uc_queue_0_threshold[0 / 1530],
                           uc_queue_1_threshold[0 / 1530],
                           mc_queue_0_threshold[0 / 1530],
                           mc_queue_1_threshold[0 / 1530].
            Expected: SAND_OK.
        */
        /* iterate with link_num = 0 */
        link_num = 0;

        uc_queue_0_threshold = 0;
        uc_queue_1_threshold = 0;
        mc_queue_0_threshold = 0;
        mc_queue_1_threshold = 0;

        result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                          uc_queue_0_threshold,
                                                          uc_queue_1_threshold,
                                                          mc_queue_0_threshold,
                                                          mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL6_PARAM_MAC(SAND_OK, rc, device_id, link_num,
                                    uc_queue_0_threshold, uc_queue_1_threshold,
                                    mc_queue_0_threshold, mc_queue_1_threshold);

        /*
            1.2. Call fx950_api_tail_drop_get_queue_thresholds with not NULL pointers and params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_thresholdGet,
                                                          &uc_queue_1_thresholdGet,
                                                          &mc_queue_0_thresholdGet,
                                                          &mc_queue_1_thresholdGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_tail_drop_get_queue_thresholds: %d, %d", device_id, link_num);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_0_threshold, uc_queue_0_thresholdGet,
                   "got another uc_queue_0_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_1_threshold, uc_queue_1_thresholdGet,
                   "got another uc_queue_1_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_0_threshold, mc_queue_0_thresholdGet,
                   "got another mc_queue_0_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_1_threshold, mc_queue_1_thresholdGet,
                   "got another mc_queue_1_threshold than was set: %d", device_id);

        /* iterate with link_num = 1 */
        link_num = 1;

        uc_queue_0_threshold = 1530;
        uc_queue_1_threshold = 1530;
        mc_queue_0_threshold = 1530;
        mc_queue_1_threshold = 1530;

        result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                          uc_queue_0_threshold,
                                                          uc_queue_1_threshold,
                                                          mc_queue_0_threshold,
                                                          mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL6_PARAM_MAC(SAND_OK, rc, device_id, link_num,
                                    uc_queue_0_threshold, uc_queue_1_threshold,
                                    mc_queue_0_threshold, mc_queue_1_threshold);

        /*
            1.2. Call fx950_api_tail_drop_get_queue_thresholds with not NULL pointers and params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_thresholdGet,
                                                          &uc_queue_1_thresholdGet,
                                                          &mc_queue_0_thresholdGet,
                                                          &mc_queue_1_thresholdGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_tail_drop_get_queue_thresholds: %d, %d", device_id, link_num);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_0_threshold, uc_queue_0_thresholdGet,
                   "got another uc_queue_0_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_1_threshold, uc_queue_1_thresholdGet,
                   "got another uc_queue_1_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_0_threshold, mc_queue_0_thresholdGet,
                   "got another mc_queue_0_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_1_threshold, mc_queue_1_thresholdGet,
                   "got another mc_queue_1_threshold than was set: %d", device_id);

        /*
            1.3. Call with link_num [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_num = 2;

        result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                          uc_queue_0_threshold,
                                                          uc_queue_1_threshold,
                                                          mc_queue_0_threshold,
                                                          mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_num);

        link_num = 0;

        /*
            1.4. Call with uc_queue_0_threshold [1531] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_queue_0_threshold = 1531;

        result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                          uc_queue_0_threshold,
                                                          uc_queue_1_threshold,
                                                          mc_queue_0_threshold,
                                                          mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, uc_queue_0_threshold = %d", device_id, uc_queue_0_threshold);

        uc_queue_0_threshold = 0;

        /*
            1.5. Call with uc_queue_1_threshold [1531] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_queue_1_threshold = 1531;

        result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                          uc_queue_0_threshold,
                                                          uc_queue_1_threshold,
                                                          mc_queue_0_threshold,
                                                          mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, uc_queue_1_threshold = %d", device_id, uc_queue_1_threshold);

        uc_queue_1_threshold = 0;

        /*
            1.6. Call with mc_queue_0_threshold [1531] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        mc_queue_0_threshold = 1531;

        result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                          uc_queue_0_threshold,
                                                          uc_queue_1_threshold,
                                                          mc_queue_0_threshold,
                                                          mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, mc_queue_0_threshold = %d", device_id, mc_queue_0_threshold);

        mc_queue_0_threshold = 0;

        /*
            1.7. Call with mc_queue_1_threshold [1531] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        mc_queue_1_threshold = 1531;

        result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                          uc_queue_0_threshold,
                                                          uc_queue_1_threshold,
                                                          mc_queue_0_threshold,
                                                          mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, mc_queue_1_threshold = %d", device_id, mc_queue_1_threshold);
    }

    link_num = 0;

    uc_queue_0_threshold = 0;
    uc_queue_1_threshold = 0;
    mc_queue_0_threshold = 0;
    mc_queue_1_threshold = 0;
    
    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                          uc_queue_0_threshold,
                                                          uc_queue_1_threshold,
                                                          mc_queue_0_threshold,
                                                          mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num,
                                                      uc_queue_0_threshold,
                                                      uc_queue_1_threshold,
                                                      mc_queue_0_threshold,
                                                      mc_queue_1_threshold);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_tail_drop_get_queue_thresholds(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       link_num,
    SAND_OUT    unsigned long       *uc_queue_0_threshold_ptr,
    SAND_OUT    unsigned long       *uc_queue_1_threshold_ptr,
    SAND_OUT    unsigned long       *mc_queue_0_threshold_ptr,
    SAND_OUT    unsigned long       *mc_queue_1_threshold_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_tail_drop_get_queue_thresholds)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_num [0 / 1], non NULL uc_queue_0_threshold_ptr,
                                     non NULL uc_queue_1_threshold_ptr,
                                     non NULL mc_queue_0_threshold_ptr,
                                     non NULL mc_queue_1_threshold_ptr.
    Expected: SAND_OK.
    1.2. Call with link_num [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with uc_queue_0_threshold_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with uc_queue_1_threshold_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with mc_queue_0_threshold_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.6. Call with mc_queue_1_threshold_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  link_num = 0;

    SAND_U32  uc_queue_0_threshold = 0;
    SAND_U32  uc_queue_1_threshold = 0;
    SAND_U32  mc_queue_0_threshold = 0;
    SAND_U32  mc_queue_1_threshold = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_num [0 / 1], non NULL uc_queue_0_threshold_ptr,
                                             non NULL uc_queue_1_threshold_ptr,
                                             non NULL mc_queue_0_threshold_ptr,
                                             non NULL mc_queue_1_threshold_ptr.
            Expected: SAND_OK.
        */
        /* iterate with link_num = 0 */
        link_num = 0;

        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_threshold,
                                                          &uc_queue_1_threshold,
                                                          &mc_queue_0_threshold,
                                                          &mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_num);

        /* iterate with link_num = 0 */
        link_num = 1;

        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_threshold,
                                                          &uc_queue_1_threshold,
                                                          &mc_queue_0_threshold,
                                                          &mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_num);

        /*
            1.3. Call with link_num [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_num = 2;

        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_threshold,
                                                          &uc_queue_1_threshold,
                                                          &mc_queue_0_threshold,
                                                          &mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_num);

        link_num = 0;

        /*
            1.3. Call with uc_queue_0_threshold_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          NULL,
                                                          &uc_queue_1_threshold,
                                                          &mc_queue_0_threshold,
                                                          &mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uc_queue_0_threshold_ptr = NULL", device_id);

        /*
            1.4. Call with uc_queue_1_threshold_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_threshold,
                                                          NULL,
                                                          &mc_queue_0_threshold,
                                                          &mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uc_queue_1_threshold_ptr = NULL", device_id);

        /*
            1.5. Call with mc_queue_0_threshold_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_threshold,
                                                          &uc_queue_1_threshold,
                                                          NULL,
                                                          &mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mc_queue_0_threshold_ptr = NULL", device_id);

        /*
            1.6. Call with mc_queue_1_threshold_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_threshold,
                                                          &uc_queue_1_threshold,
                                                          &mc_queue_0_threshold,
                                                          NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mc_queue_1_threshold_ptr = NULL", device_id);
    }

    link_num = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                          &uc_queue_0_threshold,
                                                          &uc_queue_1_threshold,
                                                          &mc_queue_0_threshold,
                                                          &mc_queue_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num,
                                                      &uc_queue_0_threshold,
                                                      &uc_queue_1_threshold,
                                                      &mc_queue_0_threshold,
                                                      &mc_queue_1_threshold);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_tail_drop_set_link_thresholds(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       link_0_threshold,
    SAND_IN     unsigned long       link_1_threshold
  )
*/
UTF_TEST_CASE_MAC(fx950_api_tail_drop_set_link_thresholds)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_0_threshold[0 / 1446], link_1_threshold[0 / 1446].
    Expected: SAND_OK.
    1.2. Call fx950_api_tail_drop_get_link_thresholds with not NULL pointers and params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with link_0_threshold [1447] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with link_1_threshold [1447] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    
    SAND_U32  link_0_threshold    = 0;
    SAND_U32  link_1_threshold    = 0;
    SAND_U32  link_0_thresholdGet = 0;
    SAND_U32  link_1_thresholdGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_0_threshold[0 / 1446], link_1_threshold[0 / 1446].
            Expected: SAND_OK.
        */
        /* iterate with link_0_threshold = 0 */
        link_0_threshold = 0;
        link_1_threshold = 0;

        result = fx950_api_tail_drop_set_link_thresholds(device_id, link_0_threshold, link_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_0_threshold, link_1_threshold);

        /*
            1.2. Call fx950_api_tail_drop_get_link_thresholds with not NULL pointers and params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_tail_drop_get_link_thresholds(device_id, &link_0_thresholdGet, &link_1_thresholdGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_tail_drop_set_link_thresholds: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(link_0_threshold, link_0_thresholdGet,
                   "got another link_0_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(link_1_threshold, link_1_thresholdGet,
                   "got another link_1_threshold than was set: %d", device_id);
        
        /* iterate with link_0_threshold = 1446 */
        link_0_threshold = 1446;
        link_1_threshold = 1446;

        result = fx950_api_tail_drop_set_link_thresholds(device_id, link_0_threshold, link_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_0_threshold, link_1_threshold);

        /*
            1.2. Call fx950_api_tail_drop_get_link_thresholds with not NULL pointers and params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_tail_drop_get_link_thresholds(device_id, &link_0_thresholdGet, &link_1_thresholdGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_tail_drop_set_link_thresholds: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(link_0_threshold, link_0_thresholdGet,
                   "got another link_0_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(link_1_threshold, link_1_thresholdGet,
                   "got another link_1_threshold than was set: %d", device_id);

        /*
            1.3. Call with link_0_threshold [1447] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_0_threshold = 1447;

        result = fx950_api_tail_drop_set_link_thresholds(device_id, link_0_threshold, link_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link_0_threshold = %d", device_id, link_0_threshold);

        link_0_threshold = 0;

        /*
            1.4. Call with link_1_threshold [1447] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_1_threshold = 1447;

        result = fx950_api_tail_drop_set_link_thresholds(device_id, link_0_threshold, link_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link_1_threshold = %d", device_id, link_1_threshold);
    }

    link_0_threshold = 0;
    link_1_threshold = 0;
    
    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_tail_drop_set_link_thresholds(device_id, link_0_threshold, link_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_tail_drop_set_link_thresholds(device_id, link_0_threshold, link_1_threshold);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_tail_drop_get_link_thresholds(
    SAND_IN     unsigned int        device_id,
    SAND_OUT    unsigned long       *link_0_threshold_ptr,
    SAND_OUT    unsigned long       *link_1_threshold_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_tail_drop_get_link_thresholds)
{
/*
    ITERATE_DEVICES
    1.1. Call with non NULL link_0_threshold_ptr, non NULL link_1_threshold_ptr.
    Expected: SAND_OK.
    1.2. Call with link_0_threshold_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with link_1_threshold_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    
    SAND_U32  link_0_threshold = 0;
    SAND_U32  link_1_threshold = 0;
    

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non NULL link_0_threshold_ptr, non NULL link_1_threshold_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_tail_drop_get_link_thresholds(device_id, &link_0_threshold, &link_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with link_0_threshold_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_link_thresholds(device_id, NULL, &link_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_0_threshold_ptr = NULL", device_id);

        /*
            1.3. Call with link_1_threshold_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_tail_drop_get_link_thresholds(device_id, &link_0_threshold, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_1_threshold_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_tail_drop_get_link_thresholds(device_id, &link_0_threshold, &link_1_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_tail_drop_get_link_thresholds(device_id, &link_0_threshold, &link_1_threshold);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_tail_drop suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_tail_drop)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_tail_drop_set_packet_thresholds)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_tail_drop_get_packet_thresholds)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_tail_drop_set_total_buffer_threshold)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_tail_drop_get_total_buffer_threshold)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_tail_drop_set_queue_thresholds)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_tail_drop_get_queue_thresholds)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_tail_drop_set_link_thresholds)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_tail_drop_get_link_thresholds)
UTF_SUIT_END_TESTS_MAC(fx950_api_tail_drop)

