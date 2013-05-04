/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgStpUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgStp, that provides
*       Spanning Tree protocol facility ExMxPm CPSS implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgStp.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_STP_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define BRG_STP_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgStpEntryRead
(
    IN  GT_U8       devNum,
    IN  GT_U32      stgId,
    OUT GT_U32      stgEntryPtr[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgStpEntryRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with stgId [0 / 4095]
                   and non-NULL stgEntryPtr.
    Expected: GT_OK.
    1.2. Call with out of range stgId [PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS = 4096]
                   and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call with stgEntryPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      stgId = 0;
    GT_U32      stgEntryPtr[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with stgId [0 / 4095]
                           and non-NULL stgEntryPtr.
            Expected: GT_OK.
        */

        /* Call with stgId [0] */
        stgId = 0;

        st = cpssExMxPmBrgStpEntryRead(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        /* Call with stgId [4095] */
        stgId = 4095;

        st = cpssExMxPmBrgStpEntryRead(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        /*
            1.2. Call with out of range stgId [PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS = 4096]
                           and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        stgId = PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS;

        st = cpssExMxPmBrgStpEntryRead(dev, stgId, stgEntryPtr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        stgId = 0;

        /*
            1.3. Call with stgEntryPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgStpEntryRead(dev, stgId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, stgEntryPtr = NULL", dev);
    }

    stgId = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgStpEntryRead(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgStpEntryRead(dev, stgId, stgEntryPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgStpEntryWrite
(
    IN  GT_U8       devNum,
    IN  GT_U32      stgId,
    IN  GT_U32      stgEntryPtr[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgStpEntryWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with stgId [0 / 255]
                   and stgEntryPtr [{0, 0, 0, 0} / {0, 32, 64, 255}].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgStpEntryRead with same stgId and non-NULL stgEntryPtr.
    Expected: GT_OK and same stgEntryPtr.
    1.3. Call with stgId [2]
                   and stgEntryPtr {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF} (no any constraints).
    Expected: GT_OK.
    1.4. Call with out of range stgId [PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS = 4096]
                   and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.5. Call with stgEntryPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      stgId   = 0;
    GT_BOOL     isEqual = GT_FALSE;
    GT_U32      stgEntryPtr[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];
    GT_U32      stgEntryGetPtr[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with stgId [0 / 255]
                           and stgEntryPtr [{0, 0, 0, 0} / {0, 32, 64, 255}].
            Expected: GT_OK.
        */

        /* Call with stgId [0] */
        stgId = 0;

        stgEntryPtr[0] = 0;
        stgEntryPtr[1] = 0;
        stgEntryPtr[2] = 0;
        stgEntryPtr[3] = 0;

        st = cpssExMxPmBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        /*
            1.2. Call cpssExMxPmBrgStpEntryRead with same stgId and non-NULL stgEntryPtr.
            Expected: GT_OK and same stgEntryPtr.
        */
        st = cpssExMxPmBrgStpEntryRead(dev, stgId, stgEntryGetPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgStpEntryRead: %d, %d", dev, stgId);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) stgEntryPtr,
                                     (GT_VOID*) stgEntryGetPtr,
                                     sizeof(stgEntryPtr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another stgEntryPtr than was set: %d", dev);

        /* Call with stgId [255] */
        stgId = 255;

        stgEntryPtr[0] = 0;
        stgEntryPtr[1] = 32;
        stgEntryPtr[2] = 64;
        stgEntryPtr[3] = 255;

        st = cpssExMxPmBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        /*
            1.2. Call cpssExMxPmBrgStpEntryRead with same stgId and non-NULL stgEntryPtr.
            Expected: GT_OK and same stgEntryPtr.
        */
        st = cpssExMxPmBrgStpEntryRead(dev, stgId, stgEntryGetPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgStpEntryRead: %d, %d", dev, stgId);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) stgEntryPtr,
                                     (GT_VOID*) stgEntryGetPtr,
                                     sizeof(stgEntryPtr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another stgEntryPtr than was set: %d", dev);

        /*
            1.3. Call with stgId [2]
                           and stgEntryPtr {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF} (no any constraints).
            Expected: GT_OK.
        */
        stgId = 2;

        stgEntryPtr[0] = 0xFFFFFFFF;
        stgEntryPtr[1] = 0xFFFFFFFF;
        stgEntryPtr[2] = 0xFFFFFFFF;
        stgEntryPtr[3] = 0xFFFFFFFF;

        st = cpssExMxPmBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        /*
            1.4. Call with out of range stgId [PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS = 4096]
                           and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        stgId = PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS;

        st = cpssExMxPmBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        stgId = 0;

        /*
            1.5. Call with stgEntryPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgStpEntryWrite(dev, stgId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, stgEntryPtr = NULL", dev);
    }

    stgId = 0;

    stgEntryPtr[0] = 0;
    stgEntryPtr[1] = 0;
    stgEntryPtr[2] = 0;
    stgEntryPtr[3] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgStpEntryWrite(dev, stgId, stgEntryPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgStpPortStateSet
(
    IN GT_U8                devNum,
    IN GT_U8                portNum,
    IN GT_U32               stgId,
    IN CPSS_STP_STATE_ENT   state
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgStpPortStateSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with stgId [0 / 32 / 65 / 4095]
                     and state [CPSS_STP_DISABLED_E /
                                CPSS_STP_BLCK_LSTN_E /
                                CPSS_STP_LRN_E /
                                CPSS_STP_FRWRD_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgStpPortStateGet with same stgId and non-NULL statePtr.
    Expected: GT_OK and same state.
    1.1.3. Call with out of range stgId [PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS = 4096]
                     and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.1.4. Call with out of range state [0x5AAAAAA5]
                     and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_STP_VALID_VIRT_PORT_CNS;

    GT_U32               stgId    = 0;
    CPSS_STP_STATE_ENT   state    = CPSS_STP_DISABLED_E;
    CPSS_STP_STATE_ENT   stateGet = CPSS_STP_DISABLED_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with stgId [0 / 32 / 65 / 4095]
                                 and state [CPSS_STP_DISABLED_E /
                                            CPSS_STP_BLCK_LSTN_E /
                                            CPSS_STP_LRN_E /
                                            CPSS_STP_FRWRD_E].
                Expected: GT_OK.
            */

            /* Call with stgId [0] */
            stgId = 0;
            state = CPSS_STP_DISABLED_E;

            st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stgId, state);

            /*
                1.1.2. Call cpssExMxPmBrgStpPortStateGet with same stgId and non-NULL statePtr.
                Expected: GT_OK and same state.
            */
            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &stateGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgStpPortStateGet: %d, %d, %d", dev, port, stgId);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another state than was set: %d, %d", dev, port);

            /* Call with stgId [32] */
            stgId = 32;
            state = CPSS_STP_BLCK_LSTN_E;

            st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stgId, state);

            /*
                1.1.2. Call cpssExMxPmBrgStpPortStateGet with same stgId and non-NULL statePtr.
                Expected: GT_OK and same state.
            */
            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &stateGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgStpPortStateGet: %d, %d, %d", dev, port, stgId);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another state than was set: %d, %d", dev, port);

            /* Call with stgId [65] */
            stgId = 65;
            state = CPSS_STP_LRN_E;

            st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stgId, state);

            /*
                1.1.2. Call cpssExMxPmBrgStpPortStateGet with same stgId and non-NULL statePtr.
                Expected: GT_OK and same state.
            */
            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &stateGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgStpPortStateGet: %d, %d, %d", dev, port, stgId);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another state than was set: %d, %d", dev, port);

            /* Call with stgId [4095] */
            stgId = 4095;
            state = CPSS_STP_FRWRD_E;

            st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stgId, state);

            /*
                1.1.2. Call cpssExMxPmBrgStpPortStateGet with same stgId and non-NULL statePtr.
                Expected: GT_OK and same state.
            */
            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &stateGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgStpPortStateGet: %d, %d, %d", dev, port, stgId);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another state than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range stgId [PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS = 4096]
                                 and other parameters the same as in 1.1.
                Expected: NON GT_OK.
            */
            stgId = PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS;

            st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stgId);

            stgId = 0;

            /*
                1.1.4. Call with out of range state [0x5AAAAAA5]
                                 and other parameters the same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            state = BRG_STP_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, state = %d", dev, port, state);
        }

        stgId = 0;
        state = CPSS_STP_DISABLED_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    stgId = 0;
    state = CPSS_STP_DISABLED_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_STP_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgStpPortStateSet(dev, port, stgId, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgStpPortStateGet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN  GT_U32              stgId,
    OUT CPSS_STP_STATE_ENT  *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgStpPortStateGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call with stgId [0 / 4095]
                     and non-NULL statePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range stgId [PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS = 4096]
                     and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with statePtr [NULL]
                     and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_STP_VALID_VIRT_PORT_CNS;

    GT_U32               stgId = 0;
    CPSS_STP_STATE_ENT   state = CPSS_STP_DISABLED_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with stgId [0 / 4095]
                                 and non-NULL statePtr.
                Expected: GT_OK.
            */

            /* Call with stgId [0] */
            stgId = 0;

            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stgId);

            /* Call with stgId [255] */
            stgId = 255;

            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stgId);

            /*
                1.1.2. Call with out of range stgId [PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS = 4096]
                                 and other parameters the same as in 1.1.
                Expected: NON GT_OK.
            */
            stgId = PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS;

            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &state);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, stgId);

            stgId = 0;

            /*
                1.1.3. Call with statePtr [NULL]
                                 and other parameters the same as in 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, statePtr = NULL", dev, port);
        }

        stgId = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    stgId = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_STP_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgStpPortStateGet(dev, port, stgId, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgStpTableAccessModeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_ENT    mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgStpTableAccessModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_VID_E / CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_STG_E]
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgStpTableAccessModeGet with not NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.3. Call with mode [0x5AAAAAA5] out of range.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_ENT    mode;
    CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_ENT    modeGet;


    mode    = CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_VID_E;
    modeGet = CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_VID_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_VID_E / CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_STG_E]
            Expected: GT_OK.
        */
        /* iterate with mode = CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_VID_E */
        mode = CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_VID_E;

        st = cpssExMxPmBrgStpTableAccessModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgStpTableAccessModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmBrgStpTableAccessModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgStpTableAccessModeGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode than was set: %d", dev);

        /* iterate with mode = CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_STG_E */
        mode = CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_STG_E;

        st = cpssExMxPmBrgStpTableAccessModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgStpTableAccessModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmBrgStpTableAccessModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgStpTableAccessModeGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode than was set: %d", dev);

        /*
            1.3. Call with mode [0x5AAAAAA5] out of range.
            Expected: GT_BAD_PARAM.
        */
        mode = BRG_STP_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgStpTableAccessModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_VID_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgStpTableAccessModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgStpTableAccessModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgStpTableAccessModeGet
