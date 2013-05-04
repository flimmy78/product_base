/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_hgl_interfaceUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_hgl_interface, that provides
*       HyperG.Link Interface APIs of 98FX950 
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_hgl_interface.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_HGL_INTERFACE_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_set_port_mac_params(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     unsigned long                               port,
    SAND_IN     FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT  *port_mac_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_set_port_mac_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], 
                   port_mac_params_ptr { serdes_lanes_mode [FX950_HGL_INTERFACE_SERDES_LANES_4 / 
                                                            FX950_HGL_INTERFACE_SERDES_LANES_6],
                                         ecc_checker_type [FX950_HGL_INTERFACE_ECC_CHECKER_SECDED / 
                                                           FX950_HGL_INTERFACE_ECC_CHECKER_CRC8] }.
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_interface_get_port_mac_params with port from 1.1 and not NULL port_mac_params_ptr.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with port_mac_params_ptr->serdes_lanes_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with port_mac_params_ptr->ecc_checker_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with port_mac_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;

    FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT port_mac_params;
    FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT port_mac_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], 
                           port_mac_params_ptr { serdes_lanes_mode [FX950_HGL_INTERFACE_SERDES_LANES_4 / 
                                                                    FX950_HGL_INTERFACE_SERDES_LANES_6],
                                                 ecc_checker_type [FX950_HGL_INTERFACE_ECC_CHECKER_SECDED / 
                                                                   FX950_HGL_INTERFACE_ECC_CHECKER_CRC8] }.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        port_mac_params.serdes_lanes_mode = FX950_HGL_INTERFACE_SERDES_LANES_4;
        port_mac_params.ecc_checker_type  = FX950_HGL_INTERFACE_ECC_CHECKER_SECDED;

        result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, &port_mac_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_hgl_interface_get_port_mac_params with port from 1.1 and not NULL port_mac_params_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, &port_mac_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_get_port_mac_params: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(port_mac_params.serdes_lanes_mode, port_mac_paramsGet.serdes_lanes_mode, 
                                     "got another port_mac_params.serdes_lanes_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_mac_params.ecc_checker_type, port_mac_paramsGet.ecc_checker_type, 
                                     "got another port_mac_params.ecc_checker_type than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;

        port_mac_params.serdes_lanes_mode = FX950_HGL_INTERFACE_SERDES_LANES_6;
        port_mac_params.ecc_checker_type  = FX950_HGL_INTERFACE_ECC_CHECKER_CRC8;

        result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, &port_mac_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_hgl_interface_get_port_mac_params with port from 1.1 and not NULL port_mac_params_ptr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, &port_mac_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_get_port_mac_params: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(port_mac_params.serdes_lanes_mode, port_mac_paramsGet.serdes_lanes_mode, 
                                     "got another port_mac_params.serdes_lanes_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_mac_params.ecc_checker_type, port_mac_paramsGet.ecc_checker_type, 
                                     "got another port_mac_params.ecc_checker_type than was set: %d", device_id);

        /*
            1.3. Call with port_mac_params_ptr->serdes_lanes_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_mac_params.serdes_lanes_mode = SAND_UTF_HGL_INTERFACE_INVALID_ENUM;

        result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, &port_mac_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, port_mac_params.serdes_lanes_mode = %d",
                                        device_id, port, port_mac_params.serdes_lanes_mode);

        port_mac_params.serdes_lanes_mode = FX950_HGL_INTERFACE_SERDES_LANES_4;

        /*
            1.4. Call with port_mac_params_ptr->ecc_checker_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_mac_params.ecc_checker_type = SAND_UTF_HGL_INTERFACE_INVALID_ENUM;

        result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, &port_mac_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, port_mac_params.ecc_checker_type = %d",
                                        device_id, port, port_mac_params.ecc_checker_type);

        port_mac_params.ecc_checker_type = FX950_HGL_INTERFACE_SERDES_LANES_4;

        /*
            1.5. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, &port_mac_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.6. Call with port_mac_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_mac_params = NULL", device_id);
    }

    port = 0;

    port_mac_params.serdes_lanes_mode = FX950_HGL_INTERFACE_SERDES_LANES_4;
    port_mac_params.ecc_checker_type  = FX950_HGL_INTERFACE_ECC_CHECKER_SECDED;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, &port_mac_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, &port_mac_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_get_port_mac_params(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     unsigned long                               port,
    SAND_OUT    FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT  *port_mac_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_get_port_mac_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], and not NULL port_mac_params_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with port_mac_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT port_mac_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], and not NULL port_mac_params_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, &port_mac_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, &port_mac_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, &port_mac_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with port_mac_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_mac_params = NULL", device_id);
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
        result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, &port_mac_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, &port_mac_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_send_ping(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_IN     unsigned long       ping_cell_data
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_send_ping)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], ping_cell_data [0 / 0xFFFFFF].
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_interface_get_tx_ping_cell_data with not NULL pointers and port from 1.1.
    Expected: SAND_OK and the same ping_cell_data_ptr as was set.
    1.3. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with ping_cell_data [0x1000000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_U32  ping_cell_data = 0;

    SAND_U32  ping_cell_dataGet = 0;
    SAND_UINT ping_sent_statusGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], ping_cell_data [0 / 0xFFFFFF].
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        ping_cell_data = 0;

        result = fx950_api_hgl_interface_send_ping(device_id, port, ping_cell_data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, ping_cell_data);

        /*
            1.2. Call fx950_api_hgl_interface_get_tx_ping_cell_data with not NULL pointers and port from 1.1.
            Expected: SAND_OK and the same ping_cell_data_ptr as was set.
        */
        result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                               &ping_cell_dataGet, 
                                                               &ping_sent_statusGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_get_tx_ping_cell_data: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(ping_cell_data, ping_cell_dataGet,
                                     "got another ping_cell_data than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        ping_cell_data = 0xFFFFFF;

        result = fx950_api_hgl_interface_send_ping(device_id, port, ping_cell_data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, ping_cell_data);

        /*
            1.2. Call fx950_api_hgl_interface_get_tx_ping_cell_data with not NULL pointers and port from 1.1.
            Expected: SAND_OK and the same ping_cell_data_ptr as was set.
        */
        result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                               &ping_cell_dataGet, 
                                                               &ping_sent_statusGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_get_tx_ping_cell_data: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(ping_cell_data, ping_cell_dataGet,
                                     "got another ping_cell_data than was set: %d", device_id);

        /*
            1.3. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_send_ping(device_id, port, ping_cell_data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with ping_cell_data [0x1000000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        ping_cell_data = 0x1000000;

        result = fx950_api_hgl_interface_send_ping(device_id, port, ping_cell_data);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, ping_cell_data = %d", 
                                         device_id, ping_cell_data);
    }

    port = 0;
    ping_cell_data = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_interface_send_ping(device_id, port, ping_cell_data);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_send_ping(device_id, port, ping_cell_data);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_get_tx_ping_cell_data(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_OUT    unsigned long       *ping_cell_data_ptr,
    SAND_OUT    unsigned int        *ping_sent_status_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_get_tx_ping_cell_data)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], not NULL ping_cell_data_ptr and ping_sent_status_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with ping_cell_data_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with ping_sent_status_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_U32  ping_cell_data = 0;
    SAND_UINT ping_sent_status = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], not NULL ping_cell_data_ptr and ping_sent_status_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &ping_sent_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &ping_sent_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &ping_sent_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with ping_cell_data_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                               NULL, 
                                                               &ping_sent_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ping_cell_data = NULL", device_id);

        /*
            1.4. Call with ping_sent_status_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ping_sent_status = NULL", device_id);
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
        result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &ping_sent_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                           &ping_cell_data, 
                                                           &ping_sent_status);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_get_rx_ping_cell_data(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_OUT    unsigned long       *ping_cell_data_ptr,
    SAND_OUT    unsigned int        *valid_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_get_rx_ping_cell_data)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], not NULL ping_cell_data_ptr and valid_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with ping_cell_data_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with valid_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_U32  ping_cell_data = 0;
    SAND_UINT valid = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], not NULL ping_cell_data_ptr and valid_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgl_interface_get_rx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &valid);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgl_interface_get_rx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &valid);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_get_rx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &valid);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with ping_cell_data_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_rx_ping_cell_data(device_id, port, 
                                                               NULL, 
                                                               &valid);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ping_cell_data = NULL", device_id);

        /*
            1.4. Call with valid_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_rx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, valid = NULL", device_id);
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
        result = fx950_api_hgl_interface_get_rx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &valid);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_get_rx_ping_cell_data(device_id, port, 
                                                               &ping_cell_data, 
                                                               &valid);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_set_fp_link_level_fc_status_enable (
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_IN     unsigned int        enable
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_set_fp_link_level_fc_status_enable)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], enable [TRUE / FALSE].
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_interface_set_fp_link_level_fc_status_enable 
              with port from 1.1. and not NULL enablePtr.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_UINT enable = 0;
    SAND_UINT enableGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], enable [TRUE / FALSE].
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port   = 0;
        enable = TRUE;

        result = fx950_api_hgl_interface_set_fp_link_level_fc_status_enable(device_id, port, enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, enable);

        /*
            1.2. Call fx950_api_hgl_interface_set_fp_link_level_fc_status_enable 
                      with port from 1.1. and not NULL enablePtr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, &enableGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_set_fp_link_level_fc_status_enable: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                                     "got another enable than was set: %d", device_id);

        /* iterate with port = 1 */
        port   = 1;
        enable = FALSE;

        result = fx950_api_hgl_interface_set_fp_link_level_fc_status_enable(device_id, port, enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, enable);

        /*
            1.2. Call fx950_api_hgl_interface_set_fp_link_level_fc_status_enable 
                      with port from 1.1. and not NULL enablePtr.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, &enableGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_set_fp_link_level_fc_status_enable: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                                     "got another enable than was set: %d", device_id);

        /*
            1.3. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_set_fp_link_level_fc_status_enable(device_id, port, enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);
    }

    port = 0;
    enable = TRUE;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_interface_set_fp_link_level_fc_status_enable(device_id, port, enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_set_fp_link_level_fc_status_enable(device_id, port, enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_get_fp_link_level_fc_status_enable (
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_OUT    unsigned int        *enable_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_get_fp_link_level_fc_status_enable)
{
/*
    ITERATE_DEVICES
    1.1.  Call with port [0 / 1] and not NULL enable_ptr
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with enable_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_UINT enable = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1.  Call with port [0 / 1] and not NULL enable_ptr
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, &enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, &enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, &enable);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with enable_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, enable = NULL", device_id);
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
        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, &enable);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, &enable);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_OUT    FX950_BMP_64        *fc_status_fp_bmp_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp)
{
/*
    ITERATE_DEVICES
    1.1. Call with  port [0 / 1], not NULL fc_status_fp_bmp_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with fc_status_fp_bmp_ptr [NULL]
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT    device_id = 0;
    SAND_U32     port = 0;
    FX950_BMP_64 fc_status_fp_bmp;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with  port [0 / 1], not NULL fc_status_fp_bmp_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp(device_id, port, &fc_status_fp_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp(device_id, port, &fc_status_fp_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp(device_id, port, &fc_status_fp_bmp);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with fc_status_fp_bmp_ptr [NULL]
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, fc_status_fp_bmp_ptr = NULL", device_id);
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
        result = fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp(device_id, port, &fc_status_fp_bmp);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp(device_id, port, &fc_status_fp_bmp);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_set_fc_tx_params(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     unsigned long                               port,
    SAND_IN     FX950_API_HGL_INTERFACE_FC_TX_PARAMS_STRUCT *fc_tx_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_set_fc_tx_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], 
                   fc_tx_params_ptr { enable [TRUE / FALSE],
                                      periodic_enable [TRUE / FALSE],
                                      refresh_rate [0 / 0xFFFFFF] }
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_interface_get_fc_tx_params with not NULL fc_tx_params_ptr and port from 1.1.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with fc_tx_params_ptr->refresh_rate [0x1000000] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with fc_tx_params_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;

    FX950_API_HGL_INTERFACE_FC_TX_PARAMS_STRUCT fc_tx_params;
    FX950_API_HGL_INTERFACE_FC_TX_PARAMS_STRUCT fc_tx_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], 
                           fc_tx_params_ptr { enable [TRUE / FALSE],
                                              periodic_enable [TRUE / FALSE],
                                              refresh_rate [0 / 0xFFFFFF] }
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        fc_tx_params.enable = TRUE;
        fc_tx_params.periodic_enable = TRUE;
        fc_tx_params.refresh_rate = 0;

        result = fx950_api_hgl_interface_set_fc_tx_params(device_id, port, &fc_tx_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_hgl_interface_get_fc_tx_params with not NULL fc_tx_params_ptr and port from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, &fc_tx_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_get_fc_tx_params: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(fc_tx_params.enable, fc_tx_paramsGet.enable, 
                                     "got another fc_tx_params.enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(fc_tx_params.periodic_enable, fc_tx_paramsGet.periodic_enable, 
                                     "got another fc_tx_params.periodic_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(fc_tx_params.refresh_rate, fc_tx_paramsGet.refresh_rate, 
                                     "got another fc_tx_params.refresh_rate than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;

        fc_tx_params.enable = FALSE;
        fc_tx_params.periodic_enable = FALSE;
        fc_tx_params.refresh_rate = 0xFFFFFF;

        result = fx950_api_hgl_interface_set_fc_tx_params(device_id, port, &fc_tx_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_hgl_interface_get_fc_tx_params with not NULL fc_tx_params_ptr and port from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, &fc_tx_paramsGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_get_fc_tx_params: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(fc_tx_params.enable, fc_tx_paramsGet.enable, 
                                     "got another fc_tx_params.enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(fc_tx_params.periodic_enable, fc_tx_paramsGet.periodic_enable, 
                                     "got another fc_tx_params.periodic_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(fc_tx_params.refresh_rate, fc_tx_paramsGet.refresh_rate, 
                                     "got another fc_tx_params.refresh_rate than was set: %d", device_id);

        /*
            1.3. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_set_fc_tx_params(device_id, port, &fc_tx_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with fc_tx_params_ptr->refresh_rate [0x1000000] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        fc_tx_params.refresh_rate = 0x1000000;

        result = fx950_api_hgl_interface_set_fc_tx_params(device_id, port, &fc_tx_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, fc_tx_params.refresh_rate = %d", 
                                    device_id, port, fc_tx_params.refresh_rate);

        fc_tx_params.refresh_rate = 0;

        /*
            1.5. Call with fc_tx_params_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_set_fc_tx_params(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, fc_tx_params = NULL", device_id);
    }

    port = 0;

    fc_tx_params.enable = TRUE;
    fc_tx_params.periodic_enable = TRUE;
    fc_tx_params.refresh_rate = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_interface_set_fc_tx_params(device_id, port, &fc_tx_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_set_fc_tx_params(device_id, port, &fc_tx_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_get_fc_tx_params(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     unsigned long                               port,
    SAND_OUT    FX950_API_HGL_INTERFACE_FC_TX_PARAMS_STRUCT *fc_tx_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_get_fc_tx_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1] and not NULL fc_tx_params_ptr.
    Expected: SAND_OK
    1.2. Call with port [2] (out of range) and other params from 1.1.
    Expected: SAND_OK.
    1.3. Call with fc_tx_params_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    FX950_API_HGL_INTERFACE_FC_TX_PARAMS_STRUCT fc_tx_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1] and not NULL fc_tx_params_ptr.
            Expected: SAND_OK
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, &fc_tx_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, &fc_tx_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range) and other params from 1.1.
            Expected: SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, &fc_tx_params);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with fc_tx_params_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, fc_tx_params = NULL", device_id);
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
        result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, &fc_tx_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, &fc_tx_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_set_fc_deadlock_limit(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_IN     unsigned long       deadlock_limit
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_set_fc_deadlock_limit)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], deadlock_limit [0 / 0xFFFF].
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_interface_get_fc_deadlock_limit with not NULL pointers and port from 1.1.
    Expected: SAND_OK and the same deadlock_limit as was set.
    1.3. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with deadlock_limit [0x10000] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_U32  deadlock_limit = 0;
    SAND_U32  deadlock_limitGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], deadlock_limit [0 / 0xFFFF].
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;
        deadlock_limit = 0;

        result = fx950_api_hgl_interface_set_fc_deadlock_limit(device_id, port, deadlock_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, deadlock_limit);

        /*
            1.2. Call fx950_api_hgl_interface_get_fc_deadlock_limit with not NULL pointers and port from 1.1.
            Expected: SAND_OK and the same deadlock_limit as was set.
        */
        result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, &deadlock_limitGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_get_fc_deadlock_limit: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(deadlock_limit, deadlock_limitGet, 
                                     "got another deadlock_limit than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;
        deadlock_limit = 0xFFFF;

        result = fx950_api_hgl_interface_set_fc_deadlock_limit(device_id, port, deadlock_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, port, deadlock_limit);

        /*
            1.2. Call fx950_api_hgl_interface_get_fc_deadlock_limit with not NULL pointers and port from 1.1.
            Expected: SAND_OK and the same deadlock_limit as was set.
        */
        result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, &deadlock_limitGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_interface_get_fc_deadlock_limit: %d, %d", device_id, port);

        UTF_VERIFY_EQUAL1_STRING_MAC(deadlock_limit, deadlock_limitGet, 
                                     "got another deadlock_limit than was set: %d", device_id);

        /*
            1.3. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_set_fc_deadlock_limit(device_id, port, deadlock_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with deadlock_limit [0x10000] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        deadlock_limit = 0x10000;

        result = fx950_api_hgl_interface_set_fc_deadlock_limit(device_id, port, deadlock_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, deadlock_limit = %d",
                                         device_id, deadlock_limit);
    }

    port = 0;
    deadlock_limit = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_interface_set_fc_deadlock_limit(device_id, port, deadlock_limit);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_set_fc_deadlock_limit(device_id, port, deadlock_limit);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_interface_get_fc_deadlock_limit(
    SAND_IN     unsigned int        device_id,
    SAND_IN     unsigned long       port,
    SAND_OUT    unsigned long       *deadlock_limit_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_interface_get_fc_deadlock_limit)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], not NULL deadlock_limit_ptr.
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with deadlock_limit_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    SAND_U32  deadlock_limit = 0;

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], not NULL deadlock_limit_ptr.
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, &deadlock_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, &deadlock_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, &deadlock_limit);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with deadlock_limit_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, deadlock_limit = NULL", device_id);
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
        result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, &deadlock_limit);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, &deadlock_limit);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_hgl_interface suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_hgl_interface)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_set_port_mac_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_get_port_mac_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_send_ping)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_get_tx_ping_cell_data)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_get_rx_ping_cell_data)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_set_fp_link_level_fc_status_enable)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_get_fp_link_level_fc_status_enable)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_set_fc_tx_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_get_fc_tx_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_set_fc_deadlock_limit)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_interface_get_fc_deadlock_limit)
UTF_SUIT_END_TESTS_MAC(fx950_api_hgl_interface)

