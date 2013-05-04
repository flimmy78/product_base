/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_spatial_multicastUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_spatial_multicast
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_spatial_multicast.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_SMS_configuration(
      SAND_IN  unsigned int       device_id,
      SAND_IN  FAP20M_SMS_CONF* sms_conf,
      SAND_OUT FAP20M_SMS_CONF* exact_sms_conf_given
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_SMS_configuration)
{
/*
    ITERATE_DEVICES
    1.1. Call with sms_conf { guaranteed_rate [0 / 100], 
                              guaranteed_burst_size [0 / 100],
                              best_effort_rate [0 / 100],
                              best_effort_burst_size [0 / 100],
                              sync_with_unicast_priorities [0 / 100] }, 
                   non-null exact_sms_conf_given. 
    Expected: SAND_OK.
    1.2. Call fap20m_get_SMS_configuration with not NULL sms_conf and other params from 1.1.
    Expected: SAND_OK and the same sms_conf as was set.
    1.3. Call with sms_conf [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
    1.4. Call with exact_sms_conf_given [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;

    FAP20M_SMS_CONF sms_conf;
    FAP20M_SMS_CONF sms_confGet;
    FAP20M_SMS_CONF exact_sms_conf_given;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with sms_conf { guaranteed_rate [0 / 100], 
                                      guaranteed_burst_size [0 / 100],
                                      best_effort_rate [0 / 100],
                                      best_effort_burst_size [0 / 100],
                                      sync_with_unicast_priorities [0 / 100] }, 
                           non-null exact_sms_conf_given. 
            Expected: SAND_OK.
        */
        /* iterate with 0 */
        sms_conf.guaranteed_rate = 0;
        sms_conf.best_effort_burst_size = 0;
        sms_conf.best_effort_rate = 0;
        sms_conf.best_effort_burst_size = 0;
        sms_conf.sync_with_unicast_priorities = 0;

        result = fap20m_set_SMS_configuration(device_id, &sms_conf, &exact_sms_conf_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_SMS_configuration with not NULL sms_conf and other params from 1.1.
            Expected: SAND_OK and the same sms_conf as was set.
        */
        result = fap20m_get_SMS_configuration(device_id, &sms_confGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_SMS_configuration: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.guaranteed_rate, sms_confGet.guaranteed_rate, 
                                     "got another sms_conf.guaranteed_rate than wa set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.best_effort_burst_size, sms_confGet.best_effort_burst_size, 
                                     "got another sms_conf.best_effort_burst_size than wa set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.best_effort_rate, sms_confGet.best_effort_rate, 
                                     "got another sms_conf.best_effort_rate than wa set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.best_effort_burst_size, sms_confGet.best_effort_burst_size, 
                                     "got another sms_conf.best_effort_burst_size than wa set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.sync_with_unicast_priorities, sms_confGet.sync_with_unicast_priorities, 
                                     "got another sms_conf.sync_with_unicast_priorities than wa set: %d", device_id);

        /* iterate with 100 */
        sms_conf.guaranteed_rate = 100;
        sms_conf.best_effort_burst_size = 100;
        sms_conf.best_effort_rate = 100;
        sms_conf.best_effort_burst_size = 100;
        sms_conf.sync_with_unicast_priorities = 100;

        result = fap20m_set_SMS_configuration(device_id, &sms_conf, &exact_sms_conf_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_SMS_configuration with not NULL sms_conf and other params from 1.1.
            Expected: SAND_OK and the same sms_conf as was set.
        */
        result = fap20m_get_SMS_configuration(device_id, &sms_confGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_SMS_configuration: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.guaranteed_rate, sms_confGet.guaranteed_rate, 
                                     "got another sms_conf.guaranteed_rate than wa set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.best_effort_burst_size, sms_confGet.best_effort_burst_size, 
                                     "got another sms_conf.best_effort_burst_size than wa set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.best_effort_rate, sms_confGet.best_effort_rate, 
                                     "got another sms_conf.best_effort_rate than wa set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.best_effort_burst_size, sms_confGet.best_effort_burst_size, 
                                     "got another sms_conf.best_effort_burst_size than wa set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(exact_sms_conf_given.sync_with_unicast_priorities, sms_confGet.sync_with_unicast_priorities, 
                                     "got another sms_conf.sync_with_unicast_priorities than wa set: %d", device_id);

        /*
            1.3. Call with sms_conf [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_SMS_configuration(device_id, NULL, &exact_sms_conf_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sms_conf = NULL", device_id);

        /*
            1.4. Call with exact_sms_conf_given [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_SMS_configuration(device_id, &sms_conf, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, exact_sms_conf_given = NULL", device_id);
    }

    sms_conf.guaranteed_rate = 0;
    sms_conf.best_effort_burst_size = 0;
    sms_conf.best_effort_rate = 0;
    sms_conf.best_effort_burst_size = 0;
    sms_conf.sync_with_unicast_priorities = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_SMS_configuration(device_id, &sms_conf, &exact_sms_conf_given);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_SMS_configuration(device_id, &sms_conf, &exact_sms_conf_given);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_SMS_configuration(
      SAND_IN  unsigned int     device_id,
      SAND_OUT FAP20M_SMS_CONF* sms_conf
  );
*/
UTF_TEST_CASE_MAC(fap20m_get_SMS_configuration)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null sms_conf. 
    Expected: SAND_OK.
    1.2. Call with sms_conf [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_SMS_CONF sms_conf;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with non-null sms_conf.
            Expected: SAND_OK.
        */
        result = fap20m_get_SMS_configuration(device_id, &sms_conf);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with sms_conf [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_SMS_configuration(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sms_conf = NULL", device_id);
    }

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_SMS_configuration(device_id, &sms_conf);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_SMS_configuration(device_id, &sms_conf);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_open_spatial_multicast_flow(
      unsigned int                   device_id,
      unsigned int                   multicast_id,
      FAP20M_SPATIAL_MULTICAST_FLOW* sp_flow
  ) ;
*/
UTF_TEST_CASE_MAC(fap20m_open_spatial_multicast_flow)
{
/*
    ITERATE_DEVICES
    1.1. Call with multicast_id [0 / 8191], sp_flow { spatial_distribution_map [0 / 0xFFFFFF],
                                                      spatial_distribution_map_op [SAND_OP_AND / SAND_OP_OR],
                                                      links_active_mask [0 / 0xFFFFFF] }.
    Expected: SAND_OK.
    1.2. Call fap20m_get_spatial_multicast_flow_params with not NULL sp_flow and other params from 1.1.
    Expected: SAND_OK and the same sp_flow as was set.
    1.3. Call with multicast_id [8192] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with sp_flow [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  multicast_id = 0;

    FAP20M_SPATIAL_MULTICAST_FLOW sp_flow;
    FAP20M_SPATIAL_MULTICAST_FLOW sp_flowGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with multicast_id [0 / 8191], sp_flow { spatial_distribution_map [0 / 0xFFFFFF],
                                                              spatial_distribution_map_op [SAND_OP_AND / SAND_OP_OR],
                                                              links_active_mask [0 / 0xFFFFFF] }.
            Expected: SAND_OK.
        */
        /* iterate with multicast_id = 0 */
        multicast_id = 0;

        sp_flow.spatial_distribution_map = 0;
        sp_flow.spatial_distribution_map_op = SAND_OP_AND;
        sp_flow.links_active_mask = 0;

        result = fap20m_open_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        /*
            1.2. Call fap20m_get_spatial_multicast_flow_params with not NULL sp_flow and other params from 1.1.
            Expected: SAND_OK and the same sp_flow as was set.
        */
        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flowGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_spatial_multicast_flow_params: %d, %d", device_id, multicast_id);

        /* Verifying values - only spatial_distribution_map because:
            - links_active_mask - is relevant for Get only
            - spatial_distribution_map_op - is not relevant for Get */
        UTF_VERIFY_EQUAL1_STRING_MAC(sp_flow.spatial_distribution_map, sp_flowGet.spatial_distribution_map, 
                                     "got another sp_flow.spatial_distribution_map than was set: %d", device_id);
        
        /* iterate with multicast_id = 8191 */
        multicast_id = 8191;

        sp_flow.spatial_distribution_map = 0xFFFFFF;
        sp_flow.spatial_distribution_map_op = SAND_OP_OR;
        sp_flow.links_active_mask = 0xFFFFFF;

        result = fap20m_open_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        /*
            1.2. Call fap20m_get_spatial_multicast_flow_params with not NULL sp_flow and other params from 1.1.
            Expected: SAND_OK and the same sp_flow as was set.
        */
        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flowGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_spatial_multicast_flow_params: %d, %d", device_id, multicast_id);

        /* Verifying values - only spatial_distribution_map because:
            - links_active_mask - is relevant for Get only
            - spatial_distribution_map_op - is not relevant for Get */
        UTF_VERIFY_EQUAL1_STRING_MAC(sp_flow.spatial_distribution_map, sp_flowGet.spatial_distribution_map, 
                                     "got another sp_flow.spatial_distribution_map than was set: %d", device_id);

        /*
            1.3. Call with multicast_id [8192] (out of range) and other params from 1.1.
            Expected: SAND_OK becuase driver permits such values and actually it configures
                      multicast entry with index multicast_id % 8K
        */
        multicast_id = 8192;

        result = fap20m_open_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        multicast_id = 8191;

        /*
            1.4. Call with sp_flow [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_open_spatial_multicast_flow(device_id, multicast_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sp_flow = NULL", device_id);        
    }

    multicast_id = 0;

    sp_flow.spatial_distribution_map = 0;
    sp_flow.spatial_distribution_map_op = SAND_OP_AND;
    sp_flow.links_active_mask = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_open_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_open_spatial_multicast_flow(device_id, multicast_id, &sp_flow);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_update_spatial_multicast_flow(
    unsigned int                   device_id,
    unsigned int                   multicast_id,
    FAP20M_SPATIAL_MULTICAST_FLOW* sp_flow
  ) ;
*/
UTF_TEST_CASE_MAC(fap20m_update_spatial_multicast_flow)
{
/*
    ITERATE_DEVICES
    1.1. Call with multicast_id [0 / 8191], sp_flow { spatial_distribution_map [0 / 0xFFFFFF],
                                                      spatial_distribution_map_op [SAND_OP_AND / SAND_OP_OR],
                                                      links_active_mask [0 / 0xFFFFFF] }.
    Expected: SAND_OK.
    1.2. Call fap20m_get_spatial_multicast_flow_params with not NULL sp_flow and other params from 1.1.
    Expected: SAND_OK and the same sp_flow as was set.
    1.3. Call with multicast_id [8192] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with sp_flow [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  multicast_id = 0;

    FAP20M_SPATIAL_MULTICAST_FLOW sp_flow;
    FAP20M_SPATIAL_MULTICAST_FLOW sp_flowGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with multicast_id [0 / 8191], sp_flow { spatial_distribution_map [0 / 0xFFFFFF],
                                                              spatial_distribution_map_op [SAND_OP_AND / SAND_OP_OR],
                                                              links_active_mask [0 / 0xFFFFFF] }.
            Expected: SAND_OK.
        */
        /* iterate with multicast_id = 0 */
        multicast_id = 0;

        sp_flow.spatial_distribution_map = 0;
        sp_flow.spatial_distribution_map_op = SAND_OP_AND;
        sp_flow.links_active_mask = 0;

        result = fap20m_update_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        /*
            1.2. Call fap20m_get_spatial_multicast_flow_params with not NULL sp_flow and other params from 1.1.
            Expected: SAND_OK and the same sp_flow as was set.
        */
        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flowGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_spatial_multicast_flow_params: %d, %d", device_id, multicast_id);

        /* Verifying values - only spatial_distribution_map because:
            - links_active_mask - is relevant for Get only
            - spatial_distribution_map_op - is not relevant for Get */
        UTF_VERIFY_EQUAL1_STRING_MAC(sp_flow.spatial_distribution_map, sp_flowGet.spatial_distribution_map, 
                                     "got another sp_flow.spatial_distribution_map than was set: %d", device_id);
        
        /* iterate with multicast_id = 8191 */
        multicast_id = 8191;

        sp_flow.spatial_distribution_map = 0xFFFFFF;
        sp_flow.spatial_distribution_map_op = SAND_OP_OR;
        sp_flow.links_active_mask = 0xFFFFFF;

        result = fap20m_update_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        /*
            1.2. Call fap20m_get_spatial_multicast_flow_params with not NULL sp_flow and other params from 1.1.
            Expected: SAND_OK and the same sp_flow as was set.
        */
        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flowGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_spatial_multicast_flow_params: %d, %d", device_id, multicast_id);

        /* Verifying values - only spatial_distribution_map because:
            - links_active_mask - is relevant for Get only
            - spatial_distribution_map_op - is not relevant for Get */
        UTF_VERIFY_EQUAL1_STRING_MAC(sp_flow.spatial_distribution_map, sp_flowGet.spatial_distribution_map, 
                                     "got another sp_flow.spatial_distribution_map than was set: %d", device_id);

        /*
            1.3. Call with multicast_id [8192] (out of range) and other params from 1.1.
            Expected: SAND_OK becuase driver permits such values and actually it configures
                      multicast entry with index multicast_id % 8K
        */
        multicast_id = 8192;

        result = fap20m_update_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        multicast_id = 8191;

        /*
            1.4. Call with sp_flow [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_update_spatial_multicast_flow(device_id, multicast_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sp_flow = NULL", device_id);        
    }

    multicast_id = 0;

    sp_flow.spatial_distribution_map = 0;
    sp_flow.spatial_distribution_map_op = SAND_OP_AND;
    sp_flow.links_active_mask = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_update_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_update_spatial_multicast_flow(device_id, multicast_id, &sp_flow);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_spatial_multicast_flow_params(
    unsigned int                    device_id,
    unsigned int                    multicast_id,
    FAP20M_SPATIAL_MULTICAST_FLOW*  sp_flow
  ) ;
*/
UTF_TEST_CASE_MAC(fap20m_get_spatial_multicast_flow_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with multicast_id [0 / 8191], not NULL sp_flow.
    Expected: SAND_OK.
    1.2. Call with multicast_id [8192] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with sp_flow [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  multicast_id = 0;
    FAP20M_SPATIAL_MULTICAST_FLOW sp_flow;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with multicast_id [0 / 8191], not NULL sp_flow.
            Expected: SAND_OK.
        */
        /* iterate with multicast_id = 0 */
        multicast_id = 0;

        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        /* iterate with multicast_id = 8191 */
        multicast_id = 8191;

        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flow);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        /*
            1.2. Call with multicast_id [8192] (out of range) and other params from 1.1.
            Expected: SAND_OK becuase driver permits such values and actually it configures
                      multicast entry with index multicast_id % 8K
        */
        multicast_id = 8192;

        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        multicast_id = 8191;

        /*
            1.3. Call with sp_flow [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, sp_flow = NULL", device_id);
    }

    multicast_id = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flow);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flow);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_close_spatial_multicast_flow(
    unsigned int     device_id,
    unsigned int     multicast_id
  ) ;
*/
UTF_TEST_CASE_MAC(fap20m_close_spatial_multicast_flow)
{
/*
    ITERATE_DEVICES
    1.1. Call with multicast_id [0 / 8191].
    Expected: SAND_OK.
    1.2. Call with multicast_id [8192] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  multicast_id = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with multicast_id [0 / 8191].
            Expected: SAND_OK.
        */
        /* iterate with multicast_id = 0 */
        multicast_id = 0;

        result = fap20m_close_spatial_multicast_flow(device_id, multicast_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        /* iterate with multicast_id = 8191 */
        multicast_id = 8191;

        result = fap20m_close_spatial_multicast_flow(device_id, multicast_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);

        /*
            1.2. Call with multicast_id [8192] (out of range).
            Expected: SAND_OK becuase driver permits such values and actually it configures
                      multicast entry with index multicast_id % 8K
        */
        multicast_id = 8192;

        result = fap20m_close_spatial_multicast_flow(device_id, multicast_id);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, multicast_id);
    }

    multicast_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_close_spatial_multicast_flow(device_id, multicast_id);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_close_spatial_multicast_flow(device_id, multicast_id);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_spatial_multicast_active_links_get(
    SAND_IN  unsigned int     device_id,      
    SAND_OUT unsigned long*   links_bitmap
  ) ;
*/
UTF_TEST_CASE_MAC(fap20m_spatial_multicast_active_links_get)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null links_bitmap. 
    Expected: SAND_OK.
    1.2. Call with links_bitmap [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   links_bitmap = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with non-null links_bitmap.
            Expected: SAND_OK.
        */
        
        result = fap20m_spatial_multicast_active_links_get(device_id, &links_bitmap);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with links_bitmap [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_spatial_multicast_active_links_get(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, links_bitmap = NULL", device_id);
    }

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_spatial_multicast_active_links_get(device_id, &links_bitmap);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_spatial_multicast_active_links_get(device_id, &links_bitmap);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_spatial_multicast_active_links_set(
    SAND_IN  unsigned int     device_id,      
    SAND_IN  unsigned long    links_bitmap
  ) ;
*/
UTF_TEST_CASE_MAC(fap20m_spatial_multicast_active_links_set)
{
/*
    ITERATE_DEVICES
    1.1. Call with links_bitmap [0 / 0xFFFFFF].
    Expected: SAND_OK.
    1.2. Call fap20m_spatial_multicast_active_links_get with not NULL links_bitmap.
    Expected: SAND_OK and the same links_bitmap as was set.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   links_bitmap = 0;
    SAND_U32   links_bitmapGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with links_bitmap [0 / 0xFFFFFF].
            Expected: SAND_OK.
        */
        /* iterate with links_bitmap = 0 */
        links_bitmap = 0;

        result = fap20m_spatial_multicast_active_links_set(device_id, links_bitmap);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, links_bitmap);

        /*
            1.2. Call fap20m_spatial_multicast_active_links_get with not NULL links_bitmap.
            Expected: SAND_OK and the same links_bitmap as was set.
        */
        result = fap20m_spatial_multicast_active_links_get(device_id, &links_bitmapGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_spatial_multicast_active_links_get: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(links_bitmap, links_bitmapGet,
                                     "got another links_bitmap than wa sset: %d", device_id);

        /* iterate with links_bitmap = 0xFFFFFF */
        links_bitmap = 0xFFFFFF;

        result = fap20m_spatial_multicast_active_links_set(device_id, links_bitmap);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, links_bitmap);

        /*
            1.2. Call fap20m_spatial_multicast_active_links_get with not NULL links_bitmap.
            Expected: SAND_OK and the same links_bitmap as was set.
        */
        result = fap20m_spatial_multicast_active_links_get(device_id, &links_bitmapGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_spatial_multicast_active_links_get: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(links_bitmap, links_bitmapGet,
                                     "got another links_bitmap than wa sset: %d", device_id);
    }

    links_bitmap = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_spatial_multicast_active_links_set(device_id, links_bitmap);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_spatial_multicast_active_links_set(device_id, links_bitmap);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_spatial_multicast suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_spatial_multicast)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_SMS_configuration)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_SMS_configuration)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_open_spatial_multicast_flow)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_update_spatial_multicast_flow)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_spatial_multicast_flow_params)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_close_spatial_multicast_flow)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_spatial_multicast_active_links_get)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_spatial_multicast_active_links_set)
UTF_SUIT_END_TESTS_MAC(fap20m_api_spatial_multicast)

