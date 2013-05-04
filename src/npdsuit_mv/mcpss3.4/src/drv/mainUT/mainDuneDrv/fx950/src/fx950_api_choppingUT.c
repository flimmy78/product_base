/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_choppingUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_chopping, that provides
*       ingress chopping process API of 98FX950 device driver
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_chopping.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_CHOPPING_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_set_max_chunk_sizes(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   max_chunk_size,
    SAND_IN     unsigned long   max_last_chunk_size
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_set_max_chunk_sizes)
{
/*
    ITERATE_DEVICES
    1.1. Call with max_chunk_size[1 / 31], max_last_chunk_size [1 / 127]
    Expected: SAND_OK.
    1.2. Call fx950_api_chopping_get_max_chunk_sizes with not NULL pointer
    Expected: SAND_OK ad the same params as was set.
    1.3. Call with max_chunk_size [0 / 32] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with max_last_chunk_size [0 / 128] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id              = 0;
    SAND_U32  max_chunk_size         = 0;
    SAND_U32  max_last_chunk_size    = 0;
    SAND_U32  max_chunk_sizeGet      = 0;
    SAND_U32  max_last_chunk_sizeGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with max_chunk_size[1 / 31], max_last_chunk_size [1 / 127]
            Expected: SAND_OK.
        */
        /* iterate with max_chunk_size = 1 */
        max_chunk_size = 1;
        max_last_chunk_size = 1;

        result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, max_chunk_size, max_last_chunk_size);

        /*
            1.2. Call fx950_api_chopping_get_max_chunk_sizes with not NULL pointer
            Expected: SAND_OK ad the same params as was set.
        */
        result = fx950_api_chopping_get_max_chunk_sizes(device_id, &max_chunk_sizeGet, &max_last_chunk_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chopping_get_max_chunk_sizes: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(max_chunk_size, max_chunk_sizeGet,
                   "got another max_chunk_size than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(max_last_chunk_size, max_last_chunk_sizeGet,
                   "got another max_chunk_size than was set: %d", device_id);

        /* iterate with max_chunk_size = 31 */
        max_chunk_size = 31;
        max_last_chunk_size = 127;

        result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, max_chunk_size, max_last_chunk_size);

        /*
            1.2. Call fx950_api_chopping_get_max_chunk_sizes with not NULL pointer
            Expected: SAND_OK ad the same params as was set.
        */
        result = fx950_api_chopping_get_max_chunk_sizes(device_id, &max_chunk_sizeGet, &max_last_chunk_sizeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chopping_get_max_chunk_sizes: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(max_chunk_size, max_chunk_sizeGet,
                   "got another max_chunk_size than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(max_last_chunk_size, max_last_chunk_sizeGet,
                   "got another max_chunk_size than was set: %d", device_id);

        /*
            1.3. Call with max_chunk_size [0 / 32] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        /* iterate with max_chunk_size = 0 */
        max_chunk_size = 0;

        result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, max_chunk_size = %d", device_id, max_chunk_size);

        /* iterate with max_chunk_size = 32 */
        max_chunk_size = 32;

        result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, max_chunk_size = %d", device_id, max_chunk_size);

        max_chunk_size = 1;

        /*
            1.4. Call with max_last_chunk_size [0 / 128] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        /* iterate with max_last_chunk_size = 0 */
        max_last_chunk_size = 0;

        result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, max_last_chunk_size = %d", device_id, max_last_chunk_size);

        /* iterate with max_last_chunk_size = 32 */
        max_last_chunk_size = 128;

        result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, max_last_chunk_size = %d", device_id, max_last_chunk_size);
    }

    max_chunk_size = 1;
    max_last_chunk_size = 1;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_get_max_chunk_sizes(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    unsigned long    *max_chunk_size_ptr,
    SAND_OUT    unsigned long    *max_last_chunk_size_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_get_max_chunk_sizes)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL max_chunk_size_ptr and not NULL max_last_chunk_size_ptr.
    Expected: SAND_OK.
    1.2. Call with max_chunk_size_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with max_last_chunk_size_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  max_chunk_size = 0;
    SAND_U32  max_last_chunk_size = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL max_chunk_size_ptr and not NULL max_last_chunk_size_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_chopping_get_max_chunk_sizes(device_id, &max_chunk_size, &max_last_chunk_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with max_chunk_size_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chopping_get_max_chunk_sizes(device_id, NULL, &max_last_chunk_size);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, max_chunk_size = NULL", device_id);

        /*
            1.3. Call with max_last_chunk_size_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chopping_get_max_chunk_sizes(device_id, &max_chunk_size, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, max_chunk_size = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_get_max_chunk_sizes(device_id, &max_chunk_size, &max_last_chunk_size);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_get_max_chunk_sizes(device_id, &max_chunk_size, &max_last_chunk_size);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_set_enable_params(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     FX950_CHOPPING_ENABLE_PARAMS_STRUCT     *chop_en_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_set_enable_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with chop_en_params { ucChopEn [TRUE / FALSE],
                                    mcChopEn [TRUE / FALSE],
                                    maxQueueNum [0 / 0xFFF] }
    Expected: SAND_OK.
    1.2. Call fx950_api_chopping_get_enable_params with not NULL pointer.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with chop_en_params->maxQueueNum [0x1000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with chop_en_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FX950_CHOPPING_ENABLE_PARAMS_STRUCT chop_en_params;
    FX950_CHOPPING_ENABLE_PARAMS_STRUCT chop_en_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with chop_en_params { ucChopEn [TRUE / FALSE],
                                            mcChopEn [TRUE / FALSE],
                                            maxQueueNum [0 / 0xFFF] }
            Expected: SAND_OK.
        */
        /* iterate with ucChopEn = TRUE */
        chop_en_params.ucChopEn = TRUE;
        chop_en_params.mcChopEn = TRUE;
        chop_en_params.maxQueueNum = 0;

        result = fx950_api_chopping_set_enable_params(device_id, &chop_en_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_chopping_get_enable_params with not NULL pointer.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_chopping_get_enable_params(device_id, &chop_en_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chopping_get_enable_params: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_params.ucChopEn, chop_en_paramsGet.ucChopEn,
                                     "got another chop_en_params.ucChopEn than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_params.mcChopEn, chop_en_paramsGet.mcChopEn,
                                     "got another chop_en_params.mcChopEn than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_params.maxQueueNum, chop_en_paramsGet.maxQueueNum,
                                     "got another chop_en_params.maxQueueNum than was set: %d", device_id);

        /* iterate with ucChopEn = FALSE */
        chop_en_params.ucChopEn = FALSE;
        chop_en_params.mcChopEn = FALSE;
        chop_en_params.maxQueueNum = 0xFFF;

        result = fx950_api_chopping_set_enable_params(device_id, &chop_en_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_chopping_get_enable_params with not NULL pointer.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_chopping_get_enable_params(device_id, &chop_en_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chopping_get_enable_params: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_params.ucChopEn, chop_en_paramsGet.ucChopEn,
                                     "got another chop_en_params.ucChopEn than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_params.mcChopEn, chop_en_paramsGet.mcChopEn,
                                     "got another chop_en_params.mcChopEn than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_params.maxQueueNum, chop_en_paramsGet.maxQueueNum,
                                     "got another chop_en_params.maxQueueNum than was set: %d", device_id);

        /*
            1.3. Call with chop_en_params->maxQueueNum [0x1000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        chop_en_params.maxQueueNum = 0x1000;

        result = fx950_api_chopping_set_enable_params(device_id, &chop_en_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, chop_en_params->maxQueueNum = %d",
                                        device_id, chop_en_params.maxQueueNum);

        chop_en_params.maxQueueNum = 0;

        /*
            1.4. Call with chop_en_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chopping_set_enable_params(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, chop_en_params_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_set_enable_params(device_id, &chop_en_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_set_enable_params(device_id, &chop_en_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_get_enable_params(
    SAND_IN     unsigned int                            device_id,
    SAND_OUT    FX950_CHOPPING_ENABLE_PARAMS_STRUCT     *chop_en_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_get_enable_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL chop_en_params_ptr.
    Expected: SAND_OK.
    1.2. Call with chop_en_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FX950_CHOPPING_ENABLE_PARAMS_STRUCT chop_en_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL chop_en_params_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_chopping_get_enable_params(device_id, &chop_en_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with chop_en_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chopping_get_enable_params(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, chop_en_params_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_get_enable_params(device_id, &chop_en_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_get_enable_params(device_id, &chop_en_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_set_enable_target_dev_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_IN     FX950_BMP_128   *chop_en_dev_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_set_enable_target_dev_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with chop_en_dev_bmp { bitmap[0,0,0,0,0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF] }.
    Expected: SAND_OK.
    1.2. Call fx950_api_chopping_get_enable_target_dev_bmp with not NULL pointers.
    expected: SAND_OK and the same params as was set.
    1.3. Call with chop_en_dev_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT     device_id = 0;
    FX950_BMP_128 chop_en_dev_bmp;
    FX950_BMP_128 chop_en_dev_bmpGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with chop_en_dev_bmp { bitmap[0,0,0,0,0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF] }.
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        chop_en_dev_bmp.bitmap[0] = 0;
        chop_en_dev_bmp.bitmap[1] = 0;
        chop_en_dev_bmp.bitmap[2] = 0;
        chop_en_dev_bmp.bitmap[3] = 0;

        result = fx950_api_chopping_set_enable_target_dev_bmp(device_id, &chop_en_dev_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_chopping_get_enable_target_dev_bmp with not NULL pointers.
            expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_chopping_get_enable_target_dev_bmp(device_id, &chop_en_dev_bmpGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chopping_get_enable_target_dev_bmp: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_dev_bmp.bitmap[0], chop_en_dev_bmpGet.bitmap[0],
                                     "got another chop_en_dev_bmp.bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_dev_bmp.bitmap[1], chop_en_dev_bmpGet.bitmap[1],
                                     "got another chop_en_dev_bmp.bitmap[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_dev_bmp.bitmap[2], chop_en_dev_bmpGet.bitmap[2],
                                     "got another chop_en_dev_bmp.bitmap[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_dev_bmp.bitmap[3], chop_en_dev_bmpGet.bitmap[3],
                                     "got another chop_en_dev_bmp.bitmap[3] than was set: %d", device_id);

        /* iterate with 0xFFFFFFFF */
        chop_en_dev_bmp.bitmap[0] = 0xFFFFFFFF;
        chop_en_dev_bmp.bitmap[1] = 0xFFFFFFFF;
        chop_en_dev_bmp.bitmap[2] = 0xFFFFFFFF;
        chop_en_dev_bmp.bitmap[3] = 0xFFFFFFFF;

        result = fx950_api_chopping_set_enable_target_dev_bmp(device_id, &chop_en_dev_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_chopping_get_enable_target_dev_bmp with not NULL pointers.
            expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_chopping_get_enable_target_dev_bmp(device_id, &chop_en_dev_bmpGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chopping_get_enable_target_dev_bmp: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_dev_bmp.bitmap[0], chop_en_dev_bmpGet.bitmap[0],
                                     "got another chop_en_dev_bmp.bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_dev_bmp.bitmap[1], chop_en_dev_bmpGet.bitmap[1],
                                     "got another chop_en_dev_bmp.bitmap[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_dev_bmp.bitmap[2], chop_en_dev_bmpGet.bitmap[2],
                                     "got another chop_en_dev_bmp.bitmap[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(chop_en_dev_bmp.bitmap[3], chop_en_dev_bmpGet.bitmap[3],
                                     "got another chop_en_dev_bmp.bitmap[3] than was set: %d", device_id);
        /*
            1.3. Call with chop_en_dev_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chopping_set_enable_target_dev_bmp(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, chop_en_dev_bmp = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_set_enable_target_dev_bmp(device_id, &chop_en_dev_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_set_enable_target_dev_bmp(device_id, &chop_en_dev_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_get_enable_target_dev_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    FX950_BMP_128   *chop_en_dev_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_get_enable_target_dev_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL chop_en_dev_bmp_ptr.
    Expected: SAND_OK.
    1.2. Call with chop_en_dev_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT     device_id = 0;
    FX950_BMP_128 chop_en_dev_bmp;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL chop_en_dev_bmp_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_chopping_get_enable_target_dev_bmp(device_id, &chop_en_dev_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with chop_en_dev_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chopping_get_enable_target_dev_bmp(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, chop_en_dev_bmp_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_get_enable_target_dev_bmp(device_id, &chop_en_dev_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_get_enable_target_dev_bmp(device_id, &chop_en_dev_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_set_enable_target_dev(
    SAND_IN     unsigned int     device_id,
    SAND_IN     unsigned long    target_dev_num,
    SAND_IN     unsigned int     chop_en
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_set_enable_target_dev)
{
/*
    ITERATE_DEVICES
    1.1. Call with target_dev_num[0 / 127], chop_en[TRUE / FALSE].
    Expected: SAND_OK.
    1.2. Call target_dev_num [FX950_NOF_DESTINATION_DEVICES] and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  target_dev_num = 0;
    SAND_UINT chop_en = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with target_dev_num[0 / 127], chop_en[TRUE / FALSE].
            Expected: SAND_OK.
        */

        /* iterate with target_dev_num = 0, chop_en = TRUE */
        target_dev_num = 0;
        chop_en = TRUE;

        result = fx950_api_chopping_set_enable_target_dev(device_id, target_dev_num, chop_en);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, target_dev_num, chop_en);

        /* iterate with target_dev_num = 127, chop_en = FALSE */
        target_dev_num = 127;
        chop_en = FALSE;

        result = fx950_api_chopping_set_enable_target_dev(device_id, target_dev_num, chop_en);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, target_dev_num, chop_en);

        /*
            1.2. Call target_dev_num [FX950_NOF_DESTINATION_DEVICES] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        target_dev_num = FX950_NOF_DESTINATION_DEVICES;

        result = fx950_api_chopping_set_enable_target_dev(device_id, target_dev_num, chop_en);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, target_dev_num);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_set_enable_target_dev(device_id, target_dev_num, chop_en);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_set_enable_target_dev(device_id, target_dev_num, chop_en);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_set_enable_tc(
    SAND_IN     unsigned int        device_id,
    SAND_IN     FX950_PACKETS_TYPE  packet_type,
    SAND_IN     unsigned int        tc_chop_bmp
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_set_enable_tc)
{
/*
    ITERATE_DEVICES
    1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST], 
                   tc_chop_bmp [0 / 0xFF].
    Expected: SAND_OK.
    1.2. Call fx950_api_chopping_get_enable_tc with not NULL tc_chop_bmp_ptr and other params from 1.1.
    Expected: SAND_OK and the same params from 1.1.
    1.3. Call with packet_type [0x5AAAAAA5] and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT          device_id = 0;
    FX950_PACKETS_TYPE packet_type = FX950_PACKETS_UNICAST;
    SAND_UINT          tc_chop_bmp = 0;
    SAND_UINT          tc_chop_bmpGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST], 
                           tc_chop_bmp [0 / 0xFF].
            Expected: SAND_OK.
        */
        /* iterate with FX950_PACKETS_UNICAST */
        packet_type = FX950_PACKETS_UNICAST;
        tc_chop_bmp = 0;

        result = fx950_api_chopping_set_enable_tc(device_id, packet_type, tc_chop_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, packet_type, tc_chop_bmp);

        /*
            1.2. Call fx950_api_chopping_get_enable_tc with not NULL tc_chop_bmp_ptr and other params from 1.1.
            Expected: SAND_OK and the same params from 1.1.
        */
        result = fx950_api_chopping_get_enable_tc(device_id, packet_type, &tc_chop_bmpGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chopping_get_enable_tc: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(tc_chop_bmp, tc_chop_bmpGet, "got another tc_chop_bmp than was set: %d", device_id);

        /* iterate with FX950_PACKETS_MULTICAST */
        packet_type = FX950_PACKETS_MULTICAST;
        tc_chop_bmp = 0xFF;

        result = fx950_api_chopping_set_enable_tc(device_id, packet_type, tc_chop_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, packet_type, tc_chop_bmp);

        /*
            1.2. Call fx950_api_chopping_get_enable_tc with not NULL tc_chop_bmp_ptr and other params from 1.1.
            Expected: SAND_OK and the same params from 1.1.
        */
        result = fx950_api_chopping_get_enable_tc(device_id, packet_type, &tc_chop_bmpGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chopping_get_enable_tc: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(tc_chop_bmp, tc_chop_bmpGet, "got another tc_chop_bmp than was set: %d", device_id);

        /*
            1.3. Call with packet_type [0x5AAAAAA5] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        packet_type = SAND_UTF_CHOPPING_INVALID_ENUM;

        result = fx950_api_chopping_set_enable_tc(device_id, packet_type, tc_chop_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);
    }

    packet_type = FX950_PACKETS_UNICAST;
    tc_chop_bmp = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_set_enable_tc(device_id, packet_type, tc_chop_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_set_enable_tc(device_id, packet_type, tc_chop_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_chopping_get_enable_tc(
    SAND_IN     unsigned int        device_id,
    SAND_IN     FX950_PACKETS_TYPE  packet_type,
    SAND_OUT    unsigned int        *tc_chop_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_chopping_get_enable_tc)
{
/*
    ITERATE_DEVICES
    1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST], 
                   and not NULL tc_chop_bmp_ptr.
    Expected: SAND_OK.
    1.2. Call with packet_type [0x5AAAAAA5] and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with tc_chop_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT          device_id = 0;
    FX950_PACKETS_TYPE packet_type = FX950_PACKETS_UNICAST;
    SAND_UINT          tc_chop_bmp = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST], 
                           and not NULL tc_chop_bmp_ptr.
            Expected: SAND_OK.
        */
        /* iterate with FX950_PACKETS_UNICAST */
        packet_type = FX950_PACKETS_UNICAST;

        result = fx950_api_chopping_get_enable_tc(device_id, packet_type, &tc_chop_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        /* iterate with FX950_PACKETS_MULTICAST */
        packet_type = FX950_PACKETS_MULTICAST;

        result = fx950_api_chopping_get_enable_tc(device_id, packet_type, &tc_chop_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        /*
            1.2. Call with packet_type [0x5AAAAAA5] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        packet_type = SAND_UTF_CHOPPING_INVALID_ENUM;

        result = fx950_api_chopping_get_enable_tc(device_id, packet_type, &tc_chop_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        packet_type = FX950_PACKETS_UNICAST;

        /*
            1.3. Call with tc_chop_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_chopping_get_enable_tc(device_id, packet_type, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, tc_chop_bmp_ptr = NULL", device_id);
    }

    packet_type = FX950_PACKETS_UNICAST;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_chopping_get_enable_tc(device_id, packet_type, &tc_chop_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_chopping_get_enable_tc(device_id, packet_type, &tc_chop_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_chopping suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_chopping)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_set_max_chunk_sizes)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_get_max_chunk_sizes)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_set_enable_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_get_enable_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_set_enable_target_dev_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_get_enable_target_dev_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_set_enable_target_dev)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_set_enable_tc)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_chopping_get_enable_tc)
UTF_SUIT_END_TESTS_MAC(fx950_api_chopping)

