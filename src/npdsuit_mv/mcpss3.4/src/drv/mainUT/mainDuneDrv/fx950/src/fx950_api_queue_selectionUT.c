/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_queue_selectionUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_queue_selection, that provides
*       egress queue selection API of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_queue_selection.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_QUEUE_SELECTION_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_set_mc_identification_mode(
    SAND_IN     unsigned int                                     device_id,
    SAND_IN     FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE  id_mode
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_set_mc_identification_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with id_mode [FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_FA_MC_ID /
                            FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MH_VID /
                            FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MH_VIDX].
    Expected: SAND_OK.
    1.2. Call fx950_api_queue_selection_get_mc_identification_mode with not NULL id_mode_ptr.
    Expectede: SAND_OK and the same param as was set.
    1.3. Call with id_mode [0x5AAAAAA5] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE  id_mode = FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_FA_MC_ID;
    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE  id_modeGet = FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_FA_MC_ID;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with id_mode [FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_FA_MC_ID /
                                    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MH_VID /
                                    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MH_VIDX].
            Expected: SAND_OK.
        */
        /* iterate with FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_FA_MC_ID */
        id_mode = FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_FA_MC_ID;

        result = fx950_api_queue_selection_set_mc_identification_mode(device_id, id_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, id_mode);

        /*
            1.2. Call fx950_api_queue_selection_get_mc_identification_mode with not NULL id_mode_ptr.
            Expectede: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_mc_identification_mode(device_id, &id_modeGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_mc_identification_mode: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(id_mode, id_modeGet, "got another id_mode than was set: %d", device_id);

        /* iterate with FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MH_VID */
        id_mode = FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MH_VID;

        result = fx950_api_queue_selection_set_mc_identification_mode(device_id, id_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, id_mode);

        /*
            1.2. Call fx950_api_queue_selection_get_mc_identification_mode with not NULL id_mode_ptr.
            Expectede: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_mc_identification_mode(device_id, &id_modeGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_mc_identification_mode: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(id_mode, id_modeGet, "got another id_mode than was set: %d", device_id);

        /* iterate with FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MH_VIDX */
        id_mode = FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MH_VIDX;

        result = fx950_api_queue_selection_set_mc_identification_mode(device_id, id_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, id_mode);

        /*
            1.2. Call fx950_api_queue_selection_get_mc_identification_mode with not NULL id_mode_ptr.
            Expectede: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_mc_identification_mode(device_id, &id_modeGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_mc_identification_mode: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(id_mode, id_modeGet, "got another id_mode than was set: %d", device_id);

        /*
            1.3. Call with id_mode [0x5AAAAAA5].
            Expected: NOT SAND_OK.
        */
        id_mode = SAND_UTF_QUEUE_SELECTION_INVALID_ENUM;

        result = fx950_api_queue_selection_set_mc_identification_mode(device_id, id_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, id_mode);
    }

    id_mode = FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_FA_MC_ID;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_set_mc_identification_mode(device_id, id_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_set_mc_identification_mode(device_id, id_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_get_mc_identification_mode(
    SAND_IN     unsigned int                                     device_id,
    SAND_OUT    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE  *id_mode_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_get_mc_identification_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null id_mode_ptr.
    Expected: SAND_OK.
    1.2. Call with id_mode_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE id_mode = FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_FA_MC_ID;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null id_mode_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_queue_selection_get_mc_identification_mode(device_id, &id_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with id_mode_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_get_mc_identification_mode(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, id_mode = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_get_mc_identification_mode(device_id, &id_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_get_mc_identification_mode(device_id, &id_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_set_mc_id_range_to_link_map(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     unsigned long                           start_mc_index,
    SAND_IN     unsigned long                           num_of_mc_indexes,
    SAND_IN     FX950_QUEUE_SELECTION_MC_ID_LINK_MODE   link_mode
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_set_mc_id_range_to_link_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with start_mc_index[0 / 8K - 1],
                   num_of_mc_indexes[4K / 1],
                   link_mode[FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_BOTH_DISABLED /
                             FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_LINK0_ENABLED / 
                             FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_LINK1_ENABLED].
    Expected: SAND_OK.
    1.2. Call fx950_api_queue_selection_get_mc_id_to_link_map with not NULL link_mode_ptr and start_mc_index from 1.1.
    Expected: SAND_OK and the same param from 1.1.
    1.3. Call with start_mc_index [8K + 1] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with num_of_mc_indexes [8K + 1] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with start_mc_index [4K + 1], num_of_mc_indexes [4K + 1] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with link_mode [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   start_mc_index = 0;
    SAND_U32   num_of_mc_indexes = 0;

    FX950_QUEUE_SELECTION_MC_ID_LINK_MODE link_mode = FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_BOTH_DISABLED;
    FX950_QUEUE_SELECTION_MC_ID_LINK_MODE link_modeGet = FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_BOTH_DISABLED;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with start_mc_index[0 / 4K / 8K - 1],
                           num_of_mc_indexes[1 / 4K / 1],
                           link_mode[FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_BOTH_DISABLED /
                                     FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_LINK0_ENABLED / 
                                     FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_LINK1_ENABLED].
            Expected: SAND_OK.
        */
        /* iterate with start_mc_index = 0 */
        start_mc_index = 0;
        num_of_mc_indexes = 1;
        link_mode = FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_BOTH_DISABLED;

        result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, start_mc_index, num_of_mc_indexes, link_mode);

        /*
            1.2. Call fx950_api_queue_selection_get_mc_id_to_link_map with not NULL link_mode_ptr and start_mc_index from 1.1.
            Expected: SAND_OK and the same param from 1.1.
        */
        result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, start_mc_index, &link_modeGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_mc_id_to_link_map: %d, %d", device_id, start_mc_index);
        UTF_VERIFY_EQUAL1_STRING_MAC(link_mode, link_modeGet, "got another link_mode than was set: %d", device_id);

        /* iterate with start_mc_index = 4K */
        start_mc_index = 0xFFF;
        num_of_mc_indexes = 0xFFF;
        link_mode = FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_LINK0_ENABLED;

        result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, start_mc_index, num_of_mc_indexes, link_mode);

        /*
            1.2. Call fx950_api_queue_selection_get_mc_id_to_link_map with not NULL link_mode_ptr and start_mc_index from 1.1.
            Expected: SAND_OK and the same param from 1.1.
        */
        result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, start_mc_index, &link_modeGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_mc_id_to_link_map: %d, %d", device_id, start_mc_index);
        UTF_VERIFY_EQUAL1_STRING_MAC(link_mode, link_modeGet, "got another link_mode than was set: %d", device_id);

        /* iterate with start_mc_index = 8K - 1 */
        start_mc_index = 0x1FFF - 1;
        num_of_mc_indexes = 1;
        link_mode = FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_LINK1_ENABLED;

        result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, start_mc_index, num_of_mc_indexes, link_mode);

        /*
            1.2. Call fx950_api_queue_selection_get_mc_id_to_link_map with not NULL link_mode_ptr and start_mc_index from 1.1.
            Expected: SAND_OK and the same param from 1.1.
        */
        result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, start_mc_index, &link_modeGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_mc_id_to_link_map: %d, %d", device_id, start_mc_index);
        UTF_VERIFY_EQUAL1_STRING_MAC(link_mode, link_modeGet, "got another link_mode than was set: %d", device_id);

        /*
            1.3. Call with start_mc_index [8k+1] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        start_mc_index = 0x2000;

        result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, start_mc_index);

        start_mc_index = 0;

        /*
            1.4. Call with num_of_mc_indexes [8K + 1] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        num_of_mc_indexes = 0x2000 + 1;

        result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, num_of_mc_indexes = %d", device_id, num_of_mc_indexes);

        num_of_mc_indexes = 0;

        /*
            1.5. Call with start_mc_index [4K + 1], num_of_mc_indexes [4K + 1] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        start_mc_index = 0xFFF + 1;
        num_of_mc_indexes = 0xFFF + 1;

        result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, start_mc_index, num_of_mc_indexes);

        start_mc_index = 0;
        num_of_mc_indexes = 1;

        /*
            1.6. Call with link_mode [0x5AAAAAA5] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        link_mode = SAND_UTF_QUEUE_SELECTION_INVALID_ENUM;

        result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link_mode = %d", device_id, link_mode);
    }

    start_mc_index = 0;
    num_of_mc_indexes = 1;
    link_mode = FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_BOTH_DISABLED;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_get_mc_id_to_link_map(
    SAND_IN     unsigned int                             device_id,
    SAND_IN     unsigned long                            mc_id,
    SAND_OUT    FX950_QUEUE_SELECTION_MC_ID_LINK_MODE    *link_mode_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_get_mc_id_to_link_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with mc_id [0 / 2K], non-null link_mode_ptr.
    Expected: SAND_OK.
    1.2. CAll with mc_id [2k + 1] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with link_mode_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   mc_id = 0;
    FX950_QUEUE_SELECTION_MC_ID_LINK_MODE link_mode = FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_BOTH_DISABLED;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with mc_id [0 / 2K], non-null link_mode_ptr.
            Expected: SAND_OK.
        */
        /* iterate with mc_id = 0 */
        mc_id = 0;

        result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, mc_id, &link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mc_id);

        /* iterate with mc_id = 2k */
        mc_id = 0x7FF;

        result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, mc_id, &link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mc_id);

        /*
            1.2. CAll with mc_id [2k + 1] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        mc_id = 0x800;

        result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, mc_id, &link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, mc_id);

        mc_id = 0;

        /*
            1.3. Call with link_mode_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, mc_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_mode = NULL", device_id);
    }

    mc_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, mc_id, &link_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, mc_id, &link_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   start_flow_id_index,
    SAND_IN     unsigned long   num_of_flow_id_indexes,
    SAND_IN     unsigned long   link,
    SAND_IN     unsigned long   priority
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_set_flow_id_range_to_link_prio_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with start_flow_id_index[0 / 16k],
                   num_of_flow_id_indexes[0 / 16k],
                   link [0 / 1],
                   priority[0 / 1].
    Expected: SAND_OK.
    1.2. Call fx950_api_queue_selection_get_flow_id_to_link_prio_map with start_flow_id_index from 1.1,
              not NULL link_ptr and priority_ptr.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with start_flow_id_index [16k+1] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with num_of_flow_id_indexes [16k+1] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with link [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with priority [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   start_flow_id_index = 0;
    SAND_U32   num_of_flow_id_indexes = 0;
    SAND_U32   link = 0;
    SAND_U32   priority = 0;

    SAND_U32   linkGet = 0;
    SAND_U32   priorityGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with start_flow_id_index[0 / 8k],
                           num_of_flow_id_indexes[1 / 8k],
                           link [0 / 1],
                           priority[0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with start_flow_id_index = 0 */
        start_flow_id_index = 0;
        num_of_flow_id_indexes = 1;
        link = 0;
        priority = 0;

        result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);

        /*
            1.2. Call fx950_api_queue_selection_get_flow_id_to_link_prio_map with start_flow_id_index from 1.1,
                      not NULL link_ptr and priority_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, start_flow_id_index, &linkGet, &priorityGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_flow_id_to_link_prio_map: %d, %d", device_id, start_flow_id_index);

        UTF_VERIFY_EQUAL1_STRING_MAC(link, linkGet, "got another link than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority, priorityGet, "got another priority than was set: %d", device_id);

        /* iterate with start_flow_id_index = 16k */
        start_flow_id_index = 0x2000;
        num_of_flow_id_indexes = 0x2000;
        link = 1;
        priority = 1;

        result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);

        /*
            1.2. Call fx950_api_queue_selection_get_flow_id_to_link_prio_map with start_flow_id_index from 1.1,
                      not NULL link_ptr and priority_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, start_flow_id_index, &linkGet, &priorityGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_flow_id_to_link_prio_map: %d, %d", device_id, start_flow_id_index);

        UTF_VERIFY_EQUAL1_STRING_MAC(link, linkGet, "got another link than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(priority, priorityGet, "got another priority than was set: %d", device_id);

        /*
            1.3. Call with start_flow_id_index [16k+1] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        start_flow_id_index = 0x4000;

        result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, start_flow_id_index = %d", device_id, start_flow_id_index);

        start_flow_id_index = 0;

        /*
            1.4. Call with num_of_flow_id_indexes [16k+1] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        num_of_flow_id_indexes = 0x4000 + 1;

        result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, num_of_flow_id_indexes = %d", device_id, num_of_flow_id_indexes);

        num_of_flow_id_indexes = 0;

        /*
            1.5. Call with link [2] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link = %d", device_id, link);

        link = 0;

        /*
            1.6. Call with priority [2] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        priority = 2;

        result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, priority = %d", device_id, priority);
    }

    start_flow_id_index = 0;
    num_of_flow_id_indexes = 0;
    link = 0;
    priority = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_get_flow_id_to_link_prio_map(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   flow_id,
    SAND_OUT    unsigned long   *link_ptr,
    SAND_OUT    unsigned long   *priority_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_get_flow_id_to_link_prio_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with flow_id[0 / 16k], non-null link_ptr and priority_ptr.
    Expected: SAND_OK.
    1.2. Call with flow_id [16k+1] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with link_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with priority_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   flow_id = 0;
    SAND_U32   link = 0;
    SAND_U32   priority = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with flow_id[0 / 16k], non-null link_ptr and priority_ptr.
            Expected: SAND_OK.
        */
        /* iterate with flow_id = 0 */
        flow_id = 0;

        result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, flow_id, &link, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, flow_id);

        /* iterate with flow_id = 16k */
        flow_id = 0x3FFF;

        result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, flow_id, &link, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, flow_id);

        /*
            1.2. Call with flow_id[16k+1] and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        flow_id = 0x4000;

        result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, flow_id, &link, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, flow_id);

        flow_id = 0;

        /*
            1.3. Call with link_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, flow_id, NULL, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_ptr = NULL", device_id);

        /*
            1.4. Call with priority_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, flow_id, &link, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, priority_ptr = NULL", device_id);
    }

    flow_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, flow_id, &link, &priority);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, flow_id, &link, &priority);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_IN     FX950_BMP_256   *cpu_code_to_link_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with cpu_code_to_link_bmp { bitmap [0,0,0,0,0,0,0,0 / 0xFFFFFFFF,...,0xFFFFFFFF] }.
    Expected: SAND_OK.
    1.2. Call fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp with not NULL cpu_code_to_link_bmp_ptr.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with cpu_code_to_link_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT       device_id = 0;
    FX950_BMP_256   cpu_code_to_link_bmp;
    FX950_BMP_256   cpu_code_to_link_bmpGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with cpu_code_to_link_bmp { bitmap [0,0,0,0,0,0,0,0 / 0xFFFFFFFF,...,0xFFFFFFFF] }.
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        cpu_code_to_link_bmp.bitmap[0] = 0;
        cpu_code_to_link_bmp.bitmap[1] = 0;
        cpu_code_to_link_bmp.bitmap[2] = 0;
        cpu_code_to_link_bmp.bitmap[3] = 0;
        cpu_code_to_link_bmp.bitmap[4] = 0;
        cpu_code_to_link_bmp.bitmap[5] = 0;
        cpu_code_to_link_bmp.bitmap[6] = 0;
        cpu_code_to_link_bmp.bitmap[7] = 0;

        result = fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp with not NULL cpu_code_to_link_bmp_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmpGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[0], cpu_code_to_link_bmpGet.bitmap[0],
                                     "got another cpu_code_to_link_bmp.bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[1], cpu_code_to_link_bmpGet.bitmap[1],
                                     "got another cpu_code_to_link_bmp.bitmap[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[2], cpu_code_to_link_bmpGet.bitmap[2],
                                     "got another cpu_code_to_link_bmp.bitmap[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[3], cpu_code_to_link_bmpGet.bitmap[3],
                                     "got another cpu_code_to_link_bmp.bitmap[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[4], cpu_code_to_link_bmpGet.bitmap[4],
                                     "got another cpu_code_to_link_bmp.bitmap[4] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[5], cpu_code_to_link_bmpGet.bitmap[5],
                                     "got another cpu_code_to_link_bmp.bitmap[5] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[6], cpu_code_to_link_bmpGet.bitmap[6],
                                     "got another cpu_code_to_link_bmp.bitmap[6] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[7], cpu_code_to_link_bmpGet.bitmap[7],
                                     "got another cpu_code_to_link_bmp.bitmap[7] than was set: %d", device_id);

        /* iterate with 0xFFFFFFFF */
        cpu_code_to_link_bmp.bitmap[0] = 0xFFFFFFFF;
        cpu_code_to_link_bmp.bitmap[1] = 0xFFFFFFFF;
        cpu_code_to_link_bmp.bitmap[2] = 0xFFFFFFFF;
        cpu_code_to_link_bmp.bitmap[3] = 0xFFFFFFFF;
        cpu_code_to_link_bmp.bitmap[4] = 0xFFFFFFFF;
        cpu_code_to_link_bmp.bitmap[5] = 0xFFFFFFFF;
        cpu_code_to_link_bmp.bitmap[6] = 0xFFFFFFFF;
        cpu_code_to_link_bmp.bitmap[7] = 0xFFFFFFFF;

        result = fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp with not NULL cpu_code_to_link_bmp_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmpGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[0], cpu_code_to_link_bmpGet.bitmap[0],
                                     "got another cpu_code_to_link_bmp.bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[1], cpu_code_to_link_bmpGet.bitmap[1],
                                     "got another cpu_code_to_link_bmp.bitmap[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[2], cpu_code_to_link_bmpGet.bitmap[2],
                                     "got another cpu_code_to_link_bmp.bitmap[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[3], cpu_code_to_link_bmpGet.bitmap[3],
                                     "got another cpu_code_to_link_bmp.bitmap[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[4], cpu_code_to_link_bmpGet.bitmap[4],
                                     "got another cpu_code_to_link_bmp.bitmap[4] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[5], cpu_code_to_link_bmpGet.bitmap[5],
                                     "got another cpu_code_to_link_bmp.bitmap[5] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[6], cpu_code_to_link_bmpGet.bitmap[6],
                                     "got another cpu_code_to_link_bmp.bitmap[6] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpu_code_to_link_bmp.bitmap[7], cpu_code_to_link_bmpGet.bitmap[7],
                                     "got another cpu_code_to_link_bmp.bitmap[7] than was set: %d", device_id);

        /*
            1.3. Call with cpu_code_to_link_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, cpu_code_to_link_bmp_ptr = NULL", device_id);
    }

    cpu_code_to_link_bmp.bitmap[0] = 0;
    cpu_code_to_link_bmp.bitmap[1] = 0;
    cpu_code_to_link_bmp.bitmap[2] = 0;
    cpu_code_to_link_bmp.bitmap[3] = 0;
    cpu_code_to_link_bmp.bitmap[4] = 0;
    cpu_code_to_link_bmp.bitmap[5] = 0;
    cpu_code_to_link_bmp.bitmap[6] = 0;
    cpu_code_to_link_bmp.bitmap[7] = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    FX950_BMP_256   *cpu_code_to_link_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null cpu_code_to_link_bmp_ptr.
    Expected: SAND_OK.
    1.2. Call with cpu_code_to_link_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_BMP_256 cpu_code_to_link_bmp;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null cpu_code_to_link_bmp_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with cpu_code_to_link_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, cpu_code_to_link_bmp_ptr = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_set_cpu_code_to_link(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       cpu_code,
    SAND_IN     unsigned long       link
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_set_cpu_code_to_link)
{
/*
    ITERATE_DEVICES
    1.1. Call with cpu_code [0 / 255], link [0 / 1].
    Expected: SAND_OK.
    1.2. Call with cpu_code [256] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with link [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   cpu_code = 0;
    SAND_U32   link = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with cpu_code [0 / 255], link [0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with cpu_code = 0 */
        cpu_code = 0;
        link = 0;

        result = fx950_api_queue_selection_set_cpu_code_to_link(device_id, cpu_code, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, cpu_code, link);

        /* iterate with cpu_code = 255 */
        cpu_code = 255;
        link = 1;

        result = fx950_api_queue_selection_set_cpu_code_to_link(device_id, cpu_code, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, cpu_code, link);

        /*
            1.2. Call with cpu_code [256] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        cpu_code = 256;

        result = fx950_api_queue_selection_set_cpu_code_to_link(device_id, cpu_code, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, cpu_code);

        cpu_code = 0;

        /*
            1.3. Call with link [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_queue_selection_set_cpu_code_to_link(device_id, cpu_code, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, cpu_code, link);
    }

    cpu_code = 0;
    link = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_set_cpu_code_to_link(device_id, cpu_code, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_set_cpu_code_to_link(device_id, cpu_code, link);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_set_ports_to_link_map_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_IN     FX950_BMP_64    *port_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_set_ports_to_link_map_bmp)
{
/*
    1.1. Call with port_bmp_ptr { bitmap [0,0 / 0xFFFFFFFF,0xFFFFFFFF] }.
    Expected: SAND_OK.
    1.2. Call fx950_api_queue_selection_get_ports_to_link_map_bmp with not NULL port_bmp_ptr.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with port_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_BMP_64 port_bmp;
    FX950_BMP_64 port_bmpGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port_bmp_ptr { bitmap [0,0 / 0xFFFFFFFF,0xFFFFFFFF] }.
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        port_bmp.bitmap[0] = 0;
        port_bmp.bitmap[1] = 0;

        result = fx950_api_queue_selection_set_ports_to_link_map_bmp(device_id, &port_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_queue_selection_get_ports_to_link_map_bmp with not NULL port_bmp_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_ports_to_link_map_bmp(device_id, &port_bmpGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_ports_to_link_map_bmp: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(port_bmp.bitmap[0], port_bmpGet.bitmap[0],
                                     "got another port_bmp.bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_bmp.bitmap[1], port_bmpGet.bitmap[1],
                                     "got another port_bmp.bitmap[1] than was set: %d", device_id);

        /* iterate with 0xFFFFFFFF */
        port_bmp.bitmap[0] = 0xFFFFFFFF;
        port_bmp.bitmap[1] = 0xFFFFFFFF;

        result = fx950_api_queue_selection_set_ports_to_link_map_bmp(device_id, &port_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_queue_selection_get_ports_to_link_map_bmp with not NULL port_bmp_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_queue_selection_get_ports_to_link_map_bmp(device_id, &port_bmpGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_ports_to_link_map_bmp: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(port_bmp.bitmap[0], port_bmpGet.bitmap[0],
                                     "got another port_bmp.bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_bmp.bitmap[1], port_bmpGet.bitmap[1],
                                     "got another port_bmp.bitmap[1] than was set: %d", device_id);

        /*
            1.3. Call with port_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_set_ports_to_link_map_bmp(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_bmp = NULL", device_id);
    }

    port_bmp.bitmap[0] = 0;
    port_bmp.bitmap[1] = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_set_ports_to_link_map_bmp(device_id, &port_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_set_ports_to_link_map_bmp(device_id, &port_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_get_ports_to_link_map_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    FX950_BMP_64    *port_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_get_ports_to_link_map_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null port_bmp_ptr.
    Expected: SAND_OK.
    1.2. Call with port_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT    device_id = 0;
    FX950_BMP_64 port_bmp;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null countValuePtr.
            Expected: SAND_OK.
        */
        result = fx950_api_queue_selection_get_ports_to_link_map_bmp(device_id, &port_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with port_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_set_ports_to_link_map_bmp(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_bmp = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_get_ports_to_link_map_bmp(device_id, &port_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_get_ports_to_link_map_bmp(device_id, &port_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_set_port_to_link_map(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port_num,
    SAND_IN     unsigned long   link
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_set_port_to_link_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with port_num [0 / 63], link [0 / 1].
    Expected: SAND_OK.
    1.2. Call with port_num [64] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with link [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port_num = 0;
    SAND_U32   link = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port_num [0 / 63], link [0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with port_num = 0 */
        port_num = 0;
        link = 0;

        result = fx950_api_queue_selection_set_port_to_link_map(device_id, port_num, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port_num, link);

        /* iterate with port_num = 63 */
        port_num = 63;
        link = 1;

        result = fx950_api_queue_selection_set_port_to_link_map(device_id, port_num, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port_num, link);

        /*
            1.2. Call with port_num [64] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port_num = 64;

        result = fx950_api_queue_selection_set_port_to_link_map(device_id, port_num, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_num);

        port_num = 0;

        /*
            1.3. Call with link [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        link = 2;

        result = fx950_api_queue_selection_set_port_to_link_map(device_id, port_num, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link = %d", device_id, link);
    }

    port_num = 0;
    link = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_set_port_to_link_map(device_id, port_num, link);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_set_port_to_link_map(device_id, port_num, link);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_dx_set_analyzer_link_prio_map(
    SAND_IN     unsigned int     device_id,
    SAND_IN     unsigned long    ingress_analyzer_link,
    SAND_IN     unsigned long    egress_analyzer_link,
    SAND_IN     unsigned long    to_analyzer_prio
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_dx_set_analyzer_link_prio_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with  ingress_analyzer_link[0 / 1],
                    egress_analyzer_link[0 / 1],
                    to_analyzer_prio[0 / 1].
    Expected: SAND_OK.
    1.2. Call fx950_api_queue_selection_dx_set_analyzer_link_prio_map with noy NULL pointers.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with ingress_analyzer_link [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with egress_analyzer_link [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with to_analyzer_prio [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   ingress_analyzer_link = 0;
    SAND_U32   egress_analyzer_link = 0;
    SAND_U32   to_analyzer_prio = 0;

    SAND_U32   ingress_analyzer_linkGet = 0;
    SAND_U32   egress_analyzer_linkGet = 0;
    SAND_U32   to_analyzer_prioGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with  ingress_analyzer_link[0 / 1],
                            egress_analyzer_link[0 / 1],
                            to_analyzer_prio[0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        ingress_analyzer_link = 0;
        egress_analyzer_link = 0;
        to_analyzer_prio = 0;

        result = fx950_api_queue_selection_dx_set_analyzer_link_prio_map(device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);

        /*
            1.2. Call fx950_api_queue_selection_dx_set_analyzer_link_prio_map with noy NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, &ingress_analyzer_linkGet, &egress_analyzer_linkGet, &to_analyzer_prioGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_dx_get_analyzer_link_prio_map: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(ingress_analyzer_link, ingress_analyzer_linkGet,
                                     "got another ingress_analyzer_link than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(egress_analyzer_link, egress_analyzer_linkGet,
                                     "got another egress_analyzer_link than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(to_analyzer_prio, to_analyzer_prioGet,
                                     "got another to_analyzer_prio than was set: %d", device_id);

        /* iterate with 1 */
        ingress_analyzer_link = 1;
        egress_analyzer_link = 1;
        to_analyzer_prio = 1;

        result = fx950_api_queue_selection_dx_set_analyzer_link_prio_map(device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);

        /*
            1.2. Call fx950_api_queue_selection_dx_set_analyzer_link_prio_map with noy NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, &ingress_analyzer_linkGet, &egress_analyzer_linkGet, &to_analyzer_prioGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_dx_get_analyzer_link_prio_map: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(ingress_analyzer_link, ingress_analyzer_linkGet,
                                     "got another ingress_analyzer_link than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(egress_analyzer_link, egress_analyzer_linkGet,
                                     "got another egress_analyzer_link than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(to_analyzer_prio, to_analyzer_prioGet,
                                     "got another to_analyzer_prio than was set: %d", device_id);

        /*
            1.3. Call with ingress_analyzer_link [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        ingress_analyzer_link = 2;

        result = fx950_api_queue_selection_dx_set_analyzer_link_prio_map(device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, ingress_analyzer_link = %d", device_id, ingress_analyzer_link);

        ingress_analyzer_link = 0;

        /*
            1.4. Call with egress_analyzer_link [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        egress_analyzer_link = 2;

        result = fx950_api_queue_selection_dx_set_analyzer_link_prio_map(device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, egress_analyzer_link = %d", device_id, egress_analyzer_link);

        egress_analyzer_link = 0;

        /*
            1.5. Call with to_analyzer_prio [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        to_analyzer_prio = 2;

        result = fx950_api_queue_selection_dx_set_analyzer_link_prio_map(device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, to_analyzer_prio = %d", device_id, to_analyzer_prio);
    }

    ingress_analyzer_link = 0;
    egress_analyzer_link = 0;
    to_analyzer_prio = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_dx_set_analyzer_link_prio_map(device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_dx_set_analyzer_link_prio_map(device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_dx_get_analyzer_link_prio_map(
    SAND_IN     unsigned int     device_id,
    SAND_OUT    unsigned long    *ingress_analyzer_link_ptr,
    SAND_OUT    unsigned long    *egress_analyzer_link_ptr,
    SAND_OUT    unsigned long    *to_analyzer_prio_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_dx_get_analyzer_link_prio_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null pointers.
    Expected: SAND_OK.
    1.2. Call with ingress_analyzer_link_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.3. Call with egress_analyzer_link_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with to_analyzer_prio_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   ingress_analyzer_link = 0;
    SAND_U32   egress_analyzer_link = 0;
    SAND_U32   to_analyzer_prio = 0;


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
        result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, &ingress_analyzer_link, &egress_analyzer_link, &to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with ingress_analyzer_link_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, NULL, &egress_analyzer_link, &to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ingress_analyzer_link = NULL", device_id);

        /*
            1.3. Call with egress_analyzer_link_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, &ingress_analyzer_link, NULL, &to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, egress_analyzer_link = NULL", device_id);

        /*
            1.4. Call with to_analyzer_prio_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, &ingress_analyzer_link, &egress_analyzer_link, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, to_analyzer_prio = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, &ingress_analyzer_link, &egress_analyzer_link, &to_analyzer_prio);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, &ingress_analyzer_link, &egress_analyzer_link, &to_analyzer_prio);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_set_prio_mapping(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     FX950_QUEUE_SELECTION_PRIO_MAP_STRUCT   *prio_map_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_set_prio_mapping)
{
/*
    ITERATE_DEVICES
    1.1. Call with prio_map_ptr {   to_cpu_priority_queue_num [0 / 1],
                                    tc_to_prio_bmp [0 / 0xFF],
                                    uc_prio_0_queue [0 / 3],
                                    uc_prio_1_queue [0 / 3],
                                    mc_prio_0_queue [0 / 3],
                                    mc_prio_1_queue [0 / 3] }
    Expected: SAND_OK.
    1.2. Call fx950_api_queue_selection_get_prio_mapping with not NULL prio_map_ptr.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with prio_map_ptr->to_cpu_priority_queue_num [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with prio_map_ptr->tc_to_prio_bmp [0x100] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with prio_map_ptr->uc_prio_0_queue [4] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with prio_map_ptr->uc_prio_1_queue [4] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with prio_map_ptr->mc_prio_0_queue [4] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with prio_map_ptr->mc_prio_1_queue [4] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.9. Call with countValuePtr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_QUEUE_SELECTION_PRIO_MAP_STRUCT prio_map;
    FX950_QUEUE_SELECTION_PRIO_MAP_STRUCT prio_mapGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with prio_map_ptr {   to_cpu_priority_queue_num [0 / 1],
                                            tc_to_prio_bmp [0 / 0xFF],
                                            uc_prio_0_queue [0 / 3],
                                            uc_prio_1_queue [0 / 3],
                                            mc_prio_0_queue [0 / 3],
                                            mc_prio_1_queue [0 / 3] }
            Expected: SAND_OK.
        */
        /* iterate with to_cpu_priority_queue_num = 0 */
        prio_map.to_cpu_priority_queue_num = 0;
        prio_map.tc_to_prio_bmp = 0;
        prio_map.uc_prio_0_queue = 0;
        prio_map.uc_prio_1_queue = 0;
        prio_map.mc_prio_0_queue = 0;
        prio_map.mc_prio_1_queue = 0;

        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_queue_selection_get_prio_mapping with not NULL prio_map_ptr.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_queue_selection_get_prio_mapping(device_id, &prio_mapGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_prio_mapping: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.to_cpu_priority_queue_num, prio_map.to_cpu_priority_queue_num,
                                     "got another prio_map.to_cpu_priority_queue_num than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.tc_to_prio_bmp, prio_map.tc_to_prio_bmp,
                                     "got another prio_map.tc_to_prio_bmp than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.uc_prio_0_queue, prio_map.uc_prio_0_queue,
                                     "got another prio_map.uc_prio_0_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.uc_prio_1_queue, prio_map.uc_prio_1_queue,
                                     "got another prio_map.uc_prio_1_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.mc_prio_0_queue, prio_map.mc_prio_0_queue,
                                     "got another prio_map.mc_prio_0_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.mc_prio_1_queue, prio_map.mc_prio_1_queue,
                                     "got another prio_map.mc_prio_1_queue than was set: %d", device_id);

        /* iterate with to_cpu_priority_queue_num = 1 */
        prio_map.to_cpu_priority_queue_num = 1;
        prio_map.tc_to_prio_bmp = 0xFF;
        prio_map.uc_prio_0_queue = 3;
        prio_map.uc_prio_1_queue = 3;
        prio_map.mc_prio_0_queue = 3;
        prio_map.mc_prio_1_queue = 3;

        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_queue_selection_get_prio_mapping with not NULL prio_map_ptr.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_queue_selection_get_prio_mapping(device_id, &prio_mapGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_get_prio_mapping: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.to_cpu_priority_queue_num, prio_map.to_cpu_priority_queue_num,
                                     "got another prio_map.to_cpu_priority_queue_num than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.tc_to_prio_bmp, prio_map.tc_to_prio_bmp,
                                     "got another prio_map.tc_to_prio_bmp than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.uc_prio_0_queue, prio_map.uc_prio_0_queue,
                                     "got another prio_map.uc_prio_0_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.uc_prio_1_queue, prio_map.uc_prio_1_queue,
                                     "got another prio_map.uc_prio_1_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.mc_prio_0_queue, prio_map.mc_prio_0_queue,
                                     "got another prio_map.mc_prio_0_queue than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(prio_map.mc_prio_1_queue, prio_map.mc_prio_1_queue,
                                     "got another prio_map.mc_prio_1_queue than was set: %d", device_id);

        /*
            1.3. Call with prio_map_ptr->to_cpu_priority_queue_num [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        prio_map.to_cpu_priority_queue_num = 2;

        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, prio_map.to_cpu_priority_queue_num = %d",
                                         device_id, prio_map.to_cpu_priority_queue_num);

        prio_map.to_cpu_priority_queue_num = 0;

        /*
            1.4. Call with prio_map_ptr->tc_to_prio_bmp [0x100] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        prio_map.tc_to_prio_bmp = 0x100;

        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, prio_map.tc_to_prio_bmp = %d",
                                         device_id, prio_map.tc_to_prio_bmp);

        prio_map.tc_to_prio_bmp = 0;

        /*
            1.5. Call with prio_map_ptr->uc_prio_0_queue [4] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        prio_map.uc_prio_0_queue = 4;

        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, prio_map.uc_prio_0_queue = %d",
                                         device_id, prio_map.uc_prio_0_queue);

        prio_map.uc_prio_0_queue = 0;

        /*
            1.6. Call with prio_map_ptr->uc_prio_1_queue [4] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        prio_map.uc_prio_1_queue = 4;

        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, prio_map.uc_prio_1_queue = %d",
                                         device_id, prio_map.uc_prio_1_queue);

        prio_map.uc_prio_1_queue = 0;

        /*
            1.7. Call with prio_map_ptr->mc_prio_0_queue [4] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        prio_map.mc_prio_0_queue = 4;

        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, prio_map.mc_prio_0_queue = %d",
                                         device_id, prio_map.mc_prio_0_queue);

        prio_map.mc_prio_0_queue = 0;

        /*
            1.8. Call with prio_map_ptr->mc_prio_1_queue [4] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        prio_map.mc_prio_1_queue = 4;

        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, prio_map.mc_prio_1_queue = %d",
                                         device_id, prio_map.mc_prio_1_queue);

        prio_map.mc_prio_1_queue = 0;

        /*
            1.9. Call with countValuePtr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_set_prio_mapping(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, prio_map = NULL", device_id);
    }

    prio_map.to_cpu_priority_queue_num = 0;
    prio_map.tc_to_prio_bmp = 0;
    prio_map.uc_prio_0_queue = 0;
    prio_map.uc_prio_1_queue = 0;
    prio_map.mc_prio_0_queue = 0;
    prio_map.mc_prio_1_queue = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_get_prio_mapping(
    SAND_IN     unsigned int                            device_id,
    SAND_OUT    FX950_QUEUE_SELECTION_PRIO_MAP_STRUCT   *prio_map_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_get_prio_mapping)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null prio_map_ptr.
    Expected: SAND_OK.
    1.2. Call with prio_map_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_QUEUE_SELECTION_PRIO_MAP_STRUCT prio_map;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null prio_map_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_queue_selection_get_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with prio_map_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_get_prio_mapping(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, prio_map = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_get_prio_mapping(device_id, &prio_map);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_get_prio_mapping(device_id, &prio_map);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_IN     FX950_BMP_128   *qos_profile_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with qos_profile_bmp_ptr { bitmap {0,0,0,0 / 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF } }
    Expected: SAND_OK.
    1.2. Call fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp with not NULL pointers.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with qos_profile_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_BMP_128 qos_profile_bmp;
    FX950_BMP_128 qos_profile_bmpGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with qos_profile_bmp_ptr { bitmap {0,0,0,0 / 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF } }
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        qos_profile_bmp.bitmap[0] = 0;
        qos_profile_bmp.bitmap[1] = 0;
        qos_profile_bmp.bitmap[2] = 0;
        qos_profile_bmp.bitmap[3] = 0;

        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmpGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qos_profile_bmp.bitmap[0], qos_profile_bmpGet.bitmap[0],
                                     "got another qos_profile_bmp.bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(qos_profile_bmp.bitmap[1], qos_profile_bmpGet.bitmap[1],
                                     "got another qos_profile_bmp.bitmap[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(qos_profile_bmp.bitmap[2], qos_profile_bmpGet.bitmap[2],
                                     "got another qos_profile_bmp.bitmap[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(qos_profile_bmp.bitmap[3], qos_profile_bmpGet.bitmap[3],
                                     "got another qos_profile_bmp.bitmap[3] than was set: %d", device_id);

        /* iterate with 0xFFFFFFFF */
        qos_profile_bmp.bitmap[0] = 0xFFFFFFFF;
        qos_profile_bmp.bitmap[1] = 0xFFFFFFFF;
        qos_profile_bmp.bitmap[2] = 0xFFFFFFFF;
        qos_profile_bmp.bitmap[3] = 0xFFFFFFFF;

        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp with not NULL pointers.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmpGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qos_profile_bmp.bitmap[0], qos_profile_bmpGet.bitmap[0],
                                     "got another qos_profile_bmp.bitmap[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(qos_profile_bmp.bitmap[1], qos_profile_bmpGet.bitmap[1],
                                     "got another qos_profile_bmp.bitmap[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(qos_profile_bmp.bitmap[2], qos_profile_bmpGet.bitmap[2],
                                     "got another qos_profile_bmp.bitmap[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(qos_profile_bmp.bitmap[3], qos_profile_bmpGet.bitmap[3],
                                     "got another qos_profile_bmp.bitmap[3] than was set: %d", device_id);

        /*
            1.3. Call with qos_profile_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, qos_profile_bmp_ptr = NULL", device_id);

    }

    qos_profile_bmp.bitmap[0] = 0;
    qos_profile_bmp.bitmap[1] = 0;
    qos_profile_bmp.bitmap[2] = 0;
    qos_profile_bmp.bitmap[3] = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_OUT    FX950_BMP_128   *qos_profile_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null qos_profile_bmp_ptr.
    Expected: SAND_OK.
    1.2. Call with qos_profile_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_BMP_128 qos_profile_bmp;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null qos_profile_bmp_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with qos_profile_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp(device_id, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, qos_profile_bmp_ptr = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       qos_profile,
    SAND_IN     unsigned long       priority_queue_num
  )
*/
UTF_TEST_CASE_MAC(fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue)
{
/*
    ITERATE_DEVICES
    1.1. Call with qos_profile [0 / 127],
                   priority_queue_num [0 / 1].
    Expected: SAND_OK.
    1.2. Call with qos_profile [128] (out of range) and other param from 1.1.
    Expected: SAND_OK.
    1.3. Call with priority_queue_num [2] (out of range) and other param from 1.1.
    Expected: SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   qos_profile = 0;
    SAND_U32   priority_queue_num = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with qos_profile [0 / 127],
                           priority_queue_num [0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with qos_profile = 0 */
        qos_profile = 0;
        priority_queue_num = 0;

        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue(device_id, qos_profile, priority_queue_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, qos_profile, priority_queue_num);

        /* iterate with qos_profile = 127 */
        qos_profile = 127;
        priority_queue_num = 1;

        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue(device_id, qos_profile, priority_queue_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, qos_profile, priority_queue_num);

        /*
            1.2. Call with qos_profile [128] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        qos_profile = 128;

        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue(device_id, qos_profile, priority_queue_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, qos_profile);

        qos_profile = 0;

        /*
            1.3. Call with priority_queue_num [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        priority_queue_num = 2;

        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue(device_id, qos_profile, priority_queue_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, priority_queue_num = %d", device_id, priority_queue_num);
    }

    qos_profile = 0;
    priority_queue_num = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue(device_id, qos_profile, priority_queue_num);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue(device_id, qos_profile, priority_queue_num);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_queue_selection suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_queue_selection)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_set_mc_identification_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_get_mc_identification_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_set_mc_id_range_to_link_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_get_mc_id_to_link_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_set_flow_id_range_to_link_prio_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_get_flow_id_to_link_prio_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_set_cpu_code_to_link)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_set_ports_to_link_map_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_get_ports_to_link_map_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_set_port_to_link_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_dx_set_analyzer_link_prio_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_dx_get_analyzer_link_prio_map)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_set_prio_mapping)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_get_prio_mapping)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue)
UTF_SUIT_END_TESTS_MAC(fx950_api_queue_selection)

