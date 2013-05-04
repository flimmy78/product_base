/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_auto_managementUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_auto_management, that provides
*       all the API funcitons to the FAP20M_B auto management module
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_auto_management.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>

/* defines */

/* Invalid enum */
#define SAND_UTF_API_AUTO_INVALID_ENUM    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_system_info_get_defaults(
    SAND_OUT FAP20M_AUTO_SYSTEM_INFO *p_auto_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_system_info_get_defaults)
{
/*
    1. Call with non-null p_auto_info.
    Expected: SAND_OK.
    2. Call with p_auto_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_AUTO_SYSTEM_INFO p_auto_info;

    /*
        1. Call with non-null p_auto_info.
        Expected: SAND_OK.
    */
    result = fap20m_api_auto_system_info_get_defaults(&p_auto_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call with p_auto_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_system_info_get_defaults(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_auto_info = NULL");
}

/*----------------------------------------------------------------------------
unsigned int
  fap20m_api_auto_init(
    SAND_IN FAP20M_AUTO_SYSTEM_INFO *p_auto_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_init)
{
/*
    1. Call with p_auto_info { max_nof_faps_in_system [0 / 5 / 10],
                               max_nof_ports_per_fap [0 / 5 / 10],
                               is_fap10v [FALSE],
                               max_nof_sub_flows_per_flow [1 / 2],
                               nof_fabric_classes [FAP20M_FABRIC_UNICAST_CLASSES / FAP20M_MAX_FABRIC_UNICAST / FAP20M_FABRIC_MULTICAST_CLASSES],
                               auto_first_queue_type [FAP20M_QUEUE_0 / FAP20M_QUEUE_1 / FAP20M_QUEUE_2],
                               auto_last_queue_type [FAP20M_QUEUE_0 / FAP20M_QUEUE_1 / FAP20M_QUEUE_2],
                               nof_faps_per_CPU [0 / 5 / 10],
                               device_info { local_fap_id  [0,..,0 / 1,..,1] } ,
                               fabric_sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND / FAP20M_SCH_RANK_LAST] }
    Expected: SAND_OK.
    2. Call with p_auto_info->is_fap10v [TRUE] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    3. Call with p_auto_info->nof_fabric_classes [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    4. Call with p_auto_info->auto_first_queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    5. Call with p_auto_info->auto_last_queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    6. Call with p_auto_info->fabric_sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    7. Call with p_auto_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_AUTO_SYSTEM_INFO p_auto_info;

/*
    1. Call with p_auto_info { max_nof_faps_in_system [0 / 5 / 10],
                               max_nof_ports_per_fap [0 / 5 / 10],
                               is_fap10v [FALSE] !!! JUST FOR :FAP20M,
                               max_nof_sub_flows_per_flow [1 / 2],
                               nof_fabric_classes [FAP20M_FABRIC_UNICAST_CLASSES / FAP20M_MAX_FABRIC_UNICAST / FAP20M_FABRIC_MULTICAST_CLASSES],
                               auto_first_queue_type [FAP20M_QUEUE_0 / FAP20M_QUEUE_1 / FAP20M_QUEUE_2],
                               auto_last_queue_type [FAP20M_QUEUE_0 / FAP20M_QUEUE_1 / FAP20M_QUEUE_2],
                               nof_faps_per_CPU [0 / 5 / 10],
                               device_info { local_fap_id  [0,..,0 / 1,..,1] } ,
                               fabric_sch_rank [FAP20M_SCH_RANK_PRIM / FAP20M_SCH_RANK_SCND / FAP20M_SCH_RANK_LAST] }
    Expected: SAND_OK.
*/
    /* iterate with max_nof_faps_in_system = 0 */
    p_auto_info.max_nof_faps_in_system = 0;
    p_auto_info.max_nof_ports_per_fap = 0;
    p_auto_info.is_fap10v = FALSE;
    p_auto_info.max_nof_sub_flows_per_flow = 1;
    p_auto_info.nof_fabric_classes = FAP20M_FABRIC_UNICAST_CLASSES;
    p_auto_info.auto_first_queue_type = FAP20M_QUEUE_0;
    p_auto_info.auto_last_queue_type = FAP20M_QUEUE_0;
    p_auto_info.nof_faps_per_CPU = 0;

    p_auto_info.device_info[0].local_fap_id = 0;
    p_auto_info.device_info[1].local_fap_id = 0;
    p_auto_info.device_info[2].local_fap_id = 0;
    p_auto_info.device_info[3].local_fap_id = 0;
    p_auto_info.device_info[4].local_fap_id = 0;
    p_auto_info.device_info[5].local_fap_id = 0;
    p_auto_info.device_info[6].local_fap_id = 0;
    p_auto_info.device_info[7].local_fap_id = 0;

    p_auto_info.fabric_sch_rank = FAP20M_SCH_RANK_PRIM;

    result = fap20m_api_auto_init( &p_auto_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* iterate with max_nof_faps_in_system = 5 */
    p_auto_info.max_nof_faps_in_system = 5;
    p_auto_info.max_nof_ports_per_fap = 5;
    p_auto_info.is_fap10v = FALSE;
    p_auto_info.max_nof_sub_flows_per_flow = 2;
    p_auto_info.nof_fabric_classes = FAP20M_MAX_FABRIC_UNICAST;
    p_auto_info.auto_first_queue_type = FAP20M_QUEUE_1;
    p_auto_info.auto_last_queue_type = FAP20M_QUEUE_1;
    p_auto_info.nof_faps_per_CPU = 5;

    p_auto_info.device_info[0].local_fap_id = 1;
    p_auto_info.device_info[1].local_fap_id = 1;
    p_auto_info.device_info[2].local_fap_id = 1;
    p_auto_info.device_info[3].local_fap_id = 1;
    p_auto_info.device_info[4].local_fap_id = 1;
    p_auto_info.device_info[5].local_fap_id = 1;
    p_auto_info.device_info[6].local_fap_id = 1;
    p_auto_info.device_info[7].local_fap_id = 1;

    p_auto_info.fabric_sch_rank = FAP20M_SCH_RANK_SCND;

    result = fap20m_api_auto_init( &p_auto_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* iterate with max_nof_faps_in_system = 10 */
    p_auto_info.max_nof_faps_in_system = 10;
    p_auto_info.max_nof_ports_per_fap = 10;
    p_auto_info.is_fap10v = FALSE;
    p_auto_info.max_nof_sub_flows_per_flow = 2;
    p_auto_info.nof_fabric_classes = FAP20M_FABRIC_MULTICAST_CLASSES;
    p_auto_info.auto_first_queue_type = FAP20M_QUEUE_2;
    p_auto_info.auto_last_queue_type = FAP20M_QUEUE_2;
    p_auto_info.nof_faps_per_CPU = 10;

    p_auto_info.device_info[0].local_fap_id = 1;
    p_auto_info.device_info[1].local_fap_id = 1;
    p_auto_info.device_info[2].local_fap_id = 1;
    p_auto_info.device_info[3].local_fap_id = 1;
    p_auto_info.device_info[4].local_fap_id = 1;
    p_auto_info.device_info[5].local_fap_id = 1;
    p_auto_info.device_info[6].local_fap_id = 1;
    p_auto_info.device_info[7].local_fap_id = 1;

    p_auto_info.fabric_sch_rank = FAP20M_SCH_RANK_LAST;

    result = fap20m_api_auto_init( &p_auto_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

/*
    2. Call with p_auto_info->is_fap10v [TRUE] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    p_auto_info.is_fap10v = TRUE;

    result = fap20m_api_auto_init( &p_auto_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_auto_info.is_fap10v = %d", p_auto_info.is_fap10v);

    p_auto_info.is_fap10v = FALSE;

/*
    3. Call with p_auto_info->nof_fabric_classes [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    p_auto_info.nof_fabric_classes = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_init( &p_auto_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_auto_info.nof_fabric_classes = %d", p_auto_info.nof_fabric_classes);

    p_auto_info.nof_fabric_classes = FAP20M_FABRIC_UNICAST_CLASSES;

/*
    4. Call with p_auto_info->auto_first_queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    p_auto_info.auto_first_queue_type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_init( &p_auto_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_auto_info.auto_first_queue_type = %d", p_auto_info.auto_first_queue_type);

    p_auto_info.auto_first_queue_type = FAP20M_QUEUE_1;

/*
    5. Call with p_auto_info->auto_last_queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    p_auto_info.auto_last_queue_type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_init( &p_auto_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_auto_info.auto_last_queue_type = %d", p_auto_info.auto_last_queue_type);

    p_auto_info.auto_last_queue_type = FAP20M_QUEUE_1;

/*
    6. Call with p_auto_info->fabric_sch_rank [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    p_auto_info.fabric_sch_rank = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_init( &p_auto_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_auto_info.fabric_sch_rank = %d", p_auto_info.fabric_sch_rank);

    p_auto_info.fabric_sch_rank = FAP20M_SCH_RANK_PRIM;

/*
    7. Call with p_auto_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    result = fap20m_api_auto_init( NULL);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_auto_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_port_get_default(
    SAND_OUT FAP20M_AUTO_PORT_INFO *p_port_info,
    SAND_IN  unsigned long           nominal_rate
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_port_get_default)
{
/*
    1. Call with nominal_rate [0 / 10], non-null p_port_info.
    Expected: SAND_OK.
    2. Call with p_port_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_U32   nominal_rate;
    FAP20M_AUTO_PORT_INFO p_port_info;

    /*
        1. Call with nominal_rate [0 / 10], non-null p_port_info.
        Expected: SAND_OK.
    */
    /* iterate with nominal_rate = 0 */
    nominal_rate = 0;

    result = fap20m_api_auto_port_get_default(&p_port_info, nominal_rate);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, nominal_rate);

    /* iterate with nominal_rate = 10 */
    nominal_rate = 10;

    result = fap20m_api_auto_port_get_default(&p_port_info, nominal_rate);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, nominal_rate);

    /*
        2. Call with p_port_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_port_get_default(NULL, nominal_rate);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_port_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_set_info_and_open_all_ports(
    SAND_IN FAP20M_AUTO_PORT_INFO *p_port_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_set_info_and_open_all_ports)
{
/*
    1. Call with p_port_info {eg_port.valid_mask [0 / 0xFFFFFFFF],
                              eg_port.nominal_bandwidth [0 / 100],
                              eg_port.credit_bandwidth [0 / 100],
                              hr_mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ / FAP20M_HR_SCHED_MODE_DUAL_WFQ] }
    Expected: SAND_OK.
    2. Call with p_port_info->eg_port.nominal_bandwidth [0xFFFFFFFF] (no any constraints)
                 and other params from 1.1.
    ExpecteD: NOT SAND_OK.
    3. Call with p_port_info->eg_port.credit_bandwidth [0xFFFFFFFF] (no any constraints)
                 and other params from 1.1.
    ExpecteD: NOT SAND_OK.
    4. Call with p_port_info->hr_mode [0x5AAAAAA5] (out of range)
                 and other params from 1.1.
    ExpecteD: NOT SAND_OK.
    5. Call with p_port_info [NULL]
                 and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_AUTO_PORT_INFO p_port_info;

    /*
        1. Call with p_port_info {eg_port.valid_mask [0 / 0xFFFFFFFF],
                                  eg_port.nominal_bandwidth [0 / 100],
                                  eg_port.credit_bandwidth [0 / 100],
                                  hr_mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ / FAP20M_HR_SCHED_MODE_DUAL_WFQ] }
        Expected: SAND_OK.
    */
    /* iterate with valid_mask = 0 */
    p_port_info.eg_port.valid_mask        = 0;
    p_port_info.eg_port.nominal_bandwidth = 0;
    p_port_info.eg_port.credit_bandwidth  = 0;
    p_port_info.hr_mode                   = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

    result = fap20m_api_auto_set_info_and_open_all_ports(&p_port_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* iterate with valid_mask = 0xFFFFFFFF */
    p_port_info.eg_port.valid_mask        = 0xFFFFFFFF;
    p_port_info.eg_port.nominal_bandwidth = 100;
    p_port_info.eg_port.credit_bandwidth  = 100;
    p_port_info.hr_mode                   = FAP20M_HR_SCHED_MODE_DUAL_WFQ;

    result = fap20m_api_auto_set_info_and_open_all_ports(&p_port_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call with p_port_info->eg_port.nominal_bandwidth [0xFFFFFFFF] (no any constraints)
                     and other params from 1.1.
        ExpecteD: NOT SAND_OK.
    */
    p_port_info.eg_port.nominal_bandwidth = 0xFFFFFFFF;
    
    result = fap20m_api_auto_set_info_and_open_all_ports(&p_port_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "p_port_info->eg_port.nominal_bandwidth = %d",
                                 p_port_info.eg_port.nominal_bandwidth);

    p_port_info.eg_port.nominal_bandwidth = 0;

    /*
        3. Call with p_port_info->eg_port.credit_bandwidth [0xFFFFFFFF] (no any constraints)
                     and other params from 1.1.
        ExpecteD: NOT SAND_OK.
    */
    p_port_info.eg_port.credit_bandwidth = 0xFFFFFFFF;
    
    result = fap20m_api_auto_set_info_and_open_all_ports(&p_port_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "p_port_info->eg_port.credit_bandwidth = %d",
                                 p_port_info.eg_port.credit_bandwidth);

    p_port_info.eg_port.credit_bandwidth = 0;

    /*
        4. Call with p_port_info->hr_mode [0x5AAAAAA5] (out of range)
                     and other params from 1.1.
        ExpecteD: NOT SAND_OK.
    */
    p_port_info.hr_mode = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_ports(&p_port_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL0_PARAM_MAC(SAND_OK, rc);

    p_port_info.hr_mode = FAP20M_HR_SCHED_MODE_DUAL_WFQ;

    /*
        3. Call with p_port_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_set_info_and_open_all_ports(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_port_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_port_get_info(
    SAND_IN  unsigned int            device_id,
    SAND_IN  unsigned int            port_id,
    SAND_OUT FAP20M_AUTO_PORT_INFO *p_port_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_port_get_info)
{
/*
    ITERATE_DEVICES_PORTS
    1.1.1. Call with non-null p_port_info.
    Expected: SAND_OK.
    1.1.2. Call with p_port_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  port_id = 0;
    FAP20M_AUTO_PORT_INFO p_port_info;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id = 0; port_id < FAP20M_NOF_DATA_PORTS; port_id++)
        {
            /*
                1.1.1. Call with non-null p_port_info.
                Expected: SAND_OK.
            */
            result = fap20m_api_auto_port_get_info(device_id, port_id, &p_port_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /*
                1.1.2. Call with p_port_info [NULL].
                Expected: SAND_NULL_POINTER_ERR.
            */
            result = fap20m_api_auto_port_get_info(device_id, port_id, NULL);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_port_info = NULL");
        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_port_get_info(device_id, port_id, &p_port_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_port_get_info(device_id, port_id, &p_port_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_port_get_info(device_id, port_id, &p_port_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_update_port(
    SAND_IN  unsigned int            device_id,
    SAND_IN  unsigned int            port_id,
    SAND_IN  FAP20M_AUTO_PORT_INFO *p_port_info,
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_update_port)
{
/*
    ITERATE_DEVICES_PORTS
    1.1.1. Call with p_port_info {eg_port.valid_mask [0 / 0xFFFFFFFF],
                                  eg_port.nominal_bandwidth [0 / 100],
                                  eg_port.credit_bandwidth [0 / 100],
                                  hr_mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ /
                                           FAP20M_HR_SCHED_MODE_DUAL_WFQ] }
    Expected: SAND_OK.
    1.1.2. Call with p_port_info->eg_port.nominal_bandwidth [0xFFFFFFFF] (no any constraints)
                 and other params from 1.1.
    ExpecteD: NOT SAND_OK.
    1.1.3. Call with p_port_info->eg_port.credit_bandwidth [0xFFFFFFFF] (no any constraints)
                 and other params from 1.1.
    ExpecteD: NOT SAND_OK.
    1.1.4. CAll with p_port_info->hr_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.5. Call with p_port_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  port_id = 0;

    FAP20M_AUTO_PORT_INFO p_port_info;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id = 0; port_id < FAP20M_NOF_DATA_PORTS; port_id++)
        {
            /*
                1.1.1. Call with p_port_info {eg_port.valid_mask [0 / 0xFFFFFFFF],
                                              eg_port.nominal_bandwidth [0 / 100],
                                              eg_port.credit_bandwidth [0 / 100],
                                              hr_mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ /
                                                       FAP20M_HR_SCHED_MODE_DUAL_WFQ] }
                               non-null exact_port_info.
                Expected: SAND_OK.
            */
            /* iterate with valid_mask = 0 */
            p_port_info.eg_port.valid_mask = 0;
            p_port_info.eg_port.nominal_bandwidth = 0;
            p_port_info.eg_port.credit_bandwidth = 0;
            p_port_info.hr_mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

            result = fap20m_api_auto_update_port(device_id, port_id, &p_port_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /* iterate with valid_mask = 0xFFFFFFFF */
            p_port_info.eg_port.valid_mask = 0xFFFFFFFF;
            p_port_info.eg_port.nominal_bandwidth = 100;
            p_port_info.eg_port.credit_bandwidth = 100;
            p_port_info.hr_mode = FAP20M_HR_SCHED_MODE_DUAL_WFQ;

            result = fap20m_api_auto_update_port(device_id, port_id, &p_port_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /*
                1.1.2. Call with p_port_info->eg_port.nominal_bandwidth [0xFFFFFFFF] (no any constraints)
                             and other params from 1.1.
                ExpecteD: NOT SAND_OK.
            */
            p_port_info.eg_port.nominal_bandwidth = 0xFFFFFFFF;
            
            result = fap20m_api_auto_update_port(device_id, port_id, &p_port_info);
        
            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_port_info->eg_port.nominal_bandwidth = %d",
                                         device_id, port_id, p_port_info.eg_port.nominal_bandwidth);
        
            p_port_info.eg_port.nominal_bandwidth = 0;

            /*
                1.1.3. Call with p_port_info->eg_port.credit_bandwidth [0xFFFFFFFF] (no any constraints)
                             and other params from 1.1.
                ExpecteD: NOT SAND_OK.
            */
            p_port_info.eg_port.credit_bandwidth = 0xFFFFFFFF;
            
            result = fap20m_api_auto_update_port(device_id, port_id, &p_port_info);
        
            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_port_info->eg_port.credit_bandwidth = %d",
                                         device_id, port_id, p_port_info.eg_port.nominal_bandwidth);
        
            p_port_info.eg_port.credit_bandwidth = 0;

            /*
                1.1.4. CAll with p_port_info->hr_mode [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_port_info.hr_mode = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port(device_id, port_id, &p_port_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_port_info.hr_mode = %d",
                                             device_id, port_id, p_port_info.hr_mode);

            p_port_info.hr_mode = FAP20M_HR_SCHED_MODE_DUAL_WFQ;

            /*
                1.1.5. Call with p_port_info [NULL].
                Expected: SAND_NULL_POINTER_ERR.
            */
            result = fap20m_api_auto_update_port(device_id, port_id, NULL);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_port_info = NULL");

        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_port_get_info(device_id, port_id, &p_port_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_port_get_info(device_id, port_id, &p_port_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_port_get_info(device_id, port_id, &p_port_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_close_port(
    SAND_IN  unsigned int            device_id,
    SAND_IN  unsigned int            port_id
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_close_port)
{
/*
    ITERATE_DEVICES_PORTS
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  port_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id = 0; port_id < FAP20M_NOF_DATA_PORTS; port_id++)
        {
            result = fap20m_api_auto_close_port(device_id, port_id);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_close_port(device_id, port_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_close_port(device_id, port_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_close_port(device_id, port_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_aggregates_get_default(
    SAND_OUT FAP20M_AUTO_AGG_INFO      *p_aggregates_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_aggregates_get_default)
{
/*
    1. Call with non-null p_aggregates_info.
    Expected: SAND_OK.
    2. Call with p_aggregates_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_AUTO_AGG_INFO p_aggregates_info;


    /*
        1. Call with non-null countValuePtr.
        Expected: SAND_OK.
    */
    result = fap20m_api_auto_aggregates_get_default(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call with p_aggregates_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_aggregates_get_default(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_aggregates_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_set_info_and_open_all_aggregates(
    SAND_IN  FAP20M_AUTO_AGG_INFO *p_aggregates_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_set_info_and_open_all_aggregates)
{
/*
    1. Call with p_aggregates_info {item {sch {id [0 / 10],
                                               type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR /
                                                     FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL],
                                               state [FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE /
                                                      FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE],
                                               sched_properties {hr.mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ /
                                                                          FAP20M_HR_SCHED_MODE_DUAL_WFQ] },
                                                                 cl.class_type [0 / 31] } } }

                                               flow {id [64 / 16K],
                                                     sub_flow[FAP20M_NOF_SUB_FLOWS] {valid_mask [0 / 0xFFFFFFFF],
                                                                                     id [64 / 16K],
                                                                                     credit_source [0 / 10],
                                                                                     shaper {max_rate [0 / 10],
                                                                                             max_burst [1 / 10] }
                                                                                     type [FAP20M_SUB_FLOW_TYPE_HR /
                                                                                           FAP20M_SUB_FLOW_TYPE_CL]

                                                                                     subflow_properties
                                                                                       { hr { sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF1 / FAP20M_SUB_FLOW_HR_ENHANCED_CLASS_AF3],
                                                                                              weight [1 / 0xFFF] }
                                                                                         cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ / FAP20M_SUB_FLOW_CL_CLASS_SP1],
                                                                                              weight [1 / 64] }  }
                                                                                      }
                                               flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW]
                                               is_slow_enabled [TRUE / FALSE] }
                                               valid [TRUE / FALSE] } },
    Expected: SAND_OK.
    2. Call p_aggregates_info->item.sch.type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    3. Call p_aggregates_info->item.sch.state [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    4. Call p_aggregates_info->item.sch.type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR],
              p_aggregates_info->item.sch.sched_properties.hr.mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    5. Call p_aggregates_info->item.sch.type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL],
              p_aggregates_info->item.sch.sched_properties.cl.class_type [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    6. Call p_aggregates_info->item.flow.sub_flow[0].type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    7. Call p_aggregates_info->item.flow.sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_HR],
              p_aggregates_info->item.flow.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    8. Call p_aggregates_info->item.flow.sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_CL],
              p_aggregates_info->item.flow.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    9. Call p_aggregates_info->item.flow.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    10. Call with countValuePtr [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;
    
    FAP20M_AUTO_AGG_INFO p_aggregates_info;
    SAND_UINT  i, j, k;


    /*
        1. Call with p_aggregates_info {item {sch {id [0 / 10],
                                                   type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR /
                                                         FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL],
                                                   state [FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE /
                                                          FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE],
                                                   sched_properties {hr.mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ /
                                                                              FAP20M_HR_SCHED_MODE_DUAL_WFQ] },
                                                                     cl.class_type [0 / 31] } } }
    
                                                   flow {id [64 / 16K],
                                                         sub_flow[FAP20M_NOF_SUB_FLOWS] {valid_mask [0 / 0xFFFFFFFF],
                                                                                         id [64 / 16K],
                                                                                         credit_source [0 / 10],
                                                                                         shaper {max_rate [0 / 10],
                                                                                                 max_burst [1 / 10] }
                                                                                         type [FAP20M_SUB_FLOW_TYPE_HR /
                                                                                               FAP20M_SUB_FLOW_TYPE_CL]
    
                                                                                         subflow_properties
                                                                                           { hr { sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF1 / FAP20M_SUB_FLOW_HR_ENHANCED_CLASS_AF3],
                                                                                                  weight [1 / 0xFFF] }
                                                                                             cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ / FAP20M_SUB_FLOW_CL_CLASS_SP1],
                                                                                                  weight [1 / 64] }  }
                                                                                          }
                                                   flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW]
                                                   is_slow_enabled [TRUE / FALSE] }
                                                   valid [TRUE / FALSE] } },
        Expected: SAND_OK.
    */
    /* iterate with sch.id = 0 */
    for(i = 0; i < FAP20M_AUTO_NOF_HIERARCY; i++)
        for(j = 0; j < FAP20M_AUTO_NOF_INDEX; j++)
        {
            p_aggregates_info.item[i][j].sch.id = 0;
            p_aggregates_info.item[i][j].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
            p_aggregates_info.item[i][j].sch.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;
            p_aggregates_info.item[i][j].sch.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

            p_aggregates_info.item[i][j].flow.id = 64;

            for(k = 0; k < FAP20M_NOF_SUB_FLOWS; k++)
            {
                p_aggregates_info.item[i][j].flow.sub_flow[k].valid_mask = 0;
                p_aggregates_info.item[i][j].flow.sub_flow[k].id = 64;
                p_aggregates_info.item[i][j].flow.sub_flow[k].credit_source = 0;
                p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_rate = 0;
                p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_burst = 1;
                p_aggregates_info.item[i][j].flow.sub_flow[k].type = FAP20M_SUB_FLOW_TYPE_HR;
                p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF1;
                p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.hr.weight = 1;
            }

            p_aggregates_info.item[i][j].flow.flow_type = FAP20M_REGULAR_SCH_FLOW;
            p_aggregates_info.item[i][j].flow.is_slow_enabled = TRUE;
            p_aggregates_info.item[i][j].valid = TRUE;
        }

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* iterate with sch.id = 10 */
    for(i = 0; i < FAP20M_AUTO_NOF_HIERARCY; i++)
        for(j = 0; j < FAP20M_AUTO_NOF_INDEX; j++)
        {
            p_aggregates_info.item[i][j].sch.id = 10;
            p_aggregates_info.item[i][j].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;
            p_aggregates_info.item[i][j].sch.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE;
            p_aggregates_info.item[i][j].sch.sched_properties.cl.class_type = 0;

            p_aggregates_info.item[i][j].flow.id = 0x3FFF;

            for(k = 0; k < FAP20M_NOF_SUB_FLOWS; k++)
            {
                p_aggregates_info.item[i][j].flow.sub_flow[k].valid_mask = 0xFFFFFFFF;
                p_aggregates_info.item[i][j].flow.sub_flow[k].id = 0x3FFF;
                p_aggregates_info.item[i][j].flow.sub_flow[k].credit_source = 0;
                p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_rate = 0;
                p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_burst = 1;
                p_aggregates_info.item[i][j].flow.sub_flow[k].type = FAP20M_SUB_FLOW_TYPE_CL;
                p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ;
                p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.cl.weight = 1;
            }

            p_aggregates_info.item[i][j].flow.flow_type = FAP20M_AGGREGATE_SCH_FLOW;
            p_aggregates_info.item[i][j].flow.is_slow_enabled = FALSE;
            p_aggregates_info.item[i][j].valid = FALSE;
        }

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call p_aggregates_info->item.sch.type [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_aggregates_info.item[0][0].sch.type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_aggregates_info.item[0][0].sch.type = %d",
                                     p_aggregates_info.item[0][0].sch.type);

    p_aggregates_info.item[0][0].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;

    /*
        3. Call p_aggregates_info->item.sch.state [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_aggregates_info.item[0][0].sch.state = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_aggregates_info.item[0][0].sch.state = %d",
                                     p_aggregates_info.item[0][0].sch.state);

    p_aggregates_info.item[0][0].sch.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE;

    /*
        4. Call p_aggregates_info->item.sch.type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR],
                  p_aggregates_info->item.sch.sched_properties.hr.mode [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_aggregates_info.item[0][0].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
    p_aggregates_info.item[0][0].sch.sched_properties.hr.mode = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_aggregates_info.item[0][0].sch.sched_properties.hr.mode = %d",
                                     p_aggregates_info.item[0][0].sch.sched_properties.hr.mode);

    p_aggregates_info.item[0][0].sch.sched_properties.hr.mode = FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE;

    /*
        5. Call p_aggregates_info->item.sch.type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL],
                  p_aggregates_info->item.sch.sched_properties.cl.class_type [32] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_aggregates_info.item[0][0].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;
    p_aggregates_info.item[0][0].sch.sched_properties.cl.class_type = 32;

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_aggregates_info.item[0][0].sch.sched_properties.cl.class_type = %d",
                                     p_aggregates_info.item[0][0].sch.sched_properties.cl.class_type);

    p_aggregates_info.item[0][0].sch.sched_properties.cl.class_type = 0;

    /*
        6. Call p_aggregates_info->item.flow.sub_flow[0].type [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_aggregates_info.item[0][0].flow.sub_flow[0].type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_aggregates_info.item[0][0].flow.sub_flow[0].type = %d",
                                     p_aggregates_info.item[0][0].flow.sub_flow[0].type);

    p_aggregates_info.item[0][0].flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;

    /*
        7. Call p_aggregates_info->item.flow.sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_HR],
                p_aggregates_info->item.flow.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_aggregates_info.item[0][0].flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
    p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class = %d",
                                     p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class);

    p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF1;

    /*
        8. Call p_aggregates_info->item.flow.sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_CL],
                  p_aggregates_info->item.flow.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_aggregates_info.item[0][0].flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
    p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val = %d",
                                     p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val);

    p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ;

    /*
        9. Call p_aggregates_info->item.flow.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_aggregates_info.item[0][0].flow.flow_type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_aggregates_info.item[0][0].flow.flow_type = %d",
                                     p_aggregates_info.item[0][0].flow.flow_type);

    p_aggregates_info.item[0][0].flow.flow_type = FAP20M_REGULAR_SCH_FLOW;

    /*
        10. Call with countValuePtr [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_set_info_and_open_all_aggregates(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_aggregates_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_port_aggregates_get_info(
    SAND_IN  unsigned int              device_id,
    SAND_IN  unsigned int              port_id,
    SAND_OUT FAP20M_AUTO_AGG_INFO   *p_aggregates_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_port_aggregates_get_info)
{
/*
    ITERATE_DEVICES_PORTS
    1.1.1. Call with non-null p_aggregates_info.
    Expected: SAND_OK.
    1.1.2. Call with p_aggregates_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   port_id = 0;
    FAP20M_AUTO_AGG_INFO p_aggregates_info;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id = 0; port_id < FAP20M_NOF_DATA_PORTS; port_id++)
        {
            /*
                1.1.1. Call with non-null p_port_info.
                Expected: SAND_OK.
            */
            result = fap20m_api_auto_port_aggregates_get_info(device_id, port_id, &p_aggregates_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /*
                1.1.2. Call with p_port_info [NULL].
                Expected: SAND_NULL_POINTER_ERR.
            */
            result = fap20m_api_auto_port_aggregates_get_info(device_id, port_id, NULL);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_aggregates_info = NULL");
        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_port_aggregates_get_info(device_id, port_id, &p_aggregates_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_port_aggregates_get_info(device_id, port_id, &p_aggregates_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_port_aggregates_get_info(device_id, port_id, &p_aggregates_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_update_port_aggregates(
    SAND_IN  unsigned int              device_id,
    SAND_IN  unsigned int              port_id,
    SAND_IN  FAP20M_AUTO_AGG_INFO   *p_aggregates_info,
    SAND_OUT FAP20M_AUTO_AGG_INFO   *exact_aggregates_info,
    SAND_IN  FAP20M_AUTO_FLOW_INFO  *p_flows_info,
    SAND_OUT FAP20M_AUTO_FLOW_INFO  *exact_flows_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_update_port_aggregates)
{
/*
    ITERATE_DEVICES_PORTS
    1.1.1. Call with p_aggregates_info {item {  sch { id [0 / 10],
                                                    type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR / FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL],
                                                    state [FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE / FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE],

                                                    sched_properties
                                                    { hr { mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ / FAP20M_HR_SCHED_MODE_DUAL_WFQ] },
                                                      cl { class_type [0 / 31] } } }

                                              flow { id [0 / 10],
                                                     sub_flow[FAP20M_NOF_SUB_FLOWS] {  valid_mask [0 / 0xFFFFFFFF],
                                                                                       id [64 / 16K],
                                                                                       credit_source [0 / 10],
                                                                                       shaper { max_rate [0 / 10], max_burst [1 / 10] }
                                                                                       type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL]

                                                                                       subflow_properties
                                                                                       { hr { sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF1 / FAP20M_SUB_FLOW_HR_ENHANCED_CLASS_AF3],
                                                                                              weight [1 / 0xFFF] }
                                                                                         cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ / FAP20M_SUB_FLOW_CL_CLASS_SP1],
                                                                                              weight [1 / 64] }  }
                                                                                      }
                                                      flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW]
                                                      is_slow_enabled [TRUE / FALSE] }
                                              valid [TRUE / FALSE] } },

                  p_flows_info { flows[] { id [64 / 16k]
                                          sub_flow[]{ valid_mask [0 / 0xFFFFFFFF],
                                                       id [64 / 16K],
                                                       credit_source [0 / 10],
                                                       shaper { max_rate [0 / 10], max_burst [0 / 10] },
                                                       type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                       subflow_properties{
                                                                hr{ sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF2 / FAP20M_SUB_FLOW_HR_CLASS_EF3],
                                                                    weight [1 / 4K] },
                                                                cl { class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
                                                                weight [1 / 64] } }  }
                                 flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW];
                                 is_slow_enabled [TRUE / FALSE] }

                  non-null exact_aggregates_info and exact_flows_info.
    Expected: SAND_OK.

    1.1.2. Call p_aggregates_info->item.sch.type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.3. Call p_aggregates_info->item.sch.state [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.1.4. Call p_aggregates_info->item.sch.type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR],
              p_aggregates_info->item.sch.sched_properties.hr.mode [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.5. Call p_aggregates_info->item.sch.type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL],
              p_aggregates_info->item.sch.sched_properties.cl.class_type [32] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.1.6. Call p_aggregates_info->item.flow.sub_flow[0].type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.7. Call p_aggregates_info->item.flow.sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_HR],
              p_aggregates_info->item.flow.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.8. Call p_aggregates_info->item.flow.sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_CL],
              p_aggregates_info->item.flow.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.

    1.1.9. Call p_aggregates_info->item.flow.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.


    1.1.10. Call p_flows_info.flows.sub_flow.type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.11. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_HR],
                p_flows_info.flows.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.12. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_CL],
                p_flows_info.flows.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.13. Call p_flows_info.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.


    1.1.14. Call with p_aggregates_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.1.15. Call with exact_aggregates_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.1.16. Call with p_flows_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.1.17. Call with exact_flows_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  port_id = 0;

    SAND_U8 i, j, k = 0;

    FAP20M_AUTO_AGG_INFO  p_aggregates_info;
    FAP20M_AUTO_AGG_INFO  exact_aggregates_info;
    FAP20M_AUTO_FLOW_INFO p_flows_info;
    FAP20M_AUTO_FLOW_INFO exact_flows_info;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id=0; port_id<FAP20M_NOF_DATA_PORTS; port_id++)
        {
            /*
                 1.1.1. Call with p_aggregates_info {item {  sch { id [0 / 10],
                                                                type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR / FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL],
                                                                state [FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE / FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE],
    
                                                                sched_properties
                                                                { hr { mode [FAP20M_HR_SCHED_MODE_SINGLE_WFQ / FAP20M_HR_SCHED_MODE_DUAL_WFQ] },
                                                                  cl { class_type [0 / 31] } } }
    
                                                          flow { id [0 / 10],
                                                                 sub_flow[FAP20M_NOF_SUB_FLOWS] {  valid_mask [0 / 0xFFFFFFFF],
                                                                                                   id [64 / 16K],
                                                                                                   credit_source [0 / 10],
                                                                                                   shaper { max_rate [0 / 10], max_burst [1 / 10] }
                                                                                                   type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL]
    
                                                                                                   subflow_properties
                                                                                                   { hr { sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF1 / FAP20M_SUB_FLOW_HR_ENHANCED_CLASS_AF3],
                                                                                                          weight [1 / 0xFFF] }
                                                                                                     cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ / FAP20M_SUB_FLOW_CL_CLASS_SP1],
                                                                                                          weight [1 / 64] }  }
                                                                                                  }
                                                                  flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW]
                                                                  is_slow_enabled [TRUE / FALSE] }
                                                          valid [TRUE / FALSE] } },
    
                              p_flows_info { flows[] { id [64 / 16k]
                                                      sub_flow[]{ valid_mask [0 / 0xFFFFFFFF],
                                                                   id [64 / 16K],
                                                                   credit_source [0 / 10],
                                                                   shaper { max_rate [0 / 10], max_burst [0 / 10] },
                                                                   type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                                   subflow_properties{
                                                                            hr{ sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF2 / FAP20M_SUB_FLOW_HR_CLASS_EF3],
                                                                                weight [1 / 4K] },
                                                                            cl { class_val [FAP20M_SUB_FLOW_CL_CLASS_SP2],
                                                                            weight [1 / 64] } }  }
                                             flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW];
                                             is_slow_enabled [TRUE / FALSE] }
    
                              non-null exact_aggregates_info and exact_flows_info.
                Expected: SAND_OK.
            */
            /* iterate with sch.id = 0 */
            for(i = 0; i < FAP20M_AUTO_NOF_HIERARCY; i++)
                for(j = 0; j < FAP20M_AUTO_NOF_INDEX; j++)
                {
                    p_aggregates_info.item[i][j].sch.id = 0;
                    p_aggregates_info.item[i][j].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
                    p_aggregates_info.item[i][j].sch.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;
                    p_aggregates_info.item[i][j].sch.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

                    p_aggregates_info.item[i][j].flow.id = 0;

                    for(k = 0; k < FAP20M_NOF_SUB_FLOWS; k++)
                    {
                        p_aggregates_info.item[i][j].flow.sub_flow[k].valid_mask = 0;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].id = 64;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].credit_source = 0;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_rate = 0;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_burst = 1;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].type = FAP20M_SUB_FLOW_TYPE_HR;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF1;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.hr.weight = 1;
                    }

                    p_aggregates_info.item[i][j].flow.flow_type = FAP20M_REGULAR_SCH_FLOW;
                    p_aggregates_info.item[i][j].flow.is_slow_enabled = TRUE;
                    p_aggregates_info.item[i][j].valid = TRUE;
                }

            for(i = 0; i < FAP20M_FABRIC_UNICAST_CLASSES; i++)
            {
                p_flows_info.flows[i].id = 64;

                for(j = 0; j < FAP20M_NOF_SUB_FLOWS; j++)
                {
                    p_flows_info.flows[i].sub_flow[j].valid_mask = 0;
                    p_flows_info.flows[i].sub_flow[j].id = 64;
                    p_flows_info.flows[i].sub_flow[j].credit_source = 0;
                    p_flows_info.flows[i].sub_flow[j].shaper.max_rate = 0;
                    p_flows_info.flows[i].sub_flow[j].shaper.max_burst = 0;
                    p_flows_info.flows[i].sub_flow[j].type = FAP20M_SUB_FLOW_TYPE_HR;
                    p_flows_info.flows[i].sub_flow[j].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;
                    p_flows_info.flows[i].sub_flow[j].subflow_properties.hr.weight = 1;
                }

                p_flows_info.flows[i].flow_type = FAP20M_REGULAR_SCH_FLOW;
                p_flows_info.flows[i].is_slow_enabled = TRUE;
            }

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /* iterate with sch.id = 10 */
            for(i = 0; i < FAP20M_AUTO_NOF_HIERARCY; i++)
                for(j = 0; j < FAP20M_AUTO_NOF_INDEX; j++)
                {
                    p_aggregates_info.item[i][j].sch.id = 10;
                    p_aggregates_info.item[i][j].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;
                    p_aggregates_info.item[i][j].sch.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE;
                    p_aggregates_info.item[i][j].sch.sched_properties.cl.class_type = 0;

                    p_aggregates_info.item[i][j].flow.id = 10;

                    for(k = 0; k < FAP20M_NOF_SUB_FLOWS; k++)
                    {
                        p_aggregates_info.item[i][j].flow.sub_flow[k].valid_mask = 0xFFFFFFFF;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].id = 0x3FFF;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].credit_source = 0;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_rate = 0;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_burst = 1;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].type = FAP20M_SUB_FLOW_TYPE_CL;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ;
                        p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.cl.weight = 1;
                    }

                    p_aggregates_info.item[i][j].flow.flow_type = FAP20M_AGGREGATE_SCH_FLOW;
                    p_aggregates_info.item[i][j].flow.is_slow_enabled = FALSE;
                    p_aggregates_info.item[i][j].valid = FALSE;
                }

            for(i = 0; i < FAP20M_FABRIC_UNICAST_CLASSES; i++)
            {
                p_flows_info.flows[i].id = 0x3FFF;

                for(j = 0; j < FAP20M_NOF_SUB_FLOWS; j++)
                {
                    p_flows_info.flows[i].sub_flow[j].valid_mask = 0xFFFFFFFF;
                    p_flows_info.flows[i].sub_flow[j].id = 0x3FFF;
                    p_flows_info.flows[i].sub_flow[j].credit_source = 10;
                    p_flows_info.flows[i].sub_flow[j].shaper.max_rate = 10;
                    p_flows_info.flows[i].sub_flow[j].shaper.max_burst = 10;
                    p_flows_info.flows[i].sub_flow[j].type = FAP20M_SUB_FLOW_TYPE_CL;
                    p_flows_info.flows[i].sub_flow[j].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP2;
                    p_flows_info.flows[i].sub_flow[j].subflow_properties.cl.weight = 1;
                }

                p_flows_info.flows[i].flow_type = FAP20M_AGGREGATE_SCH_FLOW;
                p_flows_info.flows[i].is_slow_enabled = FALSE;
            }

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /*
                1.1.2. Call p_aggregates_info->item.sch.type [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_aggregates_info.item[0][0].sch.type = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_aggregates_info.item[0][0].sch.type = %d",
                                             device_id, port_id, p_aggregates_info.item[0][0].sch.type);

            p_aggregates_info.item[0][0].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;

            /*
                1.1.3. Call p_aggregates_info->item.sch.state [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_aggregates_info.item[0][0].sch.state = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_aggregates_info.item[0][0].sch.state = %d",
                                             device_id, port_id, p_aggregates_info.item[0][0].sch.state);

            p_aggregates_info.item[0][0].sch.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE;

            /*
                1.1.4. Call p_aggregates_info->item.sch.type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR],
                          p_aggregates_info->item.sch.sched_properties.hr.mode [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_aggregates_info.item[0][0].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
            p_aggregates_info.item[0][0].sch.sched_properties.hr.mode = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_aggregates_info.item[0][0].sch.sched_properties.hr.mode = %d",
                                             device_id, port_id, p_aggregates_info.item[0][0].sch.sched_properties.hr.mode);

            p_aggregates_info.item[0][0].sch.sched_properties.hr.mode = FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE;

            /*
                1.1.5. Call p_aggregates_info->item.sch.type [FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL],
                          p_aggregates_info->item.sch.sched_properties.cl.class_type [32] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_aggregates_info.item[0][0].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;
            p_aggregates_info.item[0][0].sch.sched_properties.cl.class_type = 32;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_aggregates_info.item[0][0].sch.sched_properties.cl.class_type = %d",
                                             device_id, port_id, p_aggregates_info.item[0][0].sch.sched_properties.cl.class_type);

            p_aggregates_info.item[0][0].sch.sched_properties.cl.class_type = 0;

            /*
                1.1.6. Call p_aggregates_info->item.flow.sub_flow[0].type [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_aggregates_info.item[0][0].flow.sub_flow[0].type = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_aggregates_info.item[0][0].flow.sub_flow[0].type = %d",
                                             device_id, port_id, p_aggregates_info.item[0][0].flow.sub_flow[0].type);

            p_aggregates_info.item[0][0].flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;

            /*
                1.1.7. Call p_aggregates_info->item.flow.sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_HR],
                        p_aggregates_info->item.flow.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_aggregates_info.item[0][0].flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
            p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class = %d",
                                             device_id, port_id, p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class);

            p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF1;

            /*
                1.1.8. Call p_aggregates_info->item.flow.sub_flow[0].type [FAP20M_SUB_FLOW_TYPE_CL],
                          p_aggregates_info->item.flow.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_aggregates_info.item[0][0].flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
            p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val = %d",
                                             device_id, port_id, p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val);

            p_aggregates_info.item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ;

            /*
                1.1.9. Call p_aggregates_info->item.flow.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_aggregates_info.item[0][0].flow.flow_type = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_aggregates_info.item[0][0].flow.flow_type = %d",
                                             device_id, port_id, p_aggregates_info.item[0][0].flow.flow_type);

            p_aggregates_info.item[0][0].flow.flow_type = FAP20M_REGULAR_SCH_FLOW;

            /*
                1.1.10. Call p_flows_info.flows.sub_flow.type [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_flows_info.flows[0].sub_flow[0].type = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_flows_info.flows[0].sub_flow[0].type = %d",
                                             device_id, port_id, p_flows_info.flows[0].sub_flow[0].type);

            p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;

            /*
                1.1.11. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_HR],
                            p_flows_info.flows.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
            p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = %d",
                                             device_id, port_id, p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class);

            p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;

            /*
                1.1.12. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_CL],
                            p_flows_info.flows.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
            p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = %d",
                                             device_id, port_id, p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val);

            p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP2;

            /*
                1.1.13. Call p_flows_info.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_flows_info.flows[0].flow_type = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_flows_info.flow_type = %d",
                                             device_id, port_id, p_flows_info.flows[0].flow_type);

            p_flows_info.flows[0].flow_type = FAP20M_REGULAR_SCH_FLOW;

            /*
                1.1.14. Call with p_aggregates_info [NULL].
                Expected: SAND_NULL_POINTER_ERR.
            */
            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, NULL,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_aggregates_info = NULL");

            /*
                1.1.15. Call with exact_aggregates_info [NULL].
                Expected: SAND_NULL_POINTER_ERR.
            */
            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            NULL,
                                                            &p_flows_info,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "exact_aggregates_info = NULL");

            /*
                1.1.16. Call with p_flows_info [NULL].
                Expected: SAND_NULL_POINTER_ERR.
            */
            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            NULL,
                                                            &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_flows_info = NULL");

            /*
                1.1.17. Call with exact_flows_info [NULL].
                Expected: SAND_NULL_POINTER_ERR.
            */
            result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            NULL);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "exact_flows_info = NULL");
        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                            &exact_aggregates_info,
                                                            &p_flows_info,
                                                            &exact_flows_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    for(i = 0; i < FAP20M_AUTO_NOF_HIERARCY; i++)
        for(j = 0; j < FAP20M_AUTO_NOF_INDEX; j++)
        {
            p_aggregates_info.item[i][j].sch.id = 0;
            p_aggregates_info.item[i][j].sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR;
            p_aggregates_info.item[i][j].sch.state = FAP20M_ELEMENTARY_SCHEDULER_STATE_ENABLE;
            p_aggregates_info.item[i][j].sch.sched_properties.hr.mode = FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

            p_aggregates_info.item[i][j].flow.id = 0;

            for(k = 0; k < FAP20M_NOF_SUB_FLOWS; k++)
            {
                p_aggregates_info.item[i][j].flow.sub_flow[k].valid_mask = 0;
                p_aggregates_info.item[i][j].flow.sub_flow[k].id = 64;
                p_aggregates_info.item[i][j].flow.sub_flow[k].credit_source = 0;
                p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_rate = 0;
                p_aggregates_info.item[i][j].flow.sub_flow[k].shaper.max_burst = 1;
                p_aggregates_info.item[i][j].flow.sub_flow[k].type = FAP20M_SUB_FLOW_TYPE_HR;
                p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF1;
                p_aggregates_info.item[i][j].flow.sub_flow[k].subflow_properties.hr.weight = 1;
            }

            p_aggregates_info.item[i][j].flow.flow_type = FAP20M_REGULAR_SCH_FLOW;
            p_aggregates_info.item[i][j].flow.is_slow_enabled = TRUE;
            p_aggregates_info.item[i][j].valid = TRUE;
        }

    for(i=0; i<FAP20M_FABRIC_UNICAST_CLASSES; i++)
    {
        p_flows_info.flows[i].id = 64;

        for(j = 0; j < FAP20M_NOF_SUB_FLOWS; j++)
        {
            p_flows_info.flows[i].sub_flow[j].valid_mask = 0;
            p_flows_info.flows[i].sub_flow[j].id = 64;
            p_flows_info.flows[i].sub_flow[j].credit_source = 0;
            p_flows_info.flows[i].sub_flow[j].shaper.max_rate = 0;
            p_flows_info.flows[i].sub_flow[j].shaper.max_burst = 0;
            p_flows_info.flows[i].sub_flow[j].type = FAP20M_SUB_FLOW_TYPE_HR;
            p_flows_info.flows[i].sub_flow[j].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;
            p_flows_info.flows[i].sub_flow[j].subflow_properties.hr.weight = 1;
        }

        p_flows_info.flows[i].flow_type = FAP20M_REGULAR_SCH_FLOW;
        p_flows_info.flows[i].is_slow_enabled = TRUE;
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                        &exact_aggregates_info,
                                                        &p_flows_info,
                                                        &exact_flows_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_update_port_aggregates(device_id, port_id, &p_aggregates_info,
                                                    &exact_aggregates_info,
                                                    &p_flows_info,
                                                    &exact_flows_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_close_port_aggregates(
    SAND_IN  unsigned int            device_id,
    SAND_IN  unsigned int            port_id
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_close_port_aggregates)
{
/*
    ITERATE_DEVICES_PORTS
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  port_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id = 0; port_id < FAP20M_NOF_DATA_PORTS; port_id++)
        {
            result = fap20m_api_auto_close_port_aggregates(device_id, port_id);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_close_port_aggregates(device_id, port_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_close_port_aggregates(device_id, port_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_close_port_aggregates(device_id, port_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_flows_get_default(
    SAND_OUT FAP20M_AUTO_FLOW_INFO      *p_flows_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_flows_get_default)
{
/*
    1. Call with non-null p_flows_info.
    Expected: SAND_OK.
    2. Call with p_flows_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_AUTO_FLOW_INFO p_flows_info;


    /*
        1. Call with non-null p_flows_info.
        Expected: SAND_OK.
    */
    result = fap20m_api_auto_flows_get_default(&p_flows_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call with p_flows_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_flows_get_default(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_flows_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_set_info_and_open_all_flows(
    SAND_IN  FAP20M_AUTO_FLOW_INFO *p_flows_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_set_info_and_open_all_flows)
{
/*
    1. Call with p_flows_info { flows[] { id [64 / 16k]
                                          sub_flow[]{ valid_mask [0 / 0xFFFFFFFF],
                                                       id [64 / 16K],
                                                       credit_source [0 / 10],
                                                       shaper { max_rate [0 / 10], max_burst [0 / 10] },
                                                       type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                       subflow_properties{
                                                                hr{ sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF2 / FAP20M_SUB_FLOW_HR_CLASS_EF3],
                                                                    weight [1 / 4K] },
                                                                cl { class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
                                                                weight [1 / 64] } }  }
                                 flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW];
                                 is_slow_enabled [TRUE / FALSE] }

    Expected: SAND_OK.
    2. Call p_flows_info.flows.sub_flow.type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    3. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_HR],
                p_flows_info.flows.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    4. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_CL],
                p_flows_info.flows.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    5. Call p_flows_info.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    6. Call with p_flows_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_U32   i, j;
    FAP20M_AUTO_FLOW_INFO p_flows_info;

    /*
        1. Call with p_flows_info { flows[] { id [64 / 16k]
                                              sub_flow[]{ valid_mask [0 / 0xFFFFFFFF],
                                                           id [64 / 16K],
                                                           credit_source [0 / 10],
                                                           shaper { max_rate [0 / 10], max_burst [0 / 10] },
                                                           type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                           subflow_properties{
                                                                    hr{ sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF2 / FAP20M_SUB_FLOW_HR_CLASS_EF3],
                                                                        weight [1 / 4K] },
                                                                    cl { class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
                                                                    weight [1 / 64] } }  }
                                     flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW];
                                     is_slow_enabled [TRUE / FALSE] }
    
        Expected: SAND_OK.
    */
    /* iterate with */
    for(i = 0; i < FAP20M_FABRIC_UNICAST_CLASSES; i++)
    {
        p_flows_info.flows[i].id = 64;

        for(j = 0; j < FAP20M_NOF_SUB_FLOWS; j++)
        {
            p_flows_info.flows[i].sub_flow[j].valid_mask = 0;
            p_flows_info.flows[i].sub_flow[j].id = 64;
            p_flows_info.flows[i].sub_flow[j].credit_source = 0;
            p_flows_info.flows[i].sub_flow[j].shaper.max_rate = 0;
            p_flows_info.flows[i].sub_flow[j].shaper.max_burst = 0;
            p_flows_info.flows[i].sub_flow[j].type = FAP20M_SUB_FLOW_TYPE_HR;
            p_flows_info.flows[i].sub_flow[j].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;
            p_flows_info.flows[i].sub_flow[j].subflow_properties.hr.weight = 1;
        }

        p_flows_info.flows[i].flow_type = FAP20M_REGULAR_SCH_FLOW;
        p_flows_info.flows[i].is_slow_enabled = TRUE;
    }

    result = fap20m_api_auto_set_info_and_open_all_flows(&p_flows_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* iterate with */
    for(i = 0; i < FAP20M_FABRIC_UNICAST_CLASSES; i++)
    {
        p_flows_info.flows[i].id = 0x3FFF;

        for(j = 0; j < FAP20M_NOF_SUB_FLOWS; j++)
        {
            p_flows_info.flows[i].sub_flow[j].valid_mask = 0xFFFFFFFF;
            p_flows_info.flows[i].sub_flow[j].id = 0x3FFF;
            p_flows_info.flows[i].sub_flow[j].credit_source = 10;
            p_flows_info.flows[i].sub_flow[j].shaper.max_rate = 10;
            p_flows_info.flows[i].sub_flow[j].shaper.max_burst = 10;
            p_flows_info.flows[i].sub_flow[j].type = FAP20M_SUB_FLOW_TYPE_CL;
            p_flows_info.flows[i].sub_flow[j].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP2;
            p_flows_info.flows[i].sub_flow[j].subflow_properties.cl.weight = 1;
        }

        p_flows_info.flows[i].flow_type = FAP20M_AGGREGATE_SCH_FLOW;
        p_flows_info.flows[i].is_slow_enabled = FALSE;
    }

    result = fap20m_api_auto_set_info_and_open_all_flows(&p_flows_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call p_flows_info.flows.sub_flow.type [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_flows_info.flows[0].sub_flow[0].type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_flows(&p_flows_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_flows_info.flows[0].sub_flow[0].type = %d",
                                     p_flows_info.flows[0].sub_flow[0].type);

    p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;

    /*
        3. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_HR],
                    p_flows_info.flows.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
    p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_flows(&p_flows_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = %d",
                                     p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class);

    p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;

    /*
        4. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_CL],
                    p_flows_info.flows.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
    p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_flows(&p_flows_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = %d",
                                     p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val);

    p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP2;

    /*
        5. Call p_flows_info.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_flows_info.flows[0].flow_type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_flows(&p_flows_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "p_flows_info.flow_type = %d",
                                     p_flows_info.flows[0].flow_type);

    p_flows_info.flows[0].flow_type = FAP20M_REGULAR_SCH_FLOW;

    /*
        6. Call with p_flows_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_set_info_and_open_all_flows(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_flows_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_port_flows_get_info(
    SAND_IN  unsigned int              device_id,
    SAND_IN  unsigned int              port_id,
    SAND_OUT FAP20M_AUTO_FLOW_INFO   *p_flows_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_port_flows_get_info)
{
/*
    ITERATE_DEVICES_PORTS
    1.1.1. Call with non-null p_flows_info.
    Expected: SAND_OK.
    1.1.2. Call with p_flows_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  port_id = 0;
    FAP20M_AUTO_FLOW_INFO p_flows_info;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id = 0; port_id < FAP20M_NOF_DATA_PORTS; port_id++)
        {
            /*
                1.1.1. Call with non-null p_flows_info.
                Expected: SAND_OK.
            */
            result = fap20m_api_auto_port_flows_get_info(device_id, port_id, &p_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /*
                1.1.2. Call with p_flows_info [NULL].
                Expected: SAND_NULL_POINTER_ERR.
            */
            result = fap20m_api_auto_port_flows_get_info(device_id, port_id, NULL);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_flows_info = NULL");
        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_port_flows_get_info(device_id, port_id, &p_flows_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_port_flows_get_info(device_id, port_id, &p_flows_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_port_flows_get_info(device_id, port_id, &p_flows_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_update_port_flows(
    SAND_IN  unsigned int              device_id,
    SAND_IN  unsigned int              port_id,
    SAND_IN  FAP20M_AUTO_FLOW_INFO   *p_flows_info,
    SAND_OUT FAP20M_AUTO_FLOW_INFO   *exact_flows_info
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_update_port_flows)
{
/*
    ITERATE_DEVICES_PORTS
    1.1.1. Call with p_flows_info { flows[] { id [64 / 16k]
                                          sub_flow[]{ valid_mask [0 / 0xFFFFFFFF],
                                                       id [64 / 16K],
                                                       credit_source [0 / 10],
                                                       shaper { max_rate [0 / 10], max_burst [0 / 10] },
                                                       type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                       subflow_properties{
                                                                hr{ sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF2 / FAP20M_SUB_FLOW_HR_CLASS_EF3],
                                                                    weight [1 / 4K] },
                                                                cl { class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
                                                                weight [1 / 64] } }  }
                                 flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW];
                                 is_slow_enabled [TRUE / FALSE] }
                   non-null exact_flows_info.
    Expected: SAND_OK.
    1.1.2. Call p_flows_info.flows.sub_flow.type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.3. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_HR],
                p_flows_info.flows.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.4. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_CL],
                p_flows_info.flows.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.5. Call p_flows_info.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.1.6. Call with p_flows_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    1.1.7. Call with exact_flows_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  port_id = 0;

    FAP20M_AUTO_FLOW_INFO p_flows_info;
    FAP20M_AUTO_FLOW_INFO exact_flows_info;
    SAND_UINT  i, j;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id = 0; port_id < FAP20M_NOF_DATA_PORTS; port_id++)
        {
            /*
                1.1.1. Call with p_flows_info { flows[] { id [64 / 16k]
                                                          sub_flow[]{ valid_mask [0 / 0xFFFFFFFF],
                                                                   id [64 / 16K],
                                                                   credit_source [0 / 10],
                                                                   shaper { max_rate [0 / 10], max_burst [0 / 10] },
                                                                   type [FAP20M_SUB_FLOW_TYPE_HR / FAP20M_SUB_FLOW_TYPE_CL],
                                                                   subflow_properties{
                                                                            hr{ sp_class [FAP20M_SUB_FLOW_HR_CLASS_EF2 / FAP20M_SUB_FLOW_HR_CLASS_EF3],
                                                                                weight [1 / 4K] },
                                                                            cl { class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
                                                                            weight [1 / 64] } }  }
                                             flow_type [FAP20M_REGULAR_SCH_FLOW / FAP20M_AGGREGATE_SCH_FLOW];
                                             is_slow_enabled [TRUE / FALSE] }

                Expected: SAND_OK.
            */
            /* iterate with */
            for(i = 0; i < FAP20M_FABRIC_UNICAST_CLASSES; i++)
            {
                p_flows_info.flows[i].id = 64;

                for(j = 0; j < FAP20M_NOF_SUB_FLOWS; j++)
                {
                    p_flows_info.flows[i].sub_flow[j].valid_mask = 0;
                    p_flows_info.flows[i].sub_flow[j].id = 64;
                    p_flows_info.flows[i].sub_flow[j].credit_source = 0;
                    p_flows_info.flows[i].sub_flow[j].shaper.max_rate = 0;
                    p_flows_info.flows[i].sub_flow[j].shaper.max_burst = 0;
                    p_flows_info.flows[i].sub_flow[j].type = FAP20M_SUB_FLOW_TYPE_HR;
                    p_flows_info.flows[i].sub_flow[j].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;
                    p_flows_info.flows[i].sub_flow[j].subflow_properties.hr.weight = 1;
                }

                p_flows_info.flows[i].flow_type = FAP20M_REGULAR_SCH_FLOW;
                p_flows_info.flows[i].is_slow_enabled = TRUE;
            }

            result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /* iterate with */
            for(i = 0; i < FAP20M_FABRIC_UNICAST_CLASSES; i++)
            {
                p_flows_info.flows[i].id = 0x3FFF;

                for(j = 0; j < FAP20M_NOF_SUB_FLOWS; j++)
                {
                    p_flows_info.flows[i].sub_flow[j].valid_mask = 0xFFFFFFFF;
                    p_flows_info.flows[i].sub_flow[j].id = 0x3FFF;
                    p_flows_info.flows[i].sub_flow[j].credit_source = 10;
                    p_flows_info.flows[i].sub_flow[j].shaper.max_rate = 10;
                    p_flows_info.flows[i].sub_flow[j].shaper.max_burst = 10;
                    p_flows_info.flows[i].sub_flow[j].type = FAP20M_SUB_FLOW_TYPE_CL;
                    p_flows_info.flows[i].sub_flow[j].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP2;
                    p_flows_info.flows[i].sub_flow[j].subflow_properties.cl.weight = 1;
                }

                p_flows_info.flows[i].flow_type = FAP20M_AGGREGATE_SCH_FLOW;
                p_flows_info.flows[i].is_slow_enabled = FALSE;
            }

            result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);

            /*
                1.1.2. Call p_flows_info.flows.sub_flow.type [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_flows_info.flows[0].sub_flow[0].type = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_flows_info.flows[0].sub_flow[0].type = %d",
                                             device_id, port_id, p_flows_info.flows[0].sub_flow[0].type);

            p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;

            /*
                1.1.3. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_HR],
                            p_flows_info.flows.sub_flow[0].subflow_properties.hr.sp_class [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
            p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = %d",
                                             device_id, port_id, p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class);

            p_flows_info.flows[0].sub_flow[0].subflow_properties.hr.sp_class = FAP20M_SUB_FLOW_HR_CLASS_EF2;

            /*
                1.1.4. Call p_flows_info.flows.sub_flow.type [FAP20M_SUB_FLOW_TYPE_CL],
                            p_flows_info.flows.sub_flow[0].subflow_properties.cl.class_val [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_flows_info.flows[0].sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_CL;
            p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = %d",
                                             device_id, port_id, p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val);

            p_flows_info.flows[0].sub_flow[0].subflow_properties.cl.class_val = FAP20M_SUB_FLOW_CL_CLASS_SP2;

            /*
                1.1.5. Call p_flows_info.flow_type [0x5AAAAAA5] (out of range) and other params from 1.1.
                Expected: NOT SAND_OK.
            */
            p_flows_info.flows[0].flow_type = SAND_UTF_API_AUTO_INVALID_ENUM;

            result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, %d, p_flows_info.flow_type = %d",
                                             device_id, port_id, p_flows_info.flows[0].flow_type);
        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_update_port_flows(device_id, port_id, &p_flows_info, &exact_flows_info);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_close_port_flows(
    SAND_IN  unsigned int            device_id,
    SAND_IN  unsigned int            port_id
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_close_port_flows)
{
/*
    ITERATE_DEVICES_PORTS
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  port_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        for(port_id = 0; port_id < FAP20M_NOF_DATA_PORTS; port_id++)
        {
            result = fap20m_api_auto_close_port_flows(device_id, port_id);

            rc = sand_get_error_code_from_error_word(result);
            UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
        }

        port_id = FAP20M_NOF_DATA_PORTS;

        result = fap20m_api_auto_close_port_flows(device_id, port_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, port_id);
    }

    port_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK.  */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_api_auto_close_port_flows(device_id, port_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_api_auto_close_port_flows(device_id, port_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_queue_type_params_get_default(
    SAND_OUT FAP20M_QUEUE_TYPE_PARAM *p_queue_type_params
  );
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_queue_type_params_get_default)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null p_queue_type_params.
    Expected: SAND_OK.
    1.2. Call with p_queue_type_params [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_QUEUE_TYPE_PARAM p_queue_type_params;


    /*
        1. Call with non-null p_queue_type_params.
        Expected: SAND_OK.
    */
    result = fap20m_api_auto_queue_type_params_get_default(&p_queue_type_params);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call with p_queue_type_params [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_queue_type_params_get_default(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_queue_type_params = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_get_next_vacant_queue_type_id(
    SAND_OUT FAP20M_QUEUE_TYPE  *default_queue_type
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_get_next_vacant_queue_type_id)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null default_queue_type.
    Expected: SAND_OK.
    1.2. Call with default_queue_type [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_QUEUE_TYPE default_queue_type;


    /*
        1. Call with non-null p_queue_type_params.
        Expected: SAND_OK.
    */
    result = fap20m_api_auto_get_next_vacant_queue_type_id(&default_queue_type);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call with p_queue_type_params [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_get_next_vacant_queue_type_id(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "default_queue_type = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_queue_set_queue_params(
    SAND_IN  FAP20M_QUEUE_TYPE           queue_type,
    SAND_IN  FAP20M_QUEUE_TYPE_PARAM   *queue_type_params,
    SAND_OUT FAP20M_QUEUE_TYPE_PARAM   *exact_queue_type_params
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_queue_set_queue_params)
{
/*
    1. Call with queue_type [FAP20M_QUEUE_2 / FAP20M_QUEUE_3],
                   queue_type_params { RED_enable [TRUE / FALSE],
                                       RED_exp_weight [0 / 31],
                                       flow_slow_th_up [0 / 10],
                                       flow_stat_msg [0 / 10],
                                       max_q_size [0 / 10],
                                       flow_slow_hys_en [0 / 10],
                                       hysteresis_size [0 / 10],
                                       header_compensation [0 / 10] }
                   non-null exact_queue_type_params.
    Expected: SAND_OK.
    2. CAll fap20m_api_auto_queue_get_queue_params with not NULL pointers.
    Expected: SAND_OK and the same params as was set.

    3. Call with queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expecte: NOT SAND_OK.
    4. Call with queue_type_params_ptr->RED_exp_weight [32] (out of range) and other params from 1.1.
    Expecte: NOT SAND_OK.

    5. Call with queue_type_params [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    6. Call with exact_queue_type_params [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_QUEUE_TYPE       queue_type = FAP20M_QUEUE_2;
    FAP20M_QUEUE_TYPE_PARAM queue_type_params;
    FAP20M_QUEUE_TYPE_PARAM queue_type_paramsGet;
    FAP20M_QUEUE_TYPE_PARAM exact_queue_type_params;
    SAND_UINT               is_vacant = TRUE;


    /*
        1. Call with queue_type [FAP20M_QUEUE_2 / FAP20M_QUEUE_3],
                       queue_type_params { RED_enable [TRUE / FALSE],
                                           RED_exp_weight [0 / 31],
                                           flow_slow_th_up [0 / 10],
                                           flow_stat_msg [0 / 10],
                                           max_q_size [0 / 10],
                                           flow_slow_hys_en [0 / 10],
                                           hysteresis_size [0 / 10],
                                           header_compensation [0 / 10] }
                       non-null exact_queue_type_params.
        Expected: SAND_OK.
    */
    /* iterate with queue_type = FAP20M_QUEUE_2 */
    queue_type = FAP20M_QUEUE_2;

    queue_type_params.RED_enable = TRUE;
    queue_type_params.RED_exp_weight = 0;
    queue_type_params.flow_slow_th_up = 0;
    queue_type_params.flow_stat_msg = 0;
    queue_type_params.max_q_size = 0;
    queue_type_params.flow_slow_hys_en = 0;
    queue_type_params.hysteresis_size = 0;
    queue_type_params.header_compensation = 0;

    result = fap20m_api_auto_queue_set_queue_params(queue_type, &queue_type_params, &exact_queue_type_params);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, queue_type);

    /*
        2. Call fap20m_api_auto_queue_get_queue_params with not NULL pointers.
        Expected: SAND_OK and the same params as was set.
    */
    result = fap20m_api_auto_queue_get_queue_params(queue_type, &queue_type_paramsGet, &is_vacant);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_api_auto_queue_get_queue_params: %d", queue_type);

    /* Verifying values */
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.RED_enable, queue_type_paramsGet.RED_enable, 
                                 "got another queue_type_params.RED_enable than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.RED_exp_weight, queue_type_paramsGet.RED_exp_weight, 
                                 "got another queue_type_params.RED_exp_weight than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.flow_slow_th_up, queue_type_paramsGet.flow_slow_th_up, 
                                 "got another queue_type_params.flow_slow_th_up than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.flow_stat_msg, queue_type_paramsGet.flow_stat_msg, 
                                 "got another queue_type_params.flow_stat_msg than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.max_q_size, queue_type_paramsGet.max_q_size, 
                                 "got another queue_type_params.max_q_size than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.flow_slow_hys_en, queue_type_paramsGet.flow_slow_hys_en, 
                                 "got another queue_type_params.flow_slow_hys_en than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.hysteresis_size, queue_type_paramsGet.hysteresis_size, 
                                 "got another queue_type_params.hysteresis_size than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.header_compensation, queue_type_paramsGet.header_compensation, 
                                 "got another queue_type_params.header_compensation than was set: %d", queue_type);

    /* iterate with queue_type = FAP20M_QUEUE_3 */
    queue_type = FAP20M_QUEUE_3;

    queue_type_params.RED_enable = FALSE;
    queue_type_params.RED_exp_weight = 31;
    queue_type_params.flow_slow_th_up = 10;
    queue_type_params.flow_stat_msg = 10;
    queue_type_params.max_q_size = 10;
    queue_type_params.flow_slow_hys_en = 10;
    queue_type_params.hysteresis_size = 10;
    queue_type_params.header_compensation = 10;

    result = fap20m_api_auto_queue_set_queue_params(queue_type, &queue_type_params, &exact_queue_type_params);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, queue_type);

    /*
        2. Call fap20m_api_auto_queue_get_queue_params with not NULL pointers.
        Expected: SAND_OK and the same params as was set.
    */
    result = fap20m_api_auto_queue_get_queue_params(queue_type, &queue_type_paramsGet, &is_vacant);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_api_auto_queue_get_queue_params: %d", queue_type);

    /* Verifying values */
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.RED_enable, queue_type_paramsGet.RED_enable, 
                                 "got another queue_type_params.RED_enable than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.RED_exp_weight, queue_type_paramsGet.RED_exp_weight, 
                                 "got another queue_type_params.RED_exp_weight than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.flow_slow_th_up, queue_type_paramsGet.flow_slow_th_up, 
                                 "got another queue_type_params.flow_slow_th_up than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.flow_stat_msg, queue_type_paramsGet.flow_stat_msg, 
                                 "got another queue_type_params.flow_stat_msg than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.max_q_size, queue_type_paramsGet.max_q_size, 
                                 "got another queue_type_params.max_q_size than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.flow_slow_hys_en, queue_type_paramsGet.flow_slow_hys_en, 
                                 "got another queue_type_params.flow_slow_hys_en than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.hysteresis_size, queue_type_paramsGet.hysteresis_size, 
                                 "got another queue_type_params.hysteresis_size than was set: %d", queue_type);
    UTF_VERIFY_EQUAL1_STRING_MAC(exact_queue_type_params.header_compensation, queue_type_paramsGet.header_compensation, 
                                 "got another queue_type_params.header_compensation than was set: %d", queue_type);

    /*
        3. Call with queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expecte: NOT SAND_OK.
    */
    queue_type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_queue_set_queue_params(queue_type, &queue_type_params, &exact_queue_type_params);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, queue_type);

    queue_type = FAP20M_QUEUE_3;

    /*
        4. Call with queue_type_params_ptr->RED_exp_weight [32] (out of range) and other params from 1.1.
        Expecte: NOT SAND_OK.
    */
    queue_type_params.RED_exp_weight = 32;

    result = fap20m_api_auto_queue_set_queue_params(queue_type, &queue_type_params, &exact_queue_type_params);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, queue_type_params.RED_exp_weight = %d",
                                     queue_type, queue_type_params.RED_exp_weight);

    queue_type_params.RED_exp_weight = 32;

    /*
        5. Call with queue_type_params [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_queue_set_queue_params(queue_type, NULL, &exact_queue_type_params);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "queue_type_params = NULL");

    /*
        6. Call with exact_queue_type_params [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_queue_set_queue_params(queue_type, &queue_type_params, NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "exact_queue_type_params = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_queue_get_queue_params(
    SAND_IN  FAP20M_QUEUE_TYPE           queue_type,
    SAND_OUT FAP20M_QUEUE_TYPE_PARAM   *queue_type_params,
    SAND_OUT unsigned int                *is_vacant
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_queue_get_queue_params)
{
/*
    1. Call with queue_type [FAP20M_QUEUE_2 / FAP20M_QUEUE_3], 
                 non-null queue_type_params, is_vacant.
    Expected: SAND_OK.
    2. Call with queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expecte: NOT SAND_OK.
    3. Call with queue_type_params [NULL].
    Expected: SAND_NULL_POINTER_ERR.
    4. Call with is_vacant [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_QUEUE_TYPE        queue_type = FAP20M_QUEUE_2;
    FAP20M_QUEUE_TYPE_PARAM  queue_type_params;
    SAND_UINT                is_vacant = TRUE;


    /*
        1. Call with queue_type [FAP20M_QUEUE_2 / FAP20M_QUEUE_3], 
                     non-null queue_type_params, is_vacant.
        Expected: SAND_OK.
    */
    /* iterate with queue_type = FAP20M_QUEUE_2 */
    queue_type = FAP20M_QUEUE_2;

    result = fap20m_api_auto_queue_get_queue_params(queue_type, &queue_type_params, &is_vacant);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, queue_type);

    /* iterate with queue_type = FAP20M_QUEUE_3 */
    queue_type = FAP20M_QUEUE_3;

    result = fap20m_api_auto_queue_get_queue_params(queue_type, &queue_type_params, &is_vacant);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, queue_type);

    /*
        2. Call with queue_type [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expecte: NOT SAND_OK.
    */
    queue_type = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_queue_get_queue_params(queue_type, &queue_type_params, &is_vacant);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, queue_type);

    queue_type = FAP20M_QUEUE_3;

    /*
        3. Call with queue_type_params [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_queue_get_queue_params(queue_type, NULL, &is_vacant);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "queue_type_params = NULL");

    /*
        4. Call with is_vacant [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_queue_get_queue_params(queue_type, &queue_type_params, NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "is_vacant = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_queue_info_get_default(
    SAND_OUT FAP20M_AUTO_QUEUE_INFO *p_queue_info,
    SAND_IN  FAP20M_QUEUE_TYPE        queue_type_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_queue_info_get_default)
{
/*
    1. Call with queue_type_id [FAP20M_QUEUE_2 / FAP20M_QUEUE_3], 
                 non-null p_queue_info.
    Expected: SAND_OK.
    2. Call with queue_type_id [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expecte: NOT SAND_OK.
    3. Call with p_queue_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    FAP20M_AUTO_QUEUE_INFO p_queue_info;
    FAP20M_QUEUE_TYPE      queue_type_id = FAP20M_QUEUE_2;


    /*
        1. Call with queue_type_id [FAP20M_QUEUE_2 / FAP20M_QUEUE_3], 
                     non-null p_queue_info.
        Expected: SAND_OK.
    */
    /* iterate with queue_type_id = FAP20M_QUEUE_2 */
    queue_type_id = FAP20M_QUEUE_2;

    result = fap20m_api_auto_queue_info_get_default(&p_queue_info, queue_type_id);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "queue_type_id: %d", queue_type_id);

    /* iterate with queue_type_id = FAP20M_QUEUE_3 */
    queue_type_id = FAP20M_QUEUE_3;

    result = fap20m_api_auto_queue_info_get_default(&p_queue_info, queue_type_id);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "queue_type_id: %d", queue_type_id);

    /*
        2. Call with queue_type_id [0x5AAAAAA5] (out of range) and other params from 1.1.
        Expecte: NOT SAND_OK.
    */
    queue_type_id = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_queue_info_get_default(&p_queue_info, queue_type_id);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "queue_type_id: %d", queue_type_id);

    queue_type_id = FAP20M_QUEUE_3;

    /*
        3. Call with p_queue_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_queue_info_get_default(NULL, queue_type_id);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_queue_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_set_info_and_open_all_queues(
    SAND_IN  FAP20M_AUTO_QUEUE_INFO  *p_queues_info
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_set_info_and_open_all_queues)
{
/*
    1. Call with p_queues_info { queue_type [FAP20M_QUEUE_1, .. ,FAP20M_QUEUE_1 / 
                                             FAP20M_QUEUE_3, .. ,FAP20M_QUEUE_3] }.
    Expected: SAND_OK.
    2. Call with p_queues_info->queue_type[0] = 0x5AAAAAA5 (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    3. Call with p_queues_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_U32   index = 0;
    FAP20M_AUTO_QUEUE_INFO p_queues_info;


    /*
        1. Call with p_queues_info { queue_type [FAP20M_QUEUE_1, .. ,FAP20M_QUEUE_1 / 
                                                 FAP20M_QUEUE_3, .. ,FAP20M_QUEUE_3] }.
        Expected: SAND_OK.
    */
    /* iterate with FAP20M_QUEUE_1 */
    for(index = 0; index < FAP20M_FABRIC_UNICAST_CLASSES; index++)
    {
        p_queues_info.queue_type[index] = FAP20M_QUEUE_1;
    }

    result = fap20m_api_auto_set_info_and_open_all_queues(&p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* iterate with FAP20M_QUEUE_3 */
    for(index = 0; index < FAP20M_FABRIC_UNICAST_CLASSES; index++)
    {
        p_queues_info.queue_type[index] = FAP20M_QUEUE_3;
    }

    result = fap20m_api_auto_set_info_and_open_all_queues(&p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /*
        2. Call with p_queues_info->queue_type[0] = 0x5AAAAAA5 (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_queues_info.queue_type[0] = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_set_info_and_open_all_queues(&p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "p_queues_info.queue_type[0] = %d", p_queues_info.queue_type[0]);

    p_queues_info.queue_type[0] = FAP20M_QUEUE_3;

    /*
        3. Call with p_queues_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_set_info_and_open_all_queues(NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_queues_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_queues_get_info(
    SAND_IN  unsigned int              dest_fap_id,
    SAND_IN  unsigned int              dest_port_id,
    SAND_OUT FAP20M_AUTO_QUEUE_INFO  *p_queues_info
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_queues_get_info)
{
/*
    1. Call with dest_fap_id [0 / 31], 
                 dest_port_id [0 / 255], 
                 non-null p_queues_info.
    Expected: SAND_OK.
    2. Call with dest_fap_id [FAP20M_AUTO_MAX_NOF_FAPS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    3. Call with dest_port_id [FAP20M_NOF_DATA_PORTS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    4. Call with p_queues_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  dest_fap_id = 0;
    SAND_UINT  dest_port_id = 0;
    FAP20M_AUTO_QUEUE_INFO p_queues_info;


    /*
        1. Call with dest_fap_id [0 / 31], 
                     dest_port_id [0 / 255], 
                     non-null p_queues_info.
        Expected: SAND_OK.
    */
    /* iterate with dest_fap_id = 0 */
    dest_fap_id = 0;
    dest_port_id = 0;

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, dest_fap_id, dest_port_id);

    /* iterate with dest_fap_id = 31 */
    dest_fap_id = 31;
    dest_port_id = 255;

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, dest_fap_id, dest_port_id);

    /*
        2. Call with dest_fap_id [FAP20M_AUTO_MAX_NOF_FAPS] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    dest_fap_id = FAP20M_AUTO_MAX_NOF_FAPS;

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, dest_fap_id);

    dest_fap_id = 0;

    /*
        3. Call with dest_port_id [FAP20M_NOF_DATA_PORTS] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    dest_port_id = FAP20M_AUTO_MAX_NOF_FAPS;

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "dest_port_id = %d", dest_port_id);

    dest_port_id = 0;

    /*
        4. Call with p_queues_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_queues_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_update_queues(
    SAND_IN  unsigned int              dest_fap_id,   
    SAND_IN  unsigned int              dest_port_id,  
    SAND_IN  FAP20M_AUTO_QUEUE_INFO  *p_queues_info
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_update_queues)
{
/*
    1. Call with dest_fap_id [0 / 31], 
                   dest_port_id [0 / 255], 
                   p_queues_info { queue_type [FAP20M_QUEUE_1, .. ,FAP20M_QUEUE_1 / 
                                             FAP20M_QUEUE_3, .. ,FAP20M_QUEUE_3] }.
    Expected: SAND_OK.
    2. Call with dest_fap_id [FAP20M_AUTO_MAX_NOF_FAPS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    3. Call with dest_port_id [FAP20M_NOF_DATA_PORTS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    4. Call with p_queues_info->queue_type[0] = 0x5AAAAAA5 (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    5. Call with p_queues_info [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_U32   index = 0;
    SAND_UINT  dest_fap_id = 0;
    SAND_UINT  dest_port_id = 0;
    FAP20M_AUTO_QUEUE_INFO p_queues_info;


    /*
        1. Call with dest_fap_id [0 / 31], 
                     dest_port_id [0 / 255], 
                     p_queues_info { queue_type [FAP20M_QUEUE_1, .. ,FAP20M_QUEUE_1 / 
                                                 FAP20M_QUEUE_3, .. ,FAP20M_QUEUE_3] }.
        Expected: SAND_OK.
    */
    /* iterate with dest_fap_id = 0 */
    dest_fap_id = 0;
    dest_port_id = 0;

    for(index = 0; index < FAP20M_FABRIC_UNICAST_CLASSES; index++)
    {
        p_queues_info.queue_type[index] = FAP20M_QUEUE_1;
    }

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, dest_fap_id, dest_port_id);

    /* iterate with dest_fap_id = 31 */
    dest_fap_id = 31;
    dest_port_id = 255;

    for(index = 0; index < FAP20M_FABRIC_UNICAST_CLASSES; index++)
    {
        p_queues_info.queue_type[index] = FAP20M_QUEUE_3;
    }

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, dest_fap_id, dest_port_id);

    /*
        2. Call with dest_fap_id [FAP20M_AUTO_MAX_NOF_FAPS] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    dest_fap_id = FAP20M_AUTO_MAX_NOF_FAPS;

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, dest_fap_id);

    dest_fap_id = 0;

    /*
        3. Call with dest_port_id [FAP20M_NOF_DATA_PORTS] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    dest_port_id = FAP20M_AUTO_MAX_NOF_FAPS;

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "dest_port_id = %d", dest_port_id);

    dest_port_id = 0;

    /*
        4. Call with p_queues_info->queue_type[0] = 0x5AAAAAA5 (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    p_queues_info.queue_type[0] = SAND_UTF_API_AUTO_INVALID_ENUM;

    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "p_queues_info.queue_type[0] = %d", p_queues_info.queue_type[0]);

    p_queues_info.queue_type[0] = FAP20M_QUEUE_3;

    /*
        5. Call with p_queues_info [NULL].
        Expected: SAND_NULL_POINTER_ERR.
    */
    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, NULL);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL0_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "p_queues_info = NULL");
}

/*----------------------------------------------------------------------------*/
/*
unsigned int
  fap20m_api_auto_close_queues(
    SAND_IN  unsigned int dest_fap_id,
    SAND_IN  unsigned int dest_port_id
  )
*/
UTF_TEST_CASE_MAC(fap20m_api_auto_close_queues)
{
/*
    1. Call with dest_fap_id [0 / 31], 
                   dest_port_id [0 / 255].
    Expected: SAND_OK.
    2. Call with dest_fap_id [FAP20M_AUTO_MAX_NOF_FAPS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    3. Call with dest_port_id [FAP20M_NOF_DATA_PORTS] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT dest_fap_id = 0;
    SAND_UINT dest_port_id = 0;


    /*
        1. Call with dest_fap_id [0 / 31], 
                     dest_port_id [0 / 255], 
        Expected: SAND_OK.
    */
    /* iterate with dest_fap_id = 0 */
    dest_fap_id = 0;
    dest_port_id = 0;

    result = fap20m_api_auto_close_queues(dest_fap_id, dest_port_id);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, dest_fap_id, dest_port_id);

    /* iterate with dest_fap_id = 31 */
    dest_fap_id = 31;
    dest_port_id = 255;

    result = fap20m_api_auto_close_queues(dest_fap_id, dest_port_id);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, dest_fap_id, dest_port_id);

    /*
        2. Call with dest_fap_id [FAP20M_AUTO_MAX_NOF_FAPS] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    dest_fap_id = FAP20M_AUTO_MAX_NOF_FAPS;

    result = fap20m_api_auto_close_queues(dest_fap_id, dest_port_id);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, dest_fap_id);

    dest_fap_id = 0;

    /*
        3. Call with dest_port_id [FAP20M_NOF_DATA_PORTS] (out of range) and other params from 1.1.
        Expected: NOT SAND_OK.
    */
    dest_port_id = FAP20M_AUTO_MAX_NOF_FAPS;

    result = fap20m_api_auto_close_queues(dest_fap_id, dest_port_id);

    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(SAND_OK, rc, "dest_port_id = %d", dest_port_id);

}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_auto_management suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_auto_management)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_system_info_get_defaults)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_init)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_port_get_default)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_set_info_and_open_all_ports)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_port_get_info)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_update_port)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_close_port)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_aggregates_get_default)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_set_info_and_open_all_aggregates)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_port_aggregates_get_info)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_update_port_aggregates)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_close_port_aggregates)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_flows_get_default)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_set_info_and_open_all_flows)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_port_flows_get_info)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_update_port_flows)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_close_port_flows)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_queue_type_params_get_default)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_get_next_vacant_queue_type_id)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_queue_set_queue_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_queue_get_queue_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_queue_info_get_default)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_set_info_and_open_all_queues)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_queues_get_info)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_update_queues)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_api_auto_close_queues)
UTF_SUIT_END_TESTS_MAC(fap20m_api_auto_management)

