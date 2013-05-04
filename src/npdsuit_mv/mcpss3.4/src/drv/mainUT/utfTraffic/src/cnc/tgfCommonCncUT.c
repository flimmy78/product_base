/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonCncUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS CNC
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>
#include <common/tgfCncGen.h>
#include <cnc/prvTgfCncVlanL2L3.h>
#include <cnc/prvTgfCncEgrQue.h>


/*----------------------------------------------------------------------------*/
/*
    tgfCncVlanL2L3
*/

UTF_TEST_CASE_MAC(tgfCncVlanL2L3)
{
    prvTgfCncTestVlanL2L3Configure();

    prvTgfCncTestVlanL2L3TrafficGenerateAndCheck();

    prvTgfCncTestCommonReset();
}

/*----------------------------------------------------------------------------*/
/*
    tgfCncCounterFormatsTest
*/

UTF_TEST_CASE_MAC(tgfCncCounterFormatsTest)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Test */
    prvTgfCncTestVlanL2L3Configure();

    prvTgfCncTestVlanL2L3TrafficTestFormats();

    prvTgfCncTestCommonReset();
}

/*----------------------------------------------------------------------------*/
/*
    tgfCncWraparoundTest
*/

UTF_TEST_CASE_MAC(tgfCncWraparoundTest)
{
    prvTgfCncTestVlanL2L3Configure();

    prvTgfCncTestVlanL2L3TrafficTestWraparound();

    prvTgfCncTestCommonReset();
}

/*
    tgfCncEgrQueTest
*/

UTF_TEST_CASE_MAC(tgfCncEgrQueTest)
{
    prvTgfCncTestEgrQueConfigure();

    prvTgfCncTestEgrQueTrafficGenerateAndCheck();

    prvTgfCncTestCommonReset();
}

/*
 * Configuration of tgfCnc suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfCnc)
    UTF_SUIT_DECLARE_TEST_MAC(tgfCncVlanL2L3)
    UTF_SUIT_DECLARE_TEST_MAC(tgfCncCounterFormatsTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfCncWraparoundTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfCncEgrQueTest)
UTF_SUIT_END_TESTS_MAC(tgfCnc)

