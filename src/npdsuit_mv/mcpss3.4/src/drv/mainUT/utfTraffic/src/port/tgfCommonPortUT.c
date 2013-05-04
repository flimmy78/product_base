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
*       Enhanced UTs for CPSS Port
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <port/prvTgfPortMacCountersCapture.h>
#include <port/prvTgfPortTxResourceHistogram.h>
#include <port/prvTgfPortTx.h>

/*----------------------------------------------------------------------------*/
/*
    1.1.1 tgfPortMacMibBroadcastCaptureCntr
*/
UTF_TEST_CASE_MAC(tgfPortMacMibBroadcastCaptureCntr)
{
/*
    1. Capture Broadcast traffic 
 */
    prvTgfPortMacCountersVlanConfigurationSet();
    prvTgfPortMacCountersConfigurationSet();
    prvTgPortMacCounterCaptureBroadcastTrafficGenerate();
    prvTgfPortMacCountersRestore();
}


/*
    1.1.2 tgfPortMacMibUnicastCaptureCntr
*/
UTF_TEST_CASE_MAC(tgfPortMacMibUnicastCaptureCntr)
{
/*
    1. Capture Unicast traffic 
 */
    prvTgfPortMacCountersVlanConfigurationSet();
    prvTgfPortMacCountersConfigurationSet();
    prvTgPortMacCounterCaptureUnicastTrafficGenerate();
    prvTgfPortMacCountersRestore();
}


/*
    1.1.3 tgfPortMacMibMulticastCaptureCntr
*/
UTF_TEST_CASE_MAC(tgfPortMacMibMulticastCaptureCntr)
{
/*
    1. Capture Multicast traffic 
 */
    prvTgfPortMacCountersVlanConfigurationSet();
    prvTgfPortMacCountersConfigurationSet();
    prvTgPortMacCounterCaptureMulticastTrafficGenerate();
    prvTgfPortMacCountersRestore();
}


/*
    1.1.3 tgfPortMacMibOversizeCaptureCntr
*/
UTF_TEST_CASE_MAC(tgfPortMacMibOversizeCaptureCntr)
{
/*
    1. Capture Oversize traffic 
 */
    prvTgfPortMacCountersVlanConfigurationSet();
    prvTgfPortMacCountersConfigurationSet();
    prvTgPortMacCounterCaptureOversizeTrafficGenerate();
    prvTgfPortMacCountersRestore();
}


/*
    1.2.1 tgfPortMacMibHistogramCaptureCntr
*/
UTF_TEST_CASE_MAC(tgfPortMacMibHistogramCaptureCntr)
{
/*
    1. Generate Unicast traffic and check Histogram counters
    2. Restore Defult Settings
 */
    prvTgfPortMacCountersVlanConfigurationSet();
    prvTgfPortMacCountersConfigurationSet();
    prvTgPortMacCounterCaptureHistogramTrafficGenerate();
    prvTgfPortMacCountersRestore();
}


/*
    1.3.1 tgfPortMacMibClearOnReadCaptureCntr
*/
UTF_TEST_CASE_MAC(tgfPortMacMibClearOnReadCaptureCntr)
{
/*
    1. Generate Unicast traffic and Check Clear on Read feature
    2. Restore Defult Settings
 */
    prvTgfPortMacCountersVlanConfigurationSet();
    prvTgfPortMacCountersConfigurationSet();
    prvTgPortMacCounterClearOnReadTrafficGenerate();
    prvTgfPortMacCountersRestore();
}


/*
    tgfPortTxResourceHistogramCntr
*/
UTF_TEST_CASE_MAC(tgfPortTxResourceHistogramCntr)
{
/*
    1. Generate Unicast traffic and Check feature
    2. Restore Defult Settings
 */
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

#ifndef ASIC_SIMULATION
    prvTgfPortTxResourceHistogramVlanConfigurationSet();
    prvTgfPortTxResourceHistogramConfigurationSet();
    prvTgfPortTxResourceHistogramTrafficGenerate();
    prvTgfPortTxResourceHistogramRestore();
#endif /* ASIC_SIMULATION */
}


/*
    tgfPortTxSchedulerSp
*/
UTF_TEST_CASE_MAC(tgfPortTxSchedulerSp)
{
/*
    1. Configure egress port scheduler to work in SP arbiter 
    2. Generate wire speed Unicast traffic from 3 ports to one egress.
       Check results. Traffic Txed only from highest TC queue.
    3. Restore Default Settings
 */
#ifndef ASIC_SIMULATION
    prvTgfPortTxSpSchedulerConfigurationSet();
    prvTgfPortTxSpSchedulerTrafficGenerate();
    prvTgfPortTxSpSchedulerRestore();
#endif /* ASIC_SIMULATION */
}


/*
 * Configuration of tgfPort suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfPort)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPortMacMibBroadcastCaptureCntr)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPortMacMibUnicastCaptureCntr)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPortMacMibMulticastCaptureCntr)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPortMacMibOversizeCaptureCntr)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPortMacMibHistogramCaptureCntr)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPortMacMibClearOnReadCaptureCntr)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPortTxResourceHistogramCntr)

    UTF_SUIT_DECLARE_TEST_MAC(tgfPortTxSchedulerSp)
UTF_SUIT_END_TESTS_MAC(tgfPort)

