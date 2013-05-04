/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonBridgeUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS Bridge
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <bridge/prvTgfBasicDynamicLearning.h>
#include <bridge/prvTgfTransmitAllKindsOfTraffic.h>
#include <bridge/prvTgfVlanIngressFiltering.h>
#include <bridge/prvTgfVlanEgressFiltering.h>
#include <bridge/prvTgfVlanManipulation.h>
#include <bridge/prvTgfVlanFloodVidx.h>
#include <bridge/prvTgfFdbAging.h>
#include <bridge/prvTgfBrgGen.h>
#include <bridge/prvTgfBrgSrcId.h>
#include <bridge/prvTgfVlanTag1RemIfZero.h>
#include <bridge/prvTgfVlanKeepVlan1.h>
#include <bridge/prvTgfBrgIeeeReservedMcastLearning.h>
#include <bridge/prvTgfBrgRateLimitMcastReg.h>
#include <bridge/prvTgfFdbAuNaMessageExtFormat.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfTunnelGen.h>
/*----------------------------------------------------------------------------*/
/*
    Test the system's Rx analyzer port configuration for regular ports
*/
UTF_TEST_CASE_MAC(tgfBasicDynamicLearning)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
*/

    /* Set configuration */
    prvTgfBrgBdlConfigurationSet();

    /* Generate traffic */
    prvTgfBrgBdlTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgBdlConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test transmitting all kinds of traffic from all port
*/
UTF_TEST_CASE_MAC(tgfTransmitAllKindsOfTraffic)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
*/

    /* Set configuration */
    prvTgfBrgTransmitAllConfigurationSet();

    /* Generate traffic */
    prvTgfBrgTransmitAllTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgTransmitAllConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test if the port not in the incoming packet's VLAN the packets are dropped
*/
UTF_TEST_CASE_MAC(tgfBrgVlanIngressFiltering)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
*/

    /* Set configuration */
    prvTgfBrgVlanIngrFltConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanIngrFltTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanIngrFltConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test Vlan egress filtering
