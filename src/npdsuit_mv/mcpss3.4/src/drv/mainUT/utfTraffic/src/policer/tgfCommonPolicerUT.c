/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonPolicerUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS Policer
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <policer/prvTgfPolicerQosRemarking.h>
#include <policer/prvTgfPolicerVlanCounting.h>
#include <policer/prvTgfPolicerPortModeAddressSelect.h>
#include <common/tgfPolicerStageGen.h>

/*----------------------------------------------------------------------------*/
/*
    3.2.2 a) tgfPolicerIngress0QosRemarkingByEntry
*/
UTF_TEST_CASE_MAC(tgfPolicerIngress0QosRemarkingByEntry)
{
/*
    1. Set VLAN configuration
    2. Generate traffic for INGRESS_0 STAGE
    3. Restore VLAN configuration
 */
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    prvTgfPolicerQosRemarkingVlanConfigurationSet();

    prvTgfPolicerIngressQosRemarkingByEntryTrafficGenerate(
            PRV_TGF_POLICER_STAGE_INGRESS_0_E);

    prvTgfPolicerQosRemarkingConfigurationRestore(PRV_TGF_POLICER_STAGE_INGRESS_0_E);
}

/*----------------------------------------------------------------------------*/
/*
    3.2.2 b) tgfPolicerIngress1QosRemarkingByEntry
*/
UTF_TEST_CASE_MAC(tgfPolicerIngress1QosRemarkingByEntry)
{
/*
    1. Set VLAN configuration
    2. Generate traffic for INGRESS_1 STAGE
    3. Restore VLAN configuration
 */
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    if(GT_FALSE ==  prvTgfPolicerStageCheck(PRV_TGF_POLICER_STAGE_INGRESS_1_E))
    {
        return ;
    }

    prvTgfPolicerQosRemarkingVlanConfigurationSet();

    prvTgfPolicerIngressQosRemarkingByEntryTrafficGenerate(
            PRV_TGF_POLICER_STAGE_INGRESS_1_E);

    prvTgfPolicerQosRemarkingConfigurationRestore(PRV_TGF_POLICER_STAGE_INGRESS_1_E);
}

/*----------------------------------------------------------------------------*/
/*
    3.2.2 c) tgfPolicerIngress0QosRemarkingByTable
*/
UTF_TEST_CASE_MAC(tgfPolicerIngress0QosRemarkingByTable)
{
/*
    1. Set VLAN configuration
    2. Generate traffic for INGRESS_0 STAGE
    3. Restore VLAN configuration
 */
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    prvTgfPolicerQosRemarkingVlanConfigurationSet();

    prvTgfPolicerIngressQosRemarkingByTableTrafficGenerate(
            PRV_TGF_POLICER_STAGE_INGRESS_0_E);

    prvTgfPolicerQosRemarkingConfigurationRestore(PRV_TGF_POLICER_STAGE_INGRESS_0_E);
}

/*----------------------------------------------------------------------------*/
/*
    3.2.2 d) tgfPolicerIngress1QosRemarkingByTable
*/
UTF_TEST_CASE_MAC(tgfPolicerIngress1QosRemarkingByTable)
{
/*
    1. Set VLAN configuration
    2. Generate traffic for INGRESS_0 STAGE
    3. Restore VLAN configuration
 */
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    if(GT_FALSE ==  prvTgfPolicerStageCheck(PRV_TGF_POLICER_STAGE_INGRESS_1_E))
    {
        return ;
    }

    prvTgfPolicerQosRemarkingVlanConfigurationSet();

    prvTgfPolicerIngressQosRemarkingByTableTrafficGenerate(
            PRV_TGF_POLICER_STAGE_INGRESS_1_E);

    prvTgfPolicerQosRemarkingConfigurationRestore(PRV_TGF_POLICER_STAGE_INGRESS_1_E);
}

/*----------------------------------------------------------------------------*/
/*
    3.2.2 e) tgfPolicerQosRemarkingEgress
*/
UTF_TEST_CASE_MAC(tgfPolicerEgressQosRemarking)
{
/*
    1. Set VLAN configuration
    2. Generate traffic for EGRESS STAGE
    3. Restore VLAN configuration
 */
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    if(GT_FALSE ==  prvTgfPolicerStageCheck(PRV_TGF_POLICER_STAGE_EGRESS_E))
    {
        return ;
    }

    prvTgfPolicerQosRemarkingVlanConfigurationSet();

    prvTgfPolicerEgressQosRemarkingTrafficGenerate();

    prvTgfPolicerQosRemarkingConfigurationRestore(PRV_TGF_POLICER_STAGE_EGRESS_E);
}

