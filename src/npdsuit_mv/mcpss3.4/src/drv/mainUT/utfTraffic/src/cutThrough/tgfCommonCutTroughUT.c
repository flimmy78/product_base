/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonMirrorUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS Cut-Through
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <cutThrough/prvTgfCutTrough.h>

/*----------------------------------------------------------------------------*/
/*
    prvTgfCutThroughBypassRouterAndPolicer
*/
UTF_TEST_CASE_MAC(prvTgfCutThroughBypassRouterAndPolicer)
{
/********************************************************************
    Test 1.1 - Enabling / disabling bypassing the Router and Ingress Policer engines.

    1. Set base configuration
    2. Set Routing configuration
    3. Generate traffic
    4. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_XCAT2_E);

    /* Set base configuration */
    prvTgfCutThroughBypassRouterAndPolicerBaseConfigurationSet();

    /* Set Routing configuration */
    prvTgfCutThroughBypassRouterAndPolicerRouteConfigurationSet();

    /* Generate traffic */
    prvTgfCutThroughBypassRouterAndPolicerTrafficGenerate();

    /* Restore configuration */
    prvTgfCutThroughBypassRouterAndPolicerConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfCutThroughDefaultPacketLength 
*/
UTF_TEST_CASE_MAC(prvTgfCutThroughDefaultPacketLength)
{
/********************************************************************
    Test 1.2 - Set Default packet length for Cut-Through mode.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_XCAT2_E);

    /* Set configuration */
    prvTgfCutThroughDefaultPacketLengthConfigurationSet();

    /* Generate traffic */
    prvTgfCutThroughDefaultPacketLengthTrafficGenerate();

    /* Restore configuration */
    prvTgfCutThroughDefaultPacketLengthConfigurationRestore();
}



/*
 * Configuration of tgfCutThrough suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfCutThrough)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfCutThroughBypassRouterAndPolicer)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfCutThroughDefaultPacketLength)

UTF_SUIT_END_TESTS_MAC(tgfCutThrough)