*/
UTF_TEST_CASE_MAC(tgfBrgVlanEgressFiltering)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
*/

    /* Set configuration */
    prvTgfBrgVlanEgrFltConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanEgrFltTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanEgrFltConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationUntaggedCmdDoubleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationUntaggedCmdDoubleTag)
{
/********************************************************************
    Test 3.1 - VLAN TAG MANIPULATION - UNTAGGED COMMAND,
    incoming packet is double tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationUntaggedCmdDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationUntaggedCmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationUntaggedCmdSingleTag)
{
/********************************************************************
    Test 3.2 - VLAN TAG MANIPULATION - UNTAGGED COMMAND,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationUntaggedCmdSingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationUntaggedCmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationUntaggedCmdUntag)
{
/********************************************************************
    Test 3.3 - VLAN TAG MANIPULATION - UNTAGGED COMMAND,
    incoming packet is untagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationUntaggedCmdUntagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationTag0CmdDoubleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationTag0CmdDoubleTag)
{
/********************************************************************
    Test 3.4 - VLAN TAG MANIPULATION - TAG0 COMMAND,
    incoming packet is double tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationTag0CmdDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationTag0CmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationTag0CmdSingleTag)
{
/********************************************************************
    Test 3.5 - VLAN TAG MANIPULATION - TAG0 COMMAND,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationTag0CmdSingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationTag0CmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationTag0CmdUntag)
{
/********************************************************************
    Test 3.6 - VLAN TAG MANIPULATION - TAG0 COMMAND,
    incoming packet is untagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationTag0CmdUntagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationTag0CmdUntagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationTag0CmdUntagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationTag1CmdDoubleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationTag1CmdDoubleTag)
{
/********************************************************************
    Test 3.7 - VLAN TAG MANIPULATION - TAG1 COMMAND,
    incoming packet is double tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationTag1CmdDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationTag1CmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationTag1CmdSingleTag)
{
/********************************************************************
    Test 3.8 - VLAN TAG MANIPULATION - TAG1 COMMAND,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationTag1CmdSingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationTag1CmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationTag1CmdUntag)
{
/********************************************************************
    Test 3.9 - VLAN TAG MANIPULATION - TAG1 COMMAND,
    incoming packet is untagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationTag1CmdUntagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationTag1CmdUntagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationTag1CmdUntagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationOuterTag0InnerTag1CmdDoubleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationOuterTag0InnerTag1CmdDoubleTag)
{
/********************************************************************
    Test 3.10 - VLAN TAG MANIPULATION - Outer Tag0, Inner Tag1 command,
    incoming packet is double tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationOuterTag0InnerTag1CmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationOuterTag0InnerTag1CmdSingleTag)
{
/********************************************************************
    Test 3.11 - VLAN TAG MANIPULATION - Outer Tag0, Inner Tag1 command,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationOuterTag0InnerTag1CmdUntag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationOuterTag0InnerTag1CmdUntag)
{
/********************************************************************
    Test 3.12 - VLAN TAG MANIPULATION - Outer Tag0, Inner Tag1 command,
    incoming packet is untagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationOuterTag1InnerTag0CmdDoubleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationOuterTag1InnerTag0CmdDoubleTag)
{
/********************************************************************
    Test 3.13 - VLAN TAG MANIPULATION - Outer Tag1, Inner Tag0 command,
    incoming packet is double tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationOuterTag1InnerTag0CmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationOuterTag1InnerTag0CmdSingleTag)
{
/********************************************************************
    Test 3.14 - VLAN TAG MANIPULATION - Outer Tag1, Inner Tag0 command,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationOuterTag1InnerTag0CmdUntag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationOuterTag1InnerTag0CmdUntag)
{
/********************************************************************
    Test 3.15 - VLAN TAG MANIPULATION - Outer Tag1, Inner Tag0 command,
    incoming packet is untagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationRestore();
}
/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationPushTag0CmdDoubleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationPushTag0CmdDoubleTag)
{
/********************************************************************
    Test 3.16 - VLAN TAG MANIPULATION - "Push Tag0" command,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationPushTag0CmdDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationPushTag0CmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationPushTag0CmdSingleTag)
{
/********************************************************************
    Test 3.17 - VLAN TAG MANIPULATION - "Push Tag0" command,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationPushTag0CmdSingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationPushTag0CmdUntag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationPushTag0CmdUntag)
{
/********************************************************************
    Test 3.18 - VLAN TAG MANIPULATION - "Push Tag0" command,
    incoming packet is untagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationPushTag0CmdUntagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationPopOuterTagCmdDoubleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationPopOuterTagCmdDoubleTag)
{
/********************************************************************
    Test 3.19 - VLAN TAG MANIPULATION - "Pop Outer Tag" command,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationPopOuterTagCmdSingleTag
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationPopOuterTagCmdSingleTag)
{
/********************************************************************
    Test 3.20 - VLAN TAG MANIPULATION - "Pop Outer Tag" command,
    incoming packet is single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationIngressVlanAssignment1
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationIngressVlanAssignment1)
{
/********************************************************************
    Test 3.21 - VLAN TAG MANIPULATION - Ingress VLAN assignment

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationIngressVlanAssignment1TrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationIngressVlanAssignment2
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationIngressVlanAssignment2)
{
/********************************************************************
    Test 3.22 - VLAN TAG MANIPULATION - Ingress VLAN assignment

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationIngressVlanAssignment2TrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanManipulationChangeEgressTpid
*/
UTF_TEST_CASE_MAC(tgfVlanManipulationChangeEgressTpid)
{
/********************************************************************
    Test 3.23 - VLAN TAG MANIPULATION - Change Egress TPID during the test

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    if(GT_FALSE == prvTgfBrgVlanTr101Check())
    {
        return ;
    }

    /* Set configuration */
    prvTgfBrgVlanManipulationChangeEgressTpidConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanManipulationChangeEgressTpidTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanManipulationChangeEgressTpidConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanFloodVidx
*/
UTF_TEST_CASE_MAC(tgfVlanFloodVidx)
{
/********************************************************************
    Test 5.1 - Bridge Multicast Port Groups Basic - Unregistred MC

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    /* Set configuration */
    prvTgfBrgVlanFloodVidxConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanFloodVidxTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanFloodVidxConfigurationRestore();

}

/*----------------------------------------------------------------------------*/
/*
    tgfVlanFloodVidxAllFlooded
*/
UTF_TEST_CASE_MAC(tgfVlanFloodVidxAllFlooded)
{
/********************************************************************
    Test 5.2 - Bridge Multicast Port Groups Basic - All Flooded Traffic

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    /* Set configuration */
    prvTgfBrgVlanFloodVidxAllFloodedConfigurationSet();

    /* Generate traffic */
    prvTgfBrgVlanFloodVidxAllFloodedTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgVlanFloodVidxAllFloodedConfigurationRestore();

}


/*----------------------------------------------------------------------------*/
/*
    tgfFdbAgeBitDaRefresh
*/
UTF_TEST_CASE_MAC(tgfFdbAgeBitDaRefresh)
{
/********************************************************************
    Test - FDB Age Bit Da Refresh

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
   /*
        1. the multi-port groups device not support auto aging with removal ,
           so this test is not applicable for it
        2. the multi-port group device can't refresh with DA hit all the port
           groups , so this test in not applicable for it.
    */
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Set configuration */
    prvTgfFdbAgeBitDaRefreshConfigurationSet();

    /* Generate traffic */
    prvTgfFdbAgeBitDaRefreshTrafficGenerate();

    /* Restore configuration */
    prvTgfFdbAgeBitDaRefreshConfigurationRestore();
}

UTF_TEST_CASE_MAC(tgfBrgGenIeeeReservedMcastProfiles)
{
/********************************************************************
    Test - IEEE Reserved Mcast Profiles test

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfBrgGenIeeeReservedMcastProfilesConfigurationSet();

    /* Generate traffic */
    prvTgfBrgGenIeeeReservedMcastProfilesTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgGenIeeeReservedMcastProfilesConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgSrcIdPortForce)
{
/********************************************************************
    Test - Port force source ID assigment

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfBrgSrcIdPortForceConfigurationSet();

    /* Generate traffic */
    prvTgfBrgSrcIdPortForceTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgSrcIdPortForceConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgSrcIdFdbSaAssigment)
{
/********************************************************************
    Test - FDB SA-based source ID assigment

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfBrgSrcIdFdbSaAssigmentConfigurationSet();

    /* Generate traffic */
    prvTgfBrgSrcIdFdbSaAssigmentTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgSrcIdFdbSaAssigmentConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgSrcIdFdbDaAssigment)
{
/********************************************************************
    Test - FDB DA-based source ID assigment

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfBrgSrcIdFdbDaAssigmentConfigurationSet();

    /* Generate traffic */
    prvTgfBrgSrcIdFdbDaAssigmentTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgSrcIdFdbDaAssigmentConfigurationRestore();
}


UTF_TEST_CASE_MAC(prvTgfVlanTag1RemIfZeroDisabled)
{
/********************************************************************
    Test - VLAN Tag1 Removal If VID=0 disabled.
           Ingress packet tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_LION_E);

    /* The feature is not supported for xCat revision 1,2 */
    if(PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->revision < 3) 
        {
            return;
        }
    }

    if(GT_FALSE == prvTgfBrgTtiCheck())
    {
        return;
    }

    /* Set configuration */
    prvTgfVlanTag1RemIfZeroConfigurationSet();

    /* Generate traffic */
    prvTgfVlanTag1RemIfZeroDisabledTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanTag1RemIfZeroConfigurationRestore();
}


UTF_TEST_CASE_MAC(prvTgfVlanTag1RemIfZeroIngrPktSingleTagged)
{
/********************************************************************
    Test - VLAN Tag1 Removal If VID=0 enabled.
           Ingress packet single tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_LION_E);

    /* The feature is not supported for xCat revision 1,2 */
    if(PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->revision < 3) 
        {
            return;
        }
    }

    if(GT_FALSE == prvTgfBrgTtiCheck())
    {
        return;
    }

    /* Set configuration */
    prvTgfVlanTag1RemIfZeroConfigurationSet();

    /* Generate traffic */
    prvTgfVlanTag1RemIfZeroSingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanTag1RemIfZeroConfigurationRestore();
}


