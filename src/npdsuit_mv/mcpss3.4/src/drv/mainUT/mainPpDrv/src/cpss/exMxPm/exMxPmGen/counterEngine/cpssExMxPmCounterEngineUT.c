/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmCounterEngineUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmCounterEngine, that provides
*       CPSS EXMXPM counters functions, data structures, and
*       declarations of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/exMxPm/exMxPmGen/counterEngine/cpssExMxPmCounterEngine.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
extern GT_U32 prvExMxPmTestMemReadWrite;

/* Invalid enum */

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCounterEngineEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmCounterEngineEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmCounterEngineEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL enable    = GT_FALSE;
    GT_BOOL enableGet = GT_FALSE;

    prvExMxPmTestMemReadWrite = 1; /*force linker to add the C file where prvTestMemReadWrite defined to image */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmCounterEngineEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmCounterEngineEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmCounterEngineEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmCounterEngineEnableGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmCounterEngineEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmCounterEngineEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmCounterEngineEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmCounterEngineEnableGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non applicable devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCounterEngineEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCounterEngineEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCounterEngineEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmCounterEngineEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmCounterEngineEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmCounterEngineEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCounterEngineEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCounterEngineEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCounterEngineEccConfigSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT     behavior
) 
*/

UTF_TEST_CASE_MAC(cpssExMxPmCounterEngineEccConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with behavior [0 / 1].
    Expected: GT_OK.
    1.2. Call cpssExMxPmCounterEngineEccConfigGet with non-NULL behaviorPtr.
    Expected: GT_OK and the same behavior.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT     behavior;
    CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT     behaviorGet;

    behavior    = 0;
    behaviorGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with behavior [0 / 1].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        behavior = 0;

        st = cpssExMxPmCounterEngineEccConfigSet(dev, behavior);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, behavior);

        /*
            1.2. Call cpssExMxPmCounterEngineEccConfigGet with non-NULL behaviorPtr.
            Expected: GT_OK and the same behavior.
        */
        st = cpssExMxPmCounterEngineEccConfigGet(dev, &behaviorGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmCounterEngineEccConfigGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(behavior, behaviorGet,
                   "get another behavior than was set: %d", dev);

        /* Call with behavior [1] */
        behavior = 1;

        st = cpssExMxPmCounterEngineEccConfigSet(dev, behavior);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, behavior);

        /*
            1.2. Call cpssExMxPmCounterEngineEccConfigGet with non-NULL behaviorPtr.
            Expected: GT_OK and the same behavior.
        */
        st = cpssExMxPmCounterEngineEccConfigGet(dev, &behaviorGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmCounterEngineEccConfigGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(behavior, behaviorGet,
                   "get another behavior than was set: %d", dev);
    }

    behavior = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non applicable devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCounterEngineEccConfigSet(dev, behavior);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCounterEngineEccConfigSet(dev, behavior);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCounterEngineEccConfigGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT     *behaviorPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmCounterEngineEccConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with non-NULL behaviorPtr.
    Expected: GT_OK.
    1.2. Call function with behaviorPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT     behaviorPtr;
    behaviorPtr = 1;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with non-NULL behaviorPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmCounterEngineEccConfigGet(dev, &behaviorPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with behaviorPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmCounterEngineEccConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, behaviorPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCounterEngineEccConfigGet(dev, &behaviorPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCounterEngineEccConfigGet(dev, &behaviorPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCounterEngineClientEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_ENT   client,
    IN  GT_BOOL                                 enable
)
*/

UTF_TEST_CASE_MAC(cpssExMxPmCounterEngineClientEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmCounterEngineClientEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with client = 1 (not CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_TXQ_E) 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_ENT   client;
    
    GT_BOOL enable    = GT_FALSE;
    GT_BOOL enableGet = GT_FALSE;
    client = CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_TXQ_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmCounterEngineClientEnableSet(dev, client, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmCounterEngineClientEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmCounterEngineClientEnableGet(dev, client, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmCounterEngineClientEnableGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmCounterEngineClientEnableSet(dev, client, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmCounterEngineClientEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmCounterEngineClientEnableGet(dev, client, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmCounterEngineClientEnableGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /*
            1.3. Call with client = 1 (not CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_TXQ_E) 
            Expected: GT_BAD_PARAM.
        */
        client = 1;
        st = cpssExMxPmCounterEngineClientEnableSet(dev, client, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        client = CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_TXQ_E;

    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non applicable devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCounterEngineClientEnableSet(dev, client, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCounterEngineClientEnableSet(dev, client, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCounterEngineClientEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_ENT   client,
    OUT GT_BOOL                                 *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmCounterEngineClientEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with client = 1 (not CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_TXQ_E) 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_ENT   client;
    GT_BOOL state = GT_FALSE;
    client = CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_TXQ_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmCounterEngineClientEnableGet(dev, client, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmCounterEngineClientEnableGet(dev, client, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);

        /*
            1.3. Call with client = 1 (not CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_TXQ_E) 
            Expected: GT_BAD_PARAM.
        */
        client = 1;
        st = cpssExMxPmCounterEngineClientEnableGet(dev, client, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        client = CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_TXQ_E; 

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCounterEngineClientEnableGet(dev, client, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCounterEngineClientEnableGet(dev, client, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCounterEngineCounterGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      counterIndex,
    OUT CPSS_EXMXPM_COUNTER_ENGINE_COUNTER_STC      *counterPtr
)
*/

UTF_TEST_CASE_MAC(cpssExMxPmCounterEngineCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with non-NULL counterPtr.
    Expected: GT_OK.
    1.2. Call function with counterPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with counterIndex = 0x1001 (wrong value) 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      counterIndex;
    CPSS_EXMXPM_COUNTER_ENGINE_COUNTER_STC      counterPtr;
    counterIndex = 0; /*correct value*/


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with non-NULL counterPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmCounterEngineCounterGet(dev, counterIndex, &counterPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with counterPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmCounterEngineCounterGet(dev, counterIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterPtr = NULL", dev);

        /*
            1.3. Call with counterIndex = 0x1001 (wrong value) 
            Expected: GT_BAD_PARAM.
        */
        counterIndex = 0x1001;
        st = cpssExMxPmCounterEngineCounterGet(dev, counterIndex, &counterPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        counterIndex = 0;

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCounterEngineCounterGet(dev, counterIndex, &counterPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCounterEngineCounterGet(dev, counterIndex, &counterPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCounterEngineEccErrorInfoGet
(
    IN  GT_U8                                       devNum,
    OUT CPSS_EXMXPM_COUNTER_ENGINE_ECC_INFO_STC     *errorPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmCounterEngineEccErrorInfoGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_EXMXPM_COUNTER_ENGINE_ECC_INFO_STC     errorPtr;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmCounterEngineEccErrorInfoGet(dev, &errorPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmCounterEngineEccErrorInfoGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCounterEngineEccErrorInfoGet(dev, &errorPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCounterEngineEccErrorInfoGet(dev, &errorPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/





/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmCounter suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmCounter)

    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCounterEngineEccErrorInfoGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCounterEngineCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCounterEngineClientEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCounterEngineClientEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCounterEngineEccConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCounterEngineEccConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCounterEngineEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCounterEngineEnableGet)


UTF_SUIT_END_TESTS_MAC(cpssExMxPmCounter)

