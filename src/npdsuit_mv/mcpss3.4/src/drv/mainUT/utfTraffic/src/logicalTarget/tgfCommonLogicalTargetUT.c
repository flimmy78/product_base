/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonLogicalTargetUT.c
*
* DESCRIPTION:
*       Enhanced UTs for Logical Target Mapping
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <logicalTarget/prvTgfLogicalTargetIpv4Uc.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>


/*----------------------------------------------------------------------------*/
/*
    Test Logical Target Ipv4 unicast
*/
UTF_TEST_CASE_MAC(tgfLogicalTargetIpv4Uc)
{
/*
    1. Set Base configuration
    2. Set logical target configuration
    3. Set Route configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set Base configuration */
    prvTgfLogicalTargetIpv4UcBaseConfigurationSet();

    /* Set Logical Target Mapping  configuration */
    prvTgfLogicalTargetIpv4UcMappingConfigurationSet();

    /* Set Route configuration */
    prvTgfLogicalTargetIpv4UcRouteConfigurationSet();

    /* Generate traffic */
    prvTgfLogicalTargetIpv4UcTrafficGenerate();

    /* Restore configuration */
    prvTgfLogicalTargetIpv4UcConfigurationRestore();
}

/*----------------------------------------------------------------------------*/


/*
 * Configuration of tgfLogicalTarget suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfLogicalTarget)
    UTF_SUIT_DECLARE_TEST_MAC(tgfLogicalTargetIpv4Uc)
UTF_SUIT_END_TESTS_MAC(tgfLogicalTarget)

