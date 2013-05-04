/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_ingress_queueUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_ingress_queue that provides
*       Ingress queue identifier allocator module.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_scheduler.h>
#include <FX950/include/fap20m_api_ingress_queue.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_API_INGRESS_QUEUE_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_open_queue(
    unsigned int device_id,
    unsigned int queue_id,
    unsigned int target_fap_id,
    unsigned int target_flow_id,
    FAP20M_QUEUE_TYPE queue_type
  )
*/
UTF_TEST_CASE_MAC(fap20m_open_queue)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_id [0 / 8191],
                   target_fap_id [0 / 2047],
                   target_flow_id [0 / 16383],
                   queue_type [FAP20M_QUEUE_1 / FAP20M_QUEUE_2].
    Expected: SAND_OK.
    1.2. Call fap20m_get_queue_params with not NULL pointers and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with queue_id [8192] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.4. Call with queue_id [4], target_fap_id [2048] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with queue_id [4], target_flow_id [16384] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.6. Call with queue_id [0], target_fap_id [2048] (not relevant) and other params from 1.1.
    Expected: SAND_OK.
    1.7. Call with queue_id [0], target_flow_id [16384] (not relevant) and other params from 1.1.
    Expected: SAND_OK.

    1.8. Call with queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  queue_id = 0;
    SAND_UINT  target_fap_id = 0;
    SAND_UINT  target_flow_id = 0;
    FAP20M_QUEUE_TYPE queue_type = FAP20M_QUEUE_1;

    SAND_UINT  target_fap_idGet = 0;
    SAND_UINT  target_flow_idGet = 0;
    FAP20M_QUEUE_TYPE queue_typeGet = FAP20M_QUEUE_1;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_id [0 / 8191],
                           target_fap_id [0 / 2047],
                           target_flow_id [0 / 16383],
                           queue_type [FAP20M_QUEUE_1 / FAP20M_QUEUE_2].
            Expected: SAND_OK.
        */
        /* iterate with queue_id = 0 */
        queue_id = 0;
        target_fap_id = 0;
        target_flow_id = 0;
        queue_type = FAP20M_QUEUE_1;

        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        /*
            1.2. Call fap20m_get_queue_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_idGet, &target_flow_idGet, &queue_typeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_queue_params: %d, %d", device_id, queue_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(target_fap_id, target_fap_idGet,
                   "got another target_fap_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(target_flow_id, target_flow_idGet,
                   "got another target_flow_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(queue_type, queue_typeGet,
                   "got another queue_type than was set: %d", device_id);

        /* iterate with queue_id = 8191 */
        queue_id = 8191;
        target_fap_id = 2047;
        target_flow_id = 16383;
        queue_type = FAP20M_QUEUE_2;

        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        /*
            1.2. Call fap20m_get_queue_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_idGet, &target_flow_idGet, &queue_typeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_queue_params: %d, %d", device_id, queue_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(target_fap_id, target_fap_idGet,
                   "got another target_fap_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(target_flow_id, target_flow_idGet,
                   "got another target_flow_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(queue_type, queue_typeGet,
                   "got another queue_type than was set: %d", device_id);

        /*
            1.3. Call with queue_id [12K] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_id = 12 * 1024;

        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        queue_id = 0;

        /*
            1.4. Call with queue_id [4], target_fap_id [2048] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_id = 4;
        target_fap_id = 2048;

        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, queue_id, target_fap_id);

        target_fap_id = 0;

        /*
            1.5. Call with queue_id [4], target_flow_id [16384] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_id = 4;
        target_flow_id = 16384;

        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, target_flow_id = %d", device_id, queue_id, target_flow_id);

        target_flow_id = 0;

        /*
            1.6. Call with queue_id [0], target_fap_id [2048] (OUT Of Range) and other params from 1.1.
            Expected: SAND_OK.
        */
        queue_id = 0;
        target_fap_id = 2048;

        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, queue_id, target_fap_id);

        target_fap_id = 0;

        /*
            1.7. Call with queue_id [0], target_flow_id [16384] (not relevant) and other params from 1.1.
            Expected: SAND_OK.
        */
        queue_id = 0;
        target_flow_id = 2048;

        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, target_flow_id = %d", device_id, queue_id, target_flow_id);

        target_flow_id = 0;

        /*
            1.8. Call with queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_type = SAND_UTF_API_INGRESS_QUEUE_INVALID_ENUM;

        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);
    }

    queue_id = 0;
    target_fap_id = 0;
    target_flow_id = 0;
    queue_type = FAP20M_QUEUE_1;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_update_queue(
    unsigned int device_id,
    unsigned int queue_id,
    unsigned int target_fap_id,
    unsigned int target_flow_id,
    FAP20M_QUEUE_TYPE queue_type
  )
*/
UTF_TEST_CASE_MAC(fap20m_update_queue)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_id [0 / 8191],
                   target_fap_id [0 / 2047],
                   target_flow_id [0 / 16383],
                   queue_type [FAP20M_QUEUE_1 / FAP20M_QUEUE_2].
    Expected: SAND_OK.
    1.2. Call fap20m_get_queue_params with not NULL pointers and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with queue_id [8192] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.4. Call with queue_id [4], target_fap_id [2048] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with queue_id [4], target_flow_id [16384] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.6. Call with queue_id [0], target_fap_id [2048] (not relevant) and other params from 1.1.
    Expected: SAND_OK.
    1.7. Call with queue_id [0], target_flow_id [16384] (not relevant) and other params from 1.1.
    Expected: SAND_OK.

    1.8. Call with queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  queue_id = 0;
    SAND_UINT  target_fap_id = 0;
    SAND_UINT  target_flow_id = 0;
    FAP20M_QUEUE_TYPE queue_type = FAP20M_QUEUE_1;

    SAND_UINT  target_fap_idGet = 0;
    SAND_UINT  target_flow_idGet = 0;
    FAP20M_QUEUE_TYPE queue_typeGet = FAP20M_QUEUE_1;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_id [0 / 8191],
                           target_fap_id [0 / 2047],
                           target_flow_id [0 / 16383],
                           queue_type [FAP20M_QUEUE_1 / FAP20M_QUEUE_2].
            Expected: SAND_OK.
        */
        /* iterate with queue_id = 0 */
        queue_id = 0;
        target_fap_id = 0;
        target_flow_id = 0;
        queue_type = FAP20M_QUEUE_1;

        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        /*
            1.2. Call fap20m_get_queue_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_idGet, &target_flow_idGet, &queue_typeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_queue_params: %d, %d", device_id, queue_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(target_fap_id, target_fap_idGet,
                   "got another target_fap_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(target_flow_id, target_flow_idGet,
                   "got another target_flow_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(queue_type, queue_typeGet,
                   "got another queue_type than was set: %d", device_id);

        /* iterate with queue_id = 8191 */
        queue_id = 8191;
        target_fap_id = 2047;
        target_flow_id = 16383;
        queue_type = FAP20M_QUEUE_2;

        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        /*
            1.2. Call fap20m_get_queue_params with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_idGet, &target_flow_idGet, &queue_typeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_queue_params: %d, %d", device_id, queue_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(target_fap_id, target_fap_idGet,
                   "got another target_fap_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(target_flow_id, target_flow_idGet,
                   "got another target_flow_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(queue_type, queue_typeGet,
                   "got another queue_type than was set: %d", device_id);

        /*
            1.3. Call with queue_id [12K] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_id = 12 * 1024;

        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        queue_id = 0;

        /*
            1.4. Call with queue_id [4], target_fap_id [2048] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_id = 4;
        target_fap_id = 2048;

        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, queue_id, target_fap_id);

        target_fap_id = 0;

        /*
            1.5. Call with queue_id [4], target_flow_id [16384] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_id = 4;
        target_flow_id = 16384;

        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, target_flow_id = %d", device_id, queue_id, target_flow_id);

        target_flow_id = 0;

        /*
            1.6. Call with queue_id [0], target_fap_id [2048] (out of range) and other params from 1.1.
            Expected: SAND_OK.
        */
        queue_id = 0;
        target_fap_id = 2048;

        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, queue_id, target_fap_id);

        target_fap_id = 0;

        /*
            1.7. Call with queue_id [0], target_flow_id [16384] (not relevant) and other params from 1.1.
            Expected: SAND_OK.
        */
        queue_id = 0;
        target_flow_id = 2048;

        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, target_flow_id = %d", device_id, queue_id, target_flow_id);

        target_flow_id = 0;

        /*
            1.8. Call with queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_type = SAND_UTF_API_INGRESS_QUEUE_INVALID_ENUM;

        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);
    }

    queue_id = 0;
    target_fap_id = 0;
    target_flow_id = 0;
    queue_type = FAP20M_QUEUE_1;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_queue_params(
    unsigned int  device_id,
    unsigned int  queue_id,
    unsigned int* target_fap_id,
    unsigned int* target_flow_id,
    FAP20M_QUEUE_TYPE* queue_type
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_queue_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_id [0 / 8191], 
                   not NULL target_fap_id, target_flow_id, queue_type.
    Expected: SAND_OK.
    1.2. Call with queue_id [8192] (out of range) and other params rfom 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with target_fap_id [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with target_flow_id [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with queue_type [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  queue_id = 0;
    SAND_UINT  target_fap_id = 0;
    SAND_UINT  target_flow_id = 0;
    FAP20M_QUEUE_TYPE queue_type = FAP20M_QUEUE_1;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_id [0 / 8191], 
                           not NULL target_fap_id, target_flow_id, queue_type.
            Expected: SAND_OK.
        */
        /* iterate with queue_id = 0 */
        queue_id = 0;

        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_id, &target_flow_id, &queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        /* iterate with queue_id = 8191 */
        queue_id = 8191;

        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_id, &target_flow_id, &queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        /*
            1.2. Call with queue_id [12K] (out of range) and other params rfom 1.1.
            Expected: NOT SAND_OK.
        */
        queue_id = 12*1024;

        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_id, &target_flow_id, &queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        queue_id = 100;

        /*
            1.3. Call with target_fap_id [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_queue_params(device_id, queue_id, NULL, &target_flow_id, &queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, target_fap_id = NULL", device_id);

        /*
            1.4. Call with target_flow_id [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_id, NULL, &queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, target_flow_id = NULL", device_id);

        /*
            1.5. Call with queue_type [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_id, &target_flow_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, queue_type = NULL", device_id);
    }

    queue_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_queue_params(device_id, queue_id, &target_fap_id, &target_flow_id, &queue_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_queue_params(device_id, queue_id, &target_fap_id, &target_flow_id, &queue_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_close_queue(
    unsigned int device_id,
    unsigned int queue_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_close_queue)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_id [0 / 8191].
    Expected: SAND_OK.
    1.2. Call with queue_id [8192] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  queue_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_id [0 / 8191].
            Expected: SAND_OK.
        */
        /* iterate with queue_id = 0 */
        queue_id = 0;

        result = fap20m_close_queue(device_id, queue_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        /* iterate with queue_id = 8191 */
        queue_id = 8191;

        result = fap20m_close_queue(device_id, queue_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        /*
            1.2. Call with queue_id [12K] (out of range).
            Expected: NOT SAND_OK.
        */
        queue_id = 12 * 1024;

        result = fap20m_close_queue(device_id, queue_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);
    }

    queue_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_close_queue(device_id, queue_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_close_queue(device_id, queue_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_ingress_queue_get_size(
    unsigned int   device_id,
    unsigned int   queue_id,
    unsigned long* queue_byte_size
  )
*/
UTF_TEST_CASE_MAC(fap20m_ingress_queue_get_size)
{
/*
    ITERATE_DEVICES
    1.1. Call with queue_id [0 / 8191], not NULL queue_byte_size_ptr.
    Expected: SAND_OK.
    1.2. Call with queue_id [8192] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.2. Call with queue_byte_size_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  queue_id = 0;
    SAND_U32   queue_byte_size = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with queue_id [0 / 8191], not NULL queue_byte_size_ptr.
            Expected: SAND_OK.
        */
        /* iterate with queue_id = 0 */
        queue_id = 0;

        result = fap20m_ingress_queue_get_size(device_id, queue_id, &queue_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        /* iterate with queue_id = 8191 */
        queue_id = 8191;

        result = fap20m_ingress_queue_get_size(device_id, queue_id, &queue_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        /*
            1.2. Call with queue_id [12K] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        queue_id = 12 * 1024;

        result = fap20m_ingress_queue_get_size(device_id, queue_id, &queue_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, queue_id);

        queue_id = 0;

        /*
            1.2. Call with queue_byte_size_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_ingress_queue_get_size(device_id, queue_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, queue_byte_size = NULL", device_id);
    }

    queue_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_ingress_queue_get_size(device_id, queue_id, &queue_byte_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_ingress_queue_get_size(device_id, queue_id, &queue_byte_size);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_ingress_queue suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_ingress_queue)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_open_queue)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_update_queue)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_queue_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_close_queue)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_ingress_queue_get_size)
UTF_SUIT_END_TESTS_MAC(fap20m_api_ingress_queue)

