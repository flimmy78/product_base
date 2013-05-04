/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_chunk_context_managerUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_chunk_context_manager, that provides
*       Chunk Context Manager APIs of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_chunk_context_manager.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chunk_context_manager_set_const_context_id(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   context_id_0,
    SAND_IN     unsigned long   context_id_1
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chunk_context_manager_set_const_context_id)
{
/*
    ITERATE_DEVICES
    1.1. Call with context_id_0 [0 / 3071], context_id_1 [0 / 3071].
    Expected: SAND_OK.
    1.2. Call fx950_api_chunk_context_manager_get_const_context_id with not NULL pointers.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with context_id_0 [0x1000] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with context_id_1 [0x1000] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id       = 0;
    SAND_U32  context_id_0    = 0;
    SAND_U32  context_id_1    = 0;
    SAND_U32  context_id_0Get = 0;
    SAND_U32  context_id_1Get = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with context_id_0 [0 / 3071], context_id_1 [0 / 3071].
            Expected: SAND_OK.
        */
        /* iterate with context_id_0 = 0 */
        context_id_0 = 0;
        context_id_1 = 0; 

        result = fx950_api_chunk_context_manager_set_const_context_id(device_id, context_id_0, context_id_1);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, context_id_0, context_id_1);
        
        /*
            1.2. Call fx950_api_chunk_context_manager_get_const_context_id with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_chunk_context_manager_get_const_context_id(device_id, &context_id_0Get, &context_id_1Get);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chunk_context_manager_get_const_context_id: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(context_id_0, context_id_0Get, "got another context_id_0 than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(context_id_1, context_id_1Get, "got another context_id_1 than was set: %d", device_id);

        /* iterate with context_id_0 = 3071 */
        context_id_0 = 3071;
        context_id_1 = 3071; 

        result = fx950_api_chunk_context_manager_set_const_context_id(device_id, context_id_0, context_id_1);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, context_id_0, context_id_1);
        
        /*
            1.2. Call fx950_api_chunk_context_manager_get_const_context_id with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_chunk_context_manager_get_const_context_id(device_id, &context_id_0Get, &context_id_1Get);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chunk_context_manager_get_const_context_id: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(context_id_0, context_id_0Get, "got another context_id_0 than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(context_id_1, context_id_1Get, "got another context_id_1 than was set: %d", device_id);

        /*
            1.3. Call with context_id_0 [0x1000] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        context_id_1 = 0x1000; 

        result = fx950_api_chunk_context_manager_set_const_context_id(device_id, context_id_0, context_id_1);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, context_id_0);

        context_id_1 = 0; 

        /*
            1.4. Call with context_id_1 [0x1000] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        context_id_1 = 0x1000; 

        result = fx950_api_chunk_context_manager_set_const_context_id(device_id, context_id_0, context_id_1);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, context_id_1 = %d", device_id, context_id_1);
    }

    context_id_0 = 0;
    context_id_1 = 0; 

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chunk_context_manager_set_const_context_id(device_id, context_id_0, context_id_1);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chunk_context_manager_set_const_context_id(device_id, context_id_0, context_id_1);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chunk_context_manager_get_const_context_id(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    unsigned long   *context_id_0_ptr,
    SAND_OUT    unsigned long   *context_id_1_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chunk_context_manager_get_const_context_id)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL context_id_0_ptr, context_id_1_ptr.
    Expected: SAND_OK.
    1.2. Call with context_id_0_ptr [NULL], and other param as in 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with context_id_1_ptr [NULL], and other param as in 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id    = 0;
    SAND_U32  context_id_0 = 0;
    SAND_U32  context_id_1 = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL context_id_0_ptr, context_id_1_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_chunk_context_manager_get_const_context_id(device_id, &context_id_0, &context_id_1);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, context_id_0, context_id_1);

        /*
            1.2. Call with context_id_0_ptr [NULL], and other param as in 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chunk_context_manager_get_const_context_id(device_id, NULL, &context_id_1);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, context_id_0 = NULL", device_id);

        /*
            1.3. Call with context_id_1_ptr [NULL], and other param as in 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chunk_context_manager_get_const_context_id(device_id, &context_id_0, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, context_id_1 = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chunk_context_manager_get_const_context_id(device_id, &context_id_0, &context_id_1);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chunk_context_manager_get_const_context_id(device_id, &context_id_0, &context_id_1);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   flow_id,
    SAND_IN     unsigned int    valid,
    SAND_IN     unsigned long   context_id
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping)
{
/*
    ITERATE_DEVICES
    1.1. Call with flow_id [0 / 16K], valid [TRUE / FALSE], context_id [0 / 0xFFF].
    Expected: SAND_OK.
    1.2. Call fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping 
              with not NULL pointers and flow_id as in 1.1.
    Expected: SAND_OK and the same valid and context_id as was set.
    1.3. Call with flow_id [16K + 1] (out of range) and other params as in 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with context_id [0x1000] (out of range) and other params as in 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc     = SAND_OK;

    SAND_UINT device_id     = 0;
    SAND_U32  flow_id       = 0;
    SAND_UINT valid         = 0;
    SAND_U32  context_id    = 0;
    SAND_UINT validGet      = 0;
    SAND_U32  context_idGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with flow_id [0 / 16K], 
                           valid [TRUE / FALSE], 
                           context_id [0 / 3071].
            Expected: SAND_OK.
        */
        /* iterate with flow_id = 0 */
        flow_id    = 0;
        valid      = FALSE;
        context_id = 0;

        result = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(device_id, flow_id, valid, context_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, flow_id, valid, context_id);
        
        /*
            1.2. Call fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping 
                      with not NULL pointers and flow_id as in 1.1.
            Expected: SAND_OK and the same valid and context_id as was set.
        */
        result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &validGet, &context_idGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping: %d, %d",
                                     device_id, flow_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet, "got another valid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(context_id, context_idGet, "got another context_id than was set: %d", device_id);

        /* iterate with flow_id = 16k */
        flow_id    = 0x3FFF;
        valid      = TRUE;
        context_id = 3071;

        result = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(device_id, flow_id, valid, context_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, flow_id, valid, context_id);
        
        /*
            1.2. Call fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping 
                      with not NULL pointers and flow_id as in 1.1.
            Expected: SAND_OK and the same valid and context_id as was set.
        */
        result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &validGet, &context_idGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping: %d, %d",
                                     device_id, flow_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet, "got another valid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(context_id, context_idGet, "got another context_id than was set: %d", device_id);

        /*
            1.3. Call with flow_id [16K + 1] (out of range) and other params as in 1.1.
            Expected: NOT SAND_OK.
        */
        flow_id = 0x4000;

        result = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(device_id, flow_id, valid, context_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, flow_id = %d", device_id, flow_id);

        flow_id = 0;

        /*
            1.4. Call with context_id [0x1000] (out of range) and other params as in 1.1.
            Expected: NOT SAND_OK.
        */
        context_id = 0x1000;

        result = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(device_id, flow_id, valid, context_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, context_id = %d", device_id, context_id);
    }

    flow_id    = 0;
    valid      = TRUE;
    context_id = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(device_id, flow_id, valid, context_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(device_id, flow_id, valid, context_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   flow_id,
    SAND_OUT    unsigned int    *valid_ptr,
    SAND_OUT    unsigned long   *context_id_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping)
{
/*
    ITERATE_DEVICES
    1.1. Call with flow_id [0 / 16K], not NULL valid_ptr, context_id_ptr.
    Expected: SAND_OK.
    1.2. Call with flow_id [16K + 1] (out of range) and other params as in 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with valid_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with context_id_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id  = 0;
    SAND_U32  flow_id    = 0;
    SAND_UINT valid      = 0;
    SAND_U32  context_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with flow_id [0 / 16K], not NULL valid_ptr, context_id_ptr.
            Expected: SAND_OK.
        */
        /* iterate with flow_id = 0 */
        flow_id = 0;

        result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &valid, &context_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, flow_id);

        /* iterate with flow_id = 16K */
        flow_id = 0x3FFF;

        result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &valid, &context_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, flow_id);

        /*
            1.2. Call with flow_id [16K + 1] (out of range) and other params as in 1.1.
            Expected: NOT SAND_OK.
        */
        flow_id = 0x4000;

        result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &valid, &context_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, flow_id = %d", device_id, flow_id);

        flow_id = 0;

        /*
            1.3. Call with valid_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, NULL, &context_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, valid = NULL", device_id);

        /*
            1.4. Call with context_id_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &valid, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, context_id = NULL", device_id);
    }

    flow_id = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &valid, &context_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &valid, &context_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_chunk_context_manager suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_chunk_context_manager)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chunk_context_manager_set_const_context_id)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chunk_context_manager_get_const_context_id)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping)
UTF_SUIT_END_TESTS_MAC(fx950_api_chunk_context_manager)

