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
*       Enhanced UTs for CPSS trunk
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <trunk/prvTgfTrunk.h>
#include <common/tgfTrunkGen.h>
#include <common/tgfCommon.h>


/*----------------------------------------------------------------------------*/
/*
    tgfTrunkHashMplsModeEnableSet
*/
UTF_TEST_CASE_MAC(tgfTrunkHashMplsModeEnableSet)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);
    /* Set configuration */
    prvTgfTrunkForHashTestConfigure();

    /* Generate traffic */
    prvTgfTrunkHashTestTrafficMplsModeEnableSet();

    /* Restore configuration */
    prvTgfTrunkForHashTestConfigurationReset();
}


/*----------------------------------------------------------------------------*/
/*
    tgfTrunkHashMaskSet
*/
UTF_TEST_CASE_MAC(tgfTrunkHashMaskSet)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);
    /* Set configuration */
    prvTgfTrunkForHashTestConfigure();

    /* Generate traffic */
    prvTgfTrunkHashTestTrafficMaskSet();

    /* Restore configuration */
    prvTgfTrunkForHashTestConfigurationReset();
}

/*----------------------------------------------------------------------------*/
/*
    tgfTrunkHashIpShiftSet
*/
UTF_TEST_CASE_MAC(tgfTrunkHashIpShiftSet)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);
    /* Set configuration */
    prvTgfTrunkForHashTestConfigure();

    /* Generate traffic */
    prvTgfTrunkHashTestTrafficIpShiftSet();

    /* Restore configuration */
    prvTgfTrunkForHashTestConfigurationReset();
}

/*----------------------------------------------------------------------------*/
/*
    tgfTrunkDesignatedPorts
*/
UTF_TEST_CASE_MAC(tgfTrunkDesignatedPorts)
{
    /* run test */
    prvTgfTrunkDesignatedPorts();
}

/*----------------------------------------------------------------------------*/
/*
    tgfTrunkToCpu
*/
UTF_TEST_CASE_MAC(tgfTrunkToCpu)
{
    /* Set configuration */
    prvTgfTrunkAndVlanTestConfigure();

    /* Generate traffic */
    prvTgfTrunkToCpuTraffic();

    /* Restore configuration */
    prvTgfTrunkAndVlanTestConfigurationReset();
}

/*----------------------------------------------------------------------------*/
/*
    tgfTrunkCrcHash
*/
UTF_TEST_CASE_MAC(tgfTrunkCrcHash)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    /* Set configuration */
    prvTgfTrunkCrcHashTestInit();

    /* Generate traffic */
    prvTgfTrunkCrcHashTest();

    /* Restore configuration */
    prvTgfTrunkCrcHashTestRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfTrunkCrcHashMaskPriority
*/
UTF_TEST_CASE_MAC(tgfTrunkCrcHashMaskPriority)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    /* Set configuration */
    prvTgfTrunkCrcHashTestInit();

    /* Generate traffic */
    prvTgfTrunkCrcHashMaskPriorityTest();

    /* Restore configuration */
    prvTgfTrunkCrcHashTestRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfTrunkCascadeTrunk
*/
UTF_TEST_CASE_MAC(tgfTrunkCascadeTrunk)
{
    /* Set configuration */
    tgfTrunkCascadeTrunkTestInit();

    /* Generate traffic */
    tgfTrunkCascadeTrunkTest();

    /* Restore configuration */
    tgfTrunkCascadeTrunkTestRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfTrunkDesignatedTableModes
*/
UTF_TEST_CASE_MAC(tgfTrunkDesignatedTableModes)
{
    /* Set configuration */
    tgfTrunkDesignatedTableModesTestInit();

    /* Generate traffic */
    tgfTrunkDesignatedTableModesTest();

    /* Restore configuration */
    tgfTrunkDesignatedTableModesTestRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfTrunkMcLocalSwitching
*/
UTF_TEST_CASE_MAC(tgfTrunkMcLocalSwitching)
{
    /* Set configuration */
    tgfTrunkMcLocalSwitchingTestInit();

    /* Generate traffic */
    tgfTrunkMcLocalSwitchingTest();

    /* Restore configuration */
    tgfTrunkMcLocalSwitchingTestRestore();
}


/*
 * Configuration of tgfTrunk suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfTrunk)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkCrcHash)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkCrcHashMaskPriority)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkCascadeTrunk)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkDesignatedTableModes)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkHashMplsModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkHashMaskSet)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkHashIpShiftSet)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkDesignatedPorts)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkToCpu)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTrunkMcLocalSwitching)
UTF_SUIT_END_TESTS_MAC(tgfTrunk)
