/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonIpfixUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS IPFIX
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <ipfix/prvTgfBasicIngressPclKeys.h>
#include <ipfix/prvTgfBasicTests.h>
#include <ipfix/prvTgfAging.h>
#include <ipfix/prvTgfWraparound.h>
#include <ipfix/prvTgfDropMode.h>
#include <ipfix/prvTgfSampling.h>
#include <ipfix/prvTgfTimersUpload.h>
#include <ipfix/prvTgfPortGroupEntry.h>
#include <ipfix/prvTgfPortGroupAging.h>
#include <ipfix/prvTgfPortGroupWraparound.h>
#include <ipfix/prvTgfPortGroupAlarms.h>

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixHelloTest
*/
UTF_TEST_CASE_MAC(tgfIpfixHelloTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixHelloTestInit,              /* Set configuration */
                                 prvTgfIpfixHelloTestTrafficGenerate,   /* Generate traffic  */
                                 prvTgfIpfixHelloTestRestore);          /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixFirstIngressTest
*/
UTF_TEST_CASE_MAC(tgfIpfixFirstIngressTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixFirstIngressTestInit,         /* Set configuration */
                                 prvTgfIpfixIngressTestTrafficGenerate,   /* Generate traffic  */
                                 prvTgfIpfixIngressTestRestore);          /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixFirstUseAllIngressTest
*/
UTF_TEST_CASE_MAC(tgfIpfixFirstUseAllIngressTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixFirstUseAllIngressTestInit, /* Set configuration */
                                 prvTgfIpfixIngressTestTrafficGenerate, /* Generate traffic  */
                                 prvTgfIpfixIngressTestRestore);        /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixSecondUseAllIngressTest
*/
UTF_TEST_CASE_MAC(tgfIpfixSecondUseAllIngressTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixSecondUseAllIngressTestInit,    /* Set configuration */
                                 prvTgfIpfixIngressTestTrafficGenerate,     /* Generate traffic  */
                                 prvTgfIpfixIngressTestRestore);            /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixTimestampVerificationTest
*/
UTF_TEST_CASE_MAC(tgfIpfixTimestampVerificationTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixTimestampVerificationTestInit,              /* Set configuration */
                                 prvTgfIpfixTimestampVerificationTestTrafficGenerate,   /* Generate traffic  */
                                 prvTgfIpfixIngressTestRestore);                        /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixAgingTest
*/
UTF_TEST_CASE_MAC(tgfIpfixAgingTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixAgingTestInit,              /* Set configuration */
                                 prvTgfIpfixAgingTestTrafficGenerate,   /* Generate traffic  */
                                 prvTgfIpfixIngressTestRestore);        /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixWraparoundFreezeTest
*/
UTF_TEST_CASE_MAC(tgfIpfixWraparoundFreezeTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    /* Set configuration */
    prvTgfIpfixWraparoundTestInit();

    /* Generate traffic */
    prvTgfIpfixWraparoundFreezeTestTrafficGenerate();

    /* Restore configuration */
    prvTgfIpfixWraparoundTestRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixWraparoundClearTest
*/
UTF_TEST_CASE_MAC(tgfIpfixWraparoundClearTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    /* Set configuration */
    prvTgfIpfixWraparoundTestInit();

    /* Generate traffic */
    prvTgfIpfixWraparoundClearTestTrafficGenerate();

    /* Restore configuration */
    prvTgfIpfixWraparoundTestRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixWraparoundMaxBytesTest
*/
UTF_TEST_CASE_MAC(tgfIpfixWraparoundMaxBytesTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    /* Set configuration */
    prvTgfIpfixWraparoundTestInit();

    /* Generate traffic */
    prvTgfIpfixWraparoundMaxBytesTestTrafficGenerate();

    /* Restore configuration */
    /*prvTgfIpfixWraparoundTestRestore();*/
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixAgingTest
*/
UTF_TEST_CASE_MAC(tgfIpfixDropModeSimpleTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixDropModeSimpleTestInit,             /* Set configuration */
                                 prvTgfIpfixDropModeSimpleTestTrafficGenerate,  /* Generate traffic  */
                                 prvTgfIpfixIngressTestRestore);                /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixSamplingSimpleTest
*/
UTF_TEST_CASE_MAC(tgfIpfixSamplingSimpleTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixSamplingSimpleTestInit,             /* Set configuration */
                                 prvTgfIpfixSamplingSimpleTestTrafficGenerate,  /* Generate traffic  */
                                 prvTgfIpfixSamplingTestRestore);               /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixSamplingToCpuTest
*/
UTF_TEST_CASE_MAC(tgfIpfixSamplingToCpuTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixSamplingToCpuTestInit,              /* Set configuration */
                                 prvTgfIpfixSamplingToCpuTestTrafficGenerate,   /* Generate traffic  */
                                 prvTgfIpfixSamplingTestRestore);               /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixAlarmEventsTest
*/
UTF_TEST_CASE_MAC(tgfIpfixAlarmEventsTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    prvTgfIpfixTrafficGenManager(prvTgfIpfixAlarmEventsTestInit,            /* Set configuration */
                                 prvTgfIpfixAlarmEventsTestTrafficGenerate, /* Generate traffic  */
                                 prvTgfIpfixSamplingTestRestore);           /* Restore configuration */

}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixTimersUploadAbsoluteTest
*/
UTF_TEST_CASE_MAC(tgfIpfixTimersUploadAbsoluteTest)
{
/********************************************************************
    1. Upload timers
    2. Restore
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    /* Upload timers */
    prvTgfIpfixTimersUploadAbsoluteTest();

    /* Restore */
    prvTgfIpfixTimersUploadRestoreByClear();
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixTimersUploadIncrementalTest
*/
UTF_TEST_CASE_MAC(tgfIpfixTimersUploadIncrementalTest)
{
/********************************************************************
    1. Upload timers
    2. Restore
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    /* Upload timers */
    prvTgfIpfixTimersUploadIncrementalTest();

    /* Restore */
    prvTgfIpfixTimersUploadRestoreByClear();
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixPortGroupEntryTest
*/
UTF_TEST_CASE_MAC(tgfIpfixPortGroupEntryTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_XCAT2_E);


    prvTgfIpfixPortGroupEntryTestInit();            /* Set configuration */
    prvTgfIpfixPortGroupEntryTestTrafficGenerate(); /* Generate traffic  */
    prvTgfIpfixPortGroupEntryTestRestore();         /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixPortGroupAgingTest
*/
UTF_TEST_CASE_MAC(tgfIpfixPortGroupAgingTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_XCAT2_E);


    prvTgfIpfixPortGroupEntryTestInit();            /* Set configuration */
    prvTgfIpfixPortGroupAgingTestTrafficGenerate(); /* Generate traffic  */
    prvTgfIpfixPortGroupEntryTestRestore();         /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixPortGroupWraparoundTest
*/
UTF_TEST_CASE_MAC(tgfIpfixPortGroupWraparoundTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_XCAT2_E);


    prvTgfIpfixPortGroupEntryTestInit();                    /* Set configuration */
    prvTgfIpfixPortGroupWraparoundTestTrafficGenerate();    /* Generate traffic  */
    prvTgfIpfixPortGroupEntryTestRestore();                 /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixPortGroupAlarmsTest
*/
UTF_TEST_CASE_MAC(tgfIpfixPortGroupAlarmsTest)
{
/********************************************************************
    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                        UTF_XCAT_E | UTF_XCAT2_E);


    prvTgfIpfixPortGroupAlarmsTestInit();               /* Set configuration */
    prvTgfIpfixPortGroupAlarmsTestTrafficGenerate();    /* Generate traffic  */
    prvTgfIpfixPortGroupAlarmsTestRestore();            /* Restore configuration */
}

/*----------------------------------------------------------------------------*/
/*
    tgfIpfixTimersUploadIncrementalTest
*/
UTF_TEST_CASE_MAC(tgfIpfixTimestampToCpuTest)
{
/********************************************************************
    1. Upload timers
    2. Restore
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E |
                                        UTF_XCAT2_E);

    prvTgfIpfixTimestampToCpuTestInit();            /* Set configuration */
    prvTgfIpfixTimestampToCpuTestTrafficGenerate(); /* Generate traffic  */
    prvTgfIpfixTimestampToCpuTestRestore();         /* Restore configuration */
}

/*
 * Configuration of tgfIpfix suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfIpfix)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixHelloTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixFirstIngressTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixFirstUseAllIngressTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixSecondUseAllIngressTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixTimestampVerificationTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixAgingTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixWraparoundFreezeTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixWraparoundClearTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixWraparoundMaxBytesTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixDropModeSimpleTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixSamplingSimpleTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixSamplingToCpuTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixAlarmEventsTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixTimersUploadAbsoluteTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixTimersUploadIncrementalTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixPortGroupEntryTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixPortGroupAgingTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixPortGroupWraparoundTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixPortGroupAlarmsTest)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpfixTimestampToCpuTest)
UTF_SUIT_END_TESTS_MAC(tgfIpfix)
















