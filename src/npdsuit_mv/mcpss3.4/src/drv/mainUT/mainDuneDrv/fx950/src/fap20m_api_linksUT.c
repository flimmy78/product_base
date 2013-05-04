/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fap20m_api_linksUT.c
*
* DESCRIPTION:
*       Unit tests for fap20m_api_links, that provides
*       links API of FAP20M device driver.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <FX950/include/fap20m_api_links.h>

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <utf/utfMain.h>
#include <utf/private/prvDuneUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_links_status(
    unsigned int            device_id,
    FAP20M_ALL_LINK_STATUS* all_link_status,
    unsigned long           list_of_links
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_links_status)
{
/*
    ITERATE_DEVICES
    1.1. Call with list_of_links [0 / 0xFF], non-null all_link_status.
    Expected: SAND_OK.
    1.2. Call with all_link_status [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_ALL_LINK_STATUS all_link_status[8];
    SAND_U32 list_of_links = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with list_of_links [0 / 0xFF], non-null all_link_status.
            Expected: SAND_OK.
        */
        /* iterate with list_of_links = 0 */
        list_of_links = 0;

        result = fap20m_get_links_status(device_id, all_link_status, list_of_links);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /* iterate with list_of_links = 0xFF */
        list_of_links = 0xFF;

        result = fap20m_get_links_status(device_id, all_link_status, list_of_links);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with all_link_status [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_links_status(device_id, NULL, list_of_links);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_link_status = NULL", device_id);
    }

    list_of_links = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_links_status(device_id, all_link_status, list_of_links);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_links_status(device_id, all_link_status, list_of_links);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_connectivity_map(
    unsigned int                  device_id,
    FAP20M_ALL_LINK_CONNECTIVITY* all_link_connectivity
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_connectivity_map)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null all_link_connectivity.
    Expected: SAND_OK.
    1.2. Call with all_link_connectivity [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_ALL_LINK_CONNECTIVITY all_link_connectivity;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null all_link_connectivity.
            Expected: SAND_OK.
        */
        result = fap20m_get_connectivity_map(device_id, &all_link_connectivity);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with all_link_connectivity [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_connectivity_map(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_link_connectivity = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_connectivity_map(device_id, &all_link_connectivity);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_connectivity_map(device_id, &all_link_connectivity);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_serdes_params(
    unsigned int              device_id,
    FAP20M_ALL_SERDES_STATUS* all_serdes_status
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_serdes_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null all_serdes_status.
    Expected: SAND_OK.
    1.2. Call with all_serdes_status [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_ALL_SERDES_STATUS all_serdes_status;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null all_serdes_status.
            Expected: SAND_OK.
        */
        result = fap20m_get_serdes_params(device_id, &all_serdes_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with all_serdes_status [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_serdes_params(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_serdes_status = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_serdes_params(device_id, &all_serdes_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_serdes_params(device_id, &all_serdes_status);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_serdes_params(
    unsigned int              device_id,
    FAP20M_ALL_SERDES_STATUS* all_serdes_status
  )
*/
#if 0
UTF_TEST_CASE_MAC(fap20m_set_serdes_params)
{
/*
    ITERATE_DEVICES
    1.1. Call with all_serdes_status { single_serdes_status [] { valid [TRUE / FALSE],
                                                                 driver_tx [0 / 0xF],
                                                                 driver_eq [0 / 0xF],
                                                                 hi_drv [0 / 1],
                                                                 low_drv [0 / 1],
                                                                 single_term [0 / 1],
                                                                 local_loopback [0 / 1] }
    Expected: SAND_OK.
    1.2. Call fap20m_get_serdes_params with not NULL all_serdes_status.
    Expected: SAND_OK and the same all_serdes_status as was set.
    1.3. Call with all_serdes_status [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  index = 0;
    FAP20M_ALL_SERDES_STATUS all_serdes_status;
    FAP20M_ALL_SERDES_STATUS all_serdes_statusGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with all_serdes_status { single_serdes_status [] { valid [TRUE / FALSE],
                                                                         driver_tx [0 / 0xF],
                                                                         driver_eq [0 / 0xF],
                                                                         hi_drv [0 / 1],
                                                                         low_drv [0 / 1],
                                                                         single_term [0 / 1],
                                                                         local_loopback [0 / 1] }
            Expected: SAND_OK.
        */
        /* iterate with valid = TRUE */
        for(index = 0; index < FAP20M_NUMBER_OF_LINKS; index++)
        {
            all_serdes_status.single_serdes_status[index].valid = TRUE;
            all_serdes_status.single_serdes_status[index].driver_tx = 0;
            all_serdes_status.single_serdes_status[index].driver_eq = 0;
            all_serdes_status.single_serdes_status[index].hi_drv = 0;
            all_serdes_status.single_serdes_status[index].low_drv = 0;
            all_serdes_status.single_serdes_status[index].single_term = 0;
            all_serdes_status.single_serdes_status[index].local_loopback = 0;
        }

        result = fap20m_set_serdes_params(device_id, &all_serdes_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_serdes_params with not NULL all_serdes_status.
            Expected: SAND_OK and the same all_serdes_status as was set.
        */
        result = fap20m_get_serdes_params(device_id, &all_serdes_statusGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_serdes_params: %d", device_id);

        /* verifying values */
        for(index = 0; index < FAP20M_NUMBER_OF_LINKS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].valid, 
                                         all_serdes_statusGet.single_serdes_status[index].valid, 
                                         "all_serdes_status.single_serdes_status[%d].valid: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].driver_tx, 
                                         all_serdes_statusGet.single_serdes_status[index].driver_tx, 
                                         "all_serdes_status.single_serdes_status[%d].driver_tx: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].driver_eq, 
                                         all_serdes_statusGet.single_serdes_status[index].driver_eq, 
                                         "all_serdes_status.single_serdes_status[%d].driver_eq: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].hi_drv, 
                                         all_serdes_statusGet.single_serdes_status[index].hi_drv, 
                                         "all_serdes_status.single_serdes_status[%d].hi_drv: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].low_drv, 
                                         all_serdes_statusGet.single_serdes_status[index].low_drv, 
                                         "all_serdes_status.single_serdes_status[%d].low_drv: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].single_term, 
                                         all_serdes_statusGet.single_serdes_status[index].single_term, 
                                         "all_serdes_status.single_serdes_status[%d].single_term: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].local_loopback, 
                                         all_serdes_statusGet.single_serdes_status[index].local_loopback, 
                                         "all_serdes_status.single_serdes_status[%d].local_loopback: %d", index, device_id);
        }

        /* iterate with valid = FALSE */
        for(index = 0; index < FAP20M_NUMBER_OF_LINKS; index++)
        {
            all_serdes_status.single_serdes_status[index].valid = FALSE;
            all_serdes_status.single_serdes_status[index].driver_tx = 0xF;
            all_serdes_status.single_serdes_status[index].driver_eq = 0xF;
            all_serdes_status.single_serdes_status[index].hi_drv = 1;
            all_serdes_status.single_serdes_status[index].low_drv = 1;
            all_serdes_status.single_serdes_status[index].single_term = 1;
            all_serdes_status.single_serdes_status[index].local_loopback = 1;
        }

        result = fap20m_set_serdes_params(device_id, &all_serdes_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_get_serdes_params with not NULL all_serdes_status.
            Expected: SAND_OK and the same all_serdes_status as was set.
        */
        result = fap20m_get_serdes_params(device_id, &all_serdes_statusGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_get_serdes_params: %d", device_id);

        /* verifying values */
        for(index = 0; index < FAP20M_NUMBER_OF_LINKS; index++)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].valid, 
                                         all_serdes_statusGet.single_serdes_status[index].valid, 
                                         "all_serdes_status.single_serdes_status[%d].valid: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].driver_tx, 
                                         all_serdes_statusGet.single_serdes_status[index].driver_tx, 
                                         "all_serdes_status.single_serdes_status[%d].driver_tx: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].driver_eq, 
                                         all_serdes_statusGet.single_serdes_status[index].driver_eq, 
                                         "all_serdes_status.single_serdes_status[%d].driver_eq: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].hi_drv, 
                                         all_serdes_statusGet.single_serdes_status[index].hi_drv, 
                                         "all_serdes_status.single_serdes_status[%d].hi_drv: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].low_drv, 
                                         all_serdes_statusGet.single_serdes_status[index].low_drv, 
                                         "all_serdes_status.single_serdes_status[%d].low_drv: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].single_term, 
                                         all_serdes_statusGet.single_serdes_status[index].single_term, 
                                         "all_serdes_status.single_serdes_status[%d].single_term: %d", index, device_id);
            UTF_VERIFY_EQUAL2_STRING_MAC(all_serdes_status.single_serdes_status[index].local_loopback, 
                                         all_serdes_statusGet.single_serdes_status[index].local_loopback, 
                                         "all_serdes_status.single_serdes_status[%d].local_loopback: %d", index, device_id);
        }

        /*
            1.3. Call with all_serdes_status [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_set_serdes_params(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, all_serdes_status = NULL", device_id);
    }

    for(index = 0; index < FAP20M_NUMBER_OF_LINKS; index++)
    {
        all_serdes_status.single_serdes_status[index].valid = TRUE;
        all_serdes_status.single_serdes_status[index].driver_tx = 0;
        all_serdes_status.single_serdes_status[index].driver_eq = 0;
        all_serdes_status.single_serdes_status[index].hi_drv = 0;
        all_serdes_status.single_serdes_status[index].low_drv = 0;
        all_serdes_status.single_serdes_status[index].single_term = 0;
        all_serdes_status.single_serdes_status[index].local_loopback = 0;
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_serdes_params(device_id, &all_serdes_status);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_serdes_params(device_id, &all_serdes_status);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

#if 0
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_serdes_power_down(
    unsigned int     device_id,
    unsigned int     link_id,
    unsigned int     indication
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_serdes_power_down)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_id [0 / 23], indication [0 / 1].
    Expected: SAND_OK.
    1.2. Call with link_id [24] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  link_id = 0;
    SAND_UINT  indication = 0;

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_id [0 / 23], indication [0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with link_id = 0 */
        link_id = 0;
        indication = 0;

        result = fap20m_set_serdes_power_down(device_id, link_id, indication);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);

        /* iterate with link_id = 0 */
        link_id = 23;
        indication = 1;

        result = fap20m_set_serdes_power_down(device_id, link_id, indication);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);

        /*
            1.2. Call with link_id [24].
            Expected: NOT SAND_OK.
        */
        link_id = FAP20M_NUMBER_OF_LINKS;

        result = fap20m_set_serdes_power_down(device_id, link_id, indication);
        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);
    }

    link_id = 0;
    indication = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_serdes_power_down(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_serdes_power_down(device_id, link_id, indication);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

#if 0
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_reset_serdes(
    unsigned int     device_id,
    unsigned int     link_id,
    unsigned int     indication
  )
*/
UTF_TEST_CASE_MAC(fap20m_reset_serdes)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_id [0 / 23], indication [0 / 1].
    Expected: SAND_OK.
    1.2. Call with link_id [24] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  link_id = 0;
    SAND_UINT  indication = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_id [0 / 23], indication [0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with link_id = 0 */
        link_id = 0;
        indication = 0;

        result = fap20m_reset_serdes(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);

        /* iterate with link_id = 0 */
        link_id = 23;
        indication = 1;

        result = fap20m_reset_serdes(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);

        /*
            1.2. Call with link_id [24].
            Expected: NOT SAND_OK.
        */
        link_id = FAP20M_NUMBER_OF_LINKS;

        result = fap20m_reset_serdes(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);
    }

    link_id = 0;
    indication = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_reset_serdes(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_reset_serdes(device_id, link_id, indication);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

#if 0
/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_reset_serdes_pll(
    unsigned int     device_id,
    unsigned int     link_id,
    unsigned int     indication
  )
*/
UTF_TEST_CASE_MAC(fap20m_reset_serdes_pll)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_id [0 / 23], indication [0 / 1].
    Expected: SAND_OK.
    1.2. Call with link_id [24] (out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  link_id = 0;
    SAND_UINT  indication = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_id [0 / 23], indication [0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with link_id = 0 */
        link_id = 0;
        indication = 0;

        result = fap20m_reset_serdes_pll(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);

        /* iterate with link_id = 0 */
        link_id = 23;
        indication = 1;

        result = fap20m_reset_serdes_pll(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);

        /*
            1.2. Call with link_id [24].
            Expected: NOT SAND_OK.
        */
        link_id = FAP20M_NUMBER_OF_LINKS;

        result = fap20m_reset_serdes_pll(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, indication);
    }

    link_id = 0;
    indication = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_reset_serdes_pll(device_id, link_id, indication);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_reset_serdes_pll(device_id, link_id, indication);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}
#endif

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_link_plane_ndx(
    unsigned int     device_id,
    unsigned int     link_id,
    unsigned int     link_plane_ndx
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_link_plane_ndx)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_id [0 / 23], link_plane_ndx [0 / 1].
    Expected: SAND_OK.
    1.2. Call fap20m_get_link_plane_ndx with not NULL link_plane_ndx and link_id from 1.1.
    Expected: SAND_OK and the same link_plane_ndx as was set.
    1.3. Call with link_id [24](out of range).
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  link_id = 0;
    SAND_UINT  link_plane_ndx = 0;
    SAND_UINT  link_plane_ndxGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_id [0 / 23], link_plane_ndx [0 / 1].
            Expected: SAND_OK.
        */
        /* iterate with link_id = 0 */
        link_id = 0;
        link_plane_ndx = 0;

        result = fap20m_set_link_plane_ndx(device_id, link_id, link_plane_ndx);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, link_plane_ndx);

        /*
            1.2. Call fap20m_get_link_plane_ndx with not NULL link_plane_ndx and link_id from 1.1.
            Expected: SAND_OK and the same link_plane_ndx as was set.
        */
        result = fap20m_get_link_plane_ndx(device_id, link_id, &link_plane_ndxGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_link_plane_ndx: %d, %d", device_id, link_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(link_plane_ndx, link_plane_ndxGet, "got another link_plane_ndx than was set: %d", device_id);

        /* iterate with link_id = 0 */
        link_id = 23;
        link_plane_ndx = 1;

        result = fap20m_set_link_plane_ndx(device_id, link_id, link_plane_ndx);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, link_plane_ndx);

        /*
            1.2. Call fap20m_get_link_plane_ndx with not NULL link_plane_ndx and link_id from 1.1.
            Expected: SAND_OK and the same link_plane_ndx as was set.
        */
        result = fap20m_get_link_plane_ndx(device_id, link_id, &link_plane_ndxGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_STRING_MAC(SAND_OK, rc, "fap20m_get_link_plane_ndx: %d, %d", device_id, link_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(link_plane_ndx, link_plane_ndxGet, "got another link_plane_ndx than was set: %d", device_id);

        /*
            1.3. Call with link_id [24].
            Expected: NOT SAND_OK.
        */
        link_id = FAP20M_NUMBER_OF_LINKS;

        result = fap20m_set_link_plane_ndx(device_id, link_id, link_plane_ndx);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, link_id, link_plane_ndx);
    }

    link_id = 0;
    link_plane_ndx = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_link_plane_ndx(device_id, link_id, link_plane_ndx);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_link_plane_ndx(device_id, link_id, link_plane_ndx);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_link_plane_ndx(
    unsigned int     device_id,
    unsigned int     link_id,
    unsigned int*    link_plane_ndx
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_link_plane_ndx)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_id [0 / 23], non-null link_plane_ndx.
    Expected: SAND_OK.
    1.2. Call with link_id [24] (out of range).
    Expected: NOT SAND_OK.
    1.3. Call with link_plane_ndx [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  link_id = 0;
    SAND_UINT  link_plane_ndx = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_id [0 / 23], non-null link_plane_ndx.
            Expected: SAND_OK.
        */
        /* iterate with link_id = 0 */
        link_id = 0;

        result = fap20m_get_link_plane_ndx(device_id, link_id, &link_plane_ndx);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_id);

        /* iterate with link_id = 23 */
        link_id = 23;

        result = fap20m_get_link_plane_ndx(device_id, link_id, &link_plane_ndx);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_id);

        /*
            1.2. Call with link_id [24].
            Expected: NOT SAND_OK.
        */
        link_id = FAP20M_NUMBER_OF_LINKS;

        result = fap20m_get_link_plane_ndx(device_id, link_id, &link_plane_ndx);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(SAND_OK, rc, device_id, link_id);

        link_id = 0;

        /*
            1.3. Call with link_plane_ndx [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_link_plane_ndx(device_id, link_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_plane_ndx = NULL", device_id);
    }

    link_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_link_plane_ndx(device_id, link_id, &link_plane_ndx);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_link_plane_ndx(device_id, link_id, &link_plane_ndx);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_set_links_per_plane(
    unsigned int     device_id,
    unsigned int     spatial,
    unsigned int     member_id,
    unsigned int     links_per_plane
  )
*/
UTF_TEST_CASE_MAC(fap20m_set_links_per_plane)
{
/*
    ITERATE_DEVICES
    1.1. Call with spatial[TRUE / FALSE], member_id[0 / 2047], links_per_plane[1 / 2 / 4].
    Expected: SAND_OK.
    1.2. Call fap20m_get_links_per_plane with not NULL links_per_plane and other params from 1.1.
    Expected: SAND_OK and the same links_per_plane as was set.
    1.3. Call with member_id[2048] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with links_per_plane[0 / 3 / 5] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  spatial = 0;
    SAND_UINT  member_id = 0;
    SAND_UINT  links_per_plane = 0;
    SAND_UINT  links_per_planeGet = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with spatial[TRUE / FALSE], member_id[0 / 2047], links_per_plane[1 / 2 / 4].
            Expected: SAND_OK.
        */
        /* iterate with links_per_plane = 1 */
        spatial = TRUE;
        member_id = 0;
        links_per_plane = 1;

        result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, spatial, member_id, links_per_plane);

        /*
            1.2. Call fap20m_get_links_per_plane with not NULL links_per_plane and other params from 1.1.
            Expected: SAND_OK and the same links_per_plane as was set.
        */
        result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_planeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_get_links_per_plane: %d, %d, %d", device_id, spatial, member_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(links_per_plane, links_per_planeGet,
                   "got another links_per_plane than was set: %d", device_id);

        /* iterate with links_per_plane = 2 */
        spatial = FALSE;
        member_id = 2047;
        links_per_plane = 2;

        result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, spatial, member_id, links_per_plane);

        /*
            1.2. Call fap20m_get_links_per_plane with not NULL links_per_plane and other params from 1.1.
            Expected: SAND_OK and the same links_per_plane as was set.
        */
        result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_planeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_get_links_per_plane: %d, %d, %d", device_id, spatial, member_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(links_per_plane, links_per_planeGet,
                   "got another links_per_plane than was set: %d", device_id);

        /* iterate with links_per_plane = 4 */
        spatial = FALSE;
        member_id = 2047;
        links_per_plane = 4;

        result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL4_PARAM_MAC(SAND_OK, rc, device_id, spatial, member_id, links_per_plane);

        /*
            1.2. Call fap20m_get_links_per_plane with not NULL links_per_plane and other params from 1.1.
            Expected: SAND_OK and the same links_per_plane as was set.
        */
        result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_planeGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_STRING_MAC(SAND_OK, rc, "fap20m_get_links_per_plane: %d, %d, %d", device_id, spatial, member_id);

        UTF_VERIFY_EQUAL1_STRING_MAC(links_per_plane, links_per_planeGet,
                   "got another links_per_plane than was set: %d", device_id);

        /*
            1.3. Call with member_id[2048] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        member_id = 2048;

        result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, member_id = %d", device_id, member_id);

        member_id = 0;

        /*
            1.4. Call with links_per_plane[0 / 3 / 5] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        /* iterate with links_per_plane = 0 */
        links_per_plane = 0;

        result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, links_per_plane = %d", device_id, links_per_plane);

        /* iterate with links_per_plane = 3 */
        links_per_plane = 3;

        result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, links_per_plane = %d", device_id, links_per_plane);

        /* iterate with links_per_plane = 5 */
        links_per_plane = 5;

        result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, links_per_plane = %d", device_id, links_per_plane);
    }

    spatial = TRUE;
    member_id = 0;
    links_per_plane = 1;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_get_links_per_plane(
    unsigned int     device_id,
    unsigned int     spatial,
    unsigned int     member_id,
    unsigned int*    links_per_plane
  )
*/
UTF_TEST_CASE_MAC(fap20m_get_links_per_plane)
{
/*
    ITERATE_DEVICES
    1.1. Call with spatial[TRUE / FALSE], member_id[0 / 2047], not NULL links_per_plane.
    Expected: SAND_OK.
    1.2. Call with member_id[2048] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.3. Call with links_per_plane [NULL] and other params from 1.1.
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    SAND_UINT  spatial = 0;
    SAND_UINT  member_id = 0;
    SAND_UINT  links_per_plane = 0;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with spatial[TRUE / FALSE], member_id[0 / 2047], not NULL links_per_plane.
            Expected: SAND_OK.
        */
        /* iterate with spatial = TRUE */
        spatial = TRUE;
        member_id = 0;

        result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, spatial, member_id);

        /* iterate with spatial = FALSE */
        spatial = FALSE;
        member_id = 2047;

        result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, spatial, member_id);

        /*
            1.2. Call with member_id[2048] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        member_id = 2048;

        result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(SAND_OK, rc, device_id, spatial, member_id);

        member_id = 0;

        /*
            1.3. Call with links_per_plane [NULL] and other params from 1.1.
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_get_links_per_plane(device_id, spatial, member_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, links_per_plane = NULL", device_id);
    }

    spatial = TRUE;
    member_id = 0;

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_plane);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_plane);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_fabric_link_flow_control_set(
    SAND_IN  unsigned int             device_id,
    SAND_IN  FAP20M_FABRIC_LINK_FC* link_fc
  )
*/
UTF_TEST_CASE_MAC(fap20m_fabric_link_flow_control_set)
{
/*
    ITERATE_DEVICES
    1.1. Call with link_fc { rx_enable [0 / 0xFFFFFF]
                             tx_enable [0 / 0xFFFFFF]}
    Expected: SAND_OK.
    1.2. Call fap20m_fabric_link_flow_control_get with not NULL link_fc.
    Expected: SAND_OK and the same params as was set.
    1.3. Call with link_fc->rx_enable [0x1000000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.4. Call with link_fc->tx_enable [0x1000000] (out of range) and other params from 1.1.
    Expected: NOT SAND_OK.
    1.5. Call with link_fc [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_FABRIC_LINK_FC link_fc;
    FAP20M_FABRIC_LINK_FC link_fcGet;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with link_fc { rx_enable [0 / 0xFFFFFF]
                                     tx_enable [0 / 0xFFFFFF]}
            Expected: SAND_OK.
        */
        /* iterate with link_fc.rx_enable = 0 */
        link_fc.rx_enable = 0;
        link_fc.tx_enable = 0;

        result = fap20m_fabric_link_flow_control_set(device_id, &link_fc);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call fap20m_fabric_link_flow_control_get with not NULL link_fc.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_fabric_link_flow_control_get(device_id, &link_fcGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_fabric_link_flow_control_get: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(link_fc.rx_enable, link_fcGet.rx_enable, 
                                     "got another link_fc.rx_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(link_fc.tx_enable, link_fcGet.tx_enable, 
                                     "got another link_fc.tx_enable than was set: %d", device_id);

        /* iterate with link_fc.rx_enable = 0xFFFFFF */
        link_fc.rx_enable = 0xFFFFFF;
        link_fc.tx_enable = 0xFFFFFF;

        result = fap20m_fabric_link_flow_control_set(device_id, &link_fc);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
        /*
            1.2. Call fap20m_fabric_link_flow_control_get with not NULL link_fc.
            Expected: SAND_OK and the same params as was set.
        */
        result = fap20m_fabric_link_flow_control_get(device_id, &link_fcGet);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_OK, rc, "fap20m_fabric_link_flow_control_get: %d", device_id);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(link_fc.rx_enable, link_fcGet.rx_enable, 
                                     "got another link_fc.rx_enable than was set: %d", device_id);
        UTF_VERIFY_EQUAL1_STRING_MAC(link_fc.tx_enable, link_fcGet.tx_enable, 
                                     "got another link_fc.tx_enable than was set: %d", device_id);

        /*
            1.3. Call with link_fc->rx_enable [0x1000000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_fc.rx_enable = 0x1000000;

        result = fap20m_fabric_link_flow_control_set(device_id, &link_fc);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link_fc.tx_enable = %d", device_id, link_fc.rx_enable);

        link_fc.rx_enable = 0;

        /*
            1.4. Call with link_fc->tx_enable [0x1000000] (out of range) and other params from 1.1.
            Expected: NOT SAND_OK.
        */
        link_fc.tx_enable = 0x1000000;

        result = fap20m_fabric_link_flow_control_set(device_id, &link_fc);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(SAND_OK, rc, "%d, link_fc.tx_enable = %d", device_id, link_fc.tx_enable);

        link_fc.tx_enable = 0;

        /*
            1.5. Call with link_fc [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_fabric_link_flow_control_set(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_fc = NULL", device_id);
    }

    link_fc.rx_enable = 0;
    link_fc.tx_enable = 0;

    /* 2. For not-active devices                */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_fabric_link_flow_control_set(device_id, &link_fc);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_fabric_link_flow_control_set(device_id, &link_fc);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}

/*----------------------------------------------------------------------------*/
/*
unsigned long
  fap20m_fabric_link_flow_control_get(
    SAND_IN  unsigned int             device_id,
    SAND_OUT FAP20M_FABRIC_LINK_FC* link_fc
  )
*/
UTF_TEST_CASE_MAC(fap20m_fabric_link_flow_control_get)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-null link_fc.
    Expected: SAND_OK.
    1.2. Call with link_fc [NULL].
    Expected: SAND_NULL_POINTER_ERR.
*/
    SAND_U32   result = 0;
    SAND_RET   rc = SAND_OK;

    SAND_UINT  device_id = 0;
    FAP20M_FABRIC_LINK_FC link_fc;


    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* 1. Go over all active devices. */
    while (SAND_OK == prvDuneUtfNextDeviceGet(&device_id, TRUE))
    {
        /*
            1.1. Call with non-null link_fc.
            Expected: SAND_OK.
        */
        result = fap20m_fabric_link_flow_control_get(device_id, &link_fc);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);

        /*
            1.2. Call with link_fc [NULL].
            Expected: SAND_NULL_POINTER_ERR.
        */
        result = fap20m_fabric_link_flow_control_get(device_id, NULL);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_EQUAL1_STRING_MAC(SAND_NULL_POINTER_ERR, rc, "%d, link_fc = NULL", device_id);
    }

    /* 2. For not-active devices                 */
    /* check that function returns NOT SAND_OK. */

    /* prepare device iterator */
    rc = prvDuneUtfNextDeviceReset(&device_id);
    UTF_VERIFY_EQUAL0_PARAM_MAC(SAND_OK, rc);

    /* go over all non active devices */
    while(SAND_OK == prvDuneUtfNextDeviceGet(&device_id, FALSE))
    {
        result = fap20m_fabric_link_flow_control_get(device_id, &link_fc);

        rc = sand_get_error_code_from_error_word(result);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
    }

    /* 3. Call function with out of bound value for device id.*/
    device_id = SAND_MAX_DEVICE;

    result = fap20m_fabric_link_flow_control_get(device_id, &link_fc);
    rc = sand_get_error_code_from_error_word(result);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(SAND_OK, rc, device_id);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of fap20m_api_links suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(fap20m_api_links)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_links_status)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_connectivity_map)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_serdes_params)
#if 0
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_serdes_params)
#endif 
#if 0
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_serdes_power_down)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_reset_serdes)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_reset_serdes_pll)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_link_plane_ndx)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_link_plane_ndx)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_set_links_per_plane)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_get_links_per_plane)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_fabric_link_flow_control_set)
    UTF_SUIT_DECLARE_TEST_MAC(fap20m_fabric_link_flow_control_get)
UTF_SUIT_END_TESTS_MAC(fap20m_api_links)