/*----------------------------------------------------------------------------*/
/*
    3.2.4 tgfPolicerVlanCounting
*/
UTF_TEST_CASE_MAC(tgfPolicerVlanCounting)
{
/*
    1. Set Vlan configuration
    2. Set Policer Vlan configuration
    3. Generate traffic
    4. Restore configuration
*/

    PRV_TGF_POLICER_STAGE_TYPE_ENT    stage;

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT2_E);

    /* Set default Policer VLAN configuration */
    prvTgfDefPolicerVlanConfigurationSet();

    /* Per stage configurations and traffic */
    for (stage = PRV_TGF_POLICER_STAGE_INGRESS_0_E;
          stage <= PRV_TGF_POLICER_STAGE_EGRESS_E; stage++)
    {
        if(GT_FALSE ==  prvTgfPolicerStageCheck(stage))
        {
            continue ;
        }

        prvTgfDefPolicerVlanStageConfigurationSet(stage);

        prvTgPolicerVlanStagefTrafficGenerate(stage);

        prvTgfPolicerVlanStageConfigurationRestore(stage);
    }

    /* Restore configuration */
    prvTgfPolicerVlanConfigurationRestore();
}



/*----------------------------------------------------------------------------*/
/*
    3.2.5 prvTgfPolicerPortModeAddressSelectFullRedCmdDrop
*/
UTF_TEST_CASE_MAC(prvTgfPolicerPortModeAddressSelectFullRedCmdDrop)
{
/*
    1. Set Vlan configuration
    2. Set Policer Vlan configuration
    3. Generate traffic
    4. Restore configuration
*/

    PRV_TGF_POLICER_STAGE_TYPE_ENT    stage;

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_LION_E);

    /* Set Policer Port Mode Address Select Configuration */
    prvTgfPolicerPortModeAddressSelectConfigurationSet();

    /* Per stage configurations and traffic */
    for (stage = PRV_TGF_POLICER_STAGE_INGRESS_0_E;
          stage <= PRV_TGF_POLICER_STAGE_EGRESS_E; stage++)
    {
        if(GT_FALSE ==  prvTgfPolicerStageCheck(stage))
        {
            continue ;
        }

        prvTgfPolicerPortModeAddressSelectFullRedCmdDropTrafficGenerate(stage);

        prvTgfPolicerPortModeAddressSelectConfigurationRestore(stage);
    }

    prvTgfPolicerPortModeAddressSelectGlobalConfigurationRestore();
}


/*----------------------------------------------------------------------------*/
/*
    3.2.6 prvTgfPolicerPortModeAddressSelectFullRedCmdFrwd
*/
UTF_TEST_CASE_MAC(prvTgfPolicerPortModeAddressSelectFullRedCmdFrwd)
{
/*
    1. Set Vlan configuration
    2. Set Policer Vlan configuration
    3. Generate traffic
    4. Restore configuration
*/

    PRV_TGF_POLICER_STAGE_TYPE_ENT    stage;

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_LION_E);

    /* Set Policer Port Mode Address Select Configuration */
    prvTgfPolicerPortModeAddressSelectConfigurationSet();

    /* Per stage configurations and traffic */
    for (stage = PRV_TGF_POLICER_STAGE_INGRESS_0_E;
          stage <= PRV_TGF_POLICER_STAGE_EGRESS_E; stage++)
    {
        if(GT_FALSE ==  prvTgfPolicerStageCheck(stage))
        {
            continue ;
        }

        prvTgfPolicerPortModeAddressSelectFullRedCmdFrwdTrafficGenerate(stage);

        prvTgfPolicerPortModeAddressSelectConfigurationRestore(stage);
    }

    prvTgfPolicerPortModeAddressSelectGlobalConfigurationRestore();
}


/*----------------------------------------------------------------------------*/
/*
    3.2.7 prvTgfPolicerPortModeAddressSelectCompresYellowCmdDrop
*/
UTF_TEST_CASE_MAC(prvTgfPolicerPortModeAddressSelectCompresYellowCmdDrop)
{
/*
    1. Set Vlan configuration
    2. Set Policer Vlan configuration
    3. Generate traffic
    4. Restore configuration
*/

    PRV_TGF_POLICER_STAGE_TYPE_ENT    stage;

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_LION_E);

    /* Set Policer Port Mode Address Select Configuration */
    prvTgfPolicerPortModeAddressSelectConfigurationSet();

    /* Per stage configurations and traffic */
    for (stage = PRV_TGF_POLICER_STAGE_INGRESS_0_E;
          stage <= PRV_TGF_POLICER_STAGE_INGRESS_1_E; stage++)
    {
        if(GT_FALSE ==  prvTgfPolicerStageCheck(stage))
        {
            continue ;
        }

        prvTgfPolicerPortModeAddressSelectCompresYellowCmdDropTrafficGenerate(stage);

        prvTgfPolicerPortModeAddressSelectConfigurationRestore(stage);
    }

    prvTgfPolicerPortModeAddressSelectGlobalConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    3.2.8 prvTgfPolicerPortModeAddressSelectCompresYellowCmdFrwd
