/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_pcsUT.
*
* DESCRIPTION:
*       Unit tests for fx950_api_pcs, that provides
*       PCS APIs for HyperG. Link, HyperG. Stack and Statistic interfaces
*       of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_pcs.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_pcs_set_loopback_enable(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_IN     unsigned int    loopback_enable
  )
*/
UTF_TEST_CASE_MAC(fx950_api_pcs_set_loopback_enable)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1 / 2], loopback_enable[FALSE / TRUE].
    Expected: SAND_OK.
    1.2. Call fx950_api_pcs_get_loopback_enable with not NULL pointer and port from 1.1.
    Expected: SAND_OK and the same loopback_enable as was set.
    1.3. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;

    SAND_UINT loopback_enable    = FALSE;
    SAND_UINT loopback_enableGet = FALSE;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1 / 2], loopback_enable[FALSE / TRUE].
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        loopback_enable = FALSE;

        result = fx950_api_pcs_set_loopback_enable(device_id, port, loopback_enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, loopback_enable);

        /*
            1.2. Call fx950_api_pcs_get_loopback_enable with not NULL pointer and port from 1.1.
            Expected: SAND_OK and the same loopback_enable as was set.
        */
        result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enableGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_pcs_get_loopback_enable: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(loopback_enable, loopback_enableGet,
                   "got another loopback_enable than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        loopback_enable = TRUE;

        result = fx950_api_pcs_set_loopback_enable(device_id, port, loopback_enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, loopback_enable);

        /*
            1.2. Call fx950_api_pcs_get_loopback_enable with not NULL pointer and port from 1.1.
            Expected: SAND_OK and the same loopback_enable as was set.
        */
        result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enableGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_pcs_get_loopback_enable: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(loopback_enable, loopback_enableGet,
                   "got another loopback_enable than was set: %d", device_id);

        /* iterate with port = 2 */
        port = 2;
        loopback_enable = TRUE;

        result = fx950_api_pcs_set_loopback_enable(device_id, port, loopback_enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, loopback_enable);

        /*
            1.2. Call fx950_api_pcs_get_loopback_enable with not NULL pointer and port from 1.1.
            Expected: SAND_OK and the same loopback_enable as was set.
        */
        result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enableGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_pcs_get_loopback_enable: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(loopback_enable, loopback_enableGet,
                   "got another loopback_enable than was set: %d", device_id);

        /*
            1.3. Call with port [3] (out of range) and other pointers from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_pcs_set_loopback_enable(device_id, port, loopback_enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);
    }

    port = 0;
    loopback_enable = FALSE;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_pcs_set_loopback_enable(device_id, port, loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_pcs_set_loopback_enable(device_id, port, loopback_enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_pcs_get_loopback_enable(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_OUT    unsigned int    *loopback_enable_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_pcs_get_loopback_enable)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1 / 2] and non NILL loopback_enable_ptr.
    Expected: SAND_OK.
    1.2. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with loopback_enable_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_UINT loopback_enable = FALSE;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1 / 2] and non NILL loopback_enable_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        
        result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 2 */
        port = 2;

        result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [3] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with loopback_enable_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_pcs_get_loopback_enable(device_id, port, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, loopback_enable_ptr = NULL", device_id);
    }

    port = 0;
    
    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_pcs_get_lane_status(
    SAND_IN     unsigned int                    device_id,
    SAND_IN     unsigned long                   port,
    SAND_IN     unsigned long                   lane,
    SAND_OUT    FX950_PCS_LANE_STATUS_STRUCT    *lane_status_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_pcs_get_lane_status)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1 / 2],
                   lane [6 / 4 / 2],
                   non NULL lane_status_ptr.
    Expected: SAND_OK.
    1.2. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with lane [6] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with port_params_ptr [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_U32  lane = 0;
    FX950_PCS_LANE_STATUS_STRUCT lane_status;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1 / 2],
                           lane [6 / 4 / 2],
                           non NULL lane_status_ptr.
            Expected: SAND_NOT_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        lane = 6;
        
        result = fx950_api_pcs_get_lane_status(device_id, port, lane, &lane_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /* iterate with port = 1 */
        port = 1;
        lane = 1;
        
        result = fx950_api_pcs_get_lane_status(device_id, port, lane, &lane_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /* iterate with port = 1 */
        port = 2;
        lane = 0;
        
        result = fx950_api_pcs_get_lane_status(device_id, port, lane, &lane_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, lane);

        /*
            1.2. Call with port [3] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_pcs_get_lane_status(device_id, port, lane, &lane_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with lane [6] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        lane = 6;

        result = fx950_api_pcs_get_lane_status(device_id, port, lane, &lane_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, lane = %d", device_id, lane);

        lane = 0;

        /*
            1.4. Call with port_params_ptr [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_pcs_get_lane_status(device_id, port, lane, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_params_ptr = NULL", device_id);
    }

    port = 0;
    lane = 0;
    
    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_pcs_get_lane_status(device_id, port, lane, &lane_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_pcs_get_lane_status(device_id, port, lane, &lane_status);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_pcs_get_status(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_OUT    unsigned int    *link_up_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_pcs_get_status)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1 / 2],
                   non NULL link_up_ptr.
    Expected: SAND_OK.
    1.2. Call with port [3] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with link_up_ptr [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port      = 0;
    SAND_UINT link_up   = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1 / 2],
                           non NULL link_up_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        
        result = fx950_api_pcs_get_status(device_id, port, &link_up);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;
        
        result = fx950_api_pcs_get_status(device_id, port, &link_up);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 2 */
        port = 2;
        
        result = fx950_api_pcs_get_status(device_id, port, &link_up);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [3] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 3;

        result = fx950_api_pcs_get_status(device_id, port, &link_up);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with link_up_ptr [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_pcs_get_status(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_up_ptr = NULL", device_id);
    }

    port = 0;
    
    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_pcs_get_status(device_id, port, &link_up);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_pcs_get_status(device_id, port, &link_up);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_pcs suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_pcs)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_pcs_set_loopback_enable)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_pcs_get_loopback_enable)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_pcs_get_lane_status)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_pcs_get_status)
UTF_SUIT_END_TESTS_MAC(fx950_api_pcs)

