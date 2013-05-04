/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_cellUT.
*
* DESCRIPTION:
*       Unit tests for fap20m_api_cell, that provides
*       cell API of FAP20M device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_cell.h>
#include <SAND/Utils/include/sand_os_interface.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_CELL_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_send_sr_data_cell(
    unsigned int  device_id,
    SAND_TX_SR_DATA_CELL* sr_data_cell
  )
*/
UTF_TEST_CASE_MAC(fap20m_send_sr_data_cell)
{
/*
    ITERATE_DEVICES
    1.1. Call with sr_data_cell {body_tx [0 .. 0 /
                                          0xFF .. 0xFF /
                                          0xFFFF .. 0xFFFF /
                                          0xFFFFFFFF .. 0xFFFFFFFF],
                                 fe3_link [0 / 16 / 25 / 31],
                                 fe2_link [0 / 32 / 48 / 63],
                                 fe1_link [0 / 16 / 25 / 31],
                                 destination_entity_type [SAND_FE1_ENTITY /
                                                          SAND_FE2_ENTITY /
                                                          SAND_FE3_ENTITY /
                                                          SAND_FOP_ENTITY]},
                                 source_entity_type [SAND_FE1_ENTITY /
                                                     SAND_FE2_ENTITY /
                                                     SAND_FE3_ENTITY /
                                                     SAND_FOP_ENTITY]},
                                 source_chip_id [0 / 1023 / 2047 / -1],
                                 initial_output_link [0 / 8 / 32 / 63].
    Expected: SAND_OK.
    1.2. Call fap20m_recv_sr_data_cell with not NULL pointer
    Expected: SAND_OK ad the same params as was set.
    1.3. Call with sr_data_cell->fe3_link [32] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with sr_data_cell->fe2_link [64] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with sr_data_cell->fe1_link [32] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.6. Call with sr_data_cell->destination_entity_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.7. Call with sr_data_cell->source_entity_type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.8. Call with sr_data_cell->source_chip_id [2048] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.9. Call with sr_data_cell->initial_output_link [64] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    SAND_U8   iter      = 0;
    SAND_BOOL isEqual   = FALSE;

    SAND_TX_SR_DATA_CELL     sr_data_cell;
    FAP20M_RX_SR_DATA_BUFFER sr_data_cellGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sr_data_cell {body_tx [0 .. 0 /
                                                  0xFF .. 0xFF /
                                                  0xFFFF .. 0xFFFF /
                                                  0xFFFFFFFF .. 0xFFFFFFFF],
                                         fe3_link [0 / 16 / 25 / 31],
                                         fe2_link [0 / 32 / 48 / 63],
                                         fe1_link [0 / 16 / 25 / 31],
                                         destination_entity_type [SAND_FE1_ENTITY /
                                                                  SAND_FE2_ENTITY /
                                                                  SAND_FE3_ENTITY /
                                                                  SAND_FOP_ENTITY]},
                                         source_entity_type [SAND_FE1_ENTITY /
                                                             SAND_FE2_ENTITY /
                                                             SAND_FE3_ENTITY /
                                                             SAND_FOP_ENTITY]},
                                         source_chip_id [0 / 1023 / 2047 / -1],
                                         initial_output_link [0 / 8 / 32 / 63].
            Expected: SAND_OK.
        */
        /* iterate with body_tx [0 .. 0] */
        for (iter = 0; iter < SAND_SR_DATA_NOF_DATA_LONGS; iter++)
        {
            sr_data_cell.body_tx[iter] = 0;
        }

        sr_data_cell.fe3_link = 0;
        sr_data_cell.fe2_link = 0;
        sr_data_cell.fe1_link = 0;

        sr_data_cell.destination_entity_type = SAND_FE1_ENTITY;
        sr_data_cell.source_entity_type = SAND_FE1_ENTITY;
        sr_data_cell.source_chip_id = 0;
        sr_data_cell.initial_output_link = 0;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_recv_sr_data_cell with not NULL pointer
            Expected: SAND_OK ad the same params as was set.
        */
        result = fap20m_recv_sr_data_cell(device_id, &sr_data_cellGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_recv_sr_data_cell: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(0, sr_data_cellGet.received_not_sr_data_cell,
                   "received cell is NOT SR data cell: %d", device_id);

        if (0 == sr_data_cellGet.received_not_sr_data_cell)
        {
            /* Verifying values */
            isEqual = (SAND_BOOL) ((0 == sand_os_memcmp((void*) sr_data_cell.body_tx,
                                                        (void*) sr_data_cellGet.rx_cell.body_tx,
                                                        sizeof(sr_data_cell.body_tx[0]) * SAND_SR_DATA_NOF_DATA_LONGS)) ? TRUE : FALSE);
            UTF_VERIFY_EQUAL1_STRING_MAC(TRUE, isEqual,
                       "get another sr_data_cell->body_tx than was set: %d", device_id);
    
            UTF_VERIFY_EQUAL1_STRING_MAC(sr_data_cell.source_entity_type, sr_data_cellGet.rx_cell.source_entity_type,
                       "get another sr_data_cell->source_entity_type than was set: %d", device_id);
            UTF_VERIFY_EQUAL1_STRING_MAC(sr_data_cell.source_chip_id, sr_data_cellGet.rx_cell.source_chip_id,
                       "get another sr_data_cell->source_chip_id than was set: %d", device_id);
        }
        
        /* iterate with body_tx [0xFF .. 0xFF] */
        for (iter = 0; iter < SAND_SR_DATA_NOF_DATA_LONGS; iter++)
        {
            sr_data_cell.body_tx[iter] = 0xFF;
        }

        sr_data_cell.fe3_link = 16;
        sr_data_cell.fe2_link = 32;
        sr_data_cell.fe1_link = 16;

        sr_data_cell.destination_entity_type = SAND_FE2_ENTITY;
        sr_data_cell.source_entity_type = SAND_FE2_ENTITY;
        sr_data_cell.source_chip_id = 1023;
        sr_data_cell.initial_output_link = 8;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_recv_sr_data_cell with not NULL pointer
            Expected: SAND_OK ad the same params as was set.
        */
        result = fap20m_recv_sr_data_cell(device_id, &sr_data_cellGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_recv_sr_data_cell: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(0, sr_data_cellGet.received_not_sr_data_cell,
                   "received cell is NOT SR data cell: %d", device_id);

        if (0 == sr_data_cellGet.received_not_sr_data_cell)
        {
            /* Verifying values */
            isEqual = (SAND_BOOL) ((0 == sand_os_memcmp((void*) sr_data_cell.body_tx,
                                                        (void*) sr_data_cellGet.rx_cell.body_tx,
                                                        sizeof(sr_data_cell.body_tx[0]) * SAND_SR_DATA_NOF_DATA_LONGS)) ? TRUE : FALSE);
            UTF_VERIFY_EQUAL1_STRING_MAC(TRUE, isEqual,
                       "get another sr_data_cell->body_tx than was set: %d", device_id);
    
            UTF_VERIFY_EQUAL1_STRING_MAC(sr_data_cell.source_entity_type, sr_data_cellGet.rx_cell.source_entity_type,
                       "get another sr_data_cell->source_entity_type than was set: %d", device_id);
            UTF_VERIFY_EQUAL1_STRING_MAC(sr_data_cell.source_chip_id, sr_data_cellGet.rx_cell.source_chip_id,
                       "get another sr_data_cell->source_chip_id than was set: %d", device_id);
        }

        /* iterate with body_tx [0xFFFF .. 0xFFFF] */
        for (iter = 0; iter < SAND_SR_DATA_NOF_DATA_LONGS; iter++)
        {
            sr_data_cell.body_tx[iter] = 0xFFFF;
        }

        sr_data_cell.fe3_link = 25;
        sr_data_cell.fe2_link = 48;
        sr_data_cell.fe1_link = 25;

        sr_data_cell.destination_entity_type = SAND_FE3_ENTITY;
        sr_data_cell.source_entity_type = SAND_FE3_ENTITY;
        sr_data_cell.source_chip_id = 2047;
        sr_data_cell.initial_output_link = 32;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_recv_sr_data_cell with not NULL pointer
            Expected: SAND_OK ad the same params as was set.
        */
        result = fap20m_recv_sr_data_cell(device_id, &sr_data_cellGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_recv_sr_data_cell: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(0, sr_data_cellGet.received_not_sr_data_cell,
                   "received cell is NOT SR data cell: %d", device_id);

        if (0 == sr_data_cellGet.received_not_sr_data_cell)
        {
            /* Verifying values */
            isEqual = (SAND_BOOL) ((0 == sand_os_memcmp((void*) sr_data_cell.body_tx,
                                                        (void*) sr_data_cellGet.rx_cell.body_tx,
                                                        sizeof(sr_data_cell.body_tx[0]) * SAND_SR_DATA_NOF_DATA_LONGS)) ? TRUE : FALSE);
            UTF_VERIFY_EQUAL1_STRING_MAC(TRUE, isEqual,
                       "get another sr_data_cell->body_tx than was set: %d", device_id);
    
            UTF_VERIFY_EQUAL1_STRING_MAC(sr_data_cell.source_entity_type, sr_data_cellGet.rx_cell.source_entity_type,
                       "get another sr_data_cell->source_entity_type than was set: %d", device_id);
            UTF_VERIFY_EQUAL1_STRING_MAC(sr_data_cell.source_chip_id, sr_data_cellGet.rx_cell.source_chip_id,
                       "get another sr_data_cell->source_chip_id than was set: %d", device_id);
        }

        /* iterate with body_tx [0xFFFFFFFF .. 0xFFFFFFFF] */
        for (iter = 0; iter < SAND_SR_DATA_NOF_DATA_LONGS; iter++)
        {
            sr_data_cell.body_tx[iter] = 0xFFFFFFFF;
        }

        sr_data_cell.fe3_link = 31;
        sr_data_cell.fe2_link = 63;
        sr_data_cell.fe1_link = 31;

        sr_data_cell.destination_entity_type = SAND_FOP_ENTITY;
        sr_data_cell.source_entity_type = SAND_FOP_ENTITY;
        sr_data_cell.source_chip_id = (SAND_UINT) -1;
        sr_data_cell.initial_output_link = 63;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_recv_sr_data_cell with not NULL pointer
            Expected: SAND_OK ad the same params as was set.
        */
        result = fap20m_recv_sr_data_cell(device_id, &sr_data_cellGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_recv_sr_data_cell: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(0, sr_data_cellGet.received_not_sr_data_cell,
                   "received cell is NOT SR data cell: %d", device_id);

        if (0 == sr_data_cellGet.received_not_sr_data_cell)
        {
            /* Verifying values */
            isEqual = (SAND_BOOL) ((0 == sand_os_memcmp((void*) sr_data_cell.body_tx,
                                                        (void*) sr_data_cellGet.rx_cell.body_tx,
                                                        sizeof(sr_data_cell.body_tx[0]) * SAND_SR_DATA_NOF_DATA_LONGS)) ? TRUE : FALSE);
            UTF_VERIFY_EQUAL1_STRING_MAC(TRUE, isEqual,
                       "get another sr_data_cell->body_tx than was set: %d", device_id);
    
            UTF_VERIFY_EQUAL1_STRING_MAC(sr_data_cell.source_entity_type, sr_data_cellGet.rx_cell.source_entity_type,
                       "get another sr_data_cell->source_entity_type than was set: %d", device_id);
            UTF_VERIFY_EQUAL1_STRING_MAC(sr_data_cell.source_chip_id, sr_data_cellGet.rx_cell.source_chip_id,
                       "get another sr_data_cell->source_chip_id than was set: %d", device_id);
        }
        
        /*
            1.3. Call with sr_data_cell->fe3_link [32] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        sr_data_cell.fe3_link = 32;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sr_data_cell->fe3_link = %d",
                                         device_id, sr_data_cell.fe3_link);

        sr_data_cell.fe3_link = 0;

        /*
            1.4. Call with sr_data_cell->fe2_link [64] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        sr_data_cell.fe2_link = 64;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sr_data_cell->fe2_link = %d",
                                         device_id, sr_data_cell.fe2_link);

        sr_data_cell.fe2_link = 0;

        /*
            1.5. Call with sr_data_cell->fe1_link [32] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        sr_data_cell.fe1_link = 32;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sr_data_cell->fe1_link = %d",
                                         device_id, sr_data_cell.fe1_link);

        sr_data_cell.fe1_link = 0;

        /*
            1.6. Call with sr_data_cell->destination_entity_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        sr_data_cell.destination_entity_type = SAND_UTF_CELL_INVALID_ENUM;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sr_data_cell->destination_entity_type = %d",
                                         device_id, sr_data_cell.destination_entity_type);

        sr_data_cell.destination_entity_type = SAND_FOP_ENTITY;

        /*
            1.7. Call with sr_data_cell->source_entity_type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        sr_data_cell.source_entity_type = SAND_UTF_CELL_INVALID_ENUM;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sr_data_cell->source_entity_type = %d",
                                         device_id, sr_data_cell.source_entity_type);

        sr_data_cell.source_entity_type = SAND_FOP_ENTITY;

        /*
            1.8. Call with sr_data_cell->source_chip_id [2048] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        sr_data_cell.source_chip_id = 2048;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sr_data_cell->source_chip_id = %d",
                                         device_id, sr_data_cell.source_chip_id);

        sr_data_cell.source_chip_id = 0;

        /*
            1.9. Call with sr_data_cell->initial_output_link [64] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        sr_data_cell.initial_output_link = 64;

        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, sr_data_cell->initial_output_link = %d",
                                         device_id, sr_data_cell.initial_output_link);
    }

    for (iter = 0; iter < SAND_SR_DATA_NOF_DATA_LONGS; iter++)
    {
        sr_data_cell.body_tx[iter] = 0;
    }

    sr_data_cell.fe3_link = 0;
    sr_data_cell.fe2_link = 0;
    sr_data_cell.fe1_link = 0;

    sr_data_cell.destination_entity_type = SAND_FE1_ENTITY;
    sr_data_cell.source_entity_type = SAND_FE1_ENTITY;
    sr_data_cell.source_chip_id = 0;
    sr_data_cell.initial_output_link = 0;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_recv_sr_data_cell(
    unsigned int            device_id,
    FAP20M_RX_SR_DATA_BUFFER* sr_data_cell_buff
  )
*/
UTF_TEST_CASE_MAC(fap20m_recv_sr_data_cell)
{
/*
    ITERATE_DEVICES
    1.1. Call with not NULL sr_data_cell_buff.
    Expected: SAND_OK.
    1.2. Call with sr_data_cell_buff [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_RX_SR_DATA_BUFFER sr_data_cell_buff;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with not NULL sr_data_cell_buff.
            Expected: SAND_OK.
        */
        result = fap20m_recv_sr_data_cell(device_id, &sr_data_cell_buff);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with sr_data_cell_buff [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_recv_sr_data_cell(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sr_data_cell_buff = NULL", device_id);
    }

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_recv_sr_data_cell(device_id, &sr_data_cell_buff);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_recv_sr_data_cell(device_id, &sr_data_cell_buff);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_send_control_cell(
    SAND_IN unsigned int       device_id,
    SAND_IN SAND_CONTROL_CELL* control_cell
  )
*/
UTF_TEST_CASE_MAC(fap20m_send_control_cell)
{
/*
    ITERATE_DEVICES
    1.1. Call with control_cell {type [SAND_CONTROL_CELL_TYPE_FLOW_STATUS],
                                 dest_id [0 / 2047],
                                 source_id [0 / 2047],
                                 flow_status_info {sched_flow_id [0 / 16383],
                                                   input_q_number [0 / 16383],
                                                   flow_status [SAND_SCHEDULER_FLOW_STATUS_OFF /
                                                                SAND_SCHEDULER_FLOW_STATUS_ON]}
    Expected: SAND_OK.
    1.2. Call with control_cell->type [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with control_cell->u.flow_status_info.flow_status [0x5AAAAAA5] (out of range) and other param from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32 result = 0;
    SAND_RET rc = SAND_OK;

    SAND_UINT device_id = 0;
    
    SAND_CONTROL_CELL control_cell;
    SAND_CONTROL_CELL control_cellGet = {0};


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with control_cell {type [SAND_CONTROL_CELL_TYPE_FLOW_STATUS],
                                         dest_id [0 / 2047],
                                         source_id [0 / 2047],
                                         flow_status_info {sched_flow_id [0 / 16383],
                                                           input_q_number [0 / 16383],
                                                           flow_status [SAND_SCHEDULER_FLOW_STATUS_OFF /
                                                                        SAND_SCHEDULER_FLOW_STATUS_ON]}
            Expected: SAND_OK.
        */
        control_cell.type = SAND_CONTROL_CELL_TYPE_FLOW_STATUS;

        /* iterate with dest_id = 0 */
        control_cell.dest_id = 0;
        control_cell.source_id = 0;

        control_cell.u.flow_status_info.sched_flow_id = 0;
        control_cell.u.flow_status_info.input_q_number = 0;
        control_cell.u.flow_status_info.flow_status = SAND_SCHEDULER_FLOW_STATUS_OFF;

        result = fap20m_send_control_cell(device_id, &control_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);


        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.dest_id, control_cellGet.dest_id,
                   "get another control_cell->dest_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.source_id, control_cellGet.source_id,
                   "get another control_cell->source_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.u.flow_status_info.sched_flow_id,
                                     control_cellGet.u.flow_status_info.sched_flow_id,
                   "get another control_cell->u.flow_status_info.sched_flow_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.u.flow_status_info.input_q_number,
                                     control_cellGet.u.flow_status_info.input_q_number,
                   "get another control_cell->u.flow_status_info.input_q_number than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.u.flow_status_info.flow_status,
                                     control_cellGet.u.flow_status_info.flow_status,
                   "get another control_cell->u.flow_status_info.flow_status than was set: %d", device_id);

        /* iterate with dest_id = 2047 */
        control_cell.dest_id = 2047;
        control_cell.source_id = 2047;

        control_cell.u.flow_status_info.sched_flow_id = 16383;
        control_cell.u.flow_status_info.input_q_number = 16383;
        control_cell.u.flow_status_info.flow_status = SAND_SCHEDULER_FLOW_STATUS_ON;

        result = fap20m_send_control_cell(device_id, &control_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);


        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.dest_id, control_cellGet.dest_id,
                   "get another control_cell->dest_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.source_id, control_cellGet.source_id,
                   "get another control_cell->source_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.u.flow_status_info.sched_flow_id,
                                     control_cellGet.u.flow_status_info.sched_flow_id,
                   "get another control_cell->u.flow_status_info.sched_flow_id than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.u.flow_status_info.input_q_number,
                                     control_cellGet.u.flow_status_info.input_q_number,
                   "get another control_cell->u.flow_status_info.input_q_number than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(control_cell.u.flow_status_info.flow_status,
                                     control_cellGet.u.flow_status_info.flow_status,
                   "get another control_cell->u.flow_status_info.flow_status than was set: %d", device_id);

        /*
            1.2. Call with control_cell->type [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        control_cell.type = SAND_UTF_CELL_INVALID_ENUM;

        result = fap20m_send_control_cell(device_id, &control_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, control_cell->type = %d",
                                         device_id, control_cell.type);

        control_cell.type = SAND_CONTROL_CELL_TYPE_FLOW_STATUS;

        /*
            1.3. Call with control_cell->u.flow_status_info.flow_status [0x5AAAAAA5] (out of range) and other param from 1.1.
            Expected: NOT SAND_OK.
        */
        control_cell.u.flow_status_info.flow_status = SAND_UTF_CELL_INVALID_ENUM;

        result = fap20m_send_control_cell(device_id, &control_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, control_cell->u.flow_status_info.flow_status = %d",
                                         device_id, control_cell.u.flow_status_info.flow_status);
    }

    control_cell.type = SAND_CONTROL_CELL_TYPE_FLOW_STATUS;

    control_cell.dest_id = 0;
    control_cell.source_id = 0;

    control_cell.u.flow_status_info.sched_flow_id = 0;
    control_cell.u.flow_status_info.input_q_number = 0;
    control_cell.u.flow_status_info.flow_status = SAND_SCHEDULER_FLOW_STATUS_OFF;

    /* 2. For not-active devices               */
    /* check that function returns NOT SAND_OK */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_send_control_cell(device_id, &control_cell);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_send_control_cell(device_id, &control_cell);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_cell suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_cell)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_send_sr_data_cell)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_recv_sr_data_cell)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_send_control_cell)
UTF_SUIT_END_TESTS_MAC(fap20m_api_cell)