UTF_TEST_CASE_MAC(prvTgfVlanTag1RemIfZeroIngrPktDoubleTagged)
{
/********************************************************************
    Test - VLAN Tag1 Removal If VID=0 enabled.
           Ingress packet double tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_LION_E);

    /* The feature is not supported for xCat revision 1,2 */
    if(PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->revision < 3) 
        {
            return;
        }
    }

    if(GT_FALSE == prvTgfBrgTtiCheck())
    {
        return;
    }

    /* Set configuration */
    prvTgfVlanTag1RemIfZeroConfigurationSet();

    /* Generate traffic */
    prvTgfVlanTag1RemIfZeroDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanTag1RemIfZeroConfigurationRestore();
}


UTF_TEST_CASE_MAC(prvTgfVlanTag1RemIfZeroIngrPktUntagged)
{
/********************************************************************
    Test - VLAN Tag1 Removal If VID=0 enabled.
           Ingress packet untagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_LION_E);

    /* The feature is not supported for xCat revision 1,2 */
    if(PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->revision < 3) 
        {
            return;
        }
    }

    if(GT_FALSE == prvTgfBrgTtiCheck())
    {
        return;
    }

    /* Set configuration */
    prvTgfVlanTag1RemIfZeroConfigurationSet();

    /* Generate traffic */
    prvTgfVlanTag1RemIfZeroUntagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanTag1RemIfZeroConfigurationRestore();
}