(
    IN GT_U8                                        devNum,
    OUT CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgStpTableAccessModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_ENT  mode;


    mode    = CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_VID_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgStpTableAccessModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgStpTableAccessModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgStpTableAccessModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgStpTableAccessModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill STP group table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgStpFillStpGroupTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in STP group table.
         Call cpssExMxPmBrgStpEntryWrite with stgId [0..numEntries - 1]
                    and stgEntryPtr [{0, 0, 0, 0}].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmBrgStpEntryWrite with stgId [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in STP group table and compare with original.
         Call cpssExMxPmBrgStpEntryRead with same stgId and non-NULL stgEntryPtr.
    Expected: GT_OK and same stgEntryPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmBrgStpEntryRead with stgId [numEntries] and other params from 1.4.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    GT_U32      stgEntryPtr[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];
    GT_U32      stgEntryGetPtr[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];

    /* Fill the entry for STP group table */
    stgEntryPtr[0] = 0;
    stgEntryPtr[1] = 0;
    stgEntryPtr[2] = 0;
    stgEntryPtr[3] = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_STG_MAX_NUM_CNS;

        /* 1.2. Fill all entries in STP group table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgStpEntryWrite(dev, iTemp, stgEntryPtr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgStpEntryWrite: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmBrgStpEntryWrite(dev, numEntries, stgEntryPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgStpEntryWrite: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in STP group table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgStpEntryRead(dev, iTemp, stgEntryGetPtr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgStpEntryRead: %d, %d", dev, iTemp);
    
            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) stgEntryPtr,
                                         (GT_VOID*) stgEntryGetPtr,
                                         sizeof(stgEntryPtr))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "get another stgEntryPtr than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmBrgStpEntryRead(dev, numEntries, stgEntryGetPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgStpEntryRead: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgStp suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgStp)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgStpEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgStpEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgStpPortStateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgStpPortStateGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgStpTableAccessModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgStpTableAccessModeGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgStpFillStpGroupTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgStp)

