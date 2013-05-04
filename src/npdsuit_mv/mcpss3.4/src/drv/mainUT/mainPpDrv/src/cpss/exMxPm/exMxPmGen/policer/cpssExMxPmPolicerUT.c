/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmPolicerUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmPolicer, that provides
*       CPSS ExMxPm Policing Engine API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <cpss/exMxPm/exMxPmGen/policer/cpssExMxPmPolicer.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define POLICER_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define POLICER_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define POLICER_TESTED_VLAN_ID_CNS  100

/* Internal functions forward declaration */
static GT_STATUS prvUtfExMxPmMaxNumOfPolicerEntriesGet(IN GT_U8 dev, OUT GT_U32 *maxNumOfPolicerEntriesPtr);

/* Traffic flow direction  */
CPSS_DIRECTION_ENT mainUtCpssExMxPmPolicerDirection = CPSS_DIRECTION_INGRESS_E;

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringEnableSet
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerMeteringEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmPolicerMeteringEnableSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPolicerMeteringEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerMeteringEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmPolicerMeteringEnableSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPolicerMeteringEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerMeteringEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringEnableSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringEnableSet(dev, mainUtCpssExMxPmPolicerDirection,
                                            state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringEnableGet
(
    IN  GT_U8    dev,
    OUT GT_BOOL  *enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                NULL);
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
        st = cpssExMxPmPolicerMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerConfigSet
(
    IN GT_U8                                    dev,
    IN CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT    mode,
    IN GT_U32                                   numOfCountingEntries

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_EXMXPM_POLICER_COUNTING_BILLING_E /
                         CPSS_EXMXPM_POLICER_COUNTING_POLICY_E /
                         CPSS_EXMXPM_POLICER_COUNTING_VLAN_E]
                   and numOfCountingEntries [1 / 64 / 256].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerConfigGet with non-NULL modePtr and numOfCountingEntriesPtr.
    Expected: GT_OK and the same mode and numOfCountingEntries.
    1.3. Call with out of range mode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range numOfCountingEntries [0 / 0x2001]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with mode [CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E]
                   and out of range numOfCountingEntries [0x2001] (ignored)
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT    mode         = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;
    GT_U32                                   numOfEntries = 0;
    CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT    modeGet      = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;
    GT_U32                                   numOfEntrGet = 0;
    CPSS_EXMXPM_POLICER_CONFIG_STC           policerConf;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_EXMXPM_POLICER_COUNTING_BILLING_E /
                                 CPSS_EXMXPM_POLICER_COUNTING_POLICY_E /
                                 CPSS_EXMXPM_POLICER_COUNTING_VLAN_E]
                           and numOfCountingEntries [1 / 64 / 255].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_POLICER_COUNTING_BILLING_E], numOfCountingEntries [1] */
        policerConf.ingrCountingEntries = 1;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_BILLING_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries);

        /*
            1.2. Call cpssExMxPmPolicerConfigGet with non-NULL modePtr and numOfCountingEntriesPtr.
            Expected: GT_OK and the same mode and numOfCountingEntries.
        */
        st = cpssExMxPmPolicerConfigGet(dev, &policerConf);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfEntries, numOfEntrGet,
                       "get another numOfCountingEntries than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_POLICER_COUNTING_POLICY_E], numOfCountingEntries [64] */
        policerConf.ingrCountingEntries = 64;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_POLICY_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries);

        /*
            1.2. Call cpssExMxPmPolicerConfigGet with non-NULL modePtr and numOfCountingEntriesPtr.
            Expected: GT_OK and the same mode and numOfCountingEntries.
        */
        st = cpssExMxPmPolicerConfigGet(dev, &policerConf);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfEntries, numOfEntrGet,
                       "get another numOfCountingEntries than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_POLICER_COUNTING_VLAN_E], numOfCountingEntries [256] */
        policerConf.ingrCountingEntries = 256;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_VLAN_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries);

        /*
            1.2. Call cpssExMxPmPolicerConfigGet with non-NULL modePtr and numOfCountingEntriesPtr.
            Expected: GT_OK and the same mode and numOfCountingEntries.
        */
        st = cpssExMxPmPolicerConfigGet(dev, &policerConf);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfEntries, numOfEntrGet,
                       "get another numOfCountingEntries than was set: %d", dev);

        /*
            1.3. Call with out of range mode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        policerConf.ingrCountingEntries = 1;
        policerConf.ingrCountingMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);

        mode = CPSS_EXMXPM_POLICER_COUNTING_VLAN_E;

        /*
            1.4. Call with out of range numOfCountingEntries [0 / 257]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with numOfCountingEntries [0] */
        policerConf.ingrCountingEntries = 0;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries);

        /* Call with numOfCountingEntries [0x2001] */
        policerConf.ingrCountingEntries = 0x2001;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries);

        /*
            1.5. Call with mode [CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E]
                           and out of range numOfCountingEntries [0x2001] (ignored)
            Expected: GT_OK.
        */
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries);
    }

    policerConf.ingrCountingEntries = 1;
    policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerConfigGet
(
    IN  GT_U8                                   dev,
    OUT CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT   *modePtr,
    OUT GT_U32                                  *numOfCountingEntriesPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL modePtr and numOfCountingEntriesPtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with numOfCountingEntriesPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_CONFIG_STC          policerConf;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL modePtr and numOfCountingEntriesPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerConfigGet(dev, &policerConf);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with modePtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerConfigGet(dev, &policerConf);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerConfigGet(dev, &policerConf);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPacketSizeModeSet
(
    IN GT_U8                                dev,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT    packetSize
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPacketSizeModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with packetSize [CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E /
                                        CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E /
                                        CPSS_POLICER_PACKET_SIZE_L3_ONLY_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerPacketSizeModeGet with non-NULL packetSizePtr.
    Expected: GT_OK and the same packetSize.
    1.3. Call with packetSize [CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E] (not supported).
    Expected: NON GT_OK.
    1.4. Call function with out of range packetSize [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_POLICER_PACKET_SIZE_MODE_ENT packetSize    = CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E;
    CPSS_POLICER_PACKET_SIZE_MODE_ENT packetSizeGet = CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with packetSize [CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E /
                                                CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E /
                                                CPSS_POLICER_PACKET_SIZE_L3_ONLY_E].
            Expected: GT_OK.
        */

        /* Call with packetSize [CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E] */
        packetSize = CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E;

        st = cpssExMxPmPolicerPacketSizeModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                packetSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetSize);

        /*
            1.2. Call cpssExMxPmPolicerPacketSizeModeGet with non-NULL packetSizePtr.
            Expected: GT_OK and the same packetSize.
        */
        st = cpssExMxPmPolicerPacketSizeModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &packetSizeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPacketSizeModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(packetSize, packetSizeGet,
                       "get another packetSize than was set: %d", dev);

        /* Call with packetSize [CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E] */
        packetSize = CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E;

        st = cpssExMxPmPolicerPacketSizeModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                packetSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetSize);

        /*
            1.2. Call cpssExMxPmPolicerPacketSizeModeGet with non-NULL packetSizePtr.
            Expected: GT_OK and the same packetSize.
        */
        st = cpssExMxPmPolicerPacketSizeModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &packetSizeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPacketSizeModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(packetSize, packetSizeGet,
                       "get another packetSize than was set: %d", dev);

        /* Call with packetSize [CPSS_POLICER_PACKET_SIZE_L3_ONLY_E] */
        packetSize = CPSS_POLICER_PACKET_SIZE_L3_ONLY_E;

        st = cpssExMxPmPolicerPacketSizeModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                packetSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetSize);

        /*
            1.2. Call cpssExMxPmPolicerPacketSizeModeGet with non-NULL packetSizePtr.
            Expected: GT_OK and the same packetSize.
        */
        st = cpssExMxPmPolicerPacketSizeModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &packetSizeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPacketSizeModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(packetSize, packetSizeGet,
                       "get another packetSize than was set: %d", dev);

        /*
            1.3. Call with packetSize [CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E] (not supported).
            Expected: NON GT_OK.
        */
        packetSize = CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E;

        st = cpssExMxPmPolicerPacketSizeModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                packetSize);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetSize);

        /*
            1.3. Call function with out of range packetSize [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        packetSize = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerPacketSizeModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                packetSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, packetSize);
    }

    packetSize = CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPacketSizeModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                packetSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPacketSizeModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                            packetSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPacketSizeModeGet
(
    IN  GT_U8                                dev,
    OUT CPSS_POLICER_PACKET_SIZE_MODE_ENT    *packetSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPacketSizeModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL packetSizePtr.
    Expected: GT_OK.
    1.2. Call with packetSizePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_POLICER_PACKET_SIZE_MODE_ENT  packetSize = CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null packetSizePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerPacketSizeModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &packetSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null packetSizePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerPacketSizeModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetSizePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPacketSizeModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                &packetSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPacketSizeModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            &packetSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerVlanCountingModeSet
(
    IN GT_U8                                    dev,
    IN CPSS_EXMXPM_POLICER_VLAN_COUNT_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerVlanCountingModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with mode [CPSS_EXMXPM_POLICER_VLAN_COUNT_PACKET_E /
                                  CPSS_EXMXPM_POLICER_VLAN_COUNT_BYTE_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerVlanCountingModeGet with non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_VLAN_COUNT_MODE_ENT mode    = CPSS_EXMXPM_POLICER_VLAN_COUNT_PACKET_E;
    CPSS_EXMXPM_POLICER_VLAN_COUNT_MODE_ENT modeGet = CPSS_EXMXPM_POLICER_VLAN_COUNT_PACKET_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_EXMXPM_POLICER_VLAN_COUNT_PACKET_E /
                                          CPSS_EXMXPM_POLICER_VLAN_COUNT_BYTE_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_POLICER_VLAN_COUNT_PACKET_E] */
        mode = CPSS_EXMXPM_POLICER_VLAN_COUNT_PACKET_E;

        st = cpssExMxPmPolicerVlanCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingModeGet with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmPolicerVlanCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_POLICER_VLAN_COUNT_BYTE_E] */
        mode = CPSS_EXMXPM_POLICER_VLAN_COUNT_BYTE_E;

        st = cpssExMxPmPolicerVlanCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingModeGet with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmPolicerVlanCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerVlanCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_POLICER_VLAN_COUNT_PACKET_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerVlanCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerVlanCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                              mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerVlanCountingModeGet
(
    IN  GT_U8                                       dev,
    OUT CPSS_EXMXPM_POLICER_VLAN_COUNT_MODE_ENT     *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerVlanCountingModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_VLAN_COUNT_MODE_ENT  mode = CPSS_EXMXPM_POLICER_VLAN_COUNT_PACKET_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerVlanCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerVlanCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerVlanCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerVlanCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet
(
    IN GT_U8                dev,
    IN CPSS_PACKET_CMD_ENT  cmdTrigger,
    IN GT_BOOL              enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cmdTrigger [CPSS_PACKET_CMD_FORWARD_E /
                               CPSS_PACKET_CMD_DROP_HARD_E /
                               CPSS_PACKET_CMD_DROP_SOFT_E /
                               CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                               CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with cmdTrigger [CPSS_PACKET_CMD_ROUTE_E /
                               CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                               CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                               CPSS_PACKET_CMD_BRIDGE_E /
                               CPSS_PACKET_CMD_NONE_E] (not supported)
                   and enable [GT_FALSE and GT_TRUE].
    Expected: NOT GT_OK.
    1.4. Call with out of range cmdTrigger [0x5AAAAAA5]
                   and enable [GT_TRUE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmdTrigger = CPSS_PACKET_CMD_FORWARD_E;
    GT_BOOL              enable     = GT_FALSE;
    GT_BOOL              enableGet  = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cmdTrigger [CPSS_PACKET_CMD_FORWARD_E /
                                       CPSS_PACKET_CMD_DROP_HARD_E /
                                       CPSS_PACKET_CMD_DROP_SOFT_E /
                                       CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with cmdTrigger[CPSS_PACKET_CMD_FORWARD_E], enable [GT_FALSE] */
        enable     = GT_FALSE;
        cmdTrigger = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_DROP_HARD_E], enable [GT_FALSE] */
        cmdTrigger = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_DROP_SOFT_E], enable [GT_FALSE] */
        cmdTrigger = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_TRAP_TO_CPU_E], enable [GT_FALSE] */
        cmdTrigger = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_MIRROR_TO_CPU_E], enable [GT_FALSE] */
        cmdTrigger = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_FORWARD_E], enable [GT_TRUE] */
        enable     = GT_TRUE;
        cmdTrigger = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_DROP_HARD_E], enable [GT_TRUE] */
        cmdTrigger = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_DROP_SOFT_E], enable [GT_TRUE] */
        cmdTrigger = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_TRAP_TO_CPU_E], enable [GT_TRUE] */
        cmdTrigger = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_MIRROR_TO_CPU_E], enable [GT_TRUE] */
        cmdTrigger = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cmdTrigger, enable);

        /*
            1.2. Call cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet with same cmdTrigger and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet: %d, %d", dev, cmdTrigger);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /*
            1.3. Call with cmdTrigger [CPSS_PACKET_CMD_ROUTE_E /
                                       CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                                       CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                       CPSS_PACKET_CMD_BRIDGE_E /
                                       CPSS_PACKET_CMD_NONE_E] (not supported)
                           and enable [GT_FALSE and GT_TRUE].
            Expected: NOT GT_OK.
        */
        enable = GT_TRUE;

        /* Call with cmdTrigger[CPSS_PACKET_CMD_ROUTE_E] */
        cmdTrigger = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E] */
        cmdTrigger = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E] */
        cmdTrigger = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_BRIDGE_E] */
        cmdTrigger = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_NONE_E] */
        cmdTrigger = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /*
            1.4. Call with out of range cmdTrigger [0x5AAAAAA5]
                           and enable [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        cmdTrigger = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cmdTrigger);
    }

    enable     = GT_TRUE;
    cmdTrigger = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          cmdTrigger, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet
(
    IN  GT_U8                   dev,
    IN  CPSS_PACKET_CMD_ENT     cmdTrigger,
    OUT GT_BOOL                 *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cmdTrigger [CPSS_PACKET_CMD_FORWARD_E /
                               CPSS_PACKET_CMD_DROP_HARD_E /
                               CPSS_PACKET_CMD_DROP_SOFT_E /
                               CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                               CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                   and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with cmdTrigger [CPSS_PACKET_CMD_ROUTE_E /
                               CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                               CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                               CPSS_PACKET_CMD_BRIDGE_E /
                               CPSS_PACKET_CMD_NONE_E] (not supported)
                   and non-NULL enablePtr.
    Expected: NOT GT_OK.
    1.3. Call with out of range cmdTrigger [0x5AAAAAA5]
                   and non-NULL enablePtr.
    Expected: GT_BAD_PARAM.
    1.4. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmdTrigger = CPSS_PACKET_CMD_FORWARD_E;
    GT_BOOL              enable     = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cmdTrigger [CPSS_PACKET_CMD_FORWARD_E /
                                       CPSS_PACKET_CMD_DROP_HARD_E /
                                       CPSS_PACKET_CMD_DROP_SOFT_E /
                                       CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                           and non-NULL enablePtr.
            Expected: GT_OK.
        */

        /* Call with cmdTrigger[CPSS_PACKET_CMD_FORWARD_E] */
        cmdTrigger = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_DROP_HARD_E] */
        cmdTrigger = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_DROP_SOFT_E] */
        cmdTrigger = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        cmdTrigger = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        cmdTrigger = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /*
            1.2. Call with cmdTrigger [CPSS_PACKET_CMD_ROUTE_E /
                                       CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                                       CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                       CPSS_PACKET_CMD_BRIDGE_E /
                                       CPSS_PACKET_CMD_NONE_E] (not supported)
                           and non-NULL enablePtr.
            Expected: NOT GT_OK.
        */
        enable = GT_TRUE;

        /* Call with cmdTrigger[CPSS_PACKET_CMD_ROUTE_E] */
        cmdTrigger = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E] */
        cmdTrigger = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E] */
        cmdTrigger = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_BRIDGE_E] */
        cmdTrigger = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /* Call with cmdTrigger[CPSS_PACKET_CMD_NONE_E] */
        cmdTrigger = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmdTrigger);

        /*
            1.3. Call with out of range cmdTrigger [0x5AAAAAA5]
                           and non-NULL enablePtr.
            Expected: GT_BAD_PARAM.
        */
        cmdTrigger = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cmdTrigger);

        cmdTrigger = CPSS_PACKET_CMD_FORWARD_E;

        /*
            1.4. Call with enablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    cmdTrigger = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                              mainUtCpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          cmdTrigger, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerDropTypeSet
(
    IN GT_U8                    dev,
    IN CPSS_DROP_MODE_TYPE_ENT  dropType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerDropTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with dropType [CPSS_DROP_MODE_SOFT_E /
                                      CPSS_DROP_MODE_HARD_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerDropTypeGet with non-NULL dropTypePtr.
    Expected: GT_OK and the same dropType.
    1.3. Call function with out of range dropType [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DROP_MODE_TYPE_ENT  dropType    = CPSS_DROP_MODE_SOFT_E;
    CPSS_DROP_MODE_TYPE_ENT  dropTypeGet = CPSS_DROP_MODE_SOFT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with dropType [CPSS_DROP_MODE_SOFT_E /
                                              CPSS_DROP_MODE_HARD_E].
            Expected: GT_OK.
        */

        /* Call with dropType [CPSS_DROP_MODE_SOFT_E] */
        dropType = CPSS_DROP_MODE_SOFT_E;

        st = cpssExMxPmPolicerDropTypeSet(dev, mainUtCpssExMxPmPolicerDirection, dropType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropType);

        /*
            1.2. Call cpssExMxPmPolicerDropTypeGet with non-NULL dropTypePtr.
            Expected: GT_OK and the same dropType.
        */
        st = cpssExMxPmPolicerDropTypeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          &dropTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerDropTypeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropType, dropTypeGet,
                       "get another dropType than was set: %d", dev);

        /* Call with dropType [CPSS_DROP_MODE_HARD_E] */
        dropType = CPSS_DROP_MODE_HARD_E;

        st = cpssExMxPmPolicerDropTypeSet(dev, mainUtCpssExMxPmPolicerDirection, dropType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropType);

        /*
            1.2. Call cpssExMxPmPolicerDropTypeGet with non-NULL dropTypePtr.
            Expected: GT_OK and the same dropType.
        */
        st = cpssExMxPmPolicerDropTypeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          &dropTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerDropTypeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropType, dropTypeGet,
                       "get another dropType than was set: %d", dev);

        /*
            1.3. Call function with out of range dropType [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        dropType = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerDropTypeSet(dev, mainUtCpssExMxPmPolicerDirection, dropType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, dropType);
    }

    dropType = CPSS_DROP_MODE_SOFT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerDropTypeSet(dev, mainUtCpssExMxPmPolicerDirection, dropType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerDropTypeSet(dev, mainUtCpssExMxPmPolicerDirection, dropType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerDropTypeGet
(
    IN  GT_U8                      dev,
    OUT CPSS_DROP_MODE_TYPE_ENT    *dropTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerDropTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL dropTypePtr.
    Expected: GT_OK.
    1.2. Call with dropTypePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DROP_MODE_TYPE_ENT  dropType = CPSS_DROP_MODE_SOFT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL dropTypePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerDropTypeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          &dropType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null dropTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerPacketSizeModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropTypePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerDropTypeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          &dropType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerDropTypeGet(dev, mainUtCpssExMxPmPolicerDirection, &dropType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerCountingColorModeSet
(
    IN GT_U8                                        dev,
    IN CPSS_EXMXPM_POLICER_COLOR_COUNT_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerCountingColorModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with mode [CPSS_EXMXPM_POLICER_COLOR_COUNT_CL_E /
                                  CPSS_EXMXPM_POLICER_COLOR_COUNT_DP_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerCountingColorModeGet with non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_COLOR_COUNT_MODE_ENT mode    = CPSS_EXMXPM_POLICER_COLOR_COUNT_CL_E;
    CPSS_EXMXPM_POLICER_COLOR_COUNT_MODE_ENT modeGet = CPSS_EXMXPM_POLICER_COLOR_COUNT_CL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_EXMXPM_POLICER_COLOR_COUNT_CL_E /
                                          CPSS_EXMXPM_POLICER_COLOR_COUNT_DP_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_POLICER_COLOR_COUNT_CL_E] */
        mode = CPSS_EXMXPM_POLICER_COLOR_COUNT_CL_E;

        st = cpssExMxPmPolicerCountingColorModeSet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPolicerCountingColorModeGet with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmPolicerCountingColorModeGet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerCountingColorModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_POLICER_COLOR_COUNT_DP_E] */
        mode = CPSS_EXMXPM_POLICER_COLOR_COUNT_DP_E;

        st = cpssExMxPmPolicerCountingColorModeSet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPolicerCountingColorModeGet with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmPolicerCountingColorModeGet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerCountingColorModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerCountingColorModeSet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_POLICER_COLOR_COUNT_CL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerCountingColorModeSet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerCountingColorModeSet(dev, mainUtCpssExMxPmPolicerDirection, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerCountingColorModeGet
(
    IN  GT_U8                                       dev,
    OUT CPSS_EXMXPM_POLICER_COLOR_COUNT_MODE_ENT    *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerCountingColorModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_COLOR_COUNT_MODE_ENT  mode = CPSS_EXMXPM_POLICER_COLOR_COUNT_CL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerCountingColorModeGet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerCountingColorModeGet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerCountingColorModeGet(dev,
                                                   mainUtCpssExMxPmPolicerDirection,
                                                   &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerCountingColorModeGet(dev,
                                               mainUtCpssExMxPmPolicerDirection, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerL2RemarkModelSet
(
    IN GT_U8                                    dev,
    IN CPSS_EXMXPM_POLICER_L2_REMARK_MODEL_ENT  model
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerL2RemarkModelSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with model [CPSS_EXMXPM_POLICER_L2_REMARK_TC_E /
                                   CPSS_EXMXPM_POLICER_L2_REMARK_UP_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerL2RemarkModelGet with non-NULL modelPtr.
    Expected: GT_OK and the same model.
    1.3. Call function with out of range model [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_L2_REMARK_MODEL_ENT model    = CPSS_EXMXPM_POLICER_L2_REMARK_TC_E;
    CPSS_EXMXPM_POLICER_L2_REMARK_MODEL_ENT modelGet = CPSS_EXMXPM_POLICER_L2_REMARK_TC_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with model [CPSS_EXMXPM_POLICER_L2_REMARK_TC_E /
                                           CPSS_EXMXPM_POLICER_L2_REMARK_UP_E].
            Expected: GT_OK.
        */

        /* Call with model [CPSS_EXMXPM_POLICER_L2_REMARK_TC_E] */
        model = CPSS_EXMXPM_POLICER_L2_REMARK_TC_E;

        st = cpssExMxPmPolicerL2RemarkModelSet(dev, mainUtCpssExMxPmPolicerDirection,
                                               model);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, model);

        /*
            1.2. Call cpssExMxPmPolicerL2RemarkModelGet with non-NULL modelPtr.
            Expected: GT_OK and the same model.
        */
        st = cpssExMxPmPolicerL2RemarkModelGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               &modelGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerL2RemarkModelGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(model, modelGet,
                       "get another model than was set: %d", dev);

        /* Call with model [CPSS_EXMXPM_POLICER_L2_REMARK_UP_E] */
        model = CPSS_EXMXPM_POLICER_L2_REMARK_UP_E;

        st = cpssExMxPmPolicerL2RemarkModelSet(dev, mainUtCpssExMxPmPolicerDirection,
                                               model);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, model);

        /*
            1.2. Call cpssExMxPmPolicerL2RemarkModelGet with non-NULL modelPtr.
            Expected: GT_OK and the same model.
        */
        st = cpssExMxPmPolicerL2RemarkModelGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               &modelGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerL2RemarkModelGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(model, modelGet,
                       "get another model than was set: %d", dev);

        /*
            1.3. Call function with out of range model [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        model = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerL2RemarkModelSet(dev, mainUtCpssExMxPmPolicerDirection,
                                               model);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, model);
    }

    model = CPSS_EXMXPM_POLICER_L2_REMARK_TC_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerL2RemarkModelSet(dev, mainUtCpssExMxPmPolicerDirection,
                                               model);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerL2RemarkModelSet(dev, mainUtCpssExMxPmPolicerDirection, model);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerL2RemarkModelGet
(
    IN  GT_U8                                       dev,
    OUT CPSS_EXMXPM_POLICER_L2_REMARK_MODEL_ENT     *modelPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerL2RemarkModelGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL modelPtr.
    Expected: GT_OK.
    1.2. Call with modelPtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_L2_REMARK_MODEL_ENT  model = CPSS_EXMXPM_POLICER_L2_REMARK_TC_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modelPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerL2RemarkModelGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               &model);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modelPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerL2RemarkModelGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modelPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerL2RemarkModelGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               &model);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerL2RemarkModelGet(dev, mainUtCpssExMxPmPolicerDirection,
                                           &model);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerManagementCntrsResolutionSet
(
    IN GT_U8                                        dev,
    IN CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT         cntrSet,
    IN CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_ENT  cntrResolution
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerManagementCntrsResolutionSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E /
                            CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E /
                            CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E]
                   and cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_1B_E /
                                       CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerManagementCntrsResolutionGet with the same cntrSet and non-NULL cntrResolution.
    Expected: GT_OK and the same cntrResolution.
    1.3. Call with out of range cntrSet [0x5AAAAAA5]
                   and cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E].
    Expected: GT_BAD_PARAM.
    1.4. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E] (not supported)
                   and cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E].
    Expected: NOT GT_OK.
    1.5. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E]
                   and out of range cntrResolution [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT         cntrSet    = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
    CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_ENT  cntrRes    = CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_1B_E;
    CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_ENT  cntrResGet = CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_1B_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E /
                                    CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E /
                                    CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E]
                           and cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_1B_E /
                                               CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E].
            Expected: GT_OK.
        */

        /* Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E], cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_1B_E] */
        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
        cntrRes = CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_1B_E;

        st = cpssExMxPmPolicerManagementCntrsResolutionSet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, cntrRes);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntrSet, cntrRes);

        /*
            1.2. Call cpssExMxPmPolicerManagementCntrsResolutionGet with the same cntrSet and non-NULL cntrResolution.
            Expected: GT_OK and the same cntrResolution.
        */
        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrResGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerManagementCntrsResolutionGet: %d, %d", dev, cntrSet);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrRes, cntrResGet,
                       "get another cntrResolution than was set: %d", dev);

        /* Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E], cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E] */
        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E;
        cntrRes = CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E;

        st = cpssExMxPmPolicerManagementCntrsResolutionSet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, cntrRes);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntrSet, cntrRes);

        /*
            1.2. Call cpssExMxPmPolicerManagementCntrsResolutionGet with the same cntrSet and non-NULL cntrResolution.
            Expected: GT_OK and the same cntrResolution.
        */
        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrResGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerManagementCntrsResolutionGet: %d, %d", dev, cntrSet);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrRes, cntrResGet,
                       "get another cntrResolution than was set: %d", dev);

        /* Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E], cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E] */
        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E;

        st = cpssExMxPmPolicerManagementCntrsResolutionSet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, cntrRes);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntrSet, cntrRes);

        /*
            1.2. Call cpssExMxPmPolicerManagementCntrsResolutionGet with the same cntrSet and non-NULL cntrResolution.
            Expected: GT_OK and the same cntrResolution.
        */
        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrResGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerManagementCntrsResolutionGet: %d, %d", dev, cntrSet);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrRes, cntrResGet,
                       "get another cntrResolution than was set: %d", dev);

        /*
            1.3. Call with out of range cntrSet [0x5AAAAAA5]
                           and cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E].
            Expected: GT_BAD_PARAM.
        */
        cntrSet = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerManagementCntrsResolutionSet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, cntrRes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSet);

        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

        /*
            1.4. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E] (not supported)
                           and cntrResolution [CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_16B_E].
            Expected: NOT GT_OK.
        */
        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E;

        st = cpssExMxPmPolicerManagementCntrsResolutionSet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, cntrRes);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSet);

        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E;

        /*
            1.5. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E]
                           and out of range cntrResolution [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        cntrRes = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerManagementCntrsResolutionSet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, cntrRes);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cntrResolution = %d", dev, cntrRes);
    }

    cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
    cntrRes = CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_1B_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerManagementCntrsResolutionSet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, cntrRes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerManagementCntrsResolutionSet(dev,
                                                       mainUtCpssExMxPmPolicerDirection,
                                                       cntrSet, cntrRes);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerManagementCntrsResolutionGet
(
    IN  GT_U8                                           dev,
    IN  CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT            cntrSet,
    OUT CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_ENT     *cntrResolution
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerManagementCntrsResolutionGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E /
                            CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E /
                            CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E]
                   and non-NULL cntrResolution.
    Expected: GT_OK.
    1.2. Call with out of range cntrSet [0x5AAAAAA5]
                   and non-NULL cntrResolution.
    Expected: GT_BAD_PARAM.
    1.3. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E] (not supported)
                   and non-NULL cntrResolution.
    Expected: NOT GT_OK.
    1.4. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E]
                    and cntrResolution [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT            cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
    CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_ENT     cntrRes = CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_1B_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E /
                                    CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E /
                                    CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E]
                           and non-NULL cntrResolution.
            Expected: GT_OK.
        */

        /* Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E] */
        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrRes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSet);

        /* Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E] */
        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E;

        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrRes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSet);

        /* Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E] */
        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E;

        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrRes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSet);

        /*
            1.2. Call with out of range cntrSet [0x5AAAAAA5]
                           and non-NULL cntrResolution.
            Expected: GT_BAD_PARAM.
        */
        cntrSet = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrRes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSet);

        /*
            1.3. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E] (not supported)
                           and non-NULL cntrResolution.
            Expected: NOT GT_OK.
        */
        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E;

        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrRes);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSet);

        cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E;

        /*
            1.4. Call with cntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E]
                            and cntrResolution [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrResolution = NULL", dev);
    }

    cntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                           mainUtCpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrRes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerManagementCntrsResolutionGet(dev,
                                                       mainUtCpssExMxPmPolicerDirection,
                                                       cntrSet, &cntrRes);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPacketSizeModeForTunnelTermSet
(
    IN  GT_U8                                       dev,
    IN  CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_MODE_ENT packetSize
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPacketSizeModeForTunnelTermSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with packetSize [CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_REGULAR_ENT /
                                        CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_PASSENGER_ENT].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerPacketSizeModeForTunnelTermGet with non-NULL modelPtr.
    Expected: GT_OK and the same packetSize.
    1.3. Call function with out of range packetSize [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_MODE_ENT packetSize    = CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_REGULAR_ENT;
    CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_MODE_ENT packetSizeGet = CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_REGULAR_ENT;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with packetSize [CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_REGULAR_ENT /
                                                CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_PASSENGER_ENT].
            Expected: GT_OK.
        */

        /* Call with packetSize [CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_REGULAR_ENT] */
        packetSize = CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_REGULAR_ENT;

        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermSet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             packetSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetSize);

        /*
            1.2. Call cpssExMxPmPolicerPacketSizeModeForTunnelTermGet with non-NULL packetSizePtr.
            Expected: GT_OK and the same packetSize.
        */
        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             &packetSizeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPacketSizeModeForTunnelTermGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(packetSize, packetSizeGet,
                       "get another packetSize than was set: %d", dev);

        /* Call with packetSize [CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_PASSENGER_ENT] */
        packetSize = CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_PASSENGER_ENT;

        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermSet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             packetSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetSize);

        /*
            1.2. Call cpssExMxPmPolicerPacketSizeModeForTunnelTermGet with non-NULL packetSizePtr.
            Expected: GT_OK and the same packetSize.
        */
        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             &packetSizeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPacketSizeModeForTunnelTermGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(packetSize, packetSizeGet,
                       "get another packetSize than was set: %d", dev);

        /*
            1.3. Call function with out of range packetSize [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        packetSize = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermSet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             packetSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, packetSize);
    }

    packetSize = CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_REGULAR_ENT;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermSet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             packetSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPacketSizeModeForTunnelTermSet(dev,
                                                         mainUtCpssExMxPmPolicerDirection,
                                                         packetSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPacketSizeModeForTunnelTermGet
(
    IN  GT_U8                               dev,
    OUT CPSS_POLICER_PACKET_SIZE_MODE_ENT   *packetSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPacketSizeModeForTunnelTermGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL packetSizePtr.
    Expected: GT_OK.
    1.2. Call with packetSizePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_MODE_ENT packetSize =
                                                    CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null packetSizePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             &packetSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null packetSizePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetSizePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPacketSizeModeForTunnelTermGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             &packetSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPacketSizeModeForTunnelTermGet(dev,
                                                         mainUtCpssExMxPmPolicerDirection,
                                                         &packetSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPortMeteringEnableSet
(
    IN GT_U8    dev,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPortMeteringEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPolicerPortMeteringEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = POLICER_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPolicerPortMeteringEnableSet(dev,
                                                        mainUtCpssExMxPmPolicerDirection,
                                                        port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPolicerPortMeteringEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPolicerPortMeteringEnableGet(dev,
                                                        mainUtCpssExMxPmPolicerDirection,
                                                        port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPortMeteringEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPolicerPortMeteringEnableSet(dev,
                                                        mainUtCpssExMxPmPolicerDirection,
                                                        port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortMacCountersTxHistogramEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmPolicerPortMeteringEnableGet(dev,
                                                        mainUtCpssExMxPmPolicerDirection,
                                                        port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPortMeteringEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPolicerPortMeteringEnableSet(dev,
                                                        mainUtCpssExMxPmPolicerDirection,
                                                        port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPolicerPortMeteringEnableSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPolicerPortMeteringEnableSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = POLICER_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPortMeteringEnableSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPortMeteringEnableSet(dev,
                                                mainUtCpssExMxPmPolicerDirection,
                                                port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPortMeteringEnableGet
(
    IN  GT_U8       dev,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPortMeteringEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = POLICER_VALID_VIRT_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;

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
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPolicerPortMeteringEnableGet(dev,
                                                        mainUtCpssExMxPmPolicerDirection,
                                                        port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPolicerPortMeteringEnableGet(dev,
                                                        mainUtCpssExMxPmPolicerDirection,
                                                        port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPolicerPortMeteringEnableGet(dev,
                                                        mainUtCpssExMxPmPolicerDirection,
                                                        port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPolicerPortMeteringEnableGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPolicerPortMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                    port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = POLICER_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPortMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                    port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPortMeteringEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMruSet
(
    IN GT_U8    dev,
    IN GT_U32   mruSize
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMruSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mruSize [0 / 0x3FFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerMruGet with non-NULL mruSizePtr.
    Expected: GT_OK and the same mruSize.
    1.2. Call with mruSize [0xFFFFFFFF] (no any constraints).
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32   mruSize    = 0;
    GT_U32   mruSizeGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mruSize [0 / 0x3FFF].
            Expected: GT_OK.
        */

        /* Call with mruSize [0] */
        mruSize = 0;

        st = cpssExMxPmPolicerMruSet(dev, mainUtCpssExMxPmPolicerDirection, mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mruSize);

        /*
            1.2. Call cpssExMxPmPolicerMruGet with non-NULL mruSizePtr.
            Expected: GT_OK and the same mruSize.
        */
        st = cpssExMxPmPolicerMruGet(dev, mainUtCpssExMxPmPolicerDirection, &mruSizeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerMruGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mruSize, mruSizeGet,
                       "get another mruSize than was set: %d", dev);

        /* Call with mruSize [0x3FFF] */
        mruSize = 0x3FFF;

        st = cpssExMxPmPolicerMruSet(dev, mainUtCpssExMxPmPolicerDirection, mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mruSize);

        /*
            1.2. Call cpssExMxPmPolicerMruGet with non-NULL mruSizePtr.
            Expected: GT_OK and the same mruSize.
        */
        st = cpssExMxPmPolicerMruGet(dev, mainUtCpssExMxPmPolicerDirection, &mruSizeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerMruGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mruSize, mruSizeGet,
                       "get another mruSize than was set: %d", dev);

        /*
            1.2. Call with mruSize [0xFFFFFFFF] (no any constraints).
            Expected: GT_OK.
        */
        mruSize = 0xFFFFFFFF;

        st = cpssExMxPmPolicerMruSet(dev, mainUtCpssExMxPmPolicerDirection, mruSize);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mruSize);
    }

    mruSize = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMruSet(dev, mainUtCpssExMxPmPolicerDirection, mruSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMruSet(dev, mainUtCpssExMxPmPolicerDirection, mruSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMruGet
(
    IN  GT_U8   dev,
    OUT GT_U32  *mruSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMruGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL mruSizePtr.
    Expected: GT_OK.
    1.2. Call with mruSizePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  mruSize = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null mruSizePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerMruGet(dev, mainUtCpssExMxPmPolicerDirection, &mruSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null mruSizePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMruGet(dev, mainUtCpssExMxPmPolicerDirection, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mruSizePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMruGet(dev, mainUtCpssExMxPmPolicerDirection, &mruSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMruGet(dev, mainUtCpssExMxPmPolicerDirection, &mruSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerErrorGet
(
    IN  GT_U8                               dev,
    OUT CPSS_EXMXPM_POLICER_ENTRY_TYPE_ENT  *entryTypePtr,
    OUT GT_U32                              *entryAddrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerErrorGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL entryTypePtr and entryAddrPtr.
    Expected: GT_OK or GT_EMPTY.
    1.2. Call with entryTypePtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with entryAddrPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_ENTRY_TYPE_ENT  entryType = CPSS_EXMXPM_POLICER_ENTRY_METERING_E;
    GT_U32                              entryAddr = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL entryTypePtr and entryAddrPtr.
            Expected: GT_OK or GT_EMPTY.
        */
        st = cpssExMxPmPolicerErrorGet(dev, mainUtCpssExMxPmPolicerDirection,
                                       &entryType, &entryAddr);
        if (GT_OK != st)
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_EMPTY, st, dev);
        }

        /*
            1.2. Call with entryTypePtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerErrorGet(dev, mainUtCpssExMxPmPolicerDirection,
                                       NULL, &entryAddr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryTypePtr = NULL", dev);

        /*
            1.3. Call with entryAddrPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerErrorGet(dev, mainUtCpssExMxPmPolicerDirection,
                                       &entryType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryAddrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerErrorGet(dev, mainUtCpssExMxPmPolicerDirection,
                                       &entryType, &entryAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerErrorGet(dev, mainUtCpssExMxPmPolicerDirection,
                                   &entryType, &entryAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerErrorCounterGet
(
    IN  GT_U8   dev,
    OUT GT_U32  *cntrValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerErrorCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL cntrValuePtr.
    Expected: GT_OK.
    1.2. Call with cntrValuePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  cntrValue = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null cntrValuePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerErrorCounterGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              &cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null cntrValuePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerErrorCounterGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrValuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerErrorCounterGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              &cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerErrorCounterGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          &cntrValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerManagementCountersSet
(
    IN GT_U8                                    dev,
    IN CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT     mngCntrSet,
    IN CPSS_EXMXPM_POLICER_MNG_CNTR_TYPE_ENT    mngCntrType,
    IN CPSS_EXMXPM_POLICER_MNG_CNTR_ENTRY_STC   *mngCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerManagementCountersSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E /
                               CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E /
                               CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E],
                   mngCntrType [CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E /
                                CPSS_EXMXPM_POLICER_MNG_CNTR_YELLOW_E /
                                CPSS_EXMXPM_POLICER_MNG_CNTR_RED_E /
                                CPSS_EXMXPM_POLICER_MNG_CNTR_DROP_E]
                   and mngCntrPtr->{duMngCntr.l[0x0, 0x0 / 0x0, 0xFF / 0xFFFFFFFF, 0x3FF],
                                    packetMngCntr [0 / 32 / 0xFFFFFFFF]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerManagementCountersGet with the same mngCntrSet , mngCntrType and non-NULL mngCntrPtr.
    Expected: GT_OK and the same mngCntrPtr.
    1.3 Call with out of range mngCntrSet [0x5AAAAAA5]
                  and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range mngCntrType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with mngCntrPtr->duMngCntr.l[0xFFFFFFFF, 0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.7. Call with mngCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT     mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
    CPSS_EXMXPM_POLICER_MNG_CNTR_TYPE_ENT    mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;
    GT_BOOL                                  isEqual     = GT_FALSE;
    CPSS_EXMXPM_POLICER_MNG_CNTR_ENTRY_STC   mngCntr;
    CPSS_EXMXPM_POLICER_MNG_CNTR_ENTRY_STC   mngCntrGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E /
                                       CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E /
                                       CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E],
                           mngCntrType [CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E /
                                        CPSS_EXMXPM_POLICER_MNG_CNTR_YELLOW_E /
                                        CPSS_EXMXPM_POLICER_MNG_CNTR_RED_E /
                                        CPSS_EXMXPM_POLICER_MNG_CNTR_DROP_E]
                           and mngCntrPtr->{duMngCntr.l[0x0, 0x0 / 0x0, 0xFF / 0xFFFFFFFF, 0x3FF],
                                            packetMngCntr [0 / 32 / 0xFFFFFFFF]}.
            Expected: GT_OK.
        */

        /* Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E] */
        mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;

        mngCntr.duMngCntr.l[0] = 0x0;
        mngCntr.duMngCntr.l[1] = 0x0;
        mngCntr.packetMngCntr  = 0;

        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mngCntrSet, mngCntrType);

        /*
            1.2. Call cpssExMxPmPolicerManagementCountersGet with the same mngCntrSet , mngCntrType and non-NULL mngCntrPtr.
            Expected: GT_OK and the same mngCntrPtr.
        */
        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerManagementCountersGet: %d, %d, %d", dev, mngCntrSet, mngCntrType);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mngCntr.duMngCntr.l,
                                     (GT_VOID*) &mngCntrGet.duMngCntr.l,
                                     sizeof(mngCntr.duMngCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another mngCntrPtr->duMngCntr.l than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mngCntr.packetMngCntr, mngCntrGet.packetMngCntr,
                       "get another mngCntrPtr->packetMngCntr than was set: %d", dev);

        /* Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E] */
        mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E;
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_YELLOW_E;

        mngCntr.duMngCntr.l[0] = 0x0;
        mngCntr.duMngCntr.l[1] = 0xFF;
        mngCntr.packetMngCntr  = 32;

        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mngCntrSet, mngCntrType);

        /*
            1.2. Call cpssExMxPmPolicerManagementCountersGet with the same mngCntrSet , mngCntrType and non-NULL mngCntrPtr.
            Expected: GT_OK and the same mngCntrPtr.
        */
        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerManagementCountersGet: %d, %d, %d", dev, mngCntrSet, mngCntrType);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mngCntr.duMngCntr.l,
                                     (GT_VOID*) &mngCntrGet.duMngCntr.l,
                                     sizeof(mngCntr.duMngCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another mngCntrPtr->duMngCntr.l than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mngCntr.packetMngCntr, mngCntrGet.packetMngCntr,
                       "get another mngCntrPtr->packetMngCntr than was set: %d", dev);

        /* Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E] */
        mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E;
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_RED_E;

        mngCntr.duMngCntr.l[0] = 0xFFFFFFFF;
        mngCntr.duMngCntr.l[1] = 0x3FF;
        mngCntr.packetMngCntr  = 0xFFFFFFFF;

        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mngCntrSet, mngCntrType);

        /*
            1.2. Call cpssExMxPmPolicerManagementCountersGet with the same mngCntrSet , mngCntrType and non-NULL mngCntrPtr.
            Expected: GT_OK and the same mngCntrPtr.
        */
        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerManagementCountersGet: %d, %d, %d", dev, mngCntrSet, mngCntrType);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mngCntr.duMngCntr.l,
                                     (GT_VOID*) &mngCntrGet.duMngCntr.l,
                                     sizeof(mngCntr.duMngCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another mngCntrPtr->duMngCntr.l than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(mngCntr.packetMngCntr, mngCntrGet.packetMngCntr,
                       "get another mngCntrPtr->packetMngCntr than was set: %d", dev);

        /* Call with mngCntrType [CPSS_EXMXPM_POLICER_MNG_CNTR_DROP_E] */
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_DROP_E;

        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mngCntrSet, mngCntrType);

        /*
            1.3 Call with out of range mngCntrSet [0x5AAAAAA5]
                          and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mngCntrSet  = POLICER_INVALID_ENUM_CNS;
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;

        mngCntr.duMngCntr.l[0] = 0x0;
        mngCntr.duMngCntr.l[1] = 0x0;
        mngCntr.packetMngCntr  = 0;

        st = cpssExMxPmPolicerManagementCountersSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mngCntrSet);

        /*
            1.4. Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E;

        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mngCntrSet);

        mngCntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

        /*
            1.5. Call with out of range mngCntrType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mngCntrType = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, mngCntrType = %d", dev, mngCntrType);

        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;

        /*
            1.6. Call with mngCntrPtr->duMngCntr.l[0xFFFFFFFF, 0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mngCntr.duMngCntr.l[0] = 0xFFFFFFFF;
        mngCntr.duMngCntr.l[1] = 0xFFFFFFFF;

        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, mngCntrPtr->duMngCntr.l[0] = %d, mngCntrPtr->duMngCntr.l[2] = %d",
                                     dev, mngCntr.duMngCntr.l[0], mngCntr.duMngCntr.l[1]);

        mngCntr.duMngCntr.l[0] = 0;
        mngCntr.duMngCntr.l[1] = 0;

        /*
            1.7. Call with mngCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mngCntrPtr = NULL", dev);
    }

    mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
    mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;

    mngCntr.duMngCntr.l[0] = 0x0;
    mngCntr.duMngCntr.l[1] = 0x0;
    mngCntr.packetMngCntr  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerManagementCountersSet(dev,
                                                mainUtCpssExMxPmPolicerDirection,
                                                mngCntrSet, mngCntrType, &mngCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerManagementCountersGet
(
    IN  GT_U8                                   dev,
    IN  CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT    mngCntrSet,
    IN  CPSS_EXMXPM_POLICER_MNG_CNTR_TYPE_ENT   mngCntrType,
    OUT CPSS_EXMXPM_POLICER_MNG_CNTR_ENTRY_STC  *mngCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerManagementCountersGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E /
                               CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E /
                               CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E],
                   mngCntrType [CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E /
                                CPSS_EXMXPM_POLICER_MNG_CNTR_YELLOW_E /
                                CPSS_EXMXPM_POLICER_MNG_CNTR_RED_E /
                                CPSS_EXMXPM_POLICER_MNG_CNTR_DROP_E]
                   and non-NULL mngCntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range mngCntrSet [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range mngCntrType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with mngCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT     mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
    CPSS_EXMXPM_POLICER_MNG_CNTR_TYPE_ENT    mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;
    CPSS_EXMXPM_POLICER_MNG_CNTR_ENTRY_STC   mngCntr;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E /
                                       CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E /
                                       CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E],
                           mngCntrType [CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E /
                                        CPSS_EXMXPM_POLICER_MNG_CNTR_YELLOW_E /
                                        CPSS_EXMXPM_POLICER_MNG_CNTR_RED_E /
                                        CPSS_EXMXPM_POLICER_MNG_CNTR_DROP_E]
                           and non-NULL mngCntrPtr.
            Expected: GT_OK.
        */

        /* Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E] */
        mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;

        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mngCntrSet, mngCntrType);

        /* Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E] */
        mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET1_E;
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_YELLOW_E;

        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mngCntrSet, mngCntrType);

        /* Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E] */
        mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET2_E;
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_RED_E;

        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mngCntrSet, mngCntrType);

        /* Call with mngCntrType [CPSS_EXMXPM_POLICER_MNG_CNTR_DROP_E] */
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_DROP_E;

        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mngCntrSet, mngCntrType);

        /*
            1.2. Call with out of range mngCntrSet [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mngCntrSet  = POLICER_INVALID_ENUM_CNS;
        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;

        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mngCntrSet);

        /*
            1.3. Call with mngCntrSet [CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_DISABLED_E;

        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mngCntrSet);

        mngCntrSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

        /*
            1.4. Call with out of range mngCntrType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        mngCntrType = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, mngCntrType = %d", dev, mngCntrType);

        mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;

        /*
            1.5. Call with mngCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mngCntrPtr = NULL", dev);
    }

    mngCntrSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;
    mngCntrType = CPSS_EXMXPM_POLICER_MNG_CNTR_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                    mainUtCpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerManagementCountersGet(dev,
                                                mainUtCpssExMxPmPolicerDirection,
                                                mngCntrSet, mngCntrType, &mngCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringEntrySet
(
    IN  GT_U8                                   dev,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  *entryPtr,
    OUT CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT *tbParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0],
                   entryPtr->{countingEntryIndex[0],
                              dropRed[GT_TRUE],
                              modifyQosParams{modifyTc [GT_TRUE],
                                              modifyUp [GT_TRUE],
                                              modifyDp [GT_TRUE],
                                              modifyDscp [GT_TRUE],
                                              modifyExp [GT_TRUE]},
                              remarkMode [CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E],
                              meterColorMode [CPSS_POLICER_COLOR_BLIND_E],
                              meterMode [CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E],
                              mngCounterSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E],
                              tbParamsPtr.srTcmParams{cir [100], cbs [256], ebs [1000]}}
                   and non-NULL tbParamsPtr.
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerMeteringEntryGet with same entryIndex and non-NULL entryPtr.
    Expected: GT_OK and the same values as written - check by fields.
    1.3. Call with out of range entryIndex [maxNumOfPolicerEntries]
                   and other parameters same as in 1.1.
    Expected: non GT_OK.
    1.4. Call with entryPtr->countingEntryIndex [0xFFFF] (no any constraints)
                   and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.5. Call with entryPtr->remarkMode [0x5AAAAAA5],
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with entryPtr->meterColorMode [0x5AAAAAA5],
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with entryPtr->meterMode [0x5AAAAAA5],
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with entryPtr->mngCounterSet [0x5AAAAAA5],
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with entryPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.10. Call with tbParamsPtr [NULL]
                    and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  entryIndex = 0;
    GT_U32                                  maxIndex   = 0;
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  entry;
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  entryGet;
    CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT tbParams;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting entry size */
        st = prvUtfExMxPmMaxNumOfPolicerEntriesGet(dev, &maxIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfExMxPmMaxNumOfPolicerEntriesGet: %d", dev);

        /*
            1.1. Call with entryIndex [0],
                           entryPtr->{countingEntryIndex[0],
                                      dropRed[GT_TRUE],
                                      modifyQosParams{modifyTc [GT_TRUE],
                                                      modifyUp [GT_TRUE],
                                                      modifyDp [GT_TRUE],
                                                      modifyDscp [GT_TRUE],
                                                      modifyExp [GT_TRUE]},
                                      remarkMode [CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E],
                                      meterColorMode [CPSS_POLICER_COLOR_BLIND_E],
                                      meterMode [CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E],
                                      mngCounterSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E],
                                      tokenBucketsParams.srTcmParams{cir [100], cbs [256], ebs [1000]}}
                           and non-NULL tbParamsPtr.
            Expected: GT_OK.
        */
        entryIndex = 0;

        entry.countingEntryIndex = 0;
        entry.dropRed            = GT_TRUE;

        entry.modifyQosParams.modifyTc   = GT_TRUE;
        entry.modifyQosParams.modifyUp   = GT_TRUE;
        entry.modifyQosParams.modifyDp   = GT_TRUE;
        entry.modifyQosParams.modifyDscp = GT_TRUE;
        entry.modifyQosParams.modifyExp  = GT_TRUE;

        entry.remarkMode     = CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E;
        entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
        entry.meterMode      = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;
        entry.mngCounterSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

        entry.tokenBucketsParams.srTcmParams.cir = 100;
        entry.tokenBucketsParams.srTcmParams.cbs = 256;
        entry.tokenBucketsParams.srTcmParams.ebs = 1000;

        st = cpssExMxPmPolicerMeteringEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, &tbParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call cpssExMxPmPolicerMeteringEntryGet with same entryIndex and non-NULL entryPtr.
            Expected: GT_OK and the same values as written - check by fields.
        */
        st = cpssExMxPmPolicerMeteringEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerMeteringEntryGet: %d, %d", dev, entryIndex);

        /* Verifying entryPtr */
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.countingEntryIndex, entryGet.countingEntryIndex,
                       "get another entryPtr->countingEntryIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.dropRed, entryGet.dropRed,
                       "get another entryPtr->dropRed than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyTc, entryGet.modifyQosParams.modifyTc,
                       "get another entryPtr->modifyQosParams.modifyTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyUp, entryGet.modifyQosParams.modifyUp,
                       "get another entryPtr->modifyQosParams.modifyUp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyDp, entryGet.modifyQosParams.modifyDp,
                       "get another entryPtr->modifyQosParams.modifyDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyDscp, entryGet.modifyQosParams.modifyDscp,
                       "get another entryPtr->modifyQosParams.modifyDscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyExp, entryGet.modifyQosParams.modifyExp,
                       "get another entryPtr->modifyQosParams.modifyExp than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(entry.remarkMode, entryGet.remarkMode,
                       "get another entryPtr->remarkMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.meterColorMode, entryGet.meterColorMode,
                       "get another entryPtr->meterColorMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.meterMode, entryGet.meterMode,
                       "get another entryPtr->meterMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.mngCounterSet, entryGet.mngCounterSet,
                       "get another entryPtr->mngCounterSet than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(entry.tokenBucketsParams.srTcmParams.cir, entryGet.tokenBucketsParams.srTcmParams.cir,
                       "get another entryPtr->tokenBucketsParams.srTcmParams.cir than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.tokenBucketsParams.srTcmParams.cbs, entryGet.tokenBucketsParams.srTcmParams.cbs,
                       "get another entryPtr->tokenBucketsParams.srTcmParams.cbs than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entry.tokenBucketsParams.srTcmParams.ebs, entryGet.tokenBucketsParams.srTcmParams.ebs,
                       "get another entryPtr->tokenBucketsParams.srTcmParams.ebs than was set: %d", dev);

        /*
            1.3. Call with out of range entryIndex [maxNumOfPolicerEntries]
                           and other parameters same as in 1.1.
            Expected: non GT_OK.
        */
        entryIndex = 0x2000;

        st = cpssExMxPmPolicerMeteringEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, &tbParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.4. Call with entryPtr->countingEntryIndex [0xFFFF] (no any constraints)
                           and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.countingEntryIndex = 0xFFFF;

        st = cpssExMxPmPolicerMeteringEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, &tbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryPtr->countingEntryIndex = %d", dev, entry.countingEntryIndex);

        entry.countingEntryIndex = 0;

        /*
            1.5. Call with entryPtr->remarkMode [0x5AAAAAA5],
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.remarkMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerMeteringEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, &tbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->remarkMode = %d", dev, entry.remarkMode);

        entry.remarkMode = CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E;

        /*
            1.6. Call with entryPtr->meterColorMode [0x5AAAAAA5],
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.meterColorMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerMeteringEntrySet(dev,
                                               mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, &tbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->meterColorMode = %d", dev, entry.meterColorMode);

        entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;

        /*
            1.7. Call with entryPtr->meterMode [0x5AAAAAA5],
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.meterMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerMeteringEntrySet(dev,
                                               mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, &tbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->meterMode = %d", dev, entry.meterMode);

        entry.meterMode = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;

        /*
            1.8. Call with entryPtr->mngCounterSet [0x5AAAAAA5],
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.mngCounterSet = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerMeteringEntrySet(dev,
                                               mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, &tbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->mngCounterSet = %d", dev, entry.mngCounterSet);

        entry.mngCounterSet = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

        /*
            1.9. Call with entryPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeteringEntrySet(dev,
                                               mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, NULL, &tbParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);

        /*
            1.10. Call with tbParamsPtr [NULL]
                            and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeteringEntrySet(dev,
                                               mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tbParamsPtr = NULL", dev);
    }

    entryIndex = 0;

    entry.countingEntryIndex = 0;
    entry.dropRed            = GT_TRUE;

    entry.modifyQosParams.modifyTc   = GT_TRUE;
    entry.modifyQosParams.modifyUp   = GT_TRUE;
    entry.modifyQosParams.modifyDp   = GT_TRUE;
    entry.modifyQosParams.modifyDscp = GT_TRUE;
    entry.modifyQosParams.modifyExp  = GT_TRUE;

    entry.remarkMode     = CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E;
    entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
    entry.meterMode      = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;
    entry.mngCounterSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

    entry.tokenBucketsParams.srTcmParams.cir = 100;
    entry.tokenBucketsParams.srTcmParams.cbs = 256;
    entry.tokenBucketsParams.srTcmParams.ebs = 1000;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry, &tbParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                           entryIndex, &entry, &tbParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringEntryGet
(
    IN  GT_U8                                   dev,
    IN  GT_U32                                  entryIndex,
    OUT CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0]
                   and non-NULL entryPtr.
    Expected: GT_OK.
    1.2. Call with out of range entryIndex [maxNumOfPolicerEntries]
                   and other parameters same as in 1.1.
    Expected: non GT_OK.
    1.3. Call with entryPtr [NULL]
                   and other parameters same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  entryIndex = 0;
    GT_U32                                  maxIndex   = 0x2000;
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  entry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting entry size */
        st = prvUtfExMxPmMaxNumOfPolicerEntriesGet(dev, &maxIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfExMxPmMaxNumOfPolicerEntriesGet: %d", dev);

        /*
            1.1. Call with entryIndex [0]
                           and non-NULL entryPtr.
            Expected: GT_OK.
        */
        entryIndex = 0;

        st = cpssExMxPmPolicerMeteringEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call with out of range entryIndex [maxNumOfPolicerEntries]
                           and other parameters same as in 1.1.
            Expected: non GT_OK.
        */
        entryIndex = 0x2000;

        st = cpssExMxPmPolicerMeteringEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.3. Call with entryPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeteringEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);
    }

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               entryIndex, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                           entryIndex, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerEntryMeterParamsCalculate
(
    IN  GT_U8                                       dev,
    IN  CPSS_EXMXPM_POLICER_METER_MODE_ENT          meterMode,
    IN  CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT     *tbInParamsPtr,
    OUT CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT     *tbOutParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerEntryMeterParamsCalculate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with meterMode [CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E]
                   tbInParamsPtr->srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],
                   non NULL tbOutParamsPtr.
    Expected: GT_OK.
    1.2. Call with meterMode [CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E]
                   tbInParamsPtr->trTcmParams{cir=10000, cbs=65535, pir=256000, pbs=256000}] ],
                   non NULL tbOutParamsPtr.
    Expected: GT_OK.
    1.3. Call with out of range meterMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM;
    1.4. Call with tbInParamsPtr [NULL],
                   non NULL tbOutParamsPtr.
    Expected: GT_BAD_PTR.
    1.5. Call with tbOutParamsPtr [NULL],
                   other parameters same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_METER_MODE_ENT          meterMode = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;
    CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT     tbInParams;
    CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT     tbOutParams;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with meterMode [CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E]
                           tbInParamsPtr->srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],
                           non NULL tbOutParamsPtr.
            Expected: GT_OK.
        */
        meterMode = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;

        tbInParams.srTcmParams.cir = 1000;
        tbInParams.srTcmParams.cbs = 65536;
        tbInParams.srTcmParams.ebs = 256000;

        st = cpssExMxPmPolicerEntryMeterParamsCalculate(dev,meterMode,
                                                        &tbInParams, &tbOutParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, meterMode);

        /*
            1.2. Call with meterMode [CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E]
                           tbInParamsPtr->trTcmParams{cir=10000, cbs=65535, pir=256000, pbs=256000}] ],
                           non NULL tbOutParamsPtr.
            Expected: GT_OK.
        */
        meterMode = CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E;

        tbInParams.trTcmParams.cir = 1000;
        tbInParams.trTcmParams.cbs = 65536;
        tbInParams.trTcmParams.pir = 256000;
        tbInParams.trTcmParams.pbs = 256000;

        st = cpssExMxPmPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                        &tbInParams, &tbOutParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, meterMode);

        /*
            1.3. Call with out of range meterMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM
        */
        meterMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                        &tbInParams, &tbOutParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, meterMode);

        meterMode = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;

        /*
            1.4. Call with tbInParamsPtr [NULL],
                           non NULL tbOutParamsPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerEntryMeterParamsCalculate(dev, meterMode, NULL,
                                                        &tbOutParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tbInParamsPtr = NULL", dev);

        /*
            1.5. Call with tbOutParamsPtr [NULL],
                           other parameters same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                        &tbInParams, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tbOutParamsPtr = NULL", dev);
    }

    meterMode = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;

    tbInParams.srTcmParams.cir = 1000;
    tbInParams.srTcmParams.cbs = 65536;
    tbInParams.srTcmParams.ebs = 256000;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                        &tbInParams, &tbOutParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                    &tbInParams, &tbOutParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerBillingEntrySet
(
    IN  GT_U8                                   dev,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC   *billingCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerBillingEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_BILLING_E],
                                                      numOfCountingEntries [100] to enable Billing Counting.
    Expected: GT_OK.
    1.2. Call with entryIndex [0 / 1],
                   billingCntrPtr {greenCntr.l[0x0, 0x0 / 0xFFFFFFFF, 0x3FF],
                                   yellowCntr.l[0x0, 0x0 / 0xFFFFFFFF, 0x3FF],
                                   redCntr.l[0x0, 0x0 / 0xFFFFFFFF, 0x3FF],
                                   cntrMode [CPSS_EXMXPM_POLICER_BILLING_CNTR_16_BYTES_E /
                                             CPSS_EXMXPM_POLICER_BILLING_CNTR_1_BYTE_E]}.
    Expected: GT_OK.
    1.3. Call cpssExMxPmPolicerBillingEntryGet with same entryIndex, reset [GT_FALSE] and non-NULL billingCntrPtr.
    Expected: GT_OK and same values as written in 1.1.
    1.4. Call with out of range entryIndex [maxNumOfPolicerEntries]
                   and other parameters same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call with billingCntrPtr->greenCntr.l[0xFFFFFFFF, 0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.6. Call with billingCntrPtr->yellowCntr.l[0xFFFFFFFF, 0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.7. Call with billingCntrPtr->redCntr.l[0xFFFFFFFF, 0xFFFFFFFF]
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.8. Call with out of range billingCntrPtr->cntrMode[0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with billingCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  entryIndex = 0;
    GT_U32                                  maxIndex   = 0;
    GT_BOOL                                 isEqual    = GT_FALSE;
    GT_BOOL                                 reset      = GT_FALSE;
    CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC   billingCntr;
    CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC   billingCntrGet;
    CPSS_EXMXPM_POLICER_CONFIG_STC          policerConf;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_BILLING_E],
                                                              numOfCountingEntries [100] to enable Billing Counting.
            Expected: GT_OK.
        */
        policerConf.ingrCountingEntries = 100;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_BILLING_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerConfigSet: %d, %d, %d",
                                     dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries
);

        /* Getting max entry index */
        st = prvUtfExMxPmMaxNumOfPolicerEntriesGet(dev, &maxIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfExMxPmMaxNumOfPolicerEntriesGet: %d", dev);

        /*
            1.2. Call with entryIndex [0 / 1],
                           billingCntrPtr {greenCntr.l[0x0, 0x0 / 0xFFFFFFFF, 0x3FF],
                                           yellowCntr.l[0x0, 0x0 / 0xFFFFFFFF, 0x3FF],
                                           redCntr.l[0x0, 0x0 / 0xFFFFFFFF, 0x3FF],
                                           cntrMode [CPSS_EXMXPM_POLICER_BILLING_CNTR_16_BYTES_E /
                                                     CPSS_EXMXPM_POLICER_BILLING_CNTR_1_BYTE_E]}.
            Expected: GT_OK.
        */

        /* Call with entryIndex [0] */
        entryIndex = 0;

        billingCntr.greenCntr.l[0] = 0x0;
        billingCntr.greenCntr.l[1] = 0x0;

        billingCntr.yellowCntr.l[0] = 0x0;
        billingCntr.yellowCntr.l[1] = 0x0;

        billingCntr.redCntr.l[0] = 0x0;
        billingCntr.redCntr.l[1] = 0x0;

        billingCntr.cntrMode = CPSS_EXMXPM_POLICER_BILLING_CNTR_16_BYTES_E;

        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.3. Call cpssExMxPmPolicerBillingEntryGet with same entryIndex, reset [GT_FALSE] and non-NULL billingCntrPtr.
            Expected: GT_OK and same values as written in 1.1.
        */
        reset = GT_FALSE;

        st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, reset, &billingCntrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerBillingEntryGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.greenCntr.l,
                                     (GT_VOID*) &billingCntrGet.greenCntr.l,
                                     sizeof(billingCntr.greenCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another billingCntrPtr->greenCntr.l than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.yellowCntr.l,
                                     (GT_VOID*) &billingCntrGet.yellowCntr.l,
                                     sizeof(billingCntr.yellowCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another billingCntrPtr->yellowCntr.l than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.redCntr.l,
                                     (GT_VOID*) &billingCntrGet.redCntr.l,
                                     sizeof(billingCntr.redCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another billingCntrPtr->redCntr.l than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(billingCntr.cntrMode, billingCntrGet.cntrMode,
                       "get another billingCntrPtr->cntrMode than was set: %d", dev);

        /* Call with entryIndex [1] */
        entryIndex = 1;

        billingCntr.greenCntr.l[0] = 0xFFFFFFFF;
        billingCntr.greenCntr.l[1] = 0x3FF;

        billingCntr.yellowCntr.l[0] = 0xFFFFFFFF;
        billingCntr.yellowCntr.l[1] = 0x3FF;

        billingCntr.redCntr.l[0] = 0xFFFFFFFF;
        billingCntr.redCntr.l[1] = 0x3FF;

        billingCntr.cntrMode = CPSS_EXMXPM_POLICER_BILLING_CNTR_1_BYTE_E;

        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.3. Call cpssExMxPmPolicerBillingEntryGet with same entryIndex, reset [GT_FALSE] and non-NULL billingCntrPtr.
            Expected: GT_OK and same values as written in 1.1.
        */
        reset = GT_FALSE;

        st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, reset, &billingCntrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerBillingEntryGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.greenCntr.l,
                                     (GT_VOID*) &billingCntrGet.greenCntr.l,
                                     sizeof(billingCntr.greenCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another billingCntrPtr->greenCntr.l than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.yellowCntr.l,
                                     (GT_VOID*) &billingCntrGet.yellowCntr.l,
                                     sizeof(billingCntr.yellowCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another billingCntrPtr->yellowCntr.l than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.redCntr.l,
                                     (GT_VOID*) &billingCntrGet.redCntr.l,
                                     sizeof(billingCntr.redCntr.l))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another billingCntrPtr->redCntr.l than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(billingCntr.cntrMode, billingCntrGet.cntrMode,
                       "get another billingCntrPtr->cntrMode than was set: %d", dev);

        /*
            1.4. Call with out of range entryIndex [maxNumOfPolicerEntries]
                           and other parameters same as in 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = maxIndex + 1;

        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.5. Call with billingCntrPtr->greenCntr.l[0xFFFFFFFF, 0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        billingCntr.greenCntr.l[0] = 0xFFFFFFFF;
        billingCntr.greenCntr.l[1] = 0xFFFFFFFF;

        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, billingCntrPtr->greenCntr.l[0] = %d,billingCntrPtr->greenCntr.l[0] = %d",
                                     dev, billingCntr.greenCntr.l[0], billingCntr.greenCntr.l[1]);

        billingCntr.greenCntr.l[0] = 0x0;
        billingCntr.greenCntr.l[1] = 0x0;

        /*
            1.6. Call with billingCntrPtr->yellowCntr.l[0xFFFFFFFF, 0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        billingCntr.yellowCntr.l[0] = 0xFFFFFFFF;
        billingCntr.yellowCntr.l[1] = 0xFFFFFFFF;

        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, billingCntrPtr->yellowCntr.l[0] = %d,billingCntrPtr->yellowCntr.l[0] = %d",
                                     dev, billingCntr.yellowCntr.l[0], billingCntr.yellowCntr.l[1]);

        billingCntr.yellowCntr.l[0] = 0x0;
        billingCntr.yellowCntr.l[1] = 0x0;

        /*
            1.7. Call with billingCntrPtr->redCntr.l[0xFFFFFFFF, 0xFFFFFFFF]
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        billingCntr.redCntr.l[0] = 0xFFFFFFFF;
        billingCntr.redCntr.l[1] = 0xFFFFFFFF;

        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, billingCntrPtr->redCntr.l[0] = %d,billingCntrPtr->redCntr.l[0] = %d",
                                     dev, billingCntr.redCntr.l[0], billingCntr.redCntr.l[1]);

        billingCntr.redCntr.l[0] = 0x0;
        billingCntr.redCntr.l[1] = 0x0;

        /*
            1.8. Call with out of range billingCntrPtr->cntrMode[0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        billingCntr.cntrMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, billingCntrPtr->cntrMode = %d",
                                     dev, billingCntr.cntrMode);

        billingCntr.cntrMode = CPSS_EXMXPM_POLICER_BILLING_CNTR_16_BYTES_E;

        /*
            1.9. Call with billingCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, billingCntrPtr = NULL", dev);
    }

    entryIndex = 0;

    billingCntr.greenCntr.l[0] = 0x0;
    billingCntr.greenCntr.l[1] = 0x0;

    billingCntr.yellowCntr.l[0] = 0x0;
    billingCntr.yellowCntr.l[1] = 0x0;

    billingCntr.redCntr.l[0] = 0x0;
    billingCntr.redCntr.l[1] = 0x0;

    billingCntr.cntrMode = CPSS_EXMXPM_POLICER_BILLING_CNTR_16_BYTES_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                          entryIndex, &billingCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerBillingEntryGet
(
    IN  GT_U8                                   dev,
    IN  GT_U32                                  entryIndex,
    IN  GT_BOOL                                 reset,
    OUT CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC   *billingCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerBillingEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 1],
                   reset [GT_TRUE and GT_FALSE]
                   and non-NULL billingCntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range entryIndex [maxNumOfPolicerEntries]
                   and other parameters same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call with billingCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  entryIndex = 0;
    GT_U32                                  maxIndex   = 0;
    GT_BOOL                                 reset      = GT_FALSE;
    CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC   billingCntr;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting entry size */
        st = prvUtfExMxPmMaxNumOfPolicerEntriesGet(dev, &maxIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfExMxPmMaxNumOfPolicerEntriesGet: %d", dev);

        /*
            1.1. Call with entryIndex [0 / 1],
                           reset [GT_TRUE and GT_FALSE]
                           and non-NULL billingCntrPtr.
            Expected: GT_OK.
        */

        /* Call with entryIndex [0], reset [GT_TRUE] */
        entryIndex = 0;
        reset      = GT_TRUE;

        st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, reset, &billingCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryIndex, reset);

        /* Call with entryIndex [1], reset [GT_FALSE] */
        entryIndex = 1;
        reset      = GT_FALSE;

        st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, reset, &billingCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryIndex, reset);

        /*
            1.2. Call with out of range entryIndex [maxNumOfPolicerEntries]
                           and other parameters same as in 1.1.
            Expected: non GT_OK.
        */
        entryIndex = maxIndex + 1;

        st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, reset, &billingCntr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.3. Call with billingCntrPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, reset, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, billingCntrPtr = NULL", dev);
    }

    entryIndex = 0;
    reset      = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              entryIndex, reset, &billingCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          entryIndex, reset, &billingCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerVlanCntrSet
(
    IN  GT_U8   dev,
    IN  GT_U16  vid,
    IN  GT_U32  cntrValue
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerVlanCntrSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_VLAN_E],
                                                      numOfCountingEntries [100] to enable Billing Counting.
    Expected: GT_OK.
    1.2. Call with vid [100 / maxIndex * 4 - 1]
                   and cntrValue [0 / 100].
    Expected: GT_OK.
    1.3. Call cpssExMxPmPolicerVlanCntrGet with same vid and non-NULL cntrValuePtr.
    Expected: GT_OK and same cntrValue.
    1.4. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with cntrValue [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16                                  vid          = 0;
    GT_U32                                  cntrValue    = 0;
    GT_U32                                  cntrValueGet = 0;
    GT_U32                                  maxIndex     = 0;
    CPSS_EXMXPM_POLICER_CONFIG_STC          policerConf;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_VLAN_E],
                                                              numOfCountingEntries [100] to enable Billing Counting.
            Expected: GT_OK.
        */
        policerConf.ingrCountingEntries = 100;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_VLAN_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerConfigSet: %d, %d, %d",
                                     dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries
);

        /* Getting entry size */
        st = prvUtfExMxPmMaxNumOfPolicerEntriesGet(dev, &maxIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfExMxPmMaxNumOfPolicerEntriesGet: %d", dev);

        /*
            1.1. Call with vid [100 / 4095]
                           and cntrValue [0 / 100].
            Expected: GT_OK.
        */

        /* Call with vid [100] */
        vid       = POLICER_TESTED_VLAN_ID_CNS;
        cntrValue = 0;

        st = cpssExMxPmPolicerVlanCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, cntrValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, cntrValue);

        /*
            1.2. Call cpssExMxPmPolicerVlanCntrGet with same vid and non-NULL cntrValuePtr.
            Expected: GT_OK and same cntrValue.
        */
        st = cpssExMxPmPolicerVlanCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, &cntrValueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCntrGet: %d, %d", dev, vid);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrValue, cntrValueGet,
                       "get another cntrValue than was set: %d", dev);

        /* Call with vid [maxIndex * 4 - 1] */
        vid       = (GT_U16) (maxIndex * 4 - 1);
        cntrValue = 100;

        st = cpssExMxPmPolicerVlanCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, cntrValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, cntrValue);

        /*
            1.2. Call cpssExMxPmPolicerVlanCntrGet with same vid and non-NULL cntrValuePtr.
            Expected: GT_OK and same cntrValue.
        */
        st = cpssExMxPmPolicerVlanCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, &cntrValueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerVlanCntrGet: %d, %d", dev, vid);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrValue, cntrValueGet,
                       "get another cntrValue than was set: %d", dev);

        /*
            1.3. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmPolicerVlanCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, cntrValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, cntrValue);

        vid = POLICER_TESTED_VLAN_ID_CNS;

        /*
            1.4. Call with cntrValue [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        cntrValue = 0xFFFFFFFF;

        st = cpssExMxPmPolicerVlanCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, cntrValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, cntrValue);
    }

    vid       = POLICER_TESTED_VLAN_ID_CNS;
    cntrValue = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerVlanCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerVlanCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                      vid, cntrValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerVlanCntrGet
(
    IN  GT_U8   dev,
    IN  GT_U16  vid,
    OUT GT_U32  *cntrValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerVlanCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_VLAN_E],
                                                      numOfCountingEntries [100] to enable Billing Counting.
    Expected: GT_OK.
    1.2. Call with vid [100 / maxIndex * 4 - 1]
                   and non-NULL cntrValuePtr.
    Expected: GT_OK.
    1.3. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with cntrValuePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16                                  vid        = 0;
    GT_U32                                  cntrValue  = 0;
    GT_U32                                  maxIndex   = 0;
    CPSS_EXMXPM_POLICER_CONFIG_STC          policerConf;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting entry size */
        st = prvUtfExMxPmMaxNumOfPolicerEntriesGet(dev, &maxIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfExMxPmMaxNumOfPolicerEntriesGet: %d", dev);

        /*
            1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_VLAN_E],
                                                              numOfCountingEntries [100] to enable Billing Counting.
            Expected: GT_OK.
        */
        policerConf.ingrCountingEntries = 100;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_VLAN_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerConfigSet: %d, %d, %d",
                                     dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries
);

        /*
            1.2. Call with vid [100 / maxIndex * 4 - 1]
                           and non-NULL cntrValuePtr.
            Expected: GT_OK.
        */

        /* Call with vid [100] */
        vid = POLICER_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmPolicerVlanCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* Call with vid [maxIndex * 4 - 1] */
        vid = (GT_U16)(maxIndex * 4 - 1);

        st = cpssExMxPmPolicerVlanCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /*
            1.3. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmPolicerVlanCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        vid = POLICER_TESTED_VLAN_ID_CNS;

        /*
            1.4. Call with cntrValuePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerVlanCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrValuePtr = NULL", dev);
    }

    vid = POLICER_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerVlanCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          vid, &cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerVlanCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                      vid, &cntrValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPolicyCntrSet
(
    IN  GT_U8   dev,
    IN  GT_U32  index,
    IN  GT_U32  cntrValue
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPolicyCntrSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_POLICY_E],
                                                      numOfCountingEntries [100] to enable Billing Counting.
    Expected: GT_OK.
    1.2. Call with index [0 / 100]
                   and cntrValue [0 / 128].
    Expected: GT_OK.
    1.3. Call cpssExMxPmPolicerPolicyCntrGet with same index and non-NULL cntrValuePtr.
    Expected: GT_OK and same cntrValue.
    1.4. Call with out of range index [maxPolicerCnts]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with cntrValue [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  index        = 0;
    GT_U32                                  maxIndex     = 0;
    GT_U32                                  cntrValue    = 0;
    GT_U32                                  cntrValueGet = 0;
    CPSS_EXMXPM_POLICER_CONFIG_STC          policerConf;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting entry size */
        st = prvUtfExMxPmMaxNumOfPolicerEntriesGet(dev, &maxIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfExMxPmMaxNumOfPolicerEntriesGet: %d", dev);

        /*
            1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_POLICY_E],
                                                              numOfCountingEntries [100] to enable Billing Counting.
            Expected: GT_OK.
        */
        policerConf.ingrCountingEntries = 100;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_POLICY_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerConfigSet: %d, %d, %d",
                                     dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries
);

        /*
            1.2. Call with index [0 / 100]
                           and cntrValue [0 / 128].
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index     = 0;
        cntrValue = 0;

        st = cpssExMxPmPolicerPolicyCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, cntrValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, cntrValue);

        /*
            1.3. Call cpssExMxPmPolicerPolicyCntrGet with same index and non-NULL cntrValuePtr.
            Expected: GT_OK and same cntrValue.
        */
        st = cpssExMxPmPolicerPolicyCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, &cntrValueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPolicyCntrGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrValue, cntrValueGet,
                       "get another cntrValue than was set: %d", dev);

        /* Call with index [100] */
        index     = 100;
        cntrValue = 128;

        st = cpssExMxPmPolicerPolicyCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, cntrValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, cntrValue);

        /*
            1.3. Call cpssExMxPmPolicerPolicyCntrGet with same index and non-NULL cntrValuePtr.
            Expected: GT_OK and same cntrValue.
        */
        st = cpssExMxPmPolicerPolicyCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, &cntrValueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerPolicyCntrGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrValue, cntrValueGet,
                       "get another cntrValue than was set: %d", dev);

        /*
            1.4. Call with out of range index [maxPolicerCnts]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = maxIndex * 8;

        st = cpssExMxPmPolicerPolicyCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, cntrValue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.5. Call with cntrValue [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        cntrValue = 0xFFFFFFFF;

        st = cpssExMxPmPolicerPolicyCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, cntrValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, cntrValue);
    }

    index     = 0;
    cntrValue = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPolicyCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPolicyCntrSet(dev, mainUtCpssExMxPmPolicerDirection,
                                        index, cntrValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPolicyCntrGet
(
    IN  GT_U8   dev,
    IN  GT_U32  index,
    OUT GT_U32  *cntrValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPolicyCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_POLICY_E],
                                                      numOfCountingEntries [100] to enable Billing Counting.
    Expected: GT_OK.
    1.2. Call with index [0 / 100]
                   and non-NULL cntrValuePtr.
    Expected: GT_OK.
    1.3. Call with out of range index [maxPolicerCnts]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with cntrValuePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  index     = 0;
    GT_U32                                  maxIndex  = 0;
    GT_U32                                  cntrValue = 0;
    CPSS_EXMXPM_POLICER_CONFIG_STC          policerConf;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting entry size */
        st = prvUtfExMxPmMaxNumOfPolicerEntriesGet(dev, &maxIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfExMxPmMaxNumOfPolicerEntriesGet: %d", dev);

        /*
            1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_POLICY_E],
                                                              numOfCountingEntries [100] to enable Billing Counting.
            Expected: GT_OK.
        */
        policerConf.ingrCountingEntries = 100;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_POLICY_E;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerConfigSet: %d, %d, %d",
                                     dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries
);

        /*
            1.2. Call with index [0 / 100]
                           and non-NULL cntrValuePtr.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmPolicerPolicyCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [100] */
        index = 100;

        st = cpssExMxPmPolicerPolicyCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.3. Call with out of range index [maxPolicerCnts]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = maxIndex * 8;

        st = cpssExMxPmPolicerPolicyCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, &cntrValue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with cntrValuePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerPolicyCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrValuePtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPolicyCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            index, &cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPolicyCntrGet(dev, mainUtCpssExMxPmPolicerDirection,
                                        index, &cntrValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerQosRemarkEntrySet
(
    IN GT_U8                        dev,
    IN CPSS_EXMXPM_QOS_PARAM_ENT    remarkParamType,
    IN GT_U32                       remarkParamValue,
    IN CPSS_DP_LEVEL_ENT            confLevel,
    IN CPSS_EXMXPM_QOS_PARAM_STC    *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerQosRemarkEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E /
                                    CPSS_EXMXPM_QOS_PARAM_TC_E /
                                    CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                                    CPSS_EXMXPM_QOS_PARAM_EXP_E],
                   remarkParamValue[0 / 3 / 63 / 7],
                   confLevel[CPSS_DP_GREEN_E /
                             CPSS_DP_YELLOW_E /
                             CPSS_DP_RED_E]
                   and qosParamPtr->{tc [0 / 2 / 5 / 7],
                                     dp [CPSS_DP_RED_E  / CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E],
                                     up [0 / 2 / 5 / 7],
                                     dscp [0 / 32 / 55 / 63],
                                     exp[0 / 2 / 5 / 7]}
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerQosRemarkEntryGet with same remarkParamType, remarkParamValue, confLevel and non-NULL qosParamPtr.
    Expected: GT_OK and same qosParamPtr.
    1.3. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                                    CPSS_EXMXPM_QOS_PARAM_DP_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range remarkParamType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E],
                   out of range remarkParamValue[CPSS_USER_PRIORITY_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_TC_E],
                   out of range remarkParamValue[CPSS_TC_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSCP_E],
                   out of range remarkParamValue[CPSS_DSCP_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_EXP_E],
                   out of range remarkParamValue[CPSS_EXP_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with out of range qosParamPtr->tc [CPSS_TC_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with out of range qosParamPtr->dp [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with out of range qosParamPtr->up [CPSS_USER_PRIORITY_RANGE_CNS]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.12. Call with out of range qosParamPtr->dscp [CPSS_DSCP_RANGE_CNS]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with out of range qosParamPtr->exp [CPSS_EXP_RANGE_CNS]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.14. Call with out of range confLevel [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with qosParamPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_QOS_PARAM_ENT    remarkParamType  = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;
    GT_U32                       remarkParamValue = 0;
    CPSS_DP_LEVEL_ENT            confLevel        = CPSS_DP_GREEN_E;
    CPSS_EXMXPM_QOS_PARAM_STC    qosParam;
    CPSS_EXMXPM_QOS_PARAM_STC    qosParamGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E /
                                            CPSS_EXMXPM_QOS_PARAM_TC_E /
                                            CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                                            CPSS_EXMXPM_QOS_PARAM_EXP_E],
                           remarkParamValue[0 / 3 / 63 / 7],
                           confLevel[CPSS_DP_GREEN_E /
                                     CPSS_DP_YELLOW_E /
                                     CPSS_DP_RED_E]
                           and qosParamPtr->{tc [0 / 2 / 5 / 7],
                                             dp [CPSS_DP_RED_E  / CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E],
                                             up [0 / 2 / 5 / 7],
                                             dscp [0 / 32 / 55 / 63],
                                             exp[0 / 2 / 5 / 7]}
            Expected: GT_OK.
        */

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E] */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;
        remarkParamValue = 0;
        confLevel        = CPSS_DP_GREEN_E;

        qosParam.tc   = 0;
        qosParam.dp   = CPSS_DP_RED_E;
        qosParam.up   = 0;
        qosParam.dscp = 0;
        qosParam.exp  = 0;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, remarkParamType, remarkParamValue, confLevel);

        /*
            1.2. Call cpssExMxPmPolicerQosRemarkEntryGet with same remarkParamType, remarkParamValue, confLevel and non-NULL qosParamPtr.
            Expected: GT_OK and same qosParamPtr.
        */
        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParamGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosRemarkEntryGet: %d, %d, %d, %d",
                                     dev, remarkParamType, remarkParamValue, confLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                       "get another qosParamPtr->dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_TC_E] */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_TC_E;
        remarkParamValue = 3;
        confLevel        = CPSS_DP_YELLOW_E;

        qosParam.tc   = 2;
        qosParam.dp   = CPSS_DP_GREEN_E;
        qosParam.up   = 2;
        qosParam.dscp = 32;
        qosParam.exp  = 2;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, remarkParamType, remarkParamValue, confLevel);

        /*
            1.2. Call cpssExMxPmPolicerQosRemarkEntryGet with same remarkParamType, remarkParamValue, confLevel and non-NULL qosParamPtr.
            Expected: GT_OK and same qosParamPtr.
        */
        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParamGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosRemarkEntryGet: %d, %d, %d, %d",
                                     dev, remarkParamType, remarkParamValue, confLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                       "get another qosParamPtr->dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSCP_E] */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_DSCP_E;
        remarkParamValue = 63;
        confLevel        = CPSS_DP_RED_E;

        qosParam.tc   = 5;
        qosParam.dp   = CPSS_DP_YELLOW_E;
        qosParam.up   = 5;
        qosParam.dscp = 55;
        qosParam.exp  = 5;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, remarkParamType, remarkParamValue, confLevel);

        /*
            1.2. Call cpssExMxPmPolicerQosRemarkEntryGet with same remarkParamType, remarkParamValue, confLevel and non-NULL qosParamPtr.
            Expected: GT_OK and same qosParamPtr.
        */
        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParamGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosRemarkEntryGet: %d, %d, %d, %d",
                                     dev, remarkParamType, remarkParamValue, confLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                       "get another qosParamPtr->dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_EXP_E] */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_EXP_E;
        remarkParamValue = 7;
        confLevel        = CPSS_DP_RED_E;

        qosParam.tc   = 7;
        qosParam.dp   = CPSS_DP_YELLOW_E;
        qosParam.up   = 7;
        qosParam.dscp = 63;
        qosParam.exp  = 7;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, remarkParamType, remarkParamValue, confLevel);

        /*
            1.2. Call cpssExMxPmPolicerQosRemarkEntryGet with same remarkParamType, remarkParamValue, confLevel and non-NULL qosParamPtr.
            Expected: GT_OK and same qosParamPtr.
        */
        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParamGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosRemarkEntryGet: %d, %d, %d, %d",
                                     dev, remarkParamType, remarkParamValue, confLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                       "get another qosParamPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                       "get another qosParamPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                       "get another qosParamPtr->up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                       "get another qosParamPtr->dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                       "get another qosParamPtr->exp than was set: %d", dev);

        /*
            1.3. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                                            CPSS_EXMXPM_QOS_PARAM_DP_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E] */
        remarkParamType = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, remarkParamType);

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DP_E] */
        remarkParamType = CPSS_EXMXPM_QOS_PARAM_DP_E;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, remarkParamType);

        /*
            1.4. Call with out of range remarkParamType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        remarkParamType = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, remarkParamType);

        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;

        /*
            1.5. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E],
                           out of range remarkParamValue[CPSS_USER_PRIORITY_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;
        remarkParamValue = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, remarkParamType = %d, remarkParamValue = %d",
                                         dev, remarkParamType, remarkParamValue);

        remarkParamValue = 0;

        /*
            1.6. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_TC_E],
                           out of range remarkParamValue[CPSS_TC_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_TC_E;
        remarkParamValue = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, remarkParamType = %d, remarkParamValue = %d",
                                     dev, remarkParamType, remarkParamValue);

        remarkParamValue = 0;

        /*
            1.7. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSCP_E],
                           out of range remarkParamValue[CPSS_DSCP_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_DSCP_E;
        remarkParamValue = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, remarkParamType = %d, remarkParamValue = %d",
                                         dev, remarkParamType, remarkParamValue);

        remarkParamValue = 0;

        /*
            1.8. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_EXP_E],
                           out of range remarkParamValue[CPSS_EXP_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_EXP_E;
        remarkParamValue = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, remarkParamType = %d, remarkParamValue = %d",
                                         dev, remarkParamType, remarkParamValue);

        remarkParamValue = 0;

        /*
            1.9. Call with out of range qosParamPtr->tc [CPSS_TC_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->tc = %d", dev, qosParam.tc);

        qosParam.tc = 0;

        /*
            1.10. Call with out of range qosParamPtr->dp [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.dp = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->dp = %d", dev, qosParam.dp);

        qosParam.dp = CPSS_DP_GREEN_E;

        /*
            1.11. Call with out of range qosParamPtr->up [CPSS_USER_PRIORITY_RANGE_CNS]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.up = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->up = %d", dev, qosParam.up);

        qosParam.up = 0;

        /*
            1.12. Call with out of range qosParamPtr->dscp [CPSS_DSCP_RANGE_CNS]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->dscp = %d", dev, qosParam.dscp);

        qosParam.dscp = 0;

        /*
            1.13. Call with out of range qosParamPtr->exp [CPSS_EXP_RANGE_CNS]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        qosParam.exp = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosParamPtr->exp = %d", dev, qosParam.exp);

        qosParam.exp = 0;

        /*
            1.14. Call with out of range confLevel [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        confLevel = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, confLevel = %d", dev, confLevel);

        confLevel = CPSS_DP_GREEN_E;

        /*
            1.15. Call with qosParamPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;
    remarkParamValue = 0;
    confLevel        = CPSS_DP_GREEN_E;

    qosParam.tc   = 0;
    qosParam.dp   = CPSS_DP_RED_E;
    qosParam.up   = 0;
    qosParam.dscp = 0;
    qosParam.exp  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                            0, remarkParamType, remarkParamValue,
                                            confLevel, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerQosRemarkEntryGet
(
    IN  GT_U8                       dev,
    IN  CPSS_EXMXPM_QOS_PARAM_ENT   remarkParamType,
    IN  GT_U32                      remarkParamValue,
    IN  CPSS_DP_LEVEL_ENT           confLevel,
    OUT CPSS_EXMXPM_QOS_PARAM_STC   *qosParamPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerQosRemarkEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E /
                                    CPSS_EXMXPM_QOS_PARAM_TC_E /
                                    CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                                    CPSS_EXMXPM_QOS_PARAM_EXP_E],
                   remarkParamValue[0 / 3 / 63 / 7],
                   confLevel[CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E / CPSS_DP_RED_E]
                   and non-NULL qosParamPtr.
    Expected: GT_OK.
    1.2. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                                    CPSS_EXMXPM_QOS_PARAM_DP_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range remarkParamType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E],
                   out of range remarkParamValue[CPSS_USER_PRIORITY_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_TC_E],
                   out of range remarkParamValue[CPSS_TC_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSCP_E],
                   out of range remarkParamValue[CPSS_DSCP_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_EXP_E],
                   out of range remarkParamValue[CPSS_EXP_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with out of range confLevel [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with qosParamPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_QOS_PARAM_ENT    remarkParamType  = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;
    GT_U32                       remarkParamValue = 0;
    CPSS_DP_LEVEL_ENT            confLevel        = CPSS_DP_GREEN_E;
    CPSS_EXMXPM_QOS_PARAM_STC    qosParam;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E /
                                            CPSS_EXMXPM_QOS_PARAM_TC_E /
                                            CPSS_EXMXPM_QOS_PARAM_DSCP_E /
                                            CPSS_EXMXPM_QOS_PARAM_EXP_E],
                           remarkParamValue[0 / 3 / 63 / 7],
                           confLevel[CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E / CPSS_DP_RED_E]
                           and non-NULL qosParamPtr.
            Expected: GT_OK.
        */

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E] */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;
        remarkParamValue = 0;
        confLevel        = CPSS_DP_GREEN_E;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, remarkParamType, remarkParamValue, confLevel);

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_TC_E] */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_TC_E;
        remarkParamValue = 3;
        confLevel        = CPSS_DP_YELLOW_E;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, remarkParamType, remarkParamValue, confLevel);

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSCP_E] */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_DSCP_E;
        remarkParamValue = 63;
        confLevel        = CPSS_DP_RED_E;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, remarkParamType, remarkParamValue, confLevel);

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_EXP_E] */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_EXP_E;
        remarkParamValue = 7;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0,remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, remarkParamType, remarkParamValue, confLevel);

        /*
            1.2. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E /
                                            CPSS_EXMXPM_QOS_PARAM_DP_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E] */
        remarkParamType = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, remarkParamType);

        /* Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DP_E] */
        remarkParamType = CPSS_EXMXPM_QOS_PARAM_DP_E;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, remarkParamType);

        /*
            1.3. Call with out of range remarkParamType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        remarkParamType = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue, confLevel, &qosParam);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, remarkParamType);

        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;

        /*
            1.4. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E],
                           out of range remarkParamValue[CPSS_USER_PRIORITY_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;
        remarkParamValue = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0,remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, remarkParamType = %d, remarkParamValue = %d",
                                         dev, remarkParamType, remarkParamValue);

        remarkParamValue = 0;

        /*
            1.5. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_TC_E],
                           out of range remarkParamValue[CPSS_TC_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_TC_E;
        remarkParamValue = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, remarkParamType = %d, remarkParamValue = %d",
                                     dev, remarkParamType, remarkParamValue);

        remarkParamValue = 0;

        /*
            1.6. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_DSCP_E],
                           out of range remarkParamValue[CPSS_DSCP_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_DSCP_E;
        remarkParamValue = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, remarkParamType = %d, remarkParamValue = %d",
                                         dev, remarkParamType, remarkParamValue);

        remarkParamValue = 0;

        /*
            1.7. Call with remarkParamType [CPSS_EXMXPM_QOS_PARAM_EXP_E],
                           out of range remarkParamValue[CPSS_EXP_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        remarkParamType  = CPSS_EXMXPM_QOS_PARAM_EXP_E;
        remarkParamValue = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0,remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, remarkParamType = %d, remarkParamValue = %d",
                                         dev, remarkParamType, remarkParamValue);

        remarkParamValue = 0;

        /*
            1.8. Call with out of range confLevel [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        confLevel = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, confLevel = %d", dev, confLevel);

        confLevel = CPSS_DP_GREEN_E;

        /*
            1.9. Call with qosParamPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, qosParamPtr = NULL", dev);
    }

    remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;
    remarkParamValue = 0;
    confLevel        = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                0, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                            0, remarkParamType, remarkParamValue,
                                            confLevel, &qosParam);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringEntryRefresh
(
    IN  GT_U8   dev,
    IN  GT_U32  entryIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringEntryRefresh)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 100].
    Expected: GT_OK.
    1.3. Call with out of range index [16384].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      index = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 100].
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmPolicerMeteringEntryRefresh(dev,
                                                   mainUtCpssExMxPmPolicerDirection, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [100] */
        index = 100;

        st = cpssExMxPmPolicerMeteringEntryRefresh(dev, mainUtCpssExMxPmPolicerDirection,
                                                   index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.3. Call with out of range index [16384].
            Expected: NOT GT_OK.
        */
        index = 16384;

        st = cpssExMxPmPolicerMeteringEntryRefresh(dev, mainUtCpssExMxPmPolicerDirection,
                                                   index);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringEntryRefresh(dev, mainUtCpssExMxPmPolicerDirection,
                                                   index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringEntryRefresh(dev, mainUtCpssExMxPmPolicerDirection,
                                               index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPortQosRemarkProfileSet
(
    IN  GT_U8   dev,
    IN  GT_U32  port,
    IN  GT_U32  remarkProfileTableIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPortQosRemarkProfileSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1. Call with remarkProfileTableIndex [0 / 3]
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerPortQosRemarkProfileGet with not NULL remarkProfileTableIndexPtr
    Expected: GT_OK and the same params as was set.
    1.3. Call with remarkProfileTableIndex [4]
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8      port;

    GT_U32  remarkProfileTableIndex = 0;
    GT_U32  remarkProfileTableIndexGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1. Call with remarkProfileTableIndex [0 / 3]
                Expected: GT_OK.
            */
            /* iterate with remarkProfileTableIndex = 0 */
            remarkProfileTableIndex = 0;

            st = cpssExMxPmPolicerPortQosRemarkProfileSet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          port, remarkProfileTableIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, remarkProfileTableIndex);

            /*
                1.2. Call cpssExMxPmPolicerPortQosRemarkProfileGet with not NULL remarkProfileTableIndexPtr
                Expected: GT_OK and the same params as was set.
            */
            st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          port, &remarkProfileTableIndexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerPortQosRemarkProfileGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL1_STRING_MAC(remarkProfileTableIndex, remarkProfileTableIndexGet,
                                         "got another remarkProfileTableIndex than was set: %d", dev);

            /* iterate with remarkProfileTableIndex = 3 */
            remarkProfileTableIndex = 3;

            st = cpssExMxPmPolicerPortQosRemarkProfileSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                          port, remarkProfileTableIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, remarkProfileTableIndex);

            /*
                1.2. Call cpssExMxPmPolicerPortQosRemarkProfileGet with not NULL remarkProfileTableIndexPtr
                Expected: GT_OK and the same params as was set.
            */
            st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          port, &remarkProfileTableIndexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerPortQosRemarkProfileGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL1_STRING_MAC(remarkProfileTableIndex, remarkProfileTableIndexGet,
                                         "got another remarkProfileTableIndex than was set: %d", dev);

            /*
                1.3. Call with remarkProfileTableIndex [4]
                Expected: NOT GT_OK.
            */
            remarkProfileTableIndex = 4;

            st = cpssExMxPmPolicerPortQosRemarkProfileSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                          port, remarkProfileTableIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, remarkProfileTableIndex);
        }

        /* set correct values*/
        remarkProfileTableIndex = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPolicerPortQosRemarkProfileSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                          port, remarkProfileTableIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPolicerPortQosRemarkProfileSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                      port, remarkProfileTableIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPolicerPortQosRemarkProfileSet(dev,
                                                      mainUtCpssExMxPmPolicerDirection,
                                                      port, remarkProfileTableIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    remarkProfileTableIndex = 0;
    port = POLICER_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPortQosRemarkProfileSet(dev,
                                                      mainUtCpssExMxPmPolicerDirection,
                                                      port, remarkProfileTableIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPortQosRemarkProfileSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  port, remarkProfileTableIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerPortQosRemarkProfileGet
(
    IN  GT_U8    dev,
    IN  GT_U32   port,
    OUT GT_U32 * remarkProfileTableIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerPortQosRemarkProfileGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1. Call with not NULL remarkProfileTableIndexPtr
    Expected: GT_OK.
    1.2. Call with remarkProfileTableIndexPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8      port;

    GT_U32      remarkProfileTableIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1. Call with not NULL remarkProfileTableIndexPtr
                Expected: GT_OK.
            */
            st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          port, &remarkProfileTableIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.2. Call with remarkProfileTableIndexPtr [NULL]
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, remarkProfileTableIndexPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          port, &remarkProfileTableIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                      mainUtCpssExMxPmPolicerDirection,
                                                      port, &remarkProfileTableIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                      mainUtCpssExMxPmPolicerDirection,
                                                      port, &remarkProfileTableIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = POLICER_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                      mainUtCpssExMxPmPolicerDirection,
                                                      port, &remarkProfileTableIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerPortQosRemarkProfileGet(dev,
                                                  mainUtCpssExMxPmPolicerDirection,
                                                  port, &remarkProfileTableIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_NOT_SUPPORTED because of Errata.
    1.2. Call cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_NOT_SUPPORTED because of Errata.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet(dev,
                                                               mainUtCpssExMxPmPolicerDirection,
                                                               state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, state);

        /*
            1.2. Call cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet(dev,
                                                               mainUtCpssExMxPmPolicerDirection,
                                                               &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet(dev,
                                                               mainUtCpssExMxPmPolicerDirection,
                                                               state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, state);

        /*
            1.2. Call cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        state = GT_FALSE;

        st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet(dev,
                                                               mainUtCpssExMxPmPolicerDirection,
                                                               &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet(dev,
                                                               mainUtCpssExMxPmPolicerDirection,
                                                               state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet(dev,  mainUtCpssExMxPmPolicerDirection,  state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet
(
    IN  GT_U8    dev,
    OUT GT_BOOL  *enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet(dev,
                                                               mainUtCpssExMxPmPolicerDirection,
                                                               &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet(dev,
                                                               mainUtCpssExMxPmPolicerDirection,
                                                               NULL);
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
        st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet(dev,
                                                               mainUtCpssExMxPmPolicerDirection,
                                                               &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet(dev,  mainUtCpssExMxPmPolicerDirection,   &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringAutoRefreshRangeSet
(
    IN GT_U8    dev,
    IN GT_U32   startAddress,
    IN GT_U32   stopAddress
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringAutoRefreshRangeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with startAddress[0 / 0xfffe], stopAddress[0 / 0xfffe]
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerMeteringAutoRefreshRangeGet with not NULL pointers
    Expected: GT_OK and the same params as was set.
    1.3. Call with startAddress[0xffff]
    Expected: NOT GT_OK.
    1.4. Call with stopAddress[0xffff]
    Expected: NOT GT_OK.
    1.5. Call with startAddress[0xfffe], stopAddress[0xf000] (startAddress > stopAddress not supported)
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32   startAddress = 0;
    GT_U32   stopAddress  = 0;
    GT_U32   startAddressGet = 0;
    GT_U32   stopAddressGet  = 0;
    CPSS_EXMXPM_POLICER_CONFIG_STC policerConf;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* allocate 1 entry for counters */
        policerConf.sharedCounters = GT_TRUE;
        policerConf.ingrCountingEntries = 1;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_BILLING_E;
        /* all other memory is shared between egress and ingress meters */
        policerConf.useInternalBufMemForIngressMeters = GT_FALSE;
        policerConf.ingrMeterEntries = 8 * 1024 - 1;
        policerConf.sharedMeters = GT_TRUE;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerConf.ingrCountingMode, policerConf.ingrCountingEntries);

        /*
            1.1. Call with startAddress[0 / 0xfffe], stopAddress[0 / 0xfffe]
            Expected: GT_OK.
        */
        /* iterate with startAddress = 0 */
        startAddress = 0;
        stopAddress  = 0;

        st = cpssExMxPmPolicerMeteringAutoRefreshRangeSet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          startAddress, stopAddress);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, startAddress, stopAddress);

        /*
            1.2. Call cpssExMxPmPolicerMeteringAutoRefreshRangeGet with not NULL pointers
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshRangeGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          &startAddressGet, &stopAddressGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerMeteringAutoRefreshRangeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(startAddress, startAddressGet, "got another startAddress than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(stopAddress, stopAddressGet, "got anothre stopAddress than was set: %d", dev);

        /* iterate with startAddress = MAX entries - 1 */
        startAddress = 8 * 1024 - 2;
        stopAddress  = 8 * 1024 - 2;

        st = cpssExMxPmPolicerMeteringAutoRefreshRangeSet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          startAddress, stopAddress);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, startAddress, stopAddress);

        /*
            1.2. Call cpssExMxPmPolicerMeteringAutoRefreshRangeGet with not NULL pointers
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshRangeGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          &startAddressGet, &stopAddressGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerMeteringAutoRefreshRangeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(startAddress, startAddressGet, "got another startAddress than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(stopAddress, stopAddressGet, "got anothre stopAddress than was set: %d", dev);

        /*
            1.3. Call with startAddress[0xffff]
            Expected: NOT GT_OK.
        */
        startAddress = 0xffff;

        st = cpssExMxPmPolicerMeteringAutoRefreshRangeSet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          startAddress, stopAddress);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, startAddress);

        startAddress = 0;

        /*
            1.4. Call with stopAddress[0xffff]
            Expected: NOT GT_OK.
        */
        stopAddress = 0xffff;

        st = cpssExMxPmPolicerMeteringAutoRefreshRangeSet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          startAddress, stopAddress);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, stopAddress = %d", dev, stopAddress);

        stopAddress = 0xfffe;

        /*
            1.5. Call with startAddress[0xfffe], stopAddress[0xf000] (startAddress > stopAddress not supported)
            Expected: NOT GT_OK.
        */
        startAddress = 0xfffe;
        stopAddress  = 0xf000;

        st = cpssExMxPmPolicerMeteringAutoRefreshRangeSet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          startAddress, stopAddress);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, startAddress, stopAddress);
    }

    startAddress = 0;
    stopAddress  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringAutoRefreshRangeSet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          startAddress, stopAddress);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringAutoRefreshRangeSet(dev,
                                                      mainUtCpssExMxPmPolicerDirection,
                                                      startAddress, stopAddress);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringAutoRefreshRangeGet
(
    IN GT_U8    dev,
    OUT GT_U32  *startAddressPtr,
    OUT GT_U32  *stopAddressPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringAutoRefreshRangeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. call with not NULL startAddressPtr and stopAddressPtr
    Expected: GT_OK.
    1.2. Call with startAddressPtr [NULL]
    Expected: GT_BAD_PTR.
    1.3. Call with stopAddressPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32   startAddress = 0;
    GT_U32   stopAddress  = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. call with not NULL startAddressPtr and stopAddressPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshRangeGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          &startAddress, &stopAddress);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with startAddressPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshRangeGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          NULL, &stopAddress);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, startAddressPtr = NULL", dev);

        /*
            1.3. Call with stopAddressPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshRangeGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          &startAddress, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, stopAddressPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringAutoRefreshRangeGet(dev,
                                                          mainUtCpssExMxPmPolicerDirection,
                                                          &startAddress, &stopAddress);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringAutoRefreshRangeGet(dev,
                                                      mainUtCpssExMxPmPolicerDirection,
                                                      &startAddress, &stopAddress);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringAutoRefreshIntervalSet
(
    IN GT_U8    dev,
    IN GT_U32   interval
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringAutoRefreshIntervalSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with interval [0 / maxIntervalValue]
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerMeteringAutoRefreshIntervalGet with not NULL intervalPtr
    Expected: GT_OK and the same params as was set.
    1.3. call with interval [maxIntervalValue+1]
    Expected: NOT GT_OK
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32   interval    = 0;
    GT_U32   intervalGet = 0;
    GT_U32   maxIntervalValue;   /* maximum interval value in microS */


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /* Calculate maximum interval value in microS */
        maxIntervalValue = 0xFFFFFFFF / PRV_CPSS_PP_MAC(dev)->coreClock;

        /*
            1.1. Call with interval [0 / maxIntervalValue]
            Expected: GT_OK.
        */
        /* iterate with interval = 0 */
        interval = 0;

        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalSet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             interval);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, interval);

        /*
            1.2. Call cpssExMxPmPolicerMeteringAutoRefreshIntervalGet with not NULL intervalPtr
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             &intervalGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerMeteringAutoRefreshIntervalGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interval, intervalGet, "got another interval than was set: %d", dev);

        /* iterate with interval = maxIntervalValue */
        interval = maxIntervalValue;

        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalSet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             interval);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, interval);

        /*
            1.2. Call cpssExMxPmPolicerMeteringAutoRefreshIntervalGet with not NULL intervalPtr
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             &intervalGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerMeteringAutoRefreshIntervalGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(interval, intervalGet, "got another interval than was set: %d", dev);


        /*
            1.3. call with interval [maxIntervalValue+1]
            Expected: NOT GT_OK
        */
        interval = maxIntervalValue + 1;

        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalSet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             interval);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, interval);
    }

    interval = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalSet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             interval);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringAutoRefreshIntervalSet(dev,
                                                         mainUtCpssExMxPmPolicerDirection,
                                                         interval);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeteringAutoRefreshIntervalGet
(
    IN GT_U8    dev,
    OUT GT_U32  *intervalPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeteringAutoRefreshIntervalGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL intervalPtr
    Expected: GT_OK.
    1.2. Call with intervalPtr[NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      interval = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL intervalPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             &interval);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with intervalPtr[NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, intervalPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeteringAutoRefreshIntervalGet(dev,
                                                             mainUtCpssExMxPmPolicerDirection,
                                                             &interval);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeteringAutoRefreshIntervalGet(dev,
                                                         mainUtCpssExMxPmPolicerDirection,
                                                         &interval);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerQosUpdateEnableSet
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerQosUpdateEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerQosUpdateEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmPolicerQosUpdateEnableSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPolicerQosUpdateEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerQosUpdateEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerQosUpdateEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmPolicerQosUpdateEnableSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmPolicerQosUpdateEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmPolicerQosUpdateEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerQosUpdateEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerQosUpdateEnableSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerQosUpdateEnableSet(dev, mainUtCpssExMxPmPolicerDirection,
                                             state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerQosUpdateEnableGet
(
    IN  GT_U8    dev,
    OUT GT_BOOL  *enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerQosUpdateEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerQosUpdateEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerQosUpdateEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 NULL);
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
        st = cpssExMxPmPolicerQosUpdateEnableGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerQosUpdateEnableGet(dev, mainUtCpssExMxPmPolicerDirection, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeterResolutionSet
(
    IN GT_U8                                        devNum,
    IN CPSS_EXMXPM_POLICER_METER_RESOLUTION_ENT     resolution
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeterResolutionSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with resolution [CPSS_EXMXPM_POLICER_METER_RESOLUTION_BYTES_E / CPSS_EXMXPM_POLICER_METER_RESOLUTION_PACKETS_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerMeterResolutionGet with non-NULL resolutionPtr.
    Expected: GT_OK and the same resolutionPtr.
    1.3. Call with resolution [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_METER_RESOLUTION_ENT     resolution    = CPSS_EXMXPM_POLICER_METER_RESOLUTION_BYTES_E;
    CPSS_EXMXPM_POLICER_METER_RESOLUTION_ENT     resolutionGet = CPSS_EXMXPM_POLICER_METER_RESOLUTION_BYTES_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with resolution [CPSS_EXMXPM_POLICER_METER_RESOLUTION_BYTES_E / CPSS_EXMXPM_POLICER_METER_RESOLUTION_PACKETS_E].
            Expected: GT_OK.
        */
        resolution = CPSS_EXMXPM_POLICER_METER_RESOLUTION_BYTES_E;

        st = cpssExMxPmPolicerMeterResolutionSet(dev,
                                                 mainUtCpssExMxPmPolicerDirection,
                                                 resolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, resolution);

        /*
            1.2. Call cpssExMxPmPolicerMeterResolutionGet with non-NULL resolutionPtr.
            Expected: GT_OK and the same resolutionPtr.
        */
        st = cpssExMxPmPolicerMeterResolutionGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 &resolutionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosUpdateEnableGet: %d", dev);

        resolution = CPSS_EXMXPM_POLICER_METER_RESOLUTION_PACKETS_E;

        st = cpssExMxPmPolicerMeterResolutionSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 resolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, resolution);

        /*
            1.2. Call cpssExMxPmPolicerQosUpdateEnableGet with non-NULL resolutionPtr.
            Expected: GT_OK and the same resolutionPtr.
        */
        st = cpssExMxPmPolicerMeterResolutionGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 &resolutionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosUpdateEnableGet: %d", dev);

        /*
            1.3. Call with resolution [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        resolution = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerMeterResolutionSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 resolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, resolution);
    }

    resolution = CPSS_EXMXPM_POLICER_METER_RESOLUTION_BYTES_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeterResolutionSet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 resolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeterResolutionSet(dev, mainUtCpssExMxPmPolicerDirection,
                                             resolution);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerMeterResolutionGet
(
    IN GT_U8                                        devNum,
    OUT CPSS_EXMXPM_POLICER_METER_RESOLUTION_ENT    *resolutionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerMeterResolutionGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL resolutionPtr.
    Expected: GT_OK.
    1.2. Call with resolutionPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_METER_RESOLUTION_ENT     resolution = CPSS_EXMXPM_POLICER_METER_RESOLUTION_BYTES_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL resolutionPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPolicerMeterResolutionGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 &resolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with resolutionPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerMeterResolutionGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, resolutionPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerMeterResolutionGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                 &resolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerMeterResolutionGet(dev, mainUtCpssExMxPmPolicerDirection,
                                             &resolution);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerCountingModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT    mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerCountingModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [ CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E /
                          CPSS_EXMXPM_POLICER_COUNTING_BILLING_E /
                          CPSS_EXMXPM_POLICER_COUNTING_POLICY_E /
                          CPSS_EXMXPM_POLICER_COUNTING_VLAN_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPolicerCountingModeGet with not NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.3. Call with mode [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT    mode = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;
    CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT    modeGet = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [ CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E /
                                  CPSS_EXMXPM_POLICER_COUNTING_BILLING_E /
                                  CPSS_EXMXPM_POLICER_COUNTING_POLICY_E /
                                  CPSS_EXMXPM_POLICER_COUNTING_VLAN_E].
            Expected: GT_OK.
        */
        /* iterate with mode = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E */
        mode = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;

        st = cpssExMxPmPolicerCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                              mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPolicerCountingModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmPolicerCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              &modeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerCountingModeGet: %d, %d", dev, mode);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode than was set: %d", dev);

        /* iterate with mode = CPSS_EXMXPM_POLICER_COUNTING_BILLING_E */
        mode = CPSS_EXMXPM_POLICER_COUNTING_BILLING_E;

        st = cpssExMxPmPolicerCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPolicerCountingModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmPolicerCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection, &modeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerCountingModeGet: %d, %d", dev, mode);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode than was set: %d", dev);

        /* iterate with mode = CPSS_EXMXPM_POLICER_COUNTING_POLICY_E */
        mode = CPSS_EXMXPM_POLICER_COUNTING_POLICY_E;

        st = cpssExMxPmPolicerCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPolicerCountingModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmPolicerCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection, &modeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerCountingModeGet: %d, %d", dev, mode);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode than was set: %d", dev);

        /* iterate with mode = CPSS_EXMXPM_POLICER_COUNTING_VLAN_E */
        mode = CPSS_EXMXPM_POLICER_COUNTING_VLAN_E;

        st = cpssExMxPmPolicerCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPolicerCountingModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmPolicerCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection, &modeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerCountingModeGet: %d, %d", dev, mode);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet, "got another mode than was set: %d", dev);

        /*
            1.3. Call with mode [0x5AAAAAA5] (out of range).
            Expected: GT_BAD_PARAM.
        */
        mode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPmPolicerCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection,
                                              mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerCountingModeSet(dev, mainUtCpssExMxPmPolicerDirection, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerCountingModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerCountingModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT  mode = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;


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
        st = cpssExMxPmPolicerCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPolicerCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerCountingModeGet(dev, mainUtCpssExMxPmPolicerDirection,
                                          &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPolicerCountingWriteBackCacheFlush
(
    IN GT_U8                                    devNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerCountingWriteBackCacheFlush)
{
/*
    ITERATE_DEVICES (ExMxPm)
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;




    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssExMxPmPolicerCountingWriteBackCacheFlush(dev,
                                                          mainUtCpssExMxPmPolicerDirection);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPolicerCountingWriteBackCacheFlush(dev,
                                                          mainUtCpssExMxPmPolicerDirection);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPolicerCountingWriteBackCacheFlush(dev,
                                                      mainUtCpssExMxPmPolicerDirection);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Metering table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerFillMeteringTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Metering table.
         Call cpssExMxPmPolicerMeteringEntrySet with entryIndex [0..numEntries - 1],
                                                   entryPtr->{countingEntryIndex[0],
                                                              dropRed[GT_TRUE],
                                                              modifyQosParams{modifyTc [GT_TRUE],
                                                                              modifyUp [GT_TRUE],
                                                                              modifyDp [GT_TRUE],
                                                                              modifyDscp [GT_TRUE],
                                                                              modifyExp [GT_TRUE]},
                                                              remarkMode [CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E],
                                                              meterColorMode [CPSS_POLICER_COLOR_BLIND_E],
                                                              meterMode [CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E],
                                                              mngCounterSet [CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E],
                                                              tbParamsPtr.srTcmParams{cir [100], cbs [256], ebs [1000]}}
                                                   and non-NULL tbParamsPtr.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmPolicerMeteringEntrySet with entryIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Metering table and compare with original.
         Call cpssExMxPmPolicerMeteringEntryGet with same entryIndex and non-NULL entryPtr.
    Expected: GT_OK and the same values as written - check by fields.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmPolicerMeteringEntryGet with entryIndex [numEntries] and non-NULL entryPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;
    CPSS_EXMXPM_POLICER_CONFIG_STC policerConf;
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  entry;
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC  entryGet;
    CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT tbParams;


    cpssOsBzero((GT_VOID*) &entry, sizeof(entry));
    cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));
    cpssOsBzero((GT_VOID*) &tbParams, sizeof(tbParams));
    cpssOsBzero((GT_VOID*) &policerConf, sizeof(policerConf));

    /* Fill the entry for Metering table */
    entry.countingEntryIndex = 0;
    entry.dropRed            = GT_TRUE;

    entry.modifyQosParams.modifyTc   = GT_TRUE;
    entry.modifyQosParams.modifyUp   = GT_TRUE;
    entry.modifyQosParams.modifyDp   = GT_TRUE;
    entry.modifyQosParams.modifyDscp = GT_TRUE;
    entry.modifyQosParams.modifyExp  = GT_TRUE;

    entry.remarkMode     = CPSS_EXMXPM_POLICER_REMARK_MODE_L3_E;
    entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
    entry.meterMode      = CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E;
    entry.mngCounterSet  = CPSS_EXMXPM_POLICER_MNG_CNTR_SET0_E;

    entry.tokenBucketsParams.srTcmParams.cir = 100;
    entry.tokenBucketsParams.srTcmParams.cbs = 256;
    entry.tokenBucketsParams.srTcmParams.ebs = 1000;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        st = cpssExMxPmPolicerConfigGet(dev, &policerConf);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPolicerConfigGet: %d", dev);

        numEntries = policerConf.ingrMeterEntries;

        /* 1.2. Fill all entries in Metering table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            entry.tokenBucketsParams.srTcmParams.cir = iTemp % 100;
            entry.tokenBucketsParams.srTcmParams.cbs = iTemp % 100;
            entry.tokenBucketsParams.srTcmParams.ebs = iTemp % 100;

            st = cpssExMxPmPolicerMeteringEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                   iTemp, &entry, &tbParams);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerMeteringEntrySet: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmPolicerMeteringEntrySet(dev,
                                               mainUtCpssExMxPmPolicerDirection,
                                               numEntries, &entry, &tbParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerMeteringEntrySet: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in Metering table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            entry.tokenBucketsParams.srTcmParams.cir = iTemp % 100;
            entry.tokenBucketsParams.srTcmParams.cbs = iTemp % 100;
            entry.tokenBucketsParams.srTcmParams.ebs = iTemp % 100;

            st = cpssExMxPmPolicerMeteringEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                   iTemp, &entryGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerMeteringEntryGet: %d, %d", dev, iTemp);

            /* Verifying entryPtr */
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.countingEntryIndex, entryGet.countingEntryIndex,
                           "get another entryPtr->countingEntryIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.dropRed, entryGet.dropRed,
                           "get another entryPtr->dropRed than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyTc, entryGet.modifyQosParams.modifyTc,
                           "get another entryPtr->modifyQosParams.modifyTc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyUp, entryGet.modifyQosParams.modifyUp,
                           "get another entryPtr->modifyQosParams.modifyUp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyDp, entryGet.modifyQosParams.modifyDp,
                           "get another entryPtr->modifyQosParams.modifyDp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyDscp, entryGet.modifyQosParams.modifyDscp,
                           "get another entryPtr->modifyQosParams.modifyDscp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.modifyQosParams.modifyExp, entryGet.modifyQosParams.modifyExp,
                           "get another entryPtr->modifyQosParams.modifyExp than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(entry.remarkMode, entryGet.remarkMode,
                           "get another entryPtr->remarkMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.meterColorMode, entryGet.meterColorMode,
                           "get another entryPtr->meterColorMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.meterMode, entryGet.meterMode,
                           "get another entryPtr->meterMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.mngCounterSet, entryGet.mngCounterSet,
                           "get another entryPtr->mngCounterSet than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(entry.tokenBucketsParams.srTcmParams.cir, entryGet.tokenBucketsParams.srTcmParams.cir,
                           "get another entryPtr->tokenBucketsParams.srTcmParams.cir than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.tokenBucketsParams.srTcmParams.cbs, entryGet.tokenBucketsParams.srTcmParams.cbs,
                           "get another entryPtr->tokenBucketsParams.srTcmParams.cbs than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(entry.tokenBucketsParams.srTcmParams.ebs, entryGet.tokenBucketsParams.srTcmParams.ebs,
                           "get another entryPtr->tokenBucketsParams.srTcmParams.ebs than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmPolicerMeteringEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                               numEntries, &entryGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerMeteringEntryGet: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Billing table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerFillBillingTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_BILLING_E],
                                                      numOfCountingEntries [PRV_CPSS_EXMXPM_POLICER_MAX_INDEX_CNS]
                                                      to enable Billing Counting.
    Expected: GT_OK.
    1.2. Get table Size.
    Expected: GT_OK.
    1.3. Fill all entries in Billing table.
         Call cpssExMxPmPolicerBillingEntrySet with entryIndex [0..numEntries - 1],
                                                   billingCntrPtr {greenCntr.l[0x0, 0x0 ],
                                                                   yellowCntr.l[0x0, 0x0 ],
                                                                   redCntr.l[0x0, 0x0 ],
                                                                   cntrMode [CPSS_EXMXPM_POLICER_BILLING_CNTR_16_BYTES_E]}.
    Expected: GT_OK.
    1.4. Try to write entry with index out of range.
         Call cpssExMxPmPolicerBillingEntrySet with entryIndex [numEntries],
    Expected: NOT GT_OK.
    1.5. Read all entries in Billing table and compare with original.
         Call cpssExMxPmPolicerBillingEntryGet with same entryIndex, reset [GT_FALSE] and non-NULL billingCntrPtr.
    Expected: GT_OK and same values as written in 1.2.
    1.6. Try to read entry with index out of range.
         Call cpssExMxPmPolicerBillingEntryGet with entryIndex [numEntries], reset [GT_FALSE] and non-NULL billingCntrPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;
    GT_BOOL     reset      = GT_FALSE;
    CPSS_EXMXPM_POLICER_CONFIG_STC          policerConf;

    CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC   billingCntr;
    CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC   billingCntrGet;


    cpssOsBzero((GT_VOID*) &billingCntr, sizeof(billingCntr));
    cpssOsBzero((GT_VOID*) &billingCntrGet, sizeof(billingCntrGet));
    cpssOsBzero((GT_VOID*) &policerConf, sizeof(policerConf));

    policerConf.ingrCountingEntries = 0;
    policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_DISABLE_E;

    /* configure Counters to operate in Billing mode,
      and use 1/2 of the policer memory memory */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerConfigSet: %d", dev);


    }

    /* Fill the entry for Billing table */
    reset = GT_FALSE;
    billingCntr.greenCntr.l[0] = 0x0;
    billingCntr.greenCntr.l[1] = 0x0;

    billingCntr.yellowCntr.l[0] = 0x0;
    billingCntr.yellowCntr.l[1] = 0x0;

    billingCntr.redCntr.l[0] = 0x0;
    billingCntr.redCntr.l[1] = 0x0;

    billingCntr.cntrMode = CPSS_EXMXPM_POLICER_BILLING_CNTR_16_BYTES_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmPolicerConfigSet with mode [CPSS_EXMXPM_POLICER_COUNTING_BILLING_E],
                                                              numOfCountingEntries [PRV_CPSS_EXMXPM_POLICER_MAX_INDEX_CNS]
                                                              to enable Billing Counting.
            Expected: GT_OK.
        */
        policerConf.ingrCountingEntries = PRV_CPSS_EXMXPM_POLICER_MAX_INDEX_CNS;
        policerConf.ingrCountingMode = CPSS_EXMXPM_POLICER_COUNTING_BILLING_E;
        policerConf.ingrMeterEntries = 0;
        policerConf.sharedMeters = GT_TRUE;
        policerConf.sharedCounters = GT_TRUE;

        st = cpssExMxPmPolicerConfigSet(dev, &policerConf);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerConfigSet: %d, %d, %d",
                                     dev, policerConf.ingrCountingMode,
                                     policerConf.ingrCountingEntries);

        /* 1.2. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_POLICER_MAX_INDEX_CNS;

        /* 1.3. Fill all entries in Billing table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            billingCntr.greenCntr.l[0]  = iTemp;
            billingCntr.yellowCntr.l[0] = iTemp;
            billingCntr.redCntr.l[1]    = (iTemp % 0x3FF);

            st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  iTemp, &billingCntr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerBillingEntrySet: %d, %d", dev, iTemp);
        }

        /* 1.4. Try to write entry with index out of range. */
        st = cpssExMxPmPolicerBillingEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                              numEntries, &billingCntr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerBillingEntrySet: %d, %d", dev, numEntries);

        /* 1.5. Read all entries in Billing table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            billingCntr.greenCntr.l[0]  = iTemp;
            billingCntr.yellowCntr.l[0] = iTemp;
            billingCntr.redCntr.l[1]    = (iTemp % 0x3FF);

            st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                  iTemp, reset, &billingCntrGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerBillingEntryGet: %d, %d", dev, iTemp);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.greenCntr.l,
                                         (GT_VOID*) &billingCntrGet.greenCntr.l,
                                         sizeof(billingCntr.greenCntr.l))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another billingCntrPtr->greenCntr.l than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.yellowCntr.l,
                                         (GT_VOID*) &billingCntrGet.yellowCntr.l,
                                         sizeof(billingCntr.yellowCntr.l))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another billingCntrPtr->yellowCntr.l than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &billingCntr.redCntr.l,
                                         (GT_VOID*) &billingCntrGet.redCntr.l,
                                         sizeof(billingCntr.redCntr.l))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another billingCntrPtr->redCntr.l than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(billingCntr.cntrMode, billingCntrGet.cntrMode,
                           "get another billingCntrPtr->cntrMode than was set: %d", dev);
        }

        /* 1.6. Try to read entry with index out of range. */
        st = cpssExMxPmPolicerBillingEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                              numEntries, reset, &billingCntrGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerBillingEntryGet: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Qos Remark table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmPolicerFillQosRemarkTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in QosRemark table.
         Call cpssExMxPmPolicerQosRemarkEntrySet with remarkProfileTableIndex [0..numEntries - 1]
                                                   remarkParamType [CPSS_EXMXPM_QOS_PARAM_UP_E],
                                                   remarkParamValue[0],
                                                   confLevel[CPSS_DP_GREEN_E]
                                                   and qosParamPtr->{tc [0],
                                                                     dp [CPSS_DP_RED_E],
                                                                     up [0],
                                                                     dscp [0],
                                                                     exp[0]}
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmPolicerQosRemarkEntrySet with remarkProfileTableIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in QosRemark table and compare with original.
         Call cpssExMxPmPolicerQosRemarkEntryGet with same remarkParamType, remarkParamValue, confLevel and non-NULL qosParamPtr.
    Expected: GT_OK and same qosParamPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmPolicerQosRemarkEntryGet with remarkProfileTableIndex [numEntries] and non-NULL qosParamPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_QOS_PARAM_ENT    remarkParamType  = CPSS_EXMXPM_QOS_PARAM_DSA_QOS_PROFILE_E;
    GT_U32                       remarkParamValue = 0;
    CPSS_DP_LEVEL_ENT            confLevel        = CPSS_DP_GREEN_E;
    CPSS_EXMXPM_QOS_PARAM_STC    qosParam;
    CPSS_EXMXPM_QOS_PARAM_STC    qosParamGet;


    cpssOsBzero((GT_VOID*) &qosParam, sizeof(qosParam));
    cpssOsBzero((GT_VOID*) &qosParamGet, sizeof(qosParamGet));

    /* Fill the entry for QosRemark table */
    remarkParamType  = CPSS_EXMXPM_QOS_PARAM_UP_E;
    remarkParamValue = 0;
    confLevel        = CPSS_DP_GREEN_E;

    qosParam.tc   = 0;
    qosParam.dp   = CPSS_DP_RED_E;
    qosParam.up   = 0;
    qosParam.dscp = 0;
    qosParam.exp  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 4;/*PRV_CPSS_EXMXPM_DEV_TBLS_MAC(dev)->IPLR.ingrPolicerReMarkingMemory.numOfEntries;*/

        /* 1.2. Fill all entries in QosRemark table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            qosParam.tc   = (iTemp % 7);
            qosParam.up   = (iTemp % 7);
            qosParam.dscp = (iTemp % 63);
            qosParam.exp  = (iTemp % 7);

            st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                    iTemp, remarkParamType, remarkParamValue,
                                                    confLevel, &qosParam);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosRemarkEntrySet: %d, %d, %d", dev, iTemp, remarkParamType);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmPolicerQosRemarkEntrySet(dev, mainUtCpssExMxPmPolicerDirection,
                                                numEntries, remarkParamType, remarkParamValue,
                                                confLevel, &qosParam);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosRemarkEntrySet: %d, %d, %d", dev, iTemp, remarkParamType);

        /* 1.4. Read all entries in QosRemark table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            qosParam.tc   = (iTemp % 7);
            qosParam.up   = (iTemp % 7);
            qosParam.dscp = (iTemp % 63);
            qosParam.exp  = (iTemp % 7);

            st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                    iTemp, remarkParamType, remarkParamValue, confLevel, &qosParamGet);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosRemarkEntryGet: %d, %d, %d, %d",
                                         dev, iTemp, remarkParamType, remarkParamValue);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.tc, qosParamGet.tc,
                           "get another qosParamPtr->tc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dp, qosParamGet.dp,
                           "get another qosParamPtr->dp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.up, qosParamGet.up,
                           "get another qosParamPtr->up than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.dscp, qosParamGet.dscp,
                           "get another qosParamPtr->dscp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(qosParam.exp, qosParamGet.exp,
                           "get another qosParamPtr->exp than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmPolicerQosRemarkEntryGet(dev, mainUtCpssExMxPmPolicerDirection,
                                                numEntries, remarkParamType, remarkParamValue,
                                                confLevel, &qosParamGet);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmPolicerQosRemarkEntryGet: %d, %d, %d, %d",
                                     dev, iTemp, remarkParamType, remarkParamValue);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmPolicer suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmPolicer)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPacketSizeModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPacketSizeModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerVlanCountingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerVlanCountingModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerDropTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerDropTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerCountingColorModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerCountingColorModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerL2RemarkModelSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerL2RemarkModelGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerManagementCntrsResolutionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerManagementCntrsResolutionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPacketSizeModeForTunnelTermSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPacketSizeModeForTunnelTermGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPortMeteringEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPortMeteringEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMruSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMruGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerErrorGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerErrorCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerManagementCountersSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerManagementCountersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerEntryMeterParamsCalculate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerBillingEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerBillingEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerVlanCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerVlanCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPolicyCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPolicyCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerQosRemarkEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerQosRemarkEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringEntryRefresh)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPortQosRemarkProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerPortQosRemarkProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringAutoRefreshRangeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringAutoRefreshRangeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringAutoRefreshIntervalSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeteringAutoRefreshIntervalGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerQosUpdateEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerQosUpdateEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeterResolutionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerMeterResolutionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerCountingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerCountingModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerCountingWriteBackCacheFlush)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerFillMeteringTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerFillBillingTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPolicerFillQosRemarkTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmPolicer)


/*******************************************************************************
* prvUtfExMxPmMaxNumOfPolicerEntriesGet
*
* DESCRIPTION:
*       This routine returns number of policer entries.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       maxNumOfPolicerEntriesPtr - (pointer to) max entry number
*
*       GT_OK           - Get table size was OK
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfExMxPmMaxNumOfPolicerEntriesGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *maxNumOfPolicerEntriesPtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(maxNumOfPolicerEntriesPtr);

    /* check if dev active and from ExMxPm family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    *maxNumOfPolicerEntriesPtr = PRV_CPSS_EXMXPM_PP_MAC(dev)->policerInfo.ingrNumOfCountingEntries;

    return GT_OK;
}