UTF_TEST_CASE_MAC(prvTgfVlanTag1RemIfZeroDoubleTagDisable)
{
/********************************************************************
    Test - VLAN Tag1 Removal If VID=0 enabled for not double tagged
           packets.
           Ingress packet double tagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_LION_E);

    /* The feature is not supported for xCat revision 1,2 */
    if(PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->revision < 3) 
        {
            return;
        }
    }

    if(GT_FALSE == prvTgfBrgTtiCheck())
    {
        return;
    }

    /* Set configuration */
    prvTgfVlanTag1RemIfZeroConfigurationSet();

    /* Generate traffic */
    prvTgfVlanTag1RemIfZeroDoubleTagDisableTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanTag1RemIfZeroConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfVlanTag1RemIfZeroTag1AssignmentIsNotZero)
{
/********************************************************************
    Test - VLAN Tag1 Removal If VID=0 enabled.
           packets.
           Ingress packet double tagged
           Tag1 Vlan TTI assignment is not zero

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_LION_E);

    /* The feature is not supported for xCat revision 1,2 */
    if(PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->revision < 3) 
        {
            return;
        }
    }

    if(GT_FALSE == prvTgfBrgTtiCheck())
    {
        return;
    }

    /* Set configuration */
    prvTgfVlanTag1RemIfZeroConfigurationSet();

    /* Generate traffic */
    prvTgfVlanTag1RemIfZeroTag1AssignmentIsNotZeroTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanTag1RemIfZeroConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfVlanKeepVlan1EnTagStateVlan1Vlan0DoubleTag)
{
/********************************************************************
    Test - Keep Vlan Tag1 enabled.
           Tag state is Vlan1&Vlan0
           packet ingress with double tag {Tag0, Tag1}

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Set configuration */
    prvTgfVlanKeepVlan1ConfigurationSet();

    /* Generate traffic */
    prvTgfVlanKeepVlan1EnTagStateVlan1Vlan0DoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanKeepVlan1ConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfVlanKeepVlan1EnTagStateVlan0DoubleTag)
{
/********************************************************************
    Test - Keep Vlan Tag1 enabled.
           Tag state is Vlan0
           packet ingress with double tag {Tag0, Tag1}

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Set configuration */
    prvTgfVlanKeepVlan1ConfigurationSet();

    /* Generate traffic */
    prvTgfVlanKeepVlan1EnTagStateVlan0DoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanKeepVlan1ConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfVlanKeepVlan1EnTagStateUntaggedDoubleTag)
{
/********************************************************************
    Test - Keep Vlan Tag1 enabled.
           Tag state is untagged
           packet ingress with double tag {Tag0, Tag1}

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Set configuration */
    prvTgfVlanKeepVlan1ConfigurationSet();

    /* Generate traffic */
    prvTgfVlanKeepVlan1EnTagStateUntaggedDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanKeepVlan1ConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfVlanKeepVlan1EnTagStateVlan0SingleTag)
{
/********************************************************************
    Test - Keep Vlan Tag1 enabled.
           Tag state is Vlan0
           packet ingress with single tag {Tag0}

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Set configuration */
    prvTgfVlanKeepVlan1ConfigurationSet();

    /* Generate traffic */
    prvTgfVlanKeepVlan1EnTagStateVlan0SingleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanKeepVlan1ConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfVlanKeepVlan1DisTagStateVlan0DoubleTag)
{
/********************************************************************
    Test - Keep Vlan Tag1 disabled.
           Tag state is Vlan0
           packet ingress with single tag {Tag0}

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Set configuration */
    prvTgfVlanKeepVlan1ConfigurationSet();

    /* Generate traffic */
    prvTgfVlanKeepVlan1DisTagStateVlan0DoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanKeepVlan1ConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfVlanKeepVlan1DisTagStateUntaggedDoubleTag)
{
/********************************************************************
    Test - Keep Vlan Tag1 disabled.
           Tag state is untagged
           packet ingress untagged

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Set configuration */
    prvTgfVlanKeepVlan1ConfigurationSet();

    /* Generate traffic */
    prvTgfVlanKeepVlan1DisTagStateUntaggedDoubleTagTrafficGenerate();

    /* Restore configuration */
    prvTgfVlanKeepVlan1ConfigurationRestore();
}


