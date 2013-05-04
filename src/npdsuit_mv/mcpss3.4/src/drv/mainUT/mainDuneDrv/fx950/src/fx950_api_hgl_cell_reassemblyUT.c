/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fx950_api_hgl_cell_reassemblyUT.c
*
* DESCRIPTION:
*       Unit tests for fx950_api_hgl_cell_reassembly, that provides
*       HyperG.link Cell Reasembly API of 98FX950 device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fx950_api_hgl_cell_reassembly.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_HGL_CELL_REASSEMBLY_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_cell_reassembly_set_link_config(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     unsigned long                               link_number,
    SAND_IN     FX950_HGL_CELL_REASSEMBLY_LINK_CFG_STRUCT   *cell_reassembly_link_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_cell_reassembly_set_link_config)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_number[0 / 1], 
                   cell_reassembly_link_cfg_ptr{ link_enable[TRUE / FALSE], 
                                                 interface_type[FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_CELLS / FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_PACKETS], 
                                                 mh_desc_type[FX950_MH_FORMAT_STANDART / FX950_MH_FORMAT_EXTENDED] }
    Expected: SAND_OK.
    1.2. Call with fx950_api_hgl_cell_reassembly_get_link_config with not NULL pointer and link_number as in 1.1.
    Expected: SAND_OK.
    1.3. Call with link_number [2] (out of range) 
              and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call cell_reassembly_link_cfg_ptr->interface_type [0x5AAAAAA5] (out of range) 
              and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with cell_reassembly_link_cfg_ptr->mh_desc_type [0x5AAAAAA5] (out of range)
              and other params from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with cell_reassembly_link_cfg_ptr [NULL] and other param as in 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id   = 0;
    SAND_U32  link_number = 0;

    FX950_HGL_CELL_REASSEMBLY_LINK_CFG_STRUCT cell_reassembly_link_cfg;
    FX950_HGL_CELL_REASSEMBLY_LINK_CFG_STRUCT cell_reassembly_link_cfgGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_number[0 / 1], 
                           cell_reassembly_link_cfg_ptr{ link_enable[TRUE / FALSE], 
                                                         interface_type[FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_CELLS / FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_PACKETS], 
                                                         mh_desc_type[FX950_MH_FORMAT_STANDART / FX950_MH_FORMAT_EXTENDED] }
            Expected: SAND_OK.
        */
        /* iterate with link_number = 0 */
        link_number = 0;
        cell_reassembly_link_cfg.link_enable = TRUE;
        cell_reassembly_link_cfg.interface_type = FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_CELLS;
        cell_reassembly_link_cfg.mh_desc_type = FX950_MH_FORMAT_STANDART;

        result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, &cell_reassembly_link_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        /*
            1.2. Call with fx950_api_hgl_cell_reassembly_get_link_config with not NULL pointer and link_number as in 1.1.
            Expected: SAND_OK.
        */
        result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, &cell_reassembly_link_cfgGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_cell_reassembly_get_link_config: %d, %d", device_id, link_number);

        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_link_cfg.link_enable, cell_reassembly_link_cfgGet.link_enable, 
                                     "got another cell_reassembly_link_cfg.link_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_link_cfg.interface_type, cell_reassembly_link_cfgGet.interface_type, 
                                     "got another cell_reassembly_link_cfg.interface_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_link_cfg.mh_desc_type, cell_reassembly_link_cfgGet.mh_desc_type, 
                                     "got another cell_reassembly_link_cfg.mh_desc_type than was set: %d", device_id);

        /* iterate with link_number = 1 */
        link_number = 1;
        cell_reassembly_link_cfg.link_enable = FALSE;
        cell_reassembly_link_cfg.interface_type = FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_PACKETS;
        cell_reassembly_link_cfg.mh_desc_type = FX950_MH_FORMAT_EXTENDED;

        result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, &cell_reassembly_link_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        /*
            1.2. Call with fx950_api_hgl_cell_reassembly_get_link_config with not NULL pointer and link_number as in 1.1.
            Expected: SAND_OK.
        */
        result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, &cell_reassembly_link_cfgGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fx950_api_hgl_cell_reassembly_get_link_config: %d, %d", device_id, link_number);

        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_link_cfg.link_enable, cell_reassembly_link_cfgGet.link_enable, 
                                     "got another cell_reassembly_link_cfg.link_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_link_cfg.interface_type, cell_reassembly_link_cfgGet.interface_type, 
                                     "got another cell_reassembly_link_cfg.interface_type than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_link_cfg.mh_desc_type, cell_reassembly_link_cfgGet.mh_desc_type, 
                                     "got another cell_reassembly_link_cfg.mh_desc_type than was set: %d", device_id);

        /*
            1.3. Call with link_number [2] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_number = 2;

        result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, &cell_reassembly_link_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        link_number = 0;

        /*
            1.4. Call cell_reassembly_link_cfg_ptr->interface_type [0x5AAAAAA5] (out of range)
                      and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        cell_reassembly_link_cfg.interface_type = SAND_UTF_HGL_CELL_REASSEMBLY_INVALID_ENUM;

        result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, &cell_reassembly_link_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, cell_reassembly_link_cfg.interface_type = %d",
                                         device_id, cell_reassembly_link_cfg.interface_type);

        cell_reassembly_link_cfg.interface_type = FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_CELLS;

        /*
            1.5. Call with cell_reassembly_link_cfg_ptr->mh_desc_type [0x5AAAAAA5] (out of range)
                      and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        cell_reassembly_link_cfg.mh_desc_type = SAND_UTF_HGL_CELL_REASSEMBLY_INVALID_ENUM;

        result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, &cell_reassembly_link_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, cell_reassembly_link_cfg.mh_desc_type = %d",
                                         device_id, cell_reassembly_link_cfg.mh_desc_type);

        cell_reassembly_link_cfg.mh_desc_type = FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_CELLS;

        /*
            1.6. Call with cell_reassembly_link_cfg_ptr [NULL] and other param as in 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, cell_reassembly_link_cfg = NULL", device_id);
    }

    link_number = 0;
    cell_reassembly_link_cfg.link_enable    = TRUE;
    cell_reassembly_link_cfg.interface_type = FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE_CELLS;
    cell_reassembly_link_cfg.mh_desc_type   = FX950_MH_FORMAT_STANDART;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, &cell_reassembly_link_cfg);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, &cell_reassembly_link_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_cell_reassembly_get_link_config(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     unsigned long                               link_number,
    SAND_OUT    FX950_HGL_CELL_REASSEMBLY_LINK_CFG_STRUCT   *cell_reassembly_link_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_cell_reassembly_get_link_config)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_number [0 / 1] not NULL cell_reassembly_link_cfg_ptr.
    Expected: SAND_OK.
    1.2. Call with link_number [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with cell_reassembly_link_cfg_ptr [NULL] and other param from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id  = 0;
    SAND_U32 link_number = 0;
    FX950_HGL_CELL_REASSEMBLY_LINK_CFG_STRUCT cell_reassembly_link_cfg;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_number [0 / 1] not NULL cell_reassembly_link_cfg_ptr.
            Expected: SAND_OK.
        */
        /* iterate with link_number = 0 */
        link_number = 0;

        result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, &cell_reassembly_link_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        /* iterate with link_number = 1 */
        link_number = 1;

        result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, &cell_reassembly_link_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        /*
            1.2. Call with link_number [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        link_number = 2;

        result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, &cell_reassembly_link_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        link_number = 1;

        /*
            1.3. Call with cell_reassembly_link_cfg_ptr [NULL] and other param from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, cell_reassembly_link_cfg = NULL", device_id);
    }

    link_number = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, &cell_reassembly_link_cfg);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, &cell_reassembly_link_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_cell_reassembly_set_engine_cfg(
    SAND_IN     unsigned int                                 device_id,
    SAND_IN     FX950_HGL_CELL_REASSEMBLY_ENGINE_CFG_STRUCT 
                                                *cell_reassembly_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_cell_reassembly_set_engine_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with cell_reassembly_cfg_ptr { mc_id0_enable[TRUE / FALSE],
                                             fc_enable[TRUE / FALSE],
                                             max_bc[0 / 0x3fff] }
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_cell_reassembly_get_engine_cfg with not NULL pointer.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with cell_reassembly_cfg_ptr->max_bc = 0x4000 (out of range)
              and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with cell_reassembly_cfg_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FX950_HGL_CELL_REASSEMBLY_ENGINE_CFG_STRUCT cell_reassembly_cfg;
    FX950_HGL_CELL_REASSEMBLY_ENGINE_CFG_STRUCT cell_reassembly_cfgGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with cell_reassembly_cfg_ptr { mc_id0_enable[TRUE / FALSE],
                                                     fc_enable[TRUE / FALSE],
                                                     max_bc[0 / 0x3fff] }
            Expected: SAND_OK.
        */
        /* iterate with TRUE */
        cell_reassembly_cfg.mc_id0_enable = TRUE;
        cell_reassembly_cfg.fc_enable = TRUE;
        cell_reassembly_cfg.max_bc = 0;

        result = fx950_api_hgl_cell_reassembly_set_engine_cfg(device_id, &cell_reassembly_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_hgl_cell_reassembly_get_engine_cfg with not NULL pointer.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_cell_reassembly_get_engine_cfg(device_id, &cell_reassembly_cfgGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chunk_context_manager_get_const_context_id: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_cfg.mc_id0_enable, cell_reassembly_cfgGet.mc_id0_enable, 
                                     "got another cell_reassembly_cfg.mc_id0_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_cfg.fc_enable, cell_reassembly_cfgGet.fc_enable, 
                                     "got another cell_reassembly_cfg.fc_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_cfg.max_bc, cell_reassembly_cfgGet.max_bc, 
                                     "got another cell_reassembly_cfg.max_bc than was set: %d", device_id);

        /* iterate with FALSE */
        cell_reassembly_cfg.mc_id0_enable = FALSE;
        cell_reassembly_cfg.fc_enable = FALSE;
        cell_reassembly_cfg.max_bc = 0x3fff;

        result = fx950_api_hgl_cell_reassembly_set_engine_cfg(device_id, &cell_reassembly_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fx950_api_hgl_cell_reassembly_get_engine_cfg with not NULL pointer.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_cell_reassembly_get_engine_cfg(device_id, &cell_reassembly_cfgGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fx950_api_chunk_context_manager_get_const_context_id: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_cfg.mc_id0_enable, cell_reassembly_cfgGet.mc_id0_enable, 
                                     "got another cell_reassembly_cfg.mc_id0_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_cfg.fc_enable, cell_reassembly_cfgGet.fc_enable, 
                                     "got another cell_reassembly_cfg.fc_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(cell_reassembly_cfg.max_bc, cell_reassembly_cfgGet.max_bc, 
                                     "got another cell_reassembly_cfg.max_bc than was set: %d", device_id);

        /*
            1.3. Call with cell_reassembly_cfg_ptr->max_bc = 0x4000 
                      (out of range) and otherparams from 1.1.
            Expected: NOT SAND_OK.
        */
        cell_reassembly_cfg.max_bc = 0x4000;

        result = fx950_api_hgl_cell_reassembly_set_engine_cfg(device_id, &cell_reassembly_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        cell_reassembly_cfg.max_bc = 0;

        /*
            1.4. Call with cell_reassembly_cfg_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_cell_reassembly_set_engine_cfg(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, cell_reassembly_cfg = NULL", device_id);
    }

    cell_reassembly_cfg.mc_id0_enable = TRUE;
    cell_reassembly_cfg.fc_enable = TRUE;
    cell_reassembly_cfg.max_bc = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_cell_reassembly_set_engine_cfg(device_id, &cell_reassembly_cfg);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_cell_reassembly_set_engine_cfg(device_id, &cell_reassembly_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_cell_reassembly_get_engine_cfg(
    SAND_IN     unsigned int                                device_id,
    SAND_OUT    FX950_HGL_CELL_REASSEMBLY_ENGINE_CFG_STRUCT *cell_reassembly_cfg_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_cell_reassembly_get_engine_cfg)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL cell_reassembly_cfg_ptr.
    Expected: SAND_OK.
    1.2. Call with cell_reassembly_cfg_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FX950_HGL_CELL_REASSEMBLY_ENGINE_CFG_STRUCT cell_reassembly_cfg;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL cell_reassembly_cfg_ptr.
            Expected: SAND_OK.
        */
        result = fx950_api_hgl_cell_reassembly_get_engine_cfg(device_id, &cell_reassembly_cfg);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with cell_reassembly_cfg_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_cell_reassembly_get_engine_cfg(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, cell_reassembly_cfg = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_cell_reassembly_get_engine_cfg(device_id, &cell_reassembly_cfg);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_cell_reassembly_get_engine_cfg(device_id, &cell_reassembly_cfg);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_cell_reassembly_set_buffer_thresholds(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     unsigned long                               link_number,
    SAND_IN     FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE       buffer_type,
    SAND_IN     FX950_HGL_CELL_REASSEMBLY_THRESHOLD_ENTRY   *buffer_threshold_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_cell_reassembly_set_buffer_thresholds)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_number [0 / 1], 
                   buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_UC / 
                                FX950_HGL_CELL_REASSEMBLY_BUFFER_MH], 
                   buffer_threshold { xoff_threshold [0 / 0x1ff],
                                      drop_threshold [0 / 0x1ff],
                                      xon_threshold [0 / 0x1ff] }
    Expected: SAND_OK.
    1.2. Call fx950_api_hgl_cell_reassembly_get_buffer_thresholds 
              with not NULL buffer_threshold_ptr and other param from 1.1.
    Expected: SAND_OK and the same param as was set.
    1.3. Call with link_number [2] and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with buffer_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.

    1.5. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MC],
                   buffer_threshold_ptr->xoff_threshold [0x100] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MC],
                   buffer_threshold_ptr->drop_threshold [0x100] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MC],
                   buffer_threshold_ptr->xon_threshold [0x100] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.


    1.8. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                   buffer_threshold_ptr->xoff_threshold [0x100]
                   and other param from 1.1.
    Expected: SAND_OK.
    1.9. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                   buffer_threshold_ptr->drop_threshold [0x100]
                   and other param from 1.1.
    Expected: SAND_OK.
    1.10. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                   buffer_threshold_ptr->xon_threshold [0x100]
                   and other param from 1.1.
    Expected: SAND_OK.


    1.11. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                   buffer_threshold_ptr->xoff_threshold [0x200] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.12. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                   buffer_threshold_ptr->drop_threshold [0x200] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.
    1.13. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                   buffer_threshold_ptr->xon_threshold [0x200] (out of range)
                   and other param from 1.1.
    Expected: NOT SAND_OK.

    1.14. Call with buffer_threshold_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT                             device_id = 0;
    SAND_U32                              link_number = 0;
    FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_UC;

    FX950_HGL_CELL_REASSEMBLY_THRESHOLD_ENTRY buffer_threshold;
    FX950_HGL_CELL_REASSEMBLY_THRESHOLD_ENTRY buffer_thresholdGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_number [0 / 1], 
                           buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_UC / 
                                        FX950_HGL_CELL_REASSEMBLY_BUFFER_MH], 
                           buffer_threshold { xoff_threshold [0 / 0x1ff],
                                              drop_threshold [0 / 0x1ff],
                                              xon_threshold [0 / 0x1ff] }
            Expected: SAND_OK.
        */
        /* iterate with link_number = 0 */
        link_number = 0;
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_UC;
        buffer_threshold.xoff_threshold = 0;
        buffer_threshold.drop_threshold = 0;
        buffer_threshold.xon_threshold = 0;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, link_number, buffer_type, &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_number, buffer_type);

        /*
            1.2. Call fx950_api_hgl_cell_reassembly_get_buffer_thresholds 
            with not NULL buffer_threshold_ptr and other param from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_thresholdGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_chunk_context_manager_get_const_context_id: %d, %d, %d", 
                                     device_id, link_number, buffer_type);

        UTF_VERIFY_EQUAL1_STRING_MAC(buffer_threshold.xoff_threshold, buffer_thresholdGet.xoff_threshold, 
                                     "got another buffer_threshold.xoff_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(buffer_threshold.drop_threshold, buffer_thresholdGet.drop_threshold, 
                                     "got another buffer_threshold.drop_threshold than was set: %d", device_id);        
        UTF_VERIFY_EQUAL1_STRING_MAC(buffer_threshold.xon_threshold, buffer_thresholdGet.xon_threshold, 
                                     "got another buffer_threshold.xon_threshold than was set: %d", device_id);

        /* iterate with link_number = 1 */
        link_number = 1;
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MH;
        buffer_threshold.xoff_threshold = 0x1ff;
        buffer_threshold.drop_threshold = 0x1ff;
        buffer_threshold.xon_threshold = 0x1ff;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, link_number, buffer_type, &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_number, buffer_type);

        /*
            1.2. Call fx950_api_hgl_cell_reassembly_get_buffer_thresholds 
            with not NULL buffer_threshold_ptr and other param from 1.1.
            Expected: SAND_OK and the same param as was set.
        */
        result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_thresholdGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fx950_api_chunk_context_manager_get_const_context_id: %d, %d, %d", 
                                     device_id, link_number, buffer_type);

        UTF_VERIFY_EQUAL1_STRING_MAC(buffer_threshold.xoff_threshold, buffer_thresholdGet.xoff_threshold, 
                                     "got another buffer_threshold.xoff_threshold than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(buffer_threshold.drop_threshold, buffer_thresholdGet.drop_threshold, 
                                     "got another buffer_threshold.drop_threshold than was set: %d", device_id);        
        UTF_VERIFY_EQUAL1_STRING_MAC(buffer_threshold.xon_threshold, buffer_thresholdGet.xon_threshold, 
                                     "got another buffer_threshold.xon_threshold than was set: %d", device_id);

        /*
            1.3. Call with link_number [2] and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        link_number = 2;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        link_number = 0;

        /*
            1.4. Call with buffer_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buffer_type = SAND_UTF_HGL_CELL_REASSEMBLY_INVALID_ENUM;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_type = %d", device_id, buffer_type);

        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_UC;

        /*
            1.5. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MC],
                           buffer_threshold_ptr->xoff_threshold [0x100] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MC;
        buffer_threshold.xoff_threshold = 0x400;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.xoff_threshold = %d", 
                                         device_id, buffer_threshold.xoff_threshold);

        buffer_threshold.xoff_threshold = 0;

        /*
            1.6. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MC],
                           buffer_threshold_ptr->drop_threshold [0x400] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MC;
        buffer_threshold.drop_threshold = 0x400;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.drop_threshold = %d", 
                                         device_id, buffer_threshold.drop_threshold);

        buffer_threshold.drop_threshold = 0;
        
        /*
            1.7. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MC],
                           buffer_threshold_ptr->xon_threshold [0x100] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MC;
        buffer_threshold.xon_threshold = 0x400;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.xon_threshold = %d", 
                                         device_id, buffer_threshold.xon_threshold);

        buffer_threshold.xon_threshold = 0;

        /*
            1.8. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                           buffer_threshold_ptr->xoff_threshold [0x100]
                           and other param from 1.1.
            Expected: SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MH;
        buffer_threshold.xoff_threshold = 0x100;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.xoff_threshold = %d", 
                                     device_id, buffer_threshold.xoff_threshold);

        buffer_threshold.xoff_threshold = 0;
        
        /*
            1.9. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                           buffer_threshold_ptr->drop_threshold [0x100]
                           and other param from 1.1.
            Expected: SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MH;
        buffer_threshold.drop_threshold = 0x100;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.drop_threshold = %d", 
                                     device_id, buffer_threshold.drop_threshold);

        buffer_threshold.drop_threshold = 0;

        /*
            1.10. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                           buffer_threshold_ptr->xon_threshold [0x100]
                           and other param from 1.1.
            Expected: SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MH;
        buffer_threshold.xon_threshold = 0x100;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.xon_threshold = %d", 
                                     device_id, buffer_threshold.xon_threshold);

        buffer_threshold.xon_threshold = 0;
        
        /*
            1.11. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                           buffer_threshold_ptr->xoff_threshold [0x200] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MH;
        buffer_threshold.xoff_threshold = 0x200;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.xoff_threshold = %d", 
                                         device_id, buffer_threshold.xoff_threshold);

        buffer_threshold.xoff_threshold = 0;

        /*
            1.12. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                           buffer_threshold_ptr->drop_threshold [0x200] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MH;
        buffer_threshold.drop_threshold = 0x200;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.drop_threshold = %d", 
                                         device_id, buffer_threshold.drop_threshold);

        buffer_threshold.drop_threshold = 0;
        
        /*
            1.13. Call with buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_MH],
                           buffer_threshold_ptr->xon_threshold [0x200] (out of range)
                           and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MH;
        buffer_threshold.xon_threshold = 0x200;

        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_threshold.xon_threshold = %d", 
                                         device_id, buffer_threshold.xon_threshold);

        buffer_threshold.xon_threshold = 0;
        
        /*
            1.8. Call with buffer_threshold_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, buffer_threshold = NULL", device_id);
    }

    link_number = 0;
    buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_UC;
    buffer_threshold.xoff_threshold = 0;
    buffer_threshold.drop_threshold = 0;
    buffer_threshold.xon_threshold  = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                             link_number, 
                                                             buffer_type, 
                                                             &buffer_threshold);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, 
                                                             link_number, 
                                                             buffer_type, 
                                                             &buffer_threshold);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fx950_api_hgl_cell_reassembly_get_buffer_thresholds(
    SAND_IN     unsigned int                                device_id,
    SAND_IN     unsigned long                               link_number,
    SAND_IN     FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE       buffer_type,
    SAND_OUT    FX950_HGL_CELL_REASSEMBLY_THRESHOLD_ENTRY   *buffer_threshold_ptr
  )
*/
UTF_TEST_CASE_MAC(fx950_api_hgl_cell_reassembly_get_buffer_thresholds)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_number [0 / 1], 
                   buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_UC / 
                                FX950_HGL_CELL_REASSEMBLY_BUFFER_MC], 
                   not NULL buffer_threshold_ptr.
    Expected: SAND_OK.
    1.2. Call with link_number [2] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with buffer_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with buffer_threshold_ptr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U32  link_number = 0;
    FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_UC;
    FX950_HGL_CELL_REASSEMBLY_THRESHOLD_ENTRY buffer_threshold;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_number [0 / 1], 
                           buffer_type [FX950_HGL_CELL_REASSEMBLY_BUFFER_UC / 
                                        FX950_HGL_CELL_REASSEMBLY_BUFFER_MC], 
                           not NULL buffer_threshold_ptr.
            Expected: SAND_OK.
        */
        /* iterate with link_number = 0 */
        link_number = 0;
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_UC;

        result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, link_number, buffer_type, &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        /* iterate with link_number = 1 */
        link_number = 1;
        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_MC;

        result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, link_number, buffer_type, &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        /*
            1.2. Call with link_number [2] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        link_number = 2;

        result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, link_number, buffer_type, &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_number);

        link_number = 0;

        /*
            1.3. Call with buffer_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        buffer_type = SAND_UTF_HGL_CELL_REASSEMBLY_INVALID_ENUM;

        result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, buffer_type = %d", 
                                         device_id, buffer_type);

        buffer_type = FX950_HGL_CELL_REASSEMBLY_BUFFER_UC;

        /*
            1.4. Call with buffer_threshold_ptr [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, buffer_threshold = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, 
                                                                     link_number, 
                                                                     buffer_type, 
                                                                     &buffer_threshold);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, 
                                                                 link_number, 
                                                                 buffer_type, 
                                                                 &buffer_threshold);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fx950_api_hgl_cell_reassembly suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fx950_api_hgl_cell_reassembly)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_cell_reassembly_set_link_config)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_cell_reassembly_get_link_config)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_cell_reassembly_set_engine_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_cell_reassembly_get_engine_cfg)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_cell_reassembly_set_buffer_thresholds)
    UTF_SUIT_DECLARE_TEST_MAC(fx950_api_hgl_cell_reassembly_get_buffer_thresholds)
UTF_SUIT_END_TESTS_MAC(fx950_api_hgl_cell_reassembly)

