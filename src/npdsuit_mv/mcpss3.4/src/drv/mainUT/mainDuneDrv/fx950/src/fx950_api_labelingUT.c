/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_labelingUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_labeling, that provides
*       labeling process API of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <FX950/include/fx950_chip_defines.h>
#include <FX950/include/fx950_api_labeling.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_LABELING_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_add_mc_header_enable(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned int    add_mc_header_enable
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_add_mc_header_enable)
{
/*
    ITERATE_DEVICES
    1.1. Call with add_mc_header_enable [TRUE / FALSE].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_add_mc_header_enable with not NULL add_mc_header_enable_ptr.
    Expected: SAND_OK and the same add_mc_header_enable as was set.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  add_mc_header_enable = 0;
    SAND_UINT  add_mc_header_enableGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with add_mc_header_enable [TRUE / FALSE].
            Expected: SAND_OK.
        */
        /* iterate with add_mc_header_enable = TRUE */
        add_mc_header_enable = TRUE;

        result = fx950_api_labeling_set_add_mc_header_enable(device_id, add_mc_header_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, add_mc_header_enable);

        /*
            1.2. Call fx950_api_labeling_get_add_mc_header_enable with not NULL add_mc_header_enable_ptr.
            Expected: SAND_OK and the same add_mc_header_enable as was set.
        */
        result = fx950_api_labeling_get_add_mc_header_enable(device_id, &add_mc_header_enableGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_add_mc_header_enable: %d", device_id);

        /*
            1.1. Call with add_mc_header_enable [TRUE / FALSE].
            Expected: SAND_OK.
        */
        /* iterate with add_mc_header_enable = FALSE */
        add_mc_header_enable = FALSE;

        result = fx950_api_labeling_set_add_mc_header_enable(device_id, add_mc_header_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, add_mc_header_enable);

        /*
            1.2. Call fx950_api_labeling_get_add_mc_header_enable with not NULL add_mc_header_enable_ptr.
            Expected: SAND_OK and the same add_mc_header_enable as was set.
        */
        result = fx950_api_labeling_get_add_mc_header_enable(device_id, &add_mc_header_enableGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_add_mc_header_enable: %d", device_id);
    }

    add_mc_header_enable = FALSE;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_add_mc_header_enable(device_id, add_mc_header_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_add_mc_header_enable(device_id, add_mc_header_enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_add_mc_header_enable(
    SAND_IN     unsigned int  device_id,
    SAND_OUT    unsigned int *add_mc_header_enable_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_add_mc_header_enable)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null add_mc_header_enable_ptr.
    Expected: SAND_OK.
    1.2. Call with add_mc_header_enable_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  add_mc_header_enable = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null add_mc_header_enable_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_labeling_get_add_mc_header_enable(device_id, &add_mc_header_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with add_mc_header_enable_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_add_mc_header_enable(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, add_mc_header_enable = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_add_mc_header_enable(device_id, &add_mc_header_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_add_mc_header_enable(device_id, &add_mc_header_enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_cpu_code_label_map(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   cpu_code,
    SAND_IN     unsigned long   voq
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_cpu_code_label_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with cpu_code[0 / 255], voq[0 / 0x2FFF].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_cpu_code_label_map with not NULL voq_ptr and cpu_code from 1.1.
    Expected: SAND_OK and the same voq as was set.
    1.3. Call with cpu_code [256] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with voq [0x3000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   cpu_code = 0;
    SAND_U32   voq = 0;
    SAND_U32   voqGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with cpu_code[0 / 255], voq[0 / 0x2FFF].
            Expected: SAND_OK.
        */
        /* iterate with cpu_code = 0 */
        cpu_code = 0;
        voq = 0;

        result = fx950_api_labeling_set_cpu_code_label_map(device_id, cpu_code, voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, cpu_code, voq);

        /*
            1.2. Call fx950_api_labeling_get_cpu_code_label_map with not NULL voq_ptr and cpu_code from 1.1.
            Expected: SAND_OK and the same voq as was set.
        */
        result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &voqGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_cpu_code_label_map: %d, %d", device_id, cpu_code);

        UTF_VERIFY_EQUAL1_STRING_MAC(voq, voqGet, "got another voq than was set: %d", device_id);

        /* iterate with cpu_code = 255 */
        cpu_code = 255;
        voq = 0x2FFF;

        result = fx950_api_labeling_set_cpu_code_label_map(device_id, cpu_code, voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, cpu_code, voq);

        /*
            1.2. Call fx950_api_labeling_get_cpu_code_label_map with not NULL voq_ptr and cpu_code from 1.1.
            Expected: SAND_OK and the same voq as was set.
        */
        result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &voqGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_cpu_code_label_map: %d, %d", device_id, cpu_code);

        UTF_VERIFY_EQUAL1_STRING_MAC(voq, voqGet, "got another voq than was set: %d", device_id);

        /*
            1.3. Call with cpu_code [256] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        cpu_code = 256;

        result = fx950_api_labeling_set_cpu_code_label_map(device_id, cpu_code, voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, cpu_code);

        cpu_code = 0;

        /*
            1.4. Call with voq [0x3000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        voq = 0x3000;

        result = fx950_api_labeling_set_cpu_code_label_map(device_id, cpu_code, voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, voq = %d", device_id, voq);
    }

    cpu_code = 0;
    voq = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_cpu_code_label_map(device_id, cpu_code, voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_cpu_code_label_map(device_id, cpu_code, voq);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_cpu_code_label_map(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   cpu_code,
    SAND_OUT    unsigned long   *voq_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_cpu_code_label_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with cpu_code[0 / 255], not NULL voq_ptr.
    Expected: SAND_OK.
    1.2. Call with cpu_code [256] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with voq_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   cpu_code = 0;
    SAND_U32   voq = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with cpu_code[0 / 255], not NULL voq_ptr.
            Expected: SAND_OK.
        */
        /* iterate with cpu_code = 0 */
        cpu_code = 0;

        result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, cpu_code);

        /* iterate with cpu_code = 255 */
        cpu_code = 255;

        result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, cpu_code);

        /*
            1.2. Call with cpu_code [256] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        cpu_code = 256;

        result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, cpu_code);

        cpu_code = 0;

        /*
            1.3. Call with voq_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, voq = NULL", device_id);
    }

    cpu_code = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &voq);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &voq);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_priority_map(
    SAND_IN     unsigned int        device_id,
    SAND_IN     FX950_PACKETS_TYPE  packet_type,
    SAND_IN     unsigned long       priority_new[FX950_LABELING_NOF_PRIORITIES]
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_priority_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST]
               and priority_new [0,0,0,0,..0 / 7,7,7,7,..7].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_priority_map with not NULL priority_new and other param from 1.1.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with packet_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with priority_new[0] = 8 (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with priority_new [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT           device_id   = 0;
    FX950_PACKETS_TYPE  packet_type = FX950_PACKETS_UNICAST;

    SAND_U32   priority_new[FX950_LABELING_NOF_PRIORITIES];
    SAND_U32   priority_newGet[FX950_LABELING_NOF_PRIORITIES];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST]
                       and priority_new [0,0,0,0,..0 / 7,7,7,7,..7].
            Expected: SAND_OK.
        */
        /* iterate with FX950_PACKETS_UNICAST */
        packet_type = FX950_PACKETS_UNICAST;

        priority_new[0] = 0;
        priority_new[1] = 0;
        priority_new[2] = 0;
        priority_new[3] = 0;
        priority_new[4] = 0;
        priority_new[5] = 0;
        priority_new[6] = 0;
        priority_new[7] = 0;

        result = fx950_api_labeling_set_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        /*
            1.2. Call fx950_api_labeling_get_priority_map with not NULL priority_new and other param from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_labeling_get_priority_map(device_id, packet_type, priority_newGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_priority_map: %d, %d", device_id, packet_type);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[0], priority_newGet[0],
                                     "got another priority_new[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[1], priority_newGet[1],
                                     "got another priority_new[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[2], priority_newGet[2],
                                     "got another priority_new[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[3], priority_newGet[3],
                                     "got another priority_new[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[4], priority_newGet[4],
                                     "got another priority_new[4] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[5], priority_newGet[5],
                                     "got another priority_new[5] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[6], priority_newGet[6],
                                     "got another priority_new[6] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[7], priority_newGet[7],
                                     "got another priority_new[7] than was set: %d", device_id);

        /* iterate with FX950_PACKETS_MULTICAST */
        packet_type = FX950_PACKETS_MULTICAST;

        priority_new[0] = 7;
        priority_new[1] = 7;
        priority_new[2] = 7;
        priority_new[3] = 7;
        priority_new[4] = 7;
        priority_new[5] = 7;
        priority_new[6] = 7;
        priority_new[7] = 7;

        result = fx950_api_labeling_set_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        /*
            1.2. Call fx950_api_labeling_get_priority_map with not NULL priority_new and other param from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_labeling_get_priority_map(device_id, packet_type, priority_newGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_priority_map: %d, %d", device_id, packet_type);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[0], priority_newGet[0],
                                     "got another priority_new[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[1], priority_newGet[1],
                                     "got another priority_new[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[2], priority_newGet[2],
                                     "got another priority_new[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[3], priority_newGet[3],
                                     "got another priority_new[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[4], priority_newGet[4],
                                     "got another priority_new[4] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[5], priority_newGet[5],
                                     "got another priority_new[5] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[6], priority_newGet[6],
                                     "got another priority_new[6] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority_new[7], priority_newGet[7],
                                     "got another priority_new[7] than was set: %d", device_id);

        /*
            1.3. Call with packet_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        packet_type = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_set_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        packet_type = FX950_PACKETS_MULTICAST;

        /*
            1.4. Call with priority_new[0] = 8 (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        priority_new[0] = 8;

        result = fx950_api_labeling_set_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, priority_new[0] = %d",
                                        device_id, priority_new[0]);

        priority_new[0] = 0;

        /*
            1.5. Call with priority_new [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_set_priority_map(device_id, packet_type, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, priority_new = NULL", device_id);
    }

    packet_type = FX950_PACKETS_UNICAST;

    priority_new[0] = 0;
    priority_new[1] = 0;
    priority_new[2] = 0;
    priority_new[3] = 0;
    priority_new[4] = 0;
    priority_new[5] = 0;
    priority_new[6] = 0;
    priority_new[7] = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_priority_map(device_id, packet_type, priority_new);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_priority_map(
    SAND_IN     unsigned int        device_id,
    SAND_IN     FX950_PACKETS_TYPE  packet_type,
    SAND_OUT    unsigned long       priority_new[FX950_LABELING_NOF_PRIORITIES]
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_priority_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST]
                   and not NULL priority_new.
    Expected: SAND_OK.
    1.2. Call with packet_type [0x5AAAAAA5] and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with priority_new [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT           device_id   = 0;
    FX950_PACKETS_TYPE  packet_type = FX950_PACKETS_UNICAST;
    SAND_U32            priority_new[FX950_LABELING_NOF_PRIORITIES];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST]
                       and not NULL priority_new.
            Expected: SAND_OK.
        */
        /* iterate with FX950_PACKETS_UNICAST */
        packet_type = FX950_PACKETS_UNICAST;

        result = fx950_api_labeling_get_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        /* iterate with FX950_PACKETS_MULTICAST */
        packet_type = FX950_PACKETS_MULTICAST;

        result = fx950_api_labeling_get_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        /*
            1.2. Call with packet_type [0x5AAAAAA5] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        packet_type = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_get_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        packet_type = FX950_PACKETS_MULTICAST;

        /*
            1.3. Call with priority_new [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_priority_map(device_id, packet_type, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, priority_new = NULL", device_id);
    }

    packet_type = FX950_PACKETS_UNICAST;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_priority_map(device_id, packet_type, priority_new);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_priority_map(device_id, packet_type, priority_new);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_link_base_queue_number(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   link_base_queue_number[FX950_NOF_HGS_LINKS]
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_link_base_queue_number)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_base_queue_number {0,0 / 0x2FFF,0x2FFF}.
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_link_base_queue_number with not NULL link_base_queue_number_ptr.
    Expected: SAND_OK and the same link_base_queue_number as was set.
    1.3. Call with link_base_queue_number[0] = 0x3000 (out of range).
    Expected: NOT SAND_OK.
    1.4. Call with link_base_queue_number_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;
    
    SAND_UINT  device_id = 0;
    SAND_U32   link_base_queue_number[FX950_NOF_HGS_LINKS];
    SAND_U32   link_base_queue_numberGet[FX950_NOF_HGS_LINKS];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_base_queue_number {0,0 / 0x2FFF,0x2FFF}.
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        link_base_queue_number[0] = 0;
        link_base_queue_number[1] = 0;

        result = fx950_api_labeling_set_link_base_queue_number(device_id, link_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_labeling_get_link_base_queue_number with not NULL link_base_queue_number_ptr.
            Expected: SAND_OK and the same link_base_queue_number as was set..
        */
        result = fx950_api_labeling_get_link_base_queue_number(device_id, link_base_queue_numberGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_link_base_queue_number: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(link_base_queue_number[0], link_base_queue_numberGet[0],
                                         "got another link_base_queue_number[0] than was set: %d", device_id);

        /* iterate with 0x2FFF */
        link_base_queue_number[0] = 0x2FFF;
        link_base_queue_number[1] = 0x2FFF;

        result = fx950_api_labeling_set_link_base_queue_number(device_id, link_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_labeling_get_link_base_queue_number with not NULL link_base_queue_number_ptr.
            Expected: SAND_OK and the same link_base_queue_number as was set..
        */
        result = fx950_api_labeling_get_link_base_queue_number(device_id, link_base_queue_numberGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_link_base_queue_number: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(link_base_queue_number[0], link_base_queue_numberGet[0],
                                         "got another link_base_queue_number[0] than was set: %d", device_id);

        /*
            1.3. Call with link_base_queue_number[0] = 12k+1 (out of range).
            Expected: NOT SAND_OK.
        */
        link_base_queue_number[0] = 0x3000;

        result = fx950_api_labeling_set_link_base_queue_number(device_id, link_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link_base_queue_number[0] = %d",
                                         device_id, link_base_queue_number[0]);

        link_base_queue_number[0] = 0;

        /*
            1.4. Call with link_base_queue_number_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_set_link_base_queue_number(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_base_queue_number = NULL", device_id);
    }

    link_base_queue_number[0] = 0;
    link_base_queue_number[1] = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_link_base_queue_number(device_id, link_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_link_base_queue_number(device_id, link_base_queue_number);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_link_base_queue_number(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    unsigned long   link_base_queue_number[FX950_NOF_HGS_LINKS]
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_link_base_queue_number)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null link_base_queue_number.
    Expected: SAND_OK.
    1.2. Call with link_base_queue_number [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   link_base_queue_number[FX950_NOF_HGS_LINKS];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null link_base_queue_number.
            Expected: SAND_OK.
        */
        result = fx950_api_labeling_get_link_base_queue_number(device_id, link_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with link_base_queue_number [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_link_base_queue_number(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_base_queue_number = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_link_base_queue_number(device_id, link_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_link_base_queue_number(device_id, link_base_queue_number);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_uc_label_tbl_entry(
    SAND_IN     unsigned int                        device_id,
    SAND_IN     unsigned long                       index,
    SAND_IN     FX950_LABELING_UC_LABEL_PARAM_ENTRY *uc_label_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_uc_label_tbl_entry)
{
/*
    ITERATE_DEVICES
    1.1. Call with index [0 / 10 / 127],
                   uc_label_params{ valid [TRUE],
                                    use_flow_mode [FX950_LABELING_UC_LABEL_BY_PORT /
                                                   FX950_LABELING_UC_LABEL_BY_VID /
                                                   FX950_LABELING_UC_LABEL_BY_FLOW],
                                    port_base_queue_number [0 / 10 / 0x2FFF],
                                    flow_base_queue_number [0 / 10 / 0x2FFF] }
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_uc_label_tbl_entry with not NULL uc_label_params_ptr and the same index as was set.
    Expected: SAND_OK and the same uc_label_params as was set.
    1.3. Call with index [128] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with uc_label_params_ptr->use_flow_mode [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.

    -- uc_label_params_ptr->valid [TRUE] --
    1.5. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_PORT],
                   uc_label_params_ptr->port_base_queue_number [0x3000] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_FLOW],
                   uc_label_params_ptr->port_base_queue_number [0x3000] (not relevant) and other param from 1.1.
    Expected: SAND_OK.
    1.7. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_FLOW], 
                   uc_label_params_ptr->flow_base_queue_number [0x3000] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_PORT], 
                   uc_label_params_ptr->flow_base_queue_number [0x3000] (not relevant) and other param from 1.1.
    Expected: SAND_OK.
    1.9. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_VID],
                   uc_label_params_ptr->port_base_queue_number [0x3000] (not relevant) and other param from 1.1.
    Expected: SAND_OK.
    1.10. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_VID], 
                   uc_label_params_ptr->flow_base_queue_number [0x3000] (not relevant) and other param from 1.1.
    Expected: SAND_OK.

    -- uc_label_params_ptr->valid [FALSE] --
    1.11. Call with uc_label_params_ptr->use_flow_mode [0x5AAAAAA5] (not relevant) and other param from 1.1.
    Expected: SAND_OK.
    1.12. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_PORT],
                   uc_label_params_ptr->port_base_queue_number [0x3000] (not relevant) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.13. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_FLOW], 
                   uc_label_params_ptr->flow_base_queue_number [0x3000] (not relevant) and other param from 1.1.
    Expected: NOT SAND_OK.

    1.14. Call with uc_label_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   index = 0;

    FX950_LABELING_UC_LABEL_PARAM_ENTRY uc_label_params;
    FX950_LABELING_UC_LABEL_PARAM_ENTRY uc_label_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with index [0 / 10 / 127],
                           uc_label_params{ valid [TRUE],
                                            use_flow_mode [FX950_LABELING_UC_LABEL_BY_PORT /                                                            
                                                           FX950_LABELING_UC_LABEL_BY_FLOW],
                                            port_base_queue_number [0 / 10 / 0x2FFF],
                                            flow_base_queue_number [0 / 10 / 0x2FFF] }
            Expected: SAND_OK.
        */
        /* iterate with index = 0*/
        index = 0;

        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_PORT;
        uc_label_params.port_base_queue_number = 0;
        uc_label_params.flow_base_queue_number = 0;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, index);

        /*
            1.2. Call fx950_api_labeling_get_uc_label_tbl_entry with not NULL uc_label_params_ptr and the same index as was set.
            Expected: SAND_OK and the same uc_label_params as was set.
        */
        result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, &uc_label_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_uc_label_tbl_entry: %d, %d", device_id, index);

        UTF_VERIFY_EQUAL1_STRING_MAC(uc_label_params.valid, uc_label_paramsGet.valid,
                                     "got another uc_label_params.valid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_label_params.use_flow_mode, uc_label_paramsGet.use_flow_mode,
                                     "got another uc_label_params.use_flow_mode than was set: %d", device_id);

        /* iterate with index = 10*/
        index = 10;

        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_PORT;
        uc_label_params.port_base_queue_number = 10;
        uc_label_params.flow_base_queue_number = 10;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, index);

        /*
            1.2. Call fx950_api_labeling_get_uc_label_tbl_entry with not NULL uc_label_params_ptr and the same index as was set.
            Expected: SAND_OK and the same uc_label_params as was set.
        */
        result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, &uc_label_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_uc_label_tbl_entry: %d, %d", device_id, index);

        UTF_VERIFY_EQUAL1_STRING_MAC(uc_label_params.valid, uc_label_paramsGet.valid,
                                     "got another uc_label_params.valid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_label_params.use_flow_mode, uc_label_paramsGet.use_flow_mode,
                                     "got another uc_label_params.use_flow_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_label_params.port_base_queue_number, uc_label_paramsGet.port_base_queue_number,
                                     "got another uc_label_params.port_base_queue_number than was set: %d", device_id);

        /* iterate with index = 127 */
        index = 127;

        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_FLOW;
        uc_label_params.port_base_queue_number = 0xFFF;
        uc_label_params.flow_base_queue_number = 0xFFF;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, index);

        /*
            1.2. Call fx950_api_labeling_get_uc_label_tbl_entry with not NULL uc_label_params_ptr and the same index as was set.
            Expected: SAND_OK and the same uc_label_params as was set.
        */
        result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, &uc_label_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_uc_label_tbl_entry: %d, %d", device_id, index);

        UTF_VERIFY_EQUAL1_STRING_MAC(uc_label_params.valid, uc_label_paramsGet.valid,
                                     "got another uc_label_params.valid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_label_params.use_flow_mode, uc_label_paramsGet.use_flow_mode,
                                     "got another uc_label_params.use_flow_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(uc_label_params.flow_base_queue_number, uc_label_paramsGet.flow_base_queue_number,
                                     "got another uc_label_params.flow_base_queue_number than was set: %d", device_id);

        /*
            1.3. Call with index [128] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        index = 128;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, index);

        index = 0;

        /*
            1.4. Call with uc_label_params_ptr->use_flow_mode [0x5AAAAAA5] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_label_params.use_flow_mode = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, uc_label_params.use_flow_mode = %d",
                                         device_id, uc_label_params.use_flow_mode);

        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_PORT;

        /* -- uc_label_params_ptr->valid [TRUE] -- */
        
        /*
            1.5. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_PORT],
                           uc_label_params_ptr->port_base_queue_number [0x3000] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_PORT;
        uc_label_params.port_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(SAND_OK, rc, 
                                         "%d, ->valid = %d, ->use_flow_mode = %d, ->port_base_queue_number = %d",
                                         device_id, 
                                         uc_label_params.valid, 
                                         uc_label_params.use_flow_mode, 
                                         uc_label_params.port_base_queue_number);

        uc_label_params.port_base_queue_number = 0;

        /*
            1.6. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_FLOW],
                           uc_label_params_ptr->port_base_queue_number [0x3000] (OUT OF RANGE) and other param from 1.1.
            Expected: SAND_OK.
        */
        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_FLOW;
        uc_label_params.port_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(SAND_OK, rc, 
                                     "%d, ->valid = %d, ->use_flow_mode = %d, ->port_base_queue_number = %d",
                                     device_id, 
                                     uc_label_params.valid, 
                                     uc_label_params.use_flow_mode, 
                                     uc_label_params.port_base_queue_number);

        uc_label_params.port_base_queue_number = 0;

        /*        
            1.7. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_FLOW], 
                           uc_label_params_ptr->flow_base_queue_number [0x3000] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_FLOW;
        uc_label_params.flow_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(SAND_OK, rc, 
                                         "%d, ->valid = %d, ->use_flow_mode = %d, ->flow_base_queue_number = %d",
                                         device_id, 
                                         uc_label_params.valid, 
                                         uc_label_params.use_flow_mode, 
                                         uc_label_params.flow_base_queue_number);

        uc_label_params.flow_base_queue_number = 0;

        /*
            1.8. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_PORT], 
                           uc_label_params_ptr->flow_base_queue_number [0x3000] (out of range) and other param from 1.1.
            Expected: SAND_OK.
        */
        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_PORT;
        uc_label_params.flow_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(SAND_OK, rc, 
                                     "%d, ->valid = %d, ->use_flow_mode = %d, ->flow_base_queue_number = %d",
                                     device_id, 
                                     uc_label_params.valid, 
                                     uc_label_params.use_flow_mode, 
                                     uc_label_params.flow_base_queue_number);

        uc_label_params.flow_base_queue_number = 0;

        /*        
            1.9. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_VID] (not supported),
                           uc_label_params_ptr->port_base_queue_number [0x3000] (not relevant) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_VID;
        uc_label_params.port_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(SAND_OK, rc, 
                                     "%d, ->valid = %d, ->use_flow_mode = %d, ->port_base_queue_number = %d",
                                     device_id, 
                                     uc_label_params.valid, 
                                     uc_label_params.use_flow_mode, 
                                     uc_label_params.port_base_queue_number);

        uc_label_params.port_base_queue_number = 0;

        /*
            1.10. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_VID] (not supported), 
                           uc_label_params_ptr->flow_base_queue_number [0x3000] (not relevant) and other param from 1.1.
            Expected: SAND_OK.
        */
        uc_label_params.valid = TRUE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_VID;
        uc_label_params.flow_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(SAND_OK, rc, 
                                     "%d, ->valid = %d, ->use_flow_mode = %d, ->flow_base_queue_number = %d",
                                     device_id, 
                                     uc_label_params.valid, 
                                     uc_label_params.use_flow_mode, 
                                     uc_label_params.flow_base_queue_number);

        uc_label_params.flow_base_queue_number = 0;

        /* -- uc_label_params_ptr->valid [FALSE] -- */

        /*
            1.11. Call with uc_label_params_ptr->use_flow_mode [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_label_params.valid = FALSE;
        uc_label_params.use_flow_mode = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, uc_label_params.use_flow_mode = %d",
                                     device_id, uc_label_params.use_flow_mode);

        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_PORT;

        /*
            1.12. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_PORT],
                           uc_label_params_ptr->port_base_queue_number [0x3000] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_label_params.valid = FALSE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_PORT;
        uc_label_params.port_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(SAND_OK, rc, 
                                     "%d, ->valid = %d, ->use_flow_mode = %d, ->port_base_queue_number = %d",
                                     device_id, 
                                     uc_label_params.valid, 
                                     uc_label_params.use_flow_mode, 
                                     uc_label_params.port_base_queue_number);

        uc_label_params.port_base_queue_number = 0;

        /*
            1.13. Call with uc_label_params_ptr->use_flow_mode [FX950_LABELING_UC_LABEL_BY_FLOW], 
                           uc_label_params_ptr->flow_base_queue_number [0x3000] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        uc_label_params.valid = FALSE;
        uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_FLOW;
        uc_label_params.flow_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(SAND_OK, rc, 
                                     "%d, ->valid = %d, ->use_flow_mode = %d, ->flow_base_queue_number = %d",
                                     device_id, 
                                     uc_label_params.valid, 
                                     uc_label_params.use_flow_mode, 
                                     uc_label_params.flow_base_queue_number);

        uc_label_params.flow_base_queue_number = 0;

        /*
            1.14. Call with uc_label_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uc_label_params = NULL", device_id);
    }

    index = 0;

    uc_label_params.valid = TRUE;
    uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_PORT;
    uc_label_params.port_base_queue_number = 0;
    uc_label_params.flow_base_queue_number = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_uc_label_tbl_entry(
    SAND_IN     unsigned int                        device_id,
    SAND_IN     unsigned long                       index,
    SAND_OUT    FX950_LABELING_UC_LABEL_PARAM_ENTRY *uc_label_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_uc_label_tbl_entry)
{
/*
    ITERATE_DEVICES
    1.1. Call with index [0 / 127] non-null uc_label_params_ptr.
    Expected: SAND_OK.
    1.2. Call with index [128] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with uc_label_params_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   index = 0;
    FX950_LABELING_UC_LABEL_PARAM_ENTRY uc_label_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with index [0 / 127] non-null uc_label_params_ptr.
            Expected: SAND_OK.
        */
        /* iterate with index = 0 */
        index = 0;

        result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, index);

        /* iterate with index = 127 */
        index = 127;

        result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, index);

        /*
            1.2. Call with index [128] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        index = 128;

        result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, index);

        index = 0;

        /*
            1.3. Call with uc_label_params_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uc_label_params = NULL", device_id);
    }

    index = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, &uc_label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, index, &uc_label_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_ingress_labeling_parameters(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     FX950_LABELING_INGRESS_PARAMS_STRUCT    *label_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_ingress_labeling_parameters)
{
/*
    ITERATE_DEVICES
    1.1. Call with label_params {   uc_label_mode [FX950_LABELING_UC_LABEL_BY_PORT / FX950_LABELING_UC_LABEL_BY_VID]
                                    port_add_enable [TRUE / FALSE],
                                    flow_add_enable [TRUE / FALSE],
                                    uc_priority_num_bit [0 / 3],
                                    uc_flow_id_num_bit [0 / 16],
                                    uc_label_tbl_access_mode [FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_PORT / FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_DEVICE],
                                    mailbox_device [0 / 31],
                                    mc_label_mode = [FX950_LABELING_MC_LABEL_BY_VIDX / FX950_LABELING_MC_LABEL_BY_VID],
                                    mc_flow_vid_vidx_width [0 / 16],
                                    mc_prio_width [0 / FX950_MC_PRIO_WIDTH_MAX] }

    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_ingress_labeling_parameters with not NULL label_params_ptr
    Expected: SAND_OK and the same label_params_ptr as was set.
    1.3. Call with label_params_ptr->uc_label_mode [0x5AAAAAA5] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with label_params_ptr->uc_priority_num_bit [4] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with label_params_ptr->uc_flow_id_num_bit [17] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with label_params_ptr->uc_label_tbl_access_mode [0x5AAAAAA5] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with label_params_ptr->mailbox_device [32] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with label_params_ptr->mc_label_mode [0x5AAAAAA5] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.9. Call with label_params_ptr->mc_flow_vid_vidx_width [17] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.10. Call with label_params_ptr->mc_prio_width [FX950_MC_PRIO_WIDTH_MAX+1] and other params from 1.1.
    Expected: NOT SAND_OK.
    1.11. Call with label_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_LABELING_INGRESS_PARAMS_STRUCT label_params;
    FX950_LABELING_INGRESS_PARAMS_STRUCT label_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with label_params {   uc_label_mode [FX950_LABELING_UC_LABEL_BY_PORT / FX950_LABELING_UC_LABEL_BY_VID]
                                            port_add_enable [TRUE / FALSE],
                                            flow_add_enable [TRUE / FALSE],
                                            uc_priority_num_bit [0 / 3],
                                            uc_flow_id_num_bit [0 / 16],
                                            uc_label_tbl_access_mode [FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_PORT / FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_DEVICE],
                                            mailbox_device [0 / 31],
                                            mc_label_mode = [FX950_LABELING_MC_LABEL_BY_VIDX / FX950_LABELING_MC_LABEL_BY_VID],
                                            mc_flow_vid_vidx_width [0 / 16],
                                            mc_prio_width [0 / FX950_MC_PRIO_WIDTH_MAX] }

            Expected: SAND_OK.
        */
        /* iterate with FX950_LABELING_UC_LABEL_BY_PORT */
        label_params.uc_label_mode = FX950_LABELING_UC_LABEL_BY_PORT;
        label_params.port_add_enable = TRUE;
        label_params.flow_add_enable = TRUE;
        label_params.uc_priority_num_bit = 0;
        label_params.uc_flow_id_num_bit = 0;
        label_params.uc_label_tbl_access_mode = FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_PORT;
        label_params.mailbox_device = 0;
        label_params.mc_label_mode = FX950_LABELING_MC_LABEL_BY_VIDX;
        label_params.mc_flow_vid_vidx_width = 0;
        label_params.mc_prio_width = 0;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_labeling_get_ingress_labeling_parameters with not NULL label_params_ptr
            Expected: SAND_OK and the same label_params_ptr as was set.
        */
        result = fx950_api_labeling_get_ingress_labeling_parameters(device_id, &label_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_ingress_labeling_parameters: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.uc_label_mode, label_paramsGet.uc_label_mode,
                                     "got another label_params.uc_label_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.port_add_enable, label_paramsGet.port_add_enable,
                                     "got another label_params.port_add_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.flow_add_enable, label_paramsGet.flow_add_enable,
                                     "got another label_params.flow_add_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.uc_priority_num_bit, label_paramsGet.uc_priority_num_bit,
                                     "got another label_params.uc_priority_num_bit than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.uc_flow_id_num_bit, label_paramsGet.uc_flow_id_num_bit,
                                     "got another label_params.uc_flow_id_num_bit than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.uc_label_tbl_access_mode, label_paramsGet.uc_label_tbl_access_mode,
                                     "got another label_params.uc_label_tbl_access_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.mailbox_device, label_paramsGet.mailbox_device,
                                     "got another label_params.mailbox_device than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.mc_label_mode, label_paramsGet.mc_label_mode,
                                     "got another label_params.mc_label_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.mc_flow_vid_vidx_width, label_paramsGet.mc_flow_vid_vidx_width,
                                     "got another label_params.mc_flow_vid_vidx_width than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.mc_prio_width, label_paramsGet.mc_prio_width,
                                     "got another label_params.mc_prio_width than was set: %d", device_id);

        /* iterate with FX950_LABELING_UC_LABEL_BY_PORT */
        label_params.uc_label_mode = FX950_LABELING_UC_LABEL_BY_VID;
        label_params.port_add_enable = FALSE;
        label_params.flow_add_enable = FALSE;
        label_params.uc_priority_num_bit = 3;
        label_params.uc_flow_id_num_bit = 16;
        label_params.uc_label_tbl_access_mode = FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_DEVICE;
        label_params.mailbox_device = 31;
        label_params.mc_label_mode = FX950_LABELING_MC_LABEL_BY_VID;
        label_params.mc_flow_vid_vidx_width = 16;
        label_params.mc_prio_width = FX950_MC_PRIO_WIDTH_MAX;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_labeling_get_ingress_labeling_parameters with not NULL label_params_ptr
            Expected: SAND_OK and the same label_params_ptr as was set.
        */
        result = fx950_api_labeling_get_ingress_labeling_parameters(device_id, &label_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_ingress_labeling_parameters: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.uc_label_mode, label_paramsGet.uc_label_mode,
                                     "got another label_params.uc_label_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.port_add_enable, label_paramsGet.port_add_enable,
                                     "got another label_params.port_add_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.flow_add_enable, label_paramsGet.flow_add_enable,
                                     "got another label_params.flow_add_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.uc_priority_num_bit, label_paramsGet.uc_priority_num_bit,
                                     "got another label_params.uc_priority_num_bit than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.uc_flow_id_num_bit, label_paramsGet.uc_flow_id_num_bit,
                                     "got another label_params.uc_flow_id_num_bit than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.uc_label_tbl_access_mode, label_paramsGet.uc_label_tbl_access_mode,
                                     "got another label_params.uc_label_tbl_access_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.mailbox_device, label_paramsGet.mailbox_device,
                                     "got another label_params.mailbox_device than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.mc_label_mode, label_paramsGet.mc_label_mode,
                                     "got another label_params.mc_label_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.mc_flow_vid_vidx_width, label_paramsGet.mc_flow_vid_vidx_width,
                                     "got another label_params.mc_flow_vid_vidx_width than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(label_params.mc_prio_width, label_paramsGet.mc_prio_width,
                                     "got another label_params.mc_prio_width than was set: %d", device_id);

        /*
            1.3. Call with label_params_ptr->uc_label_mode [0x5AAAAAA5] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        label_params.uc_label_mode = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, label_params.uc_label_mode = %d",
                                        device_id, label_params.uc_label_mode);

        label_params.uc_label_mode = FX950_LABELING_UC_LABEL_BY_PORT;

        /*
            1.4. Call with label_params_ptr->uc_priority_num_bit [4] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        label_params.uc_priority_num_bit = 4;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, label_params.uc_priority_num_bit = %d",
                                        device_id, label_params.uc_priority_num_bit);

        label_params.uc_priority_num_bit = 0;

        /*
            1.5. Call with label_params_ptr->uc_flow_id_num_bit [17] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        label_params.uc_flow_id_num_bit = 17;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, label_params.uc_flow_id_num_bit = %d",
                                        device_id, label_params.uc_flow_id_num_bit);

        label_params.uc_flow_id_num_bit = 0;

        /*
            1.6. Call with label_params_ptr->uc_label_tbl_access_mode [0x5AAAAAA5] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        label_params.uc_label_tbl_access_mode = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, label_params.uc_label_tbl_access_mode = %d",
                                        device_id, label_params.uc_label_tbl_access_mode);

        label_params.uc_label_tbl_access_mode = FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_PORT;

        /*
            1.7. Call with label_params_ptr->mailbox_device [32] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        label_params.mailbox_device = 32;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, label_params.mailbox_device = %d",
                                        device_id, label_params.mailbox_device);

        label_params.mailbox_device = 0;

        /*
            1.8. Call with label_params_ptr->mc_label_mode [0x5AAAAAA5] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        label_params.mc_label_mode = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, label_params.mc_label_mode = %d",
                                        device_id, label_params.mc_label_mode);

        label_params.mc_label_mode = FX950_LABELING_MC_LABEL_BY_VID;

        /*
            1.9. Call with label_params_ptr->mc_flow_vid_vidx_width [17] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        label_params.mc_flow_vid_vidx_width = 17;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, label_params.mc_flow_vid_vidx_width = %d",
                                        device_id, label_params.mc_flow_vid_vidx_width);

        label_params.mc_flow_vid_vidx_width = FX950_LABELING_MC_LABEL_BY_VID;

        /*
            1.10. Call with label_params_ptr->mc_prio_width [FX950_MC_PRIO_WIDTH_MAX+1] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        label_params.mc_prio_width = FX950_MC_PRIO_WIDTH_MAX+1;

        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, label_params.mc_prio_width = %d",
                                        device_id, label_params.mc_prio_width);

        label_params.mc_prio_width = 0;

        /*
            1.11. Call with label_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, label_params = NULL", device_id);
    }

    label_params.uc_label_mode = FX950_LABELING_UC_LABEL_BY_PORT;
    label_params.port_add_enable = TRUE;
    label_params.flow_add_enable = TRUE;
    label_params.uc_priority_num_bit = 0;
    label_params.uc_flow_id_num_bit = 0;
    label_params.uc_label_tbl_access_mode = FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_PORT;
    label_params.mailbox_device = 0;
    label_params.mc_label_mode = FX950_LABELING_MC_LABEL_BY_VIDX;
    label_params.mc_flow_vid_vidx_width = 0;
    label_params.mc_prio_width = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_ingress_labeling_parameters(
    SAND_IN     unsigned int                            device_id,
    SAND_OUT    FX950_LABELING_INGRESS_PARAMS_STRUCT    *label_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_ingress_labeling_parameters)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null label_params_ptr.
    Expected: SAND_OK.
    1.2. Call with label_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_LABELING_INGRESS_PARAMS_STRUCT label_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null label_params_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_labeling_get_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with label_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_ingress_labeling_parameters(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, label_params = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_ingress_labeling_parameters(device_id, &label_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_ingress_labeling_parameters(device_id, &label_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_dx_set_analyzer_target_cfg(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     FX950_DIRECTION_TYPE                    direction,
    SAND_IN     FX950_LABELING_ANALYZER_PARAM_STRUCT    *analyzer_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_dx_set_analyzer_target_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS],
                   analyzer_params { device [0 / 31],
                                     port [0 / 63],
                                     tc [0 / 7],
                                     dp [0 / 3] }
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_dx_get_analyzer_target_cfg with not NULL analyzer_params_ptr and direction from 1.1.
    Expected: SAND_OK and the same analyzer_params_ptr as was set.
    1.3. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with analyzer_params_ptr->device [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with analyzer_params_ptr->port [64] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with analyzer_params_ptr->tc [8] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with analyzer_params_ptr->dp [4] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with analyzer_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT            device_id = 0;
    FX950_DIRECTION_TYPE direction = FX950_DIRECTION_INGRESS;

    FX950_LABELING_ANALYZER_PARAM_STRUCT analyzer_params;
    FX950_LABELING_ANALYZER_PARAM_STRUCT analyzer_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS],
                           analyzer_params { device [0 / 31],
                                             port [0 / 63],
                                             tc [0 / 7],
                                             dp [0 / 3] }
            Expected: SAND_OK.
        */
        /* iterate with FX950_DIRECTION_INGRESS */
        direction = FX950_DIRECTION_INGRESS;

        analyzer_params.device = 0;
        analyzer_params.port = 0;
        analyzer_params.tc = 0;
        analyzer_params.dp = 0;

        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /*
            1.2. Call fx950_api_labeling_dx_get_analyzer_target_cfg with not NULL analyzer_params_ptr and direction from 1.1.
            Expected: SAND_OK and the same analyzer_params_ptr as was set.
        */
        result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, &analyzer_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_dx_get_analyzer_target_cfg: %d, %d",
                                     device_id, direction);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(analyzer_params.device, analyzer_paramsGet.device,
                                     "got another analyzer_params.device than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(analyzer_params.port, analyzer_paramsGet.port,
                                     "got another analyzer_params.port than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(analyzer_params.tc, analyzer_paramsGet.tc,
                                     "got another analyzer_params.tc than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(analyzer_params.dp, analyzer_paramsGet.dp,
                                     "got another analyzer_params.dp than was set: %d", device_id);

        /* iterate with FX950_DIRECTION_EGRESS */
        direction = FX950_DIRECTION_EGRESS;

        analyzer_params.device = 31;
        analyzer_params.port = 63;
        analyzer_params.tc = 7;
        analyzer_params.dp = 3;

        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /*
            1.2. Call fx950_api_labeling_dx_get_analyzer_target_cfg with not NULL analyzer_params_ptr and direction from 1.1.
            Expected: SAND_OK and the same analyzer_params_ptr as was set.
        */
        result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, &analyzer_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_dx_get_analyzer_target_cfg: %d, %d",
                                     device_id, direction);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(analyzer_params.device, analyzer_paramsGet.device,
                                     "got another analyzer_params.device than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(analyzer_params.port, analyzer_paramsGet.port,
                                     "got another analyzer_params.port than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(analyzer_params.tc, analyzer_paramsGet.tc,
                                     "got another analyzer_params.tc than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(analyzer_params.dp, analyzer_paramsGet.dp,
                                     "got another analyzer_params.dp than was set: %d", device_id);

        /*
            1.3. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        direction = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        direction = FX950_DIRECTION_INGRESS;

        /*
            1.4. Call with analyzer_params_ptr->device [32] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        analyzer_params.device = 32;

        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, analyzer_params_ptr->device = %d",
                                         device_id, analyzer_params.device);

        analyzer_params.device = 0;

        /*
            1.5. Call with analyzer_params_ptr->port [64] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        analyzer_params.port = 64;

        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, analyzer_params_ptr->port = %d",
                                         device_id, analyzer_params.port);

        analyzer_params.port = 0;

        /*
            1.6. Call with analyzer_params_ptr->tc [8] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        analyzer_params.tc = 8;

        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, analyzer_params_ptr->tc = %d",
                                         device_id, analyzer_params.tc);

        analyzer_params.tc = 0;

        /*
            1.7. Call with analyzer_params_ptr->dp [4] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        analyzer_params.dp = 4;

        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, analyzer_params_ptr->dp = %d",
                                         device_id, analyzer_params.dp);

        analyzer_params.dp = 0;

        /*
            1.8. Call with analyzer_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, analyzer_params = NULL", device_id);
    }

    direction = FX950_DIRECTION_INGRESS;
    analyzer_params.device = 0;
    analyzer_params.port = 0;
    analyzer_params.tc = 0;
    analyzer_params.dp = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_dx_get_analyzer_target_cfg(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     FX950_DIRECTION_TYPE                    direction,
    SAND_OUT    FX950_LABELING_ANALYZER_PARAM_STRUCT    *analyzer_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_dx_get_analyzer_target_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS], non-null analyzer_params_ptr.
    Expected: SAND_OK.
    1.2. Call with direction [0x5AAAAAA5] (out of range).
    Expected: NOT SAND_OK.
    1.3. Call with analyzer_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_DIRECTION_TYPE direction = FX950_DIRECTION_INGRESS;
    FX950_LABELING_ANALYZER_PARAM_STRUCT analyzer_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS], non-null analyzer_params_ptr.
            Expected: SAND_OK.
        */
        /* iterate with FX950_DIRECTION_INGRESS */
        direction = FX950_DIRECTION_INGRESS;

        result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /* iterate with FX950_DIRECTION_EGRESS */
        direction = FX950_DIRECTION_EGRESS;

        result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /*
            1.2. Call with direction [0x5AAAAAA5] (out of range).
            Expected: NOT SAND_OK.
        */
        direction = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /*
            1.3. Call with analyzer_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, analyzer_params = NULL", device_id);
    }

    direction = FX950_DIRECTION_INGRESS;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, &analyzer_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, &analyzer_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_qos_to_prio_map(
    SAND_IN     unsigned int            device_id,
    SAND_IN     FX950_PACKETS_TYPE      packet_type,
    SAND_IN     unsigned long           qos_profile,
    SAND_IN     unsigned long           priority
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_qos_to_prio_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST],
                   qos_profile [0 / 127],
                   priority [0 / 7].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_qos_to_prio_map with not NULL priority and other param from 1.1.
    Expected: SAND_OK.
    1.3. Call with packet_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with qos_profile [128] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with priority [8] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT          device_id = 0;
    FX950_PACKETS_TYPE packet_type = FX950_PACKETS_UNICAST;
    SAND_U32           qos_profile = 0;
    SAND_U32           priority = 0;
    SAND_U32           priorityGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST],
                           qos_profile [0 / 127],
                           priority [0 / 7].
            Expected: SAND_OK.
        */
        /* iterate with FX950_PACKETS_UNICAST */
        packet_type = FX950_PACKETS_UNICAST;
        qos_profile = 0;
        priority = 0;

        result = fx950_api_labeling_set_qos_to_prio_map(device_id, packet_type, qos_profile, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, packet_type, qos_profile, priority);

        /*
            1.2. Call fx950_api_labeling_get_qos_to_prio_map with not NULL priority and other param from 1.1.
            Expected: SAND_OK.
        */
        result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priorityGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_qos_to_prio_map: %d, %d, %d",
                                     device_id, packet_type, qos_profile);

        UTF_VERIFY_EQUAL1_STRING_MAC(priority, priorityGet, "got another priority than was set: %d", device_id);

        /* iterate with FX950_PACKETS_MULTICAST */
        packet_type = FX950_PACKETS_MULTICAST;
        qos_profile = 127;
        priority = 7;

        result = fx950_api_labeling_set_qos_to_prio_map(device_id, packet_type, qos_profile, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, packet_type, qos_profile, priority);

        /*
            1.2. Call fx950_api_labeling_get_qos_to_prio_map with not NULL priority and other param from 1.1.
            Expected: SAND_OK.
        */
        result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priorityGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_qos_to_prio_map: %d, %d, %d",
                                     device_id, packet_type, qos_profile);

        UTF_VERIFY_EQUAL1_STRING_MAC(priority, priorityGet, "got another priority than was set: %d", device_id);

        /*
            1.3. Call with packet_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        packet_type = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_set_qos_to_prio_map(device_id, packet_type, qos_profile, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        packet_type = FX950_PACKETS_UNICAST;

        /*
            1.4. Call with qos_profile [128] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        qos_profile = 128;

        result = fx950_api_labeling_set_qos_to_prio_map(device_id, packet_type, qos_profile, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, qos_profile = %d", device_id, qos_profile);

        qos_profile = 0;

        /*
            1.5. Call with priority [8] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        priority = 8;

        result = fx950_api_labeling_set_qos_to_prio_map(device_id, packet_type, qos_profile, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, priority = %d", device_id, priority);
    }

    packet_type = FX950_PACKETS_UNICAST;
    qos_profile = 0;
    priority = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_qos_to_prio_map(device_id, packet_type, qos_profile, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_qos_to_prio_map(device_id, packet_type, qos_profile, priority);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_qos_to_prio_map(
    SAND_IN     unsigned int            device_id,
    SAND_IN     FX950_PACKETS_TYPE      packet_type,
    SAND_IN     unsigned long           qos_profile,
    SAND_OUT    unsigned long           *priority_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_qos_to_prio_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST],
                   qos_profile [0 / 127],
                   not NULL priority_ptr.
    Expected: SAND_OK.
    1.2. Call with packet_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with qos_profile [128] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with priority [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT          device_id = 0;
    FX950_PACKETS_TYPE packet_type = FX950_PACKETS_UNICAST;
    SAND_U32           qos_profile = 0;
    SAND_U32           priority = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with packet_type [FX950_PACKETS_UNICAST / FX950_PACKETS_MULTICAST],
                           qos_profile [0 / 127],
                           not NULL priority_ptr.
            Expected: SAND_OK.
        */
        /* iterate with FX950_PACKETS_UNICAST */
        packet_type = FX950_PACKETS_UNICAST;
        qos_profile = 0;

        result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, packet_type, qos_profile);

        /* iterate with FX950_PACKETS_MULTICAST */
        packet_type = FX950_PACKETS_MULTICAST;
        qos_profile = 127;

        result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, packet_type, qos_profile);

        /*
            1.2. Call with packet_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        packet_type = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, packet_type);

        packet_type = FX950_PACKETS_UNICAST;

        /*
            1.3. Call with qos_profile [128] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        qos_profile = 128;

        result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, qos_profile = %d", device_id, qos_profile);

        qos_profile = 0;

        /*
            1.4. Call with priority [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, priority = NULL", device_id);
    }

    packet_type = FX950_PACKETS_UNICAST;
    qos_profile = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priority);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_mc_base_queue_number(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long    mc_base_queue_number
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_mc_base_queue_number)
{
/*
    ITERATE_DEVICES
    1.1. Call with mc_base_queue_number [0 / 0x2FFF].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_mc_base_queue_number with not NULL mc_base_queue_number_ptr.
    Expected: SAND_OK and the same mc_base_queue_numberas was set..
    1.2. Call with mc_base_queue_number [0x3000] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   mc_base_queue_number = 0;
    SAND_U32   mc_base_queue_numberGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with mc_base_queue_number [0 / 0x2FFF].
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        mc_base_queue_number = 0;

        result = fx950_api_labeling_set_mc_base_queue_number(device_id, mc_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mc_base_queue_number);

        /*
            1.2. Call fx950_api_labeling_get_mc_base_queue_number with not NULL mc_base_queue_number_ptr.
            Expected: SAND_OK and the same mc_base_queue_numberas was set..
        */
        result = fx950_api_labeling_get_mc_base_queue_number(device_id, &mc_base_queue_numberGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_mc_base_queue_number: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(mc_base_queue_number, mc_base_queue_numberGet, "got another mc_base_queue_number than was set: %d", device_id);

        /* iterate with 0x2FFF */
        mc_base_queue_number = 0x2FFF;

        result = fx950_api_labeling_set_mc_base_queue_number(device_id, mc_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mc_base_queue_number);

        /*
            1.2. Call fx950_api_labeling_get_mc_base_queue_number with not NULL mc_base_queue_number_ptr.
            Expected: SAND_OK and the same mc_base_queue_numberas was set..
        */
        result = fx950_api_labeling_get_mc_base_queue_number(device_id, &mc_base_queue_numberGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_mc_base_queue_number: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(mc_base_queue_number, mc_base_queue_numberGet, "got another mc_base_queue_number than was set: %d", device_id);

        /*
            1.2. Call with mc_base_queue_number [0x3000] (out of range).
            Expected: NOT SAND_OK.
        */
        mc_base_queue_number = 0x3000;

        result = fx950_api_labeling_set_mc_base_queue_number(device_id, mc_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mc_base_queue_number);
    }

    mc_base_queue_number = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_mc_base_queue_number(device_id, mc_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_mc_base_queue_number(device_id, mc_base_queue_number);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_mc_base_queue_number(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    unsigned long   *mc_base_queue_number_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_mc_base_queue_number)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null mc_base_queue_number_ptr.
    Expected: SAND_OK.
    1.2. Call with mc_base_queue_number_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   mc_base_queue_number = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null mc_base_queue_number_ptr.
            Expected: SAND_OK.
        */

        result = fx950_api_labeling_get_mc_base_queue_number(device_id, &mc_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with mc_base_queue_number_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_mc_base_queue_number(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mc_base_queue_number_ptr = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_mc_base_queue_number(device_id, &mc_base_queue_number);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_mc_base_queue_number(device_id, &mc_base_queue_number);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_dx_set_mc_id_assignment(
    SAND_IN     unsigned int                        device_id,
    SAND_IN     FX950_LABELING_MC_ID_ASSIGN_MODE    mc_id_assign_mode,
    SAND_IN     unsigned long                       flood_all_mc_id
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_dx_set_mc_id_assignment)
{
/*
    ITERATE_DEVICES
    1.1. Call with mc_id_assign_mode [FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL / FX950_LABELING_MC_ID_ASSIGN_TRUST_L3_L2_REG],
                   flood_all_mc_id [0 / 2047].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_dx_get_mc_id_assignment with not NULL pointers
    Expected: SAND_OK and the same params as was set.
    1.3. Call with mc_id_assign_mode [0x5AAAAAA5] (out of range).
    Expected: NOT SAND_OK.
    1.4. Call with mc_id_assign_mode [FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL] and flood_all_mc_id [2048] (out of range).
    Expected: NOT SAND_OK.
    1.5. Call with mc_id_assign_mode [FX950_LABELING_MC_ID_ASSIGN_TRUST_L3_L2_REG] and flood_all_mc_id [2048] (not relevant).
    Expected: SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_LABELING_MC_ID_ASSIGN_MODE mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL;
    FX950_LABELING_MC_ID_ASSIGN_MODE mc_id_assign_modeGet = FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL;
    SAND_U32   flood_all_mc_id = 0;
    SAND_U32   flood_all_mc_idGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with mc_id_assign_mode [FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL /
                                              FX950_LABELING_MC_ID_ASSIGN_TRUST_L3_L2_REG],
                           flood_all_mc_id [0 / 2047].
            Expected: SAND_OK.
        */
        /* iterate with mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL */
        mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL;
        flood_all_mc_id = 0;

        result = fx950_api_labeling_dx_set_mc_id_assignment(device_id, mc_id_assign_mode, flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, mc_id_assign_mode, flood_all_mc_id);

        /*
            1.2. Call fx950_api_labeling_dx_get_mc_id_assignment with not NULL pointers
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_labeling_dx_get_mc_id_assignment(device_id, &mc_id_assign_modeGet, &flood_all_mc_idGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_dx_get_mc_id_assignment: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_id_assign_mode, mc_id_assign_modeGet,
                                     "got another mc_id_assign_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(flood_all_mc_id, flood_all_mc_id,
                                     "got another flood_all_mc_id than was set: %d", device_id);

        /* iterate with mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_TRUST_L3_L2_REG */
        mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_TRUST_L3_L2_REG;
        flood_all_mc_id = 2047;

        result = fx950_api_labeling_dx_set_mc_id_assignment(device_id, mc_id_assign_mode, flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, mc_id_assign_mode, flood_all_mc_id);

        /*
            1.2. Call fx950_api_labeling_dx_get_mc_id_assignment with not NULL pointers
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_labeling_dx_get_mc_id_assignment(device_id, &mc_id_assign_modeGet, &flood_all_mc_idGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_dx_get_mc_id_assignment: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(mc_id_assign_mode, mc_id_assign_modeGet,
                                     "got another mc_id_assign_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(flood_all_mc_id, flood_all_mc_id,
                                     "got another flood_all_mc_id than was set: %d", device_id);

        /*
            1.3. Call with mc_id_assign_mode [0x5AAAAAA5].
            Expected: NOT SAND_OK.
        */
        mc_id_assign_mode = SAND_UTF_LABELING_INVALID_ENUM;

        result = fx950_api_labeling_dx_set_mc_id_assignment(device_id, mc_id_assign_mode, flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mc_id_assign_mode);

        mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL;

        /*
            1.4. Call with mc_id_assign_mode [FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL]
                           and flood_all_mc_id [2048] (out of range).
            Expected: NOT SAND_OK.
        */
        flood_all_mc_id = 2048;

        result = fx950_api_labeling_dx_set_mc_id_assignment(device_id, mc_id_assign_mode, flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, mc_id_assign_mode, flood_all_mc_id);

        /*
            1.5. Call with mc_id_assign_mode [FX950_LABELING_MC_ID_ASSIGN_TRUST_L3_L2_REG]
                           and flood_all_mc_id [2048] (not relevant).
            Expected: SAND_OK.
        */
        mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_TRUST_L3_L2_REG;

        result = fx950_api_labeling_dx_set_mc_id_assignment(device_id, mc_id_assign_mode, flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, mc_id_assign_mode, flood_all_mc_id);
    }

    mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL;
    flood_all_mc_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_dx_set_mc_id_assignment(device_id, mc_id_assign_mode, flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_dx_set_mc_id_assignment(device_id, mc_id_assign_mode, flood_all_mc_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_dx_get_mc_id_assignment(
    SAND_IN     unsigned int                        device_id,
    SAND_OUT    FX950_LABELING_MC_ID_ASSIGN_MODE    *mc_id_assign_mode_ptr,
    SAND_OUT    unsigned long                       *flood_all_mc_id_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_dx_get_mc_id_assignment)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null pointers.
    Expected: SAND_OK.
    1.2. Call with mc_id_assign_mode_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with flood_all_mc_id_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_LABELING_MC_ID_ASSIGN_MODE mc_id_assign_mode = FX950_LABELING_MC_ID_ASSIGN_FLOOD_ALL;
    SAND_U32   flood_all_mc_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null pointers.
            Expected: SAND_OK.
        */
        result = fx950_api_labeling_dx_get_mc_id_assignment(device_id, &mc_id_assign_mode, &flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with mc_id_assign_mode_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_dx_get_mc_id_assignment(device_id, NULL, &flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, mc_id_assign_mode_ptr = NULL", device_id);

        /*
            1.3. Call with flood_all_mc_id_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_dx_get_mc_id_assignment(device_id, &mc_id_assign_mode, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, flood_all_mc_id_ptr = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_dx_get_mc_id_assignment(device_id, &mc_id_assign_mode, &flood_all_mc_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_dx_get_mc_id_assignment(device_id, &mc_id_assign_mode, &flood_all_mc_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_dx_set_control_packets_dp(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   to_cpu_dp,
    SAND_IN     unsigned long   from_cpu_dp
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_dx_set_control_packets_dp)
{
/*
    ITERATE_DEVICES
    1.1. Call with to_cpu_dp[0 / 3], from_cpu_dp[0 / 3].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_dx_get_control_packets_dp with  not NULL pointers
    Expected: SAND_OK and the same params as was set.
    1.3. Call with to_cpu_dp [4] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with from_cpu_dp [4] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   to_cpu_dp = 0;
    SAND_U32   from_cpu_dp = 0;

    SAND_U32   to_cpu_dpGet = 0;
    SAND_U32   from_cpu_dpGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with to_cpu_dp[0 / 3], from_cpu_dp[0 / 3].
            Expected: SAND_OK.
        */
        /* iterate with to_cpu_dp = 0 */
        to_cpu_dp = 0;
        from_cpu_dp = 0;

        result = fx950_api_labeling_dx_set_control_packets_dp(device_id, to_cpu_dp, from_cpu_dp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, to_cpu_dp, from_cpu_dp);

        /*
            1.2. Call fx950_api_labeling_dx_get_control_packets_dp with  not NULL pointers
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_labeling_dx_get_control_packets_dp(device_id, &to_cpu_dpGet, &from_cpu_dpGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_dx_get_control_packets_dp: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(to_cpu_dp, to_cpu_dpGet, "got another to_cpu_dp than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(from_cpu_dp, from_cpu_dpGet, "got another from_cpu_dp than was set: %d", device_id);

        /* iterate with to_cpu_dp = 3 */
        to_cpu_dp = 3;
        from_cpu_dp = 3;

        result = fx950_api_labeling_dx_set_control_packets_dp(device_id, to_cpu_dp, from_cpu_dp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, to_cpu_dp, from_cpu_dp);

        /*
            1.2. Call fx950_api_labeling_dx_get_control_packets_dp with  not NULL pointers
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_labeling_dx_get_control_packets_dp(device_id, &to_cpu_dpGet, &from_cpu_dpGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_dx_get_control_packets_dp: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(to_cpu_dp, to_cpu_dpGet, "got another to_cpu_dp than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(from_cpu_dp, from_cpu_dpGet, "got another from_cpu_dp than was set: %d", device_id);

        /*
            1.3. Call with to_cpu_dp [4] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        to_cpu_dp = 4;

        result = fx950_api_labeling_dx_set_control_packets_dp(device_id, to_cpu_dp, from_cpu_dp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, to_cpu_dp);

        to_cpu_dp = 0;

        /*
            1.4. Call with from_cpu_dp [4] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        from_cpu_dp = 4;

        result = fx950_api_labeling_dx_set_control_packets_dp(device_id, to_cpu_dp, from_cpu_dp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, from_cpu_dp = %d", device_id, from_cpu_dp);
    }

    to_cpu_dp = 0;
    from_cpu_dp = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_dx_set_control_packets_dp(device_id, to_cpu_dp, from_cpu_dp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_dx_set_control_packets_dp(device_id, to_cpu_dp, from_cpu_dp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_dx_get_control_packets_dp(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    unsigned long   *to_cpu_dp_ptr,
    SAND_OUT    unsigned long   *from_cpu_dp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_dx_get_control_packets_dp)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null pointers.
    Expected: SAND_OK.
    1.2. Call with to_cpu_dp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with from_cpu_dp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   to_cpu_dp = 0;
    SAND_U32   from_cpu_dp = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null pointers.
            Expected: SAND_OK.
        */
        result = fx950_api_labeling_dx_get_control_packets_dp(device_id, &to_cpu_dp, &from_cpu_dp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with to_cpu_dp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_dx_get_control_packets_dp(device_id, NULL, &from_cpu_dp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, to_cpu_dp = NULL", device_id);

        /*
            1.3. Call with from_cpu_dp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_dx_get_control_packets_dp(device_id, &to_cpu_dp, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, from_cpu_dp = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_dx_get_control_packets_dp(device_id, &to_cpu_dp, &from_cpu_dp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_dx_get_control_packets_dp(device_id, &to_cpu_dp, &from_cpu_dp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_dx_set_qos_profile_to_dp_map(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   qos_profile,
    SAND_IN     unsigned long   dp_num
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_dx_set_qos_profile_to_dp_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with qos_profile[0 / 127], dp_num[0 / 3].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_dx_get_qos_profile_to_dp_map with not NULL dp_num_ptr and other param from 1.1.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with qos_profile[128] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with dp_num[4] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   qos_profile = 0;
    SAND_U32   dp_num = 0;
    SAND_U32   dp_numGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with qos_profile[0 / 127], dp_num[0 / 3].
            Expected: SAND_OK.
        */
        /* iterate with qos_profile = 0 */
        qos_profile = 0;
        dp_num = 0;

        result = fx950_api_labeling_dx_set_qos_profile_to_dp_map(device_id, qos_profile, dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, qos_profile, dp_num);

        /*
            1.2. Call fx950_api_labeling_dx_get_qos_profile_to_dp_map with not NULL dp_num_ptr and other param from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, &dp_numGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_dx_get_qos_profile_to_dp_map: %d, %d",
                                    device_id, qos_profile);

        UTF_VERIFY_EQUAL1_STRING_MAC(dp_num, dp_numGet, "got another dp_num than was set: %d", device_id);

        /* iterate with qos_profile = 127 */
        qos_profile = 127;
        dp_num = 3;

        result = fx950_api_labeling_dx_set_qos_profile_to_dp_map(device_id, qos_profile, dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, qos_profile, dp_num);

        /*
            1.2. Call fx950_api_labeling_dx_get_qos_profile_to_dp_map with not NULL dp_num_ptr and other param from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, &dp_numGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_dx_get_qos_profile_to_dp_map: %d, %d",
                                    device_id, qos_profile);

        UTF_VERIFY_EQUAL1_STRING_MAC(dp_num, dp_numGet, "got another dp_num than was set: %d", device_id);

        /*
            1.3. Call with qos_profile[128] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        qos_profile = 128;

        result = fx950_api_labeling_dx_set_qos_profile_to_dp_map(device_id, qos_profile, dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qos_profile);

        qos_profile = 0;

        /*
            1.4. Call with dp_num[4] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        dp_num = 4;

        result = fx950_api_labeling_dx_set_qos_profile_to_dp_map(device_id, qos_profile, dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, dp_num = %d", device_id, dp_num);
    }

    qos_profile = 0;
    dp_num = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_dx_set_qos_profile_to_dp_map(device_id, qos_profile, dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_dx_set_qos_profile_to_dp_map(device_id, qos_profile, dp_num);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_dx_get_qos_profile_to_dp_map(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   qos_profile,
    SAND_OUT    unsigned long   *dp_num_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_dx_get_qos_profile_to_dp_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with qos_profile [0 / 127] and not NULL dp_num_ptr.
    Expected: SAND_OK.
    1.2. Call with qos_profile [128] (out of range) and other  param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with dp_num_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   qos_profile = 0;
    SAND_U32   dp_num = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with qos_profile [0 / 127] and not NULL dp_num_ptr.
            Expected: SAND_OK.
        */
        /* iterate with qos_profile = 0 */
        qos_profile = 0;

        result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, &dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qos_profile);

        /* iterate with qos_profile = 127 */
        qos_profile = 127;

        result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, &dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qos_profile);

        /*
            1.2. Call with qos_profile [128] and other  param from 1.1.
            Expected: NOT SAND_OK.
        */
        qos_profile = 128;

        result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, &dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qos_profile);

        qos_profile = 0;

        /*
            1.3. Call with dp_num_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, dp_num_ptr = NULL", device_id);
    }

    qos_profile = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, &dp_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, &dp_num);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_set_coexist_mode(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned int    coexist_enable
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_set_coexist_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with coexist_enable[TRUE / FALSE].
    Expected: SAND_OK.
    1.2. Call fx950_api_labeling_get_coexist_mode with not NULL coexist_enable_ptr.
    Expected: SAND_OK and the same param as was set.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  coexist_enable = 0;
    SAND_UINT  coexist_enableGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with coexist_enable[TRUE / FALSE].
            Expected: SAND_OK.
        */
        /* iterate with coexist_enable = TRUE */
        coexist_enable = TRUE;

        result = fx950_api_labeling_set_coexist_mode(device_id, coexist_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, coexist_enable);

        /*
            1.2. Call fx950_api_labeling_get_coexist_mode with not NULL coexist_enable_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_labeling_get_coexist_mode(device_id, &coexist_enableGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_coexist_mode: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(coexist_enable, coexist_enableGet, "got another coexist_enable than was set: %d", device_id);

        /* iterate with coexist_enable = FALSE */
        coexist_enable = FALSE;

        result = fx950_api_labeling_set_coexist_mode(device_id, coexist_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, coexist_enable);

        /*
            1.2. Call fx950_api_labeling_get_coexist_mode with not NULL coexist_enable_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_labeling_get_coexist_mode(device_id, &coexist_enableGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_labeling_get_coexist_mode: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(coexist_enable, coexist_enableGet, "got another coexist_enable than was set: %d", device_id);
    }

    coexist_enable = TRUE;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_set_coexist_mode(device_id, coexist_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_set_coexist_mode(device_id, coexist_enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_labeling_get_coexist_mode(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    unsigned int    *coexist_enable_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_labeling_get_coexist_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null coexist_enable_ptr.
    Expected: SAND_OK.
    1.2. Call with coexist_enable_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  coexist_enable = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null coexist_enable_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_labeling_get_coexist_mode(device_id, &coexist_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with coexist_enable_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_labeling_get_coexist_mode(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, coexist_enable = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_labeling_get_coexist_mode(device_id, &coexist_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_labeling_get_coexist_mode(device_id, &coexist_enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_labeling suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_labeling)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_dx_get_analyzer_target_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_dx_get_control_packets_dp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_dx_get_mc_id_assignment)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_dx_get_qos_profile_to_dp_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_dx_set_analyzer_target_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_dx_set_control_packets_dp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_dx_set_mc_id_assignment)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_dx_set_qos_profile_to_dp_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_add_mc_header_enable)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_coexist_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_cpu_code_label_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_ingress_labeling_parameters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_link_base_queue_number)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_mc_base_queue_number)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_priority_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_qos_to_prio_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_get_uc_label_tbl_entry)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_add_mc_header_enable)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_coexist_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_cpu_code_label_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_ingress_labeling_parameters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_link_base_queue_number)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_mc_base_queue_number)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_priority_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_qos_to_prio_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_labeling_set_uc_label_tbl_entry)
UTF_SUIT_END_TESTS_MAC(fx950_api_labeling)