*/
UTF_TEST_CASE_MAC(prvTgfPolicerPortModeAddressSelectCompresYellowCmdFrwd)
{
/*
    1. Set Vlan configuration
    2. Set Policer Vlan configuration
    3. Generate traffic
    4. Restore configuration
*/

    PRV_TGF_POLICER_STAGE_TYPE_ENT    stage;

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_LION_E);

    /* Set Policer Port Mode Address Select Configuration */
    prvTgfPolicerPortModeAddressSelectConfigurationSet();

    /* Per stage configurations and traffic */
    for (stage = PRV_TGF_POLICER_STAGE_INGRESS_0_E;
          stage <= PRV_TGF_POLICER_STAGE_INGRESS_1_E; stage++)
    {
        if(GT_FALSE ==  prvTgfPolicerStageCheck(stage))
        {
            continue ;
        }

        prvTgfPolicerPortModeAddressSelectCompresYellowCmdFrwdTrafficGenerate(stage);

        prvTgfPolicerPortModeAddressSelectConfigurationRestore(stage);
    }

    prvTgfPolicerPortModeAddressSelectGlobalConfigurationRestore();
}


/*----------------------------------------------------------------------------*/
/*
    3.2.9 prvTgfPolicerPortModeAddressSelectCompresYellowCmdFrwd
*/
UTF_TEST_CASE_MAC(prvTgfPolicerPortModeAddressSelectFullGreen)
{
/*
    1. Set Vlan configuration
    2. Set Policer Vlan configuration
    3. Generate traffic
    4. Restore configuration
*/

    PRV_TGF_POLICER_STAGE_TYPE_ENT    stage;

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_LION_E);

    /* Set Policer Port Mode Address Select Configuration */
    prvTgfPolicerPortModeAddressSelectConfigurationSet();

    /* Per stage configurations and traffic */
    for (stage = PRV_TGF_POLICER_STAGE_EGRESS_E;
          stage <= PRV_TGF_POLICER_STAGE_EGRESS_E; stage++)
    {
        if(GT_FALSE ==  prvTgfPolicerStageCheck(stage))
        {
            continue ;
        }

        prvTgfPolicerPortModeAddressSelectFullGreenTrafficGenerate(stage);

        prvTgfPolicerPortModeAddressSelectConfigurationRestore(stage);
    }

    prvTgfPolicerPortModeAddressSelectGlobalConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    3.2.9 prvTgfPolicerPortModeAddressSelectCompresGreen
*/
UTF_TEST_CASE_MAC(prvTgfPolicerPortModeAddressSelectCompresGreen)
{
/*
    1. Set Vlan configuration
    2. Set Policer Vlan configuration
    3. Generate traffic
    4. Restore configuration
*/

    PRV_TGF_POLICER_STAGE_TYPE_ENT    stage;

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_LION_E);

    /* Set Policer Port Mode Address Select Configuration */
    prvTgfPolicerPortModeAddressSelectConfigurationSet();

    /* Per stage configurations and traffic */
    for (stage = PRV_TGF_POLICER_STAGE_EGRESS_E;
          stage <= PRV_TGF_POLICER_STAGE_EGRESS_E; stage++)
    {
        if(GT_FALSE ==  prvTgfPolicerStageCheck(stage))
        {
            continue ;
        }

        prvTgfPolicerPortModeAddressSelectCompresGreenTrafficGenerate(stage);

        prvTgfPolicerPortModeAddressSelectConfigurationRestore(stage);
    }

    prvTgfPolicerPortModeAddressSelectGlobalConfigurationRestore();
}



/*
 * Configuration of tgfPolicer suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfPolicer)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPolicerIngress0QosRemarkingByEntry)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPolicerIngress1QosRemarkingByEntry)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPolicerIngress0QosRemarkingByTable)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPolicerIngress1QosRemarkingByTable)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPolicerEgressQosRemarking)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPolicerVlanCounting)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPolicerPortModeAddressSelectFullRedCmdDrop)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPolicerPortModeAddressSelectFullRedCmdFrwd)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPolicerPortModeAddressSelectCompresYellowCmdDrop)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPolicerPortModeAddressSelectCompresYellowCmdFrwd)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPolicerPortModeAddressSelectFullGreen)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPolicerPortModeAddressSelectCompresGreen)
UTF_SUIT_END_TESTS_MAC(tgfPolicer)

