/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonIpLpmEngineUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS ipLpmEngine
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <ipLpmEngine/prvTgfDynamicPartitionIpLpmUcRouting.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

extern GT_STATUS appDemoDbEntryGet
(
    IN  GT_CHAR *namePtr,
    OUT GT_U32  *valuePtr
);

/*----------------------------------------------------------------------------*/
/*
    Test lpm algorithm (push down) and unicast routing under dynamic partition
*/
UTF_TEST_CASE_MAC(tgfDynamicPartitionIpLpmPushDownUcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/
    /* Set Base configuration */
    prvTgfDynamicPartitionIpLpmPushDownUcBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfDynamicPartitionIpLpmPushDownUcRouteConfigurationSet();

    /* Generate traffic */
    prvTgfDynamicPartitionIpLpmPushDownUcRoutingTrafficGenerate();

    /* Restore configuration */
    prvTgfDynamicPartitionIpLpmPushDownUcRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test lpm algorithm (push up) and unicast routing under dynamic partition
*/
UTF_TEST_CASE_MAC(tgfDynamicPartitionIpLpmPushUpUcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfDynamicPartitionIpLpmPushUpUcBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfDynamicPartitionIpLpmPushUpUcRouteConfigurationSet();

    /* Generate traffic */
    prvTgfDynamicPartitionIpLpmPushUpUcRoutingTrafficGenerate();

    /* Restore configuration */
    prvTgfDynamicPartitionIpLpmPushUpUcRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test lpm algorithm (defragmentation enable) and unicast routing under dynamic partition
*/
UTF_TEST_CASE_MAC(tgfDynamicPartitionIpLpmDefragUcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfDynamicPartitionIpLpmDefragUcBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfDynamicPartitionIpLpmDefragUcRouteConfigurationSet();

    /* Generate traffic */
    prvTgfDynamicPartitionIpLpmDefragUcRoutingTrafficGenerate();

    /* Restore configuration */
    prvTgfDynamicPartitionIpLpmDefragUcRoutingConfigurationRestore();
}


/*
 * Configuration of tgfIp suit
 */
UTF_SUIT_WITH_PREINIT_CONDITION_BEGIN_TESTS_MAC(tgfIpLpmEngine)
    UTF_SUIT_DECLARE_TEST_MAC(tgfDynamicPartitionIpLpmPushUpUcRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfDynamicPartitionIpLpmPushDownUcRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfDynamicPartitionIpLpmDefragUcRouting)
UTF_SUIT_WITH_PREINIT_CONDITION_END_TESTS_MAC(tgfIpLpmEngine,"lpmDbPartitionEnable",0)

