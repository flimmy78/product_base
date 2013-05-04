/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonVntUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS Vnt
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <cos/prvTgfCos.h>
#include <common/tgfCosGen.h>
#include <common/tgfCommon.h>


/*----------------------------------------------------------------------------*/
/*
    tgfCosTrustExpModeSet
*/
UTF_TEST_CASE_MAC(tgfCosTrustExpModeSet)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfCosTestCommonConfigure();

    /* Generate traffic */
    prvTgfCosExpTestTrafficGenerateAndCheck();

    /* Restore configuration */
    prvTgfCosTestCommonReset();
}


/*----------------------------------------------------------------------------*/
/*
    tgfCosL2TrustModeVlanTagSelectSet
*/
UTF_TEST_CASE_MAC(tgfCosL2TrustModeVlanTagSelectSet)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfCosTestCommonConfigure();

    /* Generate traffic */
    prvTgfCosVlanTagSelectTestTrafficGenerateAndCheck();

    /* Restore configuration */
    prvTgfCosTestCommonReset();
}



/*
 * Configuration of tgfCos suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfCos)
    UTF_SUIT_DECLARE_TEST_MAC(tgfCosTrustExpModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(tgfCosL2TrustModeVlanTagSelectSet)
UTF_SUIT_END_TESTS_MAC(tgfCos)

