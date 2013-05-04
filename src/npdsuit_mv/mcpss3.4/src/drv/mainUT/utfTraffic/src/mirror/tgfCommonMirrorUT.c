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
*       Enhanced UTs for CPSS Mirror
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <mirror/prvTgfMirror.h>

/*----------------------------------------------------------------------------*/
/*
    prvTgfIngressMirrorHopByHop
*/
UTF_TEST_CASE_MAC(prvTgfIngressMirrorHopByHop)
{
/********************************************************************
    Test 8.1 - Ingress mirroring functionality in hop-by-hop mode.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfIngressMirrorHopByHopConfigurationSet();

    /* Generate traffic */
    prvTgfIngressMirrorHopByHopTrafficGenerate();

    /* Restore configuration */
    prvTgfIngressMirrorHopByHopConfigurationRestore();
}
/*----------------------------------------------------------------------------*/
/*
    prvTgfIngressMirrorSourceBased
*/
UTF_TEST_CASE_MAC(prvTgfIngressMirrorSourceBased)
{
/********************************************************************
    Test 8.2 - Ingress mirroring functionality in source-based mode.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfIngressMirrorSourceBasedConfigurationSet();

    /* Generate traffic */
    prvTgfIngressMirrorSourceBasedTrafficGenerate();

    /* Restore configuration */
    prvTgfIngressMirrorSourceBasedConfigurationRestore();
}
/*----------------------------------------------------------------------------*/
/*
    prvTgfIngressMirrorHighestIndexSelection
*/
UTF_TEST_CASE_MAC(prvTgfIngressMirrorHighestIndexSelection)
{
/********************************************************************
    Test 8.3 - Port source-based mirroring and other ingress mirroring mechanisms.
               (Selected analyzer is the one with the higher index.)

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfIngressMirrorHighestIndexSelectionConfigurationSet();

    /* Generate traffic */
    prvTgfIngressMirrorHighestIndexSelectionTrafficGenerate();

    /* Restore configuration */
    prvTgfIngressMirrorHighestIndexSelectionConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfEgressMirrorHopByHop
*/
UTF_TEST_CASE_MAC(prvTgfEgressMirrorHopByHop)
{
/********************************************************************
    Test 8.4 - Egress mirroring functionality in hop-by-hop mode.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfEgressMirrorHopByHopConfigurationSet();

    /* Generate traffic */
    prvTgfEgressMirrorHopByHopTrafficGenerate();

    /* Restore configuration */
    prvTgfEgressMirrorHopByHopConfigurationRestore();
}
/*----------------------------------------------------------------------------*/
/*
    prvTgfEgressMirrorSourceBased
*/
UTF_TEST_CASE_MAC(prvTgfEgressMirrorSourceBased)
{
/********************************************************************
    Test 8.5 - Egress mirroring functionality in source-based mode.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfEgressMirrorSourceBasedConfigurationSet();

    /* Generate traffic */
    prvTgfEgressMirrorSourceBasedTrafficGenerate();

    /* Restore configuration */
    prvTgfEgressMirrorSourceBasedConfigurationRestore();
}
/*----------------------------------------------------------------------------*/
/*
    prvTgfIngressMirrorForwardingModeChange
*/
UTF_TEST_CASE_MAC(prvTgfIngressMirrorForwardingModeChange)
{
/********************************************************************
    Test 8.6 - Transforming from hop-by-hop mode to source-based mode and back
               to hop-by-hop mode for ingress mirroring.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfIngressMirrorForwardingModeChangeConfigurationSet();

    /* Generate traffic */
    prvTgfIngressMirrorForwardingModeChangeTrafficGenerate();

    /* Restore configuration */
    prvTgfIngressMirrorForwardingModeChangeConfigurationRestore();
}
/*----------------------------------------------------------------------------*/
/*
    prvTgfEgressMirrorForwardingModeChange
*/
UTF_TEST_CASE_MAC(prvTgfEgressMirrorForwardingModeChange)
{
/********************************************************************
    Test 8.7 - Transforming from hop-by-hop mode to source-based mode and back
               to hop-by-hop mode for egress mirroring.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfEgressMirrorForwardingModeChangeConfigurationSet();

    /* Generate traffic */
    prvTgfEgressMirrorForwardingModeChangeTrafficGenerate();

    /* Restore configuration */
    prvTgfEgressMirrorForwardingModeChangeConfigurationRestore();
}
/*----------------------------------------------------------------------------*/
/*
    prvTgfIngressMirrorVlanTagRemoval
*/
UTF_TEST_CASE_MAC(prvTgfIngressMirrorVlanTagRemoval)
{
/********************************************************************
    Test 8.8 - VLAN Tag Removal of ingress Mirrored Traffic.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Set configuration */
    prvTgfIngressMirrorVlanTagRemovalConfigurationSet();

    /* Generate traffic */
    prvTgfIngressMirrorVlanTagRemovalTrafficGenerate();

    /* Restore configuration */
    prvTgfIngressMirrorVlanTagRemovalConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfEgressMirrorVlanTagRemoval
*/
UTF_TEST_CASE_MAC(prvTgfEgressMirrorVlanTagRemoval)
{
/********************************************************************
    Test 8.9 - VLAN Tag Removal of egress Mirrored Traffic.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Set configuration */
    prvTgfEgressMirrorVlanTagRemovalConfigurationSet();

    /* Generate traffic */
    prvTgfEgressMirrorVlanTagRemovalTrafficGenerate();

    /* Restore configuration */
    prvTgfEgressMirrorVlanTagRemovalConfigurationRestore();
}


/*
 * Configuration of tgfMirror suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfMirror)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIngressMirrorHopByHop)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIngressMirrorSourceBased)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIngressMirrorHighestIndexSelection)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfEgressMirrorHopByHop)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfEgressMirrorSourceBased)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIngressMirrorForwardingModeChange)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfEgressMirrorForwardingModeChange)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfEgressMirrorVlanTagRemoval) 
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIngressMirrorVlanTagRemoval)

UTF_SUIT_END_TESTS_MAC(tgfMirror)

