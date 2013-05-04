/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_statistics_interfaceUT.
*
* DESCRIPTION:
*       Unit tests for fx950_api_statistics_interface, that provides
*       Statistics Interface APIs of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_statistics_interface.h>
#include <FX950/include/fx950_api_framework.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_interface_set_ingress_cfg(
    SAND_IN     unsigned int                                    device_id,
    SAND_IN     FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT   *ingr_stat_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_interface_set_ingress_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with ingr_stat_cfg_ptr {message_include_vid [FALSE / TRUE],
                                      statistics_enable [FALSE / TRUE],
                                      link_dev_num [0 / 7],
                                      eq_const_operation [FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT /
                                                          FX950_STATISTICS_INTERFACE_OPERATION_ADD],
                                      eq_const_bc [0 / 0xFFF],
                                      dq_const_operation [FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT /
                                                          FX950_STATISTICS_INTERFACE_OPERATION_ADD],
                                      dq_const_bc [0 / 0xFFF] }
    Expected: SAND_OK.
    1.2. Call fx950_api_statistics_interface_get_ingress_cfg with not NULL ingr_stat_cfg_ptr.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with ingr_stat_cfg_ptr->link_dev_num [SAND_MAX_DEVICE] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with ingr_stat_cfg_ptr->eq_const_operation [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with ingr_stat_cfg_ptr->eq_const_bc [0x1000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with ingr_stat_cfg_ptr->dq_const_operation [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with ingr_stat_cfg_ptr->dq_const_bc [0x1000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with ingr_stat_cfg_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT ingr_stat_cfg;
    FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT ingr_stat_cfgGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with ingr_stat_cfg_ptr {message_include_vid [FALSE / TRUE],
                                              statistics_enable [FALSE / TRUE],
                                              link_dev_num [0 / 7],
                                              eq_const_operation [FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT /
                                                                  FX950_STATISTICS_INTERFACE_OPERATION_ADD],
                                              eq_const_bc [0 / 0xFFF],
                                              dq_const_operation [FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT /
                                                                  FX950_STATISTICS_INTERFACE_OPERATION_ADD],
                                              dq_const_bc [0 / 0xFFF] }
            Expected: SAND_OK.
        */
        /* iterate with ingr_stat_cfg_ptr->message_include_vid = FALSE */
        ingr_stat_cfg.message_include_vid = FALSE;
        ingr_stat_cfg.statistics_enable = FALSE;
        ingr_stat_cfg.link_dev_num = 0;
        ingr_stat_cfg.eq_const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;
        ingr_stat_cfg.eq_const_bc = 0;
        ingr_stat_cfg.dq_const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;
        ingr_stat_cfg.dq_const_bc = 0;

        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_statistics_interface_get_ingress_cfg with not NULL ingr_stat_cfg_ptr.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_statistics_interface_get_ingress_cfg(device_id, &ingr_stat_cfgGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_statistics_interface_get_ingress_cfg: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.message_include_vid, ingr_stat_cfgGet.message_include_vid,
                                     "got another ingr_stat_cfg_ptr->message_include_vid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.statistics_enable, ingr_stat_cfgGet.statistics_enable,
                                     "got another ingr_stat_cfg_ptr->statistics_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.link_dev_num, ingr_stat_cfgGet.link_dev_num,
                                     "got another ingr_stat_cfg_ptr->link_dev_num than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.eq_const_operation, ingr_stat_cfgGet.eq_const_operation,
                                     "got another ingr_stat_cfg_ptr->eq_const_operation than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.eq_const_bc, ingr_stat_cfgGet.eq_const_bc,
                                     "got another ingr_stat_cfg_ptr->eq_const_bc than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.dq_const_operation, ingr_stat_cfgGet.dq_const_operation,
                                     "got another ingr_stat_cfg_ptr->dq_const_operation than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.dq_const_bc, ingr_stat_cfgGet.dq_const_bc,
                                     "got another ingr_stat_cfg_ptr->dq_const_bc than was set: %d", device_id);
        
        /* iterate with ingr_stat_cfg_ptr->message_include_vid = TRUE */
        ingr_stat_cfg.message_include_vid = TRUE;
        ingr_stat_cfg.statistics_enable = TRUE;
        ingr_stat_cfg.link_dev_num = 7;
        ingr_stat_cfg.eq_const_operation = FX950_STATISTICS_INTERFACE_OPERATION_ADD;
        ingr_stat_cfg.eq_const_bc = 0xFFF;
        ingr_stat_cfg.dq_const_operation = FX950_STATISTICS_INTERFACE_OPERATION_ADD;
        ingr_stat_cfg.dq_const_bc = 0xFFF;

        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_statistics_interface_get_ingress_cfg with not NULL ingr_stat_cfg_ptr.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_statistics_interface_get_ingress_cfg(device_id, &ingr_stat_cfgGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_statistics_interface_get_ingress_cfg: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.message_include_vid, ingr_stat_cfgGet.message_include_vid,
                                     "got another ingr_stat_cfg_ptr->message_include_vid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.statistics_enable, ingr_stat_cfgGet.statistics_enable,
                                     "got another ingr_stat_cfg_ptr->statistics_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.link_dev_num, ingr_stat_cfgGet.link_dev_num,
                                     "got another ingr_stat_cfg_ptr->link_dev_num than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.eq_const_operation, ingr_stat_cfgGet.eq_const_operation,
                                     "got another ingr_stat_cfg_ptr->eq_const_operation than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.eq_const_bc, ingr_stat_cfgGet.eq_const_bc,
                                     "got another ingr_stat_cfg_ptr->eq_const_bc than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.dq_const_operation, ingr_stat_cfgGet.dq_const_operation,
                                     "got another ingr_stat_cfg_ptr->dq_const_operation than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(ingr_stat_cfg.dq_const_bc, ingr_stat_cfgGet.dq_const_bc,
                                     "got another ingr_stat_cfg_ptr->dq_const_bc than was set: %d", device_id);

        /*
            1.3. Call with ingr_stat_cfg_ptr->link_dev_num [SAND_MAX_DEVICE] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        ingr_stat_cfg.link_dev_num = FX950_NOF_DESTINATION_DEVICES;

        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, ingr_stat_cfg_ptr->link_dev_num = %d",
                                         device_id, ingr_stat_cfg.link_dev_num);

        ingr_stat_cfg.link_dev_num = 0;

        /*
            1.4. Call with ingr_stat_cfg_ptr->eq_const_operation [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        ingr_stat_cfg.eq_const_operation = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, ingr_stat_cfg_ptr->eq_const_operation = %d",
                                         device_id, ingr_stat_cfg.eq_const_operation);

        ingr_stat_cfg.eq_const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;

        /*
            1.5. Call with ingr_stat_cfg_ptr->eq_const_bc [0x1000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        ingr_stat_cfg.eq_const_bc = 0x1000;

        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, ingr_stat_cfg_ptr->eq_const_bc = %d",
                                         device_id, ingr_stat_cfg.eq_const_bc);

        ingr_stat_cfg.eq_const_bc = 0;

        /*
            1.6. Call with ingr_stat_cfg_ptr->dq_const_operation [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        ingr_stat_cfg.dq_const_operation = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, ingr_stat_cfg_ptr->dq_const_operation = %d",
                                         device_id, ingr_stat_cfg.dq_const_operation);

        ingr_stat_cfg.dq_const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;

        /*
            1.7. Call with ingr_stat_cfg_ptr->dq_const_bc [0x1000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        ingr_stat_cfg.dq_const_bc = 0x1000;

        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, ingr_stat_cfg_ptr->dq_const_bc = %d",
                                         device_id, ingr_stat_cfg.dq_const_bc);

        ingr_stat_cfg.dq_const_bc = 0;

        /*
            1.8. Call with ingr_stat_cfg_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ingr_stat_cfg_ptr = NULL", device_id);
    }

    ingr_stat_cfg.message_include_vid = FALSE;
    ingr_stat_cfg.statistics_enable = FALSE;
    ingr_stat_cfg.link_dev_num = 0;
    ingr_stat_cfg.eq_const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;
    ingr_stat_cfg.eq_const_bc = 0;
    ingr_stat_cfg.dq_const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;
    ingr_stat_cfg.dq_const_bc = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_interface_get_ingress_cfg(
    SAND_IN     unsigned int                                    device_id,
    SAND_OUT    FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT   *ingr_stat_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_interface_get_ingress_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null ingr_stat_cfg_ptr.
    Expected: SAND_OK.
    1.2. Call with ingr_stat_cfg_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT ingr_stat_cfg;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null ingr_stat_cfg_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_statistics_interface_get_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with ingr_stat_cfg_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_interface_get_ingress_cfg(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ingr_stat_cfg_ptr = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_interface_get_ingress_cfg(device_id, &ingr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_interface_get_ingress_cfg(device_id, &ingr_stat_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_interface_set_mac_params(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     FX950_DIRECTION_TYPE                        direction,
    SAND_IN     FX950_STATISTICS_INTERFACE_MAC_PARAM_STRUCT *stat_mac_param_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_interface_set_mac_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS],
                   stat_mac_param_ptr {data_cycle_threshold [0 / 0xFFFF],
                                      idle_cycle_threshold [0 / 0xF]}
    Expected: SAND_OK.
    1.2. Call fx950_api_statistics_interface_get_mac_params with not NULL stat_mac_param_ptr.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with stat_mac_param_ptr->data_cycle_threshold [0x10000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with ingr_stat_cfg_ptr->idle_cycle_threshold [0x10] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with ingr_stat_cfg_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT            device_id = 0;
    FX950_DIRECTION_TYPE direction = FX950_DIRECTION_INGRESS;

    FX950_STATISTICS_INTERFACE_MAC_PARAM_STRUCT stat_mac_param;
    FX950_STATISTICS_INTERFACE_MAC_PARAM_STRUCT stat_mac_paramGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS],
                           stat_mac_param_ptr {data_cycle_threshold [0 / 0xFFFF],
                                              idle_cycle_threshold [0 / 0xF]}
            Expected: SAND_OK.
        */
        /* iterate with direction = FX950_DIRECTION_INGRESS */
        direction = FX950_DIRECTION_INGRESS;

        stat_mac_param.data_cycle_threshold = 0;
        stat_mac_param.idle_cycle_threshold = 0;

        result = fx950_api_statistics_interface_set_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /*
            1.2. Call fx950_api_statistics_interface_get_mac_params with not NULL stat_mac_param_ptr.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_statistics_interface_get_mac_params(device_id, direction, &stat_mac_paramGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_statistics_interface_get_mac_params: %d, %d", device_id, direction);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(stat_mac_param.data_cycle_threshold, stat_mac_paramGet.data_cycle_threshold,
                                     "got another stat_mac_param_ptr->data_cycle_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(stat_mac_param.idle_cycle_threshold, stat_mac_paramGet.idle_cycle_threshold,
                                     "got another stat_mac_param_ptr->idle_cycle_threshold than was set: %d", device_id);

        /* iterate with direction = FX950_DIRECTION_EGRESS */
        direction = FX950_DIRECTION_EGRESS;

        stat_mac_param.data_cycle_threshold = 0xFFFF;
        stat_mac_param.idle_cycle_threshold = 0xF;

        result = fx950_api_statistics_interface_set_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /*
            1.2. Call fx950_api_statistics_interface_get_mac_params with not NULL stat_mac_param_ptr.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_statistics_interface_get_mac_params(device_id, direction, &stat_mac_paramGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_statistics_interface_get_mac_params: %d, %d", device_id, direction);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(stat_mac_param.data_cycle_threshold, stat_mac_paramGet.data_cycle_threshold,
                                     "got another stat_mac_param_ptr->data_cycle_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(stat_mac_param.idle_cycle_threshold, stat_mac_paramGet.idle_cycle_threshold,
                                     "got another stat_mac_param_ptr->idle_cycle_threshold than was set: %d", device_id);

        /*
            1.3. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        direction = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_set_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        direction = FX950_DIRECTION_INGRESS;

        /*
            1.4. Call with stat_mac_param_ptr->data_cycle_threshold [0x10000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        stat_mac_param.data_cycle_threshold = 0x10000;

        result = fx950_api_statistics_interface_set_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, stat_mac_param_ptr->data_cycle_threshold = %d",
                                         device_id, stat_mac_param.data_cycle_threshold);

        stat_mac_param.data_cycle_threshold = 0;

        /*
            1.5. Call with ingr_stat_cfg_ptr->idle_cycle_threshold [0x10] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        stat_mac_param.idle_cycle_threshold = 0x10;

        result = fx950_api_statistics_interface_set_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, stat_mac_param_ptr->idle_cycle_threshold = %d",
                                         device_id, stat_mac_param.idle_cycle_threshold);

        stat_mac_param.idle_cycle_threshold = 0;

        /*
            1.6. Call with ingr_stat_cfg_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_interface_set_mac_params(device_id, direction, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ingr_stat_cfg_ptr = NULL", device_id);
    }

    direction = FX950_DIRECTION_INGRESS;

    stat_mac_param.data_cycle_threshold = 0;
    stat_mac_param.idle_cycle_threshold = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_interface_set_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_interface_set_mac_params(device_id, direction, &stat_mac_param);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_interface_get_mac_params(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     FX950_DIRECTION_TYPE                        direction,
    SAND_OUT    FX950_STATISTICS_INTERFACE_MAC_PARAM_STRUCT *stat_mac_param_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_interface_get_mac_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS]
                   and non-null stat_mac_param_ptr.
    Expected: SAND_OK.
    1.2. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with stat_mac_param_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT            device_id = 0;
    FX950_DIRECTION_TYPE direction = FX950_DIRECTION_INGRESS;

    FX950_STATISTICS_INTERFACE_MAC_PARAM_STRUCT stat_mac_param;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS]
                           and non-null stat_mac_param_ptr.
            Expected: SAND_OK.
        */
        /* iterate with direction = FX950_DIRECTION_INGRESS */
        direction = FX950_DIRECTION_INGRESS;

        result = fx950_api_statistics_interface_get_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /* iterate with direction = FX950_DIRECTION_EGRESS */
        direction = FX950_DIRECTION_EGRESS;

        result = fx950_api_statistics_interface_get_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /*
            1.2. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        direction = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_get_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        direction = FX950_DIRECTION_INGRESS;

        /*
            1.3. Call with stat_mac_param_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_interface_get_mac_params(device_id, direction, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, stat_mac_param_ptr = NULL", device_id);
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
        result = fx950_api_statistics_interface_get_mac_params(device_id, direction, &stat_mac_param);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_interface_get_mac_params(device_id, direction, &stat_mac_param);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_interface_get_counter(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     FX950_STATISTICS_INTERFACE_COUNTER_TYPE     counter_type,
    SAND_IN     FX950_DIRECTION_TYPE                        direction,
    SAND_OUT    SAND_64CNT*                                 result_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_interface_get_counter)
{
/*
    ITERATE_DEVICES
    1.1. Call with counter_type [FX950_STATISTICS_INTERFACE_COUNTER_IN_EVENTS /
                                 FX950_STATISTICS_INTERFACE_COUNTER_PARITY_ERROR]
                   direction [FX950_DIRECTION_INGRESS /
                              FX950_DIRECTION_EGRESS]
                   and non-null stat_mac_param_ptr.
    Expected: SAND_OK.
    1.2. Call with counter_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with result_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT            device_id = 0;
    FX950_STATISTICS_INTERFACE_COUNTER_TYPE counter_type = FX950_STATISTICS_INTERFACE_COUNTER_IN_EVENTS;
    FX950_DIRECTION_TYPE                    direction    = FX950_DIRECTION_INGRESS;
    SAND_64CNT result_ptr;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with counter_type [FX950_STATISTICS_INTERFACE_COUNTER_IN_EVENTS /
                                         FX950_STATISTICS_INTERFACE_COUNTER_PARITY_ERROR]
                           direction [FX950_DIRECTION_INGRESS /
                                      FX950_DIRECTION_EGRESS]
                           and non-null stat_mac_param_ptr.
            Expected: SAND_OK.
        */
        /* iterate with counter_type = FX950_STATISTICS_INTERFACE_COUNTER_IN_EVENTS */
        counter_type = FX950_STATISTICS_INTERFACE_COUNTER_IN_EVENTS;
        direction = FX950_DIRECTION_INGRESS;

        result = fx950_api_statistics_interface_get_counter(device_id, counter_type, direction, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, counter_type, direction);

        /* iterate with counter_type = FX950_STATISTICS_INTERFACE_COUNTER_PARITY_ERROR */
        counter_type = FX950_STATISTICS_INTERFACE_COUNTER_PARITY_ERROR;
        direction = FX950_DIRECTION_INGRESS;

        result = fx950_api_statistics_interface_get_counter(device_id, counter_type, direction, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, counter_type, direction);

        /*
            1.2. Call with counter_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        counter_type = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_get_counter(device_id, counter_type, direction, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, counter_type);

        counter_type = FX950_STATISTICS_INTERFACE_COUNTER_IN_EVENTS;

        /*
            1.3. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        direction = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_get_counter(device_id, counter_type, direction, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, direction = %d", device_id, direction);

        direction = FX950_DIRECTION_INGRESS;

        /*
            1.4. Call with result_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_interface_get_counter(device_id, counter_type, direction, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, result_ptr = NULL", device_id);
    }

    counter_type = FX950_STATISTICS_INTERFACE_COUNTER_IN_EVENTS;
    direction = FX950_DIRECTION_INGRESS;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_interface_get_counter(device_id, counter_type, direction, &result_ptr);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_interface_get_counter(device_id, counter_type, direction, &result_ptr);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_interface_get_all_counters(
    SAND_IN  unsigned int                               device_id,
    SAND_IN  FX950_DIRECTION_TYPE                       direction,
    SAND_OUT FX950_STATISTICS_INTERFACE_ALL_COUNTERS*   all_counters_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_interface_get_all_counters)
{
/*
    ITERATE_DEVICES
    1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS]
                   and non-null stat_mac_param_ptr.
    Expected: SAND_OK.
    1.2. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with all_counters_ptr [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT            device_id = 0;
    FX950_DIRECTION_TYPE direction = FX950_DIRECTION_INGRESS;

    FX950_STATISTICS_INTERFACE_ALL_COUNTERS all_counters;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with direction [FX950_DIRECTION_INGRESS / FX950_DIRECTION_EGRESS]
                           and non-null all_counters_ptr.
            Expected: SAND_OK.
        */
        /* iterate with direction = FX950_DIRECTION_INGRESS */
        direction = FX950_DIRECTION_INGRESS;

        result = fx950_api_statistics_interface_get_all_counters(device_id, direction, &all_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /* iterate with direction = FX950_DIRECTION_EGRESS */
        direction = FX950_DIRECTION_EGRESS;

        result = fx950_api_statistics_interface_get_all_counters(device_id, direction, &all_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        /*
            1.2. Call with direction [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        direction = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_get_all_counters(device_id, direction, &all_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, direction);

        direction = FX950_DIRECTION_INGRESS;

        /*
            1.3. Call with all_counters_ptr [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_interface_get_all_counters(device_id, direction, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_counters_ptr = NULL", device_id);
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
        result = fx950_api_statistics_interface_get_all_counters(device_id, direction, &all_counters);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_interface_get_all_counters(device_id, direction, &all_counters);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_interface_set_egress_cfg(
    SAND_IN     unsigned int                                    device_id,
    SAND_IN     FX950_STATISTICS_INTERFACE_EGRESS_CFG_STRUCT   *egr_stat_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_interface_set_egress_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with egr_stat_cfg_ptr {message_include_vid [FALSE / TRUE],
                                     const_operation [FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT /
                                                      FX950_STATISTICS_INTERFACE_OPERATION_ADD],
                                     const_bc [0 / 0xFFF],
                                     send_statistics_type [FX950_STATISTICS_INTERFACE_SEND_STAT_NONE /
                                                           FX950_STATISTICS_INTERFACE_SEND_STAT_ALL_PACKETS]}
    Expected: SAND_OK.
    1.2. Call fx950_api_statistics_interface_get_egress_cfg with not NULL egr_stat_cfg_ptr.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with egr_stat_cfg_ptr->const_operation [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with egr_stat_cfg_ptr->const_bc [0x1000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with egr_stat_cfg_ptr->send_statistics_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with egr_stat_cfg_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_STATISTICS_INTERFACE_EGRESS_CFG_STRUCT egr_stat_cfg;
    FX950_STATISTICS_INTERFACE_EGRESS_CFG_STRUCT egr_stat_cfgGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with egr_stat_cfg_ptr {message_include_vid [FALSE / TRUE],
                                             const_operation [FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT /
                                                              FX950_STATISTICS_INTERFACE_OPERATION_ADD],
                                             const_bc [0 / 0xFFF],
                                             send_statistics_type [FX950_STATISTICS_INTERFACE_SEND_STAT_NONE /
                                                                   FX950_STATISTICS_INTERFACE_SEND_STAT_ALL_PACKETS]}
            Expected: SAND_OK.
        */
        /* iterate with ingr_stat_cfg_ptr->message_include_vid = FALSE */
        egr_stat_cfg.message_include_vid = FALSE;
        egr_stat_cfg.const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;
        egr_stat_cfg.const_bc = 0;
        egr_stat_cfg.send_statistics_type = FX950_STATISTICS_INTERFACE_SEND_STAT_NONE;

        result = fx950_api_statistics_interface_set_egress_cfg(device_id, &egr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_statistics_interface_get_egress_cfg with not NULL egr_stat_cfg_ptr.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_statistics_interface_get_egress_cfg(device_id, &egr_stat_cfgGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_statistics_interface_get_egress_cfg: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(egr_stat_cfg.message_include_vid, egr_stat_cfgGet.message_include_vid,
                                     "got another egr_stat_cfg_ptr->message_include_vid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(egr_stat_cfg.const_operation, egr_stat_cfgGet.const_operation,
                                     "got another egr_stat_cfg_ptr->const_operation than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(egr_stat_cfg.const_bc, egr_stat_cfgGet.const_bc,
                                     "got another egr_stat_cfg_ptr->const_bc than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(egr_stat_cfg.send_statistics_type, egr_stat_cfgGet.send_statistics_type,
                                     "got another egr_stat_cfg_ptr->send_statistics_type than was set: %d", device_id);

        /* iterate with ingr_stat_cfg_ptr->message_include_vid = TRUE */
        egr_stat_cfg.message_include_vid = TRUE;
        egr_stat_cfg.const_operation = FX950_STATISTICS_INTERFACE_OPERATION_ADD;
        egr_stat_cfg.const_bc = 0xFFF;
        egr_stat_cfg.send_statistics_type = FX950_STATISTICS_INTERFACE_SEND_STAT_ALL_PACKETS;

        result = fx950_api_statistics_interface_set_egress_cfg(device_id, &egr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_statistics_interface_get_egress_cfg with not NULL egr_stat_cfg_ptr.
            Expected: SAND_OK and the same params as was set.
        */
        result = fx950_api_statistics_interface_get_egress_cfg(device_id, &egr_stat_cfgGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_statistics_interface_get_egress_cfg: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(egr_stat_cfg.message_include_vid, egr_stat_cfgGet.message_include_vid,
                                     "got another egr_stat_cfg_ptr->message_include_vid than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(egr_stat_cfg.const_operation, egr_stat_cfgGet.const_operation,
                                     "got another egr_stat_cfg_ptr->const_operation than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(egr_stat_cfg.const_bc, egr_stat_cfgGet.const_bc,
                                     "got another egr_stat_cfg_ptr->const_bc than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(egr_stat_cfg.send_statistics_type, egr_stat_cfgGet.send_statistics_type,
                                     "got another egr_stat_cfg_ptr->send_statistics_type than was set: %d", device_id);

        /*
            1.3. Call with egr_stat_cfg_ptr->const_operation [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        egr_stat_cfg.const_operation = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_set_egress_cfg(device_id, &egr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, egr_stat_cfg_ptr->const_operation = %d",
                                         device_id, egr_stat_cfg.const_operation);

        egr_stat_cfg.const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;

        /*
            1.4. Call with egr_stat_cfg_ptr->const_bc [0x1000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        egr_stat_cfg.const_bc = 0x1000;

        result = fx950_api_statistics_interface_set_egress_cfg(device_id, &egr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, egr_stat_cfg_ptr->const_bc = %d",
                                         device_id, egr_stat_cfg.const_bc);

        egr_stat_cfg.const_bc = 0;

        /*
            1.5. Call with egr_stat_cfg_ptr->send_statistics_type [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        egr_stat_cfg.send_statistics_type = SAND_UTF_STATISTICS_INTERFACE_INVALID_ENUM;

        result = fx950_api_statistics_interface_set_egress_cfg(device_id, &egr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, egr_stat_cfg_ptr->send_statistics_type = %d",
                                         device_id, egr_stat_cfg.send_statistics_type);

        egr_stat_cfg.send_statistics_type = FX950_STATISTICS_INTERFACE_SEND_STAT_NONE;

        /*
            1.6. Call with egr_stat_cfg_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_interface_set_egress_cfg(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, egr_stat_cfg_ptr = NULL", device_id);
    }

    egr_stat_cfg.message_include_vid = FALSE;
    egr_stat_cfg.const_operation = FX950_STATISTICS_INTERFACE_OPERATION_SUBSTRUCT;
    egr_stat_cfg.const_bc = 0;
    egr_stat_cfg.send_statistics_type = FX950_STATISTICS_INTERFACE_SEND_STAT_NONE;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_interface_set_egress_cfg(device_id, &egr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_interface_set_egress_cfg(device_id, &egr_stat_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_statistics_interface_get_egress_cfg(
    SAND_IN     unsigned int                                    device_id,
    SAND_OUT    FX950_STATISTICS_INTERFACE_EGRESS_CFG_STRUCT   *egr_stat_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_statistics_interface_get_egress_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null egr_stat_cfg_ptr.
    Expected: SAND_OK.
    1.2. Call with egr_stat_cfg_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FX950_STATISTICS_INTERFACE_EGRESS_CFG_STRUCT egr_stat_cfg;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null egr_stat_cfg_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_statistics_interface_get_egress_cfg(device_id, &egr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with egr_stat_cfg_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_statistics_interface_get_egress_cfg(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, egr_stat_cfg_ptr = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_statistics_interface_get_egress_cfg(device_id, &egr_stat_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_statistics_interface_get_egress_cfg(device_id, &egr_stat_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_statistics_interface suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_statistics_interface)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_interface_set_ingress_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_interface_get_ingress_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_interface_set_mac_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_interface_get_mac_params)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_interface_get_counter)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_interface_get_all_counters)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_interface_set_egress_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_statistics_interface_get_egress_cfg)
UTF_SUIT_END_TESTS_MAC(fx950_api_statistics_interface)