UTF_TEST_CASE_MAC(prvTgfBrgGenIeeeReservedMcastLearningEnable)
{
/********************************************************************
    Test - IEEE Reserved Multicast Learning for trapped packets 
           is enabled.
           Packet command is Trap to CPU
           packet is trapped to CPU and is learned

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfBrgGenIeeeReservedMcastLearningConfigurationSet();

    /* Generate traffic */
    prvTgfBrgGenIeeeReservedMcastLearningEnableTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgGenIeeeReservedMcastLearningConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgGenIeeeReservedMcastLearningDisable)
{
/********************************************************************
    Test - IEEE Reserved Multicast Learning for trapped packets 
           is disabled.
           Packet command is Trap to CPU
           packet is trapped to CPU and is not learned

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfBrgGenIeeeReservedMcastLearningConfigurationSet();

    /* Generate traffic */
    prvTgfBrgGenIeeeReservedMcastLearningDisableTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgGenIeeeReservedMcastLearningConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgGenIeeeReservedMcastLearningDisableTrapDisable)
{
/********************************************************************
    Test - IEEE Reserved Multicast Learning for trapped packets 
           is disabled.
           Packet command is Forward
           packet is not trapped to CPU and is learned

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfBrgGenIeeeReservedMcastLearningConfigurationSet();

    /* Generate traffic */
    prvTgfBrgGenIeeeReservedMcastLearningDisableTrapDisableTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgGenIeeeReservedMcastLearningConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgGenRateLimitMcastRegEnableRegPkts)
{
/********************************************************************
    Test - Enable Rate Limit on ingress port for multicast 
           registered packets.
           Rate Limit is set to 1.
           Send 2 Registered packets
           One packet is dropped.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfBrgGenRateLimitMcastRegConfigurationSet();

    /* Generate traffic */
    prvTgfBrgGenRateLimitMcastRegEnableRegTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgGenRateLimitMcastRegConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgGenRateLimitMcastRegDisableRegPkts)
{
/********************************************************************
    Test - Disable Rate Limit on ingress port for multicast 
           registered packets.
           Rate Limit is set to 1.
           Send 2 Registered packets
           No packet is dropped.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfBrgGenRateLimitMcastRegConfigurationSet();

    /* Generate traffic */
    prvTgfBrgGenRateLimitMcastRegDisableRegTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgGenRateLimitMcastRegConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgGenRateLimitMcastRegEnableUnregPkts)
{
/********************************************************************
    Test - Enable Rate Limit on ingress port for multicast 
           registered packets.
           Rate Limit is set to 1.
           Send 2 Unregistered packets
           No packet is dropped.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfBrgGenRateLimitMcastRegConfigurationSet();

    /* Generate traffic */
    prvTgfBrgGenRateLimitMcastRegEnableUnregTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgGenRateLimitMcastRegConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfBrgGenRateLimitMcastRegDisableUnregEnableRegPkts)
{
/********************************************************************
    Test - Disable Rate Limit on ingress port for multicast 
           registered packets.
           Enable Rate Limit on ingress port for multicast 
           unregistered packets.
           Rate Limit is set to 1.
           Send 2 Registered packets
           No packet is dropped.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfBrgGenRateLimitMcastRegConfigurationSet();

    /* Generate traffic */
    prvTgfBrgGenRateLimitMcastRegDisableUnregEnableRegTrafficGenerate();

    /* Restore configuration */
    prvTgfBrgGenRateLimitMcastRegConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfFdbAuNaMessageExtFormatWithTag1DueToNewAddr)
{
/********************************************************************
    Test - Disable learning.
           Send double tagged packet with unknown unicast.
           Send from port1 double tagged packet:
           The AU NA message is updated correctly including the extended 
           parameters: vid1, up0 and isMoved.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfFdbAuNaMessageExtFormatConfigurationSet();

    /* Generate traffic */
    prvTgfFdbAuNaMessageExtFormatWithTag1DueToNewAddrTrafficGenerate();

    /* Restore configuration */
    prvTgfFdbAuNaMessageExtFormatConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfFdbAuNaMessageExtFormatDueToMovedPort)
{
/********************************************************************
    Test -  Learn MAC on FDB on port2.
            Disable learning.
            Send single tagged packet with known unicast, but from  
            different source (port).
            Send from port1 single tagged packet:
            The AU NA message is updated correctly including the  
            extended parameters: up0, isMoved oldSrcId, 
            oldDstInterface and oldAssociatedDevNum.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfFdbAuNaMessageExtFormatConfigurationSet();

    /* Generate traffic */
    prvTgfFdbAuNaMessageExtFormatDueToMovedPortTrafficGenerate();

    /* Restore configuration */
    prvTgfFdbAuNaMessageExtFormatConfigurationRestore();
}

