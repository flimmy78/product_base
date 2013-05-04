/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_routingUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_routing, that provides
*       links API of FAP20M device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_routing.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_unicast_distribution_table(
    unsigned int            device_id,
    FAP20M_DIST_UNI_REPORT* uni_report
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_unicast_distribution_table)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null uni_report. 
    Expected: SAND_OK.
    1.2. Call with uni_report [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_DIST_UNI_REPORT uni_report;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    uni_report.offset = 0;
    uni_report.num_entries = 1;

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /* 
            1.1. Call with non-null uni_report.
            Expected: SAND_OK.
        */
        result = fap20m_get_unicast_distribution_table(device_id, &uni_report);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with uni_report [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_unicast_distribution_table(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, uni_report = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_unicast_distribution_table(device_id, &uni_report);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_unicast_distribution_table(device_id, &uni_report);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_spatial_multicast_distribution_table(
    unsigned int     device_id,      
    unsigned long*   result_ptr,      
    unsigned int     start_table_entry,      
    unsigned int     nof_table_entries 
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_spatial_multicast_distribution_table)
{
/*
    ITERATE_DEVICES
    1.1. Call with start_table_entry [0 / 2046], nof_table_entries [2047 / 1], non-null result_ptr. 
    Expected: SAND_OK.
    1.2. Call with start_table_entry [2047] and nof_table_entries [2] (out of range)
    Expected: NOT SAND_OK.
    1.3. Call with result_ptr [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_U32   table [2048];
    SAND_UINT  start_table_entry = 0;
    SAND_UINT  nof_table_entries = 0;
    

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with start_table_entry [0 / 2046], 
                           nof_table_entries [2047 / 1], 
                           non-null result_ptr. 
            Expected: SAND_OK.
        */
        /* iterate with start_table_entry = 0 */
        start_table_entry = 0;
        nof_table_entries = 2047;

        result = fap20m_get_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /* iterate with start_table_entry = 2046 */
        start_table_entry = 2046;
        nof_table_entries = 1;

        result = fap20m_get_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. CAll with start_table_entry [2047] and nof_table_entries [2] (out of range)
            Expected: NOT SAND_OK.
        */
        start_table_entry = 2047;
        nof_table_entries = 2;

        result = fap20m_get_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, start_table_entry = %d, nof_table_entries = %d", 
                                        device_id, start_table_entry, nof_table_entries);

        start_table_entry = 0;
        nof_table_entries = 2047;

        /*
            1.3. Call with result_ptr [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_spatial_multicast_distribution_table(device_id, NULL, start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, table = NULL", device_id);
    }

    start_table_entry = 0;
    nof_table_entries = 2047;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                             start_table_entry, nof_table_entries);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_spatial_multicast_distribution_table(
    unsigned int     device_id,      
    unsigned long*   table,      
    unsigned int     start_table_entry,      
    unsigned int     nof_table_entries 
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_spatial_multicast_distribution_table)
{
/*
    ITERATE_DEVICES
    1.1. Call with start_table_entry [0 / 2046], 
                   nof_table_entries [2047 / 1], 
                   table [0,0..0 / 0xFF,0xFF..0xFF]. 
    Expected: SAND_OK.
    1.2. Call fap20m_get_spatial_multicast_distribution_table with not NULL table and other params from 1.1.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with start_table_entry [2047] and nof_table_entries [2] (out of range)
    Expected: NOT SAND_OK.
    1.4. Call with table [NULL]. 
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  index = 0;
    SAND_U32   table [2048];
    SAND_U32   tableGet [2048];
    SAND_UINT  start_table_entry = 0;
    SAND_UINT  nof_table_entries = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with start_table_entry [0 / 2046], 
                           nof_table_entries [2047 / 1], 
                           table [0,0..0 / 0xFF,0xFF..0xFF]. 
            Expected: SAND_OK.
        */
        /* iterate with start_table_entry = 0 */
        start_table_entry = 0;
        nof_table_entries = 2048;

        for(index = 0; index < 2048; index++)
        {
            table[index] = 0;
        }

        result = fap20m_set_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_spatial_multicast_distribution_table with not NULL table and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_spatial_multicast_distribution_table(device_id, (unsigned long*) tableGet,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_spatial_multicast_distribution_table: %d", device_id);

        for(index = 0; index < 2048; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(table[index], tableGet[index],
                       "got another table[%d] than was set: %d", index, device_id);
        }

        /* iterate with start_table_entry = 2046 */
        start_table_entry = 2046;
        nof_table_entries = 1;

        table[0] = 0xFF;
        
        result = fap20m_set_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_spatial_multicast_distribution_table with not NULL table and other params from 1.1.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_get_spatial_multicast_distribution_table(device_id, (unsigned long*) tableGet,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_spatial_multicast_distribution_table: %d", device_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(table[0], tableGet[0],
                   "got another table[2046] than was set: %d", device_id);
        
        /*
            1.3. Call with start_table_entry [2047] and nof_table_entries [2] (out of range)
            Expected: NOT SAND_OK.
        */
        start_table_entry = 2047;
        nof_table_entries = 2;

        result = fap20m_set_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(SAND_OK, rc, "%d, start_table_entry = %d, nof_table_entries = %d", 
                                         device_id, start_table_entry, nof_table_entries);

        start_table_entry = 0;
        nof_table_entries = 2047;

        /*
            1.4. Call with table [NULL]. 
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_spatial_multicast_distribution_table(device_id, NULL, start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, result = NULL", device_id);
    }

    start_table_entry = 0;
    nof_table_entries = 2047;

    for(index = 0; index < 2048; index++)
    {
        table[index] = 0;
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                                 start_table_entry, nof_table_entries);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_spatial_multicast_distribution_table(device_id, (unsigned long*) table,
                                                             start_table_entry, nof_table_entries);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_routing suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_routing)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_unicast_distribution_table)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_spatial_multicast_distribution_table)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_spatial_multicast_distribution_table)
UTF_SUIT_END_TESTS_MAC(fap20m_api_routing)


