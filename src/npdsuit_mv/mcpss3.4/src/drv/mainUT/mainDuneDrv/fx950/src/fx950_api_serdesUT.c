/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_serdesUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_serdes, that provides
*       Serdes APIs of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_serdes.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>


/* defines */

/* Invalid enum */
#define SAND_UTF_API_SERDES_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_set_swap_select(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_IN     unsigned long   lane,
    SAND_IN     unsigned long   pcs_tx_lane,
    SAND_IN     unsigned long   pcs_rx_lane
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_set_swap_select)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1],
                   lane [0 / 3],
                   pcs_tx_lane [0 / 3],
                   pcs_rx_lane [0 / 3].
    Expected: SAND_OK.
    1.2. Call fx950_api_serdes_get_swap_select with not NULL pointers and port, lane from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with pcs_tx_lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with pcs_rx_lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   lane = 0;
    SAND_U32   pcs_tx_lane = 0;
    SAND_U32   pcs_rx_lane = 0;
    SAND_U32   pcs_tx_laneGet = 0;
    SAND_U32   pcs_rx_laneGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1],
                           lane [0 / 3],
                           pcs_tx_lane [0 / 3],
                           pcs_rx_lane [0 / 3].
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        lane = 0;
        pcs_tx_lane = 0;
        pcs_rx_lane = 0;

        result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        /*
            1.2. Call fx950_api_serdes_get_swap_select with not NULL pointers and port, lane from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_laneGet, &pcs_rx_laneGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_swap_select: %d, %d, %d",
                                     device_id, port, lane);

        UTF_VERIFY_EQUAL1_STRING_MAC(pcs_tx_lane, pcs_tx_laneGet,
                                     "got another pcs_tx_lane than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(pcs_rx_lane, pcs_rx_laneGet,
                                     "got another pcs_rx_lane than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        lane = 1;
        pcs_tx_lane = 1;
        pcs_rx_lane = 1;

        result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        /*
            1.2. Call fx950_api_serdes_get_swap_select with not NULL pointers and port, lane from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_laneGet, &pcs_rx_laneGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_swap_select: %d, %d, %d",
                                     device_id, port, lane);

        UTF_VERIFY_EQUAL1_STRING_MAC(pcs_tx_lane, pcs_tx_laneGet,
                                     "got another pcs_tx_lane than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(pcs_rx_lane, pcs_rx_laneGet,
                                     "got another pcs_rx_lane than was set: %d", device_id);

        /*
            1.3. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        lane = 6;

        result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, lane = %d", device_id, lane);

        lane = 0;

        /*
            1.5. Call with pcs_tx_lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        pcs_tx_lane = 6;

        result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, pcs_tx_lane = %d", device_id, pcs_tx_lane);

        pcs_tx_lane = 0;

        /*
            1.6. Call with pcs_rx_lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        pcs_rx_lane = 6;

        result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, pcs_rx_lane = %d", device_id, pcs_rx_lane);
    }

    port = 0;
    lane = 0;
    pcs_tx_lane = 0;
    pcs_rx_lane = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_get_swap_select(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_IN     unsigned long   lane,
    SAND_OUT    unsigned long   *pcs_tx_lane_ptr,
    SAND_OUT    unsigned long   *pcs_rx_lane_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_get_swap_select)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1],
                   lane [0 / 3],
                   not NULL pcs_tx_lane_ptr, pcs_rx_lane_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with pcs_tx_lane_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with pcs_rx_lane_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   lane = 0;
    SAND_U32   pcs_tx_lane = 0;
    SAND_U32   pcs_rx_lane = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1],
                           lane [0 / 3],
                           not NULL pcs_tx_lane_ptr, pcs_rx_lane_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        lane = 0;

        result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_lane, &pcs_rx_lane);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /* iterate with port = 1 */
        port = 1;
        lane = 1;

        result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_lane, &pcs_rx_lane);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /*
            1.2. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_lane, &pcs_rx_lane);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        lane = 6;

        result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_lane, &pcs_rx_lane);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, lane = %d", device_id, lane);

        lane = 0;

        /*
            1.4. Call with pcs_tx_lane_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_swap_select(device_id, port, lane, NULL, &pcs_rx_lane);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, pcs_tx_lane = NULL", device_id);

        /*
            1.5. Call with pcs_rx_lane_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_lane, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, pcs_rx_lane = NULL", device_id);
    }

    port = 0;
    lane = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_lane, &pcs_rx_lane);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_lane, &pcs_rx_lane);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_set_reset_enable_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_IN     unsigned long   reset_enable_bmp
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_set_reset_enable_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1 / 2], reset_enable_bmp [0 / 0xF / 0x3F].
    Expected: SAND_OK.
    1.2. Call fx950_api_serdes_get_reset_enable_bmp with not NULL reset_bmp_ptr and port from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with port [3] (out of range) and other para ms from 1.1.
    ExpecteD: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   reset_enable_bmp = 0;
    SAND_U32   reset_enable_bmpGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1 / 2], reset_enable_bmp [0 / 0xF / 0x3F].
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        reset_enable_bmp = 0;

        result = fx950_api_serdes_set_reset_enable_bmp(device_id, port, reset_enable_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, reset_enable_bmp);

        /*
            1.2. Call fx950_api_serdes_get_reset_enable_bmp with not NULL reset_bmp_ptr and port from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmpGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_reset_enable_bmp: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(reset_enable_bmp, reset_enable_bmpGet, "got another reset_enable_bmpthan was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        reset_enable_bmp = 0xF;

        result = fx950_api_serdes_set_reset_enable_bmp(device_id, port, reset_enable_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, reset_enable_bmp);

        /*
            1.2. Call fx950_api_serdes_get_reset_enable_bmp with not NULL reset_bmp_ptr and port from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmpGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_reset_enable_bmp: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(reset_enable_bmp, reset_enable_bmpGet, "got another reset_enable_bmpthan was set: %d", device_id);

        /* iterate with port = 2 */
        port = 2;
        reset_enable_bmp = 0x3F;

        result = fx950_api_serdes_set_reset_enable_bmp(device_id, port, reset_enable_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, reset_enable_bmp);

        /*
            1.2. Call fx950_api_serdes_get_reset_enable_bmp with not NULL reset_bmp_ptr and port from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmpGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_reset_enable_bmp: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(reset_enable_bmp, reset_enable_bmpGet, "got another reset_enable_bmpthan was set: %d", device_id);

        /*
            1.3. Call with port [3] (out of range) and other para ms from 1.1.
            ExpecteD: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_serdes_set_reset_enable_bmp(device_id, port, reset_enable_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);
    }

    port = 0;
    reset_enable_bmp = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_set_reset_enable_bmp(device_id, port, reset_enable_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_set_reset_enable_bmp(device_id, port, reset_enable_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_get_reset_enable_bmp(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_OUT    unsigned long   *reset_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_get_reset_enable_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 2], non-null reset_bmp_ptr.
    Expected: SAND_OK.
    1.2. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with reset_bmp_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   reset_enable_bmp = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], non-null reset_bmp_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [3] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with reset_bmp_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, reset_enable_bmp = NULL", device_id);
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
        result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_set_loopback_enable(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_IN     unsigned long   lane,
    SAND_IN     unsigned int    individual_loopback_enable,
    SAND_IN     unsigned  int   individual_line_loopback_enable
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_set_loopback_enable)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1],
                   lane [0 / 3],
                   individual_loopback_enable [0 / 1],
                   individual_line_loopback_enable [0 / 1].
    Expected: SAND_OK.
    1.2. Call fx950_api_serdes_get_loopback_enable with not NULL pointers and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with individual_loopback_enable [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with individual_line_loopback_enable [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   lane = 0;
    SAND_UINT  individual_loopback_enable = 0;
    SAND_UINT  individual_line_loopback_enable = 0;
    SAND_UINT  individual_loopback_enableGet = 0;
    SAND_UINT  individual_line_loopback_enableGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1],
                           lane [0 / 3],
                           individual_loopback_enable [0 / 1],
                           individual_line_loopback_enable [0 / 1].
            Expected: SAND_OK.
        */
        /* iterat ewith port = 0 */
        port = 0;
        lane = 0;
        individual_loopback_enable = 0;
        individual_line_loopback_enable = 0;

        result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);

        /*
            1.2. Call fx950_api_serdes_get_loopback_enable with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enableGet, &individual_line_loopback_enableGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_loopback_enable: %d, %d, %d", device_id, port, lane);

        UTF_VERIFY_EQUAL1_STRING_MAC(individual_loopback_enable, individual_loopback_enableGet,
                                     "got another individual_loopback_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(individual_line_loopback_enable, individual_line_loopback_enableGet,
                                     "got another individual_line_loopback_enable was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        lane = 0;
        individual_loopback_enable = 1;
        individual_line_loopback_enable = 1;

        result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);

        /*
            1.2. Call fx950_api_serdes_get_loopback_enable with not NULL pointers and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enableGet, &individual_line_loopback_enableGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_loopback_enable: %d, %d, %d", device_id, port, lane);

        UTF_VERIFY_EQUAL1_STRING_MAC(individual_loopback_enable, individual_loopback_enableGet,
                                     "got another individual_loopback_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(individual_line_loopback_enable, individual_line_loopback_enableGet,
                                     "got another individual_line_loopback_enable was set: %d", device_id);

        /*
            1.3. Call with port [3] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        lane = 6;

        result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, lane = %d", device_id, lane);

        lane = 0;

        /*
            1.5. Call with individual_loopback_enable [2] (out of range) and other params from 1.1.
            Expected: SAND_OK - fx950 does not checks "BOOL" "enable" parameters.
        */
        individual_loopback_enable = 6;

        result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, individual_loopback_enable = %d", device_id, individual_loopback_enable);

        individual_loopback_enable = 0;

        /*
            1.6. Call with individual_line_loopback_enable [2] (out of range) and other params from 1.1.
            Expected: SAND_OK - fx950 does not checks "BOOL" "enable" parameters.
        */
        individual_line_loopback_enable = 6;

        result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, individual_line_loopback_enable = %d", device_id, individual_line_loopback_enable);
    }

    port = 0;
    lane = 0;
    individual_loopback_enable = 0;
    individual_line_loopback_enable = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, individual_loopback_enable, individual_line_loopback_enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_get_loopback_enable(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_IN     unsigned long   lane,
    SAND_OUT    unsigned int    *individual_loopback_enable_ptr,
    SAND_OUT    unsigned int    *individual_line_loopback_enable_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_get_loopback_enable)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], lane [0 / 3] and non-null pointers.
    Expected: SAND_OK.
    1.2. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with individual_loopback_enable_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with individual_line_loopback_enable_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   lane = 0;
    SAND_UINT  individual_loopback_enable = 0;
    SAND_UINT  individual_line_loopback_enable = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], lane [0 / 3] and non-null pointers.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        lane = 0;

        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enable, &individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /* iterate with port = 1 */
        port = 1;
        lane = 1;

        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enable, &individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /*
            1.2. Call with port [3] (out of range) and other params fromc 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enable, &individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        lane = 6;

        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enable, &individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, lane = %d", device_id, lane);

        lane = 0;

        /*
            1.4. Call with individual_loopback_enable_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, NULL, &individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, individual_loopback_enable = NULL", device_id);

        /*
            1.5. Call with individual_line_loopback_enable_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enable, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, individual_line_loopback_enable = NULL", device_id);
    }

    port = 0;
    lane = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enable, &individual_line_loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, &individual_loopback_enable, &individual_line_loopback_enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_set_hgs_port_speed_mode(
    SAND_IN     unsigned int                    device_id,
    SAND_IN     unsigned long                   port,
    SAND_IN     FX950_SERDES_HGS_SPEED_MODE     speed_mode
  )
*/
#if 0 /* existing appDemo does not supports HGS ports */
UTF_TEST_CASE_MAC(fx950_api_serdes_set_hgs_port_speed_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], speed_mode [FX950_SERDES_HGS_SPEED_10 / FX950_SERDES_HGS_SPEED_12]
    Expected: SAND_OK.
    1.2. Call fx950_api_serdes_get_hgs_port_speed_mode with not NULL pointers and port from 1.1.
    ExpecteD: SAND_OK and the same param as was set.
    1.3. Call with  port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with speed_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;

    FX950_SERDES_HGS_SPEED_MODE speed_mode    = FX950_SERDES_HGS_SPEED_10;
    FX950_SERDES_HGS_SPEED_MODE speed_modeGet = FX950_SERDES_HGS_SPEED_10;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], speed_mode [FX950_SERDES_HGS_SPEED_10 / FX950_SERDES_HGS_SPEED_12]
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        speed_mode = FX950_SERDES_HGS_SPEED_10;

        result = fx950_api_serdes_set_hgs_port_speed_mode(device_id, port, speed_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, speed_mode);

        /*
            1.2. Call fx950_api_serdes_get_hgs_port_speed_mode with not NULL pointers and port from 1.1.
            ExpecteD: SAND_OK and the same param as was set.
        */
        result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, &speed_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_hgs_port_speed_mode: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(speed_mode, speed_modeGet, "got another speed_mode than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        speed_mode = FX950_SERDES_HGS_SPEED_12;

        result = fx950_api_serdes_set_hgs_port_speed_mode(device_id, port, speed_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, speed_mode);

        /*
            1.2. Call fx950_api_serdes_get_hgs_port_speed_mode with not NULL pointers and port from 1.1.
            ExpecteD: SAND_OK and the same param as was set.
        */
        result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, &speed_modeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_hgs_port_speed_mode: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(speed_mode, speed_modeGet, "got another speed_mode than was set: %d", device_id);

        /*
            1.3. Call with  port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_serdes_set_hgs_port_speed_mode(device_id, port, speed_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with speed_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        speed_mode = SAND_UTF_API_SERDES_INVALID_ENUM;

        result = fx950_api_serdes_set_hgs_port_speed_mode(device_id, port, speed_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d speed_mode = %d", device_id, speed_mode);
    }

    port = 0;
    speed_mode = FX950_SERDES_HGS_SPEED_10;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_set_hgs_port_speed_mode(device_id, port, speed_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_set_hgs_port_speed_mode(device_id, port, speed_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_get_hgs_port_speed_mode(
    SAND_IN     unsigned int                    device_id,
    SAND_IN     unsigned long                   port,
    SAND_OUT    FX950_SERDES_HGS_SPEED_MODE     *speed_mode_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_get_hgs_port_speed_mode)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], non-null speed_mode_ptr.
    Expected: SAND_OK.
    1.2. Call with  port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with speed_mode_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    FX950_SERDES_HGS_SPEED_MODE speed_mode = FX950_SERDES_HGS_SPEED_10;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], non-null speed_mode_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, &speed_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, &speed_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with  port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, &speed_mode);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with speed_mode_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, speed_mode = NULL", device_id);
    }

    port =  0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, &speed_mode);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, &speed_mode);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_set_lane_tx_params(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_IN     unsigned long       lane,
    SAND_IN     unsigned long       out_current,
    SAND_IN     unsigned long       pre_emphasis_current_control
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_set_lane_tx_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], lane [0 / 3], out_current [0 / 7], pre_emphasis_current_control [0 / 15].
    Expected: SAND_OK.
    1.2. Call fx950_api_serdes_get_lane_tx_params with not NULL pointers and port, lane from 1.1.
    ExpecteD: SAND_OK and tha same params as was set.
    1.3. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with out_current [8] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with pre_emphasis_current_control [16] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   lane = 0;
    SAND_U32   out_current = 0;
    SAND_U32   pre_emphasis_current_control = 0;
    SAND_U32   out_currentGet = 0;
    SAND_U32   pre_emphasis_current_controlGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], lane [0 / 4], out_current [0 / 7], pre_emphasis_current_control [0 / 15].
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        lane = 0;
        out_current = 0;
        pre_emphasis_current_control = 0;

        result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, port, lane, out_current, pre_emphasis_current_control);

        /*
            1.2. Call fx950_api_serdes_get_lane_tx_params with not NULL pointers and port, lane from 1.1.
            ExpecteD: SAND_OK and tha same params as was set.
        */
        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_currentGet, &pre_emphasis_current_controlGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_lane_tx_params: %d, %d, %d", device_id, port, lane);

        UTF_VERIFY_EQUAL1_STRING_MAC(out_current, out_currentGet,
                                    "got another out_current than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(pre_emphasis_current_control, pre_emphasis_current_controlGet,
                                    "got another pre_emphasis_current_control than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        lane = 1;
        out_current = 7;
        pre_emphasis_current_control = 15;

        result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL5_PARAM_MAC(SAND_OK, rc, device_id, port, lane, out_current, pre_emphasis_current_control);

        /*
            1.2. Call fx950_api_serdes_get_lane_tx_params with not NULL pointers and port, lane from 1.1.
            ExpecteD: SAND_OK and tha same params as was set.
        */
        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_currentGet, &pre_emphasis_current_controlGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_serdes_get_lane_tx_params: %d, %d, %d", device_id, port, lane);

        UTF_VERIFY_EQUAL1_STRING_MAC(out_current, out_currentGet,
                                    "got another out_current than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(pre_emphasis_current_control, pre_emphasis_current_controlGet,
                                    "got another pre_emphasis_current_control than was set: %d", device_id);

        /*
            1.3. Call with port [3] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        lane = 6;

        result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, lane = %d", device_id, lane);

        lane = 0;

        /*
            1.5. Call with out_current [8] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        out_current = 8;

        result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, out_current = %d", device_id, out_current);

        out_current = 0;

        /*
            1.6. Call with pre_emphasis_current_control [16] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        pre_emphasis_current_control = 16;

        result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, pre_emphasis_current_control = %d", device_id, pre_emphasis_current_control);
    }

    port = 0;
    lane = 0;
    out_current = 0;
    pre_emphasis_current_control = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, pre_emphasis_current_control);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_get_lane_tx_params(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_IN     unsigned long       lane,
    SAND_OUT    unsigned long       *out_current_ptr,
    SAND_OUT    unsigned long       *pre_emphasis_current_control_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_get_lane_tx_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], lane [0 / 3], non-null out_current_ptr and pre_emphasis_current_control_ptr.
    Expected: SAND_OK.
    1.2. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with out_current_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with pre_emphasis_current_control_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   lane = 0;
    SAND_U32   out_current = 0;
    SAND_U32   pre_emphasis_current_control = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], lane [0 / 3], non-null out_current_ptr and pre_emphasis_current_control_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        lane = 0;

        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_current, &pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /* iterate with port = 1 */
        port = 1;
        lane = 1;

        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_current, &pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /*
            1.2. Call with port [3] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_current, &pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        lane = 6;

        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_current, &pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, lane = %d", device_id, lane);

        lane = 0;

        /*
            1.4. Call with out_current_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, NULL, &pre_emphasis_current_control);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, out_current_ptr = NULL", device_id);

        /*
            1.5. Call with pre_emphasis_current_control_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_current, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, pre_emphasis_current_control_ptr = NULL", device_id);
    }

    port = 0;
    lane = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_current, &pre_emphasis_current_control);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_current, &pre_emphasis_current_control);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_serdes_get_lane_signal_status(
    SAND_IN     unsigned int                        device_id,
    SAND_IN     unsigned long                       port,
    SAND_IN     unsigned long                       lane,
    SAND_OUT    FX950_SERDES_SIGNAL_STATUS_STRUCT   *signal_status_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_serdes_get_lane_signal_status)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], lane [0 / 3], non-null signal_status_ptr.
    Expected: SAND_OK.
    1.2. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with signal_status_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U32   lane = 0;
    FX950_SERDES_SIGNAL_STATUS_STRUCT signal_status;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], lane [0 / 3], non-null signal_status_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        lane = 0;

        result = fx950_api_serdes_get_lane_signal_status(device_id, port, lane, &signal_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /* iterate with port = 1 */
        port = 1;
        lane = 1;

        result = fx950_api_serdes_get_lane_signal_status(device_id, port, lane, &signal_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /*
            1.2. Call with port [3] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_serdes_get_lane_signal_status(device_id, port, lane, &signal_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        lane = 6;

        result = fx950_api_serdes_get_lane_signal_status(device_id, port, lane, &signal_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, lane = %d", device_id, lane);

        lane = 0;

        /*
            1.4. Call with signal_status_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_serdes_get_lane_signal_status(device_id, port, lane, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, signal_status = NULL", device_id);
    }

    port = 0;
    lane = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_serdes_get_lane_signal_status(device_id, port, lane, &signal_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_serdes_get_lane_signal_status(device_id, port, lane, &signal_status);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_serdes suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_serdes)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_set_swap_select)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_get_swap_select)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_set_reset_enable_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_get_reset_enable_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_set_loopback_enable)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_get_loopback_enable)
#if 0 /* existing appDemo does not supports HGS ports */
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_set_hgs_port_speed_mode)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_get_hgs_port_speed_mode)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_set_lane_tx_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_get_lane_tx_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_serdes_get_lane_signal_status)
UTF_SUIT_END_TESTS_MAC(fx950_api_serdes)



