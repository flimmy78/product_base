/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_hgs_interfaceUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_hgs_interface, that provides
*       Hyper G.Stack Network interface and MACs (Media Access 
*       Controllers) API of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <FX950/include/fx950_api_hgs_interface.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_HGS_INTERFACE_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgs_interface_set_port_parameters(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     unsigned long                           port,
    SAND_IN     FX950_HGS_INTERFACE_PORT_PARAM_STRUCT   *port_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgs_interface_set_port_parameters)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], port_params {  port_enable [TRUE / FALSE],
                                                force_link_pass_enable [TRUE / FALSE],
                                                force_link_down_enable [TRUE / FALSE],
                                                rx_crc_check_enable [TRUE / FALSE],
                                                padding_disable [TRUE / FALSE],
                                                preamble_length_rx_mode [FX950_HGS_INTERFACE_PREAMBLE_STANDART / 
                                                                         FX950_HGS_INTERFACE_PREAMBLE_SHORT],
                                                preamble_length_tx_mode [FX950_HGS_INTERFACE_PREAMBLE_STANDART / 
                                                                         FX950_HGS_INTERFACE_PREAMBLE_SHORT],
                                                mru_size [0 / 10300],
                                                tx_ipg_mode [FX950_HGS_INTERFACE_PORT_IPG_LAN_PHY / 
                                                             FX950_HGS_INTERFACE_PORT_IPG_WAN_PHY],
                                                fixed_ipg_base_type [FX950_HGS_INTERFACE_PORT_BASE_IPG_12B / 
                                                                     FX950_HGS_INTERFACE_PORT_BASE_IPG_8B],
                                                rx_crc_mode [FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE / 
                                                             FX950_HGS_INTERFACE_PORT_CRC_FOUR_BYTES],
                                                tx_crc_mode [FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE / 
                                                             FX950_HGS_INTERFACE_PORT_CRC_FOUR_BYTES] }
    Expected: SAND_OK.
    1.2. Call fx950_api_hgs_interface_get_port_parameters with not NULL port_params_ptr and other params from 1.1.
    Expected: SAND_OK and the same port_params as was set.
    1.3. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with port_params_ptr->preamble_length_rx_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with port_params_ptr->preamble_length_tx_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with port_params_ptr->mru_size [10301] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with port_params_ptr->tx_ipg_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with port_params_ptr->fixed_ipg_base_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.9. Call with port_params_ptr->rx_crc_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.10. Call with port_params_ptr->tx_crc_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.11. Call with port_params_ptr [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;

    FX950_HGS_INTERFACE_PORT_PARAM_STRUCT port_params;
    FX950_HGS_INTERFACE_PORT_PARAM_STRUCT port_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], port_params {  port_enable [TRUE / FALSE],
                                                        force_link_pass_enable [TRUE / FALSE],
                                                        force_link_down_enable [TRUE / FALSE],
                                                        rx_crc_check_enable [TRUE / FALSE],
                                                        padding_disable [TRUE / FALSE],
                                                        preamble_length_rx_mode [FX950_HGS_INTERFACE_PREAMBLE_STANDART / 
                                                                                 FX950_HGS_INTERFACE_PREAMBLE_SHORT],
                                                        preamble_length_tx_mode [FX950_HGS_INTERFACE_PREAMBLE_STANDART / 
                                                                                 FX950_HGS_INTERFACE_PREAMBLE_SHORT],
                                                        mru_size [0 / 10300],
                                                        tx_ipg_mode [FX950_HGS_INTERFACE_PORT_IPG_LAN_PHY / 
                                                                     FX950_HGS_INTERFACE_PORT_IPG_WAN_PHY],
                                                        fixed_ipg_base_type [FX950_HGS_INTERFACE_PORT_BASE_IPG_12B / 
                                                                             FX950_HGS_INTERFACE_PORT_BASE_IPG_8B],
                                                        rx_crc_mode [FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE / 
                                                                     FX950_HGS_INTERFACE_PORT_CRC_FOUR_BYTES],
                                                        tx_crc_mode [FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE / 
                                                                     FX950_HGS_INTERFACE_PORT_CRC_FOUR_BYTES] }
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        port_params.port_enable = TRUE;
        port_params.force_link_pass_enable = TRUE;
        port_params.force_link_down_enable = TRUE;
        port_params.rx_crc_check_enable = TRUE;
        port_params.padding_disable = TRUE;
        port_params.preamble_length_rx_mode = FX950_HGS_INTERFACE_PREAMBLE_STANDART;
        port_params.preamble_length_tx_mode = FX950_HGS_INTERFACE_PREAMBLE_STANDART;
        port_params.mru_size = 0;
        port_params.tx_ipg_mode  = FX950_HGS_INTERFACE_PORT_IPG_LAN_PHY;
        port_params.fixed_ipg_base_type = FX950_HGS_INTERFACE_PORT_BASE_IPG_12B;
        port_params.rx_crc_mode = FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE;
        port_params.tx_crc_mode = FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_hgs_interface_get_port_parameters with not NULL port_params_ptr and other params from 1.1.
            Expected: SAND_OK and the same port_params as was set.
        */
        result = fx950_api_hgs_interface_get_port_parameters(device_id, port, &port_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgs_interface_get_port_parameters: %d, %d", device_id, port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.port_enable, port_paramsGet.port_enable, 
                                     "got another port_params.port_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.force_link_pass_enable, port_paramsGet.force_link_pass_enable, 
                                     "got another port_params.force_link_pass_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.force_link_down_enable, port_paramsGet.force_link_down_enable, 
                                     "got another port_params.force_link_down_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.rx_crc_check_enable, port_paramsGet.rx_crc_check_enable, 
                                     "got another port_params.rx_crc_check_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.padding_disable, port_paramsGet.padding_disable, 
                                     "got another port_params.padding_disable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.preamble_length_rx_mode, port_paramsGet.preamble_length_rx_mode, 
                                     "got another port_params.preamble_length_rx_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.preamble_length_tx_mode, port_paramsGet.preamble_length_tx_mode, 
                                     "got another port_params.preamble_length_tx_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.mru_size, port_paramsGet.mru_size, 
                                     "got another port_params.mru_size than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.tx_ipg_mode, port_paramsGet.tx_ipg_mode, 
                                     "got another port_params.tx_ipg_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.fixed_ipg_base_type, port_paramsGet.fixed_ipg_base_type, 
                                     "got another port_params.fixed_ipg_base_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.rx_crc_mode, port_paramsGet.rx_crc_mode, 
                                     "got another port_params.rx_crc_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.tx_crc_mode, port_paramsGet.tx_crc_mode, 
                                     "got another port_params.tx_crc_mode than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;

        port_params.port_enable = FALSE;
        port_params.force_link_pass_enable = FALSE;
        port_params.force_link_down_enable = FALSE;
        port_params.rx_crc_check_enable = FALSE;
        port_params.padding_disable = FALSE;
        port_params.preamble_length_rx_mode = FX950_HGS_INTERFACE_PREAMBLE_SHORT;
        port_params.preamble_length_tx_mode = FX950_HGS_INTERFACE_PREAMBLE_SHORT;
        port_params.mru_size = 10300;
        port_params.tx_ipg_mode  = FX950_HGS_INTERFACE_PORT_IPG_WAN_PHY;
        port_params.fixed_ipg_base_type = FX950_HGS_INTERFACE_PORT_BASE_IPG_8B;
        port_params.rx_crc_mode = FX950_HGS_INTERFACE_PORT_CRC_FOUR_BYTES;
        port_params.tx_crc_mode = FX950_HGS_INTERFACE_PORT_CRC_FOUR_BYTES;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_hgs_interface_get_port_parameters with not NULL port_params_ptr and other params from 1.1.
            Expected: SAND_OK and the same port_params as was set.
        */
        result = fx950_api_hgs_interface_get_port_parameters(device_id, port, &port_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgs_interface_get_port_parameters: %d, %d", device_id, port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.port_enable, port_paramsGet.port_enable, 
                                     "got another port_params.port_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.force_link_pass_enable, port_paramsGet.force_link_pass_enable, 
                                     "got another port_params.force_link_pass_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.force_link_down_enable, port_paramsGet.force_link_down_enable, 
                                     "got another port_params.force_link_down_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.rx_crc_check_enable, port_paramsGet.rx_crc_check_enable, 
                                     "got another port_params.rx_crc_check_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.padding_disable, port_paramsGet.padding_disable, 
                                     "got another port_params.padding_disable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.preamble_length_rx_mode, port_paramsGet.preamble_length_rx_mode, 
                                     "got another port_params.preamble_length_rx_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.preamble_length_tx_mode, port_paramsGet.preamble_length_tx_mode, 
                                     "got another port_params.preamble_length_tx_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.mru_size, port_paramsGet.mru_size, 
                                     "got another port_params.mru_size than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.tx_ipg_mode, port_paramsGet.tx_ipg_mode, 
                                     "got another port_params.tx_ipg_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.fixed_ipg_base_type, port_paramsGet.fixed_ipg_base_type, 
                                     "got another port_params.fixed_ipg_base_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.rx_crc_mode, port_paramsGet.rx_crc_mode, 
                                     "got another port_params.rx_crc_mode than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_params.tx_crc_mode, port_paramsGet.tx_crc_mode, 
                                     "got another port_params.tx_crc_mode than was set: %d", device_id);
        /*
            1.3. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with port_params_ptr->preamble_length_rx_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_params.preamble_length_rx_mode = SAND_UTF_HGS_INTERFACE_INVALID_ENUM;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port_params.preamble_length_rx_mode = %d",
                                        device_id, port_params.preamble_length_rx_mode);

        port_params.preamble_length_rx_mode = FX950_HGS_INTERFACE_PREAMBLE_STANDART;

        /*
            1.5. Call with port_params_ptr->preamble_length_tx_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_params.preamble_length_tx_mode = SAND_UTF_HGS_INTERFACE_INVALID_ENUM;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port_params.preamble_length_tx_mode = %d",
                                        device_id, port_params.preamble_length_tx_mode);

        port_params.preamble_length_tx_mode = FX950_HGS_INTERFACE_PREAMBLE_STANDART;

        /*
            1.6. Call with port_params_ptr->mru_size [10301] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_params.mru_size = 10301;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port_params.mru_size = %d",
                                        device_id, port_params.mru_size);

        port_params.mru_size = 0;

        /*
            1.7. Call with port_params_ptr->tx_ipg_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_params.tx_ipg_mode = SAND_UTF_HGS_INTERFACE_INVALID_ENUM;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port_params.tx_ipg_mode = %d",
                                        device_id, port_params.tx_ipg_mode);

        port_params.tx_ipg_mode = FX950_HGS_INTERFACE_PORT_IPG_LAN_PHY;

        /*
            1.8. Call with port_params_ptr->fixed_ipg_base_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_params.fixed_ipg_base_type = SAND_UTF_HGS_INTERFACE_INVALID_ENUM;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port_params.fixed_ipg_base_type = %d",
                                        device_id, port_params.fixed_ipg_base_type);

        port_params.fixed_ipg_base_type = FX950_HGS_INTERFACE_PORT_BASE_IPG_12B;

        /*
            1.9. Call with port_params_ptr->rx_crc_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_params.rx_crc_mode = SAND_UTF_HGS_INTERFACE_INVALID_ENUM;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port_params.rx_crc_mode = %d",
                                        device_id, port_params.rx_crc_mode);

        port_params.rx_crc_mode = FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE;

        /*
            1.10. Call with port_params_ptr->tx_crc_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_params.tx_crc_mode = SAND_UTF_HGS_INTERFACE_INVALID_ENUM;

        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port_params.tx_crc_mode = %d",
                                        device_id, port_params.tx_crc_mode);

        port_params.tx_crc_mode = FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE;

        /*
            1.11. Call with port_params_ptr [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_params_ptr = NULL", device_id);
    }

    port = 0;

    port_params.port_enable = TRUE;
    port_params.force_link_pass_enable = TRUE;
    port_params.force_link_down_enable = TRUE;
    port_params.rx_crc_check_enable = TRUE;
    port_params.padding_disable = TRUE;
    port_params.preamble_length_rx_mode = FX950_HGS_INTERFACE_PREAMBLE_STANDART;
    port_params.preamble_length_tx_mode = FX950_HGS_INTERFACE_PREAMBLE_STANDART;
    port_params.mru_size = 0;
    port_params.tx_ipg_mode  = FX950_HGS_INTERFACE_PORT_IPG_LAN_PHY;
    port_params.fixed_ipg_base_type = FX950_HGS_INTERFACE_PORT_BASE_IPG_12B;
    port_params.rx_crc_mode = FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE;
    port_params.tx_crc_mode = FX950_HGS_INTERFACE_PORT_CRC_ONE_BYTE;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgs_interface_get_port_parameters(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     unsigned long                           port,
    SAND_OUT    FX950_HGS_INTERFACE_PORT_PARAM_STRUCT   *port_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgs_interface_get_port_parameters)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1] non-null port_params. 
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range)
    Expected: NOT SAND_OK.
    1.3. Call with port_params [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    FX950_HGS_INTERFACE_PORT_PARAM_STRUCT port_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1] non-null port_params. 
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgs_interface_get_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgs_interface_get_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range)
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgs_interface_get_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with port_params [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgs_interface_get_port_parameters(device_id, port, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_params_ptr = NULL", device_id);
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
        result = fx950_api_hgs_interface_get_port_parameters(device_id, port, &port_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgs_interface_get_port_parameters(device_id, port, &port_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgs_interface_get_port_status(
    SAND_IN     unsigned int                            device_id,
    SAND_IN     unsigned long                           port,
    SAND_OUT    FX950_HGS_INTERFACE_PORT_STATUS_STRUCT  *port_status_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgs_interface_get_port_status)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1] non-null port_status_ptr. 
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range)
    Expected: NOT SAND_OK.
    1.3. Call with port_status_ptr [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;
    FX950_HGS_INTERFACE_PORT_STATUS_STRUCT port_status;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1] non-null port_status_ptr. 
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgs_interface_get_port_status(device_id, port, &port_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgs_interface_get_port_status(device_id, port, &port_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range)
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgs_interface_get_port_status(device_id, port, &port_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with port_status_ptr [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgs_interface_get_port_status(device_id, port, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_status = NULL", device_id);
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
        result = fx950_api_hgs_interface_get_port_status(device_id, port, &port_status);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgs_interface_get_port_status(device_id, port, &port_status);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgs_interface_set_port_fc_parameters(
    SAND_IN     unsigned int                                        device_id,
    SAND_IN     unsigned long                                       port,
    SAND_IN     FX950_HGS_INTERFACE_PORT_FLOW_CONTROL_PARAMS_STRUCT *port_fc_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgs_interface_set_port_fc_parameters)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], 
                   port_fc_params { periodic_xoff_enable [TRUE / FALSE],
                                    periodic_xon_enable [TRUE / FALSE],
                                    rx_fc_enable [TRUE / FALSE],
                                    tx_fc_enable [TRUE / FALSE] }
    Expected: SAND_OK.
    1.2. Call fx950_api_hgs_interface_get_port_fc_parameters with not NULL pointer and port from 1.1.
    Expected: SAND_OK and the same port_fc_params_ptr as was set.
    1.3. Call with port [2] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.2. Call with port_fc_params_ptr [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  port = 0;

    FX950_HGS_INTERFACE_PORT_FLOW_CONTROL_PARAMS_STRUCT port_fc_params;
    FX950_HGS_INTERFACE_PORT_FLOW_CONTROL_PARAMS_STRUCT port_fc_paramsGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], 
                           port_fc_params { periodic_xoff_enable [TRUE / FALSE],
                                            periodic_xon_enable [TRUE / FALSE],
                                            rx_fc_enable [TRUE / FALSE],
                                            tx_fc_enable [TRUE / FALSE] }
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        port_fc_params.periodic_xoff_enable = TRUE;
        port_fc_params.periodic_xon_enable = TRUE;
        port_fc_params.rx_fc_enable = TRUE;
        port_fc_params.tx_fc_enable = TRUE;
        
        result = fx950_api_hgs_interface_set_port_fc_parameters(device_id, port, &port_fc_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_hgs_interface_get_port_fc_parameters with not NULL pointer and port from 1.1.
            Expected: SAND_OK and the same port_fc_params_ptr as was set.
        */
        result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, &port_fc_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgs_interface_get_port_fc_parameters: %d, %d",
                                    device_id, port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(port_fc_params.periodic_xoff_enable, port_fc_paramsGet.periodic_xoff_enable, 
                                     "got another port_fc_params.periodic_xoff_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_fc_params.periodic_xon_enable, port_fc_paramsGet.periodic_xon_enable, 
                                     "got another port_fc_params.periodic_xon_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_fc_params.rx_fc_enable, port_fc_paramsGet.rx_fc_enable, 
                                     "got another port_fc_params.rx_fc_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_fc_params.tx_fc_enable, port_fc_paramsGet.tx_fc_enable, 
                                     "got another port_fc_params.tx_fc_enable than was set: %d", device_id);

        /* iterate with port = 0 */
        port = 1;

        port_fc_params.periodic_xoff_enable = FALSE;
        port_fc_params.periodic_xon_enable = FALSE;
        port_fc_params.rx_fc_enable = FALSE;
        port_fc_params.tx_fc_enable = FALSE;
        
        result = fx950_api_hgs_interface_set_port_fc_parameters(device_id, port, &port_fc_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_hgs_interface_get_port_fc_parameters with not NULL pointer and port from 1.1.
            Expected: SAND_OK and the same port_fc_params_ptr as was set.
        */
        result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, &port_fc_paramsGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgs_interface_get_port_fc_parameters: %d, %d",
                                    device_id, port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(port_fc_params.periodic_xoff_enable, port_fc_paramsGet.periodic_xoff_enable, 
                                     "got another port_fc_params.periodic_xoff_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_fc_params.periodic_xon_enable, port_fc_paramsGet.periodic_xon_enable, 
                                     "got another port_fc_params.periodic_xon_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_fc_params.rx_fc_enable, port_fc_paramsGet.rx_fc_enable, 
                                     "got another port_fc_params.rx_fc_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(port_fc_params.tx_fc_enable, port_fc_paramsGet.tx_fc_enable, 
                                     "got another port_fc_params.tx_fc_enable than was set: %d", device_id);

        /*
            1.3. Call with port [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgs_interface_set_port_fc_parameters(device_id, port, &port_fc_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.2. Call with port_fc_params_ptr [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgs_interface_set_port_fc_parameters(device_id, port, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_fc_params = NULL", device_id);
    }

    port = 0;

    port_fc_params.periodic_xoff_enable = TRUE;
    port_fc_params.periodic_xon_enable = TRUE;
    port_fc_params.rx_fc_enable = TRUE;
    port_fc_params.tx_fc_enable = TRUE;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgs_interface_set_port_fc_parameters(device_id, port, &port_fc_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgs_interface_set_port_fc_parameters(device_id, port, &port_fc_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgs_interface_get_port_fc_parameters(
    SAND_IN     unsigned int                                        device_id,
    SAND_IN     unsigned long                                       port,
    SAND_OUT    FX950_HGS_INTERFACE_PORT_FLOW_CONTROL_PARAMS_STRUCT *port_fc_params_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgs_interface_get_port_fc_parameters)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], not NULL port_fc_params_ptr;
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range)
    Expected: NOT SAND_OK.
    1.3. Call with port_fc_params_ptr [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    FX950_HGS_INTERFACE_PORT_FLOW_CONTROL_PARAMS_STRUCT port_fc_params;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], not NULL port_fc_params_ptr;
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, &port_fc_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, &port_fc_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range)
            Expected: NOT SAND_OK.
        */
        /* iterate with port = 0 */
        port = 2;

        result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, &port_fc_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with port_fc_params_ptr [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_fc_params = NULL", device_id);
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
        result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, &port_fc_params);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, &port_fc_params);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgs_interface_set_fc_packet_sa(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_IN     unsigned char   sa[6]
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgs_interface_set_fc_packet_sa)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], sa[6] {0,0,0,0,0,0 / 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,}
    Expected: SAND_OK.
    1.2. Call fx950_api_hgs_interface_get_fc_packet_sa with not NULL pointer and port from 1.1.
    Expected: SAND_OK and the same sa as was set;
    1.3. Call with port [2] (out of range) and other pointers from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with sa [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U8    sa[6];
    SAND_U8    saGet[6];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], sa[6] {0,0,0,0,0,0 / 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,}
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        sa[0] = 0;
        sa[1] = 0;
        sa[2] = 0;
        sa[3] = 0;
        sa[4] = 0;
        sa[5] = 0;

        result = fx950_api_hgs_interface_set_fc_packet_sa(device_id, port, sa);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_hgs_interface_get_fc_packet_sa with not NULL pointer and port from 1.1.
            Expected: SAND_OK and the same sa as was set;
        */
        result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, saGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgs_interface_get_fc_packet_sa: %d, %d", device_id, port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[0], saGet[0], "got another sa[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[1], saGet[1], "got another sa[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[2], saGet[2], "got another sa[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[3], saGet[3], "got another sa[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[4], saGet[4], "got another sa[4] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[5], saGet[5], "got another sa[5] than was set: %d", device_id);

        /* iterate with port = 1 */
        port = 1;

        sa[0] = 0xFF;
        sa[1] = 0xFF;
        sa[2] = 0xFF;
        sa[3] = 0xFF;
        sa[4] = 0xFF;
        sa[5] = 0xFF;

        result = fx950_api_hgs_interface_set_fc_packet_sa(device_id, port, sa);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call fx950_api_hgs_interface_get_fc_packet_sa with not NULL pointer and port from 1.1.
            Expected: SAND_OK and the same sa as was set;
        */
        result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, saGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgs_interface_get_fc_packet_sa: %d, %d", device_id, port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[0], saGet[0], "got another sa[0] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[1], saGet[1], "got another sa[1] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[2], saGet[2], "got another sa[2] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[3], saGet[3], "got another sa[3] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[4], saGet[4], "got another sa[4] than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(sa[5], saGet[5], "got another sa[5] than was set: %d", device_id);

        /*
            1.3. Call with port [2] (out of range) and other pointers from 1.1.
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgs_interface_set_fc_packet_sa(device_id, port, sa);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.4. Call with sa [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgs_interface_set_fc_packet_sa(device_id, port, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sa = NULL", device_id);
    }

    port = 0;

    sa[0] = 0; sa[1] = 0; sa[2] = 0; sa[3] = 0; sa[4] = 0; sa[5] = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgs_interface_set_fc_packet_sa(device_id, port, sa);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgs_interface_set_fc_packet_sa(device_id, port, sa);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgs_interface_get_fc_packet_sa(
    SAND_IN     unsigned int    device_id,
    SAND_IN     unsigned long   port,
    SAND_OUT    unsigned char   sa[6]
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgs_interface_get_fc_packet_sa)
{
/*
    ITERATE_DEVICES
    1.1. Call with port [0 / 1], not NULL sa;
    Expected: SAND_OK.
    1.2. Call with port [2] (out of range)
    Expected: NOT SAND_OK.
    1.3. Call with sa [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port = 0;
    SAND_U8    sa[6];


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with port [0 / 1], not NULL sa;
            Expected: SAND_OK.
        */
        /* iterate with port = 0 */
        port = 0;

        result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, sa);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /* iterate with port = 1 */
        port = 1;

        result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, sa);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        /*
            1.2. Call with port [2] (out of range)
            Expected: NOT SAND_OK.
        */
        port = 2;

        result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, sa);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port);

        port = 0;

        /*
            1.3. Call with sa [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sa = NULL", device_id);
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
        result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, sa);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, sa);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_hgs_interface suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_hgs_interface)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgs_interface_set_port_parameters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgs_interface_get_port_parameters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgs_interface_get_port_status)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgs_interface_set_port_fc_parameters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgs_interface_get_port_fc_parameters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgs_interface_set_fc_packet_sa)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgs_interface_get_fc_packet_sa)
UTF_SUIT_END_TESTS_MAC(fx950_api_hgs_interface)

