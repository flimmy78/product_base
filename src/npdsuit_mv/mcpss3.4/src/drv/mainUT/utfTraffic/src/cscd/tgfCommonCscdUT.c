/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonCscdUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS Cascading
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <cscd/prvTgfCscd.h>

/*----------------------------------------------------------------------------*/
/*
    prvTgfCscdSingleTargetDestination
*/
UTF_TEST_CASE_MAC(prvTgfCscdSingleTargetDestination)
{
/********************************************************************
    Test 1.1 - Packet sending according to device map table for not local devices.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_NONE_FAMILY_E);

    /* Set configuration */
    prvTgfCscdSingleTargetDestinationConfigurationSet();

    /* Generate traffic */
    prvTgfCscdSingleTargetDestinationTrafficGenerate();

    /* Restore configuration */
    prvTgfCscdSingleTargetDestinationConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfCscdSingleTargetDestinationLocalDevice
*/
UTF_TEST_CASE_MAC(prvTgfCscdSingleTargetDestinationLocalDevice)
{
/********************************************************************
    Test 1.2 - Packet sending according to device map table for local devices.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Set configuration */
    prvTgfCscdSingleTargetDestinationLocalDeviceConfigurationSet();

    /* Generate traffic */
    prvTgfCscdSingleTargetDestinationLocalDeviceTrafficGenerate();

    /* Restore configuration */
    prvTgfCscdSingleTargetDestinationLocalDeviceConfigurationRestore();
}


/*
 * Configuration of tgfCscd suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfCscd)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfCscdSingleTargetDestination)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfCscdSingleTargetDestinationLocalDevice)

UTF_SUIT_END_TESTS_MAC(tgfCscd)