UTF_TEST_CASE_MAC(prvTgfFdbAuNaMessageExtFormatDueToMovedTrunk)
{
/********************************************************************
    Test -  Learn MAC on FDB on trunk.
            Disable learning.
            Send single tagged packet with known unicast, but from 
            different source (port).
            Send from port1 single tagged packet:
            The AU NA message is updated correctly including the 
            extended parameters: up0, isMoved oldSrcId, 
            oldDstInterface and oldAssociatedDevNum.

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | 
                                        UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Set configuration */
    prvTgfFdbAuNaMessageExtFormatConfigurationSet();

    /* Generate traffic */
    prvTgfFdbAuNaMessageExtFormatDueToMovedTrunkTrafficGenerate();

    /* Restore configuration */
    prvTgfFdbAuNaMessageExtFormatConfigurationRestore();
}



/*
 * Configuration of tgfBridge suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfBridge)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicDynamicLearning)
    UTF_SUIT_DECLARE_TEST_MAC(tgfTransmitAllKindsOfTraffic)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBrgVlanIngressFiltering)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBrgVlanEgressFiltering)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationUntaggedCmdDoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationUntaggedCmdSingleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationUntaggedCmdUntag)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationTag0CmdDoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationTag0CmdSingleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationTag0CmdUntag)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationTag1CmdDoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationTag1CmdSingleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationTag1CmdUntag)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationOuterTag0InnerTag1CmdDoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationOuterTag0InnerTag1CmdSingleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationOuterTag0InnerTag1CmdUntag)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationOuterTag1InnerTag0CmdDoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationOuterTag1InnerTag0CmdSingleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationOuterTag1InnerTag0CmdUntag)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationPushTag0CmdDoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationPushTag0CmdSingleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationPushTag0CmdUntag)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationPopOuterTagCmdDoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationPopOuterTagCmdSingleTag)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationIngressVlanAssignment1)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationIngressVlanAssignment2)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanManipulationChangeEgressTpid)

    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanFloodVidx)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVlanFloodVidxAllFlooded)

    UTF_SUIT_DECLARE_TEST_MAC(tgfFdbAgeBitDaRefresh)

    UTF_SUIT_DECLARE_TEST_MAC(tgfBrgGenIeeeReservedMcastProfiles)

    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgSrcIdPortForce)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgSrcIdFdbSaAssigment)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgSrcIdFdbDaAssigment)

    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanTag1RemIfZeroDisabled)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanTag1RemIfZeroIngrPktSingleTagged)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanTag1RemIfZeroIngrPktDoubleTagged)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanTag1RemIfZeroIngrPktUntagged)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanTag1RemIfZeroDoubleTagDisable)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanTag1RemIfZeroTag1AssignmentIsNotZero)

    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanKeepVlan1EnTagStateVlan1Vlan0DoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanKeepVlan1EnTagStateVlan0DoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanKeepVlan1EnTagStateUntaggedDoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanKeepVlan1EnTagStateVlan0SingleTag)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanKeepVlan1DisTagStateVlan0DoubleTag)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfVlanKeepVlan1DisTagStateUntaggedDoubleTag)

    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgGenIeeeReservedMcastLearningEnable)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgGenIeeeReservedMcastLearningDisable)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgGenIeeeReservedMcastLearningDisableTrapDisable)

    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgGenRateLimitMcastRegEnableRegPkts)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgGenRateLimitMcastRegDisableRegPkts)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgGenRateLimitMcastRegEnableUnregPkts)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfBrgGenRateLimitMcastRegDisableUnregEnableRegPkts)

    UTF_SUIT_DECLARE_TEST_MAC(prvTgfFdbAuNaMessageExtFormatWithTag1DueToNewAddr)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfFdbAuNaMessageExtFormatDueToMovedPort)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfFdbAuNaMessageExtFormatDueToMovedTrunk)



UTF_SUIT_END_TESTS_MAC(tgfBridge)

