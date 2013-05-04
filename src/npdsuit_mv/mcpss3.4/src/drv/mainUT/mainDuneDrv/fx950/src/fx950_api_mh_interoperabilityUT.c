/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_mh_interoperabilityUT.
*
* DESCRIPTION:
*       Unit tests for fx950_api_mh_interoperability, that provides
*       Marvell Header Interoperability API of 98FX950 device driver
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_mh_interoperability.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_MH_INTEROPERABILITY_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_mh_interoperability_set_mc_dest_mode(
    SAND_IN     unsigned int            device_id,
    SAND_IN     FX950_MH_FORMAT_TYPE    mh_format_type
  )
*/
UTF_TEST_CASE_MAC(fx950_api_mh_interoperability_set_mc_dest_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with mh_format_type[FX950_MH_FORMAT_STANDART / FX950_MH_FORMAT_EXTENDED].
    Expected: SAND_OK.
    1.2. Call fx950_api_mh_interoperability_get_mc_dest_mode with not NULL mh_format_type_ptr.
    Expected: SAND_OK ad the same mh_format_type as was set.
    1.3. Call with mh_format_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT            device_id         = 0;
    FX950_MH_FORMAT_TYPE mh_format_type    = FX950_MH_FORMAT_STANDART;
    FX950_MH_FORMAT_TYPE mh_format_typeGet = FX950_MH_FORMAT_STANDART;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with mh_format_type[FX950_MH_FORMAT_STANDART / FX950_MH_FORMAT_EXTENDED].
            Expected: SAND_OK.
        */
        /* iterate with mh_format_type = FX950_MH_FORMAT_STANDART */
        mh_format_type = FX950_MH_FORMAT_STANDART;

        result = fx950_api_mh_interoperability_set_mc_dest_mode(device_id, mh_format_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mh_format_type);

        /*
            1.2. Call fx950_api_mh_interoperability_get_mc_dest_mode with not NULL mh_format_type_ptr.
            Expected: SAND_OK ad the same params as was set.
        */
        result = fx950_api_mh_interoperability_get_mc_dest_mode(device_id, &mh_format_typeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_mh_interoperability_get_mc_dest_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type, mh_format_typeGet,
                   "got another mh_format_type than was set: %d", device_id);

        /* iterate with mh_format_type = FX950_MH_FORMAT_EXTENDED */
        mh_format_type = FX950_MH_FORMAT_EXTENDED;

        result = fx950_api_mh_interoperability_set_mc_dest_mode(device_id, mh_format_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mh_format_type);

        /*
            1.2. Call fx950_api_mh_interoperability_get_mc_dest_mode with not NULL mh_format_type_ptr.
            Expected: SAND_OK ad the same params as was set.
        */
        result = fx950_api_mh_interoperability_get_mc_dest_mode(device_id, &mh_format_typeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_mh_interoperability_get_mc_dest_mode: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type, mh_format_typeGet,
                   "got another mh_format_type than was set: %d", device_id);
        
        /*
            1.3. Call with mh_format_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        mh_format_type = SAND_UTF_MH_INTEROPERABILITY_INVALID_ENUM;

        result = fx950_api_mh_interoperability_set_mc_dest_mode(device_id, mh_format_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mh_format_type);
    }

    mh_format_type = FX950_MH_FORMAT_STANDART;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_mh_interoperability_set_mc_dest_mode(device_id, mh_format_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_mh_interoperability_set_mc_dest_mode(device_id, mh_format_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_mh_interoperability_get_mc_dest_mode(
    SAND_IN     unsigned int           device_id,
    SAND_OUT    FX950_MH_FORMAT_TYPE   *mh_format_type_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_mh_interoperability_get_mc_dest_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL mh_format_type_ptr.
    Expected: SAND_OK.
    1.2. Call with mh_format_type_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT            device_id      = 0;
    FX950_MH_FORMAT_TYPE mh_format_type = FX950_MH_FORMAT_STANDART;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL mh_format_type_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_mh_interoperability_get_mc_dest_mode(device_id, &mh_format_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with mh_format_type_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_mh_interoperability_get_mc_dest_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mh_format_type_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_mh_interoperability_get_mc_dest_mode(device_id, &mh_format_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_mh_interoperability_get_mc_dest_mode(device_id, &mh_format_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_mh_interoperability_set_uc_dest_mode(
    SAND_IN     unsigned int    device_id,
    SAND_IN     FX950_BMP_128   *mh_format_type_vector_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_mh_interoperability_set_uc_dest_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with mh_format_type_vector_ptr->bitmap {0, 0, 0, 0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
    Expected: SAND_OK.
    1.2. Call fx950_api_mh_interoperability_get_uc_dest_mode with not NULL pointer.
    Expected: SAND_OK and the same mh_format_type_vector as was set.
    1.3. Call with mh_format_type_vector_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT     device_id = 0;
    FX950_BMP_128 mh_format_type_vector;
    FX950_BMP_128 mh_format_type_vectorGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with mh_format_type_vector_ptr->bitmap {0, 0, 0, 0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
            Expected: SAND_OK.
        */
        /* iterate with mh_format_type_vector_ptr->bitmap {0, 0, 0, 0} */
        mh_format_type_vector.bitmap[0] = 0;
        mh_format_type_vector.bitmap[1] = 0;
        mh_format_type_vector.bitmap[2] = 0;
        mh_format_type_vector.bitmap[3] = 0;

        result = fx950_api_mh_interoperability_set_uc_dest_mode(device_id, &mh_format_type_vector);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_mh_interoperability_get_uc_dest_mode with not NULL pointer.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_mh_interoperability_get_uc_dest_mode(device_id, &mh_format_type_vectorGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_mh_interoperability_get_uc_dest_mode: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type_vector.bitmap[0], mh_format_type_vectorGet.bitmap[0],
                                     "got another mh_format_type_vector_ptr->bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type_vector.bitmap[1], mh_format_type_vectorGet.bitmap[1],
                                     "got another mh_format_type_vector_ptr->bitmap[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type_vector.bitmap[2], mh_format_type_vectorGet.bitmap[2],
                                     "got another mh_format_type_vector_ptr->bitmap[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type_vector.bitmap[3], mh_format_type_vectorGet.bitmap[3],
                                     "got another mh_format_type_vector_ptr->bitmap[3] than was set: %d", device_id);

        /* iterate with mh_format_type_vector_ptr->bitmap {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF} */
        mh_format_type_vector.bitmap[0] = 0xFFFFFFFF;
        mh_format_type_vector.bitmap[1] = 0xFFFFFFFF;
        mh_format_type_vector.bitmap[2] = 0xFFFFFFFF;
        mh_format_type_vector.bitmap[3] = 0xFFFFFFFF;

        result = fx950_api_mh_interoperability_set_uc_dest_mode(device_id, &mh_format_type_vector);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_mh_interoperability_get_uc_dest_mode with not NULL pointer.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_mh_interoperability_get_uc_dest_mode(device_id, &mh_format_type_vectorGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_mh_interoperability_get_uc_dest_mode: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type_vector.bitmap[0], mh_format_type_vectorGet.bitmap[0],
                                     "got another mh_format_type_vector_ptr->bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type_vector.bitmap[1], mh_format_type_vectorGet.bitmap[1],
                                     "got another mh_format_type_vector_ptr->bitmap[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type_vector.bitmap[2], mh_format_type_vectorGet.bitmap[2],
                                     "got another mh_format_type_vector_ptr->bitmap[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mh_format_type_vector.bitmap[3], mh_format_type_vectorGet.bitmap[3],
                                     "got another mh_format_type_vector_ptr->bitmap[3] than was set: %d", device_id);

        /*
            1.3. Call with mh_format_type_vector_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_mh_interoperability_set_uc_dest_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mh_format_type_vector_ptr = NULL", device_id);
    }

    mh_format_type_vector.bitmap[0] = 0;
    mh_format_type_vector.bitmap[1] = 0;
    mh_format_type_vector.bitmap[2] = 0;
    mh_format_type_vector.bitmap[3] = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_mh_interoperability_set_uc_dest_mode(device_id, &mh_format_type_vector);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_mh_interoperability_set_uc_dest_mode(device_id, &mh_format_type_vector);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_mh_interoperability_get_uc_dest_mode(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    FX950_BMP_128   *mh_format_type_vector_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_mh_interoperability_get_uc_dest_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL mh_format_type_vector_ptr.
    Expected: SAND_OK.
    1.2. Call with mh_format_type_vector_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT     device_id = 0;
    FX950_BMP_128 mh_format_type_vector;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL mh_format_type_vector_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_mh_interoperability_get_uc_dest_mode(device_id, &mh_format_type_vector);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with mh_format_type_vector_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_mh_interoperability_get_uc_dest_mode(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mh_format_type_vector_ptr = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_mh_interoperability_get_uc_dest_mode(device_id, &mh_format_type_vector);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_mh_interoperability_get_uc_dest_mode(device_id, &mh_format_type_vector);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_mh_interoperability_set_uc_dest_device_mode(
    SAND_IN     unsigned int           device_id,
    SAND_IN     unsigned int           dest_device_id,
    SAND_IN     FX950_MH_FORMAT_TYPE   mh_format_type  
  )
*/
UTF_TEST_CASE_MAC(fx950_api_mh_interoperability_set_uc_dest_device_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with dest_device_id [0 / SAND_MAX_DEVICE - 1],
                   mh_format_type [FX950_MH_FORMAT_STANDART / FX950_MH_FORMAT_EXTENDED].
    Expected: SAND_OK.
    1.2. Call with dest_device_id [FX950_NOF_DESTINATION_DEVICES] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with mh_format_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT            device_id = 0;
    SAND_UINT            dest_device_id = 0;
    FX950_MH_FORMAT_TYPE mh_format_type = FX950_MH_FORMAT_STANDART;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with dest_device_id [0 / SAND_MAX_DEVICE - 1],
                           mh_format_type [FX950_MH_FORMAT_STANDART / FX950_MH_FORMAT_EXTENDED].
            Expected: SAND_OK.
        */
        /* iterate with dest_device_id = 0 */
        dest_device_id = 0;
        mh_format_type = FX950_MH_FORMAT_STANDART;

        result = fx950_api_mh_interoperability_set_uc_dest_device_mode(device_id, dest_device_id, mh_format_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, dest_device_id, mh_format_type);

        /* iterate with dest_device_id = SAND_MAX_DEVICE - 1 */
        dest_device_id = SAND_MAX_DEVICE - 1;
        mh_format_type = FX950_MH_FORMAT_EXTENDED;

        result = fx950_api_mh_interoperability_set_uc_dest_device_mode(device_id, dest_device_id, mh_format_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, dest_device_id, mh_format_type);

        /*
            1.2. Call with dest_device_id [FX950_NOF_DESTINATION_DEVICES] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        dest_device_id = FX950_NOF_DESTINATION_DEVICES;
        
        result = fx950_api_mh_interoperability_set_uc_dest_device_mode(device_id, dest_device_id, mh_format_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, dest_device_id);

        dest_device_id = 0;

        /*
            1.3. Call with mh_format_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        mh_format_type = SAND_UTF_MH_INTEROPERABILITY_INVALID_ENUM;

        result = fx950_api_mh_interoperability_set_uc_dest_device_mode(device_id, dest_device_id, mh_format_type);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, mh_format_type = %d",
                                        device_id, mh_format_type);
    }

    dest_device_id = 0;
    mh_format_type = FX950_MH_FORMAT_STANDART;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_mh_interoperability_set_uc_dest_device_mode(device_id, dest_device_id, mh_format_type);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_mh_interoperability_set_uc_dest_device_mode(device_id, dest_device_id, mh_format_type);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_mh_interoperability suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_mh_interoperability)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_mh_interoperability_set_mc_dest_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_mh_interoperability_get_mc_dest_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_mh_interoperability_set_uc_dest_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_mh_interoperability_get_uc_dest_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_mh_interoperability_set_uc_dest_device_mode)
UTF_SUIT_END_TESTS_MAC(fx950_api_mh_interoperability)


