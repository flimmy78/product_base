/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_schedulerUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_scheduler, that provides
*       Hyper G. Link traffic scheduling API of 98FX950 device 
*   driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_scheduler.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_API_SCHEDULER_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_set_mtu_size_mode(
    SAND_IN     unsigned int                    device_id,
    SAND_IN     FX950_HGL_SCHEDULER_MTU_SIZE_MODE   mtu_size_mode
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_set_mtu_size_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call mtu_size_mode [FX950_HGL_SCHEDULER_MTU_SIZE_16K / 
                             FX950_HGL_SCHEDULER_MTU_SIZE_8K / 
                             FX950_HGL_SCHEDULER_MTU_SIZE_2K].
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_scheduler_get_mtu_size_mode with not NULL mtu_size_mode_ptr.
    Expected: SAND_OK and other params as was set.
    1.3. Call with mtu_size_mode [0x5AAAAAA5].
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_HGL_SCHEDULER_MTU_SIZE_MODE   mtu_size_mode = FX950_HGL_SCHEDULER_MTU_SIZE_16K;
    FX950_HGL_SCHEDULER_MTU_SIZE_MODE   mtu_size_modeGet = FX950_HGL_SCHEDULER_MTU_SIZE_16K;


    mtu_size_mode = FX950_HGL_SCHEDULER_MTU_SIZE_16K;
    mtu_size_modeGet = FX950_HGL_SCHEDULER_MTU_SIZE_16K;

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call mtu_size_mode [FX950_HGL_SCHEDULER_MTU_SIZE_16K / 
                                     FX950_HGL_SCHEDULER_MTU_SIZE_8K / 
                                     FX950_HGL_SCHEDULER_MTU_SIZE_2K].
            Expected: SAND_OK.
        */
        /* iterate with FX950_HGL_SCHEDULER_MTU_SIZE_16K */
        mtu_size_mode = FX950_HGL_SCHEDULER_MTU_SIZE_16K;

        result = fx950_api_hgl_scheduler_set_mtu_size_mode(device_id, mtu_size_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mtu_size_mode);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_mtu_size_mode with not NULL mtu_size_mode_ptr.
            Expected: SAND_OK and other params as was set.
        */
        result = fx950_api_hgl_scheduler_get_mtu_size_mode(device_id, &mtu_size_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_mtu_size_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(mtu_size_mode, mtu_size_mode, "got another mtu_size_mode than was set: %d", device_id);

        /* iterate with FX950_HGL_SCHEDULER_MTU_SIZE_8K */
        mtu_size_mode = FX950_HGL_SCHEDULER_MTU_SIZE_8K;

        result = fx950_api_hgl_scheduler_set_mtu_size_mode(device_id, mtu_size_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mtu_size_mode);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_mtu_size_mode with not NULL mtu_size_mode_ptr.
            Expected: SAND_OK and other params as was set.
        */
        result = fx950_api_hgl_scheduler_get_mtu_size_mode(device_id, &mtu_size_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_mtu_size_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(mtu_size_mode, mtu_size_mode, "got another mtu_size_mode than was set: %d", device_id);

        /* iterate with FX950_HGL_SCHEDULER_MTU_SIZE_2K */
        mtu_size_mode = FX950_HGL_SCHEDULER_MTU_SIZE_2K;

        result = fx950_api_hgl_scheduler_set_mtu_size_mode(device_id, mtu_size_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mtu_size_mode);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_mtu_size_mode with not NULL mtu_size_mode_ptr.
            Expected: SAND_OK and other params as was set.
        */
        result = fx950_api_hgl_scheduler_get_mtu_size_mode(device_id, &mtu_size_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_mtu_size_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(mtu_size_mode, mtu_size_mode, "got another mtu_size_mode than was set: %d", device_id);

        /*
            1.3. Call with mtu_size_mode [0x5AAAAAA5].
            Expected: NOT SAND_OK.
        */
        mtu_size_mode = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fx950_api_hgl_scheduler_set_mtu_size_mode(device_id, mtu_size_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mtu_size_mode);
    }

    mtu_size_mode = FX950_HGL_SCHEDULER_MTU_SIZE_16K;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_set_mtu_size_mode(device_id, mtu_size_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_set_mtu_size_mode(device_id, mtu_size_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_get_mtu_size_mode(
    SAND_IN     unsigned int                    device_id,
    SAND_OUT    FX950_HGL_SCHEDULER_MTU_SIZE_MODE   *mtu_size_mode_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_get_mtu_size_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null mtu_size_mode_ptr.
    Expected: SAND_OK.
    1.2. Call with mtu_size_mode_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_HGL_SCHEDULER_MTU_SIZE_MODE mtu_size_mode = FX950_HGL_SCHEDULER_MTU_SIZE_16K;

    
    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null mtu_size_mode_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_hgl_scheduler_get_mtu_size_mode(device_id, &mtu_size_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with mtu_size_mode_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_mtu_size_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mtu_size_mode = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_get_mtu_size_mode(device_id, &mtu_size_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_get_mtu_size_mode(device_id, &mtu_size_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       link,
    SAND_IN     unsigned long       sp_weight
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight)
{
/*
    ITERATE_DEVICES
    1.1. Call with link [0 / 1], sp_weight [0 / 254].
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_scheduler_get_link_level_2_weight 
              with not NULL sp_weight_ptr and other params from 1.1.
    Expected: SAND_OK and other params as was set.
    1.3. Call with link [2] (out of range) and other params as was set.
    Expected: NOT SAND_OK.
    1.4. Call with sp_weight [255] (out of range) and other params as was set.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link = 0;
    SAND_U32   sp_weight = 0;
    SAND_U32   sp_weightGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link [0 / 1], sp_weight [0 / 254].
            Expected: SAND_OK.
        */
        /* iterate with link = 0 */
        link = 0;
        sp_weight = 0;

        result = fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(device_id, link, sp_weight);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link, sp_weight);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_link_level_2_weight 
                      with not NULL sp_weight_ptr and other params from 1.1.
            Expected: SAND_OK and other params as was set.
        */
        result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, &sp_weightGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_link_level_2_weight: %d, %d", device_id, link);

        UTF_VERIFY_EQUAL1_STRING_MAC(sp_weight, sp_weight, "got another sp_weight than was set: %d", device_id);

        /* iterate with link = 1 */
        link = 1;
        sp_weight = 254;

        result = fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(device_id, link, sp_weight);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link, sp_weight);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_link_level_2_weight 
                      with not NULL sp_weight_ptr and other params from 1.1.
            Expected: SAND_OK and other params as was set.
        */
        result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, &sp_weightGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_link_level_2_weight: %d, %d", device_id, link);

        UTF_VERIFY_EQUAL1_STRING_MAC(sp_weight, sp_weight, "got another sp_weight than was set: %d", device_id);

        /*
            1.3. Call with link [2] (out of range) and other params as was set.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(device_id, link, sp_weight);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        link = 0;

        /*
            1.4. Call with sp_weight [255] (out of range) and other params as was set.
            Expected: NOT SAND_OK.
        */
        sp_weight = 255;

        result = fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(device_id, link, sp_weight);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sp_weight = %d", device_id, sp_weight);
    }

    link = 0;
    sp_weight = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(device_id, link, sp_weight);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(device_id, link, sp_weight);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_get_link_level_2_weight(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       link,
    SAND_OUT    unsigned long       *sp_weight_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_get_link_level_2_weight)
{
/*
    ITERATE_DEVICES
    1.1. Call with link [0 / 1], non-null sp_weight_ptr.
    Expected: SAND_OK.
    1.2. Call with link [2] (out of range) and other params as was set.
    Expected: NOT SAND_OK.
    1.3. Call with sp_weight_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link = 0;
    SAND_U32   sp_weight = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link [0 / 1], non-null sp_weight_ptr.
            Expected: SAND_OK.
        */
        /* iterate with link = 0 */
        link = 0;

        result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, &sp_weight);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        /* iterate with link = 1 */
        link = 1;

        result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, &sp_weight);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        /*
            1.2. Call with link [2] (out of range) and other params as was set.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, &sp_weight);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        link = 0;

        /*
            1.3. Call with sp_weight_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sp_weight = NULL", device_id);
    }

    link = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, &sp_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, &sp_weight);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(
    SAND_IN     unsigned int            device_id,
    SAND_IN     unsigned long           link,
    SAND_IN     FX950_HGL_SCHEDULER_TYPE    level_2_scheduler_type
  ) ;
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_set_link_level_2_scheduler_type)
{
/*
    ITERATE_DEVICES
    1.1. Call with link [0 / 1], 
                   level_2_scheduler_type [FX950_HGL_SCHEDULER_SP / 
                                           FX950_HGL_SCHEDULER_SDWRR].
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_scheduler_get_link_level_2_scheduler_type with not NULL level_2_scheduler_type_ptr and other params from 1.1.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with link [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with level_2_scheduler_type [0x5AAAAAA5] and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link      = 0;

    FX950_HGL_SCHEDULER_TYPE level_2_scheduler_type    = FX950_HGL_SCHEDULER_SDWRR;
    FX950_HGL_SCHEDULER_TYPE level_2_scheduler_typeGet = FX950_HGL_SCHEDULER_SDWRR;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link [0 / 1], 
                           level_2_scheduler_type [FX950_HGL_SCHEDULER_SP / 
                                                   FX950_HGL_SCHEDULER_SDWRR].
            Expected: SAND_OK.
        */
        /* iterate with link = 0 */
        link = 0;
        level_2_scheduler_type = FX950_HGL_SCHEDULER_SP;

        result = fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(device_id, link, level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link, level_2_scheduler_type);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_link_level_2_scheduler_type with not NULL level_2_scheduler_type_ptr and other params from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, &level_2_scheduler_typeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_link_level_2_scheduler_type: %d, %d", device_id, link);

        UTF_VERIFY_EQUAL1_STRING_MAC(level_2_scheduler_type, level_2_scheduler_typeGet, 
                                     "got another level_2_scheduler_type than was set: %d", device_id);

        /* iterate with link = 1 */
        link = 1;
        level_2_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;

        result = fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(device_id, link, level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link, level_2_scheduler_type);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_link_level_2_scheduler_type with not NULL level_2_scheduler_type_ptr and other params from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, &level_2_scheduler_typeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_link_level_2_scheduler_type: %d, %d", device_id, link);

        UTF_VERIFY_EQUAL1_STRING_MAC(level_2_scheduler_type, level_2_scheduler_typeGet, 
                                     "got another level_2_scheduler_type than was set: %d", device_id);

        /*
            1.3. Call with link [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(device_id, link, level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        link = 0;

        /*
            1.4. Call with level_2_scheduler_type [0x5AAAAAA5] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        level_2_scheduler_type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(device_id, link, level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, level_2_scheduler_type = %d",
                                         device_id, level_2_scheduler_type);
    }

    link = 0;
    level_2_scheduler_type = FX950_HGL_SCHEDULER_SP;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(device_id, link, level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(device_id, link, level_2_scheduler_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(
    SAND_IN     unsigned int            device_id,
    SAND_IN     unsigned long           link,
    SAND_OUT    FX950_HGL_SCHEDULER_TYPE    *level_2_scheduler_type_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_get_link_level_2_scheduler_type)
{
/*
    ITERATE_DEVICES
    1.1. Call with link [0 / 1], non-null level_2_scheduler_type_ptr.
    Expected: SAND_OK.
    1.2. Call with link [2] (out of range) and other params as was set.
    Expected: NOT SAND_OK.
    1.3. Call with level_2_scheduler_type_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link = 0;
    FX950_HGL_SCHEDULER_TYPE level_2_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link [0 / 1], non-null level_2_scheduler_type_ptr.
            Expected: SAND_OK.
        */
        /* iterate with link = 0 */
        link = 0;

        result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, &level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        /* iterate with link = 1 */
        link = 1;

        result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, &level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        /*
            1.2. Call with link [2] (out of range) and other params as was set.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, &level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        link = 0;

        /*
            1.3. Call with level_2_scheduler_type_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, level_2_scheduler_type_ptr = NULL", device_id);
    }

    link = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, &level_2_scheduler_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, &level_2_scheduler_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_set_link_queue_scheduling_type(
    SAND_IN     unsigned int                device_id,
    SAND_IN     unsigned long               link,
    SAND_IN     FX950_HGL_SCHEDULER_TYPE    uc_queue_0_scheduler_type,
    SAND_IN     FX950_HGL_SCHEDULER_TYPE    uc_queue_1_scheduler_type,
    SAND_IN     FX950_HGL_SCHEDULER_TYPE    mc_queue_0_scheduler_type,
    SAND_IN     FX950_HGL_SCHEDULER_TYPE    mc_queue_1_scheduler_type
  ) ;
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_set_link_queue_scheduling_type)
{
/*
    ITERATE_DEVICES
    1.1. Call with link [0 / 1], 
                   uc_queue_0_scheduler_type [FX950_HGL_SCHEDULER_SP / FX950_HGL_SCHEDULER_SDWRR], 
                   uc_queue_1_scheduler_type [FX950_HGL_SCHEDULER_SP / FX950_HGL_SCHEDULER_SDWRR], 
                   mc_queue_0_scheduler_type [FX950_HGL_SCHEDULER_SP / FX950_HGL_SCHEDULER_SDWRR], 
                   mc_queue_1_scheduler_type [FX950_HGL_SCHEDULER_SP / FX950_HGL_SCHEDULER_SDWRR].
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_scheduler_get_link_queue_scheduling_type with not NULL pointers and link from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with link [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with uc_queue_0_scheduler_type [0x5AAAAAA5] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with uc_queue_1_scheduler_type [0x5AAAAAA5] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with mc_queue_0_scheduler_type [0x5AAAAAA5] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with mc_queue_1_scheduler_type [0x5AAAAAA5] and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link = 0;

    FX950_HGL_SCHEDULER_TYPE uc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;
    FX950_HGL_SCHEDULER_TYPE uc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;
    FX950_HGL_SCHEDULER_TYPE mc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;
    FX950_HGL_SCHEDULER_TYPE mc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;

    FX950_HGL_SCHEDULER_TYPE uc_queue_0_scheduler_typeGet = FX950_HGL_SCHEDULER_SDWRR;
    FX950_HGL_SCHEDULER_TYPE uc_queue_1_scheduler_typeGet = FX950_HGL_SCHEDULER_SDWRR;
    FX950_HGL_SCHEDULER_TYPE mc_queue_0_scheduler_typeGet = FX950_HGL_SCHEDULER_SDWRR;
    FX950_HGL_SCHEDULER_TYPE mc_queue_1_scheduler_typeGet = FX950_HGL_SCHEDULER_SDWRR;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link [0 / 1], 
                           uc_queue_0_scheduler_type [FX950_HGL_SCHEDULER_SP / FX950_HGL_SCHEDULER_SDWRR], 
                           uc_queue_1_scheduler_type [FX950_HGL_SCHEDULER_SP / FX950_HGL_SCHEDULER_SDWRR], 
                           mc_queue_0_scheduler_type [FX950_HGL_SCHEDULER_SP / FX950_HGL_SCHEDULER_SDWRR], 
                           mc_queue_1_scheduler_type [FX950_HGL_SCHEDULER_SP / FX950_HGL_SCHEDULER_SDWRR].
            Expected: SAND_OK.
        */
        /* iterate with link = 0 */
        link = 0;

        uc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SP;
        uc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SP;
        mc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SP;
        mc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SP;

        result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                        uc_queue_0_scheduler_type,
                                                                        uc_queue_1_scheduler_type,
                                                                        mc_queue_0_scheduler_type,
                                                                        mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL6_PARAM_MAC(SAND_OK, rc, device_id, link,
                                                 uc_queue_0_scheduler_type,
                                                 uc_queue_1_scheduler_type,
                                                 mc_queue_0_scheduler_type,
                                                 mc_queue_1_scheduler_type);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_link_queue_scheduling_type with not NULL pointers and link from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id, link,
                                                                        &uc_queue_0_scheduler_typeGet,
                                                                        &uc_queue_1_scheduler_typeGet,
                                                                        &mc_queue_0_scheduler_typeGet,
                                                                        &mc_queue_1_scheduler_typeGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_link_queue_scheduling_type: %d, %d", device_id, link);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_0_scheduler_type, uc_queue_0_scheduler_typeGet, 
                                     "got another uc_queue_0_scheduler_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_1_scheduler_type, uc_queue_1_scheduler_typeGet, 
                                     "got another uc_queue_1_scheduler_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_0_scheduler_type, mc_queue_0_scheduler_typeGet, 
                                     "got another mc_queue_0_scheduler_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_1_scheduler_type, mc_queue_1_scheduler_typeGet, 
                                     "got another mc_queue_1_scheduler_type than was set: %d", device_id);

        /* iterate with link = 1 */
        link = 1;

        uc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;
        uc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;
        mc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;
        mc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;

        result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                        uc_queue_0_scheduler_type,
                                                                        uc_queue_1_scheduler_type,
                                                                        mc_queue_0_scheduler_type,
                                                                        mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL6_PARAM_MAC(SAND_OK, rc, device_id, link,
                                                 uc_queue_0_scheduler_type,
                                                 uc_queue_1_scheduler_type,
                                                 mc_queue_0_scheduler_type,
                                                 mc_queue_1_scheduler_type);

        /*
            1.2. Call fx950_api_hgl_scheduler_get_link_queue_scheduling_type with not NULL pointers and link from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id, link,
                                                                        &uc_queue_0_scheduler_typeGet,
                                                                        &uc_queue_1_scheduler_typeGet,
                                                                        &mc_queue_0_scheduler_typeGet,
                                                                        &mc_queue_1_scheduler_typeGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_link_queue_scheduling_type: %d, %d", device_id, link);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_0_scheduler_type, uc_queue_0_scheduler_typeGet, 
                                     "got another uc_queue_0_scheduler_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_1_scheduler_type, uc_queue_1_scheduler_typeGet, 
                                     "got another uc_queue_1_scheduler_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_0_scheduler_type, mc_queue_0_scheduler_typeGet, 
                                     "got another mc_queue_0_scheduler_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_1_scheduler_type, mc_queue_1_scheduler_typeGet, 
                                     "got another mc_queue_1_scheduler_type than was set: %d", device_id);

        /*
            1.3. Call with link [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                        uc_queue_0_scheduler_type,
                                                                        uc_queue_1_scheduler_type,
                                                                        mc_queue_0_scheduler_type,
                                                                        mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        link = 0;

        /*
            1.4. Call with uc_queue_0_scheduler_type [0x5AAAAAA5] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_queue_0_scheduler_type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                        uc_queue_0_scheduler_type,
                                                                        uc_queue_1_scheduler_type,
                                                                        mc_queue_0_scheduler_type,
                                                                        mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, uc_queue_0_scheduler_type = %d", 
                                         device_id, uc_queue_0_scheduler_type);

        uc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;

        /*
            1.5. Call with uc_queue_1_scheduler_type [0x5AAAAAA5] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_queue_1_scheduler_type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                        uc_queue_0_scheduler_type,
                                                                        uc_queue_1_scheduler_type,
                                                                        mc_queue_0_scheduler_type,
                                                                        mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, uc_queue_1_scheduler_type = %d", 
                                         device_id, uc_queue_1_scheduler_type);

        uc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;

        /*
            1.6. Call with mc_queue_0_scheduler_type [0x5AAAAAA5] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        mc_queue_0_scheduler_type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                        uc_queue_0_scheduler_type,
                                                                        uc_queue_1_scheduler_type,
                                                                        mc_queue_0_scheduler_type,
                                                                        mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, mc_queue_0_scheduler_type = %d", 
                                         device_id, mc_queue_0_scheduler_type);

        mc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SDWRR;

        /*
            1.7. Call with mc_queue_1_scheduler_type [0x5AAAAAA5] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        mc_queue_1_scheduler_type = SAND_UTF_API_SCHEDULER_INVALID_ENUM;

        result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                        uc_queue_0_scheduler_type,
                                                                        uc_queue_1_scheduler_type,
                                                                        mc_queue_0_scheduler_type,
                                                                        mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, mc_queue_1_scheduler_type = %d", 
                                         device_id, mc_queue_1_scheduler_type);
    }

    link = 0;

    uc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SP;
    uc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SP;
    mc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SP;
    mc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SP;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                        uc_queue_0_scheduler_type,
                                                                        uc_queue_1_scheduler_type,
                                                                        mc_queue_0_scheduler_type,
                                                                        mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link,
                                                                    uc_queue_0_scheduler_type,
                                                                    uc_queue_1_scheduler_type,
                                                                    mc_queue_0_scheduler_type,
                                                                    mc_queue_1_scheduler_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_get_link_queue_scheduling_type(
    SAND_IN     unsigned int                device_id,
    SAND_IN     unsigned long               link,
    SAND_OUT    FX950_HGL_SCHEDULER_TYPE    *uc_queue_0_scheduler_type_ptr,
    SAND_OUT    FX950_HGL_SCHEDULER_TYPE    *uc_queue_1_scheduler_type_ptr,
    SAND_OUT    FX950_HGL_SCHEDULER_TYPE    *mc_queue_0_scheduler_type_ptr,
    SAND_OUT    FX950_HGL_SCHEDULER_TYPE    *mc_queue_1_scheduler_type_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_get_link_queue_scheduling_type)
{
/*
    ITERATE_DEVICES
    1.1. Call with link [0 / 1] and non-null pointers.
    Expected: SAND_OK.
    1.2. Call with link [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with uc_queue_0_scheduler_type [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with uc_queue_1_scheduler_type [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with mc_queue_0_scheduler_type [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.6. Call with mc_queue_1_scheduler_type [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link = 0;

    FX950_HGL_SCHEDULER_TYPE uc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SP;
    FX950_HGL_SCHEDULER_TYPE uc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SP;
    FX950_HGL_SCHEDULER_TYPE mc_queue_0_scheduler_type = FX950_HGL_SCHEDULER_SP;
    FX950_HGL_SCHEDULER_TYPE mc_queue_1_scheduler_type = FX950_HGL_SCHEDULER_SP;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link [0 / 1] and non-null pointers.
            Expected: SAND_OK.
        */
        /* iterate with link = 0 */
        link = 0;

        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                        &uc_queue_0_scheduler_type,
                                                                        &uc_queue_1_scheduler_type,
                                                                        &mc_queue_0_scheduler_type,
                                                                        &mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        /* iterate with link = 1 */
        link = 1;

        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                        &uc_queue_0_scheduler_type,
                                                                        &uc_queue_1_scheduler_type,
                                                                        &mc_queue_0_scheduler_type,
                                                                        &mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        /*
            1.2. Call with link [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                        &uc_queue_0_scheduler_type,
                                                                        &uc_queue_1_scheduler_type,
                                                                        &mc_queue_0_scheduler_type,
                                                                        &mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        link = 0;

        /*
            1.3. Call with uc_queue_0_scheduler_type [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                        NULL,
                                                                        &uc_queue_1_scheduler_type,
                                                                        &mc_queue_0_scheduler_type,
                                                                        &mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uc_queue_0_scheduler_type = NULL", device_id);

        /*
            1.4. Call with uc_queue_1_scheduler_type [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                        &uc_queue_0_scheduler_type,
                                                                        NULL,
                                                                        &mc_queue_0_scheduler_type,
                                                                        &mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uc_queue_1_scheduler_type = NULL", device_id);

        /*
            1.5. Call with mc_queue_0_scheduler_type [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                        &uc_queue_0_scheduler_type,
                                                                        &uc_queue_1_scheduler_type,
                                                                        NULL,
                                                                        &mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mc_queue_0_scheduler_type = NULL", device_id);

        /*
            1.6. Call with mc_queue_1_scheduler_type [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                        &uc_queue_0_scheduler_type,
                                                                        &uc_queue_1_scheduler_type,
                                                                        &mc_queue_0_scheduler_type,
                                                                        NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mc_queue_1_scheduler_type = NULL", device_id);
    }

    link = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                        &uc_queue_0_scheduler_type,
                                                                        &uc_queue_1_scheduler_type,
                                                                        &mc_queue_0_scheduler_type,
                                                                        &mc_queue_1_scheduler_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id,link,
                                                                    &uc_queue_0_scheduler_type,
                                                                    &uc_queue_1_scheduler_type,
                                                                    &mc_queue_0_scheduler_type,
                                                                    &mc_queue_1_scheduler_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       link,
    SAND_IN     unsigned long       uc_queue_0_weight,
    SAND_IN     unsigned long       uc_queue_1_weight,
    SAND_IN     unsigned long       mc_queue_0_weight,
    SAND_IN     unsigned long       mc_queue_1_weight
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights)
{
/*
    ITERATE_DEVICES
    1.1. Call with link [0 / 1], 
                   uc_queue_0_weight [0 / 254], 
                   uc_queue_1_weight [0 / 254], 
                   mc_queue_0_weight [0 / 254], 
                   mc_queue_1_weight [0 / 254].
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights with not NULL pointers and link from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with link [2] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with uc_queue_0_weight [255] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with uc_queue_1_weight [255] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with mc_queue_0_weight [255] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with mc_queue_1_weight [255] and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link = 0;
    SAND_U32   uc_queue_0_weight = 0;
    SAND_U32   uc_queue_1_weight = 0;
    SAND_U32   mc_queue_0_weight = 0;
    SAND_U32   mc_queue_1_weight = 0;

    SAND_U32   uc_queue_0_weightGet = 0;
    SAND_U32   uc_queue_1_weightGet = 0;
    SAND_U32   mc_queue_0_weightGet = 0;
    SAND_U32   mc_queue_1_weightGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link [0 / 1], 
                           uc_queue_0_weight [0 / 254], 
                           uc_queue_1_weight [0 / 254], 
                           mc_queue_0_weight [0 / 254], 
                           mc_queue_1_weight [0 / 254].
            Expected: SAND_OK.
        */
        /* iterate with link = 0 */
        link = 0;
        uc_queue_0_weight = 0; 
        uc_queue_1_weight = 0; 
        mc_queue_0_weight = 0; 
        mc_queue_1_weight = 0; 

        result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                      uc_queue_0_weight, 
                                                                      uc_queue_1_weight, 
                                                                      mc_queue_0_weight, 
                                                                      mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL6_PARAM_MAC(SAND_OK, rc, device_id, link, 
                                                 uc_queue_0_weight, 
                                                 uc_queue_1_weight, 
                                                 mc_queue_0_weight, 
                                                 mc_queue_1_weight);
        /*
            1.2. Call fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights with not NULL pointers and link from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weightGet, 
                                                                      &uc_queue_1_weightGet, 
                                                                      &mc_queue_0_weightGet, 
                                                                      &mc_queue_1_weightGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights: %d, %d", device_id, link);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_0_weight, uc_queue_0_weightGet, 
                                     "got another uc_queue_0_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_1_weight, uc_queue_1_weightGet, 
                                     "got another uc_queue_1_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_0_weight, mc_queue_0_weightGet, 
                                     "got another mc_queue_0_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_1_weight, mc_queue_1_weightGet, 
                                     "got another mc_queue_1_weight than was set: %d", device_id);

        /* iterate with link = 1 */
        link = 1;
        uc_queue_0_weight = 254; 
        uc_queue_1_weight = 254; 
        mc_queue_0_weight = 254; 
        mc_queue_1_weight = 254; 

        result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                      uc_queue_0_weight, 
                                                                      uc_queue_1_weight, 
                                                                      mc_queue_0_weight, 
                                                                      mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL6_PARAM_MAC(SAND_OK, rc, device_id, link, 
                                                 uc_queue_0_weight, 
                                                 uc_queue_1_weight, 
                                                 mc_queue_0_weight, 
                                                 mc_queue_1_weight);
        /*
            1.2. Call fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights with not NULL pointers and link from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weightGet, 
                                                                      &uc_queue_1_weightGet, 
                                                                      &mc_queue_0_weightGet, 
                                                                      &mc_queue_1_weightGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights: %d, %d", device_id, link);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_0_weight, uc_queue_0_weightGet, 
                                     "got another uc_queue_0_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_queue_1_weight, uc_queue_1_weightGet, 
                                     "got another uc_queue_1_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_0_weight, mc_queue_0_weightGet, 
                                     "got another mc_queue_0_weight than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_queue_1_weight, mc_queue_1_weightGet, 
                                     "got another mc_queue_1_weight than was set: %d", device_id);

        /*
            1.3. Call with link [2] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                      uc_queue_0_weight, 
                                                                      uc_queue_1_weight, 
                                                                      mc_queue_0_weight, 
                                                                      mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        link = 0;

        /*
            1.4. Call with uc_queue_0_weight [255] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_queue_0_weight = 255;

        result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                      uc_queue_0_weight, 
                                                                      uc_queue_1_weight, 
                                                                      mc_queue_0_weight, 
                                                                      mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, uc_queue_0_weight = %d", 
                                         device_id, uc_queue_0_weight);

        uc_queue_0_weight = 0;

        /*
            1.5. Call with uc_queue_1_weight [255] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_queue_1_weight = 255;

        result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                      uc_queue_0_weight, 
                                                                      uc_queue_1_weight, 
                                                                      mc_queue_0_weight, 
                                                                      mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, uc_queue_1_weight = %d", 
                                         device_id, uc_queue_1_weight);

        uc_queue_1_weight = 0;

        /*
            1.6. Call with mc_queue_0_weight [255] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        mc_queue_0_weight = 255;

        result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                      uc_queue_0_weight, 
                                                                      uc_queue_1_weight, 
                                                                      mc_queue_0_weight, 
                                                                      mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, mc_queue_0_weight = %d", 
                                         device_id, mc_queue_0_weight);

        mc_queue_0_weight = 0;

        /*
            1.7. Call with mc_queue_1_weight [255] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        mc_queue_1_weight = 255;

        result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                      uc_queue_0_weight, 
                                                                      uc_queue_1_weight, 
                                                                      mc_queue_0_weight, 
                                                                      mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, mc_queue_1_weight = %d", 
                                         device_id, mc_queue_1_weight);
    }

    link = 0;

    uc_queue_0_weight = 0;
    uc_queue_1_weight = 0;
    mc_queue_0_weight = 0;
    mc_queue_1_weight = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                      uc_queue_0_weight, 
                                                                      uc_queue_1_weight, 
                                                                      mc_queue_0_weight, 
                                                                      mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                                                  uc_queue_0_weight, 
                                                                  uc_queue_1_weight, 
                                                                  mc_queue_0_weight, 
                                                                  mc_queue_1_weight);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       link,
    SAND_OUT    unsigned long       *uc_queue_0_weight_ptr,
    SAND_OUT    unsigned long       *uc_queue_1_weight_ptr,
    SAND_OUT    unsigned long       *mc_queue_0_weight_ptr,
    SAND_OUT    unsigned long       *mc_queue_1_weight_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights)
{
/*
    ITERATE_DEVICES
    1.1. Call with link [0 / 1] and non-null pointers.
    Expected: SAND_OK.
    1.2. Call with link [2] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with uc_queue_0_weight_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with uc_queue_1_weight_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with mc_queue_0_weight_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.6. Call with mc_queue_1_weight_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link = 0;
    SAND_U32   uc_queue_0_weight = 0;
    SAND_U32   uc_queue_1_weight = 0;
    SAND_U32   mc_queue_0_weight = 0;
    SAND_U32   mc_queue_1_weight = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link [0 / 1] and non-null pointers.
            Expected: SAND_OK.
        */
        /* iterate with link = 0 */
        link = 0;

        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weight, 
                                                                      &uc_queue_1_weight, 
                                                                      &mc_queue_0_weight, 
                                                                      &mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        /* iterate with link = 1 */
        link = 1;

        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weight, 
                                                                      &uc_queue_1_weight, 
                                                                      &mc_queue_0_weight, 
                                                                      &mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        /*
            1.2. Call with link [2] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weight, 
                                                                      &uc_queue_1_weight, 
                                                                      &mc_queue_0_weight, 
                                                                      &mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link);

        link = 0;

        /*
            1.3. Call with uc_queue_0_weight_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      NULL, 
                                                                      &uc_queue_1_weight, 
                                                                      &mc_queue_0_weight, 
                                                                      &mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uc_queue_0_weight = NULL", device_id);

        /*
            1.4. Call with uc_queue_1_weight_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weight, 
                                                                      NULL, 
                                                                      &mc_queue_0_weight, 
                                                                      &mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uc_queue_1_weight = NULL", device_id);

        /*
            1.5. Call with mc_queue_0_weight_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weight, 
                                                                      &uc_queue_1_weight, 
                                                                      NULL, 
                                                                      &mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mc_queue_0_weight = NULL", device_id);

        /*
            1.6. Call with mc_queue_1_weight_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weight, 
                                                                      &uc_queue_1_weight, 
                                                                      &mc_queue_0_weight, 
                                                                      NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mc_queue_1_weight = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                      &uc_queue_0_weight, 
                                                                      &uc_queue_1_weight, 
                                                                      &mc_queue_0_weight, 
                                                                      &mc_queue_1_weight);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                                                  &uc_queue_0_weight, 
                                                                  &uc_queue_1_weight, 
                                                                  &mc_queue_0_weight, 
                                                                  &mc_queue_1_weight);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_scheduler suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_scheduler)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_set_mtu_size_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_get_mtu_size_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_get_link_level_2_weight)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_set_link_level_2_scheduler_type)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_get_link_level_2_scheduler_type)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_set_link_queue_scheduling_type)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_get_link_queue_scheduling_type)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights)
UTF_SUIT_END_TESTS_MAC(fx950_api_scheduler)


