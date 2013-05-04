/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_egress_portsUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_egress_ports that provides
*       diagnostics API of FAP20M device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_scheduler.h>
#include <FX950/include/fap20m_api_egress_ports.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_API_EGRESS_PORTS_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned long 
  fap20m_api_egress_ports_sw_db_load(
    SAND_IN FAP20M_API_EGRESS_PORTS_SW_DB* sw_db_module_buff
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_egress_ports_sw_db_load)
{
/*
    1. Call with sw_db_module_buff { fap20m_egress_port_qos_table_valid [][] [0 / 10] ,
                                       fap20m_egress_port_qos_table[][] { ports[] { valid_mask [0 / 0xFFFFFFFF],
                                                                                    nominal_bandwidth [0 / 10],
                                                                                    credit_bandwidth [0 / 10] } }
                                       fap20m_port_nominal_rate[i][j][k] [0 / 10] }
    Expected: SAND_OK.
    2. Call fap20m_api_egress_ports_sw_db_save with not NULL sw_db_module_buff_ptr.
    Expected: SAND_OK and the same params from 1.
    3. Call with sw_db_module_buff [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  i, j, k;
    FAP20M_API_EGRESS_PORTS_SW_DB sw_db_module_buff;
    FAP20M_API_EGRESS_PORTS_SW_DB sw_db_module_buffGet;


    /*
        1. Call with sw_db_module_buff {   fap20m_egress_port_qos_table_valid [][] [0 / 10] ,
                                           fap20m_egress_port_qos_table[][] { ports[] { valid_mask [0 / 0xFFFFFFFF],
                                                                                        nominal_bandwidth [0 / 10],
                                                                                        credit_bandwidth [0 / 10] } }
                                           fap20m_port_nominal_rate[][][] [0 / 10] }
        Expected: SAND_OK.
    */
    /* iterate with 0 */
    for(i = 0; i < SAND_MAX_DEVICE; i++)
        for(j = 0; j < FAP20M_SCH_RANK_LAST; j++)
        {
            sw_db_module_buff.fap20m_egress_port_qos_table_valid[i][j] = 0;

            for(k = 0; k < FAP20M_NOF_DATA_PORTS; k++)
            {
                sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].valid_mask = 0;
                sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].nominal_bandwidth = 0;
                sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].credit_bandwidth = 0;
            }

            for(k = 0; k < FAP20M_NOF_SCH_PORTS; k++)
            {
                sw_db_module_buff.fap20m_port_nominal_rate[i][j][k] = 0;
            }
        }

    result = fap20m_api_egress_ports_sw_db_load( &sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call fap20m_api_egress_ports_sw_db_save with not NULL sw_db_module_buff_ptr.
        Expected: SAND_OK and the same params from 1.
    */
    result = fap20m_api_egress_ports_sw_db_save( &sw_db_module_buffGet);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_OK, rc, "fap20m_api_egress_ports_sw_db_save");

    /* verifying values */
    for(i = 0; i < SAND_MAX_DEVICE; i++)
        for(j = 0; j < FAP20M_SCH_RANK_LAST; j++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(sw_db_module_buff.fap20m_egress_port_qos_table_valid[i][j], 
                                         sw_db_module_buffGet.fap20m_egress_port_qos_table_valid[i][j], 
                                         "got another sw_db_module_buff.fap20m_egress_port_qos_table_valid[%d][%d] than was set", i, j);

            for(k = 0; k < FAP20M_NOF_DATA_PORTS; k++)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].valid_mask, 
                                             sw_db_module_buffGet.fap20m_egress_port_qos_table[i][j].ports[k].valid_mask, 
                                             "got another sw_db_module_buff.fap20m_egress_port_qos_table[%d][%d].ports[%d].valid_mask than was set", i, j, k);
                UTF_VERIFY_EQUAL3_STRING_MAC(sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].nominal_bandwidth, 
                                             sw_db_module_buffGet.fap20m_egress_port_qos_table[i][j].ports[k].nominal_bandwidth, 
                                             "got another sw_db_module_buff.fap20m_egress_port_qos_table[%d][%d].ports[%d].nominal_bandwidth than was set", i, j, k);
                UTF_VERIFY_EQUAL3_STRING_MAC(sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].credit_bandwidth, 
                                             sw_db_module_buffGet.fap20m_egress_port_qos_table[i][j].ports[k].credit_bandwidth, 
                                             "got another sw_db_module_buff.fap20m_egress_port_qos_table[%d][%d].ports[%d].credit_bandwidth than was set", i, j, k);
            }

            for(k = 0; k < FAP20M_NOF_SCH_PORTS; k++)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(sw_db_module_buff.fap20m_port_nominal_rate[i][j][k], 
                                             sw_db_module_buffGet.fap20m_port_nominal_rate[i][j][k], 
                                             "got another sw_db_module_buff.fap20m_port_nominal_rate[%d][%d][%d] than was set", i, j, k);
            }
        }

        /* iterate with 10 */
        for(i = 0; i < SAND_MAX_DEVICE; i++)
            for(j = 0; j < FAP20M_SCH_RANK_LAST; j++)
            {
                sw_db_module_buff.fap20m_egress_port_qos_table_valid[i][j] = 10;

                for(k=0; k < FAP20M_NOF_DATA_PORTS; k++)
                {
                    sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].valid_mask = 0xFFFFFFFF;
                    sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].nominal_bandwidth = 10;
                    sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].credit_bandwidth = 10;
                }

                for(k = 0; k < FAP20M_NOF_SCH_PORTS; k++)
                {
                    sw_db_module_buff.fap20m_port_nominal_rate[i][j][k] = 10;
                }
            }

        result = fap20m_api_egress_ports_sw_db_load( &sw_db_module_buff);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

        /*
            2. Call fap20m_api_egress_ports_sw_db_save with not NULL sw_db_module_buff_ptr.
            Expected: SAND_OK and the same params from 1.
        */
        result = fap20m_api_egress_ports_sw_db_save( &sw_db_module_buffGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL0_STRING_MAC(SAND_OK, rc, "fap20m_api_egress_ports_sw_db_save");

        /* verifying values */
        for(i = 0; i < SAND_MAX_DEVICE; i++)
            for(j = 0; j < FAP20M_SCH_RANK_LAST; j++)
            {

                UTF_VERIFY_EQUAL2_STRING_MAC(sw_db_module_buff.fap20m_egress_port_qos_table_valid[i][j], 
                                             sw_db_module_buffGet.fap20m_egress_port_qos_table_valid[i][j], 
                                             "got another sw_db_module_buff.fap20m_egress_port_qos_table_valid[%d][%d] than was set", i, j);

                for(k = 0; k < FAP20M_NOF_DATA_PORTS; k++)
                {
                    UTF_VERIFY_EQUAL3_STRING_MAC(sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].valid_mask, 
                                                 sw_db_module_buffGet.fap20m_egress_port_qos_table[i][j].ports[k].valid_mask, 
                                                 "got another sw_db_module_buff.fap20m_egress_port_qos_table[%d][%d].ports[%d].valid_mask than was set", i, j, k);
                    UTF_VERIFY_EQUAL3_STRING_MAC(sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].nominal_bandwidth, 
                                                 sw_db_module_buffGet.fap20m_egress_port_qos_table[i][j].ports[k].nominal_bandwidth, 
                                                 "got another sw_db_module_buff.fap20m_egress_port_qos_table[%d][%d].ports[%d].nominal_bandwidth than was set", i, j, k);
                    UTF_VERIFY_EQUAL3_STRING_MAC(sw_db_module_buff.fap20m_egress_port_qos_table[i][j].ports[k].credit_bandwidth, 
                                                 sw_db_module_buffGet.fap20m_egress_port_qos_table[i][j].ports[k].credit_bandwidth, 
                                                 "got another sw_db_module_buff.fap20m_egress_port_qos_table[%d][%d].ports[%d].credit_bandwidth than was set", i, j, k);
                }

                for(k = 0; k < FAP20M_NOF_SCH_PORTS; k++)
                {
                    UTF_VERIFY_EQUAL3_STRING_MAC(sw_db_module_buff.fap20m_port_nominal_rate[i][j][k], 
                                                 sw_db_module_buffGet.fap20m_port_nominal_rate[i][j][k], 
                                                 "got another sw_db_module_buff.fap20m_port_nominal_rate[%d][%d][%d] than was set", i, j, k);
                }
            }

    /*
        3. Call with sw_db_module_buff [NULL]. 
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_egress_ports_sw_db_load(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sw_db_module_buff = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned long 
  fap20m_api_egress_ports_sw_db_save(
    SAND_OUT FAP20M_API_EGRESS_PORTS_SW_DB* sw_db_module_buff
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_egress_ports_sw_db_save)
{
/*
    1. Call with non-null sw_db_module_buff. 
    Expected: SAND_OK.
    2. Call with sw_db_module_buff [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;


    FAP20M_API_EGRESS_PORTS_SW_DB sw_db_module_buff;

    /*
        1. Call with non-null sw_db_module_buff. 
        Expected: SAND_OK.
    */
    result = fap20m_api_egress_ports_sw_db_save( &sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call with sw_db_module_buff [NULL]. 
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_egress_ports_sw_db_save(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sw_db_module_buff = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_port_desc_init(
      unsigned int device_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_port_desc_init)
{
/*
    ITERATE_DEVICES
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        result = fap20m_port_desc_init(device_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_port_desc_init(device_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_port_desc_init(device_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned short
  fap20m_port_desc_get_nominal_rate(
    SAND_IN   unsigned int      device_id,
    SAND_IN   FAP20M_SCH_RANK   sch_rank,
    SAND_IN   unsigned int      scheduler_port_id,
    SAND_OUT  unsigned long*    nominal_rate
  )
*/
UTF_TEST_CASE_MAC(fap20m_port_desc_get_nominal_rate)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                   scheduler_port_id [0 / 255], 
                   not NULL nominal_rate.
    Expected: SAND_OK.
    1.2. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call scheduler_port_id [FAP20M_NOF_SCH_PORTS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with nominal_rate [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT scheduler_port_id = 0;
    SAND_U32 nominal_rate = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                           scheduler_port_id [0 / 255], 
                           not NULL nominal_rate.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        scheduler_port_id = 0;

        result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, scheduler_port_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        scheduler_port_id = 255;

        result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, scheduler_port_id);

        /*
            1.2. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_EGRESS_PORTS_INVALID_ENUM;

        result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rate);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.3. Call scheduler_port_id [FAP20M_NOF_SCH_PORTS] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler_port_id = FAP20M_NOF_SCH_PORTS;

        result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler_port_id = %d", device_id, scheduler_port_id);

        scheduler_port_id = 0;

        /*
            1.4. Call with nominal_rate [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, nominal_rate = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;
    scheduler_port_id = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rate);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned short
  fap20m_port_desc_set_nominal_rate(
    SAND_IN   unsigned int      device_id,
    SAND_IN   FAP20M_SCH_RANK   sch_rank,
    SAND_IN   unsigned int      scheduler_port_id,
    SAND_IN   unsigned int      nominal_rate
  )
*/
UTF_TEST_CASE_MAC(fap20m_port_desc_set_nominal_rate)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                   scheduler_port_id [0 / 255], 
                   nominal_rate [0 / 255].
    Expected: SAND_OK.
    1.2. Call fap20m_port_desc_get_nominal_rate with not NULL nominal_rate_ptr and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call scheduler_port_id [FAP20M_NOF_SCH_PORTS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK   sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT  scheduler_port_id = 0;
    SAND_UINT  nominal_rate = 0;
    SAND_U32   nominal_rateGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                           scheduler_port_id [0 / 255], 
                           nominal_rate [0 / 255].
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;
        scheduler_port_id = 0;
        nominal_rate = 0;

        result = fap20m_port_desc_set_nominal_rate(device_id, sch_rank, scheduler_port_id, nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, scheduler_port_id, nominal_rate);

        /*
            1.2. Call fap20m_port_desc_get_nominal_rate with not NULL nominal_rate_ptr and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rateGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_port_desc_get_nominal_rate: %d, %d, %d",
                                     device_id, sch_rank, scheduler_port_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(nominal_rate, nominal_rateGet, "got another nominal_rate than was set: %d", device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_SCND;
        scheduler_port_id = 255;
        nominal_rate = 255;

        result = fap20m_port_desc_set_nominal_rate(device_id, sch_rank, scheduler_port_id, nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, scheduler_port_id, nominal_rate);

        /*
            1.2. Call fap20m_port_desc_get_nominal_rate with not NULL nominal_rate_ptr and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rateGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_port_desc_get_nominal_rate: %d, %d, %d", device_id, sch_rank, scheduler_port_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(nominal_rate, nominal_rateGet, "got another nominal_rate than was set: %d", device_id);

        /*
            1.3. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_EGRESS_PORTS_INVALID_ENUM;

        result = fap20m_port_desc_set_nominal_rate(device_id, sch_rank, scheduler_port_id, nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_LAST;

        /*
            1.4. Call scheduler_port_id [FAP20M_NOF_SCH_PORTS] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        scheduler_port_id = FAP20M_NOF_SCH_PORTS;

        result = fap20m_port_desc_set_nominal_rate(device_id, sch_rank, scheduler_port_id, nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, scheduler_port_id = %d", device_id, scheduler_port_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;
    scheduler_port_id = 0;
    nominal_rate = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_port_desc_set_nominal_rate(device_id, sch_rank, scheduler_port_id, nominal_rate);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_port_desc_set_nominal_rate(device_id, sch_rank, scheduler_port_id, nominal_rate);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_egress_ports_qos(
    SAND_IN     unsigned int                   device_id,
    SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_IN     FAP20M_EGRESS_PORT_QOS_TABLE   *ports_qos,
    SAND_OUT    FAP20M_EGRESS_PORT_QOS_TABLE   *exact_ports_qos
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_egress_ports_qos)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                   ports_qos[] { valid_mask [0 / 0xFFFFFF],
                                 nominal_bandwidth [0 / 255],
                                 credit_bandwidth [0 / 255] },
                   non-null exact_ports_qos.
    Expected: SAND_OK.
    1.2. Call fap20m_get_egress_ports_qos with not NULL ports_qos_ptr and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with ports_qos [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.5. Call with exact_ports_qos [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  index = 0;
    FAP20M_SCH_RANK              sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_EGRESS_PORT_QOS_TABLE ports_qos;
    FAP20M_EGRESS_PORT_QOS_TABLE ports_qosGet;
    FAP20M_EGRESS_PORT_QOS_TABLE exact_ports_qos;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                           ports_qos[] { valid_mask [0 / 0xFFFFFF],
                                         nominal_bandwidth [0 / 255],
                                         credit_bandwidth [0 / 255] },
                           non-null exact_ports_qos.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
        {
            ports_qos.ports[index].valid_mask = 0;
            ports_qos.ports[index].nominal_bandwidth = 0;
            ports_qos.ports[index].credit_bandwidth  = 0;
        }

        result = fap20m_set_egress_ports_qos(device_id, sch_rank, &ports_qos, &exact_ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_egress_ports_qos with not NULL ports_qos_ptr and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */

        for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
        {
            ports_qosGet.ports[index].valid_mask = 0;
        }
        result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qosGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_egress_ports_qos: %d, %d", device_id, sch_rank);

        /* verifying values */
        for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
        {
            /* compare only zero valid_mask value. Other params are meaningless */
            UTF_VERIFY_EQUAL2_STRING_MAC(0, 
                                         ports_qosGet.ports[index].valid_mask, 
                                         "got another ports_qos.ports[%d].valid_mask than was set: %d", index, device_id);
        }

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
        {
            ports_qos.ports[index].valid_mask = 0xFFFFFFFF;
            ports_qos.ports[index].nominal_bandwidth = 255;
            ports_qos.ports[index].credit_bandwidth  = 255;
        }

        result = fap20m_set_egress_ports_qos(device_id, sch_rank, &ports_qos, &exact_ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call fap20m_get_egress_ports_qos with not NULL ports_qos_ptr and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */

        for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
        {
            ports_qosGet.ports[index].valid_mask = FAP20M_EGRESS_PORT_QOS_MASK_ALL;
        }
        result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qosGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_egress_ports_qos: %d, %d", device_id, sch_rank);

        /* verifying values */
        for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(FAP20M_EGRESS_PORT_QOS_MASK_ALL, 
                                         ports_qosGet.ports[index].valid_mask, 
                                         "got another ports_qos.ports[%d].valid_mask than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_ports_qos.ports[index].nominal_bandwidth, 
                                         ports_qosGet.ports[index].nominal_bandwidth, 
                                         "got another ports_qos.ports[%d].nominal_bandwidth than was set: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(exact_ports_qos.ports[index].credit_bandwidth, 
                                         ports_qosGet.ports[index].credit_bandwidth, 
                                         "got another ports_qos.ports[%d].credit_bandwidth than was set: %d", index, device_id);
        }

        /*
            1.3. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_EGRESS_PORTS_INVALID_ENUM;

        result = fap20m_set_egress_ports_qos(device_id, sch_rank, &ports_qos, &exact_ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call with ports_qos [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_egress_ports_qos(device_id, sch_rank, NULL, &exact_ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ports_qos = NULL", device_id);

        /*
            1.5. Call with exact_ports_qos [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_egress_ports_qos(device_id, sch_rank, &ports_qos, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_ports_qos = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
    {
        ports_qos.ports[index].valid_mask = 0;
        ports_qos.ports[index].nominal_bandwidth = 0;
        ports_qos.ports[index].credit_bandwidth  = 0;
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_egress_ports_qos(device_id, sch_rank, &ports_qos, &exact_ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_egress_ports_qos(device_id, sch_rank, &ports_qos, &exact_ports_qos);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_one_egress_port_qos(
    SAND_IN     unsigned int               device_id,
    SAND_IN     FAP20M_SCH_RANK            sch_rank,
    SAND_IN     unsigned int               port_id,
    SAND_IN     FAP20M_EGRESS_PORT_QOS   *port_qos,
    SAND_OUT    FAP20M_EGRESS_PORT_QOS   *exact_port_qos
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_one_egress_port_qos)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                   port_id [0 / 255],
                   port_qos { valid_mask [0 / 0xFFFFFF],
                              nominal_bandwidth [0 / 255],
                              credit_bandwidth [0 / 255] },
                   non-null exact_port_qos.
    Expected: SAND_OK.
    1.2. Call fap20m_get_egress_ports_qos with not NULL ports_qos_ptr and other params from 1.1.
    Expected: SAND_OK and the same ports_qos.ports[ port_id ] as was set.
    1.3. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call port_id [FAP20M_NOF_SCH_PORTS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with port_qos [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.6. Call with exact_port_qos [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT              device_id = 0;
    SAND_UINT              index = 0;
    FAP20M_SCH_RANK        sch_rank = FAP20M_SCH_RANK_PRIM;
    SAND_UINT              port_id = 0;
    FAP20M_EGRESS_PORT_QOS port_qos;
    FAP20M_EGRESS_PORT_QOS_TABLE ports_qosGet;
    FAP20M_EGRESS_PORT_QOS exact_port_qos;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND], 
                           port_id [0 / 255],
                           ports_qos[] { valid_mask [0 / 0xFFFFFF],
                                         nominal_bandwidth [0 / 255],
                                         credit_bandwidth [0 / 255] },
                           non-null exact_ports_qos.
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        port_id = 0;

        port_qos.valid_mask = 0;
        port_qos.nominal_bandwidth = 0;
        port_qos.credit_bandwidth  = 0;

        result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, &port_qos, &exact_port_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port_id);

        /*
            1.2. Call fap20m_get_egress_ports_qos with not NULL ports_qos_ptr and other params from 1.1.
            Expected: SAND_OK and the same ports_qos.ports[ port_id ] as was set.
        */
        for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
        {
            ports_qosGet.ports[index].valid_mask = 0;
        }

        result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qosGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_egress_ports_qos: %d, %d", device_id, sch_rank);

        /* verifying values */
        /* compare only valid_mask with FAP20M_EGRESS_PORT_QOS_MASK_ALL value. 
          Other params are set in previouse test and should not be checked 
          in this one */
        UTF_VERIFY_EQUAL2_STRING_MAC(FAP20M_EGRESS_PORT_QOS_MASK_ALL, 
                                     ports_qosGet.ports[port_id].valid_mask, 
                                     "got another ports_qos.ports[%d].valid_mask than was set: %d", port_id, device_id);

        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        port_id = 255;

        port_qos.valid_mask = 0xFFFFFFFF;
        port_qos.nominal_bandwidth = 255;
        port_qos.credit_bandwidth  = 255;

        result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, &port_qos, &exact_port_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, sch_rank, port_id);

        /*
            1.2. Call fap20m_get_egress_ports_qos with not NULL ports_qos_ptr and other params from 1.1.
            Expected: SAND_OK and the same ports_qos.ports[ port_id ] as was set.
        */
        for(index = 0; index < FAP20M_NOF_DATA_PORTS; index++)
        {
            ports_qosGet.ports[index].valid_mask = FAP20M_EGRESS_PORT_QOS_MASK_ALL;
        }
        result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qosGet);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_egress_ports_qos: %d, %d", device_id, sch_rank);

        /* verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(FAP20M_EGRESS_PORT_QOS_MASK_ALL, 
                                     ports_qosGet.ports[port_id].valid_mask, 
                                     "got another ports_qos.ports[%d].valid_mask than was set: %d", port_id, device_id);
        UTF_VERIFY_EQUAL2_STRING_MAC(exact_port_qos.nominal_bandwidth, 
                                     ports_qosGet.ports[port_id].nominal_bandwidth, 
                                     "got another ports_qos.ports[%d].nominal_bandwidth than was set: %d", port_id, device_id);
        UTF_VERIFY_EQUAL2_STRING_MAC(exact_port_qos.credit_bandwidth, 
                                     ports_qosGet.ports[port_id].credit_bandwidth, 
                                     "got another ports_qos.ports[%d].credit_bandwidth than was set: %d", port_id, device_id);

        /*
            1.3. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_EGRESS_PORTS_INVALID_ENUM;

        result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, &port_qos, &exact_port_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_SCND;

        /*
            1.4. Call port_id [FAP20M_NOF_SCH_PORTS] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        port_id = FAP20M_NOF_SCH_PORTS;

        result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, &port_qos, &exact_port_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, port_id = %d", device_id, port_id);

        port_id = 0;

        /*
            1.5. Call with ports_qos [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, NULL, &exact_port_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, port_qos = NULL", device_id);

        /*
            1.6. Call with exact_ports_qos [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, &port_qos, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_port_qos = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    port_id = 0;

    port_qos.valid_mask = 0;
    port_qos.nominal_bandwidth = 0;
    port_qos.credit_bandwidth  = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, &port_qos, &exact_port_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, &port_qos, &exact_port_qos);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_egress_ports_qos(
    SAND_IN     unsigned int                     device_id,
  SAND_IN     FAP20M_SCH_RANK                sch_rank,
    SAND_INOUT  FAP20M_EGRESS_PORT_QOS_TABLE   *ports_qos
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_egress_ports_qos)
{
/*
    ITERATE_DEVICES
    1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                   non-null ports_qos. 
    Expected: SAND_OK.
    1.2. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with ports_qos [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SCH_RANK sch_rank = FAP20M_SCH_RANK_PRIM;
    FAP20M_EGRESS_PORT_QOS_TABLE ports_qos;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND],
                           non-null ports_qos. 
            Expected: SAND_OK.
        */
        /* iterate with sch_rank = FAP20M_SCH_RANK_PRIM */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /* iterate with sch_rank = FAP20M_SCH_RANK_SCND */
        sch_rank = FAP20M_SCH_RANK_PRIM;

        result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        /*
            1.2. Call sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        sch_rank = SAND_UTF_API_EGRESS_PORTS_INVALID_ENUM;

        result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, sch_rank);

        sch_rank = FAP20M_SCH_RANK_LAST;

        /*
            1.3. Call with ports_qos [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_egress_ports_qos(device_id, sch_rank, NULL);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, ports_qos = NULL", device_id);
    }

    sch_rank = FAP20M_SCH_RANK_PRIM;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qos);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qos);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_egress_ports suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_egress_ports)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_egress_ports_sw_db_load)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_egress_ports_sw_db_save)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_port_desc_init)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_port_desc_get_nominal_rate)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_port_desc_set_nominal_rate)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_egress_ports_qos)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_one_egress_port_qos)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_egress_ports_qos)
UTF_SUIT_END_TESTS_MAC(fap20m_api_egress_ports)

